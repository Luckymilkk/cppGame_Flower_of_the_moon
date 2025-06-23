#pragma once
#include <string>
#include <unordered_map>

namespace domain {

struct Spell {
    std::string id;
    std::string name;
    std::string description;
    std::string type;
    int costSteps = 0;
    bool requiresMagicAccessToLearn = false;
    int manaCost = 0;
    std::unordered_map<std::string, int> effects;
    
    void cast();
};

} // namespace domain 