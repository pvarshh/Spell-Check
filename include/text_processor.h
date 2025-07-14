#ifndef TEXT_PROCESSOR_H
#define TEXT_PROCESSOR_H

#include <string>
#include <vector>
#include <regex>
#include <cctype>
#include <algorithm>

namespace spellcheck
{

    /**
     * @brief Text processing utilities for spell checking
     */
    class TextProcessor
    {
    private:
        // Regular expressions for various text patterns
        std::regex url_regex_;
        std::regex email_regex_;
        std::regex number_regex_;
        std::regex word_regex_;

        bool ignore_urls_;
        bool ignore_emails_;
        bool ignore_numbers_;
        bool case_sensitive_;

    public:
        /**
         * @brief Constructor
         */
        TextProcessor();

        /**
         * @brief Destructor
         */
        ~TextProcessor() = default;

        /**
         * @brief Extract words from text
         * @param text Input text
         * @return Vector of words with their positions
         */
        std::vector<std::pair<std::string, size_t>> extractWords(const std::string &text) const;

        /**
         * @brief Extract words from text with line numbers
         * @param text Input text
         * @return Vector of words with line and column positions
         */
        std::vector<std::tuple<std::string, size_t, size_t>> extractWordsWithLines(const std::string &text) const;

        /**
         * @brief Normalize word for spell checking
         * @param word Input word
         * @return Normalized word
         */
        std::string normalizeWord(const std::string &word) const;

        /**
         * @brief Check if word should be ignored during spell checking
         * @param word Word to check
         * @return true if word should be ignored
         */
        bool shouldIgnoreWord(const std::string &word) const;

        /**
         * @brief Remove punctuation from word
         * @param word Input word
         * @return Word without punctuation
         */
        std::string removePunctuation(const std::string &word) const;

        /**
         * @brief Convert word to lowercase
         * @param word Input word
         * @return Lowercase word
         */
        std::string toLowerCase(const std::string &word) const;

        /**
         * @brief Check if string is a URL
         * @param text Text to check
         * @return true if text is a URL
         */
        bool isUrl(const std::string &text) const;

        /**
         * @brief Check if string is an email address
         * @param text Text to check
         * @return true if text is an email
         */
        bool isEmail(const std::string &text) const;

        /**
         * @brief Check if string is a number
         * @param text Text to check
         * @return true if text is a number
         */
        bool isNumber(const std::string &text) const;

        /**
         * @brief Check if word contains only alphabetic characters
         * @param word Word to check
         * @return true if word is alphabetic
         */
        bool isAlphabetic(const std::string &word) const;

        /**
         * @brief Split text into sentences
         * @param text Input text
         * @return Vector of sentences
         */
        std::vector<std::string> splitIntoSentences(const std::string &text) const;

        /**
         * @brief Count words in text
         * @param text Input text
         * @return Number of words
         */
        size_t countWords(const std::string &text) const;

        /**
         * @brief Count lines in text
         * @param text Input text
         * @return Number of lines
         */
        size_t countLines(const std::string &text) const;

        // Configuration setters
        void setIgnoreUrls(bool ignore) { ignore_urls_ = ignore; }
        void setIgnoreEmails(bool ignore) { ignore_emails_ = ignore; }
        void setIgnoreNumbers(bool ignore) { ignore_numbers_ = ignore; }
        void setCaseSensitive(bool sensitive) { case_sensitive_ = sensitive; }

        // Configuration getters
        bool ignoreUrls() const { return ignore_urls_; }
        bool ignoreEmails() const { return ignore_emails_; }
        bool ignoreNumbers() const { return ignore_numbers_; }
        bool isCaseSensitive() const { return case_sensitive_; }

        /**
         * @brief Read file contents
         * @param file_path Path to file
         * @return File contents as string
         */
        static std::string readFile(const std::string &file_path);

        /**
         * @brief Write text to file
         * @param file_path Path to file
         * @param content Content to write
         * @return true if successful
         */
        static bool writeFile(const std::string &file_path, const std::string &content);

        /**
         * @brief Check if file exists
         * @param file_path Path to file
         * @return true if file exists
         */
        static bool fileExists(const std::string &file_path);
    };

} // namespace spellcheck

#endif // TEXT_PROCESSOR_H
