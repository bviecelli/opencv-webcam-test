#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QGraphicsScene>

using namespace std;
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

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

    int morph_size;
    int detectLicensePlate(Mat *im);
    double licensePlateFound(Mat *img, QString *licensePlate);

public slots:
    void capture();
private slots:
    void toggleLiveCam();
    void editCamAddr();
    void updateParam(int n);
};

#endif // MAINWINDOW_H
