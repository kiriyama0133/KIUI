#include "hpps/window.hpp"
#include "hpps/window_class.hpp"
#include <iostream>
#include <algorithm>
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
            glfwTerminate();
        }

        boost::shared_ptr<Window> WindowManager::CreateNativeWindow(
            const std::string& windowTitle,
            int width,
            int height,
            bool isFrameless) {
            
            GLFWwindow* glfwWindow = glfwCreateWindow(width, height, windowTitle.c_str(), nullptr, nullptr);
            if (!glfwWindow) {
                std::cerr << "Failed to create GLFW window" << std::endl;
                return nullptr;
            }

            // 设置窗口属性
            if (isFrameless) {
                glfwSetWindowAttrib(glfwWindow, GLFW_DECORATED, GLFW_FALSE);
            }

            // 创建 Window 对象并追踪
            auto window = boost::make_shared<Window>(glfwWindow);
            trackedWindows_.push_back(window);
            
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
                    window->OnUpdate();
                    window->OnRender();
                    ++it;
                }
            }
            
            isLoopRunning_ = false;
        }

    } // namespace foundation
} // namespace KiUI