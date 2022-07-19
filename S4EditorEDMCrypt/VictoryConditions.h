#pragma once
#include "SegmentEnums.h"

namespace S4MAP::S4MapFile::Segments::Data::Static {
	struct SVictoryConditions {
		struct SDefeatPlayerVictoryCondition {
			struct SPlayerDefeatBool { constexpr static size_t MAX_CONDITIONS = 8; bool m_hasToDefeatPlayer; };
			bool m_enable;
			StaticArray<SPlayerDefeatBool, SPlayerDefeatBool::MAX_CONDITIONS> m_defeatPlayerBool;
		} m_defeatPlayerVictoryCondition;
		struct SDestroyBuildingVictoryCondition {
			struct SPlayerBuildingType { constexpr static size_t MAX_CONDITIONS = 10;  EPlayer m_fromPlayer; EBuildingType m_buildingToDestroy; };
			bool m_enable;
			StaticArray<SPlayerBuildingType, SPlayerBuildingType::MAX_CONDITIONS> m_conditions;
		} m_destroyBuildingVictoryCondition;
		struct SOwnLandsVictoryCondition {
			struct SOwnLandRequirement { constexpr static size_t MAX_CONDITIONS = 5; bool m_enable; uint16_t m_x; uint16_t m_y; };
			bool m_enable;
			StaticArray<SOwnLandRequirement, SOwnLandRequirement::MAX_CONDITIONS> m_conditions;
		} m_ownLandsVictoryCondition;
		struct STimeVictoryCondition {
			struct SPlayerSurviveTime { constexpr static size_t MAX_CONDITIONS = 8; uint16_t m_playerTimeHasToSurvive; };
			bool m_enable;
			StaticArray<SPlayerSurviveTime, SPlayerSurviveTime::MAX_CONDITIONS> m_conditions;
		} m_timeVictoryCondition;
		struct SProductGoodsVictoryCondition {
			struct SProduceRequirement { constexpr static size_t MAX_CONDITIONS = 3; uint16_t m_amount; EStackType m_stackType; };
			bool m_enable;
			StaticArray<SProduceRequirement, SProduceRequirement::MAX_CONDITIONS> m_produceGoods;
		} m_produceGoodsVictoryCondition;
	};
}