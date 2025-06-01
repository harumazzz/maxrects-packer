#include "abstract_bin.h"
#include <algorithm>

namespace MaxRects {

	template<typename RectType, typename Numeric>
	AbstractBin<RectType, Numeric>::AbstractBin(Numeric max_w, Numeric max_h, const PackingOptions<Numeric>& opts)
		: max_width{max_w}, max_height{max_h}, width{max_w}, height{max_h}, options{opts}, dirty_counter{0} {
	}
	template<typename RectType, typename Numeric>
	auto AbstractBin<RectType, Numeric>::is_dirty() const noexcept -> bool {
		if (dirty_counter > std::size_t{0}) {
			return true;
		}
		return std::any_of(rects.begin(), rects.end(), 
			[](const auto& rect) { return rect.is_dirty(); 
		});
	}
	template<typename RectType, typename Numeric>
	auto AbstractBin<RectType, Numeric>::set_dirty(
		bool is_dirty
	) noexcept -> void {
		dirty_counter = is_dirty ? dirty_counter + std::size_t{1} : std::size_t{0};
		
		if (!is_dirty) {
			if constexpr (std::is_same_v<RectType, Rectangle<Numeric>>) {
				for (auto& rect : rects) {
					rect.set_dirty(false);
				}
			}
		}
	}
	template<typename RectType, typename Numeric>
	auto AbstractBin<RectType, Numeric>::reset() -> void {
		if (rects.capacity() > 100) {
			std::vector<RectType>{}.swap(rects);
		} else {
			rects.clear();
		}
		
		width = max_width;
		height = max_height;
		set_dirty(false);
	}

	template<typename RectType, typename Numeric>
	auto AbstractBin<RectType, Numeric>::update_size() -> void {
		this->calculate_max_dimensions();
	}


	template class AbstractBin<Rectangle<float>, float>;

	template class AbstractBin<Rectangle<double>, double>;

	template class AbstractBin<Rectangle<int>, int>;

}
