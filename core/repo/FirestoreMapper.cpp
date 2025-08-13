#include "FirestoreMapper.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <algorithm>

QJsonObject FirestoreMapper::presenceToFirestoreDocument(const QString& name, const QString& status, int64_t lastHeartbeat) {
    QJsonObject doc;
    QJsonObject fields;
    
    fields["name"] = createFirestoreValue("stringValue", name);
    fields["status"] = createFirestoreValue("stringValue", status);
    fields["lastHeartbeat"] = createFirestoreValue("integerValue", QString::number(lastHeartbeat));
    
    doc["fields"] = fields;
    return doc;
}

QJsonObject FirestoreMapper::messageToFirestoreDocument(const Message& message) {
    QJsonObject doc;
    QJsonObject fields;
    
    fields["author"] = createFirestoreValue("stringValue", message.author);
    fields["name"] = createFirestoreValue("stringValue", message.authorName);
    fields["text"] = createFirestoreValue("stringValue", message.text);
    fields["ts"] = createFirestoreValue("integerValue", QString::number(message.timestamp));
    
    doc["fields"] = fields;
    return doc;
}

std::vector<User> FirestoreMapper::parsePresenceCollection(const QByteArray& jsonData) {
    std::vector<User> users;
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &error);
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse presence collection:" << error.errorString();
        return users;
    }
    
    QJsonObject root = doc.object();
    QJsonArray documents = root["documents"].toArray();
    
    for (const auto& docValue : documents) {
        QJsonObject docObj = docValue.toObject();
        QString fullName = docObj["name"].toString();
        
        int lastSlash = fullName.lastIndexOf('/');
        if (lastSlash == -1) continue;
        
        QString userId = fullName.mid(lastSlash + 1);
        Presence presence = parsePresenceDocument(docObj, userId);
        
        User user(presence.userId, presence.name);
        user.status = presence.status == "online" ? UserStatus::Online : UserStatus::Offline;
        user.lastSeen = presence.lastHeartbeat;
        
        users.push_back(user);
    }
    
    return users;
}

std::vector<Message> FirestoreMapper::parseMessageCollection(const QByteArray& jsonData, int64_t sinceMs) {
    std::vector<Message> messages;
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &error);
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse message collection:" << error.errorString();
        return messages;
    }
    
    QJsonObject root = doc.object();
    QJsonArray documents = root["documents"].toArray();
    
    for (const auto& docValue : documents) {
        QJsonObject docObj = docValue.toObject();
        QString fullName = docObj["name"].toString();
        
        int lastSlash = fullName.lastIndexOf('/');
        if (lastSlash == -1) continue;
        
        QString messageId = fullName.mid(lastSlash + 1);
        Message message = parseMessageDocument(docObj, messageId);
        
        if (message.timestamp > sinceMs) {
            messages.push_back(message);
        }
    }
    
    std::sort(messages.begin(), messages.end());
    return messages;
}

Presence FirestoreMapper::parsePresenceDocument(const QJsonObject& doc, const QString& userId) {
    QJsonObject fields = doc["fields"].toObject();
    
    QString name = extractStringValue(fields["name"].toObject());
    QString status = extractStringValue(fields["status"].toObject());
    int64_t lastHeartbeat = extractIntValue(fields["lastHeartbeat"].toObject());
    
    return Presence(userId, name, status, lastHeartbeat);
}

Message FirestoreMapper::parseMessageDocument(const QJsonObject& doc, const QString& documentId) {
    QJsonObject fields = doc["fields"].toObject();
    
    Message message;
    message.id = documentId;
    message.author = extractStringValue(fields["author"].toObject());
    message.authorName = extractStringValue(fields["name"].toObject());
    message.text = extractStringValue(fields["text"].toObject());
    message.timestamp = extractIntValue(fields["ts"].toObject());
    
    return message;
}

QJsonObject FirestoreMapper::createFirestoreValue(const QString& type, const QString& value) {
    QJsonObject firestoreValue;
    firestoreValue[type] = value;
    return firestoreValue;
}

QString FirestoreMapper::extractStringValue(const QJsonObject& firestoreValue) {
    return firestoreValue["stringValue"].toString();
}

int64_t FirestoreMapper::extractIntValue(const QJsonObject& firestoreValue) {
    return firestoreValue["integerValue"].toString().toLongLong();
}