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
        User("jon", "Jon"),
        User("arya", "Arya"),
        User("tyrion", "Tyrion"),
        User("daenerys", "Daenerys"),
        User("sansa", "Sansa")
    };
}

void FriendsService::setCurrentUser(const QString& userId, const QString& displayName) {
    m_currentUserId = userId;
    m_currentDisplayName = displayName;
}

void FriendsService::updatePresence() {
    if (!m_isUpdating || !m_client) {
        return;
    }
    
    std::vector<User> presenceUsers = m_client->listPresence();
    int64_t currentTime = m_clock->currentTimeMillis();
    
    // Start with static friends
    m_friendsWithPresence = m_staticFriends;
    
    // Add ALL users from presence collection (real logged-in users)
    for (const auto& presenceUser : presenceUsers) {
        // Check if this user is already in our friends list
        auto existingIt = std::find_if(m_friendsWithPresence.begin(), m_friendsWithPresence.end(),
            [&presenceUser](const User& user) {
                return user.id == presenceUser.id;
            });
        
        if (existingIt == m_friendsWithPresence.end()) {
            // Add new user from presence
            m_friendsWithPresence.push_back(presenceUser);
        }
    }
    
    // Update presence status for all users in the list
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
            
            // Update display name from presence if available
            if (!presenceIt->displayName.isEmpty()) {
                friendUser.displayName = presenceIt->displayName;
            }
        } else {
            // No presence record - mark as offline
            friendUser.status = UserStatus::Offline;
        }
    }
    
    // Sort the list: current user first, then online users, then offline users
    std::sort(m_friendsWithPresence.begin(), m_friendsWithPresence.end(),
        [this](const User& a, const User& b) {
            // Current user always comes first
            if (a.id == m_currentUserId) return true;
            if (b.id == m_currentUserId) return false;
            
            // Then sort by online status (online first)
            if (a.status != b.status) {
                return a.status == UserStatus::Online;
            }
            
            // Finally sort alphabetically by display name
            return a.displayName < b.displayName;
        });
    
    emit friendsPresenceUpdated(m_friendsWithPresence);
}