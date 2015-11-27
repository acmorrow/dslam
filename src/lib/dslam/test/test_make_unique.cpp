#include <dslam/make_unique.hpp>
#include <gtest/gtest.h>

namespace {

TEST(MakeUniqueTest, MakePrimitive) {
    auto const x = dslam::make_unique<int>();
    EXPECT_EQ(0, *x);
}

TEST(MakeUniqueTest, MakePrimitiveWithArg) {
    int const value = 4;
    auto const x = dslam::make_unique<int>(value);
    EXPECT_EQ(value, *x);
}

TEST(MakeUniqueTest, ArgumentForwarding) {
    const int kNoArgCtor = 0;
    const int kOneArgCtor = 1;
    const int kThreeArgCtor = 2;

    struct VariousCtors {
        VariousCtors() : value_(0), ctor_id_(kNoArgCtor) {}

        VariousCtors(int value) : value_(value), ctor_id_(kOneArgCtor) {}

        VariousCtors(int value, float const&, double&&) : value_(value), ctor_id_(kThreeArgCtor) {}

        const int value_;
        const int ctor_id_;
    };

    {
        auto const x = dslam::make_unique<VariousCtors>();
        EXPECT_EQ(0, x->value_);
        EXPECT_EQ(kNoArgCtor, x->ctor_id_);
    }

    {
        int const value = 4;
        auto const x = dslam::make_unique<VariousCtors>(value);
        EXPECT_EQ(value, x->value_);
        EXPECT_EQ(kOneArgCtor, x->ctor_id_);
    }

    {
        int const int_value = 4;
        float const float_value = 1.1;
        double double_value = 2.2;
        auto const x =
            dslam::make_unique<VariousCtors>(int_value, float_value, std::move(double_value));
        EXPECT_EQ(int_value, x->value_);
        EXPECT_EQ(kThreeArgCtor, x->ctor_id_);
    }
}
}  // namespace

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
