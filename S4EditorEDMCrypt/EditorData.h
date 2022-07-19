#pragma once
#include "SegmentEnums.h"

namespace S4MAP::S4MapFile::Segments::Data::Static {
	struct SEditorData {
		// 0
		// modern editor always 0x10 (old versions 0x9)
		uint16_t m_editorVersion;
		// modern editor always 0xb800
		uint16_t m_unk2;
		// 1
		// modern editor always 
		uint16_t m_unk4;
		// modern editor always 0x04
		uint16_t m_unk6;
		// 2
		uint32_t m_unusedInModern1;
		// 3
		uint32_t m_unusedInModern2;
		// 4
		uint32_t m_unusedInModern3;
	};
}