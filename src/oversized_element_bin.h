#pragma once

#include "abstract_bin.h"

namespace MaxRects {

	template<typename RectType = Rectangle<float>, typename Numeric = float>
	class OversizedElementBin : public AbstractBin<RectType, Numeric> {
	public:
		explicit OversizedElementBin(const RectType& rect);    explicit OversizedElementBin(Numeric width, Numeric height, std::any data = {});

		auto add(const RectType&) -> RectType* override;
		
		auto add(Numeric width, Numeric height, std::any data) -> RectType*;

		auto repack() -> std::vector<RectType> override;

		auto clone() const -> std::unique_ptr<AbstractBin<RectType, Numeric>> override;
		
		auto reset() -> void override;

	protected:
		auto calculate_max_dimensions() -> void override;
	};

}
