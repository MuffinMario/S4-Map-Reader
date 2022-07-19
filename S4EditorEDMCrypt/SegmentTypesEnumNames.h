#pragma once
#include "SegmentTypesEnum.h"
#include <map>
namespace S4MAP::S4MapFile::Segments {
	const std::map<EStaticSegmentIDs, StringView> SEGMENT_HEADER_IDENTIFIERS{
		{EStaticSegmentIDs::HEADER_SIGNAL_END,"End Segment Signal"},
		{EStaticSegmentIDs::MAP_INFO,"Map Information"},
		{EStaticSegmentIDs::PLAYER_INFO,"Player Information"},
		{EStaticSegmentIDs::TEAM_INFO,"Team Information"},
		{EStaticSegmentIDs::MAP_PREVIEW_IMAGE,"Map Preview"},
		{EStaticSegmentIDs::RANDOM_UNUSED_MAYBE,"Random Map Data - Unused (?)"},
		{EStaticSegmentIDs::GROUND_DATA,"Ground Data"},
		{EStaticSegmentIDs::SETTLERS,"Settlers"},
		{EStaticSegmentIDs::BUILDINGS,"Buildings"},
		{EStaticSegmentIDs::STACKS,"Stacks"},
		{EStaticSegmentIDs::VICTORY_CONDITIONS,"Victory Condition"},
		{EStaticSegmentIDs::LOCAL_DESCRIPTION,"Description Local"},
		{EStaticSegmentIDs::LOCAL_TIPS,"Tips Local"},
		{EStaticSegmentIDs::GROUND,"Landscape/Ground"},
		{EStaticSegmentIDs::ENGLISH_DESCRIPTION,"Description English"},
		{EStaticSegmentIDs::ENGLISH_TIPS,"Tips English"},
		{EStaticSegmentIDs::LUA_SCRIPT,"Lua Script"},
		{EStaticSegmentIDs::HEADER_SIGNAL_EDM ,"EDM Format Signal"},
		{EStaticSegmentIDs::EDITOR_INFO ,"Editor Information"}
	};
}