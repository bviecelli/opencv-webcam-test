#ifndef CAMFRAME_H
#define CAMFRAME_H

#include <QGraphicsView>

class CamFrame : public QGraphicsView
{
public :
    CamFrame(QWidget* centralWidget) : QGraphicsView(centralWidget) {}
    CamFrame(QGraphicsScene * scene, QWidget * parent = 0) : QGraphicsView(scene, parent){}
    ~CamFrame() {}
    void setImage(QImage img);

protected :
    void paintEvent(QPaintEvent* paintEventInfo);

private :
    QImage image;
};

#endif // CAMFRAME_H
