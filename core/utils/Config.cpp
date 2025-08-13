#include "Config.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

AppConfig AppConfig::loadFromFile(const QString& filePath) {
    AppConfig config;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open config file:" << filePath;
        return config;
    }
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse config JSON:" << error.errorString();
        return config;
    }
    
    QJsonObject obj = doc.object();
    
    config.apiKey = obj["apiKey"].toString();
    config.projectId = obj["projectId"].toString();
    config.databaseId = obj.value("databaseId").toString("(default)");
    config.pollIntervalMs = obj.value("pollIntervalMs").toInt(1200);
    config.presenceHeartbeatMs = obj.value("presenceHeartbeatMs").toInt(5000);
    config.presenceStaleMs = obj.value("presenceStaleMs").toInt(12000);
    
    return config;
}

bool AppConfig::isValid() const {
    return !apiKey.isEmpty() && !projectId.isEmpty() && 
           pollIntervalMs > 0 && presenceHeartbeatMs > 0 && presenceStaleMs > 0;
}