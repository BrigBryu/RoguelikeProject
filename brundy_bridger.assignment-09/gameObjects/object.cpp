#include "object.hpp"
#include "dungeon.hpp"
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <ncurses.h>

const char object_symbol[] = {
    '*', 
    '|', 
    ')', 
    '}', 
    '~', 
    '[', 
    ']', 
    '(', 
    '{', 
    '\\', 
    '"', 
    '=', 
    '`', 
    '?', 
    '!', 
    '$', 
    '/', 
    ',', 
    '-', 
    '%', 
};

extern ObjectList* globalObjectList;

Object::Object() {
    name = "Default Object";
    description = "A default object";
    type = objtype_no_type;
    color = COLOR_WHITE;
    hit = 0;
    damage = dice(0, 0, 0);
    dodge = 0;
    defence = 0;
    weight = 0;
    speed = 0;
    attribute = 0;
    value = 0;
    position = nullptr;
    isArtifact = false;
    isVisible = true;
}

Object::Object(const std::string &name,
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
               bool isArtifact) {
    this->name = name;
    this->description = description;
    this->type = type;
    this->color = color;
    this->hit = hit;
    this->damage = damage;
    this->dodge = dodge;
    this->defence = defence;
    this->weight = weight;
    this->speed = speed;
    this->attribute = attribute;
    this->value = value;
    this->position = position;
    this->isArtifact = isArtifact;
    this->isVisible = true;
}

Object::~Object() {
    if (position) {
        delete position;
        position = nullptr;
    }
}

char Object::get_symbol() const {
    return object_symbol[type];
}

void Object::print() const {
    std::cout << "Object: " << name << std::endl;
    std::cout << "Description: " << description << std::endl;
    std::cout << "Symbol: " << get_symbol() << std::endl;
    std::cout << "Position: (" << position->x << "," << position->y << ")" << std::endl;
    std::cout << "Hit: " << hit << std::endl;
    std::cout << "Damage: " << damage.get_base() << "+" << damage.get_number() << "d" << damage.get_sides() << std::endl;
    std::cout << "Artifact: " << (isArtifact ? "Yes" : "No") << std::endl;
}

ObjectList::ObjectList() {
}

ObjectList::~ObjectList() {
    for (Object* obj : objects) {
        delete obj;
    }
    objects.clear();
    artifactsPickedUp.clear();
}

void ObjectList::add_object(Object* object) {
    objects.push_back(object);
}

void ObjectList::remove_object(int index) {
    if (index >= 0 && index < (int)objects.size()) {
        if (objects[index]->get_is_artifact()) {
            mark_artifact_picked_up(objects[index]->get_name());
        }
        
        delete objects[index];
        objects.erase(objects.begin() + index);
    }
}

Object* ObjectList::get_object(int index) {
    if (index >= 0 && index < (int)objects.size()) {
        return objects[index];
    }
    return nullptr;
}

int ObjectList::get_count() const {
    return objects.size();
}

void ObjectList::mark_artifact_picked_up(const std::string &name) {
    for (size_t i = 0; i < artifactsPickedUp.size(); i++) {
        if (objects[i]->get_name() == name) {
            artifactsPickedUp[i] = true;
            return;
        }
    }
    
    artifactsPickedUp.push_back(true);
}

bool ObjectList::is_artifact_picked_up(const std::string &name) {
    for (size_t i = 0; i < artifactsPickedUp.size(); i++) {
        if (objects[i]->get_name() == name && artifactsPickedUp[i]) {
            return true;
        }
    }
    return false;
}

void ObjectList::clear() {
    objects.clear();
}

void ObjectList::generate_objects(Dungeon* dungeon, int count) {
    // Clear existing objects
    for (size_t i = 0; i < objects.size(); i++) {
        delete objects[i];
    }
    objects.clear();
    
    // Check if descriptions
    bool useDescriptions = !dungeon->object_descriptions.empty();
    
    for (int attempt = 0; attempt < count * 5 && (int)objects.size() < count; attempt++) {
        if (dungeon->numRooms <= 0) {
            return;
        }
        
        int roomIndex = rand() % dungeon->numRooms;
        Rectangle room = dungeon->rooms[roomIndex];
        int x = room.bottomLeft.x + (rand() % room.width);
        int y = room.bottomLeft.y + (rand() % room.height);
        
        Point* position = new Point();
        position->x = x;
        position->y = y;
        
        bool isArtifact = false;
        std::string name, description;
        object_type_t type;
        uint32_t color;
        int hit, dodge, defence, weight, speed, attribute, value;
        dice damage(0, 1, 1);
        
        if (useDescriptions) {
            int descIndex = rand() % dungeon->object_descriptions.size();
            const object_description& desc = dungeon->object_descriptions[descIndex];
            
            name = desc.get_name();
            description = desc.get_description();
            
            type = desc.get_type();
            
            color = desc.get_color();
            
            const dice& hitDice = desc.get_hit();
            hit = hitDice.get_base();
            for (int i = 0; i < hitDice.get_number(); i++) {
                hit += (rand() % hitDice.get_sides()) + 1;
            }
            
            damage = desc.get_damage();
            
            const dice& dodgeDice = desc.get_dodge();
            dodge = dodgeDice.get_base();
            for (int i = 0; i < dodgeDice.get_number(); i++) {
                dodge += (rand() % dodgeDice.get_sides()) + 1;
            }
            
            const dice& defenceDice = desc.get_defence();
            defence = defenceDice.get_base();
            for (int i = 0; i < defenceDice.get_number(); i++) {
                defence += (rand() % defenceDice.get_sides()) + 1;
            }
            
            const dice& weightDice = desc.get_weight();
            weight = weightDice.get_base();
            for (int i = 0; i < weightDice.get_number(); i++) {
                weight += (rand() % weightDice.get_sides()) + 1;
            }
            
            const dice& speedDice = desc.get_speed();
            speed = speedDice.get_base();
            for (int i = 0; i < speedDice.get_number(); i++) {
                speed += (rand() % speedDice.get_sides()) + 1;
            }
            
            const dice& attrDice = desc.get_attribute();
            attribute = attrDice.get_base();
            for (int i = 0; i < attrDice.get_number(); i++) {
                attribute += (rand() % attrDice.get_sides()) + 1;
            }
            
            const dice& valueDice = desc.get_value();
            value = valueDice.get_base();
            for (int i = 0; i < valueDice.get_number(); i++) {
                value += (rand() % valueDice.get_sides()) + 1;
            }
            
            std::string lowerName = name;
            for (size_t i = 0; i < lowerName.length(); i++) {
                lowerName[i] = tolower(lowerName[i]);
            }
            isArtifact = (lowerName.find("artifact") != std::string::npos);
        } else {
            type = (object_type_t)((rand() % 19) + 1);
            
            uint32_t colors[] = {COLOR_RED, COLOR_GREEN, COLOR_YELLOW, COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN, COLOR_WHITE};
            color = colors[rand() % 7];
            
            hit = rand() % 10;
            damage = dice(rand() % 5, 1 + rand() % 3, 4 + rand() % 8);
            dodge = rand() % 10;
            defence = rand() % 10;
            weight = rand() % 20;
            speed = rand() % 10;
            attribute = rand() % 10;
            value = rand() % 100;
            
            isArtifact = (rand() % 10 == 0);
            
            name = (isArtifact ? "Artifact " : "") + std::string("Object ") + std::to_string(objects.size() + 1);
            description = std::string("This is ") + (isArtifact ? "a unique artifact" : "a common object");
        }
        
        if (isArtifact && is_artifact_picked_up(name)) {
            delete position;
            position = nullptr;
            continue;
        }
        
        if (rand() % 100 < 30) {
            delete position;
            position = nullptr;
            continue;
        }
        
        Object* object = new Object(
            name,
            description,
            type,
            color,
            hit,
            damage,
            dodge,
            defence,
            weight,
            speed,
            attribute,
            value,
            position,
            isArtifact
        );
        
        add_object(object);
    }
}