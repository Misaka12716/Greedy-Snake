#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include <QTimer>
#include <QAction>
#include "mainwindow.h"
class QGraphicsScene;
class QKeyEvent;

class Snake;
class Food;
class Wall;
class toxin;

class GameController : public QObject
{
    Q_OBJECT
public:
    GameController(QGraphicsScene &scene, QObject *parent = 0);
    ~GameController();

    void snakeAteFood(Food *food);
    void snakeAtetoxin(toxin *t);
    void snakeAteWall();
    void snakeAteItself();

    QAction *getResmueAction(){ return resumeAction;}
    void setResumeAction(QAction* r){ resumeAction = r; }
    void setModel1Action(QAction* r){Model1Action=r;}
    void setModel2Action(QAction* r){Model2Action=r;}

    //给一个坐标，返回该坐标的信息
    char GetMapInformation(int x,int y){return map[x][y];}

    int ReadScore(int mapnum,int GameModel);//读取历史分数

public slots:
    void pause();
    void resume();
    void gameOver();
    void SpeedSetting();//速度设置
    void ChangeModel1();//切换为正常模式
    void ChangeModel2();//切换为有毒模式
    void MapSetting();//设置地图
protected:
     bool eventFilter(QObject *object, QEvent *event);

private:
    void handleKeyPressed(QKeyEvent *event);
    void addNewFood();
    void addNewtoxin(Food*food);
    void addNewWall();
    void setResume();
    void setModel1();
    void setModel2();
    void ChangeMap(int map_num);//切换地图
    void WriteScore();//将本次得到的分数录入

    QAction *resumeAction;
    QAction *Model1Action;
    QAction *Model2Action;
    QTimer timer;

    //GameController的两个成员变量
    QGraphicsScene &scene;
    Snake *snake;
    bool isPause;
    int difficulty;//困难度，用来调整frequence
    int frequence;//
    int CurrentScore;//游戏得分
    bool GameModel;//游戏模式
    int mapnum;//地图的编号
    char map[25][25];//记录各个元素的坐标，以免重复

};

#endif
