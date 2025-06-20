#include "ItemManager.h"
#include <iostream>

namespace domain {

void ItemManager::addItem(const Item& item) {
    if (item.id.empty()) {
        std::cerr << "Ошибка: попытка добавить предмет с пустым id! Имя предмета: " << item.name << std::endl;
        return;
    }
    items[item.id] = item;
}

const Item* ItemManager::getItem(const std::string& id) const {
    auto it = items.find(id);
    if (it != items.end()) {
        return &it->second;
    }
    return nullptr;
}

const Item* ItemManager::findItemByName(const std::string& name) const {
    for (const auto& pair : items) {
        if (pair.second.name == name) {
            return &pair.second;
        }
    }
    return nullptr;
}

std::vector<Item> ItemManager::getAllItems() const {
    std::vector<Item> result;
    for (const auto& pair : items) {
        result.push_back(pair.second);
    }
    return result;
}

} // namespace domain 