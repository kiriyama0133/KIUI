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

class WindowManagerTest : public ::testing::Test {
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

// Simple test: Create a window and drag it to another display to test DPI notification
TEST_F(WindowManagerTest, DpiChangeInteractiveTest) {
    auto& manager = WindowManager::GetSharedInstance();
    
    auto window = manager.CreateNativeWindow("DPI Test", 800, 600, false, true);
    ASSERT_NE(window, nullptr);
    
    std::atomic<int> dpiChangeCount{0};
    
    window->OnContentScaleChanged.connect([&](float xScale, float yScale) {
        dpiChangeCount++;
        std::cout << "DPI Changed: X=" << xScale << ", Y=" << yScale << std::endl;
    });
    
    std::cout << "Initial DPI: X=" << window->GetContentScaleX() << ", Y=" << window->GetContentScaleY() << std::endl;
    std::cout << "Drag window to another display..." << std::endl;
    
    for (int i = 0; i < 300; ++i) {
        glfwPollEvents();
        manager.PollMainThreadTasks();
        if (window->ShouldClose()) break;
        if (dpiChangeCount.load() > 0) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// Simple test: Execute background task and callback on UI thread
TEST_F(WindowManagerTest, BackgroundTaskTest) {
    auto& manager = WindowManager::GetSharedInstance();
    
    // Create a context object
    struct TestContext {
        int value = 0;
    };
    auto context = boost::make_shared<TestContext>();
    
    std::atomic<bool> taskCompleted{false};
    std::atomic<bool> callbackExecuted{false};
    std::atomic<std::thread::id> taskThreadId;
    std::atomic<std::thread::id> callbackThreadId;
    std::thread::id mainThreadId = std::this_thread::get_id();
    
    // Execute background task
    manager.ExecuteBackgroundTask<TestContext>(
        context,
        // Background task: simulate time-consuming work
        [&]() -> int {
            taskThreadId = std::this_thread::get_id();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            taskCompleted = true;
            return 42;
        },
        // Callback on UI thread
        [&](boost::shared_ptr<TestContext> ctx, int result) {
            callbackThreadId = std::this_thread::get_id();
            callbackExecuted = true;
            ctx->value = result;
            std::cout << "Callback executed on UI thread, result: " << result << std::endl;
        }
    );
    
    // Poll main thread tasks until callback is executed
    int pollCount = 0;
    while (!callbackExecuted && pollCount < 100) {
        manager.PollMainThreadTasks();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        pollCount++;
    }
    
    // Verify task was executed in background thread
    EXPECT_TRUE(taskCompleted.load());
    EXPECT_NE(taskThreadId.load(), mainThreadId);
    
    // Verify callback was executed on main thread
    EXPECT_TRUE(callbackExecuted.load());
    EXPECT_EQ(callbackThreadId.load(), mainThreadId);
    EXPECT_EQ(context->value, 42);
}
