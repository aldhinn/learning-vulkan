// TODO : DELETE when real tests are written.

#include "vk_tut/application.h"

#include <gtest/gtest.h>

namespace vk::tut {
    class DummyTestFixture : public ::testing::Test {
    protected:
        inline void SetUp() override {
            // Gets executed before each test.
        }
        inline void TearDown() override {
            // Gets executed after each test.
        }
    };

    TEST_F(DummyTestFixture, dummy_test) {
        EXPECT_TRUE(true);
    }
}