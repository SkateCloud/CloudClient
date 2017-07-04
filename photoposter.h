#ifndef PHOTOPOSTER_H
#define PHOTOPOSTER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QString>
#include <QJsonObject>

class PhotoPoster : public QObject
{
    Q_OBJECT
public:
    explicit PhotoPoster(QObject *parent = nullptr);
    void post(QString filename, QString imgName);
    bool isFinished();
    bool hasError();
    QString status();
    double code();
signals:
    void responseFinished(QJsonObject);
public slots:
    void readResult();
private:
    QNetworkAccessManager *m_manager;
    bool m_isFinished;
    bool m_hasError;
    QString m_status;
    double m_code;
};

#endif // PHOTOPOSTER_H
