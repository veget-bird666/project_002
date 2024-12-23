#include "widget.h"
#include "ui_widget.h"
#include <QPainter> // 添加此行
#include <QTimer>
#include "gamemap.h"
#include <vector>
#include <QMouseEvent>
#include <utility>  // 含pair
#include <QDebug>

/*
 * 新加内容：
 * 删除swapStone里的判断代码
 * 在paintEvent开头加入逻辑判断代码
 * 在paintEvent开头加入背景绘制代码
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */

// 地图格子
std::vector<std::vector<Space*>> spaces;

// 爆炸位置
std::vector<std::vector<bool>> toBomb;

// 消融位置
std::vector<std::vector<bool>> toMelt;

// 画板距离地图边界的距离
const int i_off = 80;
const int j_off = 50;

//方块大小
const int block_size = 50;

// 地图大小
const int row = 8;
const int col = 8;

// 重绘时间间隔
const int duration = 16;

// 选中方块坐标
std::vector<std::pair<int, int>> selected_points;

// 有关地图状态的参数
int score = 0;
bool map_swaping = false;
bool map_falling = false;
bool map_bombing = false;

// 有关道具及鼠标选择的布尔值、范围
bool prop_bomb = false;
bool prop_refresh = false;
bool prop_tip = false;
bool prop_hammer = false;

int bomb_num = 3;
int refresh_num = 3;
int tip_num = 3;
int hammer_num = 3;



// 有关地图相关操作的函数
void mapInitial();
void SwapStone(int i1, int j1, int i2, int j2);
void performBomb();
void goDown();
void flush();
bool checkMap();
bool checkOne(int i, int j);
void clearMap();
void performMelt();


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , timer(new QTimer(this))
{

    // 加载图片
    picInitial();

    ui->setupUi(this);

    // 地图初始化
    toBomb.assign(row, std::vector<bool>(col, false));
    toMelt.assign(row, std::vector<bool>(col, false));
    mapInitial();


    // 连接 QTimer 的 timeout 信号到槽函数
    connect(timer, &QTimer::timeout, this, &Widget::updateWidget);

    // 启动定时器，每隔一段时间重绘一次
    timer->start(duration);
}


Widget::~Widget()
{
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            delete spaces[i][j];
        }
    }

    delete ui;
}


// 实现paintEvent方法
void Widget::paintEvent(QPaintEvent *event)
{



    QPainter painter(this);

    checkMap();

    performBomb();
    performMelt();

    goDown();
    flush();

    // 绘制格子背景
    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            QColor color(30+i*18,160 , 30+j*18, 250); // 设置格子背景渐变颜色
            painter.fillRect(j_off + j*block_size,i_off + i*block_size,block_size,block_size,color);
        }
    }


    // 绘制宝石及地形
    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            int space_type = spaces[i][j]->GetType();
            int type = spaces[i][j]->GetGemstone()->GetType();
            if(toBomb[i][j] == false){
                painter.drawPixmap(j_off+j*block_size,i_off+i*block_size,pic_fruits[type]);


            } else{
                Gemstone *g = spaces[i][j]->GetGemstone();
                if(g->bomb_life >= 20){
                    painter.drawPixmap(j_off+j*block_size,i_off+i*block_size,pic_destroy1);
                }
                if(g->bomb_life >= 10){
                    painter.drawPixmap(j_off+j*block_size,i_off+i*block_size,pic_destroy2);
                }
                if(g->bomb_life >= 0){
                    painter.drawPixmap(j_off+j*block_size,i_off+i*block_size,pic_destroy3);
                }
            }
            if(space_type > 0){
                painter.setOpacity(0.62);
                painter.drawPixmap(j_off+j*block_size,i_off+i*block_size,pic_ices[space_type-1]);
                painter.setOpacity(1.0);
            }
        }
    }

    // 刷新得分
    ui->scoreLabel->setText(QString::number(score));

    // 绘制道具
    painter.drawPixmap(50, 490, pic_bomb.scaled( 50, 50, Qt::KeepAspectRatio));
    painter.drawPixmap(140, 490, pic_refresh.scaled( 50, 50, Qt::KeepAspectRatio));
    painter.drawPixmap(230, 490, pic_hammer.scaled( 50, 50, Qt::KeepAspectRatio));
    painter.drawPixmap(320, 490, pic_tip.scaled( 50, 50, Qt::KeepAspectRatio));

    if(!selected_points.empty()){

        // 创建颜色对象，设置透明度
        QColor color(100, 100, 100, 150); // 150：透明度为约 59%
        int i = selected_points[0].first;
        int j = selected_points[0].second;
        ui->label_2->setText(QString::number(i)+","+QString::number(j));
        painter.fillRect(j_off + j*block_size,i_off + i*block_size,block_size,block_size,color);
    }

    timer->start(duration);

}


// 实现mousePressEvent方法
void Widget::mousePressEvent(QMouseEvent *event)
{
    int click_x = event->pos().x(); // 获取 x 坐标
    int click_y = event->pos().y(); // 获取 y 坐标

    ui->label->setText(QString::number(click_x)+","+QString::number(click_y));

    int click_i = (click_y-i_off)/block_size; // 获取当前i
    int click_j = (click_x-j_off)/block_size; // 获取当前j

    // 判断两次选择是否重复
    bool is_repeat = !selected_points.empty() && click_i == selected_points[0].first
                     && click_j == selected_points[0].second;

    // 判断是否已选择道具
    bool is_select_prop = prop_bomb || prop_refresh || prop_hammer || prop_tip;

    // 判断是否点击了不可交换地形
    bool is_select_terrain = false;
    if(click_i>=0 && click_i<row && click_j>=0 && click_j<col)
        is_select_terrain = (spaces[click_i][click_j]->GetType() != 0);

    if(click_i>=0 && click_i<row && click_j>=0 && click_j<col
        && !is_repeat && !is_select_prop && !is_select_terrain){   // 判断i,j是否合法及其他合法情况
        selected_points.push_back(std::make_pair(click_i,click_j));

        // 若已经选择两个方块
        if(selected_points.size() == 2){
            int i1 = selected_points[0].first;
            int j1 = selected_points[0].second;
            int i2 = selected_points[1].first;
            int j2 = selected_points[1].second;
            SwapStone(i1,j1,i2,j2);
            selected_points.clear();
        }
    }

    // 若选择炸弹
    if(prop_bomb){

        if(click_i>=0 && click_i<row && click_j>=0 && click_j<col){
            toBomb[click_i][click_j] = true;

            if(click_j-1>=0)
                toBomb[click_i][click_j-1] = true;
            if(click_j+1<col)
                toBomb[click_i][click_j+1] = true;

            if(click_i-1>=0){
                toBomb[click_i-1][click_j] = true;
                if(click_j-1>=0)
                    toBomb[click_i-1][click_j-1] = true;
                if(click_j+1<col)
                    toBomb[click_i-1][click_j+1] = true;
            }

            if(click_i+1<row){
                toBomb[click_i+1][click_j] = true;
                if(click_j-1>=0)
                    toBomb[click_i+1][click_j-1] = true;
                if(click_j+1<col)
                    toBomb[click_i+1][click_j+1] = true;
            }

            prop_bomb = false;
        }
    }

    // 若选择刷新
    if(prop_refresh && click_i>=0 && click_i<row && click_j>=0 && click_j<col){
        for(int i=0; i<row;i++){
            for(int j=0;j<col;j++){
                Gemstone *g = spaces[i][j]->GetGemstone();
                spaces[i][j]->SetGemstone(nullptr);
                delete g;
            }
        }
        prop_refresh = false;
    }


    // 若选择锤子
    if(prop_hammer && click_i>=0 && click_i<row && click_j>=0 && click_j<col){
        int type = spaces[click_i][click_j]->GetGemstone()->GetType();
        for(int i=0;i<row;i++){
            for(int j=0;j<col;j++){
                Gemstone *g = spaces[i][j]->GetGemstone();
                if(type == g->GetType()){
                    toBomb[i][j] = true;
                }
            }
        }
        prop_hammer = false;
    }


    // 判断是否点击了道具
    if(click_y >= 490 && click_y <= 490+50){
        // 炸弹道具区
        if(click_x >= 50 && click_x <= 50+50){
            selected_points.clear();
            prop_bomb = true;
            prop_hammer = false;
            prop_refresh = false;
            prop_tip = false;
        }
        // 刷新道具区
        if(click_x >= 140 && click_x <= 140+50){
            selected_points.clear();
            prop_bomb = false;
            prop_hammer = false;
            prop_refresh = true;
            prop_tip = false;
        }
        // 锤子道具区
        if(click_x >= 230 && click_x <= 230+50){
            selected_points.clear();
            prop_bomb = false;
            prop_hammer = true;
            prop_refresh = false;
            prop_tip = false;
        }
        // 提示道具区
        if(click_x >= 320 && click_x <= 320+50){
            selected_points.clear();
            prop_bomb = false;
            prop_hammer = false;
            prop_refresh = false;
            prop_tip = true;
        }
    }
}

void Widget::updateWidget(){
    update(); // 请求重绘窗口
}


// 图片加载
void Widget::picInitial(){

    // 宝石图片的加载
    pic_mine1.load(":/material/picture/mine/mine1.png");
    pic_mine2.load(":/material/picture/mine/mine2.png");
    pic_mine3.load(":/material/picture/mine/mine3.png");
    pic_mine4.load(":/material/picture/mine/mine4.png");
    pic_mine5.load(":/material/picture/mine/mine5.png");
    pic_mine6.load(":/material/picture/mine/mine6.png");
    pic_mine7.load(":/material/picture/mine/mine7.png");
    pic_mine8.load(":/material/picture/mine/mine8.png");

    pic_mines = {pic_mine1,pic_mine2,pic_mine3,pic_mine4,pic_mine5,pic_mine6,pic_mine7,pic_mine8};

    pic_fruit1.load(":/material/picture/mine/fruit1.png");
    pic_fruit2.load(":/material/picture/mine/fruit2.png");
    pic_fruit3.load(":/material/picture/mine/fruit3.png");
    pic_fruit4.load(":/material/picture/mine/fruit4.png");
    pic_fruit5.load(":/material/picture/mine/fruit5.png");
    pic_fruit6.load(":/material/picture/mine/fruit6.png");
    pic_fruit7.load(":/material/picture/mine/fruit7.png");
    pic_fruit8.load(":/material/picture/mine/fruit8.png");

    pic_fruits = {pic_fruit1,pic_fruit2,pic_fruit3,pic_fruit4,pic_fruit5,pic_fruit6,pic_fruit7,pic_fruit8};

    pic_ice1.load(":/material/picture/mine/ice1.png");
    pic_ice2.load(":/material/picture/mine/ice2.png");
    pic_ice3.load(":/material/picture/mine/ice3.png");

    pic_ices = {pic_ice1,pic_ice2,pic_ice3};

    pic_destroy1.load(":/material/picture/mine/destroy1.png");
    pic_destroy2.load(":/material/picture/mine/destroy2.png");
    pic_destroy3.load(":/material/picture/mine/destroy3.png");

    pic_bomb.load(":/material/picture/tools/bomb.png");
    pic_refresh.load(":/material/picture/tools/refresh.png");
    pic_hammer.load(":/material/picture/tools/hammer.png");
    pic_tip.load(":/material/picture/tools/tip.jpg");

}


// 地图初始化
void mapInitial() {
    spaces.resize(row, std::vector<Space*>(col));

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            spaces[i][j] = new Space();
            Gemstone* gemstone = new Gemstone();
            spaces[i][j]->SetGemstone(gemstone);
        }
    }

    spaces[3][2]->SetType(3);
    spaces[3][3]->SetType(3);
    spaces[3][4]->SetType(3);
    spaces[3][5]->SetType(3);
    spaces[3][6]->SetType(3);
    spaces[4][2]->SetType(3);
    spaces[4][3]->SetType(3);
    spaces[4][4]->SetType(3);
    spaces[4][5]->SetType(3);
    spaces[4][6]->SetType(3);

}


// 交换格子的宝石
void SwapStone(int i1, int j1, int i2, int j2) {
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

    if (checkMap()) {
        std::cout << "交换后存在可消除部分" << std::endl;

    } else {
        std::cout << "交换后不存在可消除部分" << std::endl;
        spaces[i1][j1]->SetGemstone(g1);
        spaces[i2][j2]->SetGemstone(g2);
    }
}

// 根据toBomb位置执行消融操作
void performMelt() {

    // 根据消融位置执行消融操作
    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            int space_type = spaces[i][j]->GetType();
            if(toMelt[i][j] && space_type >0){

                spaces[i][j]->SetType(space_type-1);
                toMelt[i][j] = false;
            }
        }
    }

}



// 执行爆炸
void performBomb() {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            if (toBomb[i][j]) {
                Gemstone *g = spaces[i][j]->GetGemstone();
                if(g->bomb_life > 0)
                {

                    g->bomb_life--;
                    qDebug()<<"宝石爆炸周期减少至"<<g->bomb_life;
                }else{

                    delete spaces[i][j]->GetGemstone();
                    spaces[i][j]->SetGemstone(nullptr);
                    toBomb[i][j] = false;
                    score += 50;

                    // 让指定位置融化
                    toMelt[i][j] = true;
                    if(j-1>=0)
                        toMelt[i][j-1] = true;
                    if(j+1<col)
                        toMelt[i][j+1] = true;
                    if(i-1>=0){
                        toMelt[i-1][j] = true;
                    }
                    if(i+1<row){
                        toMelt[i+1][j] = true;
                    }

                }
            }
        }
    }
}


// 实现下落
void goDown() {
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


// 刷新空位置宝石
void flush() {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            if (spaces[i][j]->GetGemstone() == nullptr) {
                Gemstone* g = new Gemstone();
                spaces[i][j]->SetGemstone(g);
            }
        }
    }
}



// 判断地图是否有可消除部分
bool checkMap() {
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


// 检查一点为中心的5*5部分是否可消除
bool checkOne(int i, int j) {
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



// 清除地图至不可消除
void clearMap() {

    if(checkMap()){
        performBomb();
        goDown();
        flush();
    }

}
