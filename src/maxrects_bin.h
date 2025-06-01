#pragma once

#include "abstract_bin.h"
#include <algorithm>
#include <cmath>
#include <optional>
#include <limits>
#include <span>

namespace MaxRects {

	template<typename Numeric = float>
	constexpr Numeric edge_max_value = Numeric{4096};

	template<typename Numeric = float>
	constexpr Numeric edge_min_value = Numeric{128};

	template<typename RectType = Rectangle<float>, typename Numeric = float>
	class MaxRectsBin : public AbstractBin<RectType, Numeric> {
	public:
		bool vertical_expand{false};
		Rectangle<Numeric> stage{};
		Numeric border{Numeric{}};

		explicit MaxRectsBin(Numeric max_w = edge_max_value<Numeric>, Numeric max_h = edge_max_value<Numeric>,
							Numeric padding = Numeric{}, const PackingOptions<Numeric>& opts = {});    auto add(const RectType& rect) -> RectType* override;
		
		auto add(RectType&& rect) -> RectType* override;

		auto add(Numeric width, Numeric height, std::any data) -> RectType*;
		
		auto add_bulk(std::span<RectType> rects) -> std::vector<RectType*>;

		auto repack() -> std::vector<RectType> override;

		auto reset(bool deep_reset = false) -> void;

		auto clone() const -> std::unique_ptr<AbstractBin<RectType, Numeric>> override;

		auto find_position_for_new_node_bottom_left(Numeric width, Numeric height, 
												Numeric& best_y, Numeric& best_x) const -> bool;    auto find_position_for_new_node_best_short_side_fit(Numeric width, Numeric height,
														Numeric& best_short_side_fit, Numeric& best_long_side_fit) -> Rectangle<Numeric>;

		auto find_position_for_new_node_best_long_side_fit(Numeric width, Numeric height,
														Numeric& best_short_side_fit, Numeric& best_long_side_fit) -> Rectangle<Numeric>;

		auto find_position_for_new_node_best_area_fit(Numeric width, Numeric height,
													Numeric& best_area_fit, Numeric& best_short_side_fit) -> Rectangle<Numeric>;

		auto place_rectangle(const Rectangle<Numeric>& node) -> void;

		auto split_free_node(const Rectangle<Numeric>& used_node) -> void;
		
		auto split_free_rect_by_node(const Rectangle<Numeric>& free_rect, const Rectangle<Numeric>& used_node) -> bool;

		auto prune_free_list() -> void;
		
		auto place(const RectType& rect) -> std::optional<RectType>;

		auto find_best_position(Numeric width, Numeric height) -> Rectangle<Numeric>;

		auto finalize_placement(const RectType& rect, const Rectangle<Numeric>& position) -> RectType;

		auto update_bin_size(const Rectangle<Numeric>& placed_rect) -> void;
		
		auto next_power_of_two(Numeric value) -> Numeric;

	protected:
		std::vector<Rectangle<Numeric>> free_rectangles{};
		std::vector<Rectangle<Numeric>> used_rectangles{};

		auto calculate_max_dimensions() -> void override;
	};

}
