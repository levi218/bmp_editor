#ifndef DIALOGINFOR_H
#define DIALOGINFOR_H

#include <QDialog>
#include "libs/bmplib.h"
namespace Ui {
class DialogInfor;
}

class DialogInfor : public QDialog
{
    Q_OBJECT

public:
    explicit DialogInfor(QWidget *parent = 0);
    void setup(const llib::Header *header,QString filename);
    ~DialogInfor();

private:
    Ui::DialogInfor *ui;
};

#endif // DIALOGINFOR_H
