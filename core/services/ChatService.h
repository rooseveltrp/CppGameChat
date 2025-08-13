#pragma once

#include "IFirestoreClient.h"
#include "models/Message.h"
#include "utils/Clock.h"
#include <QObject>
#include <QTimer>
#include <QString>
#include <memory>

class ChatService : public QObject {
    Q_OBJECT

public:
    explicit ChatService(std::shared_ptr<IFirestoreClient> client,
                        std::shared_ptr<IClock> clock = std::make_shared<SystemClock>(),
                        QObject* parent = nullptr);
    
    void startPolling(const QString& roomId, int intervalMs = 1200);
    void stopPolling();
    
    bool sendMessage(const QString& roomId, const QString& author, 
                    const QString& authorName, const QString& text);
    
    void fetchNewMessages(const QString& roomId);
    
    int64_t getLastFetchedTimestamp() const { return m_lastFetchedTimestamp; }

signals:
    void messagesReceived(const std::vector<Message>& messages);
    void messageSent(bool success, const Message& message);

private slots:
    void pollMessages();

private:
    std::shared_ptr<IFirestoreClient> m_client;
    std::shared_ptr<IClock> m_clock;
    QTimer m_pollTimer;
    QString m_currentRoomId;
    int64_t m_lastFetchedTimestamp = 0;
    bool m_isPolling = false;
};