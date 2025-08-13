#include "FriendsService.h"
#include <algorithm>

FriendsService::FriendsService(std::shared_ptr<IFirestoreClient> client,
                              std::shared_ptr<IClock> clock,
                              QObject* parent)
    : QObject(parent), m_client(client), m_clock(clock) {
    
    m_staticFriends = getStaticFriendsList();
    m_friendsWithPresence = m_staticFriends;
    
    connect(&m_presenceTimer, &QTimer::timeout, this, &FriendsService::updatePresence);
}

void FriendsService::startPresenceUpdates(int intervalMs, int64_t staleThresholdMs) {
    m_staleThresholdMs = staleThresholdMs;
    m_isUpdating = true;
    
    updatePresence();
    
    m_presenceTimer.setInterval(intervalMs);
    m_presenceTimer.start();
}

void FriendsService::stopPresenceUpdates() {
    m_presenceTimer.stop();
    m_isUpdating = false;
}

std::vector<User> FriendsService::getStaticFriendsList() {
    return {
        User("arthas", "Arthas"),
        User("jaina", "Jaina"),
        User("thrall", "Thrall"),
        User("sylvanas", "Sylvanas"),
        User("tyrande", "Tyrande")
    };
}

void FriendsService::updatePresence() {
    if (!m_isUpdating || !m_client) {
        return;
    }
    
    std::vector<User> presenceUsers = m_client->listPresence();
    int64_t currentTime = m_clock->currentTimeMillis();
    
    m_friendsWithPresence = m_staticFriends;
    
    for (auto& friendUser : m_friendsWithPresence) {
        auto presenceIt = std::find_if(presenceUsers.begin(), presenceUsers.end(),
            [&friendUser](const User& presenceUser) {
                return presenceUser.id == friendUser.id;
            });
        
        if (presenceIt != presenceUsers.end()) {
            bool isOnline = presenceIt->status == UserStatus::Online &&
                           (currentTime - presenceIt->lastSeen) < m_staleThresholdMs;
            
            friendUser.status = isOnline ? UserStatus::Online : UserStatus::Offline;
            friendUser.lastSeen = presenceIt->lastSeen;
            
            if (!presenceIt->displayName.isEmpty()) {
                friendUser.displayName = presenceIt->displayName;
            }
        } else {
            friendUser.status = UserStatus::Offline;
        }
    }
    
    emit friendsPresenceUpdated(m_friendsWithPresence);
}