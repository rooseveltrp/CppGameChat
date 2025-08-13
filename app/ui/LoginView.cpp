#include "LoginView.h"
#include <QKeyEvent>

LoginView::LoginView(QWidget* parent) : QWidget(parent) {
    setupUI();
}

void LoginView::setupUI() {
    // Set the main background color
    setStyleSheet("LoginView { background-color: #131823; }");
    
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setAlignment(Qt::AlignCenter);
    m_mainLayout->setSpacing(30);
    
    m_titleLabel = new QLabel("CppGameChat");
    m_titleLabel->setStyleSheet(
        "font-size: 32px; "
        "font-weight: bold; "
        "color: #0098fa; "
        "font-family: 'Arial Black', Arial, sans-serif; "
        "text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.8);"
    );
    m_titleLabel->setAlignment(Qt::AlignCenter);
    
    m_instructionLabel = new QLabel("Enter your username to\njoin the battlefield:");
    m_instructionLabel->setStyleSheet(
        "font-size: 16px; "
        "color: #ffffff; "
        "font-weight: 500; "
        "margin: 10px 0px; "
        "line-height: 1.3;"
    );
    m_instructionLabel->setAlignment(Qt::AlignCenter);
    m_instructionLabel->setWordWrap(true);
    
    m_usernameEdit = new QLineEdit();
    m_usernameEdit->setPlaceholderText("Enter your warrior name...");
    m_usernameEdit->setStyleSheet(
        "QLineEdit { "
        "   padding: 12px 16px; "
        "   border: 2px solid #002b4f; "
        "   border-radius: 8px; "
        "   font-size: 16px; "
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
    m_usernameEdit->setMaxLength(20);
    
    m_enterChatButton = new QPushButton("⚔ ENTER BATTLEFIELD ⚔");
    m_enterChatButton->setStyleSheet(
        "QPushButton { "
        "   background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
        "               stop: 0 #0098fa, stop: 1 #006cc4); "
        "   color: #ffffff; "
        "   border: 2px solid #0098fa; "
        "   border-radius: 8px; "
        "   padding: 15px 25px; "
        "   font-size: 16px; "
        "   font-weight: bold; "
        "   font-family: Arial, sans-serif; "
        "   text-shadow: 1px 1px 2px rgba(0, 0, 0, 0.8); "
        "}"
        "QPushButton:hover { "
        "   background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
        "               stop: 0 #33a7ff, stop: 1 #0098fa); "
        "   border-color: #33a7ff; "
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
    m_enterChatButton->setEnabled(false);
    
    QWidget* centerWidget = new QWidget();
    centerWidget->setMaximumWidth(400);
    centerWidget->setStyleSheet(
        "QWidget { "
        "   background-color: rgba(0, 43, 79, 0.8); "
        "   border: 2px solid #0098fa; "
        "   border-radius: 15px; "
        "   padding: 20px; "
        "}"
    );
    
    QVBoxLayout* centerLayout = new QVBoxLayout(centerWidget);
    centerLayout->setSpacing(25);
    centerLayout->setContentsMargins(30, 30, 30, 30);
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