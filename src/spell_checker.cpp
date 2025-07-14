#include "spell_checker.h"
#include "dictionary.h"
#include "suggestion_engine.h"
#include "text_processor.h"

namespace spellcheck
{

    SpellChecker::SpellChecker(const std::string &dict_path)
        : case_sensitive_(false), ignore_numbers_(true), ignore_urls_(true), max_suggestions_(10)
    {

        dictionary_ = std::make_unique<Dictionary>();
        text_processor_ = std::make_unique<TextProcessor>();
        suggestion_engine_ = std::make_unique<SuggestionEngine>(dictionary_.get());

        // Configure text processor
        text_processor_->setCaseSensitive(case_sensitive_);
        text_processor_->setIgnoreNumbers(ignore_numbers_);
        text_processor_->setIgnoreUrls(ignore_urls_);

        // Load default dictionary if provided
        if (!dict_path.empty())
        {
            loadDictionary(dict_path);
        }
    }

    SpellChecker::~SpellChecker() = default;

    bool SpellChecker::loadDictionary(const std::string &dict_path)
    {
        if (!TextProcessor::fileExists(dict_path))
        {
            std::cerr << "Dictionary file not found: " << dict_path << std::endl;
            return false;
        }

        bool success = dictionary_->loadFromFile(dict_path);
        if (success)
        {
            suggestion_engine_->setDictionary(dictionary_.get());
            std::cout << "Loaded dictionary with " << dictionary_->size() << " words" << std::endl;
        }
        else
        {
            std::cerr << "Failed to load dictionary from: " << dict_path << std::endl;
        }

        return success;
    }

    void SpellChecker::addWord(const std::string &word)
    {
        if (!word.empty())
        {
            dictionary_->addWord(word);
        }
    }

    void SpellChecker::removeWord(const std::string &word)
    {
        dictionary_->removeWord(word);
    }

    bool SpellChecker::isCorrect(const std::string &word) const
    {
        if (word.empty())
        {
            return true;
        }

        // Check if word should be ignored
        if (text_processor_->shouldIgnoreWord(word))
        {
            return true;
        }

        // Normalize the word
        std::string normalized_word = text_processor_->normalizeWord(word);

        // Check in dictionary
        bool found = dictionary_->containsWord(normalized_word);

        // If case insensitive and not found, try lowercase
        if (!found && !case_sensitive_)
        {
            std::string lowercase_word = text_processor_->toLowerCase(normalized_word);
            found = dictionary_->containsWord(lowercase_word);
        }

        return found;
    }

    std::vector<std::string> SpellChecker::getSuggestions(const std::string &word) const
    {
        if (word.empty())
        {
            return {};
        }

        // Normalize the word
        std::string normalized_word = text_processor_->normalizeWord(word);

        // Generate suggestions
        std::vector<std::string> suggestions = suggestion_engine_->generateSuggestions(normalized_word);

        // Limit number of suggestions
        if (suggestions.size() > max_suggestions_)
        {
            suggestions.resize(max_suggestions_);
        }

        return suggestions;
    }

    std::vector<std::pair<std::string, size_t>> SpellChecker::checkText(const std::string &text) const
    {
        std::vector<std::pair<std::string, size_t>> misspelled_words;

        // Extract words from text
        auto words_with_positions = text_processor_->extractWords(text);

        // Check each word
        for (const auto &word_pos : words_with_positions)
        {
            const std::string &word = word_pos.first;
            size_t position = word_pos.second;

            if (!isCorrect(word))
            {
                misspelled_words.emplace_back(word, position);
            }
        }

        return misspelled_words;
    }

    std::vector<std::tuple<std::string, size_t, size_t>> SpellChecker::checkFile(const std::string &file_path) const
    {
        std::vector<std::tuple<std::string, size_t, size_t>> misspelled_words;

        // Read file contents
        std::string file_contents = TextProcessor::readFile(file_path);
        if (file_contents.empty())
        {
            std::cerr << "Could not read file: " << file_path << std::endl;
            return misspelled_words;
        }

        // Extract words with line numbers
        auto words_with_lines = text_processor_->extractWordsWithLines(file_contents);

        // Check each word
        for (const auto &word_info : words_with_lines)
        {
            const std::string &word = std::get<0>(word_info);
            size_t line = std::get<1>(word_info);
            size_t column = std::get<2>(word_info);

            if (!isCorrect(word))
            {
                misspelled_words.emplace_back(word, line, column);
            }
        }

        return misspelled_words;
    }

    std::pair<size_t, size_t> SpellChecker::getDictionaryStats() const
    {
        return dictionary_->getStats();
    }

    bool SpellChecker::saveDictionary(const std::string &dict_path) const
    {
        return dictionary_->saveToFile(dict_path);
    }

} // namespace spellcheck
