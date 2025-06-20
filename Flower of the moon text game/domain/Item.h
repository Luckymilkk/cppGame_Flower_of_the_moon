#pragma once
#include <string>
#include <unordered_map>

namespace domain {

struct Item {
    std::string id;
    std::string name;
    std::string description;
    std::string type;
    std::string prepEffectType;
    std::string bookKey;
    std::string battleActionName;
    std::string checkStat;
    int costSteps = 1;         // по умолчанию 1 шаг
    int costSlots = 0;         // по умолчанию не занимает слоты
    int prepEffectValue = 0;
    int damage = 0;
    int usesPerBattle = 0;
    bool requiresMagicAccessToUse = false;
    std::unordered_map<std::string, int> effects;
    // Методы для использования предмета
    void use();
};

} // namespace domain 