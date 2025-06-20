#ifndef DOMAIN_MONSTER_MANAGER_H_
#define DOMAIN_MONSTER_MANAGER_H_

#include "Monster.h"
#include <unordered_map>
#include <string>
#include <vector>

namespace domain {

class MonsterManager {
public:
    void AddMonster(const Monster& monster);
    const Monster* GetMonster(const std::string& name) const;
    std::vector<Monster> GetAllMonsters() const;
private:
    std::unordered_map<std::string, Monster> monsters_;
};

} // namespace domain 

#endif  // DOMAIN_MONSTER_MANAGER_H_ 