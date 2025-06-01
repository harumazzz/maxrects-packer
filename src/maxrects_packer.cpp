#include "maxrects_packer.h"
#include <algorithm>   
#include <iterator>    

namespace MaxRects {

	template<typename Numeric, typename RectType>
	MaxRectsPacker<Numeric, RectType>::MaxRectsPacker(Numeric w, Numeric h,
														Numeric pad, const PackingOptions<Numeric>& opts)
		: width{w}, height{h}, padding{pad}, options{opts}, current_bin_index{std::size_t{0}} {
	}
	template<typename Numeric, typename RectType>
	auto MaxRectsPacker<Numeric, RectType>::add(Numeric rect_width, Numeric rect_height, std::any data) -> RectType* {
		if constexpr (std::is_same_v<RectType, Rectangle<Numeric>>) {
			auto rect = RectType{rect_width, rect_height, std::move(data)};
			return add(std::move(rect));
		} else {
			auto rect = RectType{rect_width, rect_height};
			return add(std::move(rect));
		}
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
	auto MaxRectsPacker<Numeric, RectType>::add(RectType&& rect) -> RectType* {
		
		if (!can_fit_in_bin(rect)) {
			bins.push_back(std::make_unique<OversizedElementBin<RectType, Numeric>>(std::move(rect)));
			return &bins.back()->rects[std::size_t{0}];
		}
		
		
		for (auto i = std::size_t{current_bin_index}; i < bins.size(); ++i) {
			if (auto* added = bins[i]->add(rect)) {
				return added;
			}
		}

		
		auto bin = std::make_unique<MaxRectsBin<RectType, Numeric>>(width, height, padding, options);
		bins.push_back(std::move(bin));
		
		
		return bins.back()->add(std::move(rect));
	}	template<typename Numeric, typename RectType>
	auto MaxRectsPacker<Numeric, RectType>::add_array(std::span<const RectType> rects) -> void {
		if (rects.empty()) {
			return;
		}
		auto sorted_rects = std::vector<RectType>{};
		sorted_rects.reserve(rects.size());
		std::transform(rects.begin(), rects.end(), std::back_inserter(sorted_rects),
			[](const auto& rect) -> RectType { return rect; 
		});
		sort_rects(sorted_rects);
		if (bins.empty() && !sorted_rects.empty()) {
			bins.reserve(1 + sorted_rects.size() / 16);
		}
		for (auto& rect : sorted_rects) {
			add(std::move(rect));
		}
	}

	template<typename Numeric, typename RectType>
	auto MaxRectsPacker<Numeric, RectType>::add_array(const RectType* rects_ptr, std::size_t count) -> void {
		add_array(std::span<const RectType>{rects_ptr, count});
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
			unpacked.reserve(bins.size() * 16);
			
			for (auto& bin : bins) {
				if (bin->is_dirty()) {
					auto bin_unpacked = bin->repack();
					unpacked.insert(unpacked.end(), 
								std::make_move_iterator(bin_unpacked.begin()), 
								std::make_move_iterator(bin_unpacked.end()));
				}
			}
			if (!unpacked.empty()) {
				add_array(std::span<const RectType>{unpacked.data(), unpacked.size()});
			}
			return;
		}

		if (!is_dirty()) return;
		
		auto all_rects = std::vector<RectType>{};
		get_all_rects_into(all_rects);
		reset();
		add_array(std::span<const RectType>{all_rects.data(), all_rects.size()});
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
		get_all_rects_into(all_rects);
		return all_rects;
	}

	template<typename Numeric, typename RectType>
	auto MaxRectsPacker<Numeric, RectType>::get_all_rects_into(std::vector<RectType>& output) const -> void {
		auto total_size = std::size_t{0};
		for (const auto& bin : bins) {
			total_size += bin->rects.size();
		}
		
		output.clear();
		output.reserve(total_size);
		
		for (const auto& bin : bins) {
			output.insert(output.end(), bin->rects.begin(), bin->rects.end());
		}
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
	
	template<typename Numeric, typename RectType>
	auto MaxRectsPacker<Numeric, RectType>::reserve(std::size_t capacity) -> void {
		bins.reserve(capacity / 16 + 1);
		if (!bins.empty()) {
			bins[0]->rects.reserve(capacity);
		}
	}

	template class MaxRectsPacker<float, Rectangle<float>>;

	template class MaxRectsPacker<double, Rectangle<double>>;

	template class MaxRectsPacker<int, Rectangle<int>>;

}
