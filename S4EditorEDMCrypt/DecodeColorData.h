#pragma once
#include "types.h"
namespace S4::Util::Color {

	uint32_t Decode565(uint16_t color) {
		uint32_t r = (uint32_t)(((color >> 11) & 0x1F) * 527 + 23) >> 6;
		uint32_t g = (uint32_t)(((color >> 5) & 0x3F) * 259 + 33) >> 6;
		uint32_t b = (uint32_t)((color & 0x1F) * 527 + 23) >> 6;

		return ((r * 2) << 16) | ((g * 2) << 8) | (b & 0xFF);
	}
	uint16_t Encode565(uint32_t rgb888)
	{
		throw std::runtime_error("Encode565 unimplemented");
	}

}