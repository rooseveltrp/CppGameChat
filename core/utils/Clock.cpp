#include "Clock.h"
#include <QDateTime>

int64_t SystemClock::currentTimeMillis() const {
    return QDateTime::currentMSecsSinceEpoch();
}