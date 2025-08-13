#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

class LoginView : public QWidget {
    Q_OBJECT

public:
    explicit LoginView(QWidget* parent = nullptr);

signals:
    void loginRequested(const QString& username);

private slots:
    void onEnterChatClicked();
    void onUsernameChanged();

private:
    void setupUI();
    bool isUsernameValid(const QString& username) const;
    
    QLineEdit* m_usernameEdit;
    QPushButton* m_enterChatButton;
    QLabel* m_titleLabel;
    QLabel* m_instructionLabel;
    QVBoxLayout* m_mainLayout;
};