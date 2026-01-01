#include "RenderContext.hpp"
#include "RenderSurface.hpp"
#include "Box.hpp"
#include <window.hpp>
#include <window_class.hpp>
#include <GLFW/glfw3.h>
#include <core/SkCanvas.h>
#include <iostream>
#include <tracy/Tracy.hpp>
#include <boost/shared_ptr.hpp>

using namespace KiUI;
using namespace KiUI::graphics;
using namespace KiUI::widget;
using namespace KiUI::foundation;

int main() {
    ZoneScoped;
    
    // 获取窗口管理器单例
    auto& windowManager = WindowManager::GetSharedInstance();
    if (!windowManager.InitializePlatformSubsystems()) {
        std::cerr << "Failed to initialize platform subsystems" << std::endl;
        return 1;
    }
    
    // 创建窗口
    auto window = windowManager.CreateNativeWindow("Layout Test", 800, 600);
    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        return 1;
    }
    
    // 创建渲染上下文
    auto renderContext = boost::make_shared<RenderContext>();
    if (!renderContext->Initialize()) {
        std::cerr << "Failed to initialize render context" << std::endl;
        return 1;
    }
    
    // 创建渲染表面
    auto renderSurface = boost::make_shared<RenderSurface>(renderContext, window);
    if (!renderSurface->Initialize()) {
        std::cerr << "Failed to initialize render surface" << std::endl;
        return 1;
    }
    
    // 创建 UI 组件树
    // 1. 根容器 - 占满整个窗口
    auto root = boost::make_shared<widget::Box>();
    root->SetWidth(800.0f);
    root->SetHeight(600.0f);
    root->SetBackgroundColor(SK_ColorGRAY);
    root->SetPadding(Padding::All, 20.0f);
    
    // 2. 居中矩形
    auto centerRect = boost::make_shared<widget::Box>();
    centerRect->SetWidth(300.0f);
    centerRect->SetHeight(200.0f);
    centerRect->SetBackgroundColor(SK_ColorBLUE);
    centerRect->SetBorderColor(SK_ColorWHITE);
    centerRect->SetBorderWidth(BorderWidth::All, 2.0f);
    centerRect->SetAlignment(Alignment::Center);
    centerRect->SetJustification(Justification::Center);
    root->AddChild(centerRect);
    
    // 3. 靠右的子组件
    auto rightRect = boost::make_shared<widget::Box>();
    rightRect->SetWidth(150.0f);
    rightRect->SetHeight(100.0f);
    rightRect->SetBackgroundColor(SK_ColorRED);
    rightRect->SetBorderColor(SK_ColorYELLOW);
    rightRect->SetBorderWidth(BorderWidth::All, 3.0f);
    rightRect->SetMargin(Margin::Right, 10.0f);
    rightRect->SetAlignment(Alignment::End);
    rightRect->SetJustification(Justification::Start);
    root->AddChild(rightRect);
    
    // 4. 靠左的子组件
    auto leftRect = boost::make_shared<widget::Box>();
    leftRect->SetWidth(120.0f);
    leftRect->SetHeight(80.0f);
    leftRect->SetBackgroundColor(SK_ColorGREEN);
    leftRect->SetBorderColor(SK_ColorCYAN);
    leftRect->SetBorderWidth(BorderWidth::All, 2.0f);
    leftRect->SetMargin(Margin::Left, 10.0f);
    leftRect->SetAlignment(Alignment::Start);
    leftRect->SetJustification(Justification::Start);
    root->AddChild(leftRect);
    
    // 5. 在居中矩形内部添加子组件 - 测试嵌套布局
    auto centerChild1 = boost::make_shared<widget::Box>();
    centerChild1->SetWidth(100.0f);
    centerChild1->SetHeight(50.0f);
    centerChild1->SetBackgroundColor(SK_ColorYELLOW);
    centerChild1->SetBorderColor(SK_ColorBLACK);
    centerChild1->SetBorderWidth(BorderWidth::All, 1.0f);
    centerChild1->SetMargin(Margin::All, 5.0f);
    centerChild1->SetAlignment(Alignment::Center);
    centerChild1->SetJustification(Justification::Center);
    centerRect->AddChild(centerChild1);
    
    auto centerChild2 = boost::make_shared<widget::Box>();
    centerChild2->SetWidth(80.0f);
    centerChild2->SetHeight(40.0f);
    centerChild2->SetBackgroundColor(SK_ColorMAGENTA);
    centerChild2->SetBorderColor(SK_ColorWHITE);
    centerChild2->SetBorderWidth(BorderWidth::All, 1.0f);
    centerChild2->SetMargin(Margin::Top, 10.0f);
    centerChild2->SetAlignment(Alignment::End);
    centerChild2->SetJustification(Justification::End);
    centerRect->AddChild(centerChild2);
    
    // 6. 在靠右矩形内部添加子组件
    auto rightChild = boost::make_shared<widget::Box>();
    rightChild->SetWidth(60.0f);
    rightChild->SetHeight(40.0f);
    rightChild->SetBackgroundColor(SK_ColorCYAN);
    rightChild->SetBorderColor(SK_ColorBLACK);
    rightChild->SetBorderWidth(BorderWidth::All, 1.0f);
    rightChild->SetMargin(Margin::All, 5.0f);
    rightChild->SetAlignment(Alignment::Start);
    rightChild->SetJustification(Justification::Start);
    rightRect->AddChild(rightChild);
    
    // 7. 在靠左矩形内部添加嵌套子组件（测试三层嵌套）
    auto leftChild = boost::make_shared<widget::Box>();
    leftChild->SetWidth(80.0f);
    leftChild->SetHeight(40.0f);
    leftChild->SetBackgroundColor(SK_ColorWHITE);
    leftChild->SetBorderColor(SK_ColorBLACK);
    leftChild->SetBorderWidth(BorderWidth::All, 1.0f);
    leftChild->SetMargin(Margin::All, 5.0f);
    leftChild->SetAlignment(Alignment::Center);
    leftChild->SetJustification(Justification::Center);
    leftRect->AddChild(leftChild);
    
    // 计算布局（递归计算所有子组件）
    root->CalculateLayout(800.0f, 600.0f, 0.0f, 0.0f);
    
    std::cout << "Layout Test Started" << std::endl;
    std::cout << "Root: " << root->GetWidth() << "x" << root->GetHeight() << std::endl;
    std::cout << "Center Rect: " << centerRect->GetLeft() << "," << centerRect->GetTop() 
              << " " << centerRect->GetWidth() << "x" << centerRect->GetHeight() << std::endl;
    std::cout << "  Center Child1: " << centerChild1->GetLeft() << "," << centerChild1->GetTop() 
              << " " << centerChild1->GetWidth() << "x" << centerChild1->GetHeight() << std::endl;
    std::cout << "  Center Child2: " << centerChild2->GetLeft() << "," << centerChild2->GetTop() 
              << " " << centerChild2->GetWidth() << "x" << centerChild2->GetHeight() << std::endl;
    std::cout << "Right Rect: " << rightRect->GetLeft() << "," << rightRect->GetTop() 
              << " " << rightRect->GetWidth() << "x" << rightRect->GetHeight() << std::endl;
    std::cout << "  Right Child: " << rightChild->GetLeft() << "," << rightChild->GetTop() 
              << " " << rightChild->GetWidth() << "x" << rightChild->GetHeight() << std::endl;
    std::cout << "Left Rect: " << leftRect->GetLeft() << "," << leftRect->GetTop() 
              << " " << leftRect->GetWidth() << "x" << leftRect->GetHeight() << std::endl;
    std::cout << "  Left Child: " << leftChild->GetLeft() << "," << leftChild->GetTop() 
              << " " << leftChild->GetWidth() << "x" << leftChild->GetHeight() << std::endl;
    
    // 递归渲染函数
    std::function<void(boost::shared_ptr<VisualElement>, SkCanvas*, float, float)> renderElement =
        [&](boost::shared_ptr<VisualElement> element, SkCanvas* canvas, float offsetX, float offsetY) {
            if (!element || !element->GetVisibility()) {
                return;
            }
            
            // 保存画布状态
            canvas->save();
            
            // 移动到元素位置
            float x = offsetX + element->GetLeft();
            float y = offsetY + element->GetTop();
            canvas->translate(x, y);
            
            // 渲染元素
            element->Render(canvas);
            
            // 递归渲染子元素
            for (const auto& child : element->GetChildren()) {
                auto visualChild = boost::dynamic_pointer_cast<VisualElement>(child);
                if (visualChild) {
                    renderElement(visualChild, canvas, 0.0f, 0.0f); // 子元素位置已经是相对于父元素的
                }
            }
            
            // 恢复画布状态
            canvas->restore();
        };
    
    // 主循环
    while (!window->ShouldClose()) {
        FrameMark;
        ZoneScopedN("Main Loop");
        
        // 开始绘制
        {
            ZoneScopedN("BeginFrame");
            auto canvasOpt = renderSurface->BeginFrame();
            if (canvasOpt) {
                ZoneScopedN("Render UI");
                SkCanvas& canvas = canvasOpt->get();
                
                // 渲染 UI 树
                renderElement(root, &canvas, 0.0f, 0.0f);
            }
        }
        
        // 结束绘制并显示
        {
            ZoneScopedN("EndFrame");
            renderSurface->EndFrame();
        }
        
        // 处理事件
        {
            ZoneScopedN("Event Processing");
            glfwPollEvents();
            windowManager.PollMainThreadTasks();
        }
    }
    
    // 清理资源
    renderSurface->Destroy();
    renderContext->Shutdown();
    windowManager.ShutdownPlatformSubsystems();
    
    return 0;
}

