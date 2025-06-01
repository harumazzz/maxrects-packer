#include "simple_test.h"
#include "../src/maxrects_packer.h"
#include <memory>

using namespace MaxRects;

class MaxRectsPacker_test {
public:
    auto setup() -> void {
        PackingOptions opts{
            .smart = true,
            .pot = false,
            .square = false,
            .allow_rotation = false,
            .tag = false,
            .exclusive_tag = true
        };
        packer = std::make_unique<MaxRectsPacker<float, Rectangle<float>>>(1024.0f, 1024.0f, 0.0f, opts);
    }

    std::unique_ptr<MaxRectsPacker<float, Rectangle<float>>> packer{};
};

TEST("MaxRectsPacker adds first element correctly") {
    MaxRectsPacker_test test{};
    test.setup();
    
    auto* rect{test.packer->add(1000.0f, 1000.0f, 1)};
    ASSERT_NE(rect, nullptr);
    ASSERT_EQ(std::any_cast<int>(rect->data), 1);
    ASSERT_EQ(test.packer->bins.size(), 1);
}

TEST("MaxRectsPacker creates additional bin if element doesnt fit") {
    MaxRectsPacker_test test{};
    test.setup();
      
    PackingOptions<float> opts{
        .smart = false,      
        .pot = false,        
        .square = false,     
        .allow_rotation = false,  
        .tag = false,
        .exclusive_tag = true
    };
    
    test.packer = std::make_unique<MaxRectsPacker<float, Rectangle<float>>>(
        600.0f,  
        600.0f,  
        0.0f, opts);
    
    
    test.packer->add(512.0f, 512.0f, 1);
    test.packer->add(512.0f, 512.0f, 2);
    test.packer->add(512.0f, 512.0f, 3);
    
    ASSERT_GE(test.packer->bins.size(), 2);
}

TEST("MaxRectsPacker adds to existing bins if possible") {
    MaxRectsPacker_test test{};
    test.setup();
    
    test.packer->add(256.0f, 256.0f, 1);
    test.packer->add(256.0f, 256.0f, 2);
    
    ASSERT_EQ(test.packer->bins.size(), 1);
}

TEST("MaxRectsPacker adds to new bins after next is called") {
    MaxRectsPacker_test test{};
    test.setup();
    
    test.packer->add(256.0f, 256.0f, 1);
    test.packer->next();
    test.packer->add(256.0f, 256.0f, 2);
    
    ASSERT_GE(test.packer->bins.size(), 2);
}

TEST("MaxRectsPacker handles oversized elements") {
    MaxRectsPacker_test test{};
    test.setup();
    
    auto* rect{test.packer->add(2048.0f, 2048.0f, 1)};
    ASSERT_NE(rect, nullptr);
    ASSERT_EQ(test.packer->bins.size(), 1);
}

TEST("MaxRectsPacker array addition") {
    MaxRectsPacker_test test{};
    test.setup();
    
    std::vector<Rectangle<float>> rectangles{
        Rectangle<float>{100.0f, 100.0f},
        Rectangle<float>{200.0f, 100.0f},
        Rectangle<float>{150.0f, 150.0f}
    };
    
    for (auto i{static_cast<std::size_t>(0)}; i < rectangles.size(); ++i) {
        rectangles[i].set_data(static_cast<int>(i));
    }
    
    test.packer->add_array(rectangles.data(), rectangles.size());
    
    auto all_rects = test.packer->get_all_rects();
    ASSERT_EQ(all_rects.size(), 3);
}

TEST("MaxRectsPacker reset functionality") {
    MaxRectsPacker_test test{};
    test.setup();
    
    test.packer->add(100.0f, 100.0f, 1);
    test.packer->add(200.0f, 200.0f, 2);
    
    ASSERT_GT(test.packer->bins.size(), 0);
    ASSERT_EQ(test.packer->get_current_bin_index(), 0);
    
    test.packer->reset();
    
    ASSERT_EQ(test.packer->bins.size(), 0);
    ASSERT_EQ(test.packer->get_current_bin_index(), 0);
}

TEST("MaxRectsPacker dirty status detection") {
    MaxRectsPacker_test test{};
    test.setup();
    
    ASSERT_FALSE(test.packer->is_dirty());
    
    test.packer->add(100.0f, 100.0f, 1);
    ASSERT_TRUE(test.packer->is_dirty());
}

TEST("MaxRectsPacker repack functionality") {
    MaxRectsPacker_test test{};
    test.setup();
      test.packer->add(100.0f, 100.0f, 1);
    test.packer->add(200.0f, 200.0f, 2);
    
    auto initial_bin_count = test.packer->bins.size();
    test.packer->repack();
    
    
    ASSERT_GE(test.packer->bins.size(), initial_bin_count);
    
    auto all_rects = test.packer->get_all_rects();
    ASSERT_EQ(all_rects.size(), 2);
}

TEST("MaxRectsPacker get all rects") {
    MaxRectsPacker_test test{};
    test.setup();
    
    test.packer->add(100.0f, 100.0f, 1);
    test.packer->add(200.0f, 200.0f, 2);
    
    auto all_rects = test.packer->get_all_rects();
    ASSERT_EQ(all_rects.size(), 2);
}
