#include "camframe.h"

void CamFrame::setImage(QImage img)
{
    image = img; //set the new image scene
    this->scene()->update(); //update event
}

void CamFrame::paintEvent(QPaintEvent* evnt)
{
    QPainter painter(viewport());
    painter.drawImage(QRect(0,0,viewport()->width(),viewport()->height()), image, QRect(0,0, image.width(), image.height()) );
    QGraphicsView::paintEvent(evnt);
}
