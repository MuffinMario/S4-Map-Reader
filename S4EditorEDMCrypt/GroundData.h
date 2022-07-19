#pragma once
#include "types.h"
#include "GroundEnums.h"
namespace S4MAP {
	struct SResource {
		uint8_t m_resourceData;

		EResourceType getResourceType() {
			return static_cast<EResourceType>((m_resourceData & 0xF0) >> 4);
		}
		uint8_t getResourceCount() {
			return m_resourceData & 0xF;
		}
		void setResourceType(EResourceType type) {
			m_resourceData = (m_resourceData & 0x0F) | static_cast<uint8_t>(type) & 0xF0;
		}
		void setResourceCount(uint8_t count) {
			m_resourceData = (m_resourceData & 0xF0) | count & 0x0F;
		}
	};
	struct SGroundData {
		// [0x0] the object ID at this location
		EObjectType m_objectID;
		// [0x1] whose party this coordinate belongs to (0 = none) (>8 crash)
		uint8_t m_landOwnership;
		// [0x2] 
		//	1 - unwalkable ?
		//  1 << 1 - building walking paths / water obstacles ???
		//  1 << 2 - unused
		//  1 << 3 - building hitbox
		//  1 << 4 - unused
		//  1 << 5 - unused
		//  1 << 6 - land object hitbox
		//  1 << 7 - unused
		uint8_t m_occupancyFlags;
		// [0x3] resources
		SResource m_resource;
	};
}