#pragma once

#include <iostream>
#include <string>


// Designed as a struct so its fields are publicly accessible for data ingestion and lookup components
struct Player {

    // Players name (Patrick Mahomes)
    std::string name;

    // Uniform number (15)
    int jerseyNumber = 0;

    // Primary position or role in sport (Quarterback)
    std::string position;

    // Annaul compensation in whole dollars; defaults to 0 if unknown
    int salary = 0;

    // Roster role status, such as starter, backup, or reserve
    std::string rosterStatus;

    // League identifier matching data source (NFL, NBA, etc.)
    std::string league;

    // Team identifier matching data source (Kansas City Chiefs, Los Angeles Lakers, etc.)
    std::string team;

    // Default constructor ensures all fields start in a known, deterministic state
    Player() = default;

    // Parameterized constructor allows direct initialization of all fields
    Player(std::string playerName,
           int number,
           std::string playerPosition,
           int playerSalary,
           std::string status,
           std::string playerLeague,
           std::string playerTeam)
        : name(std::move(playerName)),
          jerseyNumber(number),
          position(std::move(playerPosition)),
          salary(playerSalary),
          rosterStatus(std::move(status)),
          league(std::move(playerLeague)),
          team(std::move(playerTeam)) {}


          // Prints a single line human readable summary of the players information
          void printSummary() const {
            std::cout << "Name: " << name
                      << ", Jersey Number: " << jerseyNumber
                      << ", Position: " << position
                      << ", Salary: $" << salary
                      << ", Roster Status: " << rosterStatus
                      << ", League: " << league
                      << ", Team: " << team << std::endl;
          }

};