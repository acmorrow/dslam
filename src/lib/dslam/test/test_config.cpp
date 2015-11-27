#include <dslam/config.hpp>
#include <gtest/gtest.h>

namespace {

// Check that calling get_library_version with no arguments
// works OK.
TEST(ConfigTest, GetVersionNullArgs) {
    dslam::get_library_version();
}

// Check that get_library_version gives back the major version OK.
TEST(ConfigTest, TestGetMajorVersion) {
    size_t version_major = dslam::library_version_major + 1;
    dslam::get_library_version(&version_major);
    EXPECT_EQ(dslam::library_version_major, version_major);
}

// Check that get_library_version gives back the minor version OK.
TEST(ConfigTest, TestGetMinorVersion) {
    size_t version_minor = dslam::library_version_minor + 1;
    dslam::get_library_version(nullptr, &version_minor);
    EXPECT_EQ(dslam::library_version_minor, version_minor);
}

// Check that get_library_version gives back the patch version OK.
TEST(ConfigTest, TestGetPatchVersion) {
    size_t version_patch = dslam::library_version_patch + 1;
    dslam::get_library_version(nullptr, nullptr, &version_patch);
    EXPECT_EQ(dslam::library_version_patch, version_patch);
}

}  // namespace

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
