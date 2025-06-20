#pragma once
#include "Item.h"
#include <unordered_map>
#include <string>
#include <vector>

namespace domain {

class ItemManager {
public:
    void addItem(const Item& item);
    const Item* getItem(const std::string& id) const;
    const Item* findItemByName(const std::string& name) const;
    std::vector<Item> getAllItems() const;
private:
    std::unordered_map<std::string, Item> items;
};

} // namespace domain
