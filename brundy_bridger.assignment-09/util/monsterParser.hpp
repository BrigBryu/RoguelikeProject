#ifndef MONSTERPARSER_HPP
#define MONSTERPARSER_HPP

#include <string>
#include <fstream>
#include <vector>
#include <../gameObjects/dice.hpp>
#include <../gameObjects/npc.hpp>

// Forward declaration of Dungeon class instead of including the header
class Dungeon;

class MonsterList {
    public:
        MonsterList(const std::string& filename);
        ~MonsterList();
        void printList() const;
        
        // New methods for spawning monsters
        Monster* generateMonster(Dungeon* dungeon);
        void spawnMonstersInDungeon(Dungeon* dungeon, int numMonsters);
        
        // Method to get a monster description by index
        const Monster& getMonster(size_t index) const { return monsters[index]; }
        
        // Method to get the number of available monster descriptions
        size_t getMonsterCount() const { return monsters.size(); }
        
    private:
        std::vector<Monster> monsters;
        std::vector<bool> uniquesUsed; // Track which unique monsters have been used
};

#endif