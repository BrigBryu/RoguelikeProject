#include "monsterParser.hpp"
#include <../gameObjects/dungeon.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <cctype>
#include <ctime>

std::string trim(const std::string &s) {
    size_t start = 0;
    while (start < s.size() && std::isspace(s[start])) {
        start++;
    }
    size_t end = s.size();
    while (end > start && std::isspace(s[end - 1])) {
        end--;
    }
    return s.substr(start, end - start);
}

bool parseDice(const std::string &str, Dice &d) {
    std::istringstream iss(str);
    int base, numDice, sides;
    char plus, dchar;
    if (!(iss >> base))
        return false;
    if (!(iss >> plus) || plus != '+')
        return false;
    if (!(iss >> numDice))
        return false;
    if (!(iss >> dchar) || dchar != 'd')
        return false;
    if (!(iss >> sides))
        return false;
    d = Dice(base, numDice, sides);
    return true;
}

MonsterList::MonsterList(const std::string &filename) {
    std::ifstream file(filename.c_str());
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        std::exit(EXIT_FAILURE);
    }
    
    std::string line;
    if (!std::getline(file, line)) {
        std::cerr << "File is empty." << std::endl;
        std::exit(EXIT_FAILURE);
    }
    line = trim(line);
    if (line != "RLG327 MONSTER DESCRIPTION 1") {
        std::cerr << "Invalid file header. Expected \"RLG327 MONSTER DESCRIPTION 1\"." << std::endl;
        std::exit(EXIT_FAILURE);
    }
    
    int monsterCount = 0;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line == "BEGIN MONSTER") {
            monsterCount++;
            bool valid = true;
            bool foundName = false, foundDesc = false, foundSymb = false, foundColor = false,
                 foundSpeed = false, foundAbil = false, foundHP = false, foundDAM = false, foundRRTY = false;
            
            std::string mName, mDesc, mColor;
            char mSymb = ' ';
            Dice mSpeed, mHP, mDAM;
            int mRarity = 0;
            std::vector<std::string> mAbil;
            
            while (std::getline(file, line)) {
                line = trim(line);
                if (line == "END")
                    break;
                if (line.empty())
                    continue;
                
                if (line.substr(0, 5) == "NAME ") {
                    if (foundName)
                        valid = false;
                    mName = line.substr(5);
                    foundName = true;
                }
                else if (line.substr(0, 5) == "SYMB ") {
                    if (foundSymb)
                        valid = false;
                    std::string s = line.substr(5);
                    if (s.empty()) {
                        valid = false;
                    } else {
                        mSymb = s[0];
                    }
                    foundSymb = true;
                }
                else if (line.substr(0, 6) == "COLOR ") {
                    if (foundColor)
                        valid = false;
                    mColor = line.substr(6);
                    foundColor = true;
                }
                else if (line == "DESC") {
                    if (foundDesc)
                        valid = false;
                    std::string desc;
                    while (std::getline(file, line)) {
                        if (trim(line) == ".")
                            break;
                        desc += line + "\n";
                    }
                    if (!desc.empty() && desc.back() == '\n')
                        desc.pop_back();
                    mDesc = desc;
                    foundDesc = true;
                }
                else if (line.substr(0, 6) == "SPEED ") {
                    if (foundSpeed)
                        valid = false;
                    std::string diceStr = line.substr(6);
                    if (!parseDice(diceStr, mSpeed))
                        valid = false;
                    foundSpeed = true;
                }
                else if (line.substr(0, 5) == "ABIL ") {
                    if (foundAbil)
                        valid = false;
                    std::string abilLine = line.substr(5);
                    std::istringstream iss(abilLine);
                    std::string token;
                    while (iss >> token) {
                        mAbil.push_back(token);
                    }
                    foundAbil = true;
                }
                else if (line.substr(0, 3) == "HP ") {
                    if (foundHP)
                        valid = false;
                    std::string diceStr = line.substr(3);
                    if (!parseDice(diceStr, mHP))
                        valid = false;
                    foundHP = true;
                }
                else if (line.substr(0, 4) == "DAM ") {
                    if (foundDAM)
                        valid = false;
                    std::string diceStr = line.substr(4);
                    if (!parseDice(diceStr, mDAM))
                        valid = false;
                    foundDAM = true;
                }
                else if (line.substr(0, 5) == "RRTY ") {
                    if (foundRRTY)
                        valid = false;
                    std::string rarityStr = line.substr(5);
                    try {
                        mRarity = std::stoi(rarityStr);
                    } catch (...) {
                        valid = false;
                    }
                    foundRRTY = true;
                }
            } 
            
            if (!(foundName && foundDesc && foundSymb && foundColor &&
                  foundSpeed && foundAbil && foundHP && foundDAM && foundRRTY))
                valid = false;
            
            if (valid) {
                int hpValue = mHP.roll();
                
                Point* pt = new Point();
                pt->x = 0;
                pt->y = 0;
                
                Monster monster(pt, 0, mSymb, mName, mDesc, mAbil, mColor, mSpeed, hpValue, mDAM, mRarity);
                
                monsters.push_back(monster);
            }
        }
    }
    
    uniquesUsed.resize(monsters.size(), false);
}

MonsterList::~MonsterList() {

}

// Format: NAME, DESC, SYMB, COLOR, SPEED, ABIL, HP, DAM, RRTY
void MonsterList::printList() const {
    for (size_t i = 0; i < monsters.size(); ++i) {
        const Monster &m = monsters[i];
        std::cout << m.name << std::endl;
        std::cout << m.description << std::endl;
        std::cout << m.texture << std::endl;
        std::cout << m.color << std::endl;
        std::cout << m.speed.base << "+" << m.speed.numDice << "d" << m.speed.sides << std::endl;
        // Print abilities as space-separated tokens.
        for (size_t j = 0; j < m.abilityNames.size(); ++j) {
            std::cout << m.abilityNames[j];
            if (j + 1 < m.abilityNames.size())
                std::cout << " ";
        }
        std::cout << std::endl;
        std::cout << m.hitpoints << std::endl;
        std::cout << m.damage.base << "+" << m.damage.numDice << "d" << m.damage.sides << std::endl;
        std::cout << m.rarity << std::endl;
        std::cout << std::endl;
    }
}

Monster* MonsterList::generateMonster(Dungeon* dungeon) {
    if (monsters.empty()) {
        return nullptr;
    }
    
    for (int attempt = 0; attempt < 100; attempt++) {
        size_t index = rand() % monsters.size();
        const Monster& desc = monsters[index];
        
        if (desc.name.find("UNIQUE") != std::string::npos && uniquesUsed[index]) {
            continue; // Skip this unique monster, it's already been used
        }
        
        int rarityCheck = rand() % 100;
        if (rarityCheck < desc.rarity) {
            continue; // Failed rarity check, try another monster
        }
        
        if (dungeon->numRooms <= 0) {
            return nullptr;
        }
        
        int roomIndex = rand() % dungeon->numRooms;
        Rectangle room = dungeon->rooms[roomIndex];
        int x = room.bottomLeft.x + (rand() % room.width);
        int y = room.bottomLeft.y + (rand() % room.height);
        
        Point* position = new Point();
        position->x = x;
        position->y = y;
        
        // Create the monster instance
        int hitpoints = desc.hitpoints;
        
        Monster* monster = new Monster(
            position,
            desc.attributes,
            desc.texture,
            desc.name,
            desc.description,
            desc.abilityNames,
            desc.color,
            desc.speed,
            hitpoints,
            desc.damage,  // damage remains as dice
            desc.rarity
        );
        
        // Mark unique monster as used
        if (desc.name.find("UNIQUE") != std::string::npos) {
            uniquesUsed[index] = true;
        }
        
        return monster;
    }
    
    return nullptr;
}

void MonsterList::spawnMonstersInDungeon(Dungeon* dungeon, int numMonsters) {
    for (int i = 0; i < dungeon->numMonsters; i++) {
        delete dungeon->monsters[i]->cord; // Delete the Point
        delete dungeon->monsters[i];       // Delete the Monster
    }
    dungeon->numMonsters = 0;
    
    for (int attempt = 0; attempt < numMonsters * 5 && dungeon->numMonsters < numMonsters; attempt++) {
        Monster* monster = generateMonster(dungeon);
        if (monster != nullptr) {
            dungeon->monsters[dungeon->numMonsters++] = monster;
        }
    }
}