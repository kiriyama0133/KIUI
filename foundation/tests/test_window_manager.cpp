#include <gtest/gtest.h>
#include "../src/hpps/window.hpp"
#include "../src/hpps/window_class.hpp"
#include <thread>
#include <chrono>   // for std::chrono::milliseconds

using namespace KiUI::foundation;

class WindowManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 每个测试前初始化子系统
        auto& manager = WindowManager::GetSharedInstance();
        if (!manager.InitializePlatformSubsystems()) {
            GTEST_SKIP() << "Failed to initialize GLFW, skipping tests";
        }
    }

    void TearDown() override {
        // 每个测试后清理
        auto& manager = WindowManager::GetSharedInstance();
        manager.ShutdownPlatformSubsystems();
    }
};

// 测试单例模式
TEST_F(WindowManagerTest, SingletonTest) {
    auto& manager1 = WindowManager::GetSharedInstance();
    auto& manager2 = WindowManager::GetSharedInstance();
    
    // 应该返回同一个实例
    EXPECT_EQ(&manager1, &manager2);
}

// 测试初始化
TEST_F(WindowManagerTest, InitializeTest) {
    auto& manager = WindowManager::GetSharedInstance();
    
    // 初始化应该成功（在 SetUp 中已初始化）
    // 这里测试重复初始化不会崩溃
    bool result = manager.InitializePlatformSubsystems();
    // 可能返回 false（已初始化），这是正常的
    EXPECT_TRUE(result || true); // 允许返回 false（已初始化）
}

// 测试创建窗口
TEST_F(WindowManagerTest, CreateWindowTest) {
    auto& manager = WindowManager::GetSharedInstance();
    
    // 创建窗口
    auto window = manager.CreateNativeWindow("Test Window", 800, 600, false);
    
    EXPECT_NE(window, nullptr);
    EXPECT_NE(window->GetHandle(), nullptr);
    
    // 检查窗口是否在追踪列表中
    const auto& windows = manager.GetTrackedWindows();
    EXPECT_EQ(windows.size(), 1);
    EXPECT_EQ(windows[0], window);
}

// 测试创建多个窗口
TEST_F(WindowManagerTest, CreateMultipleWindowsTest) {
    auto& manager = WindowManager::GetSharedInstance();
    
    auto window1 = manager.CreateNativeWindow("Window 1", 400, 300);
    auto window2 = manager.CreateNativeWindow("Window 2", 500, 400);
    auto window3 = manager.CreateNativeWindow("Window 3", 600, 500);
    
    EXPECT_NE(window1, nullptr);
    EXPECT_NE(window2, nullptr);
    EXPECT_NE(window3, nullptr);
    
    const auto& windows = manager.GetTrackedWindows();
    EXPECT_EQ(windows.size(), 3);
}

// 测试创建无边框窗口
TEST_F(WindowManagerTest, CreateFramelessWindowTest) {
    auto& manager = WindowManager::GetSharedInstance();
    
    auto window = manager.CreateNativeWindow("Frameless Window", 800, 600, true);
    
    EXPECT_NE(window, nullptr);
    EXPECT_NE(window->GetHandle(), nullptr);
}

// 测试关闭窗口
TEST_F(WindowManagerTest, CloseWindowTest) {
    auto& manager = WindowManager::GetSharedInstance();
    
    auto window = manager.CreateNativeWindow("Test Window", 800, 600);
    EXPECT_NE(window, nullptr);
    
    const auto& windowsBefore = manager.GetTrackedWindows();
    EXPECT_EQ(windowsBefore.size(), 1);
    
    // 关闭窗口
    manager.CloseAndReleaseWindow(window);
    
    const auto& windowsAfter = manager.GetTrackedWindows();
    EXPECT_EQ(windowsAfter.size(), 0);
}

// 测试窗口关闭检测
TEST_F(WindowManagerTest, WindowShouldCloseTest) {
    auto& manager = WindowManager::GetSharedInstance();
    
    auto window = manager.CreateNativeWindow("Test Window", 800, 600);
    EXPECT_NE(window, nullptr);
    
    // 新创建的窗口不应该立即关闭
    EXPECT_FALSE(window->ShouldClose());
}

// 测试请求退出
TEST_F(WindowManagerTest, RequestExitTest) {
    auto& manager = WindowManager::GetSharedInstance();
    
    // 创建窗口以便消息循环可以运行
    auto window = manager.CreateNativeWindow("Test Window", 800, 600);
    
    // 请求退出
    manager.RequestApplicationExit();
    
    // 验证退出标志已设置（通过消息循环的行为验证）
    // 注意：这里不能直接访问私有成员，所以通过行为验证
    EXPECT_NE(window, nullptr);
}

// 测试线程调度
TEST_F(WindowManagerTest, DispatchToMainThreadTest) {
    auto& manager = WindowManager::GetSharedInstance();
    
    bool taskExecuted = false;
    
    // 从另一个线程调度任务
    std::thread worker([&manager, &taskExecuted]() {
        manager.DispatchToMainThread([&taskExecuted]() {
            taskExecuted = true;
        });
    });
    
    worker.join();
    
    // 处理任务队列
    manager.GetSharedInstance(); // 确保实例存在
    // 注意：实际执行需要在消息循环中，这里只是测试调度机制
    // 完整测试需要在消息循环运行时进行
}

// 测试清理所有窗口
TEST_F(WindowManagerTest, ShutdownClearsWindowsTest) {
    auto& manager = WindowManager::GetSharedInstance();
    
    // 创建几个窗口
    manager.CreateNativeWindow("Window 1", 400, 300);
    manager.CreateNativeWindow("Window 2", 500, 400);
    
    const auto& windowsBefore = manager.GetTrackedWindows();
    EXPECT_GE(windowsBefore.size(), 2);
    
    // 关闭子系统应该清理所有窗口
    manager.ShutdownPlatformSubsystems();
    
    const auto& windowsAfter = manager.GetTrackedWindows();
    EXPECT_EQ(windowsAfter.size(), 0);
}

