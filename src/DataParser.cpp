#include "DataParser.hpp"
#include "Player.hpp"

#include <cctype>       // std::isspace
#include <charconv>     // std::from_chars for fast, locale-independent number parsing
#include <fstream>      // std::ifstream for reading files from disk
#include <string>       // std::string and std::getline
#include <string_view>  // Read-only string views to avoid unnecessary string copies
#include <system_error> // std::errc error codes returned by std::from_chars
#include <vector>       // std::vector for dynamic lists of fields, players, and errors

namespace {

constexpr std::size_t ExpectedColumns = 7;

// removes leading and trailing whitespace from a string view and returns a clean std::string
std::string trimWhitespace(std::string_view input) {
    std::size_t start = 0;
    // Advance past leading whitespace, the goal is to find the start of the non-whitespace content
    while (start < input.size() &&
        std::isspace(static_cast<unsigned char>(input[start]))) {
        ++start;

    }

    std::size_t end = input.size();
    // Move backward past trailing whitespace, the goal is to find the end of the non-whitespace content
    while (end > start &&
        std::isspace(static_cast<unsigned char>(input[end - 1]))) {
        --end;
    }

    // Return the trimmed substring, starting from the first non-whitespace character and ending at the last non-whitespace character
    return std::string(input.substr(start, end - start));
}


}