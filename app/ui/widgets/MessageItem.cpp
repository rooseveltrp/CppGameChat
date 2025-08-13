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
    // Set background and styling for the message item
    setStyleSheet(
        "MessageItem { "
        "   background-color: rgba(0, 43, 79, 0.3); "
        "   border: 1px solid #0098fa; "
        "   border-radius: 8px; "
        "   margin: 4px 0px; "
        "   padding: 2px; "
        "}"
    );
    
    m_mainLayout = new QHBoxLayout(this);
    m_contentLayout = new QVBoxLayout();
    
    m_authorLabel = new QLabel();
    m_authorLabel->setStyleSheet(
        "color: #0098fa; "
        "font-weight: bold; "
        "font-size: 14px; "
        "padding: 2px 4px;"
    );
    
    m_textLabel = new QLabel();
    m_textLabel->setWordWrap(true);
    m_textLabel->setStyleSheet(
        "color: #ffffff; "
        "margin-left: 8px; "
        "margin-top: 4px; "
        "font-size: 13px; "
        "line-height: 1.4;"
    );
    
    m_timestampLabel = new QLabel();
    m_timestampLabel->setStyleSheet(
        "color: #8ca0b3; "
        "font-size: 11px; "
        "padding: 2px 4px;"
    );
    m_timestampLabel->setAlignment(Qt::AlignRight);
    
    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addWidget(m_authorLabel);
    topLayout->addStretch(); // Push timestamp to the right
    topLayout->addWidget(m_timestampLabel);
    
    m_contentLayout->addLayout(topLayout);
    m_contentLayout->addWidget(m_textLabel);
    
    m_mainLayout->addLayout(m_contentLayout);
    m_mainLayout->setContentsMargins(12, 8, 12, 8);
    
    updateMessage(m_message);
}

QString MessageItem::formatTimestamp(int64_t timestamp) const {
    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(timestamp);
    return dateTime.toString("hh:mm:ss");
}