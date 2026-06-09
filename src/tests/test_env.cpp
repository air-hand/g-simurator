#include <gtest/gtest.h>

#include <cstdlib>

import std;
import utils;

namespace
{

class ScopedEnv final
{
public:
    ScopedEnv(const char* name, const char* value) : name_(name), original_(read_current(name))
    {
        set(value);
    }

    ~ScopedEnv()
    {
        if (original_)
        {
            set(original_->c_str());
        }
        else
        {
            set("");
        }
    }

    ScopedEnv(const ScopedEnv&) = delete;
    ScopedEnv& operator=(const ScopedEnv&) = delete;

private:
    static std::optional<std::string> read_current(const char* name)
    {
        char* value = nullptr;
        size_t size = 0;
        if (_dupenv_s(&value, &size, name) != 0 || value == nullptr)
        {
            return std::nullopt;
        }

        std::unique_ptr<char, decltype(&std::free)> owned(value, std::free);
        if (size <= 1)
        {
            return std::string{};
        }
        return owned.get();
    }

    void set(const char* value)
    {
        EXPECT_EQ(_putenv_s(name_, value), 0);
    }

    const char* name_;
    std::optional<std::string> original_;
};

TEST(test_env, get_env_returns_value)
{
    ScopedEnv env("SIMURATOR_TEST_ENV_VALUE", "simurator-test-value");

    const auto value = sim::utils::get_env("SIMURATOR_TEST_ENV_VALUE");

    ASSERT_TRUE(value);
    EXPECT_EQ(*value, "simurator-test-value");
}

TEST(test_env, get_env_returns_nullopt_for_missing_value)
{
    ScopedEnv env("SIMURATOR_TEST_ENV_MISSING", "");

    const auto value = sim::utils::get_env("SIMURATOR_TEST_ENV_MISSING");

    EXPECT_FALSE(value);
}

}
