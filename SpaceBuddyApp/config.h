#ifndef CONFIG_H
#define CONFIG_H

#include <QString>

class Config {
public:
    static QString getServerUrl() {
#ifdef QT_DEBUG
        return QStringLiteral("ws://localhost:8080");
#else
        // Production server - replace with your actual server URL
        return QStringLiteral("wss://your-production-server.com");
#endif
    }

    static bool isDebugMode() {
#ifdef QT_DEBUG
        return true;
#else
        return false;
#endif
    }
};

#endif // CONFIG_H
