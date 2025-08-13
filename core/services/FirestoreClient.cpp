#include "FirestoreClient.h"
#include "repo/FirestoreMapper.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QEventLoop>
#include <QTimer>
#include <QUrlQuery>
#include <QDateTime>
#include <QDebug>

FirestoreClient::FirestoreClient(const FirestoreConfig& config, QObject* parent)
    : QObject(parent), m_config(config), m_networkManager(std::make_unique<QNetworkAccessManager>(this)) {
}

bool FirestoreClient::setPresence(const QString& userId, const QString& name,
                                 const QString& status, int64_t millisSinceEpoch) {
    QString url = buildDocumentUrl("presence", userId);
    QNetworkRequest request = createRequest(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonObject presenceDoc = FirestoreMapper::presenceToFirestoreDocument(name, status, millisSinceEpoch);
    QByteArray data = QJsonDocument(presenceDoc).toJson();
    
    QByteArray response;
    return performSynchronousRequest(request, data, "PATCH", response);
}

std::vector<User> FirestoreClient::listPresence() {
    QString url = buildCollectionUrl("presence");
    QNetworkRequest request = createRequest(url);
    
    QByteArray response;
    if (!performSynchronousRequest(request, QByteArray(), "GET", response)) {
        return {};
    }
    
    return FirestoreMapper::parsePresenceCollection(response);
}

bool FirestoreClient::postMessage(const QString& roomId, const Message& message) {
    QString collectionPath = QString("rooms/%1/messages").arg(roomId);
    QString url = buildCollectionUrl(collectionPath);
    
    QNetworkRequest request = createRequest(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonObject messageDoc = FirestoreMapper::messageToFirestoreDocument(message);
    QByteArray data = QJsonDocument(messageDoc).toJson();
    
    QByteArray response;
    return performSynchronousRequest(request, data, "POST", response);
}

std::vector<Message> FirestoreClient::listMessages(const QString& roomId, int64_t sinceMs) {
    QString collectionPath = QString("rooms/%1/messages").arg(roomId);
    QString url = buildCollectionUrl(collectionPath);
    QNetworkRequest request = createRequest(url);
    
    QByteArray response;
    if (!performSynchronousRequest(request, QByteArray(), "GET", response)) {
        return {};
    }
    
    return FirestoreMapper::parseMessageCollection(response, sinceMs);
}

QString FirestoreClient::buildDocumentUrl(const QString& collectionPath, const QString& documentId) const {
    QString baseUrl = QString("https://firestore.googleapis.com/v1/projects/%1/databases/%2/documents/%3")
                      .arg(m_config.projectId, m_config.databaseId, collectionPath);
    
    if (!documentId.isEmpty()) {
        baseUrl += "/" + documentId;
    }
    
    return baseUrl;
}

QString FirestoreClient::buildCollectionUrl(const QString& collectionPath) const {
    return QString("https://firestore.googleapis.com/v1/projects/%1/databases/%2/documents/%3")
           .arg(m_config.projectId, m_config.databaseId, collectionPath);
}

QNetworkRequest FirestoreClient::createRequest(const QString& url) const {
    QUrl urlObj(url);
    QUrlQuery query;
    query.addQueryItem("key", m_config.apiKey);
    urlObj.setQuery(query);
    
    return QNetworkRequest(urlObj);
}

bool FirestoreClient::performSynchronousRequest(QNetworkRequest request, const QByteArray& data,
                                               const QString& method, QByteArray& response) {
    QEventLoop loop;
    QNetworkReply* reply = nullptr;
    
    if (method == "GET") {
        reply = m_networkManager->get(request);
    } else if (method == "POST") {
        reply = m_networkManager->post(request, data);
    } else if (method == "PATCH") {
        reply = m_networkManager->sendCustomRequest(request, "PATCH", data);
    } else {
        return false;
    }
    
    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);
    timeoutTimer.setInterval(10000);
    
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    connect(&timeoutTimer, &QTimer::timeout, &loop, &QEventLoop::quit);
    
    timeoutTimer.start();
    loop.exec();
    
    bool success = false;
    if (timeoutTimer.isActive()) {
        timeoutTimer.stop();
        if (reply->error() == QNetworkReply::NoError) {
            response = reply->readAll();
            success = true;
        } else {
            qWarning() << "Network error:" << reply->errorString() << "HTTP status:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            qWarning() << "Response:" << reply->readAll();
        }
    } else {
        qWarning() << "Request timed out";
    }
    
    reply->deleteLater();
    return success;
}