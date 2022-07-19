#pragma once
#include "types.h"
#include "util.h"
namespace S4::Util {
	class CByteBufferReader {
		size_t m_byteCursor;
		const SequenceByteContainer& m_refByteContainer;
	public:

		CByteBufferReader(const SequenceByteContainer& refByteContainer) : m_byteCursor{ 0 }, m_refByteContainer{ refByteContainer } {

		}

		size_t getBufferSize() const { return m_refByteContainer.size(); }

		size_t getByteCursor() const { return m_byteCursor; }

		void seek(size_t beginOffset) { m_byteCursor = beginOffset; }

		bool eof() const { return m_byteCursor >= m_refByteContainer.size(); }

		size_t bytesLeft() const { return m_refByteContainer.size() - m_byteCursor; }

		bool hasBytesLeft(size_t bytes) const { return bytesLeft() >= bytes; }

		template<typename T>
		S4::Util::CByteBufferReader& operator >>(T& data)
		{
			auto readByteCount = sizeof(T);
			if (!hasBytesLeft(readByteCount))
				throw std::runtime_error("Cannot read bytes, not enough buffer left");

			if constexpr (std::is_enum_v<T>)
				data = static_cast<T>(BitUtils::read < std::underlying_type<T>::type > (m_refByteContainer, m_byteCursor));
			else if constexpr (std::is_fundamental_v<T>)
				data = BitUtils::read<T>(m_refByteContainer, m_byteCursor);
			else
				static_assert("Trying to read non-fundamental and non-enum type in CByteBufferReader operator >>");

			m_byteCursor += sizeof(T);
			return *this;
		}
	};
}