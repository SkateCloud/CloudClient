#ifndef PHOTOPOSTER_H
#define PHOTOPOSTER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QString>

class PhotoPoster : public QObject
{
    Q_OBJECT
public:
    explicit PhotoPoster(QObject *parent = nullptr);
    void post(QString filename);
signals:

public slots:
private:
    QNetworkAccessManager *m_manager;
};

#endif // PHOTOPOSTER_H
