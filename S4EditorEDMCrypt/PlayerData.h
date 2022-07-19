#pragma once
#include "SegmentEnums.h"

namespace S4MAP::S4MapFile::Segments::Data::Static {
#pragma pack(push,1)
	struct SPlayerData {
		ETribe m_tribe;
		uint32_t m_startPosX;
		uint32_t m_startPosY;
		StaticCharArray<32> m_playerName;
		uint8_t m_alwaysNullTerminator;
	};
#pragma pack(pop,1)
}