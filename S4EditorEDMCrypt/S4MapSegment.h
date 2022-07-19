#pragma once
#include "types.h"
#include "S4MapSegmentHeader.h"
namespace S4MAP::S4MapFile::Segments {
	struct SS4MapSegment {
		Header::SS4MapSegmentHeader m_segmentHeader;
		SequenceByteContainer m_byteContent;
		SequenceByteContainer m_compressedContent;
	};
}