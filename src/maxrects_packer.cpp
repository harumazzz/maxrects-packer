#include "maxrects_packer.h"

namespace MaxRects {

	template<typename Numeric, typename RectType>
	MaxRectsPacker<Numeric, RectType>::MaxRectsPacker(Numeric w, Numeric h,
														Numeric pad, const PackingOptions<Numeric>& opts)
		: width{w}, height{h}, padding{pad}, options{opts}, current_bin_index{std::size_t{0}} {
	}

	template<typename Numeric, typename RectType>
	auto MaxRectsPacker<Numeric, RectType>::add(Numeric rect_width, Numeric rect_height, std::any data) -> RectType* {
		auto rect = RectType{rect_width, rect_height};
		if constexpr (std::is_same_v<RectType, Rectangle<Numeric>>) {
			rect.set_data(std::move(data));
		}
		return add(rect);
	}

	template<typename Numeric, typename RectType>
	auto MaxRectsPacker<Numeric, RectType>::add(const RectType& rect) -> RectType* {
		
		if (!can_fit_in_bin(rect)) {
			bins.push_back(std::make_unique<OversizedElementBin<RectType, Numeric>>(rect));
			return &bins.back()->rects[std::size_t{0}];
		}
		
		
		for (auto i = std::size_t{current_bin_index}; i < bins.size(); ++i) {
			if (auto* added = bins[i]->add(rect)) {
				return added;
			}
		}

		
		auto bin = std::make_unique<MaxRectsBin<RectType, Numeric>>(width, height, padding, options);
		bins.push_back(std::move(bin));
		
		
		return bins.back()->add(rect);
	}

	template<typename Numeric, typename RectType>
	auto MaxRectsPacker<Numeric, RectType>::add_array(const RectType* rects_ptr, std::size_t count) -> void {
		auto rects_span = std::span<const RectType>{rects_ptr, count};
		auto sorted_rects = std::vector<RectType>{rects_span.begin(), rects_span.end()};
		
		if (!options.tag || options.exclusive_tag) {
			sort_rects(sorted_rects);
			for (const auto& rect : sorted_rects) {
				add(rect);
			}
		} else {
			sort_rects(sorted_rects);
			for (const auto& rect : sorted_rects) {
				add(rect);
			}
		}
	}

	template<typename Numeric, typename RectType>
	auto MaxRectsPacker<Numeric, RectType>::reset() -> void {
		bins.clear();
		current_bin_index = std::size_t{0};
	}

	template<typename Numeric, typename RectType>
	auto MaxRectsPacker<Numeric, RectType>::repack(bool quick) -> void {
		if (quick) {
			auto unpacked = std::vector<RectType>{};
			for (auto& bin : bins) {
				if (bin->is_dirty()) {
					auto bin_unpacked = bin->repack();
					unpacked.insert(unpacked.end(), bin_unpacked.begin(), bin_unpacked.end());
				}
			}
			if (!unpacked.empty()) {
				add_array(unpacked.data(), unpacked.size());
			}
			return;
		}

		if (!is_dirty()) return;
		
		auto all_rects = get_all_rects();
		reset();
		add_array(all_rects.data(), all_rects.size());
	}

	template<typename Numeric, typename RectType>
	auto MaxRectsPacker<Numeric, RectType>::next() -> std::size_t {
		current_bin_index = bins.size();
		return current_bin_index;
	}

	template<typename Numeric, typename RectType>
	auto MaxRectsPacker<Numeric, RectType>::get_current_bin_index() const noexcept -> std::size_t {
		return current_bin_index;
	}

	template<typename Numeric, typename RectType>
	auto MaxRectsPacker<Numeric, RectType>::is_dirty() const noexcept -> bool {
		return std::any_of(bins.begin(), bins.end(),
						[](const auto& bin) { return bin->is_dirty(); });
	}

	template<typename Numeric, typename RectType>
	auto MaxRectsPacker<Numeric, RectType>::get_all_rects() const -> std::vector<RectType> {
		auto all_rects = std::vector<RectType>{};
		for (const auto& bin : bins) {
			all_rects.insert(all_rects.end(), bin->rects.begin(), bin->rects.end());
		}
		return all_rects;
	}

	template<typename Numeric, typename RectType>
	auto MaxRectsPacker<Numeric, RectType>::can_fit_in_bin(const RectType& rect) const noexcept -> bool {
		return (rect.w <= width && rect.h <= height) ||
			(options.allow_rotation && rect.w <= height && rect.h <= width);
	}

	template<typename Numeric, typename RectType>
	auto MaxRectsPacker<Numeric, RectType>::sort_rects(std::vector<RectType>& rects) const -> void {
		std::sort(rects.begin(), rects.end(), [this](const auto& a, const auto& b) {
			if (options.logic == PackingLogic::MaxEdge) {
				return std::max(a.w, a.h) > std::max(b.w, b.h);
			} else {
				return a.area() > b.area();
			}
		});
	}


	template class MaxRectsPacker<float, Rectangle<float>>;

	template class MaxRectsPacker<double, Rectangle<double>>;

	template class MaxRectsPacker<int, Rectangle<int>>;

}
