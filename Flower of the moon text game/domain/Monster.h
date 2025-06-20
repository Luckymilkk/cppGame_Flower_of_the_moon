#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace domain {

struct Monster {
    std::string name;
    int health;
    int attack;
    int defense;
    int experienceReward;
    std::string bookKey;
    std::vector<std::string> favorableActions;
    std::vector<std::string> unfavorableActions;
    std::vector<std::string> neutralActions;
    std::unordered_map<std::string, int> stats;
    // Методы для управления монстром
    void takeDamage(int amount);
    bool isAlive() const;
};

} // namespace domain 