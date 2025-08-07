#pragma once
#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include <sys/resource.h>


class PerformanceProfiler {

private:
    struct TimingData {
        std::string name;
        std::vector<double> measurements;
        double totalTime = 0.0;
        size_t callCount = 0;
    };
    
    std::vector<TimingData> timers;
    std::chrono::high_resolution_clock::time_point startTime;
    
public:
    
    int collisionCheck = 0;
    int collisionVerified = 0;

    void printCollisionStats() {
        std::cout << "Collision Checks: " << collisionCheck << std::endl;
        std::cout << "Collision Verified: " << collisionVerified << std::endl;
        std::cout << "Collision Verification Rate: "
                  << (collisionCheck > 0 ? static_cast<float>(collisionVerified) / collisionCheck * 100.0f : 0.0f)
                  << "%" << std::endl;
        collisionCheck = 0;
        collisionVerified = 0;
    }

    void printMemoryUsage() {
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        std::cout << "Memory: " << usage.ru_maxrss << " KB" << std::endl;
    }

    void startTimer(const std::string& name) {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    void endTimer(const std::string& name) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        double microseconds = duration.count();
        
        // Find or create timer
        auto it = std::find_if(timers.begin(), timers.end(), 
                              [&name](const TimingData& t) { return t.name == name; });
        
        if (it == timers.end()) {
            TimingData newTimer;
            newTimer.name = name;
            newTimer.measurements.push_back(microseconds);
            newTimer.totalTime = microseconds;
            newTimer.callCount = 1;
            timers.push_back(newTimer);
        } else {
            it->measurements.push_back(microseconds);
            it->totalTime += microseconds;
            it->callCount++;
            
            // Keep only last 100 measurements for rolling average
            if (it->measurements.size() > 100) {
                it->totalTime -= it->measurements.front();
                it->measurements.erase(it->measurements.begin());
                it->callCount = it->measurements.size();
            }
        }
    }
    
    void printStats() {
        std::cout << "\n=== Performance Stats ===" << std::endl;
        for (const auto& timer : timers) {
            if (timer.callCount > 0) {
                double avgTime = timer.totalTime / timer.callCount;
                double minTime = *std::min_element(timer.measurements.begin(), timer.measurements.end());
                double maxTime = *std::max_element(timer.measurements.begin(), timer.measurements.end());
                
                std::cout << timer.name << ":" << std::endl;
                std::cout << "  Avg: " << avgTime << "μs (" << avgTime/1000.0 << "ms)" << std::endl;
                std::cout << "  Min: " << minTime << "μs" << std::endl;
                std::cout << "  Max: " << maxTime << "μs" << std::endl;
                std::cout << "  Calls: " << timer.callCount << std::endl;
                std::cout << "  Total: " << timer.totalTime/1000.0 << "ms" << std::endl;
                std::cout << std::endl;
            }
        }
    }
    
    double getAverageTime(const std::string& name) {
        auto it = std::find_if(timers.begin(), timers.end(), 
                              [&name](const TimingData& t) { return t.name == name; });
        return (it != timers.end() && it->callCount > 0) ? it->totalTime / it->callCount : 0.0;
    }
};

// RAII Timer for automatic timing
class ScopedTimer {
private:
    PerformanceProfiler& profiler;
    std::string name;
    
public:
    ScopedTimer(PerformanceProfiler& p, const std::string& n) : profiler(p), name(n) {
        profiler.startTimer(name);
    }
    
    ~ScopedTimer() {
        profiler.endTimer(name);
    }
};

// Macro for easy timing
#define PROFILE_SCOPE(profiler, name) ScopedTimer timer(profiler, name)
#define PROFILE_FUNCTION(profiler) ScopedTimer timer(profiler, __FUNCTION__)

// Global profiler instance
extern PerformanceProfiler g_profiler;
