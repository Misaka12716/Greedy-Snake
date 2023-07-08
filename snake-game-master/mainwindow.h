#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qsettings.h>

class QGraphicsScene;
class QGraphicsView;

class GameController;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void adjustViewSize();
	void newGame();
	void gameHelp();
    void HistoryScore();

private:
	void createActions();
	void createMenus();

    void initScene();
    void initSceneBackground();

    QGraphicsScene *scene;
    QGraphicsView *view;

    GameController *game;

	QAction *newGameAction;
	QAction *pauseAction;
	QAction *resumeAction;
	QAction *exitAction;
    QAction *SpeedSettingAction;
	QAction *gameHelpAction;
    QAction *ChangeModel1Action;
    QAction *ChangeModel2Action;
    QAction *MapSettingAction;
    QAction *HistoryScoreAction;
};

#endif
