#include "mainwindow.h"
#include <QDataStream>
#include <QFile>
#include <QHttpPart>
#include <QHttpMultiPart>
#include <QScopedPointer>
#include <QNetworkProxy>

#include "photoposter.h"

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
account("10005"),
password("10005")
{
QWidget *widget = new QWidget();
this->setCentralWidget(widget);

camera = new QCamera();
view_finder = new QCameraViewfinder();
camera_image_capture = new QCameraImageCapture(camera);
capture_button = new QPushButton();
save_button = new QPushButton();
exit_button = new QPushButton();
display_label = new QLabel();
networkManager = new QNetworkAccessManager();

QGridLayout *mainlayout = new QGridLayout(this);

widget->setLayout(mainlayout);

mainlayout->addWidget(view_finder);
mainlayout->addWidget(display_label);
mainlayout->addWidget(capture_button);
mainlayout->addWidget(save_button);
mainlayout->addWidget(exit_button);

connect(capture_button, &QPushButton::clicked, this, &MainWindow::captureImage);
connect(save_button, &QPushButton::clicked, this, &MainWindow::saveImage);
connect(exit_button, &QPushButton::clicked, this, &MainWindow::stopUpload);
connect(camera_image_capture, &QCameraImageCapture::imageCaptured, this, &MainWindow::displayImage);

camera_image_capture->setCaptureDestination(QCameraImageCapture::CaptureToFile);
camera->setCaptureMode(QCamera::CaptureStillImage);
camera->setViewfinder(view_finder);
camera->start(); //启动摄像头

this->translateLanguage();
}

MainWindow::~MainWindow()
{

}
void MainWindow::translateLanguage()
{
this->setWindowTitle("车载监控软件");
capture_button->setText(tr("拍照"));
save_button->setText(tr("手动上传"));
exit_button->setText(tr("暂停自动上传"));
}

void MainWindow::displayImage(int id, QImage image)
{
display_label->setPixmap(QPixmap::fromImage(image));
}

void MainWindow::captureImage()
{
//截图
fileName = "/Users/mac/qtclient.jpg";
camera_image_capture->capture(fileName);

}

void MainWindow::UpLoadForm(QString Path,QMap<QString,QString> params,QString fileFormName,QFile *uploadFile,QString newFileName)
{
    PhotoPoster *poster = new PhotoPoster();
    poster->post("/Users/mac/qtclient.jpg");
}


void MainWindow::saveImage()
{
captureImage();
QString path="http://localhost:8080"; //服务器的url
QMap<QString,QString> params_send; //上传的普通参数 在本程序中 需要上传一个普通参数为"username"
params_send.insert("account",account);
params_send.insert("password", password);
QString fileFormName="image"; //上传文件表单中的名字

QFile *file = new QFile();

QString newFileName="1A.png";
UpLoadForm(path,params_send,fileFormName,file,newFileName);


}

void MainWindow::stopUpload() { }

/*
 * Path: 服务器的url地址
 * params: 普通的表单参数
 * fileFormName: 需要上传文件表单中的名字
 * uploadFile: 要上传的文件
 * newFileName: 服务器保存该文件时文件名
 */


void MainWindow::onFileUploadFinished(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << reply->errorString();
    }
    else {
        qDebug() << "No error";
    }
}
