#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QUrlQuery>
#include <QFile>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QGridLayout>
#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QLabel>
#include <QPushButton>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMainWindow>
#include <QUuid>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
signals:

private slots:
    void saveImage();
    void captureImage();
    void translateLanguage();
    void displayImage(int id, QImage image);
    void cancelAutoUpload();
    void UpLoadForm(QString Path,QMap<QString,QString> params,QString fileFormName,QFile *uploadFile,QString newFileName);
    void showResult(QJsonObject jsonObject);
    void timerUpdate();
    bool isFuzzy(QString filename);
    void commandSuccessed(QString ip);
    void commandFailed(QString ip);
    void IPtimerUpdate();
private:
    Ui::MainWindow *ui;

    QCamera *camera;//系统摄像头设备
    QCameraViewfinder *view_finder;//摄像头取景器部件
    QCameraImageCapture *camera_image_capture;//截图部件

    QPushButton *capture_button;
    QPushButton *save_button;
    QPushButton *exit_button;
    QLabel *display_label;

    QString fileName;
    QString account;
    QString password;
    QString imgName;

    QTimer *timer;
    QTimer *IPtimer;

    const QString CONFIG_PATH = "G://QTimages/";
    bool AUTO_UPLOAD = true;
    int INTERATOR_TIME = 5*1000;

};

#endif // MAINWINDOW_H
