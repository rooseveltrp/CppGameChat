#pragma once

#include <QString>

struct AppConfig {
    QString apiKey;
    QString projectId;
    QString databaseId = "(default)";
    int pollIntervalMs = 1200;
    int presenceHeartbeatMs = 5000;
    int presenceStaleMs = 12000;
    
    static AppConfig loadFromFile(const QString& filePath);
    bool isValid() const;
};