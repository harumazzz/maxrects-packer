#pragma once

#include "maxrects_bin.h"
#include "oversized_element_bin.h"
#include <memory>
#include <vector>
#include <algorithm>
#include <span>

namespace MaxRects {

	template<typename Numeric = float, typename RectType = Rectangle<Numeric>>
	class MaxRectsPacker {
	public:
		std::vector<std::unique_ptr<AbstractBin<RectType, Numeric>>> bins{};
		PackingOptions<Numeric> options{};
		Numeric width{};
		Numeric height{};
		Numeric padding{};

		explicit MaxRectsPacker(Numeric w = Numeric{}, Numeric h = Numeric{},
							Numeric pad = Numeric{}, const PackingOptions<Numeric>& opts = {});

		auto add(Numeric rect_width, Numeric rect_height, std::any data = {}) -> RectType*;

		auto add(const RectType& rect) -> RectType*;    auto add_array(const RectType* rects_ptr, std::size_t count) -> void;

		auto reset() -> void;

		auto repack(bool quick = true) -> void;

		auto next() -> std::size_t;

		[[nodiscard]] auto get_current_bin_index() const noexcept -> std::size_t;

		[[nodiscard]] auto is_dirty() const noexcept -> bool;

		[[nodiscard]] auto get_all_rects() const -> std::vector<RectType>;

	private:
		std::size_t current_bin_index{};

		[[nodiscard]] auto can_fit_in_bin(const RectType& rect) const noexcept -> bool;

		auto sort_rects(std::vector<RectType>& rects) const -> void;
	};

}
