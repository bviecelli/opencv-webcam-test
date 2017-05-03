#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Common.h"
#include <QMainWindow>
#include <QTimer>
#include <QGraphicsScene>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>
#include <opencv2/videoio.hpp>
#include <tesseract/baseapi.h>

#define LPR_WHITE_LIST "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ-"

using namespace std;
using namespace cv;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTimer timer;
    VideoCapture cap;
    QGraphicsScene scene;
    bool liveIsRunning;
    QString camAddr;
    CascadeClassifier cascade;

    tesseract::TessBaseAPI *ocr;
    QString whiteList;
    Ptr<CLAHE> cla;

    double block_size;
    int detectLicensePlate(Mat *im);
    double licensePlateFound(Mat *img, QString *licensePlate);

public slots:
    void capture();
private slots:
    void toggleLiveCam();
    void editCamAddr();
    void updateParam(double n);
};

#endif // MAINWINDOW_H
