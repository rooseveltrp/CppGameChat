#pragma once

#include <cstdint>

class IClock {
public:
    virtual ~IClock() = default;
    virtual int64_t currentTimeMillis() const = 0;
};

class SystemClock : public IClock {
public:
    int64_t currentTimeMillis() const override;
};

class MockClock : public IClock {
public:
    explicit MockClock(int64_t timeMs = 0) : m_timeMs(timeMs) {}
    
    int64_t currentTimeMillis() const override { return m_timeMs; }
    void setTime(int64_t timeMs) { m_timeMs = timeMs; }
    void advance(int64_t deltaMs) { m_timeMs += deltaMs; }
    
private:
    int64_t m_timeMs;
};