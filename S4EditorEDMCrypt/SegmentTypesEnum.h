#pragma once
#include "types.h"

namespace S4MAP::S4MapFile::Segments {
	enum class EStaticSegmentIDs : uint16_t {
		HEADER_SIGNAL_END,
		MAP_INFO,
		PLAYER_INFO,
		TEAM_INFO,
		MAP_PREVIEW_IMAGE,
		// in s3 apparently segment denoting random map containing seed, mirror information,...
		RANDOM_UNUSED_MAYBE,
		GROUND_DATA,
		SETTLERS,
		BUILDINGS,
		STACKS,
		VICTORY_CONDITIONS,
		LOCAL_DESCRIPTION,
		LOCAL_TIPS,
		GROUND,
		ENGLISH_DESCRIPTION,
		ENGLISH_TIPS,
		LUA_SCRIPT,
		HEADER_SIGNAL_EDM = 0x40,
		// 0x41 unused (implemented in editor though)
		EDITOR_INFO = 0x42
	};
}