// Written by: Hannah Bruns, Tyrus Draper, Kaelin Hoang, Khanhly Nguyen, Megan Schnedar
#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>


class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~ClickableLabel();
     int ID;

signals:
    void clicked(int);

protected:
    void mousePressEvent(QMouseEvent* event);
};

#endif // CLICKABLELABEL_H
