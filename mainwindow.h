#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QGraphicsScene>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/nonfree/nonfree.hpp>

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

    Mat liveImageTemplate;
    Ptr<FeatureDetector> featureDetector;
    Ptr<DescriptorExtractor> descriptorExtractor;
    BFMatcher descriptorMatcher;

    std::vector<KeyPoint> liveQueryKeypoints;
    Mat liveQueryDescriptors;

    int sift_detector(Mat *im);


public slots:
    void capture();
private slots:
    void findImageFile();
    void toggleLiveCam();
};

#endif // MAINWINDOW_H
