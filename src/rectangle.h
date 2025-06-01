#pragma once

#include <cstddef>
#include <any>
#include <type_traits>

#ifdef _MSC_VER
#pragma warning(disable: 4201)
#endif

namespace MaxRects {
	template<typename T>
	concept number_type = std::is_arithmetic_v<T>;
	template<typename Numeric = float>
	struct Rectangle {
		Numeric w{Numeric{}};
		Numeric h{Numeric{}};
		Numeric x{Numeric{}};
		Numeric y{Numeric{}};
		std::any data{};   
		std::size_t dirty_counter{std::size_t{0}};

		struct {
			bool oversized : 1;
			bool rot : 1;
			bool allow_rotation : 1;
		};
		
		constexpr Rectangle() {
			oversized = false;
			rot = false;
			allow_rotation = false;
		}
		
		constexpr Rectangle(Numeric width, Numeric height, Numeric x_pos = Numeric{}, Numeric y_pos = Numeric{}, 
						bool rotation = false, bool allow_rot = false) noexcept
			: w{width}, h{height}, x{x_pos}, y{y_pos} {
			oversized = false;
			rot = rotation;
			allow_rotation = allow_rot;
		}

		constexpr Rectangle(Numeric width, Numeric height, std::any user_data, Numeric x_pos = Numeric{}, Numeric y_pos = Numeric{}, 
						bool rotation = false, bool allow_rot = false) noexcept
			: w{width}, h{height}, x{x_pos}, y{y_pos}, data{std::move(user_data)} {
			oversized = false;
			rot = rotation;
			allow_rotation = allow_rot;
		}

		Rectangle(const Rectangle&) = default;
		Rectangle(Rectangle&&) noexcept = default;
		Rectangle& operator=(const Rectangle&) = default;
		Rectangle& operator=(Rectangle&&) noexcept = default;

		[[nodiscard]] constexpr auto area() const noexcept -> Numeric;

		[[nodiscard]] constexpr auto collides_with(const Rectangle& other) const noexcept -> bool;

		[[nodiscard]] constexpr auto contains(const Rectangle& other) const noexcept -> bool;

		constexpr auto set_width(Numeric new_width) noexcept -> void;

		constexpr auto set_height(Numeric new_height) noexcept -> void;

		constexpr auto set_x(Numeric new_x) noexcept -> void;

		constexpr auto set_y(Numeric new_y) noexcept -> void;

		constexpr auto set_rotation(bool new_rot) noexcept -> void;

		constexpr auto set_allow_rotation(bool allow_rot) noexcept -> void;

		auto set_data(std::any new_data) -> void;

		[[nodiscard]] constexpr auto is_dirty() const noexcept -> bool;

		constexpr auto set_dirty(bool is_dirty = true) noexcept -> void;

		[[nodiscard]] static constexpr auto collide(const Rectangle& first, const Rectangle& second) noexcept -> bool {
			return first.collides_with(second);
		}

		[[nodiscard]] static constexpr auto contain(const Rectangle& first, const Rectangle& second) noexcept -> bool {
			return first.contains(second);
		}

		[[nodiscard]] constexpr auto operator==(const Rectangle& other) const noexcept -> bool;
	};

}
