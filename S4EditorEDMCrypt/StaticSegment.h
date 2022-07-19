#pragma once
#include "S4MapSegmentHeader.h"
#include "MapData.h"
#include "PlayerData.h"
#include "Settler.h"
#include "Building.h"
#include "Stack.h"
#include "VictoryConditions.h"
#include "EditorData.h"
#include "DummyData.h"
#include "TeamData.h"
#include "ByteBufferReader.h"

#include <functional>

namespace S4MAP::S4MapFile::Segments {
	static_assert(sizeof(char) == 1, "char type must be 1 byte");
	static_assert(sizeof(bool) == 1, "bool type must be 1 byte");
	static_assert(sizeof(Data::Static::SDummyData) == 0x4, "Data Segment {SDummyData} Struct not 4 Bytes");
	static_assert(sizeof(Data::Static::SMapData) == 0x18, "Data Segment {SMapData} Struct not 0x18 Bytes");
	static_assert(sizeof(Data::Static::SPlayerData) == 0x2D, "Data Segment {SPlayerData} Struct not 0x2D Bytes");

	// variable size static_assert(sizeof(STeamData) == 1, "Data Segment {STeamData} Struct not xxxxxxxx Bytes");
	static_assert(sizeof(Data::Static::STeamData::STeamPlayerData) == 2, "Data Segment {STeamData::STeamPlayerData} Struct not 2 Bytes");

	static_assert(sizeof(Data::Static::SSettler) == 0x6, "Data Segment {SSettler} Struct not 6 Bytes");
	static_assert(sizeof(Data::Static::SBuilding) == 0x14, "Data Segment {SBuilding} Struct not 4 Bytes");
	static_assert(sizeof(Data::Static::SStack) == 0x8, "Data Segment {SStack} Struct not 8 Bytes");
	static_assert(sizeof(Data::Static::SVictoryConditions) == 0x53, "Data Segment {SVictoryConditions} Struct not 0x53 Bytes");
	static_assert(sizeof(Data::Static::SEditorData) == 0x14, "Data Segment {SVictoryConditions} Struct not 0x14 Bytes");


	struct SStaticSegment {
		template <typename T>
		static void deserializeSegment(T& v,const SequenceByteContainer& inflatedContent)
		{
			S4::Util::CByteBufferReader buf(inflatedContent);
			if constexpr (std::is_same<T,SequenceContainer<Data::Static::SSettler>>::value)
			{
				foreachInflatedChunkTypeArray<Data::Static::SSettler>(buf, v, [](Data::Static::SSettler& newSettler, S4::Util::CByteBufferReader& buf) {
					buf >> newSettler.m_x
						>> newSettler.m_y
						>> newSettler.m_settlerType
						>> newSettler.m_party;
					});
			}
			else if constexpr (std::is_same<T,SequenceContainer < Data::Static::SBuilding>>::value) {
				foreachInflatedChunkTypeArray<Data::Static::SBuilding>(buf, v, [](Data::Static::SBuilding& newBuilding,S4::Util::CByteBufferReader& buf) {
					buf >> newBuilding.m_x >> newBuilding.m_y
						>> newBuilding.m_buildingType >> newBuilding.m_party
						>> newBuilding.m_occupied 
						>> newBuilding.m_swordManLv1Count >> newBuilding.m_swordManLv2Count >> newBuilding.m_swordManLv3Count
						>> newBuilding.m_bowManLv1Count >> newBuilding.m_bowManLv2Count >> newBuilding.m_bowManLv3Count
						>> newBuilding.m_unk0xD_always0>> newBuilding.m_unk0xE_always0 >> newBuilding.m_unk0xF_always0
						>> newBuilding.m_unk0x10_always0 >> newBuilding.m_unk0x11_always0 >> newBuilding.m_unk0x12_always0
						>> newBuilding.m_unk0x13_always0;
					});
			}
			else if constexpr (std::is_same<T,SequenceContainer < Data::Static::SStack>>::value) {
				foreachInflatedChunkTypeArray<Data::Static::SStack>(buf, v, [](Data::Static::SStack& newStack, S4::Util::CByteBufferReader& buf) {
					buf >> newStack.m_x
						>> newStack.m_y
						>> newStack.m_stackType
						>> newStack.m_amount
						>> newStack.m_alwaysMinus2
						>> newStack.m_unused;
					// special case in editor (has to be modified) where this can be 0, something with stack amount will be different too. Not a single map has it != -2 though 
					/*if (newStack.m_alwaysMinus2 != -2) throw std::runtime_error("alwaysMinus2 not Minus2"); */
					});
			}
			else if constexpr (std::is_same<T,Data::Static::STeamData>::value) {
				constexpr auto minimumSize = sizeof(Data::Static::STeamData::m_constellationName) 
					+ sizeof(Data::Static::STeamData::m_alwaysNullTerminator) 
					+ sizeof(Data::Static::STeamData::STeamPlayerData);
				constexpr auto maximumSize = minimumSize + sizeof(Data::Static::STeamData::STeamPlayerData) * 7;
				if (inflatedContent.size() < minimumSize || inflatedContent.size() > maximumSize || inflatedContent.size() % 2 != 1) // 33 bytes + 2 bytes * n  n in[1,8]
					throw std::runtime_error("Bytes to deserialize STeamData Struct not fitting size: "s + std::to_string(inflatedContent.size()));

				for (size_t i = 0; i < sizeof(Data::Static::STeamData::m_constellationName);i++)
					buf >> v.m_constellationName[i];

				buf >> v.m_alwaysNullTerminator;

				size_t offs = sizeof(Data::Static::STeamData::m_constellationName) + sizeof(Data::Static::STeamData::m_alwaysNullTerminator);
				size_t players = (buf.getBufferSize() - offs) / sizeof(Data::Static::STeamData::STeamPlayerData);
				for (size_t i = 0; i < players; i++)
				{
					ETeam team;
					EPlayerType playerType;
					buf >> team >> playerType;
					v.m_teamPlayerData.push_back(Data::Static::STeamData::STeamPlayerData{ team,playerType }); // emplace_back in c++20 without needing to make ctor pls :(
				}
			}
			else if constexpr (std::is_same<T,SequenceContainer<SGroundData>>::value) {
				foreachInflatedChunkTypeArray<SGroundData>(buf, v, [](SGroundData& newGroundData, S4::Util::CByteBufferReader& buf) {
					buf >> newGroundData.m_objectID
						>> newGroundData.m_landOwnership
						>> newGroundData.m_occupancyFlags
						>> newGroundData.m_resource;
					});
			}
			else if constexpr (std::is_same<T,SequenceContainer<SGround>>::value) {
				foreachInflatedChunkTypeArray<SGround>(buf, v, [](SGround& newGround, S4::Util::CByteBufferReader& buf) {
					buf >> newGround.m_elevation
						>> newGround.m_groundType
						>> newGround.bitflags;
					});
			}
			else if constexpr (std::is_same<T,SequenceContainer<Data::Static::SPlayerData>>::value) {
				foreachInflatedChunkTypeArray<Data::Static::SPlayerData>(buf, v,
					[](Data::Static::SPlayerData& newPlayerData, S4::Util::CByteBufferReader& buf)
					{
						buf >> newPlayerData.m_tribe
							>> newPlayerData.m_startPosX
							>> newPlayerData.m_startPosY;

						size_t i = 0;

						for (size_t i = 0; i < 32; i++) {
							buf >> newPlayerData.m_playerName[i];
						}

						buf >> newPlayerData.m_alwaysNullTerminator;
					}
				);
			}
			else { 
				// simple structs, assert simple size
				assertSize<T>(inflatedContent, "Data Segment "s  + typeid(T).name() + " does not equal size of Segment struct");
				if constexpr (std::is_same<T,Data::Static::SDummyData>::value)
				{
					buf >> v.m_randomNum1To0xFFFFFFE;
				}
				else if constexpr (std::is_same<T,Data::Static::SVictoryConditions>::value) {
					size_t off = 0;

					/* DefeatPlayer */
					buf >> v.m_defeatPlayerVictoryCondition.m_enable;
					for (auto& b : v.m_defeatPlayerVictoryCondition.m_defeatPlayerBool){
						buf >> b.m_hasToDefeatPlayer;
					}
					/* DestroyBuilding */
					buf >> v.m_destroyBuildingVictoryCondition.m_enable;
					for (auto& cond : v.m_destroyBuildingVictoryCondition.m_conditions)
					{
						buf >> cond.m_fromPlayer >> cond.m_buildingToDestroy;
					}

					/* OwnLands */
					buf >> v.m_ownLandsVictoryCondition.m_enable;
					for (auto& cond : v.m_ownLandsVictoryCondition.m_conditions)
					{
						buf >> cond.m_enable >> cond.m_x >> cond.m_y;
					}

					/* TimeSurvive */
					buf >> v.m_timeVictoryCondition.m_enable;
					for (auto& cond : v.m_timeVictoryCondition.m_conditions)
					{
						buf >> cond.m_playerTimeHasToSurvive;
					}

					/* ProduceGoods*/
					buf >> v.m_produceGoodsVictoryCondition.m_enable;
					for (auto& cond : v.m_produceGoodsVictoryCondition.m_produceGoods) {
						buf >> cond.m_amount >> cond.m_stackType;
					}
				}
				else if constexpr (std::is_same<T,Data::Static::SEditorData>::value) {
					buf >> v.m_editorVersion 
						>> v.m_unk2 
						>> v.m_unk4 
						>> v.m_unk6
						>> v.m_unusedInModern1
						>> v.m_unusedInModern1
						>> v.m_unusedInModern1;
				}
				else if constexpr (std::is_same<T,Data::Static::SMapData>::value) {
					buf >> v.m_gameMode 
						>> v.m_playerCount 
						>> v.m_startResource 
						>> v.m_mapSize 
						>> v.m_unusedIsZero 
						>> v.m_mapFlags;
				}
				else {
					static_assert("Given type in deserialize not a static segment to deserialize");
				}
			}
			if (buf.bytesLeft() > 0) {
				StringStream ss;
				ss << buf.bytesLeft() << " bytes still left after deserializing " << typeid(T).name() << "\n";
				throw std::runtime_error(ss.str());
			}
		}
	private:

		template<typename T>
		static void foreachInflatedChunkTypeArray(S4::Util::CByteBufferReader& buf, SequenceContainer<T>& typePool, const std::function<void(T&, S4::Util::CByteBufferReader&)>& assignFunc)
		{
			/* Make sure container has fitting byte size for struct*/
			auto datasize = sizeof(T);
			assertMultipleSize(buf.getBufferSize(), datasize, "Segment has to be divisible by "s + std::to_string(datasize));
			/* Prepare bytes for all struct array */
			typePool.resize(buf.getBufferSize() / datasize);

			/* Assign all elements in struct by passing bytes to function that converts bytes to given struct T*/
			for (size_t i = 0; i < typePool.size(); i++)
			{
				size_t inflatedIndex = i * datasize;
				assignFunc(typePool.at(i), buf);
			}
		}
		static void assertSize(const SequenceByteContainer& container, size_t expectedContainerSize, const String& errMsg)
		{
			if (container.size() != expectedContainerSize)
				throw std::runtime_error(errMsg);
		}
		template<typename T>
		static void assertSize(const SequenceByteContainer& container, const String& errMsg)
		{
			if (container.size() != sizeof(T))
				throw std::runtime_error(errMsg);
		}
		template<typename T>
		static void assertSize(size_t size, const String& errMsg)
		{
			if (size != sizeof(T))
				throw std::runtime_error(errMsg);
		}

		static void assertMultipleSize(const SequenceByteContainer& container, size_t multipleOfSize, const String& errMsg)
		{
			if (container.size() % multipleOfSize != 0)
				throw std::runtime_error(errMsg);
		}
		static void assertMultipleSize(size_t size, size_t multipleOfSize, const String& errMsg)
		{
			if (size % multipleOfSize != 0)
				throw std::runtime_error(errMsg);
		}
	};
}