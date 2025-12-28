#ifndef WINDOW_HPP
#define WINDOW_HPP
#pragma once
#include <string>
#include <GLFW/glfw3.h>
#include <boost/signals2.hpp>
#include <boost/asio.hpp>
#include <iostream>

// 前置声明
struct GLFWwindow; 

namespace KiUI {
namespace foundation {

// 假设你的 Window 类定义在其他地方
class Window;

/**
 * @brief WindowManager: KiUI 框架的控制中枢
 * 负责底层子系统初始化、主循环驱动、多窗口追踪以及跨线程任务调度。
 */
class WindowManager {
public:
    // 获取全局单例
    static WindowManager& GetSharedInstance();
    // --- 框架生命周期管理 ---

    // 初始化图形子系统 (GLFW/ANGLE)
    bool InitializePlatformSubsystems();
    // 关闭子系统并释放资源
    void ShutdownPlatformSubsystems();
    // 启动主消息循环 (会阻塞当前线程)
    void EnterMainMessageLoop();
    // 请求退出应用程序
    void RequestApplicationExit();
    // update the focused window
    void UpdateActiveWindow(boost::shared_ptr<Window> window, bool focused);
    // get the window content scale
    float GetWindowContentScale(boost::shared_ptr<Window> window) const;
    // show 
    void Show(boost::shared_ptr<Window> window);
    // hide the window
    void Hide(boost::shared_ptr<Window> window);
    // check if the window is visible
    bool IsVisible(boost::shared_ptr<Window> window) const;

    // --- 多窗口管理 ---

    // 创建并追踪一个新窗口
    boost::shared_ptr<Window> CreateNativeWindow(
        const std::string& windowTitle, 
        int width, 
        int height, 
        bool isFrameless = false,
        bool initialShow = true
    );
    // 关闭并停止追踪指定的窗口
    void CloseAndReleaseWindow(boost::shared_ptr<Window> window);
    // on dpi changed
    // DPI 变化现在由每个 Window 对象独立处理
    // 使用 window->OnContentScaleChanged 信号监听特定窗口的 DPI 变化
    // 使用 OnScreenScaleFactorChanged 信号监听所有窗口的 DPI 变化
    // get the focused window
    boost::shared_ptr<Window> GetFocusedWindow() const {
        return focusedWindow_;
    }
    // 获取当前所有受管窗口
    const std::vector<boost::shared_ptr<Window>>& GetTrackedWindows() const {
        return trackedWindows_;
    }

    // --- (Thread Marshaling) ---

    /**
     * @brief 将任务投递到 UI 主线程执行 (类似 .NET Dispatcher.BeginInvoke)
     */
    template<typename TaskAction>
    void DispatchToMainThread(TaskAction&& task) {
        // 使用 boost::asio::post 将任务放入队列
        boost::asio::post(mainThreadContext_, std::forward<TaskAction>(task));
    }

    /**
     * @brief 处理主线程任务队列（用于测试或手动轮询）
     * 在实际应用中，这通常在 EnterMainMessageLoop() 中自动调用
     */
    void PollMainThreadTasks();

    /**
     * @brief 执行异步后台任务，并在完成后将结果返回给 UI 线程
     */
    template<typename T, typename TaskAction, typename CallbackAction>
void ExecuteBackgroundTask(boost::shared_ptr<T> contextObject, TaskAction&& task, CallbackAction&& callback) {
    boost::weak_ptr<T> weakContext = contextObject;
    boost::asio::post(backgroundThreadPool_, 
        [this, weakContext, task = std::move(task), callback = std::move(callback)]() mutable {
            try {
                auto result = task(); // 执行耗时任务

                this->DispatchToMainThread([weakContext, result = std::move(result), callback = std::move(callback)]() mutable {
                    // 回到主线程后，检查上下文对象是否还活着
                    if (auto pinnedContext = weakContext.lock()) {
                        callback(pinnedContext, result); 
                    }
                });
            } catch (const std::exception& e) {
                std::cerr << "Background Task Exception: " << e.what() << std::endl;
            }
        });
}

    // --- (Signals) ---
    
    boost::signals2::signal<void()> OnApplicationExit;
    boost::signals2::signal<void(boost::shared_ptr<Window>)> OnWindowCreated;
    boost::signals2::signal<void(boost::shared_ptr<Window>)> OnWindowClosed;
    boost::signals2::signal<void(boost::shared_ptr<Window>, int, int)> OnWindowResized;
    boost::signals2::signal<void(boost::shared_ptr<Window>, bool)> OnWindowFocusChanged;
    boost::signals2::signal<void(boost::shared_ptr<Window>, float xScale, float yScale)> OnScreenScaleFactorChanged;
    
private:
    // 构造与析构私有化
    WindowManager();
    ~WindowManager();

    // 禁止拷贝与移动
    WindowManager(const WindowManager&) = delete;
    WindowManager& operator=(const WindowManager&) = delete;
    WindowManager(WindowManager&&) = delete;
    WindowManager& operator=(WindowManager&&) = delete;

    // --- 内部成员 ---

    // 多窗口追踪容器
    std::vector<boost::shared_ptr<Window>> trackedWindows_;
    // 异步任务上下文 (解决你提到的未声明问题)
    boost::asio::io_context mainThreadContext_;
    // 工作保持器：防止 io_context 在没有任务时自动退出
    // 在 Boost 1.66+ 推荐使用 executor_work_guard
    typedef boost::asio::executor_work_guard<boost::asio::io_context::executor_type> io_worker;
    std::unique_ptr<io_worker> internalWorker_;
    boost::shared_ptr<Window> focusedWindow_;
    boost::asio::thread_pool backgroundThreadPool_{ 4 }; // 4 threads for background tasks
    bool isLoopRunning_ = false;
    bool shouldExit_ = false;
};

} // namespace foundation
} // namespace KiUI

#endif // KIUI_FOUNDATION_WINDOW_MANAGER_HPP