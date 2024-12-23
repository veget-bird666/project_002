#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>


// 宝石类
class Gemstone {
public:

    int bomb_life;
    int GetType();
    Gemstone(int type);
    Gemstone();

private:

    int x;
    int y;
    int speed;
    int stone_type;
};

// 格子类
class Space {
public:
    Space();
    Space(int );
    ~Space();
    void SetGemstone(Gemstone* g);
    Gemstone* GetGemstone();
    void SetType(int );
    int GetType();

private:
    Gemstone* gemstone;
    int space_type;
};

// 地图类
class GameMap {
public:

    std::vector<std::vector<Space*>> spaces;

    GameMap(int n, int m);
    GameMap(int n);
    ~GameMap();
    void printMap();
    int getRow();
    int getCol();
    void SwapStone(int i1, int j1, int i2, int j2);
    void simpleSwap(int i1, int j1, int i2, int j2);
    void performBomb();
    void goDown();
    void clearMap();
    void flush();
    bool checkMap();
    bool checkOne(int i, int j);


private:
    void mapInitial();
    std::vector<std::vector<bool>> toBomb;
    int row;
    int col;

};

#endif // GAMEMAP_H
