#include "LoginView.h"
#include <QKeyEvent>

LoginView::LoginView(QWidget* parent) : QWidget(parent) {
    setupUI();
}

void LoginView::setupUI() {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setAlignment(Qt::AlignCenter);
    m_mainLayout->setSpacing(20);
    
    m_titleLabel = new QLabel("CppGameChat");
    m_titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #2E7D32;");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    
    m_instructionLabel = new QLabel("Enter your username to join the chat:");
    m_instructionLabel->setStyleSheet("font-size: 14px; color: #555;");
    m_instructionLabel->setAlignment(Qt::AlignCenter);
    
    m_usernameEdit = new QLineEdit();
    m_usernameEdit->setPlaceholderText("Username");
    m_usernameEdit->setStyleSheet(
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
    m_usernameEdit->setMaxLength(20);
    
    m_enterChatButton = new QPushButton("Enter Chat");
    m_enterChatButton->setStyleSheet(
        "QPushButton { "
        "   background-color: #2E7D32; "
        "   color: white; "
        "   border: none; "
        "   border-radius: 4px; "
        "   padding: 10px 20px; "
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
    m_enterChatButton->setEnabled(false);
    
    QWidget* centerWidget = new QWidget();
    centerWidget->setMaximumWidth(300);
    
    QVBoxLayout* centerLayout = new QVBoxLayout(centerWidget);
    centerLayout->addWidget(m_titleLabel);
    centerLayout->addWidget(m_instructionLabel);
    centerLayout->addWidget(m_usernameEdit);
    centerLayout->addWidget(m_enterChatButton);
    
    m_mainLayout->addWidget(centerWidget, 0, Qt::AlignCenter);
    
    connect(m_usernameEdit, &QLineEdit::textChanged, this, &LoginView::onUsernameChanged);
    connect(m_usernameEdit, &QLineEdit::returnPressed, this, &LoginView::onEnterChatClicked);
    connect(m_enterChatButton, &QPushButton::clicked, this, &LoginView::onEnterChatClicked);
    
    m_usernameEdit->setFocus();
}

void LoginView::onEnterChatClicked() {
    QString username = m_usernameEdit->text().trimmed();
    if (isUsernameValid(username)) {
        emit loginRequested(username);
    }
}

void LoginView::onUsernameChanged() {
    QString username = m_usernameEdit->text().trimmed();
    m_enterChatButton->setEnabled(isUsernameValid(username));
}

bool LoginView::isUsernameValid(const QString& username) const {
    return !username.isEmpty() && username.length() >= 2 && username.length() <= 20;
}