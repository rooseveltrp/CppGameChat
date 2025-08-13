#pragma once

#include "models/User.h"
#include "models/Message.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QScrollArea>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QStatusBar>
#include <vector>

class MessageItem;

class ChatView : public QWidget {
    Q_OBJECT

public:
    explicit ChatView(QWidget* parent = nullptr);
    
    void updateFriendsList(const std::vector<User>& friends);
    void addMessage(const Message& message);
    void addMessages(const std::vector<Message>& messages);
    void setStatusMessage(const QString& message);

signals:
    void messageToSend(const QString& text);

private slots:
    void onSendClicked();
    void onMessageTextChanged();

private:
    void setupUI();
    void scrollToBottom();
    bool shouldAutoScroll() const;
    
    QHBoxLayout* m_mainLayout;
    QVBoxLayout* m_rightLayout;
    
    QListWidget* m_friendsList;
    QScrollArea* m_messagesScrollArea;
    QWidget* m_messagesContainer;
    QVBoxLayout* m_messagesLayout;
    QLineEdit* m_messageEdit;
    QPushButton* m_sendButton;
    QLabel* m_statusLabel;
    
    bool m_userScrolledUp = false;
};