#include <gtest/gtest.h>
#include "../src/hpps/window.hpp"
#include "../src/hpps/window_class.hpp"
#include <GLFW/glfw3.h>
#include <thread>
#include <chrono>
#include <atomic>
#include <iostream>

using namespace KiUI::foundation;

// Global cleanup to terminate GLFW after all tests
struct GLFWCleanup {
    ~GLFWCleanup() {
        try {
            auto& manager = WindowManager::GetSharedInstance();
            manager.ShutdownPlatformSubsystems();
        } catch (...) {
            // Ignore errors during cleanup
        }
    }
};
static GLFWCleanup glfwCleanup;

class WindowFocusTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto& manager = WindowManager::GetSharedInstance();
        if (!manager.InitializePlatformSubsystems()) {
            GTEST_SKIP() << "Failed to initialize GLFW, skipping tests";
        }
    }

    void TearDown() override {
        auto& manager = WindowManager::GetSharedInstance();
        auto windows = manager.GetTrackedWindows();
        for (auto& window : windows) {
            if (window) {
                manager.CloseAndReleaseWindow(window);
            }
        }
        manager.UpdateActiveWindow(nullptr, false);
        glfwPollEvents();
    }
};

// Test focus change between two windows via GLFW callbacks (user pointer)
TEST_F(WindowFocusTest, TwoWindowsFocusChangeTest) {
    auto& manager = WindowManager::GetSharedInstance();
    
    // Create two windows and show them
    auto window1 = manager.CreateNativeWindow("Window 1", 400, 300, false, true);
    auto window2 = manager.CreateNativeWindow("Window 2", 400, 300, false, true);
    
    ASSERT_NE(window1, nullptr);
    ASSERT_NE(window2, nullptr);
    ASSERT_NE(window1->GetHandle(), nullptr);
    ASSERT_NE(window2->GetHandle(), nullptr);
    
    // Verify user pointers are set correctly (key for callbacks to work)
    void* userPtr1 = glfwGetWindowUserPointer(window1->GetHandle());
    void* userPtr2 = glfwGetWindowUserPointer(window2->GetHandle());
    EXPECT_NE(userPtr1, nullptr) << "Window 1 user pointer should be set";
    EXPECT_NE(userPtr2, nullptr) << "Window 2 user pointer should be set";
    
    // Record focus change events triggered by GLFW callbacks
    std::atomic<int> window1FocusCount{0};
    std::atomic<int> window2FocusCount{0};
    std::atomic<int> managerFocusCount{0};
    
    // Connect Window focus change signals (triggered by GLFW callbacks via user pointer)
    window1->OnFocusChanged.connect([&](bool focused) {
        window1FocusCount++;
        std::cout << "Window1 OnFocusChanged: " << focused << std::endl;
    });
    
    window2->OnFocusChanged.connect([&](bool focused) {
        window2FocusCount++;
        std::cout << "Window2 OnFocusChanged: " << focused << std::endl;
    });
    
    // Connect WindowManager focus change signal
    manager.OnWindowFocusChanged.connect([&](boost::shared_ptr<Window> window, bool focused) {
        managerFocusCount++;
        std::cout << "Manager OnWindowFocusChanged: Window=" 
                  << (window == window1 ? "Window1" : "Window2") 
                  << ", Focused=" << focused << std::endl;
    });
    
    // Process initial events
    glfwPollEvents();
    manager.PollMainThreadTasks();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Reset counters after initial events
    int initialWindow1Count = window1FocusCount.load();
    int initialWindow2Count = window2FocusCount.load();
    int initialManagerCount = managerFocusCount.load();
    
    std::cout << "Initial focus counts - Window1: " << initialWindow1Count 
              << ", Window2: " << initialWindow2Count 
              << ", Manager: " << initialManagerCount << std::endl;
    std::cout << "Please manually click on Window1 and Window2 to test focus change..." << std::endl;
    std::cout << "Test will run for 30 seconds..." << std::endl;
    
    // Poll events and wait for user to manually change focus
    int pollCount = 0;
    const int maxPolls = 300; // 30 seconds
    
    while (pollCount < maxPolls) {
        glfwPollEvents();
        manager.PollMainThreadTasks();
        
        if (window1->ShouldClose() || window2->ShouldClose()) {
            break;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        pollCount++;
    }
    
    // Check if focus changes were detected
    int finalWindow1Count = window1FocusCount.load();
    int finalWindow2Count = window2FocusCount.load();
    int finalManagerCount = managerFocusCount.load();
    
    std::cout << "Final focus counts - Window1: " << finalWindow1Count 
              << ", Window2: " << finalWindow2Count 
              << ", Manager: " << finalManagerCount << std::endl;
    
    // Verify callbacks were triggered via user pointer mechanism
    if (finalWindow1Count > initialWindow1Count || finalWindow2Count > initialWindow2Count) {
        std::cout << "Focus change detected via user pointer mechanism!" << std::endl;
    } else {
        std::cout << "No focus change detected. This is OK if you didn't click on the windows." << std::endl;
    }
}

