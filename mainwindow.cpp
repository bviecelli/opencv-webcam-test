#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    timer.setInterval(17); //~59Hz
    timer.setSingleShot(true); //Use single shot to avoid re-entering function, just a OCD ;)
    connect(&timer, SIGNAL(timeout()), this, SLOT(capture()));

    ui->camFrame->setScene(&scene);

    //cap.set(CV_CAP_PROP_FRAME_WIDTH, ui->camViewer->width());
    //cap.set(CV_CAP_PROP_FRAME_HEIGHT, ui->camViewer->height());

    if(!cap.open(0))
        QMessageBox::warning(this, "Erro", "Erro ao abrir webcam!", QMessageBox::Ok);
    if(!cap.isOpened())
        QMessageBox::warning(this, "Erro", "Erro ao abrir webcam!", QMessageBox::Ok);


    IplImage* img1 = cvLoadImage("box_in_scene.png");
    image_template = Mat(img1);
    cvtColor(image_template, image_template, COLOR_BGR2GRAY);
    timer.start();
}

MainWindow::~MainWindow()
{
    if(cap.isOpened())
    {
        cap.release();
    }
    delete ui;
}

int MainWindow::sift_detector(Mat image)
{
    Mat img1;
    cvtColor(image, img1, COLOR_BGR2GRAY);

    SiftFeatureDetector detector;
    SiftDescriptorExtractor extractor;

    std::vector<KeyPoint> keypoints1;
    detector.detect(img1, keypoints1);
    Mat descriptors1;
    extractor.compute(img1, keypoints1, descriptors1);

    std::vector<KeyPoint> keypoints2;
    detector.detect(image_template, keypoints2);
    Mat descriptors2;
    extractor.compute(image_template, keypoints2, descriptors2);

    FlannBasedMatcher matcher;
    std::vector< DMatch > matches;
    matcher.match(descriptors1, descriptors2, matches);
    //matcher.knnMatch(descriptors1, descriptors2, matches, 2);

    double max_dist = 0; double min_dist = 100;

    //-- Quick calculation of max and min distances between keypoints
    for( int i = 0; i < descriptors1.rows; i++ )
    {
        double dist = matches[i].distance;
        if( dist < min_dist )
            min_dist = dist;
        if( dist > max_dist )
            max_dist = dist;
    }
    std::vector< DMatch > goodMatches;
    for( int i = 0; i < descriptors1.rows; i++ )
    {
        if( matches[i].distance <= max(2*min_dist, 0.02) )
        {
            goodMatches.push_back( matches[i]);
        }
    }
    return goodMatches.size();
}

void MainWindow::capture()
{
    if(!cap.isOpened())
        return;
    Mat frame;

    cap >> frame;
    if(frame.empty())
        return;

    flip(frame, frame, 1);

    int matches = -1;
    if(image_template.empty())
    {
        matches = sift_detector(frame);
        putText(frame, QString::number(matches).toStdString(), Point(450,450), FONT_HERSHEY_COMPLEX, 2, Scalar(0,255,0), 1);
    }
    else
    {
        putText(frame, "Image Template not loaded", Point2f(100,100), FONT_HERSHEY_PLAIN, 2, Scalar(0,0,255,255));
    }
    ui->camFrame->setImage(QImage((const unsigned char*)(frame.data), frame.cols,frame.rows,QImage::Format_RGB888).rgbSwapped());

    timer.start();
}
