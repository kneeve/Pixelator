// Written by: Hannah Bruns, Tyrus Draper, Kaelin Hoang, Khanhly Nguyen, Megan Schnedar
#ifndef FRAME_H
#define FRAME_H

#include <QVector>
#include <QPixmap>


class Frame
{
public:
    Frame();
    Frame(int, int, QColor* background = 0);
    void drawPoint(QPoint);
    void drawRect(QPoint, QPoint);
    void drawCircle(QPoint, QPoint);
    void drawLine(QPoint, QPoint);
    void drawFromColorFillVector(QVector<std::tuple<int, int, int, int>>&);
    void drawFromVector(QVector<std::tuple<int, int, int, int>>&);
    void setPenColor(QColor);
    void fillColor(QPoint);
    QColor getBackgroundColor();

    QVector<std::tuple<int, int, int, int>> pixels;
    QColor penColor = Qt::black;
    QPixmap display;
    QColor background = Qt::white;
    int width = 64;
    int height = 64;
    int penSize =1;
    int ID;
};

#endif // FRAME_H
