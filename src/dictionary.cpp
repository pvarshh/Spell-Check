#include "dictionary.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

namespace spellcheck
{

    Dictionary::Dictionary()
        : trie_root_(std::make_unique<TrieNode>()), word_count_(0), memory_usage_(0)
    {
    }

    bool Dictionary::loadFromFile(const std::string &file_path)
    {
        std::ifstream file(file_path);
        if (!file.is_open())
        {
            return false;
        }

        clear();

        std::string line;
        uint32_t default_frequency = 1;

        while (std::getline(file, line))
        {
            // Remove whitespace
            line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());

            if (line.empty())
            {
                continue;
            }

            // Check if line contains frequency information (word:frequency format)
            size_t colon_pos = line.find(':');
            if (colon_pos != std::string::npos)
            {
                std::string word = line.substr(0, colon_pos);
                uint32_t frequency = static_cast<uint32_t>(std::stoul(line.substr(colon_pos + 1)));
                addWord(word, frequency);
            }
            else
            {
                addWord(line, default_frequency);
            }
        }

        file.close();
        calculateMemoryUsage();

        return true;
    }

    bool Dictionary::saveToFile(const std::string &file_path) const
    {
        std::ofstream file(file_path);
        if (!file.is_open())
        {
            return false;
        }

        for (const auto &word_freq : word_frequencies_)
        {
            file << word_freq.first << ":" << word_freq.second << "\n";
        }

        file.close();
        return true;
    }

    void Dictionary::addWord(const std::string &word, uint32_t frequency)
    {
        if (word.empty())
        {
            return;
        }

        // Convert to lowercase for consistency
        std::string normalized_word = word;
        std::transform(normalized_word.begin(), normalized_word.end(),
                       normalized_word.begin(), ::tolower);

        // Add to hash set for fast lookup
        bool is_new_word = word_set_.find(normalized_word) == word_set_.end();
        word_set_.insert(normalized_word);

        // Add/update frequency
        word_frequencies_[normalized_word] = frequency;

        // Add to trie
        insertIntoTrie(normalized_word, frequency);

        // Add to phonetic map
        std::string phonetic_code = generatePhoneticCode(normalized_word);
        phonetic_map_[phonetic_code].push_back(normalized_word);

        if (is_new_word)
        {
            word_count_++;
        }
    }

    bool Dictionary::removeWord(const std::string &word)
    {
        std::string normalized_word = word;
        std::transform(normalized_word.begin(), normalized_word.end(),
                       normalized_word.begin(), ::tolower);

        auto it = word_set_.find(normalized_word);
        if (it == word_set_.end())
        {
            return false;
        }

        word_set_.erase(it);
        word_frequencies_.erase(normalized_word);

        // Remove from phonetic map
        std::string phonetic_code = generatePhoneticCode(normalized_word);
        auto &phonetic_words = phonetic_map_[phonetic_code];
        phonetic_words.erase(
            std::remove(phonetic_words.begin(), phonetic_words.end(), normalized_word),
            phonetic_words.end());

        if (phonetic_words.empty())
        {
            phonetic_map_.erase(phonetic_code);
        }

        word_count_--;
        return true;
    }

    bool Dictionary::containsWord(const std::string &word) const
    {
        std::string normalized_word = word;
        std::transform(normalized_word.begin(), normalized_word.end(),
                       normalized_word.begin(), ::tolower);

        return word_set_.find(normalized_word) != word_set_.end();
    }

    uint32_t Dictionary::getWordFrequency(const std::string &word) const
    {
        std::string normalized_word = word;
        std::transform(normalized_word.begin(), normalized_word.end(),
                       normalized_word.begin(), ::tolower);

        auto it = word_frequencies_.find(normalized_word);
        return (it != word_frequencies_.end()) ? it->second : 0;
    }

    std::vector<std::string> Dictionary::getWordsWithPrefix(const std::string &prefix,
                                                            size_t max_results) const
    {
        std::vector<std::string> results;

        std::string normalized_prefix = prefix;
        std::transform(normalized_prefix.begin(), normalized_prefix.end(),
                       normalized_prefix.begin(), ::tolower);

        // Navigate to the prefix in the trie
        TrieNode *current = trie_root_.get();
        for (char c : normalized_prefix)
        {
            auto it = current->children.find(c);
            if (it == current->children.end())
            {
                return results; // Prefix not found
            }
            current = it->second.get();
        }

        // Collect all words with this prefix
        collectWordsWithPrefix(current, normalized_prefix, results, max_results);

        // Sort by frequency (descending)
        std::sort(results.begin(), results.end(),
                  [this](const std::string &a, const std::string &b)
                  {
                      return getWordFrequency(a) > getWordFrequency(b);
                  });

        return results;
    }

    std::vector<std::string> Dictionary::getPhoneticMatches(const std::string &word) const
    {
        std::string phonetic_code = generatePhoneticCode(word);

        auto it = phonetic_map_.find(phonetic_code);
        if (it != phonetic_map_.end())
        {
            return it->second;
        }

        return {};
    }

    std::vector<std::string> Dictionary::getAllWords() const
    {
        std::vector<std::string> words;
        words.reserve(word_set_.size());

        for (const auto &word : word_set_)
        {
            words.push_back(word);
        }

        return words;
    }

    std::pair<size_t, size_t> Dictionary::getStats() const
    {
        return {word_count_, memory_usage_};
    }

    void Dictionary::clear()
    {
        word_set_.clear();
        word_frequencies_.clear();
        phonetic_map_.clear();
        trie_root_ = std::make_unique<TrieNode>();
        word_count_ = 0;
        memory_usage_ = 0;
    }

    void Dictionary::calculateMemoryUsage()
    {
        memory_usage_ = sizeof(Dictionary);

        // Calculate hash set memory
        for (const auto &word : word_set_)
        {
            memory_usage_ += word.size() + sizeof(std::string);
        }

        // Calculate frequency map memory
        for (const auto &word_freq : word_frequencies_)
        {
            memory_usage_ += word_freq.first.size() + sizeof(std::string) + sizeof(uint32_t);
        }

        // Calculate phonetic map memory
        for (const auto &phonetic_entry : phonetic_map_)
        {
            memory_usage_ += phonetic_entry.first.size() + sizeof(std::string);
            for (const auto &word : phonetic_entry.second)
            {
                memory_usage_ += word.size() + sizeof(std::string);
            }
        }

        // Note: Trie memory calculation is complex and omitted for simplicity
    }

    std::string Dictionary::generatePhoneticCode(const std::string &word) const
    {
        if (word.empty())
        {
            return "";
        }

        // Simple Soundex-like algorithm
        std::string code;
        code.reserve(4);

        // First character
        code += std::toupper(word[0]);

        // Convert consonants to numbers
        for (size_t i = 1; i < word.length() && code.length() < 4; ++i)
        {
            char c = std::tolower(word[i]);
            char digit = '0';

            switch (c)
            {
            case 'b':
            case 'f':
            case 'p':
            case 'v':
                digit = '1';
                break;
            case 'c':
            case 'g':
            case 'j':
            case 'k':
            case 'q':
            case 's':
            case 'x':
            case 'z':
                digit = '2';
                break;
            case 'd':
            case 't':
                digit = '3';
                break;
            case 'l':
                digit = '4';
                break;
            case 'm':
            case 'n':
                digit = '5';
                break;
            case 'r':
                digit = '6';
                break;
            default:
                continue; // Skip vowels and other characters
            }

            // Avoid consecutive duplicates
            if (code.empty() || code.back() != digit)
            {
                code += digit;
            }
        }

        // Pad with zeros
        while (code.length() < 4)
        {
            code += '0';
        }

        return code;
    }

    void Dictionary::insertIntoTrie(const std::string &word, uint32_t frequency)
    {
        TrieNode *current = trie_root_.get();

        for (char c : word)
        {
            if (current->children.find(c) == current->children.end())
            {
                current->children[c] = std::make_unique<TrieNode>();
            }
            current = current->children[c].get();
        }

        current->is_word = true;
        current->frequency = frequency;
    }

    void Dictionary::collectWordsWithPrefix(TrieNode *node, const std::string &prefix,
                                            std::vector<std::string> &results, size_t max_results) const
    {
        if (results.size() >= max_results)
        {
            return;
        }

        if (node->is_word)
        {
            results.push_back(prefix);
        }

        for (const auto &child : node->children)
        {
            collectWordsWithPrefix(child.second.get(), prefix + child.first, results, max_results);
        }
    }

} // namespace spellcheck
