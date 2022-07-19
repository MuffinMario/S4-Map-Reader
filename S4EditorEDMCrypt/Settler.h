#pragma once
#include "SegmentEnums.h"

namespace S4MAP::S4MapFile::Segments::Data::Static {
	struct SSettler {
		uint16_t m_x;
		uint16_t m_y;
		ESettlerType m_settlerType;
		uint8_t m_party;
	};
}