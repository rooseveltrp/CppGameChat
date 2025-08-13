#include <gtest/gtest.h>
#include "models/Presence.h"
#include "services/PresenceService.h"
#include "utils/Clock.h"
#include <memory>

class MockFirestoreClient : public IFirestoreClient {
public:
    bool setPresence(const QString& userId, const QString& name,
                    const QString& status, int64_t millisSinceEpoch) override {
        lastUserId = userId;
        lastName = name;
        lastStatus = status;
        lastHeartbeat = millisSinceEpoch;
        return shouldSucceed;
    }
    
    std::vector<User> listPresence() override { return {}; }
    bool postMessage(const QString&, const Message&) override { return false; }
    std::vector<Message> listMessages(const QString&, int64_t) override { return {}; }
    
    QString lastUserId;
    QString lastName;
    QString lastStatus;
    int64_t lastHeartbeat = 0;
    bool shouldSucceed = true;
};

class PresenceServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockClient = std::make_shared<MockFirestoreClient>();
        mockClock = std::make_shared<MockClock>(1000000);
        service = std::make_unique<PresenceService>(mockClient, mockClock);
    }
    
    std::shared_ptr<MockFirestoreClient> mockClient;
    std::shared_ptr<MockClock> mockClock;
    std::unique_ptr<PresenceService> service;
};

TEST_F(PresenceServiceTest, StartSetsPresenceToOnline) {
    service->start("testuser", "Test User", 5000);
    
    EXPECT_EQ(mockClient->lastUserId, "testuser");
    EXPECT_EQ(mockClient->lastName, "Test User");
    EXPECT_EQ(mockClient->lastStatus, "online");
    EXPECT_EQ(mockClient->lastHeartbeat, 1000000);
}

TEST_F(PresenceServiceTest, StopSetsPresenceToOffline) {
    service->start("testuser", "Test User", 5000);
    mockClock->setTime(2000000);
    
    service->stop();
    
    EXPECT_EQ(mockClient->lastStatus, "offline");
    EXPECT_EQ(mockClient->lastHeartbeat, 2000000);
}

TEST_F(PresenceServiceTest, IsOnlineCalculatesCorrectly) {
    int64_t currentTime = 10000;
    int64_t staleThreshold = 5000;
    
    EXPECT_TRUE(service->isOnline("online", currentTime - 1000, staleThreshold));
    
    EXPECT_FALSE(service->isOnline("online", currentTime - 6000, staleThreshold));
    
    EXPECT_FALSE(service->isOnline("offline", currentTime - 1000, staleThreshold));
}

TEST(PresenceModelTest, IsOnlineMethod) {
    Presence presence("user1", "User One", "online", 1000);
    
    EXPECT_TRUE(presence.isOnline(5000, 5000));
    EXPECT_FALSE(presence.isOnline(7000, 5000));
    
    Presence offlinePresence("user2", "User Two", "offline", 1000);
    EXPECT_FALSE(offlinePresence.isOnline(5000, 5000));
}