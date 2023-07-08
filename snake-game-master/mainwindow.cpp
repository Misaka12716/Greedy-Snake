#include <QGraphicsView>
#include <QTimer>
#include <qaction.h>
#include <qmenubar.h>
#include <qapplication.h>
#include <qmessagebox.h>
#include <QInputDialog>
#include<stdio.h>

#include "constants.h"
#include "gamecontroller.h"
#include "mainwindow.h"
#include <QIcon>

//构造函数
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      scene(new QGraphicsScene(this)),
      view(new QGraphicsView(scene, this)),
      game(new GameController(*scene, this))
{
    this->setWindowTitle("贪吃蛇");
    setCentralWidget(view);
    resize(600, 600);
//    setFixedSize(600, 600);
    setWindowIcon(QIcon(":/images/snake_ico"));

	createActions();
	createMenus();
    initScene();
    initSceneBackground();

    QTimer::singleShot(0, this, SLOT(adjustViewSize()));
}

MainWindow::~MainWindow()
{
    
}

void MainWindow::adjustViewSize()
{
    view->fitInView(scene->sceneRect(), Qt::KeepAspectRatioByExpanding);
}

void MainWindow::createActions()
{
    newGameAction = new QAction(tr("&新游戏"), this);
	newGameAction->setShortcuts(QKeySequence::New);
	newGameAction->setStatusTip(tr("Start a new game"));
	connect(newGameAction, &QAction::triggered, this, &MainWindow::newGame);

    exitAction = new QAction(tr("&离开"), this);
	exitAction->setShortcut(tr("Ctrl+Q"));
	exitAction->setStatusTip(tr("Exit the game"));
	connect(exitAction, &QAction::triggered, this, &MainWindow::close);

    ChangeModel1Action=new QAction("&切换到 正常模式");
    ChangeModel1Action->setShortcut(tr("Ctrl+M"));
    ChangeModel1Action->setStatusTip(tr("Model 1"));
    ChangeModel1Action->setEnabled(false);
    game->setModel1Action(ChangeModel1Action);
    connect(ChangeModel1Action,&QAction::triggered,game,&GameController::ChangeModel1);

    ChangeModel2Action=new QAction("&切换到 有毒模式");
    ChangeModel2Action->setShortcut(tr("Ctrl+M"));
    ChangeModel2Action->setStatusTip(tr("Model 2"));
    game->setModel2Action(ChangeModel2Action);
    connect(ChangeModel2Action,&QAction::triggered,game,&GameController::ChangeModel2);

    pauseAction = new QAction(tr("&暂停游戏"), this);
	pauseAction->setStatusTip(tr("Pause..."));
	connect(pauseAction, &QAction::triggered, game, &GameController::pause);

    resumeAction = new QAction(tr("&恢复游戏"), this);
	resumeAction->setStatusTip(tr("Resume..."));
    resumeAction->setEnabled(false);
    game->setResumeAction(resumeAction);
    connect(resumeAction, &QAction::triggered, game, &GameController::resume);

    MapSettingAction=new QAction(tr("&选择地图"),this);
    MapSettingAction->setStatusTip(tr("Choose your map..."));
    connect(MapSettingAction,&QAction::triggered,game,&GameController::MapSetting);

    SpeedSettingAction=new QAction(tr("&设置游戏难度"),this);
    SpeedSettingAction->setStatusTip(tr("Set game difficulty..."));
    connect(SpeedSettingAction,&QAction::triggered,game,&GameController::SpeedSetting);

    HistoryScoreAction=new QAction(tr("&查看历史得分"),this);
    HistoryScoreAction->setStatusTip("Your history scores...");
    connect(HistoryScoreAction,&QAction::triggered,this,&MainWindow::HistoryScore);

    gameHelpAction = new QAction(tr("&游戏帮助"), this);
	gameHelpAction->setShortcut(tr("Ctrl+H"));
	gameHelpAction->setStatusTip(tr("Help on this game"));
    connect(gameHelpAction, &QAction::triggered, this, &MainWindow::gameHelp);
}

void MainWindow::createMenus()
{
    QMenu *options = menuBar()->addMenu(tr("&选项"));
	options->addAction(newGameAction);

    options->addSeparator();
    options->addAction(MapSettingAction);
    options->addAction(SpeedSettingAction);

    options->addSeparator();
    options->addAction(ChangeModel2Action);
    options->addAction(ChangeModel1Action);

	options->addSeparator();
	options->addAction(pauseAction);
	options->addAction(resumeAction);

	options->addSeparator();
	options->addAction(exitAction);

    QMenu *help = menuBar()->addMenu(tr("&帮助"));
    help->addAction(HistoryScoreAction);
	help->addAction(gameHelpAction);
}

void MainWindow::initScene()
{
    scene->setSceneRect(-100, -100, 200, 200);
}

void MainWindow::initSceneBackground()
{
    QPixmap bg(TILE_SIZE, TILE_SIZE);
    QPainter p(&bg);
    p.setBrush(QBrush(Qt::black));//设置背景颜色
    p.drawRect(0, 0, TILE_SIZE, TILE_SIZE);

    view->setBackgroundBrush(QBrush(bg));
}

//新游戏操作
void MainWindow::newGame()
{
	QTimer::singleShot(0, game, SLOT(gameOver()));
}

//游戏帮助
void MainWindow::gameHelp()
{
    QMessageBox::about(this, tr("游戏帮助"), tr("游玩方法；使用键盘上的方向键控制贪吃蛇向一个方向移动或转向，尽力吃到更多的食物，得到更高的分数！\n\n"
        "游戏模式：在有毒模式中，伴随着红色食物出现的还有绿色毒饵，当贪吃蛇不慎吃到毒饵时，一半的身体会因为中毒而脱落，变成墙壁，同时得分也会被扣减\n\n"
        "快捷键：\n空格/P - 暂停游戏 & 恢复游戏\n"
        "Ctrl+M - 切换有毒模式 & 正常模式\n"
        "Ctrl+H - 游戏帮助"
        "Ctrl+Q - 离开游戏"));
}

//获取各个地图和模式的历史分数
void MainWindow::HistoryScore(){
    QString output;
    output.append(tr("No Wall:\n有毒模式："));
    output.append(QString::number(game->ReadScore(0,1)));
    output.append(tr("\n正常模式："));
    output.append(QString::number(game->ReadScore(0,0)));

    output.append(tr("\nBox:\n有毒模式："));
    output.append(QString::number(game->ReadScore(1,1)));
    output.append(tr("\n正常模式："));
    output.append(QString::number(game->ReadScore(1,0)));

    output.append(tr("\nRailWay:\n有毒模式："));
    output.append(QString::number(game->ReadScore(2,1)));
    output.append(tr("\n正常模式："));
    output.append(QString::number(game->ReadScore(2,0)));
    QMessageBox::about(this,tr("历史得分"),output);
}
