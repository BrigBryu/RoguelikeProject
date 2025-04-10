// monsterParser.cpp

#include "monsterParser.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <cctype>

// Assume that point.hpp is included transitively via npc.hpp.
// Helper function: trim whitespace from both ends of a string.
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

// Helper function: parse a dice expression (e.g., "7+1d4") into a Dice object.
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

// Constructor for MonsterList: reads and parses the monster definition file.
MonsterList::MonsterList(const std::string &filename) {
    std::ifstream file(filename.c_str());
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        std::exit(EXIT_FAILURE);
    }
    
    std::string line;
    // Check file header.
    if (!std::getline(file, line)) {
        std::cerr << "File is empty." << std::endl;
        std::exit(EXIT_FAILURE);
    }
    line = trim(line);
    if (line != "RLG327 MONSTER DESCRIPTION 1") {
        std::cerr << "Invalid file header. Expected \"RLG327 MONSTER DESCRIPTION 1\"." << std::endl;
        std::exit(EXIT_FAILURE);
    }
    
    // Process file lines looking for monster definitions.
    while (std::getline(file, line)) {
        line = trim(line);
        if (line == "BEGIN MONSTER") {
            bool valid = true;
            // Flags for required fields.
            bool foundName = false, foundDesc = false, foundSymb = false, foundColor = false,
                 foundSpeed = false, foundAbil = false, foundHP = false, foundDAM = false, foundRRTY = false;
            
            // Temporary storage for parsed fields.
            std::string mName, mDesc, mColor;
            char mSymb = ' ';
            Dice mSpeed, mHP, mDAM;
            int mRarity = 0;
            std::vector<std::string> mAbil;
            
            // Parse current monster block.
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
                    // Read description lines until a line with only a period.
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
                // Unknown keywords are simply ignored.
            } // end of monster block
            
            // Must have all required fields.
            if (!(foundName && foundDesc && foundSymb && foundColor &&
                  foundSpeed && foundAbil && foundHP && foundDAM && foundRRTY))
                valid = false;
            
            // If the monster definition is valid, construct the Monster.
            if (valid) {
                // For HP we roll the dice immediately to compute the hitpoints value.
                int hpValue = mHP.roll();
                Point* pt = new Point();
                pt->x = 0;
                pt->y = 0;
                // Note: We pass 0 for the attributes; the Monster constructor may set them randomly.
                Monster monster(pt, 0, mSymb, mName, mDesc, mAbil, mColor, mSpeed, hpValue, mDAM, mRarity);
                monsters.push_back(monster);
            }
            // If not valid, discard and continue to the next monster block.
        }
    }
}

// Destructor for MonsterList.
// (For this assignment, cleaning up dynamically allocated Point pointers is optional.)
MonsterList::~MonsterList() {
    // Optionally, if Monster's destructor does not free the memory for Point* cord,
    // you could iterate over monsters and delete the point, but for now we assume it's not needed.
}

// printList() prints each monster's fields in the required order.
// Format: NAME, DESC, SYMB, COLOR, SPEED, ABIL, HP, DAM, RRTY (with a blank line between monsters).
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

// End of monsterParser.cpp