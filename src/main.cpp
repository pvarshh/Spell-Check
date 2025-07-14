#include "spell_checker.h"
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>

void printUsage(const std::string &program_name)
{
    std::cout << "Usage: " << program_name << " [OPTIONS] [FILE]\n"
              << "\nOptions:\n"
              << "  -d, --dictionary PATH    Specify dictionary file (default: dictionaries/en_US.dict)\n"
              << "  -i, --interactive        Interactive mode for spell checking\n"
              << "  -c, --case-sensitive     Enable case-sensitive checking\n"
              << "  --ignore-numbers        Ignore numbers (default: true)\n"
              << "  --ignore-urls           Ignore URLs (default: true)\n"
              << "  -s, --suggestions N     Maximum number of suggestions (default: 10)\n"
              << "  -w, --word WORD         Check a single word\n"
              << "  -a, --add WORD          Add word to dictionary\n"
              << "  -r, --remove WORD       Remove word from dictionary\n"
              << "  --stats                 Show dictionary statistics\n"
              << "  -h, --help              Show this help message\n"
              << "\nExamples:\n"
              << "  " << program_name << " document.txt\n"
              << "  " << program_name << " -w \"teh\" -d my_dict.dict\n"
              << "  " << program_name << " -i\n";
}

void printSuggestions(const std::string &word, const std::vector<std::string> &suggestions)
{
    std::cout << "Word: \"" << word << "\" - ";
    if (suggestions.empty())
    {
        std::cout << "No suggestions found.\n";
    }
    else
    {
        std::cout << "Suggestions: ";
        for (size_t i = 0; i < suggestions.size(); ++i)
        {
            std::cout << suggestions[i];
            if (i < suggestions.size() - 1)
            {
                std::cout << ", ";
            }
        }
        std::cout << "\n";
    }
}

void printFileResults(const std::vector<std::tuple<std::string, size_t, size_t>> &misspelled_words,
                      spellcheck::SpellChecker &checker)
{
    if (misspelled_words.empty())
    {
        std::cout << "No spelling errors found!\n";
        return;
    }

    std::cout << "Found " << misspelled_words.size() << " spelling error(s):\n\n";

    for (const auto &error : misspelled_words)
    {
        const std::string &word = std::get<0>(error);
        size_t line = std::get<1>(error);
        size_t column = std::get<2>(error);

        std::cout << "Line " << std::setw(4) << line
                  << ", Column " << std::setw(3) << column
                  << ": \"" << word << "\"";

        auto suggestions = checker.getSuggestions(word);
        if (!suggestions.empty())
        {
            std::cout << " -> ";
            for (size_t i = 0; i < std::min(suggestions.size(), static_cast<size_t>(3)); ++i)
            {
                std::cout << suggestions[i];
                if (i < std::min(suggestions.size(), static_cast<size_t>(3)) - 1)
                {
                    std::cout << ", ";
                }
            }
        }
        std::cout << "\n";
    }
}

void interactiveMode(spellcheck::SpellChecker &checker)
{
    std::cout << "Interactive Spell Checker\n";
    std::cout << "Enter words to check (type 'quit' to exit, 'help' for commands):\n";

    std::string input;
    while (true)
    {
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input == "quit" || input == "exit")
        {
            break;
        }

        if (input == "help")
        {
            std::cout << "Commands:\n"
                      << "  <word>        Check spelling of word\n"
                      << "  add <word>    Add word to dictionary\n"
                      << "  remove <word> Remove word from dictionary\n"
                      << "  stats         Show dictionary statistics\n"
                      << "  quit/exit     Exit interactive mode\n";
            continue;
        }

        if (input.empty())
        {
            continue;
        }

        // Parse command
        std::istringstream iss(input);
        std::string command;
        iss >> command;

        if (command == "add")
        {
            std::string word;
            iss >> word;
            if (!word.empty())
            {
                checker.addWord(word);
                std::cout << "Added \"" << word << "\" to dictionary.\n";
            }
        }
        else if (command == "remove")
        {
            std::string word;
            iss >> word;
            if (!word.empty())
            {
                checker.removeWord(word);
                std::cout << "Removed \"" << word << "\" from dictionary.\n";
            }
        }
        else if (command == "stats")
        {
            auto stats = checker.getDictionaryStats();
            std::cout << "Dictionary contains " << stats.first << " words, "
                      << "using " << (stats.second / 1024) << " KB of memory.\n";
        }
        else
        {
            // Treat as word to check
            std::string word = input;
            if (checker.isCorrect(word))
            {
                std::cout << "\"" << word << "\" is spelled correctly.\n";
            }
            else
            {
                auto suggestions = checker.getSuggestions(word);
                printSuggestions(word, suggestions);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    std::string dictionary_path = "dictionaries/en_US.dict";
    std::string file_path;
    std::string word_to_check;
    std::string word_to_add;
    std::string word_to_remove;
    bool interactive = false;
    bool case_sensitive = false;
    bool ignore_numbers = true;
    bool ignore_urls = true;
    bool show_stats = false;
    size_t max_suggestions = 10;

    // Parse command line arguments
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help")
        {
            printUsage(argv[0]);
            return 0;
        }
        else if (arg == "-d" || arg == "--dictionary")
        {
            if (i + 1 < argc)
            {
                dictionary_path = argv[++i];
            }
            else
            {
                std::cerr << "Error: Dictionary path required.\n";
                return 1;
            }
        }
        else if (arg == "-i" || arg == "--interactive")
        {
            interactive = true;
        }
        else if (arg == "-c" || arg == "--case-sensitive")
        {
            case_sensitive = true;
        }
        else if (arg == "--ignore-numbers")
        {
            ignore_numbers = true;
        }
        else if (arg == "--ignore-urls")
        {
            ignore_urls = true;
        }
        else if (arg == "-s" || arg == "--suggestions")
        {
            if (i + 1 < argc)
            {
                max_suggestions = std::stoul(argv[++i]);
            }
            else
            {
                std::cerr << "Error: Number of suggestions required.\n";
                return 1;
            }
        }
        else if (arg == "-w" || arg == "--word")
        {
            if (i + 1 < argc)
            {
                word_to_check = argv[++i];
            }
            else
            {
                std::cerr << "Error: Word required.\n";
                return 1;
            }
        }
        else if (arg == "-a" || arg == "--add")
        {
            if (i + 1 < argc)
            {
                word_to_add = argv[++i];
            }
            else
            {
                std::cerr << "Error: Word to add required.\n";
                return 1;
            }
        }
        else if (arg == "-r" || arg == "--remove")
        {
            if (i + 1 < argc)
            {
                word_to_remove = argv[++i];
            }
            else
            {
                std::cerr << "Error: Word to remove required.\n";
                return 1;
            }
        }
        else if (arg == "--stats")
        {
            show_stats = true;
        }
        else if (arg[0] != '-')
        {
            file_path = arg;
        }
        else
        {
            std::cerr << "Unknown option: " << arg << "\n";
            printUsage(argv[0]);
            return 1;
        }
    }

    // Initialize spell checker
    spellcheck::SpellChecker checker(dictionary_path);

    // Configure spell checker
    checker.setCaseSensitive(case_sensitive);
    checker.setIgnoreNumbers(ignore_numbers);
    checker.setIgnoreUrls(ignore_urls);
    checker.setMaxSuggestions(max_suggestions);

    // Handle dictionary operations
    if (!word_to_add.empty())
    {
        checker.addWord(word_to_add);
        std::cout << "Added \"" << word_to_add << "\" to dictionary.\n";
    }

    if (!word_to_remove.empty())
    {
        checker.removeWord(word_to_remove);
        std::cout << "Removed \"" << word_to_remove << "\" from dictionary.\n";
    }

    // Show statistics
    if (show_stats)
    {
        auto stats = checker.getDictionaryStats();
        std::cout << "Dictionary Statistics:\n";
        std::cout << "  Words: " << stats.first << "\n";
        std::cout << "  Memory usage: " << (stats.second / 1024) << " KB\n";
        return 0;
    }

    // Handle single word checking
    if (!word_to_check.empty())
    {
        if (checker.isCorrect(word_to_check))
        {
            std::cout << "\"" << word_to_check << "\" is spelled correctly.\n";
        }
        else
        {
            auto suggestions = checker.getSuggestions(word_to_check);
            printSuggestions(word_to_check, suggestions);
        }
        return 0;
    }

    // Handle interactive mode
    if (interactive)
    {
        interactiveMode(checker);
        return 0;
    }

    // Handle file checking
    if (!file_path.empty())
    {
        auto misspelled_words = checker.checkFile(file_path);
        printFileResults(misspelled_words, checker);
        return 0;
    }

    // If no specific action, show usage
    printUsage(argv[0]);
    return 0;
}
