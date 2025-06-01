#include "simple_test.h"
#include "../src/oversized_element_bin.h"

using namespace MaxRects;

class oversized_element_bin_test {
public:
    auto setup() -> void {
        test_rect = Rectangle<float>{2000.0f, 1500.0f, 0.0f, 0.0f};
        bin = std::make_unique<OversizedElementBin<Rectangle<float>, float>>(test_rect);
    }

    Rectangle<float> test_rect{};
    std::unique_ptr<OversizedElementBin<Rectangle<float>, float>> bin{};
};

TEST("OversizedElementBin constructor with Rectangle") {
    oversized_element_bin_test test{};
    test.setup();
    
    ASSERT_FLOAT_EQ(test.bin->width, 2000.0f);
    ASSERT_FLOAT_EQ(test.bin->height, 1500.0f);
    ASSERT_EQ(test.bin->rects.size(), 1);
    ASSERT_TRUE(test.bin->rects[0].oversized);
}

TEST("OversizedElementBin constructor with dimensions") {
    auto bin_with_dims{std::make_unique<OversizedElementBin<Rectangle<float>, float>>(
        3000.0f, 2000.0f, std::any{42})};
    
    ASSERT_FLOAT_EQ(bin_with_dims->width, 3000.0f);
    ASSERT_FLOAT_EQ(bin_with_dims->height, 2000.0f);
    ASSERT_EQ(bin_with_dims->rects.size(), 1);
    ASSERT_TRUE(bin_with_dims->rects[0].oversized);
    ASSERT_EQ(std::any_cast<int>(bin_with_dims->rects[0].data), 42);
}

TEST("OversizedElementBin rejects additional rectangles") {
    oversized_element_bin_test test{};
    test.setup();
    
    Rectangle<float> additional_rect{100.0f, 100.0f};
    auto* result{test.bin->add(additional_rect)};
    ASSERT_EQ(result, nullptr);
    ASSERT_EQ(test.bin->rects.size(), 1);
}

TEST("OversizedElementBin rejects additional rectangles by dimensions") {
    oversized_element_bin_test test{};
    test.setup();
    
    auto* result{test.bin->add(100.0f, 100.0f, std::any{})};
    ASSERT_EQ(result, nullptr);
    ASSERT_EQ(test.bin->rects.size(), 1);
}

TEST("OversizedElementBin reset does nothing") {
    oversized_element_bin_test test{};
    test.setup();
    
    const auto initial_size{test.bin->rects.size()};
    const auto initial_width{test.bin->width};
    const auto initial_height{test.bin->height};
      test.bin->reset();
    
    ASSERT_EQ(test.bin->rects.size(), initial_size);
    ASSERT_FLOAT_EQ(test.bin->width, initial_width);
    ASSERT_FLOAT_EQ(test.bin->height, initial_height);
}

TEST("OversizedElementBin repack returns empty vector") {
    oversized_element_bin_test test{};
    test.setup();
    
    auto unpacked{test.bin->repack()};
    ASSERT_TRUE(unpacked.empty());
}

TEST("OversizedElementBin clone functionality") {
    oversized_element_bin_test test{};
    test.setup();
    
    auto cloned{test.bin->clone()};
    
    ASSERT_FLOAT_EQ(cloned->width, test.bin->width);
    ASSERT_FLOAT_EQ(cloned->height, test.bin->height);
    ASSERT_EQ(cloned->rects.size(), test.bin->rects.size());
      if (!cloned->rects.empty() && !test.bin->rects.empty()) {
        ASSERT_TRUE(cloned->rects[0].oversized);
        ASSERT_FLOAT_EQ(cloned->rects[0].w, test.bin->rects[0].w);
        ASSERT_FLOAT_EQ(cloned->rects[0].h, test.bin->rects[0].h);
    }
}

TEST("OversizedElementBin max dimensions match content") {
    oversized_element_bin_test test{};
    test.setup();
    
    ASSERT_FLOAT_EQ(test.bin->max_width, test.bin->width);
    ASSERT_FLOAT_EQ(test.bin->max_height, test.bin->height);
    ASSERT_FLOAT_EQ(test.bin->max_width, test.test_rect.w);
    ASSERT_FLOAT_EQ(test.bin->max_height, test.test_rect.h);
}
