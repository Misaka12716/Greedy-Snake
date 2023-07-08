#include <QEvent>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QMessageBox>
#include <QAction>
#include <iostream>
#include <QInputDialog>
#include <QFile>


#include "gamecontroller.h"
#include "food.h"
#include "toxin.h"
#include "Wall.h"
#include "snake.h"
#include "mainwindow.h"

//构造函数
GameController::GameController(QGraphicsScene &scene, QObject *parent) :
    QObject(parent),
    scene(scene),
    snake(new Snake(*this)),
    isPause(false),
    difficulty(5),//设置难度，此难度可以更改
    frequence(33),//设置每秒帧数，从而设置速度，初始定时间隔为1000/33毫秒，即每秒30帧
    CurrentScore(0),
    GameModel(0),
    mapnum(0)
{
    timer.start( 1000/frequence );

    Food *a1 = new Food(0, -50);
    scene.addItem(a1);

    ChangeMap(mapnum);

    //根据地图在地图上画墙
    for(int i=0;i<20;i++){
        for(int j=0;j<20;j++){
            if(map[i][j]=='w'){
                Wall* w=new Wall((i-10)*10,(j-10)*10);
                scene.addItem(w);
            }
        }
    }

    scene.addItem(snake);
    scene.installEventFilter(this);
    //resume();
    connect(&timer, SIGNAL(timeout()), &scene, SLOT(advance()));
   isPause = false;
}

//析构函数
GameController::~GameController()
{
}

void GameController::ChangeMap(int map_num){
   //地图初始化
   for(int i=0;i<20;i++){
        for(int j=0;j<20;j++){
            map[i][j]='\0';
        }
   }

   //根据不同地图的选择，切换游戏的地图
   switch(mapnum){
   //第一个地图：没有墙的
   case 0:
        break;
   //第二个地图：Box
   case 1:
        for(int i=0;i<20;i++){
            map[i][0]='w';
            map[i][19]='w';
            map[0][i]='w';
            map[19][i]='w';
        }
        break;
   //第三个地图：Railway
   case 2:
        for(int j=4;j<=15;j++){
            map[j][4]='w';
            map[j][7]='w';
            map[j][12]='w';
            map[j][15]='w';
        }
        for(int i=0;i<5;i++){
            map[0][i]='w';
            map[i][0]='w';
            map[19][i]='w';
            map[i][19]='w';
            map[0][19-i]='w';
            map[19-i][0]='w';
            map[19][19-i]='w';
            map[19-i][19]='w';
        }
        break;
   }
}

//当蛇吃到食物时，调用此函数
void GameController::snakeAteFood(Food *food)
{
   CurrentScore+=int(10+1.2*(difficulty-5));
   int fx=snake->head.x(),fy=snake->head.y();
   map[fx/10+10][fy/10+10]='\0';
   scene.removeItem(food);

    addNewFood();
    addNewtoxin(food);
}

//当蛇吃到毒药时，调用此函数
void GameController::snakeAtetoxin(toxin* t)
{
    //减少分数
    CurrentScore-=int(10+2*(difficulty-5));

    //如果蛇的长度过短，则直接判断游戏结束
    if(snake->length==1){
         QTimer::singleShot(0, this, SLOT(gameOver()));
    }
    for(int i=0;i<snake->length/2;++i)
    {
        int nx=snake->tail.first().x(),ny=snake->tail.first().y();
        snake->tail.removeFirst();
        Wall*w=new Wall(nx,ny);
        //蛇的后半截会变成墙
        map[nx/10+10][ny/10+10]='w';
        scene.addItem(w);
    }
    snake->length+=1;
    snake->length/=2;
    int tx=snake->head.x(),ty=snake->head.y();
    map[tx/10+10][ty/10+10]='\0';
    scene.removeItem(t);

}

//如果蛇吃到了墙
void GameController::snakeAteWall()
{
    //std::cout<<"snake ate wall"<<std::endl;
    QTimer::singleShot(0, this, SLOT(gameOver()));
}

//如果蛇吃到了自己
void GameController::snakeAteItself()
{
    //std::cout<<"snake ate itself";
    QTimer::singleShot(0, this, SLOT(gameOver()));
}

//根据方向操作
void GameController::handleKeyPressed(QKeyEvent *event)
{
    if (!isPause){
        switch (event->key()) {
            case Qt::Key_Left:
                snake->setMoveDirection(Snake::MoveLeft);
                break;
            case Qt::Key_Right:
                snake->setMoveDirection(Snake::MoveRight);
                break;
            case Qt::Key_Up:
                snake->setMoveDirection(Snake::MoveUp);
                break;
            case Qt::Key_Down:
                snake->setMoveDirection(Snake::MoveDown);
                break;
            case Qt::Key_Space:
                pause();
                break;
        }
    }
    else resume();
}

//当食物被吃掉后调用此函数，重新加一个食物
void GameController::addNewFood()
{
    int x, y;

    for(;;){
        do {
                x = (int)(rand() % 200) / 10 - 10;
                y = (int)(rand() % 200) / 10 - 10;

                x *= 10;
                y *= 10;
        } while (snake->shape().contains(snake->mapFromScene(QPointF(x + 5, y + 5))));
        if(map[x/10+10][y/10+10]=='\0'){
                map[x/10+10][y/10+10]='f';//在map数组上进行标注
                break;
        }
    }

	Food *food = new Food(x, y);
    scene.addItem(food);
}

//当食物被吃掉后调用此函数，重新加一个毒药
void GameController::addNewtoxin(Food*food)
{
    //如果正常模式，直接不放毒药
    if(!GameModel){
        return;
    }
    int x, y;

    for (;;) {
        do {
                x = (int)(rand() % 200) / 10 - 10;
                y = (int)(rand() % 200) / 10 - 10;

                x *= 10;
                y *= 10;
        } while (snake->contains(QPoint(x, y)) || food->pos() == QPoint(x, y));
        if(map[x/10+10][y/10+10]=='\0'){
                map[x/10+10][y/10+10]='t';//在map数组上进行标注
                break;
        }
    }

    toxin *t = new toxin(x, y);
    scene.addItem(t);
}

//游戏结束后统一调用的函数
void GameController::gameOver()
{
    //先断开计时器
    disconnect(&timer, SIGNAL(timeout()), &scene, SLOT(advance()));

    //编辑messagebox的输出
    QString tips="本局得分：";
    QString score=QString::number(CurrentScore);
    tips.append(score);

    //返回当前地图&模式的最高分，看是否破纪录
    int HistoryScore=ReadScore(mapnum,GameModel);
    if(CurrentScore>HistoryScore){
        tips.append("\n新纪录！");
        WriteScore();
    }
    else{
        tips.append("\n当前地图&模式的最高分：");
        tips.append(QString::number(HistoryScore));
    }

    tips.append("\n开始新游戏？");
    std::string str=tips.toStdString();
    const char* output=str.c_str();

    if (QMessageBox::Yes == QMessageBox::information(NULL,
                            tr("当前游戏已结束"), tr(output),
                            QMessageBox::Yes | QMessageBox::No,
                            QMessageBox::Yes)) {
        connect(&timer, SIGNAL(timeout()), &scene, SLOT(advance()));
        scene.clear();
        ChangeMap(mapnum);
        //根据地图在地图上画墙
        for(int i=0;i<20;i++){
                for(int j=0;j<20;j++){
                    if(map[i][j]=='w'){
                        Wall* w=new Wall((i-10)*10,(j-10)*10);
                        scene.addItem(w);
                    }
                }
        }
        CurrentScore=0;
        snake = new Snake(*this);
        scene.addItem(snake);
        addNewFood();
    }
    else {
        exit(0);
    }
}

//暂停函数
void GameController::pause()
{
    //断开定时器的信号
    disconnect(&timer, SIGNAL(timeout()),
               &scene, SLOT(advance()));
    isPause = true;
    setResume();
}

//恢复函数
void GameController::resume()
{
    //重新连接定时器的信号
    connect(&timer, SIGNAL(timeout()), &scene, SLOT(advance()));
   isPause = false;
   setResume();
}

//设置暂停/恢复按键能不能按下去
void GameController :: setResume(){
    if(isPause == true){
        resumeAction->setEnabled(true);
    }else{
        resumeAction->setEnabled(false);
    }
}

//切换游戏模式为正常模式
void GameController::ChangeModel1(){
    //断开定时器的信号
    //std::cout<<"ChangeModel1"<<std::endl;
    disconnect(&timer, SIGNAL(timeout()),
               &scene, SLOT(advance()));
    GameModel=0;
    setModel2();
    setModel1();
    QTimer::singleShot(0, this, SLOT(gameOver()));
}

//切换游戏模式为有毒模式
void GameController::ChangeModel2(){
    //断开定时器的信号
    //std::cout<<"ChangeModel2"<<std::endl;
    disconnect(&timer, SIGNAL(timeout()),
               &scene, SLOT(advance()));
    GameModel=1;
    setModel1();
    setModel2();
    QTimer::singleShot(0, this, SLOT(gameOver()));
}

//设置切换模式的按键能否按下去
void GameController::setModel2(){
    if(GameModel==true){
        Model2Action->setEnabled(false);
    }
    else{
        Model2Action->setEnabled(true);
    }
}
void GameController::setModel1(){
    if(GameModel==true){
        Model1Action->setEnabled(true);
    }
    else{
        Model1Action->setEnabled(false);
    }
}


//事件过滤器
bool GameController::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        handleKeyPressed((QKeyEvent *)event);
        return true;
    } else {
        return QObject::eventFilter(object, event);
    }
}

//设置游戏的速度
void GameController::SpeedSetting()
{
    //std::cout<<"SpeedSetting"<<std::endl;
    //先断开计时器
    disconnect(&timer, SIGNAL(timeout()), &scene, SLOT(advance()));

    //玩家输入1-10的整数,设置贪吃蛇的速度以及游戏难度
    QString dlgTitle="设置游戏难度";
    QString txtLabel="请设置你认为适合的游戏难度";
    int minValue=1,maxValue=10,stepValue=1;
    bool ok=false;
    int inputValue=QInputDialog::getInt(NULL,dlgTitle,txtLabel,difficulty,minValue,maxValue,stepValue,&ok);

    //如果确定输入
    if(ok){
        difficulty=inputValue;
        frequence=33*pow(1.2,difficulty-5);
        timer.setInterval(1000 / frequence);
        QTimer::singleShot(0, this, SLOT(gameOver()));
    }
}

//选择游戏地图
void GameController::MapSetting(){
    //先断开计时器
    //std::cout<<"ChangeMap"<<std::endl;
    disconnect(&timer, SIGNAL(timeout()), &scene, SLOT(advance()));

    //玩家在下拉菜单中选择地图
    bool ok=false;//是否点击确定
    QString dlgTitle="选择地图";
    QString txtLabel="请选择你想要游玩的地图";
    QStringList items;
    items<<tr("No Wall")<<tr("Box")<<tr("Railway");
    QString item=QInputDialog::getItem(NULL,dlgTitle,txtLabel,
        items,mapnum,false,&ok);
    if(ok){
        if(item=="No Wall"){
                mapnum=0;
        }
        else if(item=="Box"){
                mapnum=1;
        }
        else if(item=="Railway"){
                mapnum=2;

        }
        QTimer::singleShot(0, this, SLOT(gameOver()));
    }
}

//只用来写入分数数据
void GameController::WriteScore(){
    QFile *file;
    switch(mapnum){
    case 0:
        if(GameModel){
                file=new QFile("score01.txt");
        }
        else{
                file=new QFile("score00.txt");
        }
        break;
    case 1:
        if(GameModel){
                file=new QFile("score11.txt");
        }
        else{
                file=new QFile("score10.txt");
        }
        break;
    case 2:
        if(GameModel){
                file=new QFile("score21.txt");
        }
        else{
                file=new QFile("score20.txt");
        }
    }

    //用只写模式打开文件，且是覆盖写入
    if(!file->open(QIODevice::WriteOnly|QIODevice::Text)){
        return;
    }
    QTextStream scoretxt(file);
    QString sco=QString::number(CurrentScore);
    scoretxt<<sco;
    file->close();
}

//用来读取分数数据
int GameController::ReadScore(int mapnum,int GameModel){
    QFile *file;
    switch(mapnum){
    case 0:
        if(GameModel){
                file=new QFile("score01.txt");
        }
        else{
                file=new QFile("score00.txt");
        }
        break;
    case 1:
        if(GameModel){
                file=new QFile("score11.txt");
        }
        else{
                file=new QFile("score10.txt");
        }
        break;
    case 2:
        if(GameModel){
                file=new QFile("score21.txt");
        }
        else{
                file=new QFile("score20.txt");
        }
    }

    //用只读模式打开文件
    if(!file->open(QIODevice::ReadOnly|QIODevice::Text)){
        return 0;
    }
    QTextStream scoretxt(file);

    QString sco;
    sco=scoretxt.readAll();
    file->close();
    if(sco.length()==0){
        return 0;
    }
    return sco.toInt();
}
