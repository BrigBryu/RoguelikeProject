#ifndef MONSTERPARSER_HPP
#define MONSTERPARSER_HPP

#include <string>
#include <fstream>
#include <vector>
#include <../gameObjects/dice.hpp>
#include <../gameObjects/npc.hpp>

class MonsterList {
    public:
        MonsterList(const std::string& filename);
        ~MonsterList();
        void printList() const;
    private:
        std::vector<Monster> monsters;
};

#endif