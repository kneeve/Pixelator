// Kaelin Hoang, u0984462 | Tyrus Draper, u1038756 | Megan Schnedar, u0924126 | Khanhly Nguyen, u0822847 | Hannah Bruns, u0629077

#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget* parent, Qt::WindowFlags f) : QLabel(parent) {}

ClickableLabel::~ClickableLabel() {}

void ClickableLabel::mousePressEvent(QMouseEvent* event)
{
    emit clicked(ID);
}
