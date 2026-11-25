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

    // Checking if a line has no real content aka only whitespace
    bool isBlankOrWhitespace(std::string_view line) {
        // trim leading/trailing spaces and see if the remaining string is empty
        return trimWhitespace(line).empty();
    }

    // break one CSV row into individual column values while respecting quotes
    std::vector<std::string> splitCsvLine(std::string_view line) {
        // stores each finished column value and builds up characters for the column currently being read
        std::vector<std::string> fields;
        std::string current;
        // tracks whether we are currently inside a quoted field
        bool inQuotes = false;

        // scan through the line one character at a time from left to right
        for (std::size_t i = 0; i < line.size(); ++i) {
            // grabbing the current character from the line
            const char ch = line[i]; 

            if (inQuotes) {
                // inside quotes: only " has special meaning; everything else stays as is
                if (ch == '"') {
                    // if the next character is also a ", this is as escaped quote
                    if (i + 1 < line.size() && line[i + 1] == '"') {
                        current.push_back('"'); // append one literal quote to the field
                        ++i; // skip the second quote
                    } else {
                        inQuotes = false; // close the quoted section
                        // a single " closes the quoted section
                    }
                } else {

                    // normal character inside quotes 
                    current.push_back(ch);
                }
            } else {
                // outside quotes: " starts a quoted field, , ends a field, other characters build it
                if (ch == '"') {
                    inQuotes = true; // start ignoring commas until a closing quote
                } else if (ch == ',') {
                    // comma outside quotes mean this column is finished
                    fields.push_back(trimWhitespace(current));
                    current.clear(); // reset buffer for next column
                } else {
                    // regular character outside quotes
                    current.push_back(ch);
                }
            }
        }

        // save the last column after the loop ends
        fields.push_back(trimWhitespace(current));
        return fields;
}

// convert text into a clean integer and reject bad inputs like "", "abc, "or "123abc"
bool parseStrictInt(std::string_view text, int& outValue) {
    // remove outer whitespace first so spaces around a number don't cause a false fail
    const std::string trimmed = trimWhitespace(text);
    if (trimmed.empty()) {
        return false;
    }

    int value = 0; // temporary sport to hold the parsed number
    // parse an int from the start of 'trimmed' up to the end of 'trimmed'
    const auto [ptr, ec] = 
        std::from_chars(trimmed.data(), trimmed.data() + trimmed.size(), value);

    // reject if parsing errored OR if 'ptr' didn't reach the end
    if (ec != std::errc{} || ptr != trimmed.data() + trimmed.size()) {
        return false;
    }

    //parsing succeeded, write result to output variable
    outValue = value;
    return true;
}

// detect if a row is the CSV header row so it isn't treated as a player
bool isHeaderRow(const std::vector<std::string>& fields) {
    // check column count and verify the first two column names match what we expect
    return fields.size() == ExpectedColumns &&
           fields[0] == "name" &&
           fields[1] == "jerseyNumber";
}


} // end of namespace

// record problems in 'errors', and return false only if the file can't be opened
bool DataParser::parsePlayersFromCsv(const std::string& filePath,
                                     std::vector<Player>& outPlayers,
                                     std::vector<std::string>& errors) {

    // clear old results so a reused vector doesn't mix old and new data
    outPlayers.clear();
    errors.clear();

    // open the file for reading
    std::ifstream file(filePath);
    if (!file.is_open()) {
        // File couldn't be opened -> log error and signal failure via `false`
        errors.push_back("Failed to open CSV file: " + filePath);
        return false;
    }

    std::string line;          // Holds one raw line at a time from the file
    std::size_t lineNumber = 0; // Tracks 1-based line numbers for clear error messages

    // Read line by line until end-of-file
    while (std::getline(file, line)) {
        ++lineNumber; // Move to the next line number (1, 2, 3, ...)

        // Strip trailing '\r' in case the CSV uses Windows (\r\n) line endings
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        // Skip empty or spaces-only lines without counting them as errors
        if (isBlankOrWhitespace(line)) {
            continue;
        }

        // Split the current line into its separate columns
        const std::vector<std::string> fields = splitCsvLine(line);

        // If we're on line 1 and it matches the header row, skip it
        if (lineNumber == 1 && isHeaderRow(fields)) {
            continue;
        }

        // Check that the row has the exact number of columns a Player requires
        if (fields.size() != ExpectedColumns) {
            errors.push_back("Line " + std::to_string(lineNumber) +
                             ": expected " + std::to_string(ExpectedColumns) +
                             " columns, got " + std::to_string(fields.size()));
            continue; // Skip this bad row and keep reading the rest of the file
        }

        // Check that the player has a name (column 0)
        if (fields[0].empty()) {
            errors.push_back("Line " + std::to_string(lineNumber) +
                             ": player name cannot be empty");
            continue;
        }

        // Validate and convert jerseyNumber (column 1) into an int
        int jerseyNumber = 0;
        if (!parseStrictInt(fields[1], jerseyNumber)) {
            errors.push_back("Line " + std::to_string(lineNumber) +
                             ": invalid jerseyNumber '" + fields[1] + "'");
            continue;
        }

        // Validate and convert salary (column 3) into an int
        int salary = 0;
        if (!parseStrictInt(fields[3], salary)) {
            errors.push_back("Line " + std::to_string(lineNumber) +
                             ": invalid salary '" + fields[3] + "'");
            continue;
        }

        // All checks passed -> build a Player struct from the validated fields
        Player player;
        player.name = fields[0];
        player.jerseyNumber = jerseyNumber;
        player.position = fields[2];
        player.salary = salary;
        player.rosterStatus = fields[4];
        player.league = fields[5];
        player.team = fields[6];

        // Add the completed Player to the output list
        outPlayers.push_back(player);
    }

    // File was opened and processed; return true even if individual rows had errors
    return true;
}

