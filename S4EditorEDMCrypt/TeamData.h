#pragma once
#include "SegmentEnums.h"
#include "util.h"

#include <algorithm>

namespace S4MAP::S4MapFile::Segments::Data::Static {
	using namespace std::string_literals;
	struct STeamData {
		StaticCharArray<32> m_constellationName;
		uint8_t m_alwaysNullTerminator;

		struct STeamPlayerData {
			// 0-7
			ETeam m_teamID;
			EPlayerType m_playerType;
		};
		SequenceContainer<STeamPlayerData> m_teamPlayerData;


		/* TODO MOVE TODO MOVE TODO MOVE TODO MOVE TODO MOVE */
		template<typename ItFirst, typename ItLast>
		SequenceByteContainer serialize()
		{
			if (m_teamPlayerData.size() < 1 && m_teamPlayerData.size() > 8)
				throw std::runtime_error("To serialize STeamDataStruct m_teamPlayerData needs to have 1-8 players");

			SequenceByteContainer cont;
			cont.resize(sizeof(m_constellationName) + sizeof(m_alwaysNullTerminator) + sizeof(STeamPlayerData) * m_teamPlayerData.size());
			std::copy(m_constellationName.begin(), m_constellationName.end(), cont);

			// Iterate through remaining 2*n container bytes, 2*n = teamplayerdata.size()*sizeof(steamplayerdata)
			auto it = cont.begin() + sizeof(m_constellationName);
			*(it++) = m_alwaysNullTerminator;
			auto tpd_it = m_teamPlayerData.begin();
			while (it != cont.end())
			{
				*(it++) = tpd_it->m_teamID;
				*(it++) = tpd_it->m_playerType;
			}
			return cont;
		}
	};
}