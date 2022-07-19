#pragma once
#include "types.h"
#include "util.h"
#include "CodewordTable.h"
using namespace std::string_literals;
namespace S4MAP::Compress {
	/* <ident,bits offset to add to ident> */
	using S4IdentOffsetPair = std::pair<uint16_t, uint16_t>;

	const StaticArray<S4IdentOffsetPair, 0x10> INITIAL_HUFFMAN_TABLE{ {
		{0x00,0x2}, // 0 - 3 ( 0x0 + 2^0x2 - 1 )
		{0x04,0x3}, // 4 - 11 (0x4 + 2^0x3 - 1)
		{0x0C,0x3}, // 12 - 19
		{0x14,0x4}, // 20 - 27
		{0x24,0x4}, // ...
		{0x34,0x4},
		{0x44,0x4},
		{0x54,0x4},
		{0x64,0x4},
		{0x74,0x4},
		{0x84,0x4},
		{0x94,0x4},
		{0xA4,0x4},
		{0xB4,0x5},
		{0xD4,0x5},
		{0xF4,0x5},
	} };

	/* Additional distance values lookup */
	const StaticArray<S4IdentOffsetPair, 0x08> LZ_DISTANCE_TABLE{ {
		{0x00,0x0}, // 0
		{0x01,0x0}, // 1
		{0x02,0x1}, // 2 - 3
		{0x04,0x2}, // 4 - 7 
		{0x08,0x3},
		{0x10,0x4},
		{0x20,0x5},
		{0x40,0x6} // 0x40 - 0x80
	} };

	const StaticArray<S4IdentOffsetPair, 0x08> LZ_LENGTH_TABLE{ {
		{0x008,0x1},
		{0x00A,0x2},
		{0x00E,0x3},
		{0x016,0x4},
		{0x026,0x5},
		{0x046,0x6},
		{0x086,0x7},
		{0x106,0x8}
	} };




	class CS4Inflate {
		const SequenceByteContainer& m_refDeflatedData;
		SequenceByteContainer m_inflatedData;

		/* read bits of buffer, up to 32. I assume there are bit serializers that work more flexible though */
		uint32_t readToDword(uint64_t startBitIndex, uint8_t bitSize)
		{
			return readToDword(m_refDeflatedData, startBitIndex, bitSize);
		}
		static uint32_t readToDword(const SequenceByteContainer& container, uint64_t startBitIndex, uint8_t bitSize)
		{
			if (bitSize > 32)
				throw std::runtime_error("readToDword() only accepts up to 32 bits to read");
			if (startBitIndex / 8 > std::numeric_limits<size_t>::max())
				throw std::runtime_error("readToDword() startBitIndex is above maximum container range");
			uint64_t res = 0;

			//initial index
			size_t index = startBitIndex / 8;
			// initial offset
			size_t offs = startBitIndex % 8;

			size_t bytestoread = (offs + bitSize - 1) / 8 + 1;

			if (static_cast<uint64_t>(index) + bytestoread - 1 >= container.size())
				throw std::runtime_error("readToDword() trying to read beyond buffer");

			for (uint32_t i = 0; i < bytestoread; i++)
			{
				res |= static_cast<uint64_t>(container.at(index + i)) << ((bytestoread - 1 - i) * 8);
			}

			return BitUtils::readBitsUp32(res, (8 - ((startBitIndex + bitSize) % 8)) % 8, bitSize);
		}
		bool m_inflated = false;
	public:
		CS4Inflate(const SequenceByteContainer& compressedData, size_t decompressSize) : m_refDeflatedData(compressedData)
		{
			m_inflatedData.resize(decompressSize);
			m_inflatedData.assign(decompressSize, 0);
		}

		SequenceByteContainer& inflate() {

			auto huffmanTable = INITIAL_HUFFMAN_TABLE;
			Inflate::CCodewordTable codewordTable;

			uint64_t bitOffset = 0;
			size_t inflateContainerOffset = 0;
			// lambda to increment bitoffset after reading
			auto readBits = [this,&bitOffset](uint8_t bitSize) {
				auto r = readToDword(bitOffset, bitSize);
				bitOffset += bitSize;
				return r;
			};
			while(!m_inflated &&
				bitOffset/8 < m_refDeflatedData.size())
			{

				auto code = readBits(4);
				auto offsetLength = huffmanTable[code].second;
				//std::cout << "IN HUFFCODE: " << std::dec << code << " (bits left: " << (m_refDeflatedData.size()*8 - bitOffset) << ") ";
				auto symbolIndex = huffmanTable[code].first;

				/* more to read*/
				if (offsetLength)
				{
					symbolIndex += readBits(offsetLength);

					if (symbolIndex >= 274)
						throw std::runtime_error("symbol index is out of range of huffman table");
				}

				auto codeword = codewordTable.getCodeword(symbolIndex);
				//std::cout << " (codeword: " << codeword;
				//if (codeword < 256) std::cout << " - " << (char)codeword;
				//std::cout << ")\n";

				codewordTable.incrementSymbolQuantity(codeword);

				// normal byte
				if (codeword < 0x100)
				{
					//if (pDeflateOriginalComparison)
					//{
					//	if (pDeflateOriginalComparison->at(decompressContainerOffset) != codeword)
					//	{
					//		StringStream ss;
					//		ss << "Original Byte at offset 0x" << std::hex << (decompressContainerOffset) << " should be 0x" << std::hex << std::setfill('0') << std::setw(2) <<
					//			((int)pDeflateOriginalComparison->at(decompressContainerOffset)) << " but is 0x" << std::hex << std::setfill('0') << std::setw(2) <<
					//			((int)codeword);
					//		auto strerr = ss.str();
					//	}
					//}
					m_inflatedData[inflateContainerOffset++] = codeword;
				}
				// signal to rearrange and create new lengths
				else if (codeword == 272)
				{
					codewordTable.rearrange();
					uint16_t length = 0;
					uint16_t base = 0;
					for (size_t i = 0; i < 16; i++)
					{
						length--;
						uint32_t readbit = 0;
						do {
							length++;
							readbit = readBits(1);
						} while (readbit == 0);
						huffmanTable[i].first = base;
						huffmanTable[i].second = length;

						base += (1 << length);
					}
				}
				else if (codeword == 273) // eof
				{
					if (m_refDeflatedData.size() - bitOffset / 8 > 2)
					{
						if (inflateContainerOffset >= m_inflatedData.size())
						{
							throw std::runtime_error("Reached expected decompressed data, yet there is remaining data");
							break;
						}
						// skip entire remaining bits of byte
						bitOffset = bitOffset + ((8 - bitOffset % 8) % 8);
					}
					else {
						if (inflateContainerOffset < m_inflatedData.size())
						{
							throw std::runtime_error("Did not reach decompressed end of file, but compressed data one");
						}
						m_inflated = true;
						break; // sometimes filler bits afterwards
					}
				}
				else { // non signal special symbols (length,distance)
					/*
						table is actaully
						(0,0)
						(1,0)
						(2,0)
						 ...
						(7,0)
						(8,1)	start LZ_LENGTH_TABLE
						(9,8)
						 ... 

						 but first 8 values always correspond to length
					*/
					uint16_t length = codeword - 256;
					if (codeword >= 264)
					{
						auto bitcount = LZ_LENGTH_TABLE[codeword - 264].second;
						auto bits = readBits(bitcount);
						length = LZ_LENGTH_TABLE[codeword - 264].first + bits;
					}
					// dont know why we need to write 4 more. 
					// maybe it's the hard set point minimum amount of redundancy to write a distance symbol
					length += 4;
					
					auto distanceIndex = readBits(3);

					auto distanceLen = LZ_DISTANCE_TABLE[distanceIndex].second + 1;
					auto baseVal = LZ_DISTANCE_TABLE[distanceIndex].first;

					auto base = readBits(8);
					
					auto offsetRead = readBits(distanceLen);
					auto offset = offsetRead;
					offset = ((base<< distanceLen) | offsetRead) + (baseVal << 9);


					if (inflateContainerOffset + length > m_inflatedData.size())
						throw std::runtime_error("cannot write data, remaining buffer not big enough");

					auto copyPos = inflateContainerOffset - offset; /*inflateContainerOffset - ((bv | copyOffset) + (baseVal << 9));*/
						
					auto copyPosBegin = copyPos;
					
					//std::cout << "Repeat (len=" << length << ", abs_offset=" << copyPos << ", rel_offset=" << offset << ")\n";
					for (uint32_t i = 0; i < length; i++)
					{
						uint8_t data = 0;
						if (copyPos >= inflateContainerOffset) 
						{
							//														negative offset
							//															|
							//														    v 
							//keep data at 0 (very rare case were we copy e.g. (abs_off=-2,len>2) => {0,0,inflated[0],inflated[1],...}
										//std::cerr <<
										//	//throw std::runtime_error(
										//	"Trying to copy a position in buffer that has not been written yet, trying to access index: "s
										//	+ std::to_string(copyPos) + " out of "s + std::to_string(inflateContainerOffset) + " with copylength of "s + std::to_string(length) + "\n"s;
										////)
										//	;
										//	system("PAUSE");
						}
						else {
							data = m_inflatedData[copyPos];
						}
						copyPos++;
						m_inflatedData[inflateContainerOffset++] = data;
					}

				}
			}


			if (m_inflated == false)
				throw std::runtime_error("Could not decompress. Unexpected end of data block");
			return m_inflatedData;

		}
		SequenceByteContainer& getInflatedContent() {
			if (m_inflated == false) throw std::runtime_error("Content not inflated yet");
			return m_inflatedData;
		}
		bool isInflated() { return m_inflated; }
	};
}