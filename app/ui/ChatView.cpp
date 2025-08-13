#include "ChatView.h"
#include "widgets/MessageItem.h"
#include <QListWidgetItem>
#include <QScrollBar>
#include <QSplitter>

ChatView::ChatView(QWidget* parent) : QWidget(parent) {
    setupUI();
}

void ChatView::setupUI() {
    // Set main background
    setStyleSheet("ChatView { background-color: #131823; }");
    
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);
    
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setStyleSheet("QSplitter::handle { background-color: #0098fa; width: 3px; }");
    
    // Friends/Players List with gaming theme
    m_friendsList = new QListWidget();
    m_friendsList->setMaximumWidth(250);
    m_friendsList->setMinimumWidth(200);
    m_friendsList->setStyleSheet(
        "QListWidget { "
        "   background-color: #002b4f; "
        "   border: 2px solid #0098fa; "
        "   border-radius: 8px; "
        "   color: #ffffff; "
        "   font-size: 14px; "
        "   font-weight: 500; "
        "   padding: 5px; "
        "}"
        "QListWidget::item { "
        "   padding: 10px; "
        "   border-bottom: 1px solid #1a3651; "
        "   border-radius: 4px; "
        "   margin: 2px 0px; "
        "}"
        "QListWidget::item:hover { "
        "   background-color: rgba(0, 152, 250, 0.2); "
        "}"
        "QListWidget::item:selected { "
        "   background-color: rgba(0, 152, 250, 0.4); "
        "   border: 1px solid #0098fa; "
        "}"
    );
    
    QWidget* rightWidget = new QWidget();
    m_rightLayout = new QVBoxLayout(rightWidget);
    
    m_messagesContainer = new QWidget();
    m_messagesContainer->setStyleSheet("QWidget { background-color: #131823; }");
    m_messagesLayout = new QVBoxLayout(m_messagesContainer);
    m_messagesLayout->addStretch();
    
    m_messagesScrollArea = new QScrollArea();
    m_messagesScrollArea->setWidget(m_messagesContainer);
    m_messagesScrollArea->setWidgetResizable(true);
    m_messagesScrollArea->setStyleSheet(
        "QScrollArea { "
        "   border: 2px solid #0098fa; "
        "   border-radius: 8px; "
        "   background-color: #131823; "
        "}"
        "QScrollBar:vertical { "
        "   background-color: #002b4f; "
        "   width: 12px; "
        "   border-radius: 6px; "
        "}"
        "QScrollBar::handle:vertical { "
        "   background-color: #0098fa; "
        "   border-radius: 6px; "
        "   min-height: 20px; "
        "}"
        "QScrollBar::handle:vertical:hover { "
        "   background-color: #33a7ff; "
        "}"
    );
    
    QWidget* inputWidget = new QWidget();
    inputWidget->setStyleSheet("QWidget { background-color: #131823; }");
    QHBoxLayout* inputLayout = new QHBoxLayout(inputWidget);
    inputLayout->setSpacing(10);
    
    m_messageEdit = new QLineEdit();
    m_messageEdit->setPlaceholderText("💬 Send a message to your allies...");
    m_messageEdit->setStyleSheet(
        "QLineEdit { "
        "   padding: 12px 16px; "
        "   border: 2px solid #002b4f; "
        "   border-radius: 8px; "
        "   font-size: 14px; "
        "   background-color: #002b4f; "
        "   color: #ffffff; "
        "   selection-background-color: #0098fa; "
        "   font-weight: 500; "
        "   outline: none; "
        "}"
        "QLineEdit:focus { "
        "   border: 2px solid #0098fa; "
        "   background-color: #1a3651; "
        "   outline: none; "
        "}"
        "QLineEdit::placeholder { "
        "   color: #8ca0b3; "
        "}"
    );
    
    m_sendButton = new QPushButton("⚡ SEND");
    m_sendButton->setStyleSheet(
        "QPushButton { "
        "   background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
        "               stop: 0 #0098fa, stop: 1 #006cc4); "
        "   color: #ffffff; "
        "   border: 2px solid #0098fa; "
        "   border-radius: 8px; "
        "   padding: 12px 20px; "
        "   font-size: 14px; "
        "   font-weight: bold; "
        "   min-width: 80px; "
        "}"
        "QPushButton:hover { "
        "   background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
        "               stop: 0 #33a7ff, stop: 1 #0098fa); "
        "}"
        "QPushButton:pressed { "
        "   background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
        "               stop: 0 #006cc4, stop: 1 #004a8c); "
        "}"
        "QPushButton:disabled { "
        "   background-color: #404040; "
        "   border-color: #606060; "
        "   color: #808080; "
        "}"
    );
    m_sendButton->setEnabled(false);
    
    inputLayout->addWidget(m_messageEdit);
    inputLayout->addWidget(m_sendButton);
    
    m_statusLabel = new QLabel("🟢 Ready for battle");
    m_statusLabel->setStyleSheet(
        "color: #0098fa; "
        "font-size: 12px; "
        "padding: 8px; "
        "font-weight: 500; "
        "background-color: rgba(0, 43, 79, 0.5); "
        "border-radius: 4px;"
    );
    
    rightWidget->setStyleSheet("QWidget { background-color: #131823; }");
    
    m_rightLayout->addWidget(m_messagesScrollArea);
    m_rightLayout->addWidget(inputWidget);
    m_rightLayout->addWidget(m_statusLabel);
    
    splitter->addWidget(m_friendsList);
    splitter->addWidget(rightWidget);
    splitter->setSizes({250, 700});
    
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