#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <memory>
#include <fstream>
#include <iostream>

namespace spellcheck
{

    /**
     * @brief Trie node for efficient word storage and prefix matching
     */
    struct TrieNode
    {
        std::unordered_map<char, std::unique_ptr<TrieNode>> children;
        bool is_word;
        uint32_t frequency;

        TrieNode() : is_word(false), frequency(0) {}
    };

    /**
     * @brief High-performance dictionary class using multiple data structures
     */
    class Dictionary
    {
    private:
        std::unique_ptr<TrieNode> trie_root_;
        std::unordered_set<std::string> word_set_; // For O(1) lookups
        std::unordered_map<std::string, uint32_t> word_frequencies_;

        // Phonetic hash map for sound-alike matching
        std::unordered_map<std::string, std::vector<std::string>> phonetic_map_;

        size_t word_count_;
        size_t memory_usage_;

        /**
         * @brief Calculate memory usage of the dictionary
         */
        void calculateMemoryUsage();

        /**
         * @brief Generate phonetic code for a word (Soundex-like algorithm)
         * @param word Input word
         * @return Phonetic code
         */
        std::string generatePhoneticCode(const std::string &word) const;

        /**
         * @brief Insert word into trie
         * @param word Word to insert
         * @param frequency Word frequency
         */
        void insertIntoTrie(const std::string &word, uint32_t frequency);

        /**
         * @brief Collect all words with given prefix from trie
         * @param node Current trie node
         * @param prefix Current prefix
         * @param results Vector to store results
         * @param max_results Maximum number of results
         */
        void collectWordsWithPrefix(TrieNode *node, const std::string &prefix,
                                    std::vector<std::string> &results, size_t max_results) const;

    public:
        /**
         * @brief Constructor
         */
        Dictionary();

        /**
         * @brief Destructor
         */
        ~Dictionary() = default;

        // Delete copy constructor and assignment operator
        Dictionary(const Dictionary &) = delete;
        Dictionary &operator=(const Dictionary &) = delete;

        /**
         * @brief Load dictionary from file
         * @param file_path Path to dictionary file
         * @return true if successful, false otherwise
         */
        bool loadFromFile(const std::string &file_path);

        /**
         * @brief Save dictionary to file
         * @param file_path Path to save dictionary
         * @return true if successful, false otherwise
         */
        bool saveToFile(const std::string &file_path) const;

        /**
         * @brief Add word to dictionary
         * @param word Word to add
         * @param frequency Word frequency (default: 1)
         */
        void addWord(const std::string &word, uint32_t frequency = 1);

        /**
         * @brief Remove word from dictionary
         * @param word Word to remove
         * @return true if word was found and removed
         */
        bool removeWord(const std::string &word);

        /**
         * @brief Check if word exists in dictionary
         * @param word Word to check
         * @return true if word exists
         */
        bool containsWord(const std::string &word) const;

        /**
         * @brief Get word frequency
         * @param word Word to get frequency for
         * @return Word frequency (0 if not found)
         */
        uint32_t getWordFrequency(const std::string &word) const;

        /**
         * @brief Get words with given prefix
         * @param prefix Prefix to search for
         * @param max_results Maximum number of results (default: 100)
         * @return Vector of words with the prefix
         */
        std::vector<std::string> getWordsWithPrefix(const std::string &prefix,
                                                    size_t max_results = 100) const;

        /**
         * @brief Get words with similar phonetic code
         * @param word Word to find phonetic matches for
         * @return Vector of phonetically similar words
         */
        std::vector<std::string> getPhoneticMatches(const std::string &word) const;

        /**
         * @brief Get all words in dictionary
         * @return Vector of all words
         */
        std::vector<std::string> getAllWords() const;

        /**
         * @brief Get dictionary statistics
         * @return Pair of (word count, memory usage)
         */
        std::pair<size_t, size_t> getStats() const;

        /**
         * @brief Clear all words from dictionary
         */
        void clear();

        /**
         * @brief Get number of words in dictionary
         * @return Number of words
         */
        size_t size() const { return word_count_; }

        /**
         * @brief Check if dictionary is empty
         * @return true if empty
         */
        bool empty() const { return word_count_ == 0; }
    };

} // namespace spellcheck

#endif // DICTIONARY_H
