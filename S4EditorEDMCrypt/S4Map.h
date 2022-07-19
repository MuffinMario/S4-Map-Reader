#pragma once
#include "DummyData.h"
#include "MapData.h"
#include "PlayerData.h"
#include "TeamData.h"
#include "S4MapPreviewImage.h"
#include "GroundData.h"
#include "Settler.h"
#include "Building.h"
#include "Stack.h"
#include "VictoryConditions.h"
#include "Ground.h"
#include "EditorData.h"
#include <optional>


namespace S4MAP::S4MapFile {
	class CS4MapFile;
}
namespace S4MAP {
	class CS4Map {
		friend class S4MAP::S4MapFile::CS4MapFile;


		// Segment ID 0
		S4MapFile::Segments::Data::Static::SDummyData m_segment0x0_dummy{ 0 };

		// Segment ID 1
		S4MapFile::Segments::Data::Static::SMapData m_mapData;

		// Segment ID 2
		SequenceContainer<S4MapFile::Segments::Data::Static::SPlayerData> m_playerData;

		// Segment ID 3
		std::map<size_t, S4MapFile::Segments::Data::Static::STeamData> m_teamConstellations;

		// Segment ID 4
		S4MapFile::CS4MapPreviewImage m_previewImage;

		// Segment ID 5
		// unused ? only in Random Maps in-game?

		// Segment ID 6
		SequenceContainer<SGroundData> m_groundData;

		// Segment ID 7, sorted by coords
		SequenceContainer<S4MapFile::Segments::Data::Static::SSettler> m_settlers;

		// Segment ID 8
		SequenceContainer<S4MapFile::Segments::Data::Static::SBuilding> m_buildings;

		// Segment ID 9, sorted by coords
		SequenceContainer<S4MapFile::Segments::Data::Static::SStack> m_stacks;

		// Segment ID 10
		S4MapFile::Segments::Data::Static::SVictoryConditions m_victoryConditions;

		// Segment ID 11 and 12
		String m_localDescription;
		String m_localTips;

		// Segment ID 13
		SequenceContainer<SGround> m_ground;

		// Segment ID 14 and 15
		String m_englishDescription;
		String m_englishTips;

		// Segment ID 16
		String m_luaScript;

		// Segment ID 40
		S4MapFile::Segments::Data::Static::SDummyData m_segment0x40_dummy{ 0 }; // actually placeholder buffer, only header is important

		// Segment ID 42
		S4MapFile::Segments::Data::Static::SEditorData m_editorData;

		uint16_t m_mapSize;



		SequenceContainer<std::optional<S4MapFile::Segments::Data::Static::SBuilding>> m_buildingMap;
		SequenceContainer<std::optional<S4MapFile::Segments::Data::Static::SSettler>> m_settlerMap;
		SequenceContainer<std::optional<S4MapFile::Segments::Data::Static::SStack>> m_stackMap;


	public:
		CS4Map() :m_mapSize(0) {

		}
		CS4Map(uint16_t mapSize) :m_mapSize(mapSize) {
			m_ground.resize(mapSize * mapSize);
			m_groundData.resize(mapSize * mapSize);
		}

		/*
			Field Getter
		*/
		const uint16_t getMapSize() const { return m_mapSize; }
		const S4MapFile::Segments::Data::Static::SDummyData& getSegment0x0RandomNumber() const { return m_segment0x0_dummy; }
		const S4MapFile::Segments::Data::Static::SMapData& getMapData() const { return m_mapData; }
		const std::map<size_t, S4MapFile::Segments::Data::Static::STeamData>& getTeamData() const { return m_teamConstellations; }
		const S4MapFile::CS4MapPreviewImage& getPreviewImage() const { return m_previewImage; }
		const String& getLocalDescription() const { return m_localDescription; }
		const String& getLocalTips() const { return m_localTips; }
		const String& getEnglishDescription() const { return m_englishDescription; }
		const String& getEnglishTips() const { return m_englishTips; }
		const String& getLuaScript() const { return m_luaScript; }
		const S4MapFile::Segments::Data::Static::SEditorData& getEditorData() const { return m_editorData; }
		// Random number from segment 0x40. Has no value, mersenne twister forces value to be at least 1 though, so if this is 0 => no edm format
		const S4MapFile::Segments::Data::Static::SDummyData& getSegment0x40RandomNumber() const { return m_segment0x40_dummy; }
		const SequenceContainer<S4MapFile::Segments::Data::Static::SPlayerData>& getPlayerData() const { return m_playerData; }
		const SequenceContainer<SGround>& getGround() const { return m_ground; }
		const SequenceContainer<SGroundData>& getGroundData() const { return m_groundData; }
		const SequenceContainer<S4MapFile::Segments::Data::Static::SSettler>& getSettlers() const { return m_settlers; }
		const SequenceContainer<S4MapFile::Segments::Data::Static::SBuilding>& getBuildings() const { return m_buildings; }
		const SequenceContainer<S4MapFile::Segments::Data::Static::SStack>& getStacks() const { return m_stacks; }


		/*
			Field Setter
		*/
		void setSegment0x0RandomNumer(const S4MapFile::Segments::Data::Static::SDummyData& val) { m_segment0x0_dummy = val; }
		void setSegment0x40RandomNumer(const S4MapFile::Segments::Data::Static::SDummyData& val) { m_segment0x40_dummy = val; }
		void setMapData(const S4MapFile::Segments::Data::Static::SMapData& val) { m_mapData = val; }
		void setTeamData(const std::map<size_t, S4MapFile::Segments::Data::Static::STeamData>& val) { m_teamConstellations = val; }
		void setPreviewImage(const S4MapFile::CS4MapPreviewImage& val) { m_previewImage = val; }
		void setLocalDescription(const String& val) { m_localDescription = val; }
		void setLocalTips(const String& val) { m_localTips = val; }
		void setEnglishDescription(const String& val) { m_englishDescription = val; }
		void setEnglishTips(const String& val) { m_englishTips = val; }
		void setLuaScript(const String& val) { m_luaScript = val; }
		void setEditorData(const S4MapFile::Segments::Data::Static::SEditorData& val) { m_editorData = val; }

		void setPlayerData(const SequenceContainer<S4MapFile::Segments::Data::Static::SPlayerData>& cnt) {
			m_playerData.assign(cnt.begin(), cnt.end());
		}
		void setGroundData(const SequenceContainer<SGroundData>& cnt) {
			m_groundData.assign(cnt.begin(), cnt.end());
		}
		void setSettlers(const SequenceContainer<S4MapFile::Segments::Data::Static::SSettler>& cnt) {
			m_settlers.assign(cnt.begin(), cnt.end());
		}
		void setBuildings(const SequenceContainer<S4MapFile::Segments::Data::Static::SBuilding>& cnt) {
			m_buildings.assign(cnt.begin(), cnt.end());
		}
		void setStacks(const SequenceContainer<S4MapFile::Segments::Data::Static::SStack>& cnt) {
			m_stacks.assign(cnt.begin(), cnt.end());
		}
		void setGround(const SequenceContainer<SGround>& cnt) {
			m_ground.assign(cnt.begin(), cnt.end());
		}



		void generateSettlerMap() {
			m_settlerMap.resize(m_mapSize * m_mapSize, {});
			for (auto& v : m_settlers)
			{
				uint32_t i = v.m_x + v.m_y * m_mapSize;
				m_settlerMap.at(i) = v;
			}
		}

		void generateStackMap() {
			m_stackMap.resize(m_mapSize * m_mapSize, {});
			for (auto& v : m_stacks)
			{
				uint32_t i = v.m_x + v.m_y * m_mapSize;
				m_stackMap.at(i) = v;
			}
		}

		void generateBuildingMap() {
			m_buildingMap.resize(m_mapSize * m_mapSize, {});
			for (auto& v : m_buildings)
			{
				uint32_t i = v.m_x + v.m_y * m_mapSize;
				m_buildingMap.at(i) = v;
			}
		}


		/* Utils */
		bool isInMap(const uint16_t x, const uint16_t y)
		{
			return x < m_mapSize&& y < m_mapSize;
		}
		bool assertIsInMap(const uint16_t x, const uint16_t y)
		{
			if (isInMap(x, y) == false)
			{
				throw std::runtime_error("Tried indexing outside of map range");
			}
		}
		const SGround& getGroundAt(const uint16_t x, const uint16_t y) {
			assertIsInMap(x, y);
			return m_ground.at(y * m_mapSize + x);
		}
		const SGroundData& getGroundDataAt(const uint16_t x, const uint16_t y) {
			assertIsInMap(x, y);
			return m_groundData.at(y * m_mapSize + x);
		}
		const std::optional<S4MapFile::Segments::Data::Static::SBuilding>& getBuildingAt(const uint16_t x, const uint16_t y)
		{
			assertIsInMap(x, y);
			return m_buildingMap.at(x + y * m_mapSize);
		}
		const std::optional<S4MapFile::Segments::Data::Static::SSettler>& getSettlerAt(const uint16_t x, const uint16_t y)
		{
			assertIsInMap(x, y);
			return m_settlerMap.at(x + y * m_mapSize);
		}
		const std::optional<S4MapFile::Segments::Data::Static::SStack>& getStackAt(const uint16_t x, const uint16_t y)
		{
			assertIsInMap(x, y);
			return m_stackMap.at(x + y * m_mapSize);
		}
	};
}