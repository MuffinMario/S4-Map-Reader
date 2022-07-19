#pragma once
#include "SegmentEnums.h"

namespace S4MAP::S4MapFile::Segments::Data::Static {
	struct SMapData {
		// 0 = Map Type A - Conflict
		// 1 = Map Type B - Conflict
		// 2 = Map Type A - Coop
		// 3 = Map Type B - Coop
		uint32_t m_gameMode;

		uint32_t m_playerCount;
		// 1 = low 2 = medium 3 = high
		EStartResources m_startResource;

		uint32_t m_mapSize;
		uint32_t m_unusedIsZero;
		uint32_t m_mapFlags;
	};
}