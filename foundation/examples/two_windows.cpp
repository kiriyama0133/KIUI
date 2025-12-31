#include "hpps/window.hpp"
#include "hpps/window_class.hpp"
#include "hpps/logger.hpp"

using namespace KiUI::foundation;

int main() {
    Logger::Info("Starting Two Windows Demo Application");
    
    // 获取 WindowManager 单例
    auto& windowManager = WindowManager::GetSharedInstance();
    
    // 初始化平台子系统（GLFW）
    if (!windowManager.InitializePlatformSubsystems()) {
        Logger::Error("Failed to initialize platform subsystems");
        return 1;
    }
    
    Logger::Info("Platform subsystems initialized successfully");
    
    // 创建第一个窗口
    Logger::Info("Creating first window...");
    auto window1 = windowManager.CreateNativeWindow(
        "Window 1 - First Window",
        800, 600,
        false,  // 有边框
        true    // 立即显示
    );
    
    if (!window1) {
        Logger::Error("Failed to create first window");
        windowManager.ShutdownPlatformSubsystems();
        return 1;
    }
    Logger::Info("First window created successfully");
    
    // 创建第二个窗口
    Logger::Info("Creating second window...");
    auto window2 = windowManager.CreateNativeWindow(
        "Window 2 - Second Window",
        800, 600,
        false,  // 有边框
        true    // 立即显示
    );
    
    if (!window2) {
        Logger::Error("Failed to create second window");
        windowManager.ShutdownPlatformSubsystems();
        return 1;
    }
    Logger::Info("Second window created successfully");
    
    // 连接窗口事件信号
    window1->OnContentScaleChanged.connect([](float xScale, float yScale) {
        Logger::Debug("Window 1 DPI changed: xScale = {0}, yScale = {1}", xScale, yScale);
    });
    
    window2->OnContentScaleChanged.connect([](float xScale, float yScale) {
        Logger::Debug("Window 2 DPI changed: xScale = {0}, yScale = {1}", xScale, yScale);
    });
    
    window1->OnFocusChanged.connect([](bool focused) {
        Logger::Info("Window 1 focus changed: {0}", focused ? "Focused" : "Unfocused");
    });
    
    window2->OnFocusChanged.connect([](bool focused) {
        Logger::Info("Window 2 focus changed: {0}", focused ? "Focused" : "Unfocused");
    });
    
    // 连接 WindowManager 的全局信号
    windowManager.OnWindowCreated.connect([](boost::shared_ptr<Window> window) {
        Logger::Info("Window created event received");
    });
    
    windowManager.OnWindowClosed.connect([](boost::shared_ptr<Window> window) {
        Logger::Info("Window closed event received");
    });
    
    windowManager.OnWindowFocusChanged.connect([](boost::shared_ptr<Window> window, bool focused) {
        Logger::Info("Window focus changed globally: {0}", focused ? "Focused" : "Unfocused");
    });
    
    Logger::Info("Both windows created. Entering main message loop...");
    Logger::Info("Press ESC or close windows to exit");
    
    // 进入主消息循环（会阻塞，直到所有窗口关闭或调用 RequestApplicationExit）
    windowManager.EnterMainMessageLoop();
    
    Logger::Info("Main message loop exited. Shutting down...");
    
    // 清理资源
    windowManager.ShutdownPlatformSubsystems();
    
    Logger::Info("Application terminated successfully");
    
    return 0;
}

