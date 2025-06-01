#pragma once

#include "rectangle.h"
#include <vector>
#include <memory>
#include <any>
#include <string>

namespace MaxRects {

	enum struct PackingLogic : std::uint8_t {
		MaxArea = 0,
		MaxEdge = 1,
		FillWidth = 2
	};

	template<typename Numeric = float>
	struct PackingOptions {
		bool smart{true};
		bool pot{true};
		bool square{false};
		bool allow_rotation{false};
		bool tag{false};
		bool exclusive_tag{true};
		Numeric border{Numeric{}};
		PackingLogic logic{PackingLogic::MaxEdge};
	};

	template<typename RectType = Rectangle<float>, typename Numeric = float>
	class AbstractBin {
	public:
		std::vector<RectType> rects{};
		Numeric width{Numeric{}};
		Numeric height{Numeric{}};
		Numeric max_width{Numeric{}};
		Numeric max_height{Numeric{}};
		PackingOptions<Numeric> options{};
		std::any tag{};
		std::size_t dirty_counter{std::size_t{}};

		explicit AbstractBin(Numeric w = Numeric{}, Numeric h = Numeric{},
							const PackingOptions<Numeric>& opts = {});

		virtual ~AbstractBin() = default;

		virtual auto add(const RectType& rect) -> RectType* = 0;

		virtual auto repack() -> std::vector<RectType> = 0;

		virtual auto clone() const -> std::unique_ptr<AbstractBin<RectType, Numeric>> = 0;

		[[nodiscard]] virtual auto is_dirty() const noexcept -> bool;

		virtual auto set_dirty(bool is_dirty) noexcept -> void;

		virtual auto reset() -> void;

		auto update_size() -> void;

	protected:
		virtual auto calculate_max_dimensions() -> void = 0;
	};

}
