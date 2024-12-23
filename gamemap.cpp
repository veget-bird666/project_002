#include "gamemap.h"

// 宝石类的实现
int Gemstone::GetType() {
    return stone_type;
}



Gemstone::Gemstone(int type) : bomb_life(30),stone_type(type) {}

Gemstone::Gemstone() : Gemstone(std::rand() % 6) {}


// 格子类的实现
Space::Space() : gemstone(nullptr), space_type(0) {}
Space::Space(int type):space_type(type){}
Space::~Space() {
    delete gemstone;
}

void Space::SetGemstone(Gemstone* g) {
    gemstone = g;
}

Gemstone* Space::GetGemstone() {
    return gemstone;
}

int Space::GetType() {
    return space_type;
}

void Space::SetType(int type){
    space_type = type;
}

// 地图类的实现
GameMap::GameMap(int n, int m) {
    row = n;
    col = m;
    mapInitial();
    clearMap();
}

GameMap::GameMap(int n) {
    row = col = n;
    mapInitial();
    clearMap();
}

GameMap::~GameMap() {
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            delete spaces[i][j];
        }
    }
}

void GameMap::printMap() {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            Gemstone* g = spaces[i][j]->GetGemstone();
            if (g == nullptr)
                std::cout << "*" << "  ";
            else
                std::cout << " " << (char)('A' + g->GetType()) << "  ";
        }
        std::cout << std::endl << std::endl;
    }
}

int GameMap::getRow() {
    return row;
}

int GameMap::getCol() {
    return col;
}

void GameMap::SwapStone(int i1, int j1, int i2, int j2) {
    // 检查边界和邻接情况
    if (i1 < 0 || i1 >= row || j1 < 0 || j1 >= col ||
        i2 < 0 || i2 >= row || j2 < 0 || j2 >= col)
        return;

    // 两个格子必须相邻
    if (std::abs(i1 - i2) + std::abs(j1 - j2) != 1)
        return;

    std::cout << "进行交换" << std::endl;
    Gemstone* g1 = spaces[i1][j1]->GetGemstone();
    Gemstone* g2 = spaces[i2][j2]->GetGemstone();
    spaces[i1][j1]->SetGemstone(g2);
    spaces[i2][j2]->SetGemstone(g1);

    if (checkOne(i1, j1) || checkOne(i2, j2)) {
        std::cout << "交换后存在可消除部分" << std::endl;
        performBomb();
        goDown();
        flush();
        clearMap();
    } else {
        std::cout << "交换后不存在可消除部分" << std::endl;
        spaces[i1][j1]->SetGemstone(g1);
        spaces[i2][j2]->SetGemstone(g2);
    }
}

// 简单交换位置
void GameMap::simpleSwap(int i1, int j1, int i2, int j2){
    Gemstone* g1 = spaces[i1][j1]->GetGemstone();
    Gemstone* g2 = spaces[i2][j2]->GetGemstone();
    spaces[i1][j1]->SetGemstone(g2);
    spaces[i2][j2]->SetGemstone(g1);
}

void GameMap::performBomb() {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            if (toBomb[i][j]) {
                delete spaces[i][j]->GetGemstone();
                spaces[i][j]->SetGemstone(nullptr);
                toBomb[i][j] = false;
            }
        }
    }
}

void GameMap::goDown() {
    for (int i = row - 1; i >= 0; i--) {
        for (int j = col - 1; j >= 0; j--) {
            if (spaces[i][j]->GetGemstone() == nullptr) {
                int temp_i = i - 1;
                while (temp_i >= 0) {
                    if (spaces[temp_i][j]->GetGemstone() != nullptr) {
                        Gemstone* g = spaces[temp_i][j]->GetGemstone();
                        spaces[i][j]->SetGemstone(g);
                        spaces[temp_i][j]->SetGemstone(nullptr);
                        break;
                    }
                    temp_i--;
                }
            }
        }
    }
}

void GameMap::flush() {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            if (spaces[i][j]->GetGemstone() == nullptr) {
                Gemstone* g = new Gemstone();
                spaces[i][j]->SetGemstone(g);
            }
        }
    }
}


bool GameMap::checkMap() {
    bool couldBomb = false;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            if (spaces[i][j]->GetGemstone() == nullptr) continue;

            if (j < col - 2 &&
                spaces[i][j]->GetGemstone()->GetType() == spaces[i][j + 1]->GetGemstone()->GetType() &&
                spaces[i][j + 1]->GetGemstone()->GetType() == spaces[i][j + 2]->GetGemstone()->GetType()) {

                couldBomb = true;
                toBomb[i][j] = true;
                toBomb[i][j + 1] = true;
                toBomb[i][j + 2] = true;
            }

            if (i < row - 2 &&
                spaces[i][j]->GetGemstone()->GetType() == spaces[i + 1][j]->GetGemstone()->GetType() &&
                spaces[i + 1][j]->GetGemstone()->GetType() == spaces[i + 2][j]->GetGemstone()->GetType()) {

                couldBomb = true;
                toBomb[i][j] = true;
                toBomb[i + 1][j] = true;
                toBomb[i + 2][j] = true;
            }
        }
    }
    return couldBomb;
}

bool GameMap::checkOne(int i, int j) {
    if (spaces[i][j]->GetGemstone() == nullptr) return false;

    bool couldBomb = false;
    int TargetType = spaces[i][j]->GetGemstone()->GetType();
    for (int di = -2; di <= 2; di++) {
        for (int dj = -2; dj <= 2; dj++) {
            int newI = i + di;
            int newJ = j + dj;

            if (newI >= 0 && newI < row && newJ >= 0 && newJ < col) {
                if (dj <= 0 && newJ + 2 < col &&
                    spaces[newI][newJ]->GetGemstone()->GetType() == TargetType &&
                    spaces[newI][newJ + 1]->GetGemstone()->GetType() == TargetType &&
                    spaces[newI][newJ + 2]->GetGemstone()->GetType() == TargetType) {

                    couldBomb = true;
                    toBomb[newI][newJ] = true;
                    toBomb[newI][newJ + 1] = true;
                    toBomb[newI][newJ + 2] = true;
                }

                if (di <= 0 && newI + 2 < row &&
                    spaces[newI][newJ]->GetGemstone()->GetType() == TargetType &&
                    spaces[newI + 1][newJ]->GetGemstone()->GetType() == TargetType &&
                    spaces[newI + 2][newJ]->GetGemstone()->GetType() == TargetType) {

                    couldBomb = true;
                    toBomb[newI][newJ] = true;
                    toBomb[newI + 1][newJ] = true;
                    toBomb[newI + 2][newJ] = true;
                }
            }
        }
    }
    return couldBomb;
}

void GameMap::mapInitial() {
    toBomb.assign(row, std::vector<bool>(col, false));
    spaces.resize(row, std::vector<Space*>(col));

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            spaces[i][j] = new Space();
            Gemstone* gemstone = new Gemstone();
            spaces[i][j]->SetGemstone(gemstone);
        }
    }
}

// 清除地图至不可消除
void GameMap::clearMap() {

    while(checkMap()){
        performBomb();
        goDown();
        flush();
    }

}
