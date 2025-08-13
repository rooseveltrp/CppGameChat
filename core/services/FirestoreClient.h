#pragma once

#include "IFirestoreClient.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <memory>

class FirestoreClient : public QObject, public IFirestoreClient {
    Q_OBJECT

public:
    explicit FirestoreClient(const FirestoreConfig& config, QObject* parent = nullptr);
    ~FirestoreClient() override = default;

    bool setPresence(const QString& userId, const QString& name,
                    const QString& status, int64_t millisSinceEpoch) override;
    
    std::vector<User> listPresence() override;
    
    bool postMessage(const QString& roomId, const Message& message) override;
    
    std::vector<Message> listMessages(const QString& roomId, int64_t sinceMs) override;

private:
    QString buildDocumentUrl(const QString& collectionPath, const QString& documentId = "") const;
    QString buildCollectionUrl(const QString& collectionPath) const;
    QNetworkRequest createRequest(const QString& url) const;
    
    bool performSynchronousRequest(QNetworkRequest request, const QByteArray& data, 
                                  const QString& method, QByteArray& response);

    FirestoreConfig m_config;
    std::unique_ptr<QNetworkAccessManager> m_networkManager;
};