#include "text_processor.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace spellcheck
{

    TextProcessor::TextProcessor()
        : url_regex_(R"(https?://[^\s]+|www\.[^\s]+|[a-zA-Z0-9][a-zA-Z0-9-]*\.[a-zA-Z]{2,})"), email_regex_(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})"), number_regex_(R"(\d+(?:\.\d+)?)"), word_regex_(R"([a-zA-Z]+(?:'[a-zA-Z]+)?)"), ignore_urls_(true), ignore_emails_(true), ignore_numbers_(true), case_sensitive_(false)
    {
    }

    std::vector<std::pair<std::string, size_t>> TextProcessor::extractWords(const std::string &text) const
    {
        std::vector<std::pair<std::string, size_t>> words;

        std::sregex_iterator word_iter(text.begin(), text.end(), word_regex_);
        std::sregex_iterator word_end;

        for (; word_iter != word_end; ++word_iter)
        {
            const std::smatch &match = *word_iter;
            std::string word = match.str();
            size_t position = match.position();

            if (!shouldIgnoreWord(word))
            {
                words.emplace_back(normalizeWord(word), position);
            }
        }

        return words;
    }

    std::vector<std::tuple<std::string, size_t, size_t>> TextProcessor::extractWordsWithLines(const std::string &text) const
    {
        std::vector<std::tuple<std::string, size_t, size_t>> words;

        size_t line_number = 1;
        size_t line_start = 0;

        std::sregex_iterator word_iter(text.begin(), text.end(), word_regex_);
        std::sregex_iterator word_end;

        for (; word_iter != word_end; ++word_iter)
        {
            const std::smatch &match = *word_iter;
            std::string word = match.str();
            size_t position = match.position();

            // Update line number and line start
            while (line_start <= position)
            {
                size_t next_newline = text.find('\n', line_start);
                if (next_newline == std::string::npos || next_newline > position)
                {
                    break;
                }
                line_number++;
                line_start = next_newline + 1;
            }

            size_t column = position - line_start + 1;

            if (!shouldIgnoreWord(word))
            {
                words.emplace_back(normalizeWord(word), line_number, column);
            }
        }

        return words;
    }

    std::string TextProcessor::normalizeWord(const std::string &word) const
    {
        std::string normalized = removePunctuation(word);

        if (!case_sensitive_)
        {
            normalized = toLowerCase(normalized);
        }

        return normalized;
    }

    bool TextProcessor::shouldIgnoreWord(const std::string &word) const
    {
        if (word.empty())
        {
            return true;
        }

        // Check if it's a URL
        if (ignore_urls_ && isUrl(word))
        {
            return true;
        }

        // Check if it's an email
        if (ignore_emails_ && isEmail(word))
        {
            return true;
        }

        // Check if it's a number
        if (ignore_numbers_ && isNumber(word))
        {
            return true;
        }

        // Ignore very short words (1-2 characters)
        if (word.length() <= 2)
        {
            return true;
        }

        // Ignore words that are not alphabetic
        if (!isAlphabetic(word))
        {
            return true;
        }

        return false;
    }

    std::string TextProcessor::removePunctuation(const std::string &word) const
    {
        std::string result;
        result.reserve(word.length());

        for (char c : word)
        {
            if (std::isalnum(c) || c == '\'')
            { // Keep apostrophes for contractions
                result += c;
            }
        }

        return result;
    }

    std::string TextProcessor::toLowerCase(const std::string &word) const
    {
        std::string result = word;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }

    bool TextProcessor::isUrl(const std::string &text) const
    {
        return std::regex_match(text, url_regex_);
    }

    bool TextProcessor::isEmail(const std::string &text) const
    {
        return std::regex_match(text, email_regex_);
    }

    bool TextProcessor::isNumber(const std::string &text) const
    {
        return std::regex_match(text, number_regex_);
    }

    bool TextProcessor::isAlphabetic(const std::string &word) const
    {
        if (word.empty())
        {
            return false;
        }

        for (char c : word)
        {
            if (!std::isalpha(c) && c != '\'')
            { // Allow apostrophes
                return false;
            }
        }

        return true;
    }

    std::vector<std::string> TextProcessor::splitIntoSentences(const std::string &text) const
    {
        std::vector<std::string> sentences;
        std::regex sentence_regex(R"([.!?]+\s+)");

        std::sregex_token_iterator iter(text.begin(), text.end(), sentence_regex, -1);
        std::sregex_token_iterator end;

        for (; iter != end; ++iter)
        {
            std::string sentence = iter->str();
            // Trim whitespace
            sentence.erase(0, sentence.find_first_not_of(" \t\n\r"));
            sentence.erase(sentence.find_last_not_of(" \t\n\r") + 1);

            if (!sentence.empty())
            {
                sentences.push_back(sentence);
            }
        }

        return sentences;
    }

    size_t TextProcessor::countWords(const std::string &text) const
    {
        auto words = extractWords(text);
        return words.size();
    }

    size_t TextProcessor::countLines(const std::string &text) const
    {
        return std::count(text.begin(), text.end(), '\n') + 1;
    }

    std::string TextProcessor::readFile(const std::string &file_path)
    {
        std::ifstream file(file_path);
        if (!file.is_open())
        {
            return "";
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();

        return buffer.str();
    }

    bool TextProcessor::writeFile(const std::string &file_path, const std::string &content)
    {
        std::ofstream file(file_path);
        if (!file.is_open())
        {
            return false;
        }

        file << content;
        file.close();

        return true;
    }

    bool TextProcessor::fileExists(const std::string &file_path)
    {
        std::ifstream file(file_path);
        return file.good();
    }

} // namespace spellcheck
