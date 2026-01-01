#include <gtest/gtest.h>
#include "Box.hpp"
#include <boost/make_shared.hpp>

namespace KiUI {
namespace widget {

// 测试辅助函数：创建一个简单的 Box
boost::shared_ptr<Box> CreateBox(float x, float y, float width, float height) {
    auto box = boost::make_shared<Box>();
    box->SetLeft(x);
    box->SetTop(y);
    box->SetWidth(width);
    box->SetHeight(height);
    box->SetVisibility(true);
    box->SetOpacity(1.0f);
    return box;
}

// 测试单个矩形的命中测试
TEST(HitTestTest, SingleRectangle) {
    auto box = CreateBox(10.0f, 20.0f, 100.0f, 50.0f);
    
    // 点在矩形内部
    auto hit1 = box->HitTest(50.0f, 40.0f);
    ASSERT_NE(hit1, nullptr);
    EXPECT_EQ(hit1, box);
    
    // 点在矩形外部
    auto hit2 = box->HitTest(5.0f, 15.0f);
    EXPECT_EQ(hit2, nullptr);
    
    // 点在矩形边界上（左边界）
    auto hit3 = box->HitTest(10.0f, 45.0f);
    ASSERT_NE(hit3, nullptr);
    EXPECT_EQ(hit3, box);
    
    // 点在矩形边界上（右边界）
    auto hit4 = box->HitTest(110.0f, 45.0f);
    ASSERT_NE(hit4, nullptr);
    EXPECT_EQ(hit4, box);
    
    // 点在矩形边界上（上边界）
    auto hit5 = box->HitTest(50.0f, 20.0f);
    ASSERT_NE(hit5, nullptr);
    EXPECT_EQ(hit5, box);
    
    // 点在矩形边界上（下边界）
    auto hit6 = box->HitTest(50.0f, 70.0f);
    ASSERT_NE(hit6, nullptr);
    EXPECT_EQ(hit6, box);
}

// 测试父子关系的矩形（父包裹子）
TEST(HitTestTest, ParentChildRelationship) {
    // 父矩形：位置(0, 0)，大小(200, 200)
    auto parent = CreateBox(0.0f, 0.0f, 200.0f, 200.0f);
    
    // 子矩形：位置(50, 50)，大小(100, 100)
    auto child = CreateBox(50.0f, 50.0f, 100.0f, 100.0f);
    parent->AddChild(child);
    
    // 点在子矩形内部（相对于父的坐标是 100, 100）
    auto hit1 = parent->HitTest(100.0f, 100.0f);
    ASSERT_NE(hit1, nullptr);
    EXPECT_EQ(hit1, child);  // 应该返回最深层命中的子元素
    
    // 点在父矩形内但不在子矩形内（相对于父的坐标是 25, 25）
    auto hit2 = parent->HitTest(25.0f, 25.0f);
    ASSERT_NE(hit2, nullptr);
    EXPECT_EQ(hit2, parent);  // 应该返回父元素
    
    // 点在父矩形外部
    auto hit3 = parent->HitTest(250.0f, 250.0f);
    EXPECT_EQ(hit3, nullptr);
}

// 测试多层嵌套的矩形
TEST(HitTestTest, MultiLevelNesting) {
    // 第一层：位置(0, 0)，大小(300, 300)
    auto level1 = CreateBox(0.0f, 0.0f, 300.0f, 300.0f);
    
    // 第二层：位置(50, 50)，大小(200, 200)
    auto level2 = CreateBox(50.0f, 50.0f, 200.0f, 200.0f);
    level1->AddChild(level2);
    
    // 第三层：位置(50, 50)，大小(100, 100)
    auto level3 = CreateBox(50.0f, 50.0f, 100.0f, 100.0f);
    level2->AddChild(level3);
    
    // 点在第三层内部（相对于第一层的坐标是 150, 150）
    // level1(0,0) -> level2(50,50) -> level3(50,50)
    // 相对于level1: 150,150 -> 相对于level2: 100,100 -> 相对于level3: 50,50 (在level3内)
    auto hit1 = level1->HitTest(150.0f, 150.0f);
    ASSERT_NE(hit1, nullptr);
    EXPECT_EQ(hit1, level3);  // 应该返回最深层命中的第三层元素
    
    // 点在第二层内但不在第三层内（相对于第一层的坐标）
    // level1(0,0) -> level2(50,50) -> level3(50,50)
    // level2范围相对于level1: [50,50]到[250,250]
    // level3范围相对于level1: [100,100]到[200,200]
    // 选择点(75,75)相对于level1: 相对于level2是(25,25)在level2内，相对于level3是(-25,-25)不在level3内
    auto hit2 = level1->HitTest(75.0f, 75.0f);
    ASSERT_NE(hit2, nullptr);
    EXPECT_EQ(hit2, level2);  // 应该返回第二层元素
    
    // 点在第一层内但不在第二层内（相对于第一层的坐标是 25, 25）
    auto hit3 = level1->HitTest(25.0f, 25.0f);
    ASSERT_NE(hit3, nullptr);
    EXPECT_EQ(hit3, level1);  // 应该返回第一层元素
}

// 测试重叠的兄弟矩形（后画的在上层）
TEST(HitTestTest, OverlappingSiblings) {
    // 父矩形：位置(0, 0)，大小(300, 300)
    auto parent = CreateBox(0.0f, 0.0f, 300.0f, 300.0f);
    
    // 第一个子矩形：位置(50, 50)，大小(100, 100)
    auto child1 = CreateBox(50.0f, 50.0f, 100.0f, 100.0f);
    parent->AddChild(child1);
    
    // 第二个子矩形：位置(100, 100)，大小(100, 100)，与第一个重叠
    auto child2 = CreateBox(100.0f, 100.0f, 100.0f, 100.0f);
    parent->AddChild(child2);
    
    // 点在重叠区域（相对于父的坐标是 150, 150）
    // 由于 child2 是后添加的，应该返回 child2
    auto hit1 = parent->HitTest(150.0f, 150.0f);
    ASSERT_NE(hit1, nullptr);
    EXPECT_EQ(hit1, child2);  // 后画的在上层
    
    // 点在 child1 的非重叠区域（相对于父的坐标是 75, 75）
    auto hit2 = parent->HitTest(75.0f, 75.0f);
    ASSERT_NE(hit2, nullptr);
    EXPECT_EQ(hit2, child1);
    
    // 点在 child2 的非重叠区域（相对于父的坐标是 180, 180）
    auto hit3 = parent->HitTest(180.0f, 180.0f);
    ASSERT_NE(hit3, nullptr);
    EXPECT_EQ(hit3, child2);
}

// 测试不可见元素的命中测试
TEST(HitTestTest, InvisibleElement) {
    auto box = CreateBox(10.0f, 20.0f, 100.0f, 50.0f);
    box->SetVisibility(false);
    
    // 即使点在矩形内部，也不应该命中
    auto hit = box->HitTest(50.0f, 40.0f);
    EXPECT_EQ(hit, nullptr);
}

// 测试透明度为0的元素的命中测试
TEST(HitTestTest, ZeroOpacityElement) {
    auto box = CreateBox(10.0f, 20.0f, 100.0f, 50.0f);
    box->SetOpacity(0.0f);
    
    // 即使点在矩形内部，也不应该命中
    auto hit = box->HitTest(50.0f, 40.0f);
    EXPECT_EQ(hit, nullptr);
}

// 测试复杂场景：多层嵌套 + 重叠兄弟
TEST(HitTestTest, ComplexScenario) {
    // 根容器：位置(0, 0)，大小(500, 500)
    auto root = CreateBox(0.0f, 0.0f, 500.0f, 500.0f);
    
    // 第一层子容器：位置(100, 100)，大小(300, 300)
    auto container1 = CreateBox(100.0f, 100.0f, 300.0f, 300.0f);
    root->AddChild(container1);
    
    // 第二层子容器：位置(50, 50)，大小(200, 200)
    auto container2 = CreateBox(50.0f, 50.0f, 200.0f, 200.0f);
    container1->AddChild(container2);
    
    // 第三层的第一个子元素：位置(25, 25)，大小(75, 75)
    auto child1 = CreateBox(25.0f, 25.0f, 75.0f, 75.0f);
    container2->AddChild(child1);
    
    // 第三层的第二个子元素：位置(50, 50)，大小(75, 75)，与第一个重叠
    auto child2 = CreateBox(50.0f, 50.0f, 75.0f, 75.0f);
    container2->AddChild(child2);
    
    // 点在 child2 内部（相对于根的坐标）
    // 路径：root(0,0) -> container1(100,100) -> container2(50,50) -> child2(50,50)
    // 计算：100+50+50+37.5=237.5 (child2中心点)
    auto hit1 = root->HitTest(237.5f, 237.5f);
    ASSERT_NE(hit1, nullptr);
    EXPECT_EQ(hit1, child2);  // 应该返回最深层且最后添加的元素
    
    // 点在 child1 的非重叠区域（相对于根的坐标）
    // 路径：root(0,0) -> container1(100,100) -> container2(50,50) -> child1(25,25)
    // child1范围相对于root: [175,175]到[250,250]
    // child2范围相对于root: [200,200]到[275,275]
    // 选择child1中心点(212.5,212.5)相对于root: 相对于container1是(112.5,112.5) -> 相对于container2是(62.5,62.5) -> 相对于child1是(37.5,37.5)在child1内
    // 但child2在(50,50)相对于container2，范围是[200,200]到[275,275]相对于root
    // (212.5,212.5)相对于container2是(62.5,62.5)，相对于child2是(12.5,12.5)，也在child2内！
    // 由于child2是后添加的，应该返回child2。我们需要找一个只在child1内的点
    // child1范围[25,25]到[100,100]相对于container2，即[175,175]到[250,250]相对于root
    // child2范围[50,50]到[125,125]相对于container2，即[200,200]到[275,275]相对于root
    // 重叠区域是[200,200]到[250,250]相对于root
    // child1的非重叠区域是[175,175]到[200,200]相对于root
    // 选择点(187.5,187.5)相对于root: 相对于container1是(87.5,87.5) -> 相对于container2是(37.5,37.5) -> 相对于child1是(12.5,12.5)在child1内
    // 相对于child2是(37.5-50,37.5-50)=(-12.5,-12.5)不在child2内
    auto hit2 = root->HitTest(187.5f, 187.5f);
    ASSERT_NE(hit2, nullptr);
    EXPECT_EQ(hit2, child1);
    
    // 点在 container2 内但不在任何子元素内（相对于根的坐标）
    // 路径：root(0,0) -> container1(100,100) -> container2(50,50)
    // 计算：100+50+150=300 (container2中心点，但不在child范围内)
    auto hit3 = root->HitTest(300.0f, 300.0f);
    ASSERT_NE(hit3, nullptr);
    EXPECT_EQ(hit3, container2);  // 应该返回 container2
}

// 测试边界情况：点在元素边界上
TEST(HitTestTest, BoundaryCases) {
    auto box = CreateBox(10.0f, 20.0f, 100.0f, 50.0f);
    
    // 左上角
    auto hit1 = box->HitTest(10.0f, 20.0f);
    ASSERT_NE(hit1, nullptr);
    EXPECT_EQ(hit1, box);
    
    // 右下角
    auto hit2 = box->HitTest(110.0f, 70.0f);
    ASSERT_NE(hit2, nullptr);
    EXPECT_EQ(hit2, box);
    
    // 刚好在外部（右边界外1像素）
    auto hit3 = box->HitTest(111.0f, 45.0f);
    EXPECT_EQ(hit3, nullptr);
    
    // 刚好在外部（下边界外1像素）
    auto hit4 = box->HitTest(50.0f, 71.0f);
    EXPECT_EQ(hit4, nullptr);
}

} // namespace widget
} // namespace KiUI

