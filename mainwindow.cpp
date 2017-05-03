#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include "dialogeditaddr.h"
#include "possiblechar.h"
#include "detectplates.h"
#include "detectchars.h"
#include "possibleplate.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    timer.setInterval(1); //~59Hz
    timer.setSingleShot(true); //Use single shot to avoid re-entering function, just a OCD ;)
    connect(&timer, SIGNAL(timeout()), this, SLOT(capture()));
    camAddr = "/Users/bviecelli/opencv-webcam-test/video_up.mp4";
    connect(ui->actionEndereco_da_camera, SIGNAL(triggered(bool)), this, SLOT(editCamAddr()));

    liveIsRunning = false;
    connect(ui->pushButtonStartCam, SIGNAL(clicked(bool)), this, SLOT(toggleLiveCam()));

    bool blnKNNTrainingSuccessful = loadKNNDataAndTrainKNN();           // attempt KNN training

    if (blnKNNTrainingSuccessful == false) {                            // if KNN training was not successful
                                                                        // show error message
        std::cout << std::endl << std::endl << "error: error: KNN traning was not successful" << std::endl << std::endl;
    }

#ifdef USE_CASCADE_DETECTION
    block_size = 1.3;
    ui->doubleSpinBox->setValue(block_size);
    connect(ui->doubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateParam(double)));
    cv::String st = QCoreApplication::applicationDirPath().toStdString()+"/../../../cars2.xml";
    if(!cascade.load(st))
    {
        qDebug("Error creating cascade object.\r\n");
    }
    assert(&cascade);
    cla = createCLAHE(2.0);
#else
    ui->doubleSpinBox->setHidden(true);
#endif
    ocr = new tesseract::TessBaseAPI();
    // Initialize tesseract-ocr with English, without specifying tessdata path
    if (ocr->Init(NULL, "lbr")) {

    }
    whiteList = LPR_WHITE_LIST;
    if(ocr->SetVariable("tessedit_char_whitelist", whiteList.toStdString().c_str()))
    {
        qDebug("White List setup done.\r\n");
    }
    ocr->SetPageSegMode(tesseract::PSM_SINGLE_WORD);
    ocr->SetVariable("textord_old_xheight", "1");
    ocr->SetVariable("textord_min_xheight", "20");
    ocr->SetVariable("textord_max_noise_size", "25");
}

MainWindow::~MainWindow()
{
    if(cap.isOpened())
    {
        cap.release();
    }
    delete ui;
}

int MainWindow::detectLicensePlate(Mat *im)
{
#ifdef USE_CASCADE_DETECTION
    Mat imgPlate;
    cvtColor(*im, imgPlate, CV_BGR2GRAY);
    cla->apply(imgPlate, imgPlate);
    std::vector<PossiblePlate> vectorOfPossiblePlates = detectPlatesInScene(*im);

    for(auto &possiblePlate : vectorOfPossiblePlates)
    {
        cv::Point2f p2fRectPoints[4];
        possiblePlate.rrLocationOfPlateInScene.points(p2fRectPoints);            // get 4 vertices of rotated rect
        Mat imgPlate = possiblePlate.imgPlate;
        Mat imgHSV;
        std::vector<Mat> vectorOfHSVImages;
        cvtColor(imgPlate, imgHSV, CV_BGR2HSV);
        split(imgHSV, vectorOfHSVImages);
        imgPlate = vectorOfHSVImages[2];
        Mat imgTopHat;
        Mat imgBlackHat;
        Mat element = getStructuringElement(CV_SHAPE_RECT, Size(3, 3));
        morphologyEx(imgPlate, imgTopHat, CV_MOP_TOPHAT, element);
        morphologyEx(imgPlate, imgBlackHat, CV_MOP_BLACKHAT, element);
        imgPlate = (imgPlate + imgTopHat) - imgBlackHat;
        GaussianBlur(imgPlate, imgPlate, Size(5,5), 0);
        adaptiveThreshold(imgPlate, imgPlate, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 75, 10);

        ocr->SetImage((uchar*)imgPlate.data, imgPlate.size().width, imgPlate.size().height, imgPlate.channels(), imgPlate.step1());
        ocr->Recognize(0);
        cv::String plate(ocr->GetUTF8Text());
        ocr->Clear();
        qDebug(plate.c_str());
        imshow("plate", imgPlate);
        waitKey(0);
    }

#else
    std::vector<PossiblePlate> vectorOfPossiblePlates = detectPlatesInScene(*im);

    for(auto &possiblePlate : vectorOfPossiblePlates)
    {
        cv::Point2f p2fRectPoints[4];
        possiblePlate.rrLocationOfPlateInScene.points(p2fRectPoints);            // get 4 vertices of rotated rect
        //PossiblePlate::imgPlate

//        Mat imgPlate = possiblePlate.imgPlate;
//        medianBlur(imgPlate, imgPlate, 3);
//        cvtColor(imgPlate, imgPlate, CV_BGR2GRAY);
//        blur(imgPlate, imgPlate, Size(5,5));
//        Sobel(imgPlate, imgPlate, CV_8U, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
//        threshold(imgPlate, imgPlate, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY);
//        Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
//        morphologyEx(imgPlate, imgPlate, CV_MOP_CLOSE, element);

        Mat imgPlate = possiblePlate.imgPlate;
        Mat imgHSV;
        std::vector<Mat> vectorOfHSVImages;
        cvtColor(imgPlate, imgHSV, CV_BGR2HSV);
//        blur(imgPlate, imgPlate, Size(5,5));
//        Sobel(imgPlate, imgPlate, CV_8U, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
        split(imgHSV, vectorOfHSVImages);
        imgPlate = vectorOfHSVImages[2];
        Mat imgTopHat;
        Mat imgBlackHat;
        Mat element = getStructuringElement(CV_SHAPE_RECT, Size(3, 3));
        morphologyEx(imgPlate, imgTopHat, CV_MOP_TOPHAT, element);
        morphologyEx(imgPlate, imgBlackHat, CV_MOP_BLACKHAT, element);
        imgPlate = (imgPlate + imgTopHat) - imgBlackHat;
        GaussianBlur(imgPlate, imgPlate, Size(5,5), 0);
        adaptiveThreshold(imgPlate, imgPlate, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 75, 10);

//        imgPlate = maximizeContrast(imgPlate);
//        GaussianBlur(imgPlate, imgPlate, GAUSSIAN_SMOOTH_FILTER_SIZE, 0);
//        adaptiveThreshold(imgPlate, imgPlate, 255.0, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV, ADAPTIVE_THRESH_BLOCK_SIZE, ADAPTIVE_THRESH_WEIGHT);

        ocr->SetImage((uchar*)imgPlate.data, imgPlate.size().width, imgPlate.size().height, imgPlate.channels(), imgPlate.step1());
        ocr->Recognize(0);
        cv::String plate(ocr->GetUTF8Text());
        ocr->Clear();
        qDebug(plate.c_str());
        imshow("plate", imgPlate);
        waitKey(0);

        /*for (int i = 0; i < 4; i++) {                                       // draw 4 red lines
            cv::line(*im, p2fRectPoints[i], p2fRectPoints[(i + 1) % 4], SCALAR_RED, 2);
        }*/
    }
#endif

    return 0;
}

double MainWindow::licensePlateFound(Mat *img, QString *licensePlate)
{
    return 0;
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
    }
    else
    {
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

void MainWindow::updateParam(double n)
{
    //if(n % 2 == 1)
        block_size = n;
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

#ifdef USE_CASCADE_DETECTION
    Mat gs_frame;
    Mat frame_gray;
    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );
    std::vector<Rect> cars;

    cascade.detectMultiScale(frame_gray, cars, block_size, 3, CV_HAAR_DO_CANNY_PRUNING, Size(100, 40));

    int area = 0;
    int pos = 0;
    for(int i = 0 ; i < cars.size() ; i++)
    {
        rectangle(frame, cars[i], SCALAR_GREEN, 2);
        if(cars[i].area() > area)
        {
            area = cars[i].area();
            pos = i;
        }
    }

    // Crop the full image to that image contained by the rectangle myROI
    // Note that this doesn't copy the data
    if(cars.size() > 0)
    {
        getRectSubPix(frame, Size(cars[pos].width, cars[pos].height), Point2f(cars[pos].x+cars[pos].width/2, cars[pos].y+cars[pos].height/2), gs_frame);

        imshow("cropped", gs_frame);
        //waitKey(0);

        detectLicensePlate(&gs_frame);
    }
#else
    detectLicensePlate(&frame);
#endif
    ui->camFrame_2->setImage(QImage((const unsigned char*)(frame.data), frame.cols,frame.rows,QImage::Format_RGB888).rgbSwapped());

    if(liveIsRunning)
        timer.start();
    else
    {
        cap.release();
        timer.stop();
    }
}
