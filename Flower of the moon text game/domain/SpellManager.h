#ifndef DOMAIN_SPELL_MANAGER_H_
#define DOMAIN_SPELL_MANAGER_H_

#include "Spell.h"
#include <unordered_map>
#include <string>
#include <vector>

namespace domain {

class SpellManager {
public:
    void AddSpell(const Spell& spell);
    const Spell* GetSpell(const std::string& id) const;
    std::vector<Spell> GetAllSpells() const;
private:
    std::unordered_map<std::string, Spell> spells_;
};

} // namespace domain 

#endif  // DOMAIN_SPELL_MANAGER_H_ 