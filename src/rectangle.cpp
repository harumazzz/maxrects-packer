#include "Rectangle.h"

namespace MaxRects {

	template<typename Numeric>
	constexpr auto Rectangle<Numeric>::area() const noexcept -> Numeric {
		return w * h;
	}

	template<typename Numeric>
	constexpr auto Rectangle<Numeric>::collides_with(const Rectangle& other) const noexcept -> bool {
		return other.x < x + w &&
			other.x + other.w > x &&
			other.y < y + h &&
			other.y + other.h > y;
	}

	template<typename Numeric>
	constexpr auto Rectangle<Numeric>::contains(const Rectangle& other) const noexcept -> bool {
		return other.x >= x && other.y >= y &&
			other.x + other.w <= x + w &&
			other.y + other.h <= y + h;
	}

	template<typename Numeric>
	constexpr auto Rectangle<Numeric>::set_width(Numeric new_width) noexcept -> void {
		if (new_width != w) {
			w = new_width;
			dirty_counter = dirty_counter + std::size_t{1};
		}
	}

	template<typename Numeric>
	constexpr auto Rectangle<Numeric>::set_height(Numeric new_height) noexcept -> void {
		if (new_height != h) {
			h = new_height;
			dirty_counter = dirty_counter + std::size_t{1};
		}
	}

	template<typename Numeric>
	constexpr auto Rectangle<Numeric>::set_x(Numeric new_x) noexcept -> void {
		if (new_x != x) {
			x = new_x;
			dirty_counter = dirty_counter + std::size_t{1};
		}
	}

	template<typename Numeric>
	constexpr auto Rectangle<Numeric>::set_y(Numeric new_y) noexcept -> void {
		if (new_y != y) {
			y = new_y;
			dirty_counter = dirty_counter + std::size_t{1};
		}
	}

	template<typename Numeric>
	constexpr auto Rectangle<Numeric>::set_rotation(bool new_rot) noexcept -> void {
		if (!allow_rotation) return;
		
		if (rot != new_rot) {
			const auto tmp = w;
			w = h;
			h = tmp;
			rot = new_rot;
			dirty_counter = dirty_counter + std::size_t{1};
		}
	}

	template<typename Numeric>
	constexpr auto Rectangle<Numeric>::set_allow_rotation(bool allow_rot) noexcept -> void {
		if (allow_rotation != allow_rot) {
			allow_rotation = allow_rot;
			dirty_counter = dirty_counter + std::size_t{1};
		}
	}

	template<typename Numeric>
	auto Rectangle<Numeric>::set_data(std::any new_data) -> void {
		data = std::move(new_data);
		dirty_counter = dirty_counter + std::size_t{1};
	}

	template<typename Numeric>
	constexpr auto Rectangle<Numeric>::is_dirty() const noexcept -> bool {
		return dirty_counter > std::size_t{};
	}

	template<typename Numeric>
	constexpr auto Rectangle<Numeric>::set_dirty(bool is_dirty) noexcept -> void {
		dirty_counter = is_dirty ? dirty_counter + std::size_t{1} : std::size_t{};
	}

	template<typename Numeric>
	constexpr auto Rectangle<Numeric>::operator==(const Rectangle& other) const noexcept -> bool {
		return w == other.w && h == other.h && 
			x == other.x && y == other.y;
	}


	template struct Rectangle<float>;

	template struct Rectangle<double>;

	template struct Rectangle<int>;

}
