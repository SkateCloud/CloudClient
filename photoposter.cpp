#include "photoposter.h"
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QFile>
#include <QNetworkReply>

PhotoPoster::PhotoPoster(QObject *parent)
    : QObject(parent)
    , m_manager(new QNetworkAccessManager())
{

}

void PhotoPoster::post(QString filename)
{
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart accountPart;
    accountPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"account\""));
    accountPart.setBody("xiansenliu");

    QHttpPart passwordPart;
    passwordPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"password\""));
    passwordPart.setBody("password");

    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"image\"; filename=\"1A.jpg\""));

    QFile *file = new QFile(filename);
    file->open(QIODevice::ReadOnly);
    imagePart.setBodyDevice(file);
    file->setParent(multiPart);

    multiPart->append(accountPart);
    multiPart->append(passwordPart);
    multiPart->append(imagePart);

    QUrl url("http://192.168.2.2:8080/recognize");
    QNetworkRequest request(url);

    QNetworkReply *reply = m_manager->post(request, multiPart);
    multiPart->setParent(reply);
}
