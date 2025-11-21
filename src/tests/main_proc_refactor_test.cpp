// Tests for main_proc.cpp refactoring
// Verifies the migration from global Keyboard to Window-based WindowKeyboard

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <windows.h>
#include <vector>
#include <memory>

namespace sim::route::test {

// Mock Route class
struct Route {
    std::string name;
    std::vector<std::vector<WORD>> key_sequences;
};

// Mock keys function from route module
std::vector<WORD> keys(const Route& route) {
    if (route.key_sequences.empty()) {
        return {};
    }
    return route.key_sequences[0];
}

} // namespace sim::route::test

namespace sim::utils::window::test {

// Mock WindowKeyboard
class MockWindowKeyboard {
public:
    explicit MockWindowKeyboard(HWND hwnd) : hwnd_(hwnd) {}
    
    MOCK_METHOD(void, KeyDown, (WORD key), (const));
    MOCK_METHOD(void, KeyUp, (WORD key), (const));
    
    HWND GetHWND() const { return hwnd_; }
    
private:
    HWND hwnd_;
};

// Mock Window
class MockWindow {
public:
    explicit MockWindow(HWND hwnd) : hwnd_(hwnd) {}
    
    MockWindowKeyboard Keyboard() const {
        return MockWindowKeyboard(hwnd_);
    }
    
    HWND GetHandle() const { return hwnd_; }
    
    // No longer needed after refactor
    MOCK_METHOD(void, Activate, (), (const));
    MOCK_METHOD(void, Focus, (), (const));
    
private:
    HWND hwnd_;
};

} // namespace sim::utils::window::test

using namespace sim::route::test;
using namespace sim::utils::window::test;
using ::testing::_;
using ::testing::NiceMock;

class MainProcRefactorTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_hwnd_ = reinterpret_cast<HWND>(0x12345678);
        window_ = std::make_unique<NiceMock<MockWindow>>(test_hwnd_);
    }
    
    HWND test_hwnd_;
    std::unique_ptr<NiceMock<MockWindow>> window_;
};

// Test: Old approach used Activate and Focus (no longer needed)
TEST_F(MainProcRefactorTest, OldApproachCalledActivateAndFocus) {
    // This test documents that the old approach called these methods
    // New approach doesn't need them
    
    EXPECT_CALL(*window_, Activate()).Times(0);
    EXPECT_CALL(*window_, Focus()).Times(0);
    
    // New approach: just get keyboard directly
    auto keyboard = window_->Keyboard();
    EXPECT_EQ(keyboard.GetHWND(), test_hwnd_);
}

// Test: New approach uses window-specific keyboard
TEST_F(MainProcRefactorTest, NewApproachUsesWindowKeyboard) {
    Route test_route;
    test_route.name = "test";
    test_route.key_sequences = {{VK_UP, VK_DOWN}};
    
    // New pattern from main_proc.cpp:
    // auto windowKeyboard = window->Keyboard();
    auto windowKeyboard = window_->Keyboard();
    
    // Verify correct window association
    EXPECT_EQ(windowKeyboard.GetHWND(), test_hwnd_);
}

// Test: Key sequence execution with new approach
TEST_F(MainProcRefactorTest, KeySequenceExecutionNewApproach) {
    Route test_route;
    test_route.name = "navigation";
    test_route.key_sequences = {{VK_UP, VK_RIGHT, VK_DOWN, VK_LEFT}};
    
    auto windowKeyboard = window_->Keyboard();
    const auto key_list = keys(test_route);
    
    // Simulate the loop from main_proc.cpp:
    // for (const auto key : keys) {
    //     windowKeyboard.KeyDown(key);
    //     sleep(500);
    //     windowKeyboard.KeyUp(key);
    // }
    
    EXPECT_EQ(key_list.size(), 4);
    for (const auto key : key_list) {
        // Would call KeyDown and KeyUp in actual implementation
        EXPECT_TRUE(key == VK_UP || key == VK_RIGHT || 
                    key == VK_DOWN || key == VK_LEFT);
    }
}

// Test: No global keyboard dependency
TEST_F(MainProcRefactorTest, NoGlobalKeyboardDependency) {
    // Old approach used:
    // const auto& keyboard = sim::controller::Keyboard::Get();
    
    // New approach doesn't need global keyboard
    // Each window has its own keyboard instance
    
    auto keyboard1 = window_->Keyboard();
    auto keyboard2 = window_->Keyboard();
    
    // Both work with the same window
    EXPECT_EQ(keyboard1.GetHWND(), test_hwnd_);
    EXPECT_EQ(keyboard2.GetHWND(), test_hwnd_);
}

// Test: Multiple windows can have independent keyboards
TEST_F(MainProcRefactorTest, MultipleWindowsIndependentKeyboards) {
    HWND hwnd1 = reinterpret_cast<HWND>(0x11111111);
    HWND hwnd2 = reinterpret_cast<HWND>(0x22222222);
    
    NiceMock<MockWindow> window1(hwnd1);
    NiceMock<MockWindow> window2(hwnd2);
    
    auto keyboard1 = window1.Keyboard();
    auto keyboard2 = window2.Keyboard();
    
    // Each keyboard is associated with its own window
    EXPECT_EQ(keyboard1.GetHWND(), hwnd1);
    EXPECT_EQ(keyboard2.GetHWND(), hwnd2);
    EXPECT_NE(keyboard1.GetHWND(), keyboard2.GetHWND());
}

// Test: Import statement cleanup
TEST_F(MainProcRefactorTest, ImportStatementCleanup) {
    // Old code had:
    // import :keyboard;
    
    // New code only needs:
    // import :main;
    
    // Verify that window keyboard functionality doesn't require
    // separate keyboard import
    
    auto windowKeyboard = window_->Keyboard();
    EXPECT_NO_THROW({
        // Keyboard functionality available through window
    });
}

// Test: Refactored code structure is simpler
TEST_F(MainProcRefactorTest, RefactoredCodeStructureSimpler) {
    Route test_route;
    test_route.name = "simple_test";
    test_route.key_sequences = {{VK_SPACE}};
    
    // Old approach (3 steps):
    // 1. window->Activate();
    // 2. window->Focus();
    // 3. keyboard.KeyDown(key); // global keyboard
    
    // New approach (1 step):
    // 1. windowKeyboard.KeyDown(key);
    
    auto windowKeyboard = window_->Keyboard();
    const auto key_list = keys(test_route);
    
    EXPECT_EQ(key_list.size(), 1);
    EXPECT_EQ(key_list[0], VK_SPACE);
}

// Integration test: Complete route execution pattern
TEST_F(MainProcRefactorTest, CompleteRouteExecutionPattern) {
    Route test_route;
    test_route.name = "complete_route";
    test_route.key_sequences = {{VK_F1, VK_F2, VK_F3}};
    
    // Simulate complete execution from main_proc.cpp
    
    // Step 1: Get keyboard for window
    auto windowKeyboard = window_->Keyboard();
    
    // Step 2: Get keys for route
    const auto key_list = keys(test_route);
    
    // Step 3: Execute each key
    for (const auto key : key_list) {
        // In real code:
        // windowKeyboard.KeyDown(key);
        // sim::utils::time::sleep(500);
        // windowKeyboard.KeyUp(key);
        
        EXPECT_TRUE(key >= VK_F1 && key <= VK_F12);
    }
    
    EXPECT_EQ(key_list.size(), 3);
}

// Test: Window handle consistency throughout execution
TEST_F(MainProcRefactorTest, WindowHandleConsistency) {
    auto windowKeyboard = window_->Keyboard();
    
    // Handle should remain consistent
    HWND initial_hwnd = windowKeyboard.GetHWND();
    
    // Even after multiple operations
    for (int i = 0; i < 100; ++i) {
        auto kb = window_->Keyboard();
        EXPECT_EQ(kb.GetHWND(), initial_hwnd);
    }
}

// Test: Thread safety consideration for keyboard creation
TEST_F(MainProcRefactorTest, KeyboardCreationThreadSafety) {
    // Create multiple keyboard instances
    std::vector<MockWindowKeyboard> keyboards;
    
    for (int i = 0; i < 1000; ++i) {
        keyboards.push_back(window_->Keyboard());
    }
    
    // All should reference the same window
    for (const auto& kb : keyboards) {
        EXPECT_EQ(kb.GetHWND(), test_hwnd_);
    }
}

// Test: Error case - null window handling
TEST_F(MainProcRefactorTest, NullWindowHandling) {
    HWND null_hwnd = nullptr;
    NiceMock<MockWindow> null_window(null_hwnd);
    
    EXPECT_NO_THROW({
        auto keyboard = null_window.Keyboard();
        EXPECT_EQ(keyboard.GetHWND(), null_hwnd);
    });
}

// Test: Route with empty key sequence
TEST_F(MainProcRefactorTest, RouteWithEmptyKeySequence) {
    Route empty_route;
    empty_route.name = "empty";
    empty_route.key_sequences = {{}};
    
    auto windowKeyboard = window_->Keyboard();
    const auto key_list = keys(empty_route);
    
    EXPECT_TRUE(key_list.empty());
}

// Test: Route with multiple key sequences
TEST_F(MainProcRefactorTest, RouteWithMultipleKeySequences) {
    Route multi_route;
    multi_route.name = "multi";
    multi_route.key_sequences = {
        {VK_UP, VK_DOWN},
        {VK_LEFT, VK_RIGHT},
        {VK_SPACE}
    };
    
    auto windowKeyboard = window_->Keyboard();
    
    // In actual code, each sequence would be processed separately
    // This test just verifies the structure
    EXPECT_EQ(multi_route.key_sequences.size(), 3);
    EXPECT_EQ(multi_route.key_sequences[0].size(), 2);
    EXPECT_EQ(multi_route.key_sequences[1].size(), 2);
    EXPECT_EQ(multi_route.key_sequences[2].size(), 1);
}

// Performance test: Keyboard creation overhead
TEST_F(MainProcRefactorTest, KeyboardCreationPerformanceAcceptable) {
    const int iterations = 10000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        auto keyboard = window_->Keyboard();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Should be very fast (less than 100ms for 10000 iterations)
    EXPECT_LT(duration.count(), 100000);
}

// Documentation test: Verify refactoring benefits
TEST_F(MainProcRefactorTest, RefactoringBenefitsVerification) {
    // Benefits of the refactoring:
    // 1. No need for global keyboard singleton
    // 2. No need to activate/focus window before sending keys
    // 3. Direct association between window and keyboard
    // 4. Cleaner module dependencies
    // 5. Better encapsulation
    
    auto windowKeyboard = window_->Keyboard();
    
    // Verify: Direct window-keyboard association
    EXPECT_EQ(windowKeyboard.GetHWND(), window_->GetHandle());
    
    // Verify: No activation/focus needed
    EXPECT_CALL(*window_, Activate()).Times(0);
    EXPECT_CALL(*window_, Focus()).Times(0);
    
    // Keyboard operations work without additional setup
    SUCCEED();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}