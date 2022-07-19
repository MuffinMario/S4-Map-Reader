#pragma once
#include "types.h"
#include <algorithm>
namespace S4MAP::Compress::Inflate {

	constexpr const size_t CODEWORD_TABLE_SIZE = 274;
	class CCodewordTable {
		StaticArray<uint16_t, CODEWORD_TABLE_SIZE> m_aCodewordTable;
		StaticArray<uint16_t, CODEWORD_TABLE_SIZE> m_aCodewordIndexLUT;
		StaticArray<uint32_t, CODEWORD_TABLE_SIZE> m_aCodewordQuantityTable;

		void initSymbolTable() {
			// 0x100 - 0x10F
			std::generate(m_aCodewordTable.begin(), m_aCodewordTable.begin() + 16, [n = 0]()mutable{return 0x100 + n++; });

			m_aCodewordTable[16] = 0x0;
			m_aCodewordTable[17] = 0x20;
			m_aCodewordTable[18] = 0x30;
			m_aCodewordTable[19] = 0xFF;


			auto preLoopEnd = m_aCodewordTable.cbegin() + 20;
			size_t currentOffset = 20;
			for (uint16_t val = 1; val <= CODEWORD_TABLE_SIZE - 1; val++)
			{
				if (std::find(m_aCodewordTable.cbegin(), preLoopEnd, val) == preLoopEnd)
				{
					// val not found in symbol table "push" to tbl
					m_aCodewordTable[currentOffset++] = val;
				}
			}

			initIndexLUT();
		}
		void initIndexLUT() {
			for (size_t i = 0; i < CODEWORD_TABLE_SIZE; i++)
			{
				m_aCodewordIndexLUT[m_aCodewordTable[i]] = static_cast<uint16_t>(i);
			}
		}
	public:
		CCodewordTable() :m_aCodewordQuantityTable{ 0 } {
			initSymbolTable();
		}

		void incrementSymbolQuantity(uint16_t symbol)
		{
			m_aCodewordQuantityTable[symbol]++;
		}
		uint16_t getIndex(uint16_t symbol) { return m_aCodewordIndexLUT[symbol]; }

		uint16_t getCodeword(uint16_t index) { return m_aCodewordTable[index]; }

		/* Rearrange table by quantity*/
		void rearrange() {
			class StableCompare {
				const StaticArray<uint32_t, CODEWORD_TABLE_SIZE>& m_quantTbl;
			public:
				StableCompare(const StaticArray<uint32_t, CODEWORD_TABLE_SIZE>& quantityTable) : m_quantTbl(quantityTable) {}
				bool operator() (const uint16_t& a, const uint16_t& b)
				{
					/* The stable sorting way in s4 is based on value, not index of table */
					auto comp = [this](uint16_t cw) { uint32_t cmp = (m_quantTbl[cw] << 16) + cw; return cmp; };
					return comp(a) > comp(b);
				}
			};
			std::stable_sort(m_aCodewordTable.begin(), m_aCodewordTable.end(), StableCompare(m_aCodewordQuantityTable));

			std::for_each(m_aCodewordQuantityTable.begin(), m_aCodewordQuantityTable.end(), [](uint32_t& v) {v /= 2; });

			initIndexLUT();
		}
	};
}