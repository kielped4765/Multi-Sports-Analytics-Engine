#pragma once

#include "Player.hpp"
#include <string>
#include <vector>

// Providing CSV ingestion and validation utilities for loading player records from league roster files
class DataParser {
    public:
        // Reads a CSV file and converts valid rows into Player objects
        // Expected column order: name, jersey number, position, salary, rosterstatus, league, team

        // Parameters:
        // file path 
        // outPlayers
        // errors


        // Return value:
        // returns true if the file was open and read successfully
        // false if the file could not be opened

        static bool parsePlayersFromCsv(const std::string& filePath,
                                        std::vector<Player>& outPlayers,
                                        std::vector<std::string>& errors);
    };