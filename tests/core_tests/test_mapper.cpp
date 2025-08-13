#include <gtest/gtest.h>
#include "repo/FirestoreMapper.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

TEST(FirestoreMapperTest, PresenceDocumentRoundTrip) {
    QString name = "Test User";
    QString status = "online";
    int64_t heartbeat = 1234567890123;
    
    QJsonObject doc = FirestoreMapper::presenceToFirestoreDocument(name, status, heartbeat);
    
    EXPECT_TRUE(doc.contains("fields"));
    QJsonObject fields = doc["fields"].toObject();
    
    EXPECT_TRUE(fields.contains("name"));
    EXPECT_TRUE(fields.contains("status"));
    EXPECT_TRUE(fields.contains("lastHeartbeat"));
    
    EXPECT_EQ(fields["name"].toObject()["stringValue"].toString(), name);
    EXPECT_EQ(fields["status"].toObject()["stringValue"].toString(), status);
    EXPECT_EQ(fields["lastHeartbeat"].toObject()["integerValue"].toString().toLongLong(), heartbeat);
    
    Presence parsedPresence = FirestoreMapper::parsePresenceDocument(doc, "testuser");
    EXPECT_EQ(parsedPresence.userId, "testuser");
    EXPECT_EQ(parsedPresence.name, name);
    EXPECT_EQ(parsedPresence.status, status);
    EXPECT_EQ(parsedPresence.lastHeartbeat, heartbeat);
}

TEST(FirestoreMapperTest, MessageDocumentRoundTrip) {
    Message originalMessage;
    originalMessage.id = "msg123";
    originalMessage.roomId = "global";
    originalMessage.author = "testuser";
    originalMessage.authorName = "Test User";
    originalMessage.text = "Hello, world!";
    originalMessage.timestamp = 1234567890123;
    
    QJsonObject doc = FirestoreMapper::messageToFirestoreDocument(originalMessage);
    
    EXPECT_TRUE(doc.contains("fields"));
    QJsonObject fields = doc["fields"].toObject();
    
    EXPECT_TRUE(fields.contains("author"));
    EXPECT_TRUE(fields.contains("name"));
    EXPECT_TRUE(fields.contains("text"));
    EXPECT_TRUE(fields.contains("ts"));
    
    Message parsedMessage = FirestoreMapper::parseMessageDocument(doc, "msg123");
    EXPECT_EQ(parsedMessage.id, "msg123");
    EXPECT_EQ(parsedMessage.author, originalMessage.author);
    EXPECT_EQ(parsedMessage.authorName, originalMessage.authorName);
    EXPECT_EQ(parsedMessage.text, originalMessage.text);
    EXPECT_EQ(parsedMessage.timestamp, originalMessage.timestamp);
}

TEST(FirestoreMapperTest, EmptyCollectionParsing) {
    QJsonObject emptyCollection;
    emptyCollection["documents"] = QJsonArray();
    
    QByteArray emptyData = QJsonDocument(emptyCollection).toJson();
    
    std::vector<User> users = FirestoreMapper::parsePresenceCollection(emptyData);
    EXPECT_TRUE(users.empty());
    
    std::vector<Message> messages = FirestoreMapper::parseMessageCollection(emptyData);
    EXPECT_TRUE(messages.empty());
}

TEST(FirestoreMapperTest, MessageFilteringBySinceTimestamp) {
    QJsonObject collection;
    QJsonArray documents;
    
    Message oldMessage;
    oldMessage.author = "user1";
    oldMessage.authorName = "User One";
    oldMessage.text = "Old message";
    oldMessage.timestamp = 1000;
    
    Message newMessage;
    newMessage.author = "user2";
    newMessage.authorName = "User Two";
    newMessage.text = "New message";
    newMessage.timestamp = 2000;
    
    QJsonObject oldDoc;
    oldDoc["name"] = "projects/test/databases/(default)/documents/rooms/global/messages/msg1";
    oldDoc["fields"] = FirestoreMapper::messageToFirestoreDocument(oldMessage)["fields"];
    
    QJsonObject newDoc;
    newDoc["name"] = "projects/test/databases/(default)/documents/rooms/global/messages/msg2";
    newDoc["fields"] = FirestoreMapper::messageToFirestoreDocument(newMessage)["fields"];
    
    documents.append(oldDoc);
    documents.append(newDoc);
    collection["documents"] = documents;
    
    QByteArray data = QJsonDocument(collection).toJson();
    
    std::vector<Message> allMessages = FirestoreMapper::parseMessageCollection(data, 0);
    EXPECT_EQ(allMessages.size(), 2);
    
    std::vector<Message> filteredMessages = FirestoreMapper::parseMessageCollection(data, 1500);
    EXPECT_EQ(filteredMessages.size(), 1);
    EXPECT_EQ(filteredMessages[0].text, "New message");
}