#pragma once

#include "Player.hpp"

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

class RosterManager {
public:

        // adds a player if not already on roster
        // returns false and sets 'errorMessage' to "player already on roster" on duplications
        bool addPlayer(const Player& player, std::string& errorMessage);

        // loads players using DataParser; reports any parse/duplicate issues
        bool loadFromCsv(const std::string& filePath, std::vector<std::string>& errors);

        // returns the matching player if found, or std::nullopt if not found
        std::optional<Player> findPlayerByName(std::string_view name) const;

        // returns all players matching a given team or league
        std::vector<Player> getPlayersByTeam(std::string_view team) const;
        std::vector<Player> getPlayersByLeague(std::string_view league) const;

        std::size_t playerCount() const;
        long long totalSalary() const;

private:
    bool isDuplicate(const Player& candidate) const;

    std::vector<Player> players_;
    
};