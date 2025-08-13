#include "MessageItem.h"
#include <QDateTime>

MessageItem::MessageItem(const Message& message, QWidget* parent)
    : QWidget(parent), m_message(message) {
    setupUI();
}

void MessageItem::updateMessage(const Message& message) {
    m_message = message;
    
    m_authorLabel->setText(QString("<b>%1:</b>").arg(message.authorName));
    m_textLabel->setText(message.text);
    m_timestampLabel->setText(formatTimestamp(message.timestamp));
}

void MessageItem::setupUI() {
    m_mainLayout = new QHBoxLayout(this);
    m_contentLayout = new QVBoxLayout();
    
    m_authorLabel = new QLabel();
    m_authorLabel->setStyleSheet("color: #2E7D32; font-weight: bold;");
    
    m_textLabel = new QLabel();
    m_textLabel->setWordWrap(true);
    m_textLabel->setStyleSheet("color: #333; margin-left: 8px;");
    
    m_timestampLabel = new QLabel();
    m_timestampLabel->setStyleSheet("color: #666; font-size: 10px;");
    m_timestampLabel->setAlignment(Qt::AlignRight);
    
    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addWidget(m_authorLabel);
    topLayout->addWidget(m_timestampLabel);
    
    m_contentLayout->addLayout(topLayout);
    m_contentLayout->addWidget(m_textLabel);
    
    m_mainLayout->addLayout(m_contentLayout);
    m_mainLayout->setContentsMargins(8, 4, 8, 4);
    
    updateMessage(m_message);
}

QString MessageItem::formatTimestamp(int64_t timestamp) const {
    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(timestamp);
    return dateTime.toString("hh:mm:ss");
}