// Kaelin Hoang, u0984462 | Tyrus Draper, u1038756 | Megan Schnedar, u0924126 | Khanhly Nguyen, u0822847 | Hannah Bruns, u0629077
/*
 * Frame class for a sprite editor
 *
 * Class follows the following outline:
 * -Constructor
 * -Draw functionality
 * -Save/Load
 *
 * Frame is the main representation of created sprites
*/

#include "frame.h"
#include <cmath>
#include <QPainter>

Frame::Frame()
{
    Frame(width, height, &background);
}

Frame::Frame(int pixelWidth, int pixelHeight, QColor* background)
{
    this->width = pixelWidth;
    this->height = pixelHeight;

    if(background)
    {
        this->background = *background;
    }

    this->pixels = QVector<std::tuple<int,int,int,int>>(width * height);
    int r, g, b, a;
    this->background.getRgb(&r,  &g, &b, &a);

    for(int i = 0; i < pixels.size(); i++)
    {
        pixels[i] = std::tuple<int,int,int,int>(r, g, b, a);
    }

    QPixmap pixmap(width, height);
    pixmap.fill(*background);
    this->display = pixmap;
}

void Frame::setPenColor(QColor color)
{
    penColor = color;
}

QColor Frame::getBackgroundColor()
{
    return this->background;
}

void Frame::drawPoint(QPoint point)
{
    QPainter painter(&display);
    QPen pen;
    pen.setColor(penColor);
    pen.setWidth(penSize);
    painter.setPen(pen);

    int r, g, b, a;
    this->penColor.getRgb(&r, &g, &b, &a);
    painter.drawPoint(point);
}

void Frame::drawRect(QPoint start, QPoint end)
{
    int pixXStart = start.x(); // col num
    int pixYStart = start.y(); // row num

    int pixXEnd = end.x(); // col num
    int pixYEnd = end.y(); // row num

    if(pixXStart > pixXEnd)
    {
        int temp = pixXStart;
        pixXStart = pixXEnd;
        pixXEnd = temp;
    }
    if(pixYStart > pixYEnd)
    {
        int temp = pixYStart;
        pixYStart = pixYEnd;
        pixYEnd = temp;
    }

    for(int x = pixXStart; x <= pixXEnd; x++)
    {
        drawPoint(QPoint(x, pixYStart));
        drawPoint(QPoint(x, pixYEnd));
    }

    for(int y = pixYStart; y <= pixYEnd; y++)
    {
        drawPoint(QPoint(pixXStart, y ));
        drawPoint(QPoint(pixXEnd, y));
    }
}

void Frame::drawCircle(QPoint start, QPoint end)
{
    int pixXStart = start.x(); // col num
    int pixYStart = start.y(); // row num

    int pixXEnd = end.x(); // col num
    int pixYEnd = end.y(); // row num

    if(pixXStart > pixXEnd)
    {
        int temp = pixXStart;
        pixXStart = pixXEnd;
        pixXEnd = temp;
    }
    if(pixYStart > pixYEnd)
    {
        int temp = pixYStart;
        pixYStart = pixYEnd;
        pixYEnd = temp;
    }

    qreal a = (qreal)(pixXEnd - pixXStart) / 2.0;
    qreal b = (qreal)(pixYEnd - pixYStart) / 2.0;

    qreal centerX = (qreal)pixXStart + a;
    qreal centerY = (qreal)pixYStart + b;

    for(int x = pixXStart; x <= pixXEnd; x++)
    {
        qreal yIncrement = pow((1 - pow(x - centerX, 2) / pow(a, 2)) * pow(b, 2), 0.5);
        int yPlus = round(centerY + yIncrement);
        int yMinus = round(centerY - yIncrement);

        drawPoint(QPoint(x, yPlus));
        drawPoint(QPoint(x, yMinus));
    }

    for(int y = pixYStart; y <= pixYEnd; y++)
    {
        qreal xIncrement = pow((1 - pow(y - centerY, 2) / pow(b, 2)) * pow(a, 2), 0.5);
        int xPlus = round(centerX + xIncrement);
        int xMinus = round(centerX - xIncrement);
        drawPoint(QPoint(xPlus, y));
        drawPoint(QPoint(xMinus, y));
    }
}

void Frame::drawLine(QPoint start, QPoint end)
{
    QPainter painter(&display);
    QPen pen;
    pen.setColor(penColor);
    pen.setWidth(penSize);
    painter.setPen(pen);

    int r, g, b, a;
    penColor.getRgb(&r, &g, &b, &a);

    painter.drawLine(start, end);
}

void Frame::fillColor(QPoint point)
{
    QImage image = display.toImage();
    for(int x = 0; x < image.width(); x++)
    {
        for(int y = 0; y < image.height(); y++)
        {
            int pixIdx = x + height * y;
            QColor color = image.pixelColor(x, y);
            int r,g,b,a;
            color.getRgb(&r, &g, &b, &a);
            pixels[pixIdx] = std::tuple<int, int, int, int>(r, g, b, a);
        }
    }
    int pixX = point.x(); // row num
    int pixY = point.y(); // col num
    int pixIdx = pixX+height*pixY;

    std::tuple<int,int,int,int> tuple = pixels[pixIdx];

    int r, g, b, a;
    this->penColor.getRgb(&r, &g, &b, &a);

    int count = 0;

    for(int i = 0; i < pixels.size(); i++)
    {
        if(std::get<0>(pixels[i]) == std::get<0>(tuple) && std::get<1>(pixels[i]) == std::get<1>(tuple)
                        && std::get<2>(pixels[i]) == std::get<2>(tuple) && std::get<3>(pixels[i]) == std::get<3>(tuple))
        {
            pixels[i] = std::tuple<int, int, int, int>(r, g, b, a);
            count++;
        }

    }
    drawFromColorFillVector(pixels);
}

// Helper function for drawing sprites from QVector<R,G,B,A> values
// Sets pen color and calls drawPoint(QPoint)
// Used for fill bucket
void Frame::drawFromColorFillVector(QVector<std::tuple<int, int, int, int>>& pixVec)
{
    for(int y = 0; y < this->height; y++)
    {
        for(int x = 0; x < this->width; x++)
        {
            this->penColor.setRgb(std::get<0>(pixVec[x+height*y]), std::get<1>(pixVec[x+height*y]), std::get<2>(pixVec[x+height*y]));
            QPoint point(x,y);
            drawPoint(point);
        }
    }
}

// Used when loading in a new sprite (our vector is 90 degrees off of incoming vector)
void Frame::drawFromVector(QVector<std::tuple<int, int, int, int>>& pixVec)
{
    for(int y = 0; y < this->height; y++)
    {
        for(int x = 0; x < this->width; x++)
        {
            this->penColor.setRgb(std::get<0>(pixVec[x*width+y]), std::get<1>(pixVec[x*width+y]), std::get<2>(pixVec[x*width+y]));
            QPoint point(x,y);
            drawPoint(point);
        }
    }
}
