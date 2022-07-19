#pragma once
#include "SegmentEnums.h"

namespace S4MAP::S4MapFile::Segments::Data::Static {
	struct SBuilding {
		uint16_t m_x;
		uint16_t m_y;
		EBuildingType m_buildingType;
		uint8_t m_party;
		// always 1, only affects guard towers with soldiers inside
		uint8_t m_occupied;
		uint8_t m_swordManLv1Count;
		uint8_t m_swordManLv2Count;
		uint8_t m_swordManLv3Count;
		uint8_t m_bowManLv1Count;
		uint8_t m_bowManLv2Count;
		uint8_t m_bowManLv3Count;
		uint8_t m_unk0xD_always0;
		uint8_t m_unk0xE_always0;
		uint8_t m_unk0xF_always0;
		uint8_t m_unk0x10_always0;
		uint8_t m_unk0x11_always0;
		uint8_t m_unk0x12_always0;
		uint8_t m_unk0x13_always0;
	};
}