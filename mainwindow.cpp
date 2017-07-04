#include "mainwindow.h"
#include <QDataStream>
#include <QFile>
#include <QHttpPart>
#include <QHttpMultiPart>
#include <QScopedPointer>
#include <QNetworkProxy>
#include <QMessageBox>
#include <QObject>
#include <QTimer>
#include <QJsonArray>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDir>
#include <QProcess>

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

    //绑定事件
    connect(exit_button, SIGNAL(clicked(bool)), this, SLOT(cancelAutoUpload()));

    display_label = new QLabel();
    QGridLayout *mainlayout = new QGridLayout(this);

    widget->setLayout(mainlayout);

    mainlayout->addWidget(view_finder);
    mainlayout->addWidget(display_label);
    mainlayout->addWidget(capture_button);
    mainlayout->addWidget(save_button);
    mainlayout->addWidget(exit_button);

    connect(capture_button, &QPushButton::clicked, this, &MainWindow::captureImage);
    connect(save_button, &QPushButton::clicked, this, &MainWindow::saveImage);
    connect(camera_image_capture, &QCameraImageCapture::imageCaptured, this, &MainWindow::displayImage);

    camera_image_capture->setCaptureDestination(QCameraImageCapture::CaptureToFile);
    camera->setCaptureMode(QCamera::CaptureStillImage);
    camera->setViewfinder(view_finder);
    camera->start(); //启动摄像头

    // 开启定时器
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    timer->start(INTERATOR_TIME);

    //开启链接检测
    IPtimer = new QTimer(this);
    connect(IPtimer, SIGNAL(timeout()), this, SLOT(IPtimerUpdate()));
    IPtimer->start(1000);

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
    fileName = CONFIG_PATH;
    qint32 number = camera_image_capture->capture(fileName);
    fileName = fileName + "IMG_0000000";
    fileName.append(QString::number(number, 10));
    fileName.append(QString(".jpg"));
    imgName = "IMG_0000000" + QString::number(number, 10) + ".jpg";
    qDebug()<<imgName;
}

void MainWindow::UpLoadForm(QString Path,QMap<QString,QString> params,QString fileFormName,QFile *uploadFile,QString newFileName)
{
    PhotoPoster *poster = new PhotoPoster();
    poster->post(fileName, imgName);

    connect(poster, SIGNAL(responseFinished(QJsonObject)), this, SLOT(showResult(QJsonObject)));
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

/*
 * Path: 服务器的url地址
 * params: 普通的表单参数
 * fileFormName: 需要上传文件表单中的名字
 * uploadFile: 要上传的文件
 * newFileName: 服务器保存该文件时文件名
 */

void MainWindow::showResult(QJsonObject jsonObject)
{
    QMessageBox b;
    b.setText(jsonObject.value("result").toArray().at(0).toString());
    b.exec();
}
// 定时任务
void MainWindow::timerUpdate()
{

    QDir dir(CONFIG_PATH);
    if(!dir.exists())
    {
        return;
    }
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList list = dir.entryInfoList();

    int file_count = list.count();
    if(file_count <= 0)
    {
        return;
    }

    QStringList string_list;
    for(int i=0; i < list.length(); i++)
    {
        QFileInfo file_info = list.at(i);
        QString suffix = file_info.suffix();

        //超过一天就不上传
        QDateTime  file_create = file_info.created();
        QDateTime  current_date = QDateTime::currentDateTime();
        QDateTime  afterOneDayDate = file_create.addDays(1);

        if(QString::compare(suffix, QString("jpg"), Qt::CaseInsensitive) == 0, afterOneDayDate > current_date, AUTO_UPLOAD)
        {
            QString absolute_file_path = file_info.absoluteFilePath();
            string_list.append(absolute_file_path);

            PhotoPoster *poster = new PhotoPoster();
            poster->post(absolute_file_path, file_info.baseName());

            connect(poster, SIGNAL(responseFinished(QJsonObject)), this, SLOT(showResult(QJsonObject)));
        }
    }
}
void MainWindow::IPtimerUpdate()
{
    QString ip = "192.168.0.0";
    int exitCode;
    // 检测是否链接正常
#ifdef Q_OS_WIN
    QString strArg = "ping " + ip + " -n 1 -i 2";
    exitCode = QProcess::execute(strArg);
#else
    //其他平台(Linux或Mac)
    exitCode = QProcess::execute("ping",  QStringList() << "-c 1" << "-t 2" << ip));
#endif

    if(0 == exitCode)
    {
        //it's alive
        qDebug() << "shell ping " + ip + " sucessed!";
        //发射该IP在线的信号
        emit commandSuccessed(ip);
    } else {
        qDebug() << "shell ping " + ip + " failed!";
        //发射IP离线的信号
        emit commandFailed(ip);
    }
}

//取消自动上传
void MainWindow::cancelAutoUpload()
{

    qDebug()<<"key up";
    bool status = AUTO_UPLOAD;
    if (status) {
        exit_button->setText(QString("enable auto upload"));
        AUTO_UPLOAD = false;
        timer->stop();
    }else {
        exit_button->setText(QString("close auto upload"));
        AUTO_UPLOAD = true;
        timer->start(INTERATOR_TIME);
    }

}
//检测是否清晰
bool MainWindow::isFuzzy(QString filename)
{
    return true;
}

void MainWindow::commandSuccessed(QString ip)
{
    // qDebug() << ip;
}

void MainWindow::commandFailed(QString ip)
{
    // qDebug() << ip;
}
