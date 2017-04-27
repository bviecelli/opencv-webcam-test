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

    timer.setInterval(17); //~59Hz
    timer.setSingleShot(true); //Use single shot to avoid re-entering function, just a OCD ;)
    connect(&timer, SIGNAL(timeout()), this, SLOT(capture()));
    camAddr = "rtsp://192.168.1.241/tcp_live/ch0_0";
    connect(ui->actionEndereco_da_camera, SIGNAL(triggered(bool)), this, SLOT(editCamAddr()));

    liveIsRunning = false;
    connect(ui->pushButtonStartCam, SIGNAL(clicked(bool)), this, SLOT(toggleLiveCam()));
    morph_size = 10;
    ui->horizontalSlider->setValue(morph_size);
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(updateParam(int)));
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
    Mat element = getStructuringElement(0, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );
    morphologyEx( *im, *im, MORPH_TOPHAT, element );
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

void MainWindow::updateParam(int n)
{
    morph_size = n;
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
    Mat gs_frame;
    cvtColor(frame, gs_frame, CV_BGR2GRAY);
    detectLicensePlate(&gs_frame);
    ui->camFrame_2->setImage(QImage((const unsigned char*)(gs_frame.data), gs_frame.cols,gs_frame.rows,QImage::Format_Grayscale8).rgbSwapped());

    if(liveIsRunning)
        timer.start();
    else
    {
        cap.release();
        timer.stop();
    }
}
