#pragma once
#include "SegmentEnums.h"

namespace S4MAP::S4MapFile::Segments::Data::Static {
	struct SStack {
		uint16_t m_x;
		uint16_t m_y;
		EStackType m_stackType;
		uint8_t m_amount;
		//maybe 2 1-byte fields
		int8_t m_alwaysMinus2;
		uint8_t m_unused;
	};
}