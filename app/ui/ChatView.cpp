#include "ChatView.h"
#include "widgets/MessageItem.h"
#include <QListWidgetItem>
#include <QScrollBar>
#include <QSplitter>

ChatView::ChatView(QWidget* parent) : QWidget(parent) {
    setupUI();
}

void ChatView::setupUI() {
    m_mainLayout = new QHBoxLayout(this);
    
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    
    m_friendsList = new QListWidget();
    m_friendsList->setMaximumWidth(200);
    m_friendsList->setStyleSheet(
        "QListWidget { "
        "   background-color: #f5f5f5; "
        "   border: 1px solid #ddd; "
        "   border-radius: 4px; "
        "}"
        "QListWidget::item { "
        "   padding: 8px; "
        "   border-bottom: 1px solid #eee; "
        "}"
        "QListWidget::item:selected { "
        "   background-color: #e8f5e8; "
        "}"
    );
    
    QWidget* rightWidget = new QWidget();
    m_rightLayout = new QVBoxLayout(rightWidget);
    
    m_messagesContainer = new QWidget();
    m_messagesLayout = new QVBoxLayout(m_messagesContainer);
    m_messagesLayout->addStretch();
    
    m_messagesScrollArea = new QScrollArea();
    m_messagesScrollArea->setWidget(m_messagesContainer);
    m_messagesScrollArea->setWidgetResizable(true);
    m_messagesScrollArea->setStyleSheet(
        "QScrollArea { "
        "   border: 1px solid #ddd; "
        "   border-radius: 4px; "
        "   background-color: white; "
        "}"
    );
    
    QWidget* inputWidget = new QWidget();
    QHBoxLayout* inputLayout = new QHBoxLayout(inputWidget);
    
    m_messageEdit = new QLineEdit();
    m_messageEdit->setPlaceholderText("Type your message here...");
    m_messageEdit->setStyleSheet(
        "QLineEdit { "
        "   padding: 8px; "
        "   border: 2px solid #ddd; "
        "   border-radius: 4px; "
        "   font-size: 14px; "
        "}"
        "QLineEdit:focus { "
        "   border-color: #2E7D32; "
        "}"
    );
    
    m_sendButton = new QPushButton("Send");
    m_sendButton->setStyleSheet(
        "QPushButton { "
        "   background-color: #2E7D32; "
        "   color: white; "
        "   border: none; "
        "   border-radius: 4px; "
        "   padding: 8px 16px; "
        "   font-size: 14px; "
        "   font-weight: bold; "
        "}"
        "QPushButton:hover { "
        "   background-color: #1B5E20; "
        "}"
        "QPushButton:disabled { "
        "   background-color: #ccc; "
        "}"
    );
    m_sendButton->setEnabled(false);
    
    inputLayout->addWidget(m_messageEdit);
    inputLayout->addWidget(m_sendButton);
    
    m_statusLabel = new QLabel("Ready");
    m_statusLabel->setStyleSheet("color: #666; font-size: 12px; padding: 4px;");
    
    m_rightLayout->addWidget(m_messagesScrollArea);
    m_rightLayout->addWidget(inputWidget);
    m_rightLayout->addWidget(m_statusLabel);
    
    splitter->addWidget(m_friendsList);
    splitter->addWidget(rightWidget);
    splitter->setSizes({200, 600});
    
    m_mainLayout->addWidget(splitter);
    
    connect(m_messageEdit, &QLineEdit::textChanged, this, &ChatView::onMessageTextChanged);
    connect(m_messageEdit, &QLineEdit::returnPressed, this, &ChatView::onSendClicked);
    connect(m_sendButton, &QPushButton::clicked, this, &ChatView::onSendClicked);
    
    connect(m_messagesScrollArea->verticalScrollBar(), &QScrollBar::valueChanged,
            [this](int value) {
                QScrollBar* scrollBar = m_messagesScrollArea->verticalScrollBar();
                m_userScrolledUp = (value < scrollBar->maximum() - 10);
            });
}

void ChatView::updateFriendsList(const std::vector<User>& friends) {
    m_friendsList->clear();
    
    for (const auto& user : friends) {
        QListWidgetItem* item = new QListWidgetItem();
        
        QString statusIndicator = user.isOnline() ? "🟢" : "⚫";
        QString displayText = QString("%1 %2").arg(statusIndicator, user.displayName);
        
        item->setText(displayText);
        item->setData(Qt::UserRole, user.id);
        
        m_friendsList->addItem(item);
    }
}

void ChatView::addMessage(const Message& message) {
    MessageItem* messageItem = new MessageItem(message);
    messageItem->setStyleSheet(
        "MessageItem { "
        "   border-bottom: 1px solid #eee; "
        "   margin: 2px 0; "
        "}"
    );
    
    m_messagesLayout->insertWidget(m_messagesLayout->count() - 1, messageItem);
    
    if (shouldAutoScroll()) {
        scrollToBottom();
    }
}

void ChatView::addMessages(const std::vector<Message>& messages) {
    for (const auto& message : messages) {
        addMessage(message);
    }
}

void ChatView::setStatusMessage(const QString& message) {
    m_statusLabel->setText(message);
}

void ChatView::onSendClicked() {
    QString text = m_messageEdit->text().trimmed();
    if (!text.isEmpty()) {
        emit messageToSend(text);
        m_messageEdit->clear();
    }
}

void ChatView::onMessageTextChanged() {
    QString text = m_messageEdit->text().trimmed();
    m_sendButton->setEnabled(!text.isEmpty());
}

void ChatView::scrollToBottom() {
    QScrollBar* scrollBar = m_messagesScrollArea->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
    m_userScrolledUp = false;
}

bool ChatView::shouldAutoScroll() const {
    return !m_userScrolledUp;
}