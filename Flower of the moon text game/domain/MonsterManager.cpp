#include "MonsterManager.h"

namespace domain {

void MonsterManager::AddMonster(const Monster& monster) {
    monsters_[monster.name] = monster;
}

const Monster* MonsterManager::GetMonster(const std::string& name) const {
    auto it = monsters_.find(name);
    if (it != monsters_.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<Monster> MonsterManager::GetAllMonsters() const {
    std::vector<Monster> result;
    for (const auto& pair : monsters_) {
        result.push_back(pair.second);
    }
    return result;
}

} // namespace domain 