#ifndef DOMAIN_ITEM_MANAGER_H_
#define DOMAIN_ITEM_MANAGER_H_

#include "Item.h"
#include <unordered_map>
#include <string>
#include <vector>

namespace domain {

class ItemManager {
public:
    void AddItem(const Item& item);
    const Item* GetItem(const std::string& id) const;
    const Item* FindItemByName(const std::string& name) const;
    std::vector<Item> GetAllItems() const;
private:
    std::unordered_map<std::string, Item> items_;
};

} 

#endif  
