#pragma once

#include "IFirestoreClient.h"
#include "utils/Clock.h"
#include <QObject>
#include <QTimer>
#include <QString>
#include <memory>

class PresenceService : public QObject {
    Q_OBJECT

public:
    explicit PresenceService(std::shared_ptr<IFirestoreClient> client, 
                           std::shared_ptr<IClock> clock = std::make_shared<SystemClock>(),
                           QObject* parent = nullptr);
    
    void start(const QString& userId, const QString& displayName, int heartbeatIntervalMs = 5000);
    void stop();
    
    bool isOnline(const QString& status, int64_t lastHeartbeat, int64_t staleThresholdMs) const;

signals:
    void heartbeatSent(bool success);
    void started(const QString& userId);
    void stopped();

private slots:
    void sendHeartbeat();

private:
    std::shared_ptr<IFirestoreClient> m_client;
    std::shared_ptr<IClock> m_clock;
    QTimer m_heartbeatTimer;
    QString m_currentUserId;
    QString m_currentDisplayName;
    bool m_isActive = false;
};