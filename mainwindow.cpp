#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include "dialogeditaddr.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    timer.setInterval(1); //~59Hz
    timer.setSingleShot(true); //Use single shot to avoid re-entering function, just a OCD ;)
    connect(&timer, SIGNAL(timeout()), this, SLOT(capture()));
    camAddr = "rtsp://192.168.1.99/tcp_live/ch0_0";
    connect(ui->actionEndereco_da_camera, SIGNAL(triggered(bool)), this, SLOT(editCamAddr()));

    detector = ORB::create();
    matcher = DescriptorMatcher::create("BruteForce-Hamming");
//    featureDetector = Ptr<FeatureDetector>(ORB::create());
//    descriptorExtractor = Ptr<DescriptorExtractor>(ORB::create());
//    descriptorMatcher = BFMatcher::BFMatcher(NORM_HAMMING);

    bool isCreated = !( detector==NULL || matcher==NULL/* || descriptorMatcher.empty() */);
    if(!isCreated)
    {
        QMessageBox::warning(this, "Error", "Can't create feature detector or descriptor extractor or descriptor matcher.", QMessageBox::Ok);
        return;
    }

    liveIsRunning = false;
    //ui->pushButtonStartCam->setEnabled(false);
    connect(ui->toolButtonTemplateLive, SIGNAL(clicked(bool)), this, SLOT(findImageFile()));
    connect(ui->pushButtonStartCam, SIGNAL(clicked(bool)), this, SLOT(toggleLiveCam()));
}

MainWindow::~MainWindow()
{
    if(cap.isOpened())
    {
        cap.release();
    }
    delete ui;
}

int MainWindow::sift_detector(Mat *im)
{
    Mat image = *im;
    cvtColor(image, image, COLOR_BGR2GRAY);

    std::vector<KeyPoint> keypoints;
    Mat descriptors;

    detector->detectAndCompute(image, Mat(), keypoints, descriptors);
    detector->detectAndCompute(liveImageTemplate, Mat(), liveQueryKeypoints, liveQueryDescriptors);
    //featureDetector->detect(image, keypoints);
    if(keypoints.size() == 0)
    {
        return 0;
    }
    //descriptorExtractor->compute(image, keypoints, descriptors);

//    std::vector< std::vector< DMatch > > allMatches;
//    descriptorMatcher->knnMatch(queryDescriptors, descriptors, allMatches, 2);

//    std::vector< DMatch > allGoodMatches;
//    const float minRatio = 1.f / 1.4f;
//    for(int m=0;m<allMatches.size();m++)
//    {
//        if(allMatches[m].size() > 0)
//        {
//            const DMatch& bestMatch = allMatches[m][0];
//            const DMatch& betterMatch = allMatches[m][1];
//            float distanceRatio = bestMatch.distance / betterMatch.distance;
//            if (distanceRatio < minRatio)
//            {
//                allGoodMatches.push_back(bestMatch);
//            }
//        }
//    }

    std::vector<std::vector<cv::DMatch> > allMatches;
    matcher->knnMatch(liveQueryDescriptors, descriptors, allMatches, 2);
    //descriptorMatcher.knnMatch(liveQueryDescriptors, descriptors, allMatches, 2);

    std::vector< DMatch > allGoodMatches;

    for(unsigned int i=0; i<allMatches.size(); ++i)
    {
        // Apply NNDR
        if(allMatches.at(i).size() == 2 && allMatches.at(i).at(0).distance <= 0.8 * allMatches.at(i).at(1).distance)
        {
            allGoodMatches.push_back(allMatches.at(i).at(0));
        }
    }

    /*for(int m=0;m<allMatches.size();m++)
    {
        if(allMatches[m].size() > 0)
        {
            if (allMatches[m][0].distance < 0.75*allMatches[m][1].distance)
            {
                allGoodMatches.push_back(allMatches[m][0]);
            }
        }
    }*/

    if(allGoodMatches.size() > 0)
    {
        Mat img_matches;
        drawMatches( liveImageTemplate, liveQueryKeypoints, image, keypoints, allGoodMatches, img_matches);
        img_matches.copyTo(*im);
    }
    return allGoodMatches.size();

//    std::vector< DMatch > allGoodMatches;
//    const float minRatio = 1.f / 1.6f;
//    for(int m=0;m<allMatches.size();m++)
//    {
//        if(allMatches[m].size() > 0)
//        {
//            const DMatch& bestMatch = allMatches[m][0];
//            const DMatch& betterMatch = allMatches[m][1];
//            float distanceRatio = bestMatch.distance / betterMatch.distance;
//            if (distanceRatio < minRatio)
//            {
//                allGoodMatches.push_back(bestMatch);
//            }
//        }
//    }

//    return allGoodMatches.size();

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

void MainWindow::toggleLiveCam()
{
    if(ui->pushButtonStartCam->text() == "Start")
    {
        ui->pushButtonStartCam->setEnabled(false);
        ui->camFrame_2->setScene(&scene);

        if(ui->actionWebcam->isChecked())
        {
            if(!cap.open(0))
            {
                QMessageBox::warning(this, "Erro", "Erro ao abrir webcam!", QMessageBox::Ok);
                ui->pushButtonStartCam->setEnabled(true);
                return;
            }
        }
        else
        {
            if(!cap.open(camAddr.toStdString(), CV_CAP_FFMPEG))
            {
                QMessageBox::warning(this, "Erro", "Erro ao abrir câmera!", QMessageBox::Ok);
                ui->pushButtonStartCam->setEnabled(true);
                return;
            }
        }
        if(!cap.isOpened())
        {
            QMessageBox::warning(this, "Erro", "Transmissão inválida!", QMessageBox::Ok);
            ui->pushButtonStartCam->setEnabled(true);
            return;
        }

        liveIsRunning = true;
        timer.start();
        ui->pushButtonStartCam->setText("Stop");
        ui->pushButtonStartCam->setEnabled(true);
        ui->toolButtonTemplateLive->setEnabled(false);
    }
    else
    {
        ui->pushButtonStartCam->setEnabled(false);
        liveIsRunning = false;
        ui->pushButtonStartCam->setText("Start");
    }
}

void MainWindow::editCamAddr()
{
    DialogEditAddr *dlg = new DialogEditAddr(camAddr, this);
    if(dlg->exec() == QDialog::Accepted)
        camAddr = dlg->getLineEditText();
    dlg->deleteLater();
}

void MainWindow::findImageFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Image"),"",tr("Images (*.png *.jpg *.bmp *.jpeg)"));
    if(fileName.isEmpty())
        return;
    QToolButton *sender = (QToolButton*)QObject::sender();
#ifdef _WIN32
    IplImage* img1 = cvLoadImage(fileName.toStdString().c_str());
    liveImageTemplate = Mat(img1);
#elif __MACH__ || __UNIX__
    //IplImage* img1 = cvLoadImage(fileName.toStdString().c_str());
    liveImageTemplate = imread(fileName.toStdString());
#endif
    if(liveImageTemplate.empty())
    {
        QMessageBox::warning(this, "Error", "The image cannot be loaded.", QMessageBox::Ok);
        ui->pushButtonStartCam->setEnabled(false);
        ui->pushButtonDetect->setEnabled(false);
    }
    else
    {
        if(sender == ui->toolButtonTemplateLive)
        {
            ui->lineEditTemplateLive->setText(fileName);
            cvtColor(liveImageTemplate, liveImageTemplate, COLOR_BGR2GRAY);
            detector->detectAndCompute(liveImageTemplate, Mat(), liveQueryKeypoints, liveQueryDescriptors);
            ui->pushButtonStartCam->setEnabled(true);
        }
    }
}

void MainWindow::capture()
{
    if(!cap.isOpened())
    {
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
    if(!liveImageTemplate.empty())
    {
        matches = sift_detector(&frame);
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
    ui->camFrame_2->setImage(QImage((const unsigned char*)(frame.data), frame.cols,frame.rows,QImage::Format_RGB888).rgbSwapped());

    if(liveIsRunning)
        timer.start();
    else
    {
        timer.stop();
        Mat i;
        cap.read(i);
        cap.release();
        ui->pushButtonStartCam->setEnabled(true);
        ui->toolButtonTemplateLive->setEnabled(true);
    }
}
