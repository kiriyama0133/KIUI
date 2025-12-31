// 必须在任何 Windows 头文件之前定义这些宏，避免与 boost/asio 冲突
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

#include "logger.hpp"
#include "hpps/window.hpp"
#include "hpps/window_class.hpp"
#include <iostream>
namespace KiUI{
    namespace foundation{

        // Initialize Platform Subsystems
        static void GlfwErrorCallback(int error, const char* description) {
        std::cerr << "GLFW Error [" << error << "]: " << description << std::endl;
    }
        WindowManager& WindowManager::GetSharedInstance(){
            static WindowManager instance;
            return instance;
        }
        WindowManager::WindowManager() 
        : isLoopRunning_(false), shouldExit_(false){
            internalWorker_ = std::make_unique<io_worker>(boost::asio::make_work_guard(mainThreadContext_));
        }
        WindowManager::~WindowManager() {
            ShutdownPlatformSubsystems();
        }
        float WindowManager::GetWindowContentScale(boost::shared_ptr<Window> window) const{
            if (!window) return 1.0f;
            float xScale, yScale = 1.0f;
            glfwGetWindowContentScale(window->GetHandle(), &xScale, &yScale);
            return xScale;
        }
        bool WindowManager::InitializePlatformSubsystems(){
            glfwSetErrorCallback(GlfwErrorCallback);
            if(!glfwInit()){
                std::cerr << "Failed to initialize GLFW" << std::endl;
                return false;
            }
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // could not use OpenGL API
            #ifdef __APPLE__
                glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
            #endif
            return true;
    }
        void WindowManager::ShutdownPlatformSubsystems(){
            trackedWindows_.clear();
            // Terminate GLFW (it's safe to call even if not initialized)
            glfwTerminate();
        }

        void WindowManager::UpdateActiveWindow(boost::shared_ptr<Window> window, bool focused) {
            if (!window) {
                return; // 防止空指针访问
            }
            
            if (focused) {
                focusedWindow_ = window;
            } else if (focusedWindow_ == window) {
                focusedWindow_ = nullptr;
            }
            OnWindowFocusChanged(window, focused);
        }

        void WindowManager::PollMainThreadTasks() {
            mainThreadContext_.poll();
        }

        void WindowManager::Show(boost::shared_ptr<Window> window){
            glfwShowWindow(window->GetHandle());
        }

        void WindowManager::Hide(boost::shared_ptr<Window> window){
            glfwHideWindow(window->GetHandle());
        }

        bool WindowManager::IsVisible(boost::shared_ptr<Window> window) const{
            return glfwGetWindowAttrib(window->GetHandle(), GLFW_VISIBLE) != 0;
        }

        boost::shared_ptr<Window> WindowManager::CreateNativeWindow(
            const std::string& windowTitle,
            int width,
            int height,
            bool isFrameless,
            bool initialShow) {
            glfwDefaultWindowHints(); // set the default window hints
            if (isFrameless){
                glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
            }
            GLFWwindow* glfwWindow = glfwCreateWindow(width, height, windowTitle.c_str(), nullptr, nullptr);
            if (!glfwWindow) {
                std::cerr << "Failed to create GLFW window" << std::endl;
                return nullptr;
            }
            // 创建 Window 对象并追踪
            auto window = boost::make_shared<Window>(glfwWindow, isFrameless);
            trackedWindows_.push_back(window);
            
            boost::weak_ptr<Window> weakWin = window;
            window->OnContentScaleChanged.connect([this, weakWin](float xScale, float yScale) {
                if (auto pinnedWin = weakWin.lock()) {
                    this->OnScreenScaleFactorChanged(pinnedWin, xScale, yScale);
                    #ifndef NDEBUG
                    Logger::Debug("WindowManager::OnScreenScaleFactorChanged: xScale = {0}, yScale = {1}", xScale, yScale);
                    #endif
                }
            });
            // 窗口重绘
            window->OnInvalidate.connect([this, weakWin]() {
            });
            
            // 当窗口获得或失去焦点时，更新 WindowManager 的焦点窗口状态
            window->OnFocusChanged.connect([this, weakWin](bool focused) {
                if (auto pinnedWin = weakWin.lock()) {
                    this->UpdateActiveWindow(pinnedWin, focused);
                }
            });
            
            if (initialShow){
                glfwShowWindow(glfwWindow);
            }
            // 检查窗口创建时的实际焦点状态（窗口显示后可能会自动获得焦点）
            // 通过轮询一次事件来确保焦点回调被触发
            glfwPollEvents();
            // 如果窗口已经有焦点，手动更新焦点状态（因为回调可能已经触发，但确保状态同步）
            if (glfwGetWindowAttrib(glfwWindow, GLFW_FOCUSED)) {
                UpdateActiveWindow(window, true);
            }
            // 发出信号
            OnWindowCreated(window);
            
            return window;
        }

        void WindowManager::CloseAndReleaseWindow(boost::shared_ptr<Window> window) {
            if (!window) {
                return;
            }

            // 从追踪列表中移除
            auto it = std::find(trackedWindows_.begin(), trackedWindows_.end(), window);
            if (it != trackedWindows_.end()) {
                OnWindowClosed(window);
                trackedWindows_.erase(it);
            }
        }

        void WindowManager::RequestApplicationExit() {
            shouldExit_ = true;
            mainThreadContext_.stop();
        }

        // Enter Main Message Loop
        void WindowManager::EnterMainMessageLoop(){
            if (isLoopRunning_) return;
            isLoopRunning_ = true;
            shouldExit_ = false;           
            while (!shouldExit_ && !trackedWindows_.empty()){
                // 处理 GLFW 事件
                glfwPollEvents();
                // 处理异步任务队列（非阻塞）
                mainThreadContext_.poll();
                // 更新和渲染所有窗口
                for (auto it = trackedWindows_.begin(); it != trackedWindows_.end();){
                    auto& window = *it;
                    if (window->ShouldClose()) {
                        OnWindowClosed(window);
                        it = trackedWindows_.erase(it);
                        continue;
                    }
                    if (IsVisible(window)){
                    window->OnUpdate();
                        window->OnRender();
                    }
                    ++it;
                }
            }
            isLoopRunning_ = false;
        }

    } // namespace foundation
} // namespace KiUI