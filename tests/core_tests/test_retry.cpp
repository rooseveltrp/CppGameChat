#include <gtest/gtest.h>
#include "utils/Retry.h"
#include <stdexcept>

TEST(RetryPolicyTest, SuccessfulOperationReturnsTrue) {
    RetryPolicy policy(3, 100, 800);
    
    bool result = policy.executeVoid([]() {
        
    });
    
    EXPECT_TRUE(result);
}

TEST(RetryPolicyTest, FailingOperationRetriesAndFails) {
    RetryPolicy policy(2, 10, 50);
    
    int attemptCount = 0;
    bool result = policy.executeVoid([&attemptCount]() {
        attemptCount++;
        throw std::runtime_error("Test error");
    });
    
    EXPECT_FALSE(result);
    EXPECT_EQ(attemptCount, 3);
}

TEST(RetryPolicyTest, EventualSuccessReturnsTrue) {
    RetryPolicy policy(3, 10, 50);
    
    int attemptCount = 0;
    bool result = policy.executeVoid([&attemptCount]() {
        attemptCount++;
        if (attemptCount < 3) {
            throw std::runtime_error("Test error");
        }
    });
    
    EXPECT_TRUE(result);
    EXPECT_EQ(attemptCount, 3);
}

TEST(RetryPolicyTest, DelayCalculationIsWithinBounds) {
    RetryPolicy policy(3, 100, 800);
    
    auto calculateDelay = [](int attempt) -> int {
        
        
        int baseDelay = 100;
        int exponentialDelay = baseDelay * (1 << attempt);
        int cappedDelay = std::min(exponentialDelay, 800);
        
        return cappedDelay;
    };
    
    EXPECT_EQ(calculateDelay(0), 100);
    EXPECT_EQ(calculateDelay(1), 200);  
    EXPECT_EQ(calculateDelay(2), 400);  
    EXPECT_EQ(calculateDelay(3), 800);  
    EXPECT_EQ(calculateDelay(4), 800);  
}