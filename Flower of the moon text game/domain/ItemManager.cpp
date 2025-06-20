#include "ItemManager.h"
#include <iostream>

namespace domain {

void ItemManager::AddItem(const Item& item) {
    if (item.id.empty()) {
        std::cerr << "Ошибка: попытка добавить предмет с пустым id! Имя предмета: " << item.name << std::endl;
        return;
    }
    items_[item.id] = item;
}

const Item* ItemManager::GetItem(const std::string& id) const {
    auto it = items_.find(id);
    if (it != items_.end()) {
        return &it->second;
    }
    return nullptr;
}

const Item* ItemManager::FindItemByName(const std::string& name) const {
    for (const auto& pair : items_) {
        if (pair.second.name == name) {
            return &pair.second;
        }
    }
    return nullptr;
}

std::vector<Item> ItemManager::GetAllItems() const {
    std::vector<Item> result;
    for (const auto& pair : items_) {
        result.push_back(pair.second);
    }
    return result;
}

} // namespace domain 