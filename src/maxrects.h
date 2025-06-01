#pragma once

#include "rectangle.h"
#include "abstract_bin.h"
#include "maxrects_bin.h"
#include "oversized_element_bin.h"
#include "maxrects_packer.h"

namespace MaxRects {

	using Packer = MaxRectsPacker<Rectangle<float>, float>;

	using Bin = MaxRectsBin<Rectangle<float>, float>;

	using OversizedBin = OversizedElementBin<Rectangle<float>, float>;

}
