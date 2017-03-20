#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <opencv2/imgproc/imgproc.hpp>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    timer.setInterval(1); //~59Hz
    timer.setSingleShot(true); //Use single shot to avoid re-entering function, just a OCD ;)
    connect(&timer, SIGNAL(timeout()), this, SLOT(capture()));

    ui->camFrame->setScene(&scene);

    //cap.set(CV_CAP_PROP_FRAME_WIDTH, ui->camViewer->width());
    //cap.set(CV_CAP_PROP_FRAME_HEIGHT, ui->camViewer->height());

    if(!cap.open(0))
        QMessageBox::warning(this, "Erro", "Erro ao abrir webcam!", QMessageBox::Ok);
    if(!cap.isOpened())
        QMessageBox::warning(this, "Erro", "Erro ao abrir webcam!", QMessageBox::Ok);

    featureDetector = new SiftFeatureDetector;
    descriptorExtractor = new SiftDescriptorExtractor;
    descriptorMatcher = new FlannBasedMatcher;

    bool isCreated = !( featureDetector.empty() || descriptorExtractor.empty() || descriptorMatcher.empty() );
    if(!isCreated)
    {
        QMessageBox::warning(this, "Error", "Can't create feature detector or descriptor extractor or descriptor matcher.", QMessageBox::Ok);
        return;
    }

#ifdef _WIN32
    IplImage* img1 = cvLoadImage("C:\\GIT\\opencv-webcam-test\\box_in_scene.png");
    image_template = Mat(img1);
#elif
    IplImage* img1 = cvLoadImage("/Users/bviecelli/opencv-webcam-test/box_in_scene.png");
    image_template = Mat(img1);
#endif
    if(image_template.empty())
    {
        QMessageBox::warning(this, "Error", "The template image cannot be loaded.", QMessageBox::Ok);
    }
    else
    {
        cvtColor(image_template, image_template, COLOR_BGR2GRAY);
        featureDetector->detect(image_template, queryKeypoints);
        descriptorExtractor->compute(image_template, queryKeypoints, queryDescriptors);
        imshow("Query Image", image_template);
    }

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
    Mat frame;
    cvtColor(image, frame, COLOR_BGR2GRAY);

    std::vector<KeyPoint> keypoints;
    Mat descriptors;

    featureDetector->detect(frame, keypoints);
    if(keypoints.size() == 0)
    {
        return 0;
    }
    descriptorExtractor->compute(frame, keypoints ,descriptors);

    std::vector< std::vector< DMatch > > allMatches;
    descriptorMatcher->knnMatch(queryDescriptors, descriptors, allMatches, 2);

    std::vector< DMatch > allGoodMatches;
    const float minRatio = 1.f / 1.5f;
    for(int m=0;m<allMatches.size();m++)
    {
        if(allMatches[m][0].distance < 0.7*allMatches[m][1].distance)
        {
            const DMatch& bestMatch = allMatches[m][0];
            const DMatch& betterMatch = allMatches[m][1];
            float distanceRatio = bestMatch.distance / betterMatch.distance;
            if (distanceRatio < minRatio)
            {
                allGoodMatches.push_back(bestMatch);
            }
        }
    }

    return allGoodMatches.size();

//    std::vector< DMatch > matches;
//    matcher.match(descriptors1, descriptors2, matches);
//    double max_dist = 0; double min_dist = 100;

//    //-- Quick calculation of max and min distances between keypoints
//    for( int i = 0; i < descriptors1.rows; i++ )
//    {
//        double dist = matches[i].distance;
//        if( dist < min_dist )
//            min_dist = dist;
//        if( dist > max_dist )
//            max_dist = dist;
//    }
//    std::vector< DMatch > goodMatches;
//    for( int i = 0; i < descriptors1.rows; i++ )
//    {
//        if( matches[i].distance <= max(2*min_dist, 0.02) )
//        {
//            goodMatches.push_back( matches[i]);
//        }
//    }
//    return goodMatches.size();
}

void MainWindow::capture()
{
    if(!cap.isOpened())
    {
        timer.start();
        return;
    }
    Mat frame;

    cap >> frame;
    if(frame.empty())
    {
        timer.start();
        return;
    }

    flip(frame, frame, 1);

    int matches = -1;
    if(!image_template.empty())
    {
        matches = sift_detector(frame);
        putText(frame, QString::number(matches).toStdString(), Point2f(100,100), FONT_HERSHEY_COMPLEX, 4, Scalar(0,255,0), 2);
    }
    else
    {
        putText(frame, "Image Template not loaded", Point2f(100,100), FONT_HERSHEY_COMPLEX, 4, Scalar(0,0,255,255), 2);
    }

    if(matches > 10)
    {
        putText(frame, "Object Found", Point2f(350,350), FONT_HERSHEY_COMPLEX, 4 , Scalar(0,255,0), 2);
    }
    ui->camFrame->setImage(QImage((const unsigned char*)(frame.data), frame.cols,frame.rows,QImage::Format_RGB888).rgbSwapped());

    timer.start();
}
