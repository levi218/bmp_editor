#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QRubberBand>
#include <QMouseEvent>
#include <cstdlib>
#include "libs/bmplib.h"
#include "libs/color.h"
#include "libs/utilities.h"

namespace Ui {

class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

private slots:
        void on_actionOpen_triggered();

        void on_actionSave_triggered();
        void btnRectangle_clicked();
        void btnReplace_clicked();
        void btnSwap_clicked();

        void changeColor();

        void rubberBand_changed(QRect rubberBandRect, QPointF fromScenePoint, QPointF toScenePoint);
private:
    Ui::MainWindow *ui;
    QGraphicsPixmapItem *p=Q_NULLPTR;
    QGraphicsScene *scene;

    QPixmap image;
    //QRubberBand *rubberBand=NULL;
    QPoint selectionOrigin;

    int c;
    bool isSelecting = false;
    int filled=0;
    int thickness=1;
    llib::Vector top_left={0,0};
    llib::Vector bot_right={0,0};
    llib::Color color = {255,255,255};
    short mode=0;
    QString qfile=Q_NULLPTR;

    llib::Header *header=NULL;
    llib::Bitmap bitmap=NULL;
    llib::Vector img_size;
    bool selectionMode = false;
    void setToolbarStates(bool state);
    void toggleSelectionMode();


protected:
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent();
};
#endif // MAINWINDOW_H
