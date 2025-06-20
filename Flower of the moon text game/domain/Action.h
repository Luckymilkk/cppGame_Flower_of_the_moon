#pragma once
#include <string>
#include <unordered_map>

namespace domain {

struct Action {
    std::string id;
    std::string name;
    std::string description;
    std::string effectType;
    std::string targetStat;
    std::string bookKey;
    std::string requiresBook;
    std::string menuText;
    int effectValue = 0;
    int maxValue = 0;
    int costSteps = 0;
    bool requiresMagicAccess = false;
    std::unordered_map<std::string, int> effects;
    // Методы для выполнения действия
    void execute() const;
};

} // namespace domain 