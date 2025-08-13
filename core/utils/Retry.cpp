#include "Retry.h"
#include <thread>
#include <chrono>
#include <algorithm>

RetryPolicy::RetryPolicy(int maxRetries, int baseDelayMs, int maxDelayMs)
    : m_maxRetries(maxRetries), m_baseDelayMs(baseDelayMs), m_maxDelayMs(maxDelayMs), m_generator(m_randomDevice()) {
}

bool RetryPolicy::executeVoid(std::function<void()> operation) {
    for (int attempt = 0; attempt <= m_maxRetries; ++attempt) {
        try {
            operation();
            return true;
        } catch (...) {
            if (attempt == m_maxRetries) {
                return false;
            }
            
            int delayMs = calculateDelay(attempt);
            std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        }
    }
    return false;
}

int RetryPolicy::calculateDelay(int attempt) const {
    int exponentialDelay = m_baseDelayMs * (1 << attempt);
    int cappedDelay = std::min(exponentialDelay, m_maxDelayMs);
    
    std::uniform_real_distribution<double> jitterDist(0.8, 1.2);
    double jitter = jitterDist(m_generator);
    
    return static_cast<int>(cappedDelay * jitter);
}