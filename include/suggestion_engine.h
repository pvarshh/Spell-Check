#ifndef SUGGESTION_ENGINE_H
#define SUGGESTION_ENGINE_H

#include <string>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <memory>

namespace spellcheck
{

    // Forward declaration
    class Dictionary;

    /**
     * @brief Advanced suggestion engine using multiple algorithms
     */
    class SuggestionEngine
    {
    private:
        const Dictionary *dictionary_;
        size_t max_edit_distance_;
        size_t max_suggestions_;

        // Weights for different suggestion types
        double edit_distance_weight_;
        double frequency_weight_;
        double phonetic_weight_;
        double prefix_weight_;

        /**
         * @brief Calculate Levenshtein edit distance between two words
         * @param word1 First word
         * @param word2 Second word
         * @return Edit distance
         */
        size_t calculateEditDistance(const std::string &word1, const std::string &word2) const;

        /**
         * @brief Calculate Damerau-Levenshtein distance (includes transpositions)
         * @param word1 First word
         * @param word2 Second word
         * @return Edit distance including transpositions
         */
        size_t calculateDamerauLevenshteinDistance(const std::string &word1, const std::string &word2) const;

        /**
         * @brief Generate candidates by character deletion
         * @param word Input word
         * @return Vector of deletion candidates
         */
        std::vector<std::string> generateDeletionCandidates(const std::string &word) const;

        /**
         * @brief Generate candidates by character insertion
         * @param word Input word
         * @return Vector of insertion candidates
         */
        std::vector<std::string> generateInsertionCandidates(const std::string &word) const;

        /**
         * @brief Generate candidates by character substitution
         * @param word Input word
         * @return Vector of substitution candidates
         */
        std::vector<std::string> generateSubstitutionCandidates(const std::string &word) const;

        /**
         * @brief Generate candidates by character transposition
         * @param word Input word
         * @return Vector of transposition candidates
         */
        std::vector<std::string> generateTranspositionCandidates(const std::string &word) const;

        /**
         * @brief Generate candidates by splitting word
         * @param word Input word
         * @return Vector of split candidates
         */
        std::vector<std::string> generateSplitCandidates(const std::string &word) const;

        /**
         * @brief Filter and rank candidates based on various criteria
         * @param word Original word
         * @param candidates Vector of candidate words
         * @return Ranked vector of suggestions
         */
        std::vector<std::string> rankCandidates(const std::string &word,
                                                const std::vector<std::string> &candidates) const;

        /**
         * @brief Calculate suggestion score for ranking
         * @param original Original word
         * @param candidate Candidate word
         * @return Score (higher is better)
         */
        double calculateSuggestionScore(const std::string &original, const std::string &candidate) const;

        /**
         * @brief Get keyboard layout distance for character substitution
         * @param c1 First character
         * @param c2 Second character
         * @return Distance on QWERTY keyboard (lower is closer)
         */
        double getKeyboardDistance(char c1, char c2) const;

    public:
        /**
         * @brief Constructor
         * @param dictionary Pointer to dictionary to use for suggestions
         */
        explicit SuggestionEngine(const Dictionary *dictionary);

        /**
         * @brief Destructor
         */
        ~SuggestionEngine() = default;

        // Delete copy constructor and assignment operator
        SuggestionEngine(const SuggestionEngine &) = delete;
        SuggestionEngine &operator=(const SuggestionEngine &) = delete;

        /**
         * @brief Generate spelling suggestions for a word
         * @param word Misspelled word
         * @return Vector of suggested corrections, ranked by relevance
         */
        std::vector<std::string> generateSuggestions(const std::string &word) const;

        /**
         * @brief Generate suggestions using edit distance only
         * @param word Misspelled word
         * @param max_distance Maximum edit distance to consider
         * @return Vector of suggestions within edit distance
         */
        std::vector<std::string> generateEditDistanceSuggestions(const std::string &word,
                                                                 size_t max_distance) const;

        /**
         * @brief Generate phonetic suggestions
         * @param word Misspelled word
         * @return Vector of phonetically similar words
         */
        std::vector<std::string> generatePhoneticSuggestions(const std::string &word) const;

        /**
         * @brief Generate prefix-based suggestions
         * @param word Misspelled word
         * @return Vector of words with similar prefixes
         */
        std::vector<std::string> generatePrefixSuggestions(const std::string &word) const;

        // Configuration setters
        void setMaxEditDistance(size_t max_distance) { max_edit_distance_ = max_distance; }
        void setMaxSuggestions(size_t max_suggestions) { max_suggestions_ = max_suggestions; }
        void setEditDistanceWeight(double weight) { edit_distance_weight_ = weight; }
        void setFrequencyWeight(double weight) { frequency_weight_ = weight; }
        void setPhoneticWeight(double weight) { phonetic_weight_ = weight; }
        void setPrefixWeight(double weight) { prefix_weight_ = weight; }

        // Configuration getters
        size_t getMaxEditDistance() const { return max_edit_distance_; }
        size_t getMaxSuggestions() const { return max_suggestions_; }
        double getEditDistanceWeight() const { return edit_distance_weight_; }
        double getFrequencyWeight() const { return frequency_weight_; }
        double getPhoneticWeight() const { return phonetic_weight_; }
        double getPrefixWeight() const { return prefix_weight_; }

        /**
         * @brief Set dictionary reference
         * @param dictionary Pointer to dictionary
         */
        void setDictionary(const Dictionary *dictionary) { dictionary_ = dictionary; }
    };

} // namespace spellcheck

#endif // SUGGESTION_ENGINE_H
