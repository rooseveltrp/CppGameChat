#include "ChatService.h"

ChatService::ChatService(std::shared_ptr<IFirestoreClient> client,
                        std::shared_ptr<IClock> clock,
                        QObject* parent)
    : QObject(parent), m_client(client), m_clock(clock) {
    
    connect(&m_pollTimer, &QTimer::timeout, this, &ChatService::pollMessages);
}

void ChatService::startPolling(const QString& roomId, int intervalMs) {
    if (m_isPolling) {
        stopPolling();
    }
    
    m_currentRoomId = roomId;
    m_isPolling = true;
    m_lastFetchedTimestamp = 0; // Start from beginning to load existing messages
    
    fetchNewMessages(roomId);
    
    m_pollTimer.setInterval(intervalMs);
    m_pollTimer.start();
}

void ChatService::stopPolling() {
    m_pollTimer.stop();
    m_isPolling = false;
    m_currentRoomId.clear();
}

bool ChatService::sendMessage(const QString& roomId, const QString& author,
                             const QString& authorName, const QString& text) {
    if (!m_client || text.trimmed().isEmpty()) {
        return false;
    }
    
    Message message(roomId, author, authorName, text.trimmed());
    message.timestamp = m_clock->currentTimeMillis();
    
    bool success = m_client->postMessage(roomId, message);
    emit messageSent(success, message);
    
    return success;
}

void ChatService::fetchNewMessages(const QString& roomId) {
    if (!m_client) {
        return;
    }
    
    std::vector<Message> newMessages = m_client->listMessages(roomId, m_lastFetchedTimestamp);
    
    if (!newMessages.empty()) {
        // Update timestamp to the latest message timestamp + 1 to avoid duplicates
        for (const auto& msg : newMessages) {
            if (msg.timestamp > m_lastFetchedTimestamp) {
                m_lastFetchedTimestamp = msg.timestamp;
            }
        }
        m_lastFetchedTimestamp += 1; // Add 1ms to avoid fetching the same message again
        emit messagesReceived(newMessages);
    }
}

void ChatService::pollMessages() {
    if (m_isPolling && !m_currentRoomId.isEmpty()) {
        fetchNewMessages(m_currentRoomId);
    }
}