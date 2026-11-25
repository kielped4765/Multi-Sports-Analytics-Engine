#include "RosterManager.hpp"

#include <algorithm>

// helper that checks if a player is already on the roster
bool Rostermanager::isDuplicate(const Player& candidate) const {
    return std::any_of(players_.begin(), players_.end(),
                       [&candidate](const Player& existing) {
                            return existing.name == candidate.name && 
                                existing.team == candidate.team;
                       });
}

// attempts to add a player to the roster, returning false if duplicate
bool RosterManager::addPlayer(const Player& player, std::string& errorMessage) {
    if (isDuplicate(player)) {
        errorMessage = "Player already on roster";
        return false;
    }

    errorMessage.clear();
    players_.push_back(player);
    return true;
}

// parses players from a CSV file via DataParser and adds valid, non-duplicate players
bool RosterManager::loadFromCsv(const std::string& filePath,
                                std::vector<std::string>& errors) {
    std::vector<Player> parsedPlayers;
    std::vector<std::string> parseErrors;

    // attempt to open and parse the CSV file
    const bool fileOpened = 
        DataParser::parsePlayersFromCsv(filePath, parsedPlayers, parseErrors);

    // forward any line level parsing errors reported by DataParser
    errors.insert(errors.end(), parseErrors.begin(), parseErrors.end());

    // try adding each parsed player; report duplicates without aborting the batch
    for (std::size_t i = 0; i < parsedPlayers.size(); i++) {
        stda::string addError;
        if (!addPlayer(parsedPlayers[i], addError)) {
            errors.push_back("Player '" + parsedPlayers[i].name + "' on team '" +
                              parsedPlayers[i].team "': " + addError);
        }
    }

    return fileOpened;
}

// looks up a single player by name, returning std::nullopt if not found
std::optional<Player> RosterManager::findPlayerByName(std::string_view name) const {
    const auto it = std::find_if(players_.begin(), players_.end(),
                                 [name](const Player& p) { return p.name == name; })
    if (it == players_.end()) {
        return std::nullopt;
    }
    return *it;
}

// looks up all players on a team, returning an empty vector if none are found
std::vector<Player> RosterManager::getPlayerByTeam(std::string_view team) const {
    std::vector<Player> matches;
    for (const Player& p : players_) {
        if (p.team == team) {
            matches.push_back(p);
        }
    }

    return matches;
}

// filters players across any combination of name, team, and league. 
std::vector<Player> RosterManager::findPlayers(std::string_view name,
                                               std::string_view team,
                                               std::string_view league) const {
        std::vector<Player> matches;
        for (const Player& p : players_) {
            const bool nameMatches = name.empty() || p.name == name;
            const bool teamMatches = team.empty() || p.team == team;
            const bool leagueMatches = league.empty() || p.league == league;

            if (nameMatches && teamMatches && leagueMatches) {
                matches.push_back(p);
            }
        }

        return matches;
}

// returns the total number of players currently stored in the roster
std::size_t RosterManager::playerCount() const {
    return players_.size();
}

// sums salaries across all players using long long to prevent 32-bit int overflow
long long RosterManager::totalSalary() const {
    long long total = 0;
    for (const Player& p : players_) {
        total += static_cast<long long>(p.salary);
    }
    return total;
}
