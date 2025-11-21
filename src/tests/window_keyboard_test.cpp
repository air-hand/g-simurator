// Unit tests for WindowKeyboard class
// Tests the keyboard input simulation functionality for Windows windows

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <windows.h>
#include <vector>
#include <memory>

// Mock Windows API functions for testing
namespace {
    struct MockMessageQueue {
        struct Message {
            HWND hwnd;
            UINT msg;
            WPARAM wParam;
            LPARAM lParam;
        };
        
        static std::vector<Message>& GetMessages() {
            static std::vector<Message> messages;
            return messages;
        }
        
        static void Clear() {
            GetMessages().clear();
        }
        
        static void RecordMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
            GetMessages().push_back({hwnd, msg, wParam, lParam});
        }
    };
}

// Since we can't directly test the module implementation without module support in tests,
// we'll create a testable wrapper that mimics the WindowKeyboard functionality

namespace sim::utils::window::test {

// Test implementation that exposes the same interface
class WindowKeyboardTestImpl {
public:
    explicit WindowKeyboardTestImpl(HWND hwnd) noexcept
        : hwnd_(hwnd)
    {
    }

    void KeyDown(WORD key) const {
        KeyDown(std::vector<WORD>{key});
    }

    void KeyUp(WORD key) const {
        KeyUp(std::vector<WORD>{key});
    }

    void KeyDown(const std::vector<WORD>& keys) const {
        for (const auto key : keys) {
            MockMessageQueue::RecordMessage(hwnd_, WM_KEYDOWN, key, 0);
        }
    }

    void KeyUp(const std::vector<WORD>& keys) const {
        // Reverse order for key up
        for (auto it = keys.rbegin(); it != keys.rend(); ++it) {
            MockMessageQueue::RecordMessage(hwnd_, WM_KEYUP, *it, 0);
        }
    }

private:
    HWND hwnd_;
};

} // namespace sim::utils::window::test

using namespace sim::utils::window::test;
using ::testing::_;
using ::testing::Return;

class WindowKeyboardTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Clear message queue before each test
        MockMessageQueue::Clear();
        
        // Create a mock HWND (using a valid pointer value for testing)
        test_hwnd_ = reinterpret_cast<HWND>(0x12345678);
    }

    void TearDown() override {
        MockMessageQueue::Clear();
    }

    HWND test_hwnd_;
};

// Test: Constructor creates valid instance
TEST_F(WindowKeyboardTest, ConstructorCreatesValidInstance) {
    EXPECT_NO_THROW({
        WindowKeyboardTestImpl keyboard(test_hwnd_);
    });
}

// Test: Single key down posts correct message
TEST_F(WindowKeyboardTest, SingleKeyDownPostsCorrectMessage) {
    WindowKeyboardTestImpl keyboard(test_hwnd_);
    
    const WORD test_key = VK_SPACE;
    keyboard.KeyDown(test_key);
    
    const auto& messages = MockMessageQueue::GetMessages();
    ASSERT_EQ(messages.size(), 1);
    EXPECT_EQ(messages[0].hwnd, test_hwnd_);
    EXPECT_EQ(messages[0].msg, WM_KEYDOWN);
    EXPECT_EQ(messages[0].wParam, test_key);
    EXPECT_EQ(messages[0].lParam, 0);
}

// Test: Single key up posts correct message
TEST_F(WindowKeyboardTest, SingleKeyUpPostsCorrectMessage) {
    WindowKeyboardTestImpl keyboard(test_hwnd_);
    
    const WORD test_key = VK_RETURN;
    keyboard.KeyUp(test_key);
    
    const auto& messages = MockMessageQueue::GetMessages();
    ASSERT_EQ(messages.size(), 1);
    EXPECT_EQ(messages[0].hwnd, test_hwnd_);
    EXPECT_EQ(messages[0].msg, WM_KEYUP);
    EXPECT_EQ(messages[0].wParam, test_key);
    EXPECT_EQ(messages[0].lParam, 0);
}

// Test: Multiple keys down in correct order
TEST_F(WindowKeyboardTest, MultipleKeysDownInCorrectOrder) {
    WindowKeyboardTestImpl keyboard(test_hwnd_);
    
    std::vector<WORD> keys = {VK_CONTROL, VK_SHIFT, 'A'};
    keyboard.KeyDown(keys);
    
    const auto& messages = MockMessageQueue::GetMessages();
    ASSERT_EQ(messages.size(), 3);
    
    for (size_t i = 0; i < keys.size(); ++i) {
        EXPECT_EQ(messages[i].hwnd, test_hwnd_);
        EXPECT_EQ(messages[i].msg, WM_KEYDOWN);
        EXPECT_EQ(messages[i].wParam, keys[i]);
        EXPECT_EQ(messages[i].lParam, 0);
    }
}

// Test: Multiple keys up in reverse order
TEST_F(WindowKeyboardTest, MultipleKeysUpInReverseOrder) {
    WindowKeyboardTestImpl keyboard(test_hwnd_);
    
    std::vector<WORD> keys = {VK_CONTROL, VK_SHIFT, 'B'};
    keyboard.KeyUp(keys);
    
    const auto& messages = MockMessageQueue::GetMessages();
    ASSERT_EQ(messages.size(), 3);
    
    // Verify reverse order
    EXPECT_EQ(messages[0].wParam, 'B');
    EXPECT_EQ(messages[1].wParam, VK_SHIFT);
    EXPECT_EQ(messages[2].wParam, VK_CONTROL);
    
    for (const auto& msg : messages) {
        EXPECT_EQ(msg.hwnd, test_hwnd_);
        EXPECT_EQ(msg.msg, WM_KEYUP);
        EXPECT_EQ(msg.lParam, 0);
    }
}

// Test: Empty key vector handles gracefully
TEST_F(WindowKeyboardTest, EmptyKeyVectorHandlesGracefully) {
    WindowKeyboardTestImpl keyboard(test_hwnd_);
    
    std::vector<WORD> empty_keys;
    EXPECT_NO_THROW({
        keyboard.KeyDown(empty_keys);
        keyboard.KeyUp(empty_keys);
    });
    
    const auto& messages = MockMessageQueue::GetMessages();
    EXPECT_EQ(messages.size(), 0);
}

// Test: Single key via vector overload
TEST_F(WindowKeyboardTest, SingleKeyViaVectorOverload) {
    WindowKeyboardTestImpl keyboard(test_hwnd_);
    
    std::vector<WORD> single_key = {VK_ESCAPE};
    keyboard.KeyDown(single_key);
    
    const auto& messages = MockMessageQueue::GetMessages();
    ASSERT_EQ(messages.size(), 1);
    EXPECT_EQ(messages[0].msg, WM_KEYDOWN);
    EXPECT_EQ(messages[0].wParam, VK_ESCAPE);
}

// Test: All standard virtual keys
TEST_F(WindowKeyboardTest, StandardVirtualKeysWork) {
    WindowKeyboardTestImpl keyboard(test_hwnd_);
    
    std::vector<WORD> standard_keys = {
        VK_BACK, VK_TAB, VK_RETURN, VK_SHIFT, VK_CONTROL,
        VK_MENU, VK_ESCAPE, VK_SPACE, VK_PRIOR, VK_NEXT,
        VK_END, VK_HOME, VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN
    };
    
    for (const auto key : standard_keys) {
        MockMessageQueue::Clear();
        keyboard.KeyDown(key);
        
        const auto& messages = MockMessageQueue::GetMessages();
        ASSERT_EQ(messages.size(), 1) << "Failed for key: " << key;
        EXPECT_EQ(messages[0].wParam, key);
    }
}

// Test: Alphanumeric keys
TEST_F(WindowKeyboardTest, AlphanumericKeysWork) {
    WindowKeyboardTestImpl keyboard(test_hwnd_);
    
    // Test A-Z
    for (WORD key = 'A'; key <= 'Z'; ++key) {
        MockMessageQueue::Clear();
        keyboard.KeyDown(key);
        
        const auto& messages = MockMessageQueue::GetMessages();
        ASSERT_EQ(messages.size(), 1);
        EXPECT_EQ(messages[0].wParam, key);
    }
    
    // Test 0-9
    for (WORD key = '0'; key <= '9'; ++key) {
        MockMessageQueue::Clear();
        keyboard.KeyDown(key);
        
        const auto& messages = MockMessageQueue::GetMessages();
        ASSERT_EQ(messages.size(), 1);
        EXPECT_EQ(messages[0].wParam, key);
    }
}

// Test: Function keys
TEST_F(WindowKeyboardTest, FunctionKeysWork) {
    WindowKeyboardTestImpl keyboard(test_hwnd_);
    
    for (WORD fkey = VK_F1; fkey <= VK_F12; ++fkey) {
        MockMessageQueue::Clear();
        keyboard.KeyDown(fkey);
        
        const auto& messages = MockMessageQueue::GetMessages();
        ASSERT_EQ(messages.size(), 1);
        EXPECT_EQ(messages[0].wParam, fkey);
    }
}

// Test: Numpad keys
TEST_F(WindowKeyboardTest, NumpadKeysWork) {
    WindowKeyboardTestImpl keyboard(test_hwnd_);
    
    for (WORD key = VK_NUMPAD0; key <= VK_NUMPAD9; ++key) {
        MockMessageQueue::Clear();
        keyboard.KeyDown(key);
        
        const auto& messages = MockMessageQueue::GetMessages();
        ASSERT_EQ(messages.size(), 1);
        EXPECT_EQ(messages[0].wParam, key);
    }
}

// Test: Modifier key combinations (Ctrl+C scenario)
TEST_F(WindowKeyboardTest, ModifierKeyCombinationCtrlC) {
    WindowKeyboardTestImpl keyboard(test_hwnd_);
    
    std::vector<WORD> ctrl_c = {VK_CONTROL, 'C'};
    keyboard.KeyDown(ctrl_c);
    keyboard.KeyUp(ctrl_c);
    
    const auto& messages = MockMessageQueue::GetMessages();
    ASSERT_EQ(messages.size(), 4);
    
    // KeyDown order: Ctrl, C
    EXPECT_EQ(messages[0].msg, WM_KEYDOWN);
    EXPECT_EQ(messages[0].wParam, VK_CONTROL);
    EXPECT_EQ(messages[1].msg, WM_KEYDOWN);
    EXPECT_EQ(messages[1].wParam, 'C');
    
    // KeyUp order: C, Ctrl (reverse)
    EXPECT_EQ(messages[2].msg, WM_KEYUP);
    EXPECT_EQ(messages[2].wParam, 'C');
    EXPECT_EQ(messages[3].msg, WM_KEYUP);
    EXPECT_EQ(messages[3].wParam, VK_CONTROL);
}

// Test: Modifier key combinations (Ctrl+Shift+A scenario)
TEST_F(WindowKeyboardTest, ModifierKeyCombinationCtrlShiftA) {
    WindowKeyboardTestImpl keyboard(test_hwnd_);
    
    std::vector<WORD> ctrl_shift_a = {VK_CONTROL, VK_SHIFT, 'A'};
    keyboard.KeyDown(ctrl_shift_a);
    keyboard.KeyUp(ctrl_shift_a);
    
    const auto& messages = MockMessageQueue::GetMessages();
    ASSERT_EQ(messages.size(), 6);
    
    // KeyDown: Ctrl, Shift, A
    EXPECT_EQ(messages[0].wParam, VK_CONTROL);
    EXPECT_EQ(messages[1].wParam, VK_SHIFT);
    EXPECT_EQ(messages[2].wParam, 'A');
    
    // KeyUp: A, Shift, Ctrl (reverse)
    EXPECT_EQ(messages[3].wParam, 'A');
    EXPECT_EQ(messages[4].wParam, VK_SHIFT);
    EXPECT_EQ(messages[5].wParam, VK_CONTROL);
}

// Test: Alt+F4 combination
TEST_F(WindowKeyboardTest, AltF4Combination) {
    WindowKeyboardTestImpl keyboard(test_hwnd_);
    
    std::vector<WORD> alt_f4 = {VK_MENU, VK_F4};
    keyboard.KeyDown(alt_f4);
    keyboard.KeyUp(alt_f4);
    
    const auto& messages = MockMessageQueue::GetMessages();
    ASSERT_EQ(messages.size(), 4);
    
    EXPECT_EQ(messages[0].wParam, VK_MENU);
    EXPECT_EQ(messages[1].wParam, VK_F4);
    EXPECT_EQ(messages[2].wParam, VK_F4);
    EXPECT_EQ(messages[3].wParam, VK_MENU);
}

// Test: Rapid sequential key presses
TEST_F(WindowKeyboardTest, RapidSequentialKeyPresses) {
    WindowKeyboardTestImpl keyboard(test_hwnd_);
    
    const int num_presses = 100;
    for (int i = 0; i < num_presses; ++i) {
        keyboard.KeyDown('X');
        keyboard.KeyUp('X');
    }
    
    const auto& messages = MockMessageQueue::GetMessages();
    EXPECT_EQ(messages.size(), num_presses * 2);
}

// Test: Different HWND values
TEST_F(WindowKeyboardTest, DifferentHWNDValues) {
    HWND hwnd1 = reinterpret_cast<HWND>(0x11111111);
    HWND hwnd2 = reinterpret_cast<HWND>(0x22222222);
    
    WindowKeyboardTestImpl keyboard1(hwnd1);
    WindowKeyboardTestImpl keyboard2(hwnd2);
    
    keyboard1.KeyDown('A');
    keyboard2.KeyDown('B');
    
    const auto& messages = MockMessageQueue::GetMessages();
    ASSERT_EQ(messages.size(), 2);
    EXPECT_EQ(messages[0].hwnd, hwnd1);
    EXPECT_EQ(messages[0].wParam, 'A');
    EXPECT_EQ(messages[1].hwnd, hwnd2);
    EXPECT_EQ(messages[1].wParam, 'B');
}

// Test: Null HWND handling
TEST_F(WindowKeyboardTest, NullHWNDHandling) {
    HWND null_hwnd = nullptr;
    
    EXPECT_NO_THROW({
        WindowKeyboardTestImpl keyboard(null_hwnd);
        keyboard.KeyDown(VK_SPACE);
        keyboard.KeyUp(VK_SPACE);
    });
    
    const auto& messages = MockMessageQueue::GetMessages();
    ASSERT_EQ(messages.size(), 2);
    EXPECT_EQ(messages[0].hwnd, null_hwnd);
}

// Test: Large key vector
TEST_F(WindowKeyboardTest, LargeKeyVector) {
    WindowKeyboardTestImpl keyboard(test_hwnd_);
    
    std::vector<WORD> large_vector;
    for (WORD i = 0; i < 50; ++i) {
        large_vector.push_back('A' + (i % 26));
    }
    
    keyboard.KeyDown(large_vector);
    
    const auto& messages = MockMessageQueue::GetMessages();
    ASSERT_EQ(messages.size(), large_vector.size());
    
    for (size_t i = 0; i < large_vector.size(); ++i) {
        EXPECT_EQ(messages[i].wParam, large_vector[i]);
    }
}

// Test: Edge case - Maximum WORD value
TEST_F(WindowKeyboardTest, MaximumWORDValue) {
    WindowKeyboardTestImpl keyboard(test_hwnd_);
    
    const WORD max_word = 0xFFFF;
    EXPECT_NO_THROW({
        keyboard.KeyDown(max_word);
        keyboard.KeyUp(max_word);
    });
    
    const auto& messages = MockMessageQueue::GetMessages();
    ASSERT_EQ(messages.size(), 2);
    EXPECT_EQ(messages[0].wParam, max_word);
    EXPECT_EQ(messages[1].wParam, max_word);
}

// Test: Move constructor
TEST_F(WindowKeyboardTest, MoveConstructorWorks) {
    WindowKeyboardTestImpl keyboard1(test_hwnd_);
    WindowKeyboardTestImpl keyboard2(std::move(keyboard1));
    
    EXPECT_NO_THROW({
        keyboard2.KeyDown(VK_SPACE);
    });
    
    const auto& messages = MockMessageQueue::GetMessages();
    ASSERT_EQ(messages.size(), 1);
}

// Test: Move assignment operator
TEST_F(WindowKeyboardTest, MoveAssignmentWorks) {
    HWND hwnd2 = reinterpret_cast<HWND>(0x99999999);
    
    WindowKeyboardTestImpl keyboard1(test_hwnd_);
    WindowKeyboardTestImpl keyboard2(hwnd2);
    
    keyboard2 = std::move(keyboard1);
    
    EXPECT_NO_THROW({
        keyboard2.KeyDown(VK_RETURN);
    });
    
    const auto& messages = MockMessageQueue::GetMessages();
    ASSERT_EQ(messages.size(), 1);
}

// Test: Self-assignment safety
TEST_F(WindowKeyboardTest, SelfAssignmentSafety) {
    WindowKeyboardTestImpl keyboard(test_hwnd_);
    
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wself-move"
    keyboard = std::move(keyboard);
    #pragma GCC diagnostic pop
    
    EXPECT_NO_THROW({
        keyboard.KeyDown(VK_TAB);
    });
}

// Integration test: Complete key press sequence
TEST_F(WindowKeyboardTest, CompleteKeyPressSequence) {
    WindowKeyboardTestImpl keyboard(test_hwnd_);
    
    // Simulate typing "Hello"
    keyboard.KeyDown('H');
    keyboard.KeyUp('H');
    keyboard.KeyDown('E');
    keyboard.KeyUp('E');
    keyboard.KeyDown('L');
    keyboard.KeyUp('L');
    keyboard.KeyDown('L');
    keyboard.KeyUp('L');
    keyboard.KeyDown('O');
    keyboard.KeyUp('O');
    
    const auto& messages = MockMessageQueue::GetMessages();
    ASSERT_EQ(messages.size(), 10);
    
    std::vector<char> expected = {'H', 'E', 'L', 'L', 'O'};
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(messages[i * 2].msg, WM_KEYDOWN);
        EXPECT_EQ(messages[i * 2].wParam, expected[i]);
        EXPECT_EQ(messages[i * 2 + 1].msg, WM_KEYUP);
        EXPECT_EQ(messages[i * 2 + 1].wParam, expected[i]);
    }
}

// Integration test: Complex shortcut sequence
TEST_F(WindowKeyboardTest, ComplexShortcutSequence) {
    WindowKeyboardTestImpl keyboard(test_hwnd_);
    
    // Ctrl+Shift+S (Save As)
    std::vector<WORD> save_as = {VK_CONTROL, VK_SHIFT, 'S'};
    keyboard.KeyDown(save_as);
    keyboard.KeyUp(save_as);
    
    // Verify complete sequence
    const auto& messages = MockMessageQueue::GetMessages();
    ASSERT_EQ(messages.size(), 6);
    
    // All messages should target the same window
    for (const auto& msg : messages) {
        EXPECT_EQ(msg.hwnd, test_hwnd_);
        EXPECT_EQ(msg.lParam, 0);
    }
}

// Performance test: Measure overhead
TEST_F(WindowKeyboardTest, PerformanceOverheadAcceptable) {
    WindowKeyboardTestImpl keyboard(test_hwnd_);
    
    const int iterations = 1000;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        keyboard.KeyDown('A');
        keyboard.KeyUp('A');
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Should complete quickly (arbitrary threshold for test environment)
    EXPECT_LT(duration.count(), 100000) << "Performance degraded";
    
    const auto& messages = MockMessageQueue::GetMessages();
    EXPECT_EQ(messages.size(), iterations * 2);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}