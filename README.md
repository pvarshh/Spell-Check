# Industrial Spell Checker

A high-performance, industry-level spell checking tool written entirely in C++. This spell checker features advanced algorithms, multiple data structures for optimization, and extensive customization options.

## Features

### Core Functionality
- **Fast Dictionary Lookup**: Uses hash tables for O(1) word lookups
- **Trie-based Prefix Matching**: Efficient prefix-based suggestions
- **Phonetic Matching**: Soundex-like algorithm for sound-alike suggestions
- **Multiple Edit Distance Algorithms**: Levenshtein and Damerau-Levenshtein distance
- **Advanced Suggestion Engine**: Multiple algorithms with configurable weights

### Text Processing
- **Smart Word Extraction**: Handles contractions, punctuation, and special cases
- **URL and Email Detection**: Automatically ignores URLs and email addresses
- **Number Recognition**: Option to ignore numeric values
- **Case Sensitivity**: Configurable case-sensitive or case-insensitive checking
- **Multi-format Support**: Works with plain text, code files, and documents

### Performance Optimizations
- **Memory Efficient**: Optimized data structures minimize memory usage
- **Fast Algorithms**: Industry-standard algorithms optimized for speed
- **Lazy Loading**: Efficient dictionary loading and management
- **Scalable Design**: Handles large dictionaries and documents efficiently

### User Interface
- **Command Line Interface**: Full-featured CLI with comprehensive options
- **Interactive Mode**: Real-time spell checking with immediate feedback
- **Batch Processing**: Check multiple files or entire directories
- **Detailed Reporting**: Line and column numbers for errors

## Building the Project

### Prerequisites
- CMake 3.15 or later
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- Make or Ninja build system

### Build Instructions

```bash
# Clone or navigate to the project directory
cd "Spell Check"

# Create build directory
mkdir build
cd build

# Configure the project
cmake ..

# Build the project
make -j$(nproc)

# Optional: Install the binary
sudo make install
```

### Build Options

```bash
# Debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release build (default)
cmake -DCMAKE_BUILD_TYPE=Release ..

# Enable testing
cmake -DBUILD_TESTS=ON ..
```

## Usage

### Basic Usage

```bash
# Check a file
./spell_checker document.txt

# Check a single word
./spell_checker -w "teh"

# Interactive mode
./spell_checker -i

# Use custom dictionary
./spell_checker -d my_dictionary.dict document.txt
```

### Command Line Options

```
Usage: spell_checker [OPTIONS] [FILE]

Options:
  -d, --dictionary PATH    Specify dictionary file (default: dictionaries/en_US.dict)
  -i, --interactive        Interactive mode for spell checking
  -c, --case-sensitive     Enable case-sensitive checking
  --ignore-numbers         Ignore numbers (default: true)
  --ignore-urls            Ignore URLs (default: true)
  -s, --suggestions N      Maximum number of suggestions (default: 10)
  -w, --word WORD          Check a single word
  -a, --add WORD           Add word to dictionary
  -r, --remove WORD        Remove word from dictionary
  --stats                  Show dictionary statistics
  -h, --help               Show help message
```

### Examples

```bash
# Check spelling in a C++ source file
./spell_checker src/main.cpp

# Check with case sensitivity enabled
./spell_checker -c important_document.txt

# Get more suggestions for misspelled words
./spell_checker -s 15 document.txt

# Add a word to the dictionary
./spell_checker -a "cryptocurrency" -d my_dict.dict

# Check dictionary statistics
./spell_checker --stats -d dictionaries/en_US.dict

# Interactive spell checking session
./spell_checker -i
> teh
Word: "teh" - Suggestions: the, tea, ten
> add neologism
Added "neologism" to dictionary.
> stats
Dictionary contains 1543 words, using 245 KB of memory.
> quit
```

## Dictionary Format

The spell checker uses a simple text format for dictionaries:

```
word:frequency
another:100
example:50
```

- Each line contains a word and its frequency separated by a colon
- Frequency is optional (defaults to 1)
- Higher frequency words are preferred in suggestions
- Words are automatically converted to lowercase

## Architecture

### Core Components

1. **SpellChecker**: Main orchestrator class that coordinates all components
2. **Dictionary**: High-performance dictionary with multiple data structures
3. **SuggestionEngine**: Advanced suggestion generation using multiple algorithms
4. **TextProcessor**: Text parsing and normalization utilities

### Data Structures

- **Hash Table**: O(1) word lookup for spell checking
- **Trie**: Efficient prefix matching and suggestion generation
- **Phonetic Map**: Sound-alike word matching using phonetic codes
- **Frequency Map**: Word frequency storage for suggestion ranking

### Algorithms

- **Levenshtein Distance**: Standard edit distance calculation
- **Damerau-Levenshtein Distance**: Edit distance including transpositions
- **Soundex-like Phonetic Matching**: Sound-based word similarity
- **Keyboard Distance**: Physical key proximity for typo detection

## Performance Characteristics

### Time Complexity
- Word lookup: O(1) average case
- Suggestion generation: O(n*m) where n is dictionary size, m is word length
- Prefix matching: O(k) where k is number of matching words

### Memory Usage
- Dictionary: ~10-15 bytes per word (depending on word length)
- Trie: Additional ~5-8 bytes per word for prefix structure
- Phonetic map: ~3-5 bytes per word for sound matching

### Benchmarks (approximate)
- 100,000 word dictionary: ~2-3 MB memory usage
- Single word check: <1ms
- File with 1000 words: ~10-50ms
- Suggestion generation: 1-10ms per word

## Customization

### Configuration Options

The spell checker can be customized through various options:

```cpp
SpellChecker checker;
checker.setCaseSensitive(true);
checker.setIgnoreNumbers(false);
checker.setIgnoreUrls(false);
checker.setMaxSuggestions(20);
```

### Suggestion Engine Tuning

```cpp
SuggestionEngine engine;
engine.setMaxEditDistance(3);
engine.setEditDistanceWeight(1.0);
engine.setFrequencyWeight(0.8);
engine.setPhoneticWeight(0.4);
engine.setPrefixWeight(0.3);
```

## Extension Points

### Adding New Languages
1. Create language-specific dictionary files
2. Implement language-specific phonetic algorithms
3. Add character set handling for non-Latin scripts

### Custom Suggestion Algorithms
1. Inherit from SuggestionEngine
2. Override suggestion generation methods
3. Implement custom ranking algorithms

### File Format Support
1. Extend TextProcessor for new formats
2. Add format-specific word extraction logic
3. Handle format-specific ignore patterns

## Contributing

### Code Style
- Follow C++17 best practices
- Use RAII and smart pointers
- Maintain const-correctness
- Include comprehensive documentation

### Testing
- Write unit tests for new features
- Ensure backward compatibility
- Test with large dictionaries and files
- Verify memory usage and performance

### Performance Guidelines
- Profile code changes with realistic data
- Maintain O(1) lookup performance
- Optimize for common use cases
- Consider memory usage implications

## License

This project is released under the MIT License. See the LICENSE file for details.

## Technical Details

### Memory Management
- Uses RAII and smart pointers throughout
- No manual memory management required
- Automatic cleanup on destruction
- Memory usage tracking and reporting

### Thread Safety
- Current implementation is not thread-safe
- Dictionary modifications should be synchronized
- Read-only operations can be parallelized with proper design

### Error Handling
- Comprehensive error checking for file operations
- Graceful handling of malformed dictionaries
- Clear error messages for user guidance
- Robust recovery from partial failures

### Future Enhancements
- Multi-threading support for large files
- Machine learning-based suggestions
- Context-aware spell checking
- Grammar checking integration
- Web service API
- GUI interface
- Plugin architecture for text editors
