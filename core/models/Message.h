#pragma once

#include <QString>
#include <cstdint>

struct Message {
    QString id;
    QString roomId;
    QString author;
    QString authorName;
    QString text;
    int64_t timestamp = 0;
    
    Message() = default;
    Message(const QString& roomId, const QString& author, const QString& authorName, const QString& text)
        : roomId(roomId), author(author), authorName(authorName), text(text) {}
        
    bool operator==(const Message& other) const {
        return id == other.id;
    }
    
    bool operator!=(const Message& other) const {
        return !(*this == other);
    }
    
    bool operator<(const Message& other) const {
        return timestamp < other.timestamp;
    }
};