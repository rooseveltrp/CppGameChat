#pragma once

#include <QString>
#include <cstdint>

struct Presence {
    QString userId;
    QString name;
    QString status;
    int64_t lastHeartbeat = 0;
    
    Presence() = default;
    Presence(const QString& userId, const QString& name, const QString& status, int64_t lastHeartbeat)
        : userId(userId), name(name), status(status), lastHeartbeat(lastHeartbeat) {}
        
    bool isOnline(int64_t currentTimeMs, int64_t staleThresholdMs) const {
        return status == "online" && (currentTimeMs - lastHeartbeat) < staleThresholdMs;
    }
    
    bool operator==(const Presence& other) const {
        return userId == other.userId;
    }
    
    bool operator!=(const Presence& other) const {
        return !(*this == other);
    }
};