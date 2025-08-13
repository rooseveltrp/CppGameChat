#include <QApplication>
#include <QMainWindow>
#include <QStackedWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QDir>
#include <QDebug>
#include <memory>

#include "ui/LoginView.h"
#include "ui/ChatView.h"
#include "services/FirestoreClient.h"
#include "services/PresenceService.h"
#include "services/ChatService.h"
#include "services/FriendsService.h"
#include "utils/Config.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        loadConfig();
        setupServices();
        connectSignals();
    }

private slots:
    void onLoginRequested(const QString& username) {
        m_currentUsername = username;
        m_currentDisplayName = username.left(1).toUpper() + username.mid(1).toLower();
        
        if (!m_config.isValid()) {
            QMessageBox::warning(this, "Configuration Error", 
                "Please check your config.json file. Make sure apiKey and projectId are set.");
            return;
        }
        
        m_chatView->setStatusMessage("⚡ Establishing battlefield connection...");
        m_stackedWidget->setCurrentWidget(m_chatView);
        
        FirestoreConfig firestoreConfig(m_config.apiKey, m_config.projectId);
        firestoreConfig.databaseId = m_config.databaseId;
        
        m_firestoreClient = std::make_shared<FirestoreClient>(firestoreConfig, this);
        m_presenceService = std::make_unique<PresenceService>(m_firestoreClient, std::make_shared<SystemClock>(), this);
        m_chatService = std::make_unique<ChatService>(m_firestoreClient, std::make_shared<SystemClock>(), this);
        m_friendsService = std::make_unique<FriendsService>(m_firestoreClient, std::make_shared<SystemClock>(), this);
        
        connectServiceSignals();
        
        m_friendsService->setCurrentUser(m_currentUsername, m_currentDisplayName);
        m_presenceService->start(m_currentUsername, m_currentDisplayName, m_config.presenceHeartbeatMs);
        m_chatService->startPolling("global", m_config.pollIntervalMs);
        m_friendsService->startPresenceUpdates(2000, m_config.presenceStaleMs);
        
        m_chatView->setStatusMessage("🟢 Battle station online - Ready for combat!");
    }
    
    void onMessageToSend(const QString& text) {
        if (m_chatService) {
            bool success = m_chatService->sendMessage("global", m_currentUsername, m_currentDisplayName, text);
            if (!success) {
                m_chatView->setStatusMessage("❌ Message transmission failed - Check battlefield communications");
            }
        }
    }
    
    void onMessagesReceived(const std::vector<Message>& messages) {
        m_chatView->addMessages(messages);
    }
    
    void onFriendsPresenceUpdated(const std::vector<User>& friends) {
        m_chatView->updateFriendsList(friends);
    }
    
    void onHeartbeatSent(bool success) {
        if (!success) {
            m_chatView->setStatusMessage("⚠️ Communication disruption detected...");
        } else {
            m_chatView->setStatusMessage("🟢 Battle station online - Ready for combat!");
        }
    }

protected:
    void closeEvent(QCloseEvent* event) override {
        if (m_presenceService) {
            m_presenceService->stop();
        }
        if (m_chatService) {
            m_chatService->stopPolling();
        }
        if (m_friendsService) {
            m_friendsService->stopPresenceUpdates();
        }
        QMainWindow::closeEvent(event);
    }

private:
    void setupUI() {
        setWindowTitle("⚔ CppGameChat - Battlefield Commander ⚔");
        setMinimumSize(900, 700);
        resize(1200, 800);
        
        // Set main window styling
        setStyleSheet(
            "QMainWindow { "
            "   background-color: #131823; "
            "   color: #ffffff; "
            "}"
            "QStatusBar { "
            "   background-color: #002b4f; "
            "   color: #0098fa; "
            "   border-top: 2px solid #0098fa; "
            "   font-weight: bold; "
            "   padding: 4px; "
            "}"
            "QMenuBar { "
            "   background-color: #002b4f; "
            "   color: #ffffff; "
            "   border-bottom: 2px solid #0098fa; "
            "}"
        );
        
        m_stackedWidget = new QStackedWidget(this);
        setCentralWidget(m_stackedWidget);
        
        m_loginView = new LoginView();
        m_chatView = new ChatView();
        
        m_stackedWidget->addWidget(m_loginView);
        m_stackedWidget->addWidget(m_chatView);
        
        statusBar()->showMessage("🔥 Battle Station Ready - Awaiting Commands");
    }
    
    void loadConfig() {
        QString configPath = QDir::currentPath() + "/config.json";
        m_config = AppConfig::loadFromFile(configPath);
        
        if (!m_config.isValid()) {
            qWarning() << "Invalid configuration loaded from" << configPath;
            statusBar()->showMessage("Configuration error - check config.json");
        }
    }
    
    void setupServices() {
        
    }
    
    void connectSignals() {
        connect(m_loginView, &LoginView::loginRequested, this, &MainWindow::onLoginRequested);
        connect(m_chatView, &ChatView::messageToSend, this, &MainWindow::onMessageToSend);
    }
    
    void connectServiceSignals() {
        if (m_chatService) {
            connect(m_chatService.get(), &ChatService::messagesReceived, 
                   this, &MainWindow::onMessagesReceived);
        }
        
        if (m_friendsService) {
            connect(m_friendsService.get(), &FriendsService::friendsPresenceUpdated,
                   this, &MainWindow::onFriendsPresenceUpdated);
        }
        
        if (m_presenceService) {
            connect(m_presenceService.get(), &PresenceService::heartbeatSent,
                   this, &MainWindow::onHeartbeatSent);
        }
    }
    
    QStackedWidget* m_stackedWidget;
    LoginView* m_loginView;
    ChatView* m_chatView;
    
    AppConfig m_config;
    QString m_currentUsername;
    QString m_currentDisplayName;
    
    std::shared_ptr<FirestoreClient> m_firestoreClient;
    std::unique_ptr<PresenceService> m_presenceService;
    std::unique_ptr<ChatService> m_chatService;
    std::unique_ptr<FriendsService> m_friendsService;
};

#include "main.moc"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    MainWindow window;
    window.show();
    
    return app.exec();
}