#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stdio.h"
#include <iostream>
#include <algorithm>
#include <QGraphicsPixmapItem>
#include <QFileDialog>
#include <QAction>
#include <QRubberBand>
#include <QSignalMapper>
#include <QMouseEvent>
#include <QMessageBox>
#include <QPalette>
#include <QColorDialog>
#include "libs/color.h"
#include "libs/utilities.h"
#include "libs/error_message.h"
#include "libs/bmplib.h"
#include "about.h"
#include "dialoginfor.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->imageView,SIGNAL(rubberBandChanged(QRect,QPointF,QPointF)),this,SLOT(rubberBand_changed(QRect,QPointF,QPointF)));
    connect(ui->btnRectangle,SIGNAL(clicked(bool)),this,SLOT(btnRectangle_clicked()));
    connect(ui->btnReplace,SIGNAL(clicked(bool)),this,SLOT(btnReplace_clicked()));
    connect(ui->btnSwap,SIGNAL(clicked(bool)),this,SLOT(btnSwap_clicked()));

    connect(ui->btnChangeColor,SIGNAL(clicked(bool)),this,SLOT(changeColor()));

    QPalette p = ui->lblCurColor->palette();
    p.setColor(QPalette::Background,Qt::white);
    ui->lblCurColor->setPalette(p);

    setFocusPolicy(Qt::StrongFocus);
}
void MainWindow::keyPressEvent(QKeyEvent *event){
    if(event->key()==Qt::Key_Escape&&selectionMode){
        toggleSelectionMode();
        mode=0;
    }
}
void MainWindow::changeColor(){
    QColor qColor = QColorDialog::getColor(Qt::white,this,"Choose Color",Q_NULLPTR);
    QPalette p = ui->lblCurColor->palette();
    p.setColor(QPalette::Background,qColor);
    ui->lblCurColor->setPalette(p);
    color.b=qBlue(qColor.rgb());
    color.g=qGreen(qColor.rgb());
    color.r=qRed(qColor.rgb());

}
void MainWindow::resizeEvent(){
    if(p!=Q_NULLPTR) ui->imageView->fitInView(p, Qt::KeepAspectRatio);
}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::setToolbarStates(bool state){
    ui->toolbox->setEnabled(state);
}
void MainWindow::rubberBand_changed(QRect rubberBandRect, QPointF fromScenePoint, QPointF toScenePoint){
    if(selectionMode){
        if((rubberBandRect.x()==0&&rubberBandRect.y()==0)&&(rubberBandRect.height()==0&&rubberBandRect.width()==0)){
            isSelecting=false;
            toggleSelectionMode();
            switch (mode) {
                case MODE_DRAW_REC:
                    llib::BMPLib::drawRectangle(bitmap,&top_left,&bot_right,&color,INT_MAX);
                    p->setPixmap(image);
                    break;
                case MODE_SWAP:
                    llib::BMPLib::swapRegions(bitmap,&top_left,&bot_right);
                    p->setPixmap(image);
                    break;
                default:
                    break;
            }

        }
        isSelecting = true;

        top_left.x=std::min(fromScenePoint.x(),toScenePoint.x());
        bot_right.x=std::max(fromScenePoint.x(),toScenePoint.x());

        top_left.y=std::max(fromScenePoint.y(),toScenePoint.y());
        bot_right.y=std::min(fromScenePoint.y(),toScenePoint.y());


        llib::clampCoordinate(&bot_right,&img_size);
        llib::clampCoordinate(&top_left,&img_size);
    }
}
void MainWindow::toggleSelectionMode(){
    selectionMode=!selectionMode;
    if(selectionMode){
        ui->imageView->setDragMode(QGraphicsView::RubberBandDrag);
        setToolbarStates(false);
    }else{
        ui->imageView->setDragMode(QGraphicsView::NoDrag);
        setToolbarStates(true);
    }
}

void MainWindow::btnRectangle_clicked(){
    mode=MODE_DRAW_REC;
    toggleSelectionMode();
}
void MainWindow::btnReplace_clicked(){
    llib::BMPLib::replaceMostCommon(bitmap,&img_size,&color);
    p->setPixmap(image);
}
void MainWindow::btnSwap_clicked(){
    mode=MODE_SWAP;
    toggleSelectionMode();
}
void MainWindow::on_actionAbout_triggered(){
    About *dialog = new About(this);
    dialog->show();
}
void MainWindow::on_actionFileInfor_triggered(){
    DialogInfor *dialog = new DialogInfor(this);
    dialog->setup(header,qfile);
    dialog->show();
}

void MainWindow::on_actionOpen_triggered()
{
    qfile = QFileDialog::getOpenFileName(this,
        tr("Open Bitmap"), "", tr("BMP File (*.bmp)"));
    //QMessageBox::about(this,tr("Error"),qfile);
    if(qfile==Q_NULLPTR) {
        return;
    }
    char *fileName = (char*) malloc((qfile.length()+5)*sizeof(char));
    strcpy(fileName,qfile.toStdString().c_str());
    printf("%s",fileName);
    if(header!=NULL){
        llib::BMPLib::freeFile(&header,&bitmap);
    }
    llib::BMPLib::readBitmap(fileName,&header,&bitmap);


    if(err_message!=NULL){
        QMessageBox::critical(this,tr("Error"),tr(err_message),QMessageBox::Ok);
        return;
    }else{
        int row_size = (header->bpp*(header->img_width)+31)/32*4;

        img_size.x=header->img_width;
        img_size.y=header->img_height;
        QImage img ((uchar*)(*bitmap),header->img_width,header->img_height,row_size,QImage::Format_RGB888,Q_NULLPTR,Q_NULLPTR);
        QGraphicsScene* scene = new QGraphicsScene;
        image = QPixmap::fromImage(img,Qt::NoFormatConversion);
        p=scene->addPixmap(image);
        ui->imageView->setScene(scene);

        if(p!=Q_NULLPTR) ui->imageView->fitInView(p, Qt::KeepAspectRatio);

        ui->actionSave->setEnabled(true);
        ui->actionFileInfor->setEnabled(true);
        setToolbarStates(true);
    }
}

void MainWindow::on_actionSave_triggered()
{
    const char *fileName = qfile.toStdString().c_str();
    llib::BMPLib::writeBitmap(fileName,header,bitmap);

}
