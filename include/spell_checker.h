#ifndef SPELL_CHECKER_H
#define SPELL_CHECKER_H

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>
#include <functional>

namespace spellcheck
{

    // Forward declarations
    class Dictionary;
    class SuggestionEngine;
    class TextProcessor;

    /**
     * @brief Main spell checker class that coordinates all components
     */
    class SpellChecker
    {
    private:
        std::unique_ptr<Dictionary> dictionary_;
        std::unique_ptr<SuggestionEngine> suggestion_engine_;
        std::unique_ptr<TextProcessor> text_processor_;

        // Configuration options
        bool case_sensitive_;
        bool ignore_numbers_;
        bool ignore_urls_;
        size_t max_suggestions_;

    public:
        /**
         * @brief Constructor
         * @param dict_path Path to dictionary file
         */
        explicit SpellChecker(const std::string &dict_path = "");

        /**
         * @brief Destructor
         */
        ~SpellChecker();

        // Delete copy constructor and assignment operator for safety
        SpellChecker(const SpellChecker &) = delete;
        SpellChecker &operator=(const SpellChecker &) = delete;

        /**
         * @brief Load dictionary from file
         * @param dict_path Path to dictionary file
         * @return true if successful, false otherwise
         */
        bool loadDictionary(const std::string &dict_path);

        /**
         * @brief Add word to dictionary
         * @param word Word to add
         */
        void addWord(const std::string &word);

        /**
         * @brief Remove word from dictionary
         * @param word Word to remove
         */
        void removeWord(const std::string &word);

        /**
         * @brief Check if a word is spelled correctly
         * @param word Word to check
         * @return true if correct, false if misspelled
         */
        bool isCorrect(const std::string &word) const;

        /**
         * @brief Get spelling suggestions for a word
         * @param word Misspelled word
         * @return Vector of suggested corrections
         */
        std::vector<std::string> getSuggestions(const std::string &word) const;

        /**
         * @brief Check spelling of entire text
         * @param text Text to check
         * @return Vector of misspelled words with positions
         */
        std::vector<std::pair<std::string, size_t>> checkText(const std::string &text) const;

        /**
         * @brief Check spelling of file
         * @param file_path Path to file to check
         * @return Vector of misspelled words with line numbers
         */
        std::vector<std::tuple<std::string, size_t, size_t>> checkFile(const std::string &file_path) const;

        // Configuration setters
        void setCaseSensitive(bool sensitive) { case_sensitive_ = sensitive; }
        void setIgnoreNumbers(bool ignore) { ignore_numbers_ = ignore; }
        void setIgnoreUrls(bool ignore) { ignore_urls_ = ignore; }
        void setMaxSuggestions(size_t max_suggestions) { max_suggestions_ = max_suggestions; }

        // Configuration getters
        bool isCaseSensitive() const { return case_sensitive_; }
        bool ignoreNumbers() const { return ignore_numbers_; }
        bool ignoreUrls() const { return ignore_urls_; }
        size_t getMaxSuggestions() const { return max_suggestions_; }

        /**
         * @brief Get dictionary statistics
         * @return Pair of (word count, memory usage in bytes)
         */
        std::pair<size_t, size_t> getDictionaryStats() const;

        /**
         * @brief Save current dictionary to file
         * @param dict_path Path to save dictionary
         * @return true if successful, false otherwise
         */
        bool saveDictionary(const std::string &dict_path) const;
    };

} // namespace spellcheck

#endif // SPELL_CHECKER_H
