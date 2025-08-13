#pragma once

#include "IFirestoreClient.h"
#include "models/User.h"
#include "utils/Clock.h"
#include <QObject>
#include <QTimer>
#include <vector>
#include <memory>

class FriendsService : public QObject {
    Q_OBJECT

public:
    explicit FriendsService(std::shared_ptr<IFirestoreClient> client,
                           std::shared_ptr<IClock> clock = std::make_shared<SystemClock>(),
                           QObject* parent = nullptr);
    
    void startPresenceUpdates(int intervalMs = 2000, int64_t staleThresholdMs = 12000);
    void stopPresenceUpdates();
    
    std::vector<User> getFriendsWithPresence() const { return m_friendsWithPresence; }
    
    static std::vector<User> getStaticFriendsList();

signals:
    void friendsPresenceUpdated(const std::vector<User>& friends);

private slots:
    void updatePresence();

private:
    std::shared_ptr<IFirestoreClient> m_client;
    std::shared_ptr<IClock> m_clock;
    QTimer m_presenceTimer;
    std::vector<User> m_staticFriends;
    std::vector<User> m_friendsWithPresence;
    int64_t m_staleThresholdMs = 12000;
    bool m_isUpdating = false;
};