#include "photoposter.h"
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QFile>
#include <QNetworkReply>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>

PhotoPoster::PhotoPoster(QObject *parent)
    : QObject(parent)
    , m_manager(new QNetworkAccessManager())
    , m_isFinished(false)
    , m_hasError(false)
    , m_status("")
    , m_code(0)
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

    connect(reply, SIGNAL(readyRead()), this, SLOT(readResult()));
}

void PhotoPoster::readResult()
{
    QObject *object = sender();
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(object);
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << reply->errorString();
    }
    QByteArray result = reply->readAll();
    QJsonObject resultObject = QJsonDocument::fromJson(result).object();

    m_hasError = resultObject.value(QString("status")).toString() == QString("error") ? true : false;

    m_code = resultObject.value(QString("code")).toDouble();

    QJsonArray array = resultObject.value(QString("result")).toArray();
    m_status = array.at(0).toString();

    m_isFinished = true;
    emit responseFinished(resultObject);
}

bool PhotoPoster::isFinished()
{
    return m_isFinished;
}

bool PhotoPoster::hasError()
{
    return m_hasError;
}

QString PhotoPoster::status()
{
    return m_status;
}

double PhotoPoster::code()
{
    return m_code;
}
