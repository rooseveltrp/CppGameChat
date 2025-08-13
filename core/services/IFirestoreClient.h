#pragma once

#include "models/User.h"
#include "models/Message.h"
#include <QString>
#include <vector>

struct FirestoreConfig {
    QString apiKey;
    QString projectId;
    QString databaseId = "(default)";
    
    FirestoreConfig() = default;
    FirestoreConfig(const QString& apiKey, const QString& projectId)
        : apiKey(apiKey), projectId(projectId) {}
};

class IFirestoreClient {
public:
    virtual ~IFirestoreClient() = default;
    
    virtual bool setPresence(const QString& userId, const QString& name,
                            const QString& status, int64_t millisSinceEpoch) = 0;
    
    virtual std::vector<User> listPresence() = 0;
    
    virtual bool postMessage(const QString& roomId, const Message& message) = 0;
    
    virtual std::vector<Message> listMessages(const QString& roomId, int64_t sinceMs) = 0;
};