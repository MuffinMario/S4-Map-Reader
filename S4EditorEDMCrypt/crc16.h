#pragma once
// table taken from https://github.com/westerndigitalcorporation/fio/blob/master/crc
#include "types.h"
namespace S4::Util {
	class CRC16 {
	public:
		static uint16_t processByte(const uint16_t prevCRC, const uint8_t data);
		static uint16_t processBuffer(const SequenceByteContainer& buffer);
		static uint16_t processBuffer(const SequenceByteContainer& buffer, size_t offset);
		static uint16_t processBuffer(const SequenceByteContainer& buffer, size_t offset, size_t length);
	};
}
