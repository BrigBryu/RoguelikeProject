#include "pc.hpp"
#include <cstring>

PC::PC() {
    // Set default base values without equipment
    base_hitpoints = 50;
    base_speed = 10;
    base_damage = dice(0, 1, 4);
    
    // Set initial calculated values
    hitpoints = base_hitpoints;
    speed = base_speed;
    damage = base_damage;
    
    // Initialize equipment slots to nullptr
    for (int i = 0; i < 12; i++) {
        equipment[i] = nullptr;
    }
    
    // Initialize inventory slots to nullptr
    for (int i = 0; i < NUM_CARRY_SLOTS; i++) {
        inventory[i] = nullptr;
    }
}

PC::~PC() {
    // Note: We don't delete the objects as they are managed by the Dungeon class
    // This just sets pointers to null
    for (int i = 0; i < 12; i++) {
        equipment[i] = nullptr;
    }
    
    for (int i = 0; i < NUM_CARRY_SLOTS; i++) {
        inventory[i] = nullptr;
    }
}

bool PC::equipItem(char slot, Object* item) {
    if (slot < SLOT_WEAPON || slot > SLOT_RING2) {
        return false;
    }
    
    // Validate that the item type is appropriate for the slot
    if (!validateItemForSlot(item->get_type(), slot)) {
        return false;
    }
    
    int index = slot - 'a';
    equipment[index] = item;
    
    // Recalculate stats after equipping
    recalculateStats();
    
    return true;
}

bool PC::addToInventory(Object* item) {
    int slot = getFirstEmptyInventorySlot();
    if (slot == -1) {
        return false;
    }
    
    inventory[slot] = item;
    return true;
}

bool PC::hasInventorySpace() const {
    return getFirstEmptyInventorySlot() != -1;
}

int PC::getFirstEmptyInventorySlot() const {
    for (int i = 0; i < NUM_CARRY_SLOTS; i++) {
        if (inventory[i] == nullptr) {
            return i;
        }
    }
    return -1;
}

void PC::recalculateStats() {
    // Reset to base values
    hitpoints = base_hitpoints;
    speed = base_speed;
    damage = base_damage;
    
    // Apply equipment bonuses
    for (int i = 0; i < 12; i++) {
        if (equipment[i]) {
            // Add speed bonus
            speed += equipment[i]->get_speed();
            
            // Add damage for weapons (for now all equipment can add damage)
            // in a real game we'd have more complex rules about what items add damage
            if (equipment[i]->get_damage().get_base() > 0 || 
                equipment[i]->get_damage().get_number() > 0) {
                // We're adding the dice "additively" - base + dice
                damage.set_base(damage.get_base() + equipment[i]->get_damage().get_base());
                damage.set_number(damage.get_number() + equipment[i]->get_damage().get_number());
                // Sides should be the max of all dice involved
                if (equipment[i]->get_damage().get_sides() > damage.get_sides()) {
                    damage.set_sides(equipment[i]->get_damage().get_sides());
                }
            }
        }
    }
}

bool PC::validateItemForSlot(object_type_t type, char slot) const {
    // Convert slot to expected type
    object_type_t expected = slotToObjectType(slot);
    
    // Special case for rings, which can go in either ring slot
    if ((slot == SLOT_RING1 || slot == SLOT_RING2) && type == objtype_RING) {
        return true;
    }
    
    return type == expected;
}

object_type_t PC::slotToObjectType(char slot) const {
    switch (slot) {
        case SLOT_WEAPON:  return objtype_WEAPON;
        case SLOT_OFFHAND: return objtype_OFFHAND;
        case SLOT_RANGED:  return objtype_RANGED;
        case SLOT_ARMOR:   return objtype_ARMOR;
        case SLOT_HELMET:  return objtype_HELMET;
        case SLOT_CLOAK:   return objtype_CLOAK;
        case SLOT_GLOVES:  return objtype_GLOVES;
        case SLOT_BOOTS:   return objtype_BOOTS;
        case SLOT_AMULET:  return objtype_AMULET;
        case SLOT_LIGHT:   return objtype_LIGHT;
        case SLOT_RING1:   return objtype_RING;
        case SLOT_RING2:   return objtype_RING;
        default:           return objtype_no_type;
    }
} 