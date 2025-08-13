#pragma once

#include "models/User.h"
#include "models/Message.h"
#include "models/Presence.h"
#include <QJsonObject>
#include <QByteArray>
#include <vector>

class FirestoreMapper {
public:
    static QJsonObject presenceToFirestoreDocument(const QString& name, const QString& status, int64_t lastHeartbeat);
    static QJsonObject messageToFirestoreDocument(const Message& message);
    
    static std::vector<User> parsePresenceCollection(const QByteArray& jsonData);
    static std::vector<Message> parseMessageCollection(const QByteArray& jsonData, int64_t sinceMs = 0);
    
    static Presence parsePresenceDocument(const QJsonObject& doc, const QString& userId);
    static Message parseMessageDocument(const QJsonObject& doc, const QString& documentId);
    
private:
    static QJsonObject createFirestoreValue(const QString& type, const QString& value);
    static QString extractStringValue(const QJsonObject& firestoreValue);
    static int64_t extractIntValue(const QJsonObject& firestoreValue);
};