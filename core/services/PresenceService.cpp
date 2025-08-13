#include "PresenceService.h"

PresenceService::PresenceService(std::shared_ptr<IFirestoreClient> client, 
                               std::shared_ptr<IClock> clock,
                               QObject* parent)
    : QObject(parent), m_client(client), m_clock(clock) {
    
    connect(&m_heartbeatTimer, &QTimer::timeout, this, &PresenceService::sendHeartbeat);
}

void PresenceService::start(const QString& userId, const QString& displayName, int heartbeatIntervalMs) {
    if (m_isActive) {
        stop();
    }
    
    m_currentUserId = userId;
    m_currentDisplayName = displayName;
    m_isActive = true;
    
    sendHeartbeat();
    
    m_heartbeatTimer.setInterval(heartbeatIntervalMs);
    m_heartbeatTimer.start();
    
    emit started(userId);
}

void PresenceService::stop() {
    if (!m_isActive) {
        return;
    }
    
    m_heartbeatTimer.stop();
    
    if (m_client && !m_currentUserId.isEmpty()) {
        int64_t currentTime = m_clock->currentTimeMillis();
        m_client->setPresence(m_currentUserId, m_currentDisplayName, "offline", currentTime);
    }
    
    m_isActive = false;
    m_currentUserId.clear();
    m_currentDisplayName.clear();
    
    emit stopped();
}

bool PresenceService::isOnline(const QString& status, int64_t lastHeartbeat, int64_t staleThresholdMs) const {
    if (status != "online") {
        return false;
    }
    
    int64_t currentTime = m_clock->currentTimeMillis();
    return (currentTime - lastHeartbeat) < staleThresholdMs;
}

void PresenceService::sendHeartbeat() {
    if (!m_isActive || m_currentUserId.isEmpty() || !m_client) {
        return;
    }
    
    int64_t currentTime = m_clock->currentTimeMillis();
    bool success = m_client->setPresence(m_currentUserId, m_currentDisplayName, "online", currentTime);
    
    emit heartbeatSent(success);
}