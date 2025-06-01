#include "simple_test.h"
#include "../src/maxrects_bin.h"

using namespace MaxRects;

class maxrects_bin_test {
public:
    auto setup() -> void {
        auto opts = PackingOptions{
            .smart = true,
            .pot = true,
            .square = false,
            .allow_rotation = false,
            .tag = true
        };
        bin = std::make_unique<MaxRectsBin<Rectangle<float>, float>>(1024.0f, 1024.0f, 0.0f, opts);
    }

    std::unique_ptr<MaxRectsBin<Rectangle<float>, float>> bin{};
};

TEST("MaxRectsBin is initially empty") {
    maxrects_bin_test test{};
    test.setup();
    
    ASSERT_FLOAT_EQ(test.bin->width, 0.0f);
    ASSERT_FLOAT_EQ(test.bin->height, 0.0f);
}

TEST("MaxRectsBin adds rects correctly") {
    maxrects_bin_test test{};
    test.setup();
    
    auto* position{test.bin->add(200.0f, 100.0f, std::any{})};
    ASSERT_NE(position, nullptr);
    ASSERT_FLOAT_EQ(position->x, 0.0f);
    ASSERT_FLOAT_EQ(position->y, 0.0f);
}

TEST("MaxRectsBin reports and sets bin dirty status") {
    maxrects_bin_test test{};
    test.setup();
    
    test.bin->add(200.0f, 100.0f, std::any{});
    ASSERT_TRUE(test.bin->is_dirty());
    
    test.bin->set_dirty(false);
    ASSERT_FALSE(test.bin->is_dirty());
    
    test.bin->add(200.0f, 100.0f, std::any{});
    ASSERT_TRUE(test.bin->is_dirty());
      test.bin->set_dirty(false);
    test.bin->set_dirty(true);
    ASSERT_TRUE(test.bin->is_dirty());
      test.bin->reset(false);
    ASSERT_FALSE(test.bin->is_dirty());
}

TEST("MaxRectsBin rect modification makes bin dirty") {
    maxrects_bin_test test{};
    test.setup();
    
    auto* rect{test.bin->add(Rectangle<float>{200.0f, 100.0f})};
    ASSERT_NE(rect, nullptr);
    
    test.bin->set_dirty(false);
    rect->set_width(256.0f);
    ASSERT_TRUE(test.bin->is_dirty());
}

TEST("MaxRectsBin packs multiple rectangles") {
    maxrects_bin_test test{};
    test.setup();
    
    test.bin->add(100.0f, 100.0f, std::any{});
    test.bin->add(200.0f, 100.0f, std::any{});
    test.bin->add(100.0f, 200.0f, std::any{});
    
    ASSERT_EQ(test.bin->rects.size(), 3);
    ASSERT_GT(test.bin->width, 0.0f);
    ASSERT_GT(test.bin->height, 0.0f);
}

TEST("MaxRectsBin rejects oversized rectangles") {
    maxrects_bin_test test{};
    test.setup();
    
    auto* result{test.bin->add(2000.0f, 2000.0f, std::any{})};
    ASSERT_EQ(result, nullptr);
}

TEST("MaxRectsBin clone functionality") {
    maxrects_bin_test test{};
    test.setup();
    
    test.bin->add(100.0f, 100.0f, std::any{});
    test.bin->add(200.0f, 200.0f, std::any{});
    
    auto cloned{test.bin->clone()};
    ASSERT_EQ(cloned->rects.size(), test.bin->rects.size());
    ASSERT_FLOAT_EQ(cloned->width, test.bin->width);
    ASSERT_FLOAT_EQ(cloned->height, test.bin->height);
}

TEST("MaxRectsBin repack functionality") {
    maxrects_bin_test test{};
    test.setup();
    
    test.bin->add(100.0f, 100.0f, std::any{});
    test.bin->add(200.0f, 200.0f, std::any{});
    test.bin->add(50.0f, 50.0f, std::any{});
    
    const auto initial_count{test.bin->rects.size()};
    auto unpacked{test.bin->repack()};
    
    ASSERT_EQ(test.bin->rects.size(), initial_count - unpacked.size());
}

TEST("MaxRectsBin reset clears rectangles") {
    maxrects_bin_test test{};
    test.setup();
    
    test.bin->add(100.0f, 100.0f, std::any{});
    test.bin->add(200.0f, 200.0f, std::any{});
    
    ASSERT_GT(test.bin->rects.size(), 0);
    
    test.bin->reset(true);
    
    ASSERT_EQ(test.bin->rects.size(), 0);
    ASSERT_FLOAT_EQ(test.bin->width, 0.0f);
    ASSERT_FLOAT_EQ(test.bin->height, 0.0f);
}

TEST("MaxRectsBin smart sizing updates dimensions") {
    maxrects_bin_test test{};
    test.setup();
    
    test.bin->add(300.0f, 400.0f, std::any{});
    
    ASSERT_GE(test.bin->width, 300.0f);
    ASSERT_GE(test.bin->height, 400.0f);
}

TEST("MaxRectsBin pot sizing uses power of two") {
    maxrects_bin_test test{};
    test.setup();
    
    test.bin->add(300.0f, 400.0f, std::any{});
    
    auto is_power_of_two = [](float value) {
        if (value <= 0.0f) return false;
        return std::floor(std::log2(value)) == std::log2(value);
    };
    
    ASSERT_TRUE(is_power_of_two(test.bin->width));
    ASSERT_TRUE(is_power_of_two(test.bin->height));
}
