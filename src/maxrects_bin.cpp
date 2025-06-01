#include "maxrects_bin.h"
#include <optional>
#include <limits>
#include <algorithm>
#include <cmath>

namespace MaxRects {

	template<typename RectType, typename Numeric>
	MaxRectsBin<RectType, Numeric>::MaxRectsBin(Numeric max_w, Numeric max_h, Numeric padding, const PackingOptions<Numeric>& opts)
		: stage{Numeric{}, Numeric{}} {
		
		this->max_width = max_w;
		this->max_height = max_h;
		this->options = opts;
		this->width = this->options.smart ? Numeric{} : max_w;
		this->height = this->options.smart ? Numeric{} : max_h;
		border = this->options.border;
		
		free_rectangles.emplace_back(
			this->max_width + padding - border * Numeric{2},
			this->max_height + padding - border * Numeric{2},
			border,
			border
		);
		
		stage = Rectangle<Numeric>{this->width, this->height};
	}

	template<typename RectType, typename Numeric>
	auto MaxRectsBin<RectType, Numeric>::add(const RectType& rect) -> RectType* {
		if (this->options.tag && this->options.exclusive_tag) {
			
		}
		auto best_short_side_fit = std::numeric_limits<Numeric>::max();
		auto best_long_side_fit = std::numeric_limits<Numeric>::max();
		auto best_area_fit = std::numeric_limits<Numeric>::max();

		auto new_node = Rectangle<Numeric>{};

		if (this->options.logic == PackingLogic::MaxArea) {
			new_node = find_position_for_new_node_best_area_fit(rect.w, rect.h, best_area_fit, best_short_side_fit);
		} else if (this->options.logic == PackingLogic::MaxEdge) {
			new_node = find_position_for_new_node_best_long_side_fit(rect.w, rect.h, best_short_side_fit, best_long_side_fit);
		} else {
			new_node = find_position_for_new_node_best_short_side_fit(rect.w, rect.h, best_short_side_fit, best_long_side_fit);
		}
		
		if (new_node.h == Numeric{}) {
			if (this->options.allow_rotation) {
				if (this->options.logic == PackingLogic::MaxArea) {
					new_node = find_position_for_new_node_best_area_fit(rect.h, rect.w, best_area_fit, best_short_side_fit);
				} else if (this->options.logic == PackingLogic::MaxEdge) {
					new_node = find_position_for_new_node_best_long_side_fit(rect.h, rect.w, best_short_side_fit, best_long_side_fit);
				} else {
					new_node = find_position_for_new_node_best_short_side_fit(rect.h, rect.w, best_short_side_fit, best_long_side_fit);
				}
				
				if (new_node.h != Numeric{}) {
					new_node.rot = true;
				}
			}
		}
		
		if (new_node.h == Numeric{}) {
			return nullptr;
		}
		
		place_rectangle(new_node);
		
		auto result_rect = rect;
		result_rect.x = new_node.x;
		result_rect.y = new_node.y;
		result_rect.rot = new_node.rot;
		if (new_node.rot) {
			result_rect.w = rect.h;
			result_rect.h = rect.w;
		}
				this->rects.push_back(result_rect);
		this->set_dirty(true);
		
		if (this->options.smart) {
			calculate_max_dimensions();
		}
		return &this->rects.back();
	}

	template<typename RectType, typename Numeric>
	auto MaxRectsBin<RectType, Numeric>::add(RectType&& rect) -> RectType* {
		if (this->options.tag && this->options.exclusive_tag) {
			
		}
		auto best_short_side_fit = std::numeric_limits<Numeric>::max();
		auto best_long_side_fit = std::numeric_limits<Numeric>::max();
		auto best_area_fit = std::numeric_limits<Numeric>::max();

		auto new_node = Rectangle<Numeric>{};

		if (this->options.logic == PackingLogic::MaxArea) {
			new_node = find_position_for_new_node_best_area_fit(rect.w, rect.h, best_area_fit, best_short_side_fit);
		} else if (this->options.logic == PackingLogic::MaxEdge) {
			new_node = find_position_for_new_node_best_long_side_fit(rect.w, rect.h, best_short_side_fit, best_long_side_fit);
		} else {
			new_node = find_position_for_new_node_best_short_side_fit(rect.w, rect.h, best_short_side_fit, best_long_side_fit);
		}
		
		if (new_node.h == Numeric{}) {
			if (this->options.allow_rotation) {
				if (this->options.logic == PackingLogic::MaxArea) {
					new_node = find_position_for_new_node_best_area_fit(rect.h, rect.w, best_area_fit, best_short_side_fit);
				} else if (this->options.logic == PackingLogic::MaxEdge) {
					new_node = find_position_for_new_node_best_long_side_fit(rect.h, rect.w, best_short_side_fit, best_long_side_fit);
				} else {
					new_node = find_position_for_new_node_best_short_side_fit(rect.h, rect.w, best_short_side_fit, best_long_side_fit);
				}
				
				if (new_node.h != Numeric{}) {
					new_node.rot = true;
				}
			}
		}
		
		if (new_node.h == Numeric{}) {
			return nullptr;
		}
		
		place_rectangle(new_node);
		
		rect.x = new_node.x;
		rect.y = new_node.y;
		rect.rot = new_node.rot;
		if (new_node.rot) {
			auto temp_w = rect.w;
			rect.w = rect.h;
			rect.h = temp_w;
		}
		
		this->rects.push_back(std::move(rect));
		this->set_dirty(true);
		
		if (this->options.smart) {
			calculate_max_dimensions();
		}
		return &this->rects.back();
	}	template<typename RectType, typename Numeric>
	auto MaxRectsBin<RectType, Numeric>::add(Numeric width, Numeric height, std::any data) -> RectType* {
		if constexpr (std::is_same_v<RectType, Rectangle<Numeric>>) {
			auto rect = RectType{width, height, std::move(data)};
			return add(std::move(rect));
		} else {
			auto rect = RectType{width, height};
			return add(std::move(rect));
		}
	}

	template<typename RectType, typename Numeric>
	auto MaxRectsBin<RectType, Numeric>::add_bulk(std::span<RectType> rects) -> std::vector<RectType*> {
		auto results = std::vector<RectType*>{};
		results.reserve(rects.size());
		
		this->rects.reserve(this->rects.size() + rects.size());
		
		for (auto& rect : rects) {
			if (auto* added = add(std::move(rect))) {
				results.push_back(added);
			} else {
				results.push_back(nullptr);
			}
		}
		
		return results;
	}

	template<typename RectType, typename Numeric>
	auto MaxRectsBin<RectType, Numeric>::find_position_for_new_node_best_short_side_fit(
		Numeric width, Numeric height, 
		Numeric& best_short_side, Numeric& best_long_side) -> Rectangle<Numeric> {

		auto best_node = Rectangle<Numeric>{};
		best_short_side = std::numeric_limits<Numeric>::max();
		
		for (const auto& free_rect : free_rectangles) {
			if (free_rect.w >= width && free_rect.h >= height) {
				const auto leftover_horizontal = std::abs(free_rect.w - width);
				const auto leftover_vertical = std::abs(free_rect.h - height);
				const auto short_side = std::min(leftover_horizontal, leftover_vertical);
				const auto long_side = std::max(leftover_horizontal, leftover_vertical);
				
				if (short_side < best_short_side || 
					(short_side == best_short_side && long_side < best_long_side)) {
					best_node.x = free_rect.x;
					best_node.y = free_rect.y;
					best_node.w = width;
					best_node.h = height;
					best_short_side = short_side;
					best_long_side = long_side;
				}
			}
		}
		
		return best_node;
	}

	template<typename RectType, typename Numeric>
	auto MaxRectsBin<RectType, Numeric>::find_position_for_new_node_best_long_side_fit(
		Numeric width, Numeric height, 
		Numeric& best_short_side, Numeric& best_long_side) -> Rectangle<Numeric> {
		
		auto best_node = Rectangle<Numeric>{};
		best_long_side = std::numeric_limits<Numeric>::max();
		
		for (const auto& free_rect : free_rectangles) {
			if (free_rect.w >= width && free_rect.h >= height) {
				const auto leftover_horizontal = std::abs(free_rect.w - width);
				const auto leftover_vertical = std::abs(free_rect.h - height);
				const auto short_side = std::min(leftover_horizontal, leftover_vertical);
				const auto long_side = std::max(leftover_horizontal, leftover_vertical);
				
				if (long_side < best_long_side || 
					(long_side == best_long_side && short_side < best_short_side)) {
					best_node.x = free_rect.x;
					best_node.y = free_rect.y;
					best_node.w = width;
					best_node.h = height;
					best_short_side = short_side;
					best_long_side = long_side;
				}
			}
		}
		
		return best_node;
	}

	template<typename RectType, typename Numeric>
	auto MaxRectsBin<RectType, Numeric>::find_position_for_new_node_best_area_fit(
		Numeric width, Numeric height, 
		Numeric& best_area_fit, Numeric& best_short_side) -> Rectangle<Numeric> {
		
		auto best_node = Rectangle<Numeric>{};
		best_area_fit = std::numeric_limits<Numeric>::max();
		
		for (const auto& free_rect : free_rectangles) {
			if (free_rect.w >= width && free_rect.h >= height) {
				const auto area_fit = free_rect.w * free_rect.h - width * height;
				const auto leftover_horizontal = std::abs(free_rect.w - width);
				const auto leftover_vertical = std::abs(free_rect.h - height);
				const auto short_side = std::min(leftover_horizontal, leftover_vertical);
				
				if (area_fit < best_area_fit || 
					(area_fit == best_area_fit && short_side < best_short_side)) {
					best_node.x = free_rect.x;
					best_node.y = free_rect.y;
					best_node.w = width;
					best_node.h = height;
					best_area_fit = area_fit;
					best_short_side = short_side;
				}
			}
		}
		
		return best_node;
	}

	template<typename RectType, typename Numeric>
	auto MaxRectsBin<RectType, Numeric>::place_rectangle(const Rectangle<Numeric>& node) -> void {
		auto num_rects_to_process = free_rectangles.size();
		for (auto i = size_t{0}; i < num_rects_to_process; ++i) {
			if (split_free_rect_by_node(free_rectangles[i], node)) {
				free_rectangles.erase(free_rectangles.begin() + i);
				--i;
				--num_rects_to_process;
			}
		}
		
		prune_free_list();
	}

	template<typename RectType, typename Numeric>
	auto MaxRectsBin<RectType, Numeric>::calculate_max_dimensions() -> void {
		if (this->rects.empty()) {
			this->width = this->options.smart ? Numeric{} : this->max_width;
			this->height = this->options.smart ? Numeric{} : this->max_height;
			return;
		}
		
		auto max_x = Numeric{};
		auto max_y = Numeric{};
		
		for (const auto& rect : this->rects) {
			const auto right_edge = rect.x + rect.w;
			const auto bottom_edge = rect.y + rect.h;
			max_x = std::max(max_x, right_edge);
			max_y = std::max(max_y, bottom_edge);
		}
		
		if (this->options.pot) {
			this->width = next_power_of_two(max_x);
			this->height = next_power_of_two(max_y);
		} else {
			this->width = max_x;
			this->height = max_y;
		}
		
		if (this->options.square) {
			const auto max_dimension = std::max(this->width, this->height);
			this->width = max_dimension;
			this->height = max_dimension;
		}
	}
	template<typename RectType, typename Numeric>
	auto MaxRectsBin<RectType, Numeric>::repack() -> std::vector<RectType> {
		auto unpacked = std::vector<RectType>{};
		unpacked.reserve(this->rects.size());
		
		reset(false);
		auto indices = std::vector<std::size_t>(this->rects.size());
		for (auto i = std::size_t{0}; i < indices.size(); ++i) {
			indices[i] = i;
		}
		std::sort(indices.begin(), indices.end(), [this](auto& a, auto& b) {
			const auto max_a = std::max(this->rects[a].w, this->rects[a].h);
			const auto max_b = std::max(this->rects[b].w, this->rects[b].h);
			return max_b < max_a;
		});
		auto removed_indices = std::vector<std::size_t>{};
		for (auto idx : indices) {
			if (!place(this->rects[idx]).has_value()) {
				unpacked.push_back(this->rects[idx]);
				removed_indices.push_back(idx);
			}
		}
		std::sort(removed_indices.begin(), removed_indices.end(), std::greater<std::size_t>());
		for (auto idx : removed_indices) {
			this->rects.erase(this->rects.begin() + idx);
		}
		
		return unpacked;
	}

	template<typename RectType, typename Numeric>
	auto MaxRectsBin<RectType, Numeric>::reset(bool deep_reset) -> void {
		if (deep_reset) {
			this->rects.clear();
		}
		this->width = this->options.smart ? Numeric{} : this->max_width;
		this->height = this->options.smart ? Numeric{} : this->max_height;
		this->free_rectangles.clear();
		
		this->free_rectangles.emplace_back(
			this->max_width - border * Numeric{2},
			this->max_height - border * Numeric{2},
			border,
			border
		);
		
		stage = Rectangle<Numeric>{this->width, this->height};
		vertical_expand = false;
		this->set_dirty(false);
	}

	template<typename RectType, typename Numeric>
	auto MaxRectsBin<RectType, Numeric>::clone() const -> std::unique_ptr<AbstractBin<RectType, Numeric>> {
		auto cloned = std::make_unique<MaxRectsBin<RectType, Numeric>>(
			this->max_width, this->max_height, Numeric{}, this->options);
		
		cloned->width = this->width;
		cloned->height = this->height;
		cloned->free_rectangles = this->free_rectangles;
		cloned->rects = this->rects;
		cloned->tag = this->tag;
		cloned->vertical_expand = vertical_expand;
		cloned->stage = stage;
		cloned->border = border;
		
		return std::move(cloned);
	}

	template<typename RectType, typename Numeric>
	auto MaxRectsBin<RectType, Numeric>::place(const RectType& rect) -> std::optional<RectType> {
		auto best_node = find_best_position(rect.w, rect.h);
		
		if (best_node.w == Numeric{}) {
			if (this->options.allow_rotation) {
				best_node = find_best_position(rect.h, rect.w);
				if (best_node.w != Numeric{}) {
					auto rotated_rect = rect;
					if constexpr (std::is_same_v<RectType, Rectangle<Numeric>>) {
						rotated_rect.set_rotation(!rect.rot);
					}
					best_node.w = rect.h;
					best_node.h = rect.w;
					return finalize_placement(rotated_rect, best_node);
				}
			}
			return std::nullopt;
		}
		
		return finalize_placement(rect, best_node);
	}

	template<typename RectType, typename Numeric>
	auto MaxRectsBin<RectType, Numeric>::find_best_position(Numeric width, Numeric height) -> Rectangle<Numeric> {
		auto best_node = Rectangle<Numeric>{};
		auto best_short_side = std::numeric_limits<Numeric>::max();
		auto best_long_side = std::numeric_limits<Numeric>::max();
		
		for (auto& free_rect : this->free_rectangles) {
			if (free_rect.w >= width && free_rect.h >= height) {
				const auto leftover_horizontal = free_rect.w - width;
				const auto leftover_vertical = free_rect.h - height;
				const auto short_side = std::min(leftover_horizontal, leftover_vertical);
				const auto long_side = std::max(leftover_horizontal, leftover_vertical);
				
				if (short_side < best_short_side || 
					(short_side == best_short_side && long_side < best_long_side)) {
					best_node.x = free_rect.x;
					best_node.y = free_rect.y;
					best_node.w = width;
					best_node.h = height;
					best_short_side = short_side;
					best_long_side = long_side;
				}
			}
		}
		
		return best_node;
	}

	template<typename RectType, typename Numeric>
	auto MaxRectsBin<RectType, Numeric>::finalize_placement(const RectType& rect, const Rectangle<Numeric>& position) -> RectType {
		split_free_node(position);
		prune_free_list();
		update_bin_size(position);
		
		auto placed_rect = rect;
		if constexpr (std::is_same_v<RectType, Rectangle<Numeric>>) {
			placed_rect.set_x(position.x);
			placed_rect.set_y(position.y);
			placed_rect.set_dirty(true);
		} else {
			placed_rect.x = position.x;
			placed_rect.y = position.y;
		}
		
		return placed_rect;
	}

	template<typename RectType, typename Numeric>
	auto MaxRectsBin<RectType, Numeric>::split_free_node(const Rectangle<Numeric>& used_node) -> void {
		for (auto i = this->free_rectangles.size(); i-- > std::size_t{};) {
			if (split_free_rect_by_node(this->free_rectangles[i], used_node)) {
				this->free_rectangles.erase(this->free_rectangles.begin() + i);
			}
		}
	}
	template<typename RectType, typename Numeric>
	auto MaxRectsBin<RectType, Numeric>::split_free_rect_by_node(const Rectangle<Numeric>& free_rect, const Rectangle<Numeric>& used_node) -> bool {
		if (used_node.x >= free_rect.x + free_rect.w ||
			used_node.x + used_node.w <= free_rect.x ||
			used_node.y >= free_rect.y + free_rect.h ||
			used_node.y + used_node.h <= free_rect.y) {
			return false;
		}
		
		auto new_rects = std::vector<Rectangle<Numeric>>{};
		new_rects.reserve(4);
		
		if (used_node.x < free_rect.x + free_rect.w && used_node.x + used_node.w > free_rect.x) {
			if (used_node.y > free_rect.y && used_node.y < free_rect.y + free_rect.h) {
				auto new_node = free_rect;
				new_node.h = used_node.y - new_node.y;
				new_rects.emplace_back(new_node);
			}
			
			if (used_node.y + used_node.h < free_rect.y + free_rect.h) {
				auto new_node = free_rect;
				new_node.y = used_node.y + used_node.h;
				new_node.h = free_rect.y + free_rect.h - (used_node.y + used_node.h);
				new_rects.emplace_back(new_node);
			}
		}
		
		if (used_node.y < free_rect.y + free_rect.h && used_node.y + used_node.h > free_rect.y) {
			if (used_node.x > free_rect.x && used_node.x < free_rect.x + free_rect.w) {
				auto new_node = free_rect;
				new_node.w = used_node.x - new_node.x;
				new_rects.emplace_back(new_node);
			}
			
			if (used_node.x + used_node.w < free_rect.x + free_rect.w) {
				auto new_node = free_rect;
				new_node.x = used_node.x + used_node.w;
				new_node.w = free_rect.x + free_rect.w - (used_node.x + used_node.w);
				new_rects.emplace_back(new_node);
			}
		}
		
		this->free_rectangles.insert(this->free_rectangles.end(),
									std::make_move_iterator(new_rects.begin()),
									std::make_move_iterator(new_rects.end()));
		
		return true;
	}

	template<typename RectType, typename Numeric>
	auto MaxRectsBin<RectType, Numeric>::prune_free_list() -> void {
		for (auto i = this->free_rectangles.size(); i-- > std::size_t{0};) {
			for (auto j = i + std::size_t{1}; j < this->free_rectangles.size(); ++j) {
				if (this->free_rectangles[j].contains(this->free_rectangles[i])) {
					this->free_rectangles.erase(this->free_rectangles.begin() + i);
					break;
				}
				if (this->free_rectangles[i].contains(this->free_rectangles[j])) {
					this->free_rectangles.erase(this->free_rectangles.begin() + j);
					--j;
				}
			}
		}
	}

	template<typename RectType, typename Numeric>
	auto MaxRectsBin<RectType, Numeric>::update_bin_size(const Rectangle<Numeric>& placed_rect) -> void {
		if (this->options.smart) {
			const auto right_edge = placed_rect.x + placed_rect.w;
			const auto bottom_edge = placed_rect.y + placed_rect.h;
			
			if (this->options.pot) {
				this->width = next_power_of_two(std::max(this->width, right_edge));
				this->height = next_power_of_two(std::max(this->height, bottom_edge));
			} else {
				this->width = std::max(this->width, right_edge);
				this->height = std::max(this->height, bottom_edge);
			}
			
			if (this->options.square) {
				const auto max_dimension = std::max(this->width, this->height);
				this->width = max_dimension;
				this->height = max_dimension;
			}
		}
	}

	template<typename RectType, typename Numeric>
	auto MaxRectsBin<RectType, Numeric>::next_power_of_two(Numeric value) -> Numeric {
		if (value <= Numeric{}) return Numeric{1};
		if constexpr (std::is_floating_point_v<Numeric>) {
			return static_cast<Numeric>(std::pow(Numeric{2}, std::ceil(std::log2(value))));
		} else {
			Numeric power = Numeric{1};
			while (power < value) {
				power *= Numeric{2};
			}
			return power;
		}
	}

	template<typename RectType, typename Numeric>
	auto MaxRectsBin<RectType, Numeric>::find_position_for_new_node_bottom_left(
		Numeric width, Numeric height, 
		Numeric& best_y, Numeric& best_x) const -> bool {
		
		best_x = std::numeric_limits<Numeric>::max();
		best_y = std::numeric_limits<Numeric>::max();
		
		for (const auto& free_rect : free_rectangles) {
			if (free_rect.w >= width && free_rect.h >= height) {
				
				if (free_rect.y + height < best_y || 
					(free_rect.y + height == best_y && free_rect.x < best_x)) {
					best_x = free_rect.x;
					best_y = free_rect.y + height;
				}
			}
		}
		
		return best_y != std::numeric_limits<Numeric>::max();
	}


	template class MaxRectsBin<Rectangle<float>, float>;

	template class MaxRectsBin<Rectangle<double>, double>;

	template class MaxRectsBin<Rectangle<int>, int>;

}
