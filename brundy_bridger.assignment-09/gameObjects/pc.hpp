#ifndef PC_HPP
#define PC_HPP

#include <string>
#include "point.hpp"
#include "../dice.h"
#include "object.hpp"

// Equipment slot identifiers (a-l)
#define SLOT_WEAPON 'a'
#define SLOT_OFFHAND 'b'
#define SLOT_RANGED 'c'
#define SLOT_ARMOR 'd'
#define SLOT_HELMET 'e'
#define SLOT_CLOAK 'f'
#define SLOT_GLOVES 'g'
#define SLOT_BOOTS 'h'
#define SLOT_AMULET 'i'
#define SLOT_LIGHT 'j'
#define SLOT_RING1 'k'
#define SLOT_RING2 'l'

// Number of carry slots
#define NUM_CARRY_SLOTS 10

class PC {
public:
    int base_hitpoints;  // Base value without equipment
    int base_speed;      // Base value without equipment
    dice base_damage;    // Base value without equipment
    
    // Calculated values including equipment bonuses
    int hitpoints;
    int speed;
    dice damage;
    
    // Equipment slots
    Object* equipment[12]; // indices 0-11 for slots a-l
    
    // Carry slots (inventory)
    Object* inventory[NUM_CARRY_SLOTS]; // indices 0-9

    // Constructor
    PC();
    ~PC();
    
    // Equip an item to a specific slot
    bool equipItem(char slot, Object* item);
    
    // Add an item to inventory if there's space
    bool addToInventory(Object* item);
    
    // Check if inventory has space
    bool hasInventorySpace() const;
    
    // Get the first available inventory slot, returns -1 if full
    int getFirstEmptyInventorySlot() const;
    
    // Calculate and apply equipment bonuses
    void recalculateStats();
    
    // Validate that the item type matches the equipment slot
    bool validateItemForSlot(object_type_t type, char slot) const;
    
    // Convert slot name to appropriate equipment type
    object_type_t slotToObjectType(char slot) const;
};

#endif 