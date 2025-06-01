#include "simple_test.h"
#include "../src/rectangle.h"
#include <any>

using namespace MaxRects;

TEST("Rectangle Test default_construction") {
    auto default_rect = Rectangle<float>{};
    EXPECT_FLOAT_EQ(default_rect.w, 0.0f);
    EXPECT_FLOAT_EQ(default_rect.h, 0.0f);
    EXPECT_FLOAT_EQ(default_rect.x, 0.0f);
    EXPECT_FLOAT_EQ(default_rect.y, 0.0f);
    EXPECT_FALSE(default_rect.oversized);
    EXPECT_FALSE(default_rect.rot);
    EXPECT_FALSE(default_rect.allow_rotation);
}

TEST("Rectangle Test parametrized_construction") {
    auto test_rect = Rectangle<float>{50.0f, 75.0f, 5.0f, 15.0f, true, true};
    EXPECT_FLOAT_EQ(test_rect.w, 50.0f);
    EXPECT_FLOAT_EQ(test_rect.h, 75.0f);
    EXPECT_FLOAT_EQ(test_rect.x, 5.0f);
    EXPECT_FLOAT_EQ(test_rect.y, 15.0f);
    EXPECT_TRUE(test_rect.rot);
    EXPECT_TRUE(test_rect.allow_rotation);
}

TEST("Rectangle Test area_calculation") {
    auto rect = Rectangle<float>{100.0f, 200.0f, 10.0f, 20.0f};
    EXPECT_FLOAT_EQ(rect.area(), 20000.0f);
}

TEST("Rectangle Test collision_detection") {
    auto rect1 = Rectangle<float>{50.0f, 50.0f, 0.0f, 0.0f};
    auto rect2 = Rectangle<float>{30.0f, 30.0f, 25.0f, 25.0f};
    auto rect3 = Rectangle<float>{20.0f, 20.0f, 60.0f, 60.0f};
    
    EXPECT_TRUE(rect1.collides_with(rect2));
    EXPECT_FALSE(rect1.collides_with(rect3));
}

TEST("Rectangle Test containment_check") {
    auto container = Rectangle<float>{100.0f, 100.0f, 0.0f, 0.0f};
    auto contained = Rectangle<float>{50.0f, 50.0f, 25.0f, 25.0f};
    auto not_contained = Rectangle<float>{120.0f, 50.0f, 0.0f, 0.0f};
    
    EXPECT_TRUE(container.contains(contained));
    EXPECT_FALSE(container.contains(not_contained));
}

TEST("Rectangle Test static_collision_method") {
    auto rect1 = Rectangle<float>{50.0f, 50.0f, 0.0f, 0.0f};
    auto rect2 = Rectangle<float>{30.0f, 30.0f, 25.0f, 25.0f};
    
    EXPECT_TRUE(Rectangle<float>::collide(rect1, rect2));
}

TEST("Rectangle Test static_contain_method") {
    auto container = Rectangle<float>{100.0f, 100.0f, 0.0f, 0.0f};
    auto contained = Rectangle<float>{50.0f, 50.0f, 25.0f, 25.0f};
    
    EXPECT_TRUE(Rectangle<float>::contain(container, contained));
}

TEST("Rectangle Test width_setter_updates_dirty_counter") {
    auto rect = Rectangle<float>{100.0f, 200.0f};
    auto initial_counter = rect.dirty_counter;
    
    rect.set_width(150.0f);
    EXPECT_GT(rect.dirty_counter, initial_counter);
    EXPECT_FLOAT_EQ(rect.w, 150.0f);
}

TEST("Rectangle Test height_setter_updates_dirty_counter") {
    auto rect = Rectangle<float>{100.0f, 200.0f};
    auto initial_counter = rect.dirty_counter;
    
    rect.set_height(250.0f);
    EXPECT_GT(rect.dirty_counter, initial_counter);
    EXPECT_FLOAT_EQ(rect.h, 250.0f);
}

TEST("Rectangle Test rotation_with_allow_rotation_false") {
    auto rect = Rectangle<float>{100.0f, 200.0f};
    rect.allow_rotation = false;
    auto initial_w = rect.w;
    auto initial_h = rect.h;
    
    rect.set_rotation(true);
    EXPECT_FLOAT_EQ(rect.w, initial_w);
    EXPECT_FLOAT_EQ(rect.h, initial_h);
    EXPECT_FALSE(rect.rot);
}

TEST("Rectangle Test rotation_with_allow_rotation_true") {
    auto rect = Rectangle<float>{100.0f, 200.0f};
    rect.allow_rotation = true;
    
    rect.set_rotation(true);
    EXPECT_FLOAT_EQ(rect.w, 200.0f);
    EXPECT_FLOAT_EQ(rect.h, 100.0f);
    EXPECT_TRUE(rect.rot);
}

TEST("Rectangle Test dirty_status_management") {
    auto rect = Rectangle<float>{100.0f, 200.0f};
    
    EXPECT_FALSE(rect.is_dirty());
    
    rect.set_dirty(true);
    EXPECT_TRUE(rect.is_dirty());
    
    rect.set_dirty(false);
    EXPECT_FALSE(rect.is_dirty());
}

TEST("Rectangle Test data_storage") {
    auto rect = Rectangle<float>{100.0f, 200.0f};
    
    rect.set_data(42);
    EXPECT_TRUE(rect.data.has_value());
    EXPECT_EQ(std::any_cast<int>(rect.data), 42);
}
