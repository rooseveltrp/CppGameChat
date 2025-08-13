#pragma once

#include <QString>
#include <cstdint>

enum class UserStatus {
    Online,
    Offline
};

struct User {
    QString id;
    QString displayName;
    UserStatus status = UserStatus::Offline;
    int64_t lastSeen = 0;
    
    User() = default;
    User(const QString& id, const QString& displayName) 
        : id(id), displayName(displayName) {}
        
    bool isOnline() const { return status == UserStatus::Online; }
    
    bool operator==(const User& other) const {
        return id == other.id;
    }
    
    bool operator!=(const User& other) const {
        return !(*this == other);
    }
};