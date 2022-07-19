#pragma once
#include "SegmentTypesEnum.h"
namespace S4MAP::S4MapFile::Segments::Header {
	struct SS4MapSegmentHeader {
		EStaticSegmentIDs m_segmentType;
		uint16_t m_hiwordSegmentType;
		uint32_t m_fileSegmentSize;
		uint32_t m_uncompressedSize;
		uint32_t m_segmentChecksum;
		uint32_t m_unused;
		uint32_t m_mapSize;
	};
}