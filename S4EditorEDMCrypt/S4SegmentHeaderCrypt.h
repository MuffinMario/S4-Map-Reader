#pragma once
#include "types.h"
#include "S4MapSegmentHeader.h"
#include <stdexcept>

namespace S4MAP::S4MapFile::Segments::Header {

	class CSegmentHeaderCrypt {
		//technically this class is
		//		initial IV string std::string [flags?,dataptr,size,capacity] (size=4dword) this is not even used beyond seting it up....
		//		crypt IV (size=3dword)
		//		some sort of LUT (size=9dword)
		//	=>sizeof struct = 16dword
		// but we don't care we do this the less ugly way (not using std::string operations to do byte manipulation..)
		// it is also far more complex than just setting IV to a string, but since this is how s4 implements this, we can simplify alot
		StaticArray<uint32_t, 16> pBuf;
		constexpr const static size_t IV_BEGIN_INDEX = 4;
		constexpr const static size_t IV_DWORD_COUNT = 3;
		const static inline String S4_IV_STRING = "01234567890123456789";

		void setupCustomIV(const String& str) {
			if (str.size() < IV_DWORD_COUNT * sizeof(uint32_t))
				throw std::runtime_error("IV for encryption needs to be 12 bytes or longer");

			for (int i = IV_BEGIN_INDEX; i < IV_BEGIN_INDEX + IV_DWORD_COUNT; i++)
			{
				// 0, 4, 8
				uint32_t dwordOffset = sizeof(uint32_t) * (i - IV_BEGIN_INDEX);

				uint32_t goalDWord = 0;
				for (int j = 0; j < sizeof(uint32_t); j++)
				{
					// set value of lowest byte, shift it 1 byte, for entire dword
					goalDWord <<= 8;
					goalDWord |= static_cast<uint8_t>(str.at(dwordOffset + j));
				}
				pBuf[i] = goalDWord;
			}
		}
	public:
		CSegmentHeaderCrypt() {
			pBuf[0] = 0; // this is technically HIBYTE(address of pBuf[0]) (unused)

			// std::string values for (unused)
			pBuf[1] = 0x00000000;
			pBuf[2] = 0x00000000;
			pBuf[3] = 0x00000000;

			pBuf[4] = 0x13579BDF;
			pBuf[5] = 0x2468ACE0;
			pBuf[6] = 0xFDB97531;

			pBuf[7] = 0x80000062;
			pBuf[8] = 0x40000020;
			pBuf[9] = 0x10000002;
			pBuf[10] = 0x7FFFFFFF;
			pBuf[11] = 0x3FFFFFFF;
			pBuf[12] = 0x0FFFFFFF;
			pBuf[13] = 0x80000000;
			pBuf[14] = 0xC0000000;
			pBuf[15] = 0xF0000000;
		}
		// redundant, but gives clearance on whats going on
		void setupS4IV() {
			setupCustomIV(S4_IV_STRING);
		}
		uint8_t cryptByte(uint8_t byte)
		{
			int fifthDwordLowBit; // ebx
			int sixthDwordLowBit; // edx
			int iters; // esi
			unsigned int fourthDword; // eax
			unsigned int v6; // edi
			unsigned int currentFifth; // eax
			unsigned int currentSixth; // eax
			char result; // al
			char prevRes; // [esp+Fh] [ebp-5h]
			char singleBitMask; // [esp+10h] [ebp-4h]
			uint8_t resByte = byte;

			fifthDwordLowBit = pBuf[5] & 1;
			prevRes = 0;
			singleBitMask = fifthDwordLowBit;
			sixthDwordLowBit = pBuf[6] & 1;

			iters = 8;
			while (1)
			{
				fourthDword = pBuf[4];
				if ((fourthDword & 1) != 0)
				{
					v6 = fourthDword ^ pBuf[7];
					currentFifth = pBuf[5];
					pBuf[4] = pBuf[13] | (v6 >> 1);
					if ((currentFifth & 1) != 0)
					{
						fifthDwordLowBit = 1;
						singleBitMask = 1;
						pBuf[5] = pBuf[14] | ((currentFifth ^ pBuf[8]) >> 1);
					}
					else
					{
						fifthDwordLowBit = 0;
						pBuf[5] = pBuf[11] & (currentFifth >> 1);
						singleBitMask = 0;
					}
				}
				else
				{
					pBuf[4] = pBuf[10] & (fourthDword >> 1);
					currentSixth = pBuf[6];
					if ((currentSixth & 1) != 0)
					{
						pBuf[6] = pBuf[15] | ((currentSixth ^ pBuf[9]) >> 1);
						sixthDwordLowBit = 1;
					}
					else
					{
						sixthDwordLowBit = 0;
						pBuf[6] = pBuf[12] & (currentSixth >> 1);
					}
				}
				result = (prevRes << 1) | fifthDwordLowBit ^ sixthDwordLowBit;

				--iters;
				prevRes = result;
				if (!iters)
					break;
				fifthDwordLowBit = fifthDwordLowBit & 0xFFFFFF00 | singleBitMask;
			}
			// byte ^= result
			resByte ^= result;
			return resByte;
		}
		template<typename First, typename Last>
		SS4MapSegmentHeader cryptHeaderBytes(First begin, Last end) {
			if (end - begin != 0x18)
				throw std::runtime_error("Header bytes length to iterate is not 0x18 bytes");

			SS4MapSegmentHeader head;

			// little endian all dwords of the 0x18 bytes
			auto initDWord = [this](First off) {
				uint32_t dword = 0;
				for (uint32_t i = 0; i < sizeof(uint32_t); i++)
				{
					dword |= cryptByte(*off) << (8 * (i));
					off++;
				}
				return dword;
			};
			auto initWord = [this](First off) {
				uint16_t word = 0;
				for (uint16_t i = 0; i < sizeof(uint16_t); i++)
				{
					word |= cryptByte(*off) << (8 * (i));
					off++;
				}
				return word;
			};

			head.m_segmentType = static_cast<EStaticSegmentIDs>(initWord(begin));
			head.m_hiwordSegmentType = static_cast<uint16_t>(initWord(begin + sizeof(uint16_t)));
			head.m_fileSegmentSize = initDWord(begin + sizeof(uint32_t) * 1);
			head.m_uncompressedSize = initDWord(begin + sizeof(uint32_t) * 2);
			head.m_segmentChecksum = initDWord(begin + sizeof(uint32_t) * 3);
			head.m_unused = initDWord(begin + sizeof(uint32_t) * 4);
			head.m_mapSize = initDWord(begin + sizeof(uint32_t) * 5);

			return head;
		}
	};
}