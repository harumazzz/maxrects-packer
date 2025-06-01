#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <functional>

class simple_test_runner {
public:
    struct test_case {
        std::string name;
        std::function<void()> test_func;
    };

    static auto add_test(const std::string& name, std::function<void()> test_func) -> void {
        get_tests().emplace_back(test_case{name, std::move(test_func)});
    }

    static auto run_all_tests() -> bool {
        auto& tests{get_tests()};
        auto passed{0};
        auto failed{0};

        std::cout << "Running " << tests.size() << " tests...\n\n";

        for (const auto& test : tests) {
            try {
                std::cout << "Running: " << test.name << "... ";
                test.test_func();
                std::cout << "PASSED\n";
                ++passed;
            } catch (const std::exception& e) {
                std::cout << "FAILED: " << e.what() << "\n";
                ++failed;
            }
        }

        std::cout << "\nResults: " << passed << " passed, " << failed << " failed\n";
        return failed == 0;
    }

private:
    static auto get_tests() -> std::vector<test_case>& {
        static std::vector<test_case> tests{};
        return tests;
    }
};

class assertion_error : public std::runtime_error {
public:
    explicit assertion_error(const std::string& msg) : std::runtime_error{msg} {}
};

#define ASSERT_EQ(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            throw assertion_error{"Expected " #expected " == " #actual}; \
        } \
    } while(0)

#define EXPECT_EQ(expected, actual) ASSERT_EQ(expected, actual)

#define ASSERT_FLOAT_EQ(expected, actual) \
    do { \
        const auto diff{std::abs((expected) - (actual))}; \
        if (diff > 1e-6f) { \
            throw assertion_error{"Expected " #expected " == " #actual " (float)"}; \
        } \
    } while(0)

#define EXPECT_FLOAT_EQ(expected, actual) ASSERT_FLOAT_EQ(expected, actual)

#define ASSERT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            throw assertion_error{"Expected " #condition " to be true"}; \
        } \
    } while(0)

#define EXPECT_TRUE(condition) ASSERT_TRUE(condition)

#define ASSERT_FALSE(condition) \
    do { \
        if ((condition)) { \
            throw assertion_error{"Expected " #condition " to be false"}; \
        } \
    } while(0)

#define EXPECT_FALSE(condition) ASSERT_FALSE(condition)

#define ASSERT_NE(a, b) \
    do { \
        if ((a) == (b)) { \
            throw assertion_error{"Expected " #a " != " #b}; \
        } \
    } while(0)

#define EXPECT_NE(a, b) ASSERT_NE(a, b)

#define ASSERT_GE(a, b) \
    do { \
        if ((a) < (b)) { \
            throw assertion_error{"Expected " #a " >= " #b}; \
        } \
    } while(0)

#define EXPECT_GE(a, b) ASSERT_GE(a, b)

#define ASSERT_GT(a, b) \
    do { \
        if ((a) <= (b)) { \
            throw assertion_error{"Expected " #a " > " #b}; \
        } \
    } while(0)

#define EXPECT_GT(a, b) ASSERT_GT(a, b)

#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)

#define TEST_IMPL(test_name, unique_id) \
    static void CONCAT(test_func_, unique_id)(); \
    namespace { \
        static auto CONCAT(register_test_, unique_id) = []() { \
            simple_test_runner::add_test(test_name, CONCAT(test_func_, unique_id)); \
            return true; \
        }(); \
    } \
    static void CONCAT(test_func_, unique_id)()

#define TEST(test_name) TEST_IMPL(test_name, __COUNTER__)
