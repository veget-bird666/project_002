#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <vector>
QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    int x = 0;
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;

    QTimer *timer;  // 定义 QTimer 为类的成员变量

    // 图片成员变量


    QPixmap pic_mine1;
    QPixmap pic_mine2;
    QPixmap pic_mine3;
    QPixmap pic_mine4;
    QPixmap pic_mine5;
    QPixmap pic_mine6;
    QPixmap pic_mine7;
    QPixmap pic_mine8;

    QPixmap pic_fruit1;
    QPixmap pic_fruit2;
    QPixmap pic_fruit3;
    QPixmap pic_fruit4;
    QPixmap pic_fruit5;
    QPixmap pic_fruit6;
    QPixmap pic_fruit7;
    QPixmap pic_fruit8;

    QPixmap pic_destroy1;
    QPixmap pic_destroy2;
    QPixmap pic_destroy3;

    QPixmap pic_bomb;
    QPixmap pic_refresh;
    QPixmap pic_hammer;
    QPixmap pic_tip;

    QPixmap pic_ice1;
    QPixmap pic_ice2;
    QPixmap pic_ice3;

    // 图片数组
    std::vector<QPixmap> pic_mines;
    std::vector<QPixmap> pic_fruits;
    std::vector<QPixmap> pic_ices;

    void picInitial();

    // 绘画函数
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    // 实现重绘
    void updateWidget();


};
#endif // WIDGET_H
