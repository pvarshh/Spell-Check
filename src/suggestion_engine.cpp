#include "suggestion_engine.h"
#include "dictionary.h"
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <cmath>

namespace spellcheck
{

    SuggestionEngine::SuggestionEngine(const Dictionary *dictionary)
        : dictionary_(dictionary), max_edit_distance_(2), max_suggestions_(10), edit_distance_weight_(1.0), frequency_weight_(0.5), phonetic_weight_(0.3), prefix_weight_(0.2)
    {
    }

    std::vector<std::string> SuggestionEngine::generateSuggestions(const std::string &word) const
    {
        if (!dictionary_ || word.empty())
        {
            return {};
        }

        std::unordered_set<std::string> candidate_set;

        // Generate candidates using various methods
        auto deletions = generateDeletionCandidates(word);
        auto insertions = generateInsertionCandidates(word);
        auto substitutions = generateSubstitutionCandidates(word);
        auto transpositions = generateTranspositionCandidates(word);
        auto splits = generateSplitCandidates(word);
        auto phonetic = generatePhoneticSuggestions(word);
        auto prefix = generatePrefixSuggestions(word);

        // Combine all candidates
        for (const auto &candidate : deletions)
        {
            if (dictionary_->containsWord(candidate))
            {
                candidate_set.insert(candidate);
            }
        }

        for (const auto &candidate : insertions)
        {
            if (dictionary_->containsWord(candidate))
            {
                candidate_set.insert(candidate);
            }
        }

        for (const auto &candidate : substitutions)
        {
            if (dictionary_->containsWord(candidate))
            {
                candidate_set.insert(candidate);
            }
        }

        for (const auto &candidate : transpositions)
        {
            if (dictionary_->containsWord(candidate))
            {
                candidate_set.insert(candidate);
            }
        }

        for (const auto &candidate : splits)
        {
            if (dictionary_->containsWord(candidate))
            {
                candidate_set.insert(candidate);
            }
        }

        for (const auto &candidate : phonetic)
        {
            candidate_set.insert(candidate);
        }

        for (const auto &candidate : prefix)
        {
            candidate_set.insert(candidate);
        }

        // Convert set to vector for ranking
        std::vector<std::string> candidates(candidate_set.begin(), candidate_set.end());

        // Rank and return suggestions
        return rankCandidates(word, candidates);
    }

    std::vector<std::string> SuggestionEngine::generateEditDistanceSuggestions(const std::string &word,
                                                                               size_t max_distance) const
    {
        if (!dictionary_)
        {
            return {};
        }

        std::vector<std::string> suggestions;
        auto all_words = dictionary_->getAllWords();

        for (const auto &dict_word : all_words)
        {
            size_t distance = calculateEditDistance(word, dict_word);
            if (distance <= max_distance)
            {
                suggestions.push_back(dict_word);
            }
        }

        // Sort by edit distance and frequency
        std::sort(suggestions.begin(), suggestions.end(),
                  [this, &word](const std::string &a, const std::string &b)
                  {
                      size_t dist_a = calculateEditDistance(word, a);
                      size_t dist_b = calculateEditDistance(word, b);

                      if (dist_a != dist_b)
                      {
                          return dist_a < dist_b;
                      }

                      return dictionary_->getWordFrequency(a) > dictionary_->getWordFrequency(b);
                  });

        if (suggestions.size() > max_suggestions_)
        {
            suggestions.resize(max_suggestions_);
        }

        return suggestions;
    }

    std::vector<std::string> SuggestionEngine::generatePhoneticSuggestions(const std::string &word) const
    {
        if (!dictionary_)
        {
            return {};
        }

        return dictionary_->getPhoneticMatches(word);
    }

    std::vector<std::string> SuggestionEngine::generatePrefixSuggestions(const std::string &word) const
    {
        if (!dictionary_)
        {
            return {};
        }

        std::vector<std::string> suggestions;

        // Try prefixes of different lengths
        for (size_t len = std::min(word.length(), static_cast<size_t>(3)); len <= word.length(); ++len)
        {
            std::string prefix = word.substr(0, len);
            auto prefix_matches = dictionary_->getWordsWithPrefix(prefix, 20);

            suggestions.insert(suggestions.end(), prefix_matches.begin(), prefix_matches.end());
        }

        // Remove duplicates
        std::sort(suggestions.begin(), suggestions.end());
        suggestions.erase(std::unique(suggestions.begin(), suggestions.end()), suggestions.end());

        return suggestions;
    }

    size_t SuggestionEngine::calculateEditDistance(const std::string &word1, const std::string &word2) const
    {
        const size_t len1 = word1.length();
        const size_t len2 = word2.length();

        std::vector<std::vector<size_t>> dp(len1 + 1, std::vector<size_t>(len2 + 1));

        // Initialize base cases
        for (size_t i = 0; i <= len1; ++i)
        {
            dp[i][0] = i;
        }
        for (size_t j = 0; j <= len2; ++j)
        {
            dp[0][j] = j;
        }

        // Fill the DP table
        for (size_t i = 1; i <= len1; ++i)
        {
            for (size_t j = 1; j <= len2; ++j)
            {
                if (word1[i - 1] == word2[j - 1])
                {
                    dp[i][j] = dp[i - 1][j - 1];
                }
                else
                {
                    dp[i][j] = 1 + std::min({
                                       dp[i - 1][j],    // deletion
                                       dp[i][j - 1],    // insertion
                                       dp[i - 1][j - 1] // substitution
                                   });
                }
            }
        }

        return dp[len1][len2];
    }

    size_t SuggestionEngine::calculateDamerauLevenshteinDistance(const std::string &word1,
                                                                 const std::string &word2) const
    {
        const size_t len1 = word1.length();
        const size_t len2 = word2.length();

        std::vector<std::vector<size_t>> dp(len1 + 1, std::vector<size_t>(len2 + 1));

        // Initialize base cases
        for (size_t i = 0; i <= len1; ++i)
        {
            dp[i][0] = i;
        }
        for (size_t j = 0; j <= len2; ++j)
        {
            dp[0][j] = j;
        }

        // Fill the DP table
        for (size_t i = 1; i <= len1; ++i)
        {
            for (size_t j = 1; j <= len2; ++j)
            {
                size_t cost = (word1[i - 1] == word2[j - 1]) ? 0 : 1;

                dp[i][j] = std::min({
                    dp[i - 1][j] + 1,       // deletion
                    dp[i][j - 1] + 1,       // insertion
                    dp[i - 1][j - 1] + cost // substitution
                });

                // Transposition
                if (i > 1 && j > 1 &&
                    word1[i - 1] == word2[j - 2] &&
                    word1[i - 2] == word2[j - 1])
                {
                    dp[i][j] = std::min(dp[i][j], dp[i - 2][j - 2] + cost);
                }
            }
        }

        return dp[len1][len2];
    }

    std::vector<std::string> SuggestionEngine::generateDeletionCandidates(const std::string &word) const
    {
        std::vector<std::string> candidates;

        for (size_t i = 0; i < word.length(); ++i)
        {
            std::string candidate = word.substr(0, i) + word.substr(i + 1);
            candidates.push_back(candidate);
        }

        return candidates;
    }

    std::vector<std::string> SuggestionEngine::generateInsertionCandidates(const std::string &word) const
    {
        std::vector<std::string> candidates;
        const std::string alphabet = "abcdefghijklmnopqrstuvwxyz";

        for (size_t i = 0; i <= word.length(); ++i)
        {
            for (char c : alphabet)
            {
                std::string candidate = word.substr(0, i) + c + word.substr(i);
                candidates.push_back(candidate);
            }
        }

        return candidates;
    }

    std::vector<std::string> SuggestionEngine::generateSubstitutionCandidates(const std::string &word) const
    {
        std::vector<std::string> candidates;
        const std::string alphabet = "abcdefghijklmnopqrstuvwxyz";

        for (size_t i = 0; i < word.length(); ++i)
        {
            for (char c : alphabet)
            {
                if (c != word[i])
                {
                    std::string candidate = word;
                    candidate[i] = c;
                    candidates.push_back(candidate);
                }
            }
        }

        return candidates;
    }

    std::vector<std::string> SuggestionEngine::generateTranspositionCandidates(const std::string &word) const
    {
        std::vector<std::string> candidates;

        for (size_t i = 0; i < word.length() - 1; ++i)
        {
            std::string candidate = word;
            std::swap(candidate[i], candidate[i + 1]);
            candidates.push_back(candidate);
        }

        return candidates;
    }

    std::vector<std::string> SuggestionEngine::generateSplitCandidates(const std::string &word) const
    {
        std::vector<std::string> candidates;

        for (size_t i = 1; i < word.length(); ++i)
        {
            std::string first_part = word.substr(0, i);
            std::string second_part = word.substr(i);

            if (dictionary_->containsWord(first_part) && dictionary_->containsWord(second_part))
            {
                candidates.push_back(first_part + " " + second_part);
            }
        }

        return candidates;
    }

    std::vector<std::string> SuggestionEngine::rankCandidates(const std::string &word,
                                                              const std::vector<std::string> &candidates) const
    {
        std::vector<std::pair<std::string, double>> scored_candidates;

        for (const auto &candidate : candidates)
        {
            double score = calculateSuggestionScore(word, candidate);
            scored_candidates.emplace_back(candidate, score);
        }

        // Sort by score (descending)
        std::sort(scored_candidates.begin(), scored_candidates.end(),
                  [](const auto &a, const auto &b)
                  {
                      return a.second > b.second;
                  });

        // Extract the ranked words
        std::vector<std::string> ranked_suggestions;
        for (const auto &scored_candidate : scored_candidates)
        {
            ranked_suggestions.push_back(scored_candidate.first);
            if (ranked_suggestions.size() >= max_suggestions_)
            {
                break;
            }
        }

        return ranked_suggestions;
    }

    double SuggestionEngine::calculateSuggestionScore(const std::string &original,
                                                      const std::string &candidate) const
    {
        double score = 0.0;

        // Edit distance component (lower distance = higher score)
        size_t edit_distance = calculateEditDistance(original, candidate);
        double edit_score = 1.0 / (1.0 + edit_distance);
        score += edit_distance_weight_ * edit_score;

        // Frequency component
        uint32_t frequency = dictionary_->getWordFrequency(candidate);
        double freq_score = std::log(1.0 + frequency) / 10.0; // Normalized log frequency
        score += frequency_weight_ * freq_score;

        // Length similarity component
        double length_ratio = static_cast<double>(std::min(original.length(), candidate.length())) /
                              static_cast<double>(std::max(original.length(), candidate.length()));
        score += 0.1 * length_ratio;

        // Common prefix bonus
        size_t common_prefix = 0;
        size_t min_len = std::min(original.length(), candidate.length());
        for (size_t i = 0; i < min_len && original[i] == candidate[i]; ++i)
        {
            common_prefix++;
        }
        double prefix_score = static_cast<double>(common_prefix) / original.length();
        score += prefix_weight_ * prefix_score;

        return score;
    }

    double SuggestionEngine::getKeyboardDistance(char c1, char c2) const
    {
        // QWERTY keyboard layout distances (simplified)
        static const std::unordered_map<char, std::pair<int, int>> qwerty_positions = {
            {'q', {0, 0}}, {'w', {0, 1}}, {'e', {0, 2}}, {'r', {0, 3}}, {'t', {0, 4}}, {'y', {0, 5}}, {'u', {0, 6}}, {'i', {0, 7}}, {'o', {0, 8}}, {'p', {0, 9}}, {'a', {1, 0}}, {'s', {1, 1}}, {'d', {1, 2}}, {'f', {1, 3}}, {'g', {1, 4}}, {'h', {1, 5}}, {'j', {1, 6}}, {'k', {1, 7}}, {'l', {1, 8}}, {'z', {2, 0}}, {'x', {2, 1}}, {'c', {2, 2}}, {'v', {2, 3}}, {'b', {2, 4}}, {'n', {2, 5}}, {'m', {2, 6}}};

        auto it1 = qwerty_positions.find(std::tolower(c1));
        auto it2 = qwerty_positions.find(std::tolower(c2));

        if (it1 == qwerty_positions.end() || it2 == qwerty_positions.end())
        {
            return 10.0; // Large distance for unknown characters
        }

        int dx = it1->second.first - it2->second.first;
        int dy = it1->second.second - it2->second.second;

        return std::sqrt(dx * dx + dy * dy);
    }

} // namespace spellcheck
