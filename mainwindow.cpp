#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <opencv2/imgproc/imgproc.hpp>

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

void MainWindow::capture()
{
    if(!cap.isOpened())
        return;
    Mat frame;

    cap >> frame;
    if(frame.empty())
        return;
    Mat gs_frame;
    cvtColor(frame, gs_frame, CV_BGR2GRAY);
    GaussianBlur(gs_frame, gs_frame, Size(5,5), 0);
    Canny(gs_frame, gs_frame, 10, 30);
    threshold(gs_frame, gs_frame, 70, 255, THRESH_BINARY_INV);
    ui->camFrame->setImage(QImage((const unsigned char*)(gs_frame.data), gs_frame.cols,gs_frame.rows,QImage::Format_Grayscale8).rgbSwapped());

    timer.start();
}
