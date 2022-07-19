#include "types.h"
#include "util.h"
#include "crc16.h"
#include "S4MapFile.h"
#include "SegmentEnumNames.h"
#include "SegmentTypesEnum.h"
#include "GroundEnums.h"

#include <png++/png.hpp>
#include <filesystem>
#include <functional>
#include <map>
#include <optional>
#include <iostream>
#include <bitset>


using std::string_literals::operator ""s;
namespace S4MAP {

	void prt(const SequenceByteContainer& ct) {
		for (auto& e : ct)
		{
				std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)e << "\t";
		}
		std::cout << " (size: " << ct.size() << ")\n";
	}
	template<typename ItBegin,typename ItEnd>
	void prt(const ItBegin begin,const ItEnd end) {
		for (auto b = begin; b != end; b++)
		{
			std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)*b << "\t";
		}
		std::cout << " (size: " << (end-begin) << ")\n";
	}
	void prtBits(const SequenceByteContainer& ct) {
		for (auto& e : ct)
		{
			std::bitset<8> bits(e);
			std::cout << std::hex << std::setfill('0') << std::setw(2) << bits << "\t";
		}
		std::cout << " (size: " << ct.size() << ")\n";
	}

}
auto printMapData(const S4MAP::S4MapFile::CS4MapFile& mapfile)
{
	auto mapdata = mapfile.getMap().getMapData();
	std::bitset<32> flags(mapdata.m_mapFlags);
	std::cout << "GM=" << mapdata.m_gameMode << "\tSize="
		<< mapdata.m_mapSize << "\tPlayers=" << mapdata.m_playerCount << "\tResource=" << static_cast<uint32_t>(mapdata.m_startResource) << "\tunk5=" << mapdata.m_unusedIsZero << "\tm_mapFlags=" << flags << "\n";
	return mapdata;
}
auto printPlayerData(const S4MAP::S4MapFile::CS4MapFile& mapfile)
{
	auto pd = mapfile.getMap().getPlayerData();
	int i = 1;
	for (auto& pde : pd) {
		std::cout << "Player " << i++ << std::endl
			<< "Unk0=" << static_cast<uint32_t>(pde.m_tribe) << "\tStartPos=" << pde.m_startPosX << "/" << pde.m_startPosY << "\tName=" << pde.m_playerName.data() << "\tNullterm=" << (int)pde.m_alwaysNullTerminator << std::endl;
	}
	return pd;
}
auto printFilename(const S4MAP::S4MapFile::CS4MapFile& mapfile) {
	std::cout << "=========================\n==== " << mapfile.getFilename() << "\n=========================\n";
	return mapfile.getFilename();
}
auto printTeamDatas(const S4MAP::S4MapFile::CS4MapFile& mapfile)
{
	auto teamdata = mapfile.getMap().getTeamData();
	for (auto& x : teamdata)
	{
		StringStream ss;
		for (auto v : x.second.m_teamPlayerData)
		{
			ss << "{ team=" << (int)static_cast<uint8_t>(v.m_teamID) << ", player=" << (int)static_cast<uint8_t>(v.m_playerType) << " } ";
		}
		std::cout << x.first << std::endl
			<< x.second.m_constellationName.data() << " " << (int)x.second.m_alwaysNullTerminator << "\t" << ss.str() << std::endl;
	}
	return teamdata;
}
auto printEditorData(const S4MAP::S4MapFile::CS4MapFile& mapfile)
{
	auto map = mapfile.getMap();
	auto editordata = map.getEditorData();
	std::cout << "Version: " << editordata.m_editorVersion << "\n"
		<< "unk2: " << editordata.m_unk2 << "\n"
		<< "unk4: " << editordata.m_unk4 << "\n"
		<< "unk6: " << editordata.m_unk6 << "\n"
		<< "unused1: " << editordata.m_unusedInModern1 << "\n"
		<< "unused2: " << editordata.m_unusedInModern2 << "\n"
		<< "unused3: " << editordata.m_unusedInModern3 << "\n";
}
auto printBuildings(const S4MAP::S4MapFile::CS4MapFile& mapfile)
{
	auto map = mapfile.getMap();
	auto builds = map.getBuildings();

	for (auto& b : builds)
		std::cout << S4MAP::S4MapFile::Segments::Data::Static::BUILDING_NAMES.at(b.m_buildingType) << " at " << b.m_x << "/" << b.m_y << std::endl;

	return builds;
}
template<typename F>
void printDebugHeatmapPNG(const String& filename,size_t mapsize, F colorFunc)
{
	std::filesystem::create_directory("../heatmaps/");

	png::image<png::rgb_pixel> img(mapsize*1.5, mapsize);
	for (uint32_t y = 0; y < mapsize; y++)
	{
		for (uint32_t x = 0; x < mapsize; x++)
		{
			uint32_t xScew = x + img.get_width() - mapsize - y / 2;

			img[y][xScew] = colorFunc(x, y);
		}
	}
	img.write(("../heatmaps/"s + filename).c_str());
}

void currentDebugPrint(const S4MAP::S4MapFile::CS4MapFile& mapfile)
{
	using S4MAP::S4MapFile::Segments::EStaticSegmentIDs;
	using S4MAP::EGroundType;

	auto map = mapfile.getMap();
	auto mapdata = map.getMapData();
	auto playerdata = map.getPlayerData();
	auto teamdata = map.getTeamData();
	auto editordata = map.getEditorData();
	auto mapdataseg = mapfile.getSegment(EStaticSegmentIDs::MAP_INFO);
	auto playerdataseg = mapfile.getSegment(EStaticSegmentIDs::PLAYER_INFO);
	auto teamdataseg = mapfile.getSegment(EStaticSegmentIDs::TEAM_INFO);
	auto editordataseg = mapfile.getSegment(EStaticSegmentIDs::EDITOR_INFO);


	/*auto fn = printFilename(mapfile);
	auto buildings = printBuildings(mapfile);*/
	auto offsetPixelColorFunc = [&map](uint32_t offset) { return [&map,offset](const uint32_t x, const uint32_t y) {
		png::rgb_pixel pxl;
		auto data = map.getGroundDataAt(x, y);
		auto ground = map.getGroundAt(x, y);
		auto full = [](const uint8_t flag, uint8_t offset) { return ((flag >> (offset - 1)) & 1) * 255; };
		auto iswater = ground.m_groundType <= EGroundType::WATER_BEACH;

		pxl.blue =	full(data.m_occupancyFlags, offset) | 0xF;
		pxl.red =	full(data.m_occupancyFlags, offset);
		pxl.green = full(data.m_occupancyFlags, offset) | (iswater == false ? 0xF : 0);

		if (map.getBuildingAt(x, y))
		{
			pxl.blue = 0;
			pxl.green = 0;
			pxl.red = 0xFF;
		}

		return pxl; }; };

	auto name = printFilename(mapfile);

	printDebugHeatmapPNG((mapfile.getFilename() + "_heatmap8thBit.png"s).c_str(), map.getMapSize(), offsetPixelColorFunc(8));
	printDebugHeatmapPNG((mapfile.getFilename() + "_heatmap7thBit.png"s).c_str(), map.getMapSize(), offsetPixelColorFunc(7));
	printDebugHeatmapPNG((mapfile.getFilename() + "_heatmap6thBit.png"s).c_str(), map.getMapSize(), offsetPixelColorFunc(6));
	printDebugHeatmapPNG((mapfile.getFilename() + "_heatmap5thBit.png"s).c_str(), map.getMapSize(), offsetPixelColorFunc(5));
	printDebugHeatmapPNG((mapfile.getFilename() + "_heatmap4thBit.png"s).c_str(), map.getMapSize(), offsetPixelColorFunc(4));
	printDebugHeatmapPNG((mapfile.getFilename() + "_heatmap3rdBit.png"s).c_str(), map.getMapSize(), offsetPixelColorFunc(3));
	printDebugHeatmapPNG((mapfile.getFilename() + "_heatmap2ndBit.png"s).c_str(), map.getMapSize(), offsetPixelColorFunc(2));
	printDebugHeatmapPNG((mapfile.getFilename() + "_heatmap1stBit.png"s).c_str(), map.getMapSize(), offsetPixelColorFunc(1));
}

const SequenceContainer<String> testSubjects = {
	R"(D:\Ubisoft Games\thesettlers4\Map\User\Meeresblockade.map)",
	R"(D:\Ubisoft Games\thesettlers4\Map\User\AllBuildings.map)",
	R"(D:\Ubisoft Games\thesettlers4\Map\User\AllBuildings_no2ndbit.map)",
};


constexpr bool iterateAllFiles = false;
int main() {
	if constexpr (iterateAllFiles)
	{
		for (const auto& file : std::filesystem::recursive_directory_iterator(R"(D:\Ubisoft Games\thesettlers4\Map\Campaign)"))
		{
			if (!file.is_regular_file()
				//|| file.path().extension() != ".edm"
				)
				continue;
			try {
				auto mapfile = S4MAP::S4MapFile::CS4MapFile::openMapFile(file.path(), false);
				currentDebugPrint(mapfile);
			}
			catch (const std::runtime_error& re) {
				std::cout << "Could not open map " << file << " with error: " << re.what() << std::endl;
			}
		}
	}

	std::cout << "Complete\n";
	for (auto& subj : testSubjects)
	{
		try {
		auto mapfile = S4MAP::S4MapFile::CS4MapFile::openMapFile(subj, false);
		currentDebugPrint(mapfile);
		}
		catch (const std::runtime_error& re) {
			std::cout << "Could not open map " << subj << " with error: " << re.what() << std::endl;
		}
	}

}