#include "dialoginfor.h"
#include "ui_dialoginfor.h"

DialogInfor::DialogInfor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogInfor)
{
    ui->setupUi(this);
}
void DialogInfor::setup(const llib::Header *header,QString filename){
    ui->lblInfor->setText("File location: "+filename+
                          "\nFile size: "+QString::number(header->file_header.file_size)+" Bytes"+
                          "\nImage size: "+QString::number(header->img_width)+"x"+QString::number(header->img_height)+" pixels"+
                          "\nBit depth: "+QString::number(header->bpp)
                          );

}

DialogInfor::~DialogInfor()
{
    delete ui;
}
