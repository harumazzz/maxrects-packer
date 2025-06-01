#include "oversized_element_bin.h"

namespace MaxRects {

	template<typename RectType, typename Numeric>
	OversizedElementBin<RectType, Numeric>::OversizedElementBin(const RectType& rect) {
		this->width = rect.w;
		this->height = rect.h;
		this->max_width = rect.w;
		this->max_height = rect.h;
		
		auto oversized_rect = rect;
		if constexpr (std::is_same_v<RectType, Rectangle<Numeric>>) {
			oversized_rect.oversized = true;
		}
		this->rects.push_back(oversized_rect);
	}

	template<typename RectType, typename Numeric>
	OversizedElementBin<RectType, Numeric>::OversizedElementBin(Numeric width, Numeric height, std::any data) {
		this->width = width;
		this->height = height;
		this->max_width = width;
		this->max_height = height;
		
		auto oversized_rect = RectType{width, height};
		if constexpr (std::is_same_v<RectType, Rectangle<Numeric>>) {
			oversized_rect.oversized = true;
			oversized_rect.set_data(std::move(data));
		}
		this->rects.push_back(oversized_rect);
	}

	template<typename RectType, typename Numeric>
	auto OversizedElementBin<RectType, Numeric>::add(const RectType& rect) -> RectType* {
		
		(void)rect;     
		return nullptr;
	}

	template<typename RectType, typename Numeric>
	auto OversizedElementBin<RectType, Numeric>::add(Numeric width, Numeric height, std::any data) -> RectType* {
		
		(void)width;    
		(void)height;   
		(void)data;     
		return nullptr;
	}

	template<typename RectType, typename Numeric>
	auto OversizedElementBin<RectType, Numeric>::repack() -> std::vector<RectType> {
		return std::vector<RectType>{};
	}

	template<typename RectType, typename Numeric>
	auto OversizedElementBin<RectType, Numeric>::clone() const -> std::unique_ptr<AbstractBin<RectType, Numeric>> {
		if (!this->rects.empty()) {
			return std::make_unique<OversizedElementBin<RectType, Numeric>>(this->rects[0]);
		}
		return std::make_unique<OversizedElementBin<RectType, Numeric>>(this->width, this->height);
	}

	template<typename RectType, typename Numeric>
	auto OversizedElementBin<RectType, Numeric>::reset() -> void {
		
		
		this->set_dirty(false);
	}

	template<typename RectType, typename Numeric>
	auto OversizedElementBin<RectType, Numeric>::calculate_max_dimensions() -> void {
		this->max_width = this->width;
		this->max_height = this->height;
	}


	template class OversizedElementBin<Rectangle<float>, float>;

	template class OversizedElementBin<Rectangle<double>, double>;

	template class OversizedElementBin<Rectangle<int>, int>;

}
