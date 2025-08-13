#pragma once

#include "models/Message.h"
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

class MessageItem : public QWidget {
    Q_OBJECT

public:
    explicit MessageItem(const Message& message, QWidget* parent = nullptr);
    
    const Message& getMessage() const { return m_message; }
    void updateMessage(const Message& message);

private:
    void setupUI();
    QString formatTimestamp(int64_t timestamp) const;
    
    Message m_message;
    QLabel* m_authorLabel;
    QLabel* m_textLabel;
    QLabel* m_timestampLabel;
    QHBoxLayout* m_mainLayout;
    QVBoxLayout* m_contentLayout;
};