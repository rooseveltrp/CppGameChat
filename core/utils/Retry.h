#pragma once

#include <functional>
#include <random>
#include <thread>
#include <chrono>

class RetryPolicy {
public:
    RetryPolicy(int maxRetries = 3, int baseDelayMs = 100, int maxDelayMs = 800);
    
    template<typename T>
    bool execute(std::function<T()> operation, T& result) {
        for (int attempt = 0; attempt <= m_maxRetries; ++attempt) {
            try {
                result = operation();
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
    
    bool executeVoid(std::function<void()> operation);
    
private:
    int calculateDelay(int attempt) const;
    
    int m_maxRetries;
    int m_baseDelayMs;
    int m_maxDelayMs;
    mutable std::random_device m_randomDevice;
    mutable std::mt19937 m_generator;
};