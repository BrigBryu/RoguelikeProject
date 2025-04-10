#include "monsterParser.hpp"
#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
    const char *homeDir = std::getenv("HOME");
    if (!homeDir) {
        std::cerr << "HOME environment variable not set." << std::endl;
        return EXIT_FAILURE;
    }
    std::string filePath = std::string(homeDir) + "/.rlg327/monster_desc.txt";
    MonsterList monsterList(filePath);
    monsterList.printList();
    return 0;
}