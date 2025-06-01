#include "src/maxrects_packer.h"
#include <iostream>
#include <vector>

auto main() -> int {
    using namespace MaxRects;
    
    auto opts = PackingOptions{
        .smart = true,
        .pot = true,
        .square = false,
        .allow_rotation = false,
        .tag = false,
        .exclusive_tag = true,
        .border = 2.0f,
        .logic = PackingLogic::MaxEdge,
    };
    
    auto packer{MaxRectsPacker<float, Rectangle<float>>{1024.0f, 1024.0f, 1.0f, opts}};
    
    std::vector<Rectangle<float>> rectangles{
        Rectangle<float>{100.0f, 150.0f},
        Rectangle<float>{200.0f, 200.0f},
        Rectangle<float>{50.0f, 75.0f},
        Rectangle<float>{300.0f, 100.0f},
        Rectangle<float>{80.0f, 120.0f}
    };
    
    for (auto i{static_cast<std::size_t>(0)}; i < rectangles.size(); ++i) {
        rectangles[i].set_data(static_cast<int>(i));
    }
    
    packer.add_array(rectangles.data(), rectangles.size());
    
    std::cout << "Packed " << packer.get_all_rects().size() << " rectangles into " 
              << packer.bins.size() << " bins\n";
    
    for (auto i{static_cast<std::size_t>(0)}; i < packer.bins.size(); ++i) {
        const auto& bin{packer.bins[i]};
        std::cout << "Bin " << i << ": " << bin->width << "x" << bin->height 
                  << " with " << bin->rects.size() << " rectangles\n";
        
        for (const auto& rect : bin->rects) {
            std::cout << "  Rect: " << rect.w << "x" << rect.h 
                      << " at (" << rect.x << "," << rect.y << ")";
            if constexpr (std::is_same_v<decltype(rect), const Rectangle<float>&>) {
                if (rect.data.has_value()) {
                    std::cout << " data: " << std::any_cast<int>(rect.data);
                }
            }
            std::cout << "\n";
        }
    }
    
    return 0;
}
