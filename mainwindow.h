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

    Mat liveImageTemplate;
    Ptr<Feature2D> detector;
    Ptr<DescriptorMatcher> matcher;
    Ptr<FeatureDetector> featureDetector;
    Ptr<DescriptorExtractor> descriptorExtractor;

    std::vector<KeyPoint> liveQueryKeypoints;
    Mat liveQueryDescriptors;

    int sift_detector(Mat *im);


public slots:
    void capture();
private slots:
    void findImageFile();
    void toggleLiveCam();
    void editCamAddr();
};

#endif // MAINWINDOW_H
