#include "level.h"



LevelLoader::LevelLoader()
{
    currentLevel = 0;
}

LevelLoader::~LevelLoader()
{

}

int LevelLoader::getCurrentLevel(Level &level)
{
    if (currentLevel >= levels.size())
        return 0;
    level = levels[currentLevel++];
    return 1;
}

bool LevelLoader::loadLevels()
{
    ifstream file("level.txt", ifstream::in);
    int level, monsters, obstcales, bulltes, distance;

    while (file >> level)
	{
        if (! (file >> monsters >> obstcales >> bulltes >> distance) )
        {
            cout<<"corrupt file"<<endl;
            return false;
        }
        else
        {
            //printf("monsters: %d bullets: %d distance %d\n", (int)monsters, (int)bulltes, (int) distance);
            Level l;
            l.bulltes = bulltes;
            l.distance = distance;
            l.level = level;
            l.monsters = monsters;
            l.obstcales = obstcales;

            levels.push_back(l);
        }
    }

    return true;
}
