#pragma once

#include "S4MapSegmentHeader.h"
#include "S4MapSegment.h"
#include "S4Map.h"
#include "SegmentTypesEnumNames.h"
#include "StaticSegment.h"
#include "util.h"
#include "crc16.h"
#include <iostream>
#include <functional>
#include <filesystem>
#include <optional>
#include "S4SegmentHeaderCrypt.h"
#include "S4Inflate.h"

using namespace S4MAP::S4MapFile::Segments;
static_assert(sizeof(S4MAP::S4MapFile::Segments::Header::SS4MapSegmentHeader) == 0x18, "Segment Header is not 0x18 Bytes");

namespace S4MAP::S4MapFile {
	using namespace std::string_literals;

	constexpr const size_t SEGMENT_HEADER_SIZE = 0x18;

	class CS4MapFile {
	public:
		enum class EMapType : uint32_t {
			MAPTYPE_MAP = 40,
			MAPTYPE_EDM = 31
		};
	private:
		size_t m_fileSize = 0;
		uint32_t m_fileChecksum = 0;
		SequenceContainer<Segments::SS4MapSegment> m_rawSegments;
		String m_mapFileName;
		CS4Map m_s4map;
		EMapType m_mapType;


		static void printHeader(const Header::SS4MapSegmentHeader& header) {
			auto segmentType = BitUtils::LOWORD(static_cast<uint32_t>(header.m_segmentType));
			auto hiwordSegmentType = BitUtils::HIWORD(static_cast<uint32_t>(header.m_segmentType));

			if (Segments::SEGMENT_HEADER_IDENTIFIERS.find(static_cast<EStaticSegmentIDs>(segmentType)) != SEGMENT_HEADER_IDENTIFIERS.end())
				std::cout << std::hex << "0x" << segmentType << " - " << SEGMENT_HEADER_IDENTIFIERS.at(static_cast<EStaticSegmentIDs>(segmentType)) << "(high=" << hiwordSegmentType << ")" << std::endl;
			else
				std::cout << std::hex << "0x" << segmentType << "(high=" << hiwordSegmentType << ")" << std::endl;
			std::cout << "\tm_fileSegmentSize: " << header.m_fileSegmentSize << std::endl;
			std::cout << "\tm_mapSizeOrDataIDK: " << header.m_mapSize << std::endl;
			std::cout << "\tm_uncompressedSize: " << header.m_uncompressedSize << std::endl;
			std::cout << "\tm_segmentChecksum: " << header.m_segmentChecksum << std::endl;
			std::cout << "\tm_unused: " << header.m_unused << std::endl;
		}

		/* inverse byteorder of crc16 */
		static uint16_t getCRC16(const SequenceByteContainer& content, uint32_t offset, uint32_t size) {
			uint32_t crccalc = 0 | S4::Util::CRC16::processBuffer(content, offset, size);
			// s4 swaps checksums byte order then saves in little endian its [b2] [b1] [b3] [b4] (b3, b4 are obv 0 because its crc16)
			uint32_t crc_inv_byteorder = (crccalc & 0xFF) << 8 | (crccalc >> 8);
			return crc_inv_byteorder;
		}
	public:


		const CS4Map& getMap() const { return m_s4map; }
		const SequenceContainer<SS4MapSegment>& getSegments() const { return m_rawSegments; };
		std::optional<std::reference_wrapper<const SS4MapSegment>> getSegment(EStaticSegmentIDs segmentID) const {
			if (auto it = std::find_if(m_rawSegments.cbegin(), m_rawSegments.cend(), [segmentID](const SS4MapSegment& segment) {return segment.m_segmentHeader.m_segmentType == segmentID; }); it != m_rawSegments.end()) {
				return std::optional<std::reference_wrapper<const SS4MapSegment>>{*it};
			}
			return std::nullopt;
		}
		const size_t getFileSize() const { return m_fileSize; };
		const uint32_t getChecksum() const { return m_fileChecksum; };
		const String getFilename() const { return m_mapFileName; };
		const EMapType getMapType() const { return m_mapType; }

		static CS4MapFile openMapFile(const std::filesystem::path& path, bool printLog = false) {
			const auto fileContent = File::readFile(path.string());
			constexpr const uint32_t CHECKSUM_SIZE = 4, MAPTYPE_SIZE = 4;
			CS4Map map;
			CS4MapFile mapfile;

			/*
				File Size for header Check
			*/
			if (fileContent.size() < (CHECKSUM_SIZE + MAPTYPE_SIZE + SEGMENT_HEADER_SIZE))
				throw std::runtime_error("File Size too small to be map");

			const uint32_t
				crc16 = *reinterpret_cast<const uint32_t*>(fileContent.data()),
				mapType = *reinterpret_cast<const uint32_t*>(fileContent.data() + 4);


			/*
				Map Type Check
			*/
			if (mapType != static_cast<uint32_t>(EMapType::MAPTYPE_EDM) && mapType != static_cast<uint32_t>(EMapType::MAPTYPE_MAP))
				throw std::runtime_error("File is not EDM/MAP");

			/*
				Checksum check
			*/
			uint32_t calculatedContentCRC16 = getCRC16(fileContent, 8, fileContent.size() - 8);
			if (calculatedContentCRC16 != crc16)
				throw std::runtime_error("Incorrect checksum of map");


			/*
				Read segments
			*/
			uint32_t offset = CHECKSUM_SIZE + MAPTYPE_SIZE;
			while (offset < fileContent.size())
			{
				Header::CSegmentHeaderCrypt c;
				c.setupS4IV();
				auto decrypted = c.cryptHeaderBytes(fileContent.cbegin() + offset, fileContent.cbegin() + offset + SEGMENT_HEADER_SIZE);
				if (printLog)
				{
					printHeader(decrypted);
				}

				offset += SEGMENT_HEADER_SIZE;

				/*
					Verify checksum
				*/
				uint32_t segmentDataChecksum = getCRC16(fileContent, offset, decrypted.m_fileSegmentSize);
				if (segmentDataChecksum != decrypted.m_segmentChecksum)
					throw std::runtime_error("Incorrect Checksum in Data Segment ID "s + std::to_string(static_cast<uint32_t>(decrypted.m_segmentType)));

				/*
					Decompress
				*/

				SequenceByteContainer ee(fileContent.cbegin() + offset, fileContent.cbegin() + offset + decrypted.m_fileSegmentSize);
				S4MAP::Compress::CS4Inflate inflate(ee, decrypted.m_uncompressedSize);
				std::cout << "inflating " << (int)decrypted.m_segmentType << std::endl;
				auto& inflatedContent = inflate.inflate();

				/* Assign decompressed content */
				EStaticSegmentIDs segmentID = decrypted.m_segmentType;
				uint16_t segmentIDHiWord = decrypted.m_hiwordSegmentType;
				using Segments::SStaticSegment;
				using namespace Data;
				if (segmentID == EStaticSegmentIDs::HEADER_SIGNAL_END) {
					SStaticSegment::deserializeSegment(map.m_segment0x0_dummy, inflatedContent);
				}
				else if (segmentID == EStaticSegmentIDs::MAP_INFO) {
					SStaticSegment::deserializeSegment(map.m_mapData, inflatedContent);
				}
				else if (segmentID == EStaticSegmentIDs::PLAYER_INFO) {
					SStaticSegment::deserializeSegment(map.m_playerData, inflatedContent);
				}
				else if (segmentID == EStaticSegmentIDs::TEAM_INFO) {
					if (map.m_teamConstellations.find(segmentIDHiWord) != map.m_teamConstellations.end())
						throw std::runtime_error("Team Constellation with id "s + std::to_string(segmentIDHiWord) + " already exists."s);
					if (segmentIDHiWord > 9)
						throw std::runtime_error("Team Constellation with id > 9 should not exist");

					SStaticSegment::deserializeSegment(map.m_teamConstellations[segmentIDHiWord], inflatedContent);
				}
				else if (segmentID == EStaticSegmentIDs::MAP_PREVIEW_IMAGE) {
					map.m_previewImage.loadImage(inflatedContent);
				}
				else if (segmentID == EStaticSegmentIDs::RANDOM_UNUSED_MAYBE) {
					/* UNUSED */
				}
				else if (segmentID == EStaticSegmentIDs::GROUND_DATA) {
					//assign map size for everything else
					map.m_mapSize = decrypted.m_mapSize;

					SStaticSegment::deserializeSegment(map.m_groundData, inflatedContent);
				}
				else if (segmentID == EStaticSegmentIDs::SETTLERS) {
					SStaticSegment::deserializeSegment(map.m_settlers, inflatedContent);
				}
				else if (segmentID == EStaticSegmentIDs::BUILDINGS) {
					SStaticSegment::deserializeSegment(map.m_buildings, inflatedContent);
				}
				else if (segmentID == EStaticSegmentIDs::STACKS) {
					SStaticSegment::deserializeSegment(map.m_stacks, inflatedContent);
				}
				else if (segmentID == EStaticSegmentIDs::VICTORY_CONDITIONS) {
					SStaticSegment::deserializeSegment(map.m_victoryConditions, inflatedContent);
				}
				else if (segmentID == EStaticSegmentIDs::LOCAL_DESCRIPTION) {
					map.m_localDescription.assign(inflatedContent.begin(), inflatedContent.end());
				}
				else if (segmentID == EStaticSegmentIDs::LOCAL_TIPS) {
					map.m_localTips.assign(inflatedContent.begin(), inflatedContent.end());
				}
				else if (segmentID == EStaticSegmentIDs::GROUND) {
					SStaticSegment::deserializeSegment(map.m_ground, inflatedContent);
				}
				else if (segmentID == EStaticSegmentIDs::ENGLISH_DESCRIPTION) {
					map.m_englishDescription.assign(inflatedContent.begin(), inflatedContent.end());
				}
				else if (segmentID == EStaticSegmentIDs::ENGLISH_TIPS) {
					map.m_englishTips.assign(inflatedContent.begin(), inflatedContent.end());
				}
				else if (segmentID == EStaticSegmentIDs::LUA_SCRIPT) {
					map.m_luaScript.assign(inflatedContent.begin(), inflatedContent.end());
				}
				else if (segmentID == EStaticSegmentIDs::HEADER_SIGNAL_EDM) {
					SStaticSegment::deserializeSegment(map.m_segment0x40_dummy, inflatedContent);
				}
				else if (segmentID == EStaticSegmentIDs::EDITOR_INFO) {
					SStaticSegment::deserializeSegment(map.m_editorData, inflatedContent);
				}
				else { // random buf

				}

				mapfile.m_rawSegments.push_back(SS4MapSegment{ decrypted,inflatedContent,ee });



				if (decrypted.m_segmentType <= EStaticSegmentIDs::EDITOR_INFO)
					File::writeFile(""s + std::to_string(static_cast<uint32_t>(decrypted.m_segmentType)) + ".deflated_mapsgmt", inflatedContent);
				offset += decrypted.m_fileSegmentSize;

				if (decrypted.m_segmentType == EStaticSegmentIDs::HEADER_SIGNAL_END)
					break;
			}
			/*
				Padding confirmation
			*/
			auto lengthLeft = fileContent.size() - offset;
			// assert max size
			if (lengthLeft > 3)
			{
				throw std::runtime_error("There are more than 3 Bytes left to read. Assume incorrect map format");
			}
			else if (lengthLeft > 0) // has 4*n ensuring padding	
			{
				// the padding bytes are just filecontent size mod 4 in the last 4-(filecontentsize mod 4) bytes
				uint32_t paddingValue = 4 - lengthLeft; // expected padding value
				uint32_t lastBytesValue = 0;
				for (int i = 0; i < lengthLeft; i++)
				{
					lastBytesValue |= fileContent.at(offset++) << (i << 3);
				}
				if (paddingValue != lastBytesValue)
				{
					throw std::runtime_error("Padding bytes value does not equal assumed bytes");
				}
			}

			/*
				assign mapfile with the remaining stuff
			*/
			map.generateBuildingMap();
			map.generateSettlerMap();
			map.generateStackMap();
			mapfile.m_mapType = static_cast<EMapType>(mapType);
			mapfile.m_mapFileName = path.filename().filename().string();
			mapfile.m_fileChecksum = calculatedContentCRC16;
			mapfile.m_fileSize = fileContent.size();
			mapfile.m_s4map = std::move(map);
			return mapfile;
		}
	};
}