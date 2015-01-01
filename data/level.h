#ifndef LEVEL_H
#define LEVEL_H

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

struct Level {
int level, monsters, obstcales, bulltes, distance;
Level& operator=(const Level &level)
{
    this->level = level.level;
    this->monsters = level.monsters;
    this->obstcales=level.obstcales;
    this->bulltes=level.bulltes;
    this->distance = level.distance;

    return *this;
}
};

class LevelLoader{

private:
    vector<Level> levels;
    int currentLevel;

public:
    LevelLoader();
    ~LevelLoader();

    int getCurrentLevel(Level &level);
    bool loadLevels();
};

#endif
