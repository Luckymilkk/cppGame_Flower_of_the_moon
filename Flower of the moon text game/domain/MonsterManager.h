#pragma once
#include "Monster.h"
#include <unordered_map>
#include <string>
#include <vector>

namespace domain {

class MonsterManager {
public:
    void addMonster(const Monster& monster);
    const Monster* getMonster(const std::string& name) const;
    std::vector<Monster> getAllMonsters() const;
private:
    std::unordered_map<std::string, Monster> monsters;
};

} // namespace domain 