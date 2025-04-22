#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <string>
#include <vector>
#include "../dice.h"
#include "point.hpp"

class Dungeon;

typedef enum object_type {
    objtype_no_type,
    objtype_WEAPON,
    objtype_OFFHAND,
    objtype_RANGED,
    objtype_LIGHT,
    objtype_ARMOR,
    objtype_HELMET,
    objtype_CLOAK,
    objtype_GLOVES,
    objtype_BOOTS,
    objtype_AMULET,
    objtype_RING,
    objtype_SCROLL,
    objtype_BOOK,
    objtype_FLASK,
    objtype_GOLD,
    objtype_AMMUNITION,
    objtype_FOOD,
    objtype_WAND,
    objtype_CONTAINER
} object_type_t;

extern const char object_symbol[];

class Object {
private:
    std::string name;
    std::string description;
    object_type_t type;
    uint32_t color;
    int hit;
    dice damage;
    int dodge;
    int defence;
    int weight;
    int speed;
    int attribute;
    int value;
    Point* position;
    bool isArtifact;
    bool isVisible;

public:
    Object();
    
    Object(const std::string &name,
           const std::string &description,
           const object_type_t type,
           const uint32_t color,
           int hit,
           const dice &damage,
           int dodge,
           int defence,
           int weight,
           int speed,
           int attribute,
           int value,
           Point* position,
           bool isArtifact = false);
    
    ~Object();
    
    inline const std::string &get_name() const { return name; }
    inline const std::string &get_description() const { return description; }
    inline const object_type_t get_type() const { return type; }
    inline const uint32_t get_color() const { return color; }
    inline int get_hit() const { return hit; }
    inline const dice &get_damage() const { return damage; }
    inline int get_dodge() const { return dodge; }
    inline int get_defence() const { return defence; }
    inline int get_weight() const { return weight; }
    inline int get_speed() const { return speed; }
    inline int get_attribute() const { return attribute; }
    inline int get_value() const { return value; }
    inline Point* get_position() const { return position; }
    inline bool get_is_artifact() const { return isArtifact; }
    inline bool get_is_visible() const { return isVisible; }
    
    inline void set_position(Point* pos) { position = pos; }
    inline void set_is_visible(bool visible) { isVisible = visible; }
    
    char get_symbol() const;
    
    void print() const;
};

class ObjectList {
private:
    std::vector<Object*> objects;
    std::vector<bool> artifactsPickedUp;

public:
    ObjectList();
    
    ~ObjectList();
    
    void add_object(Object* object);
    
    void remove_object(int index);
    
    Object* get_object(int index);
    
    int get_count() const;
    
    void generate_objects(Dungeon* dungeon, int count);
    
    void mark_artifact_picked_up(const std::string &name);
    
    bool is_artifact_picked_up(const std::string &name);
    
    void clear();
};

extern ObjectList* globalObjectList;

#endif