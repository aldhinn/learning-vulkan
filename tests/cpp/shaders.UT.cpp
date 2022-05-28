#include "vk_tut/shaders.h"
#include "vk_tut/logging.h"

#include <gtest/gtest.h>
#include <string>
#include <iostream>
#include <time.h>
#include <limits>
#include <fstream>

namespace vk::tut {
    // Wrap the cout object.
    using ::std::cout;

    // The shaders test fixture.
    class ShadersTest : public ::testing::Test {
    protected:
        // To be run before each test.
        inline void SetUp() override {
            if (m_should_seed) {
                srand(time(NULL));
                m_should_seed = false; // Should seed only once.
            }
        }
        // To be run after each test.
        inline void TearDown() override {
            // Delete the test file if it exists.
            ::std::filesystem::remove_all(m_test_file);
        }

        const ::std::string m_test_file = _VK_TUT_PROJECT_DIR_ "/.test";
        // Determines whether to run the random number seeding.
        bool m_should_seed = true;

        // Generates a random data.
        inline file_data_t generate_random_data() {
            // Generate a random size up to 1KB
            size_t size = static_cast<size_t>(rand() % 1024);
            file_data_t buffer(size);

            // Assign random bytes.
            for (byte_t& byte : buffer) {
                // Assign a valid 8-bit value.
                byte = static_cast<byte_t>(rand() %
                    ::std::numeric_limits<byte_t>::max());
            }

            return buffer;
        }
    };

    TEST_F(ShadersTest, read_gitattributes) {
        file_data_t gitattributes_data = get_file_data(
            _VK_TUT_PROJECT_DIR_ "/.gitattributes"
        );

        VK_TUT_LOG_DEBUG("Reading out contents of .gitattributes");
        cout << "\033[0;93m " << gitattributes_data.data() << " \033[0m\n";
        VK_TUT_LOG_DEBUG("FINISHED Reading out contents of .gitattributes");
    }

    TEST_F(ShadersTest, store_and_retrieve) {
        file_data_t expected_data = generate_random_data();
        VK_TUT_LOG_DEBUG("Generated data of " +
            ::std::to_string(expected_data.size()) + " bytes.");

        // Store data.
        ::std::ofstream data_writer(m_test_file, ::std::ios::binary);
        if (!data_writer.is_open()) {
            VK_TUT_LOG_ERROR("Test file not open.");
        }
        data_writer.write(expected_data.data(), expected_data.size());
        data_writer.close();

        // Retrieve data.
        file_data_t retrieved_data = get_file_data(m_test_file);
        VK_TUT_LOG_DEBUG("Retrieved data has " +
            ::std::to_string(retrieved_data.size()) + " bytes.");

        EXPECT_EQ(expected_data.size(), retrieved_data.size());

        // Loop through the data and see if there are inequalities.
        for (int i = 0; i < expected_data.size(); i++) {
            if (expected_data[i] != retrieved_data[i]) {
                VK_TUT_LOG_ERROR("Data not stored properly.");
            }
        }
    }
}