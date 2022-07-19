#pragma once
#include <zlib.h>
#include <array>
#include <algorithm>
#include "types.h"
#include <stdexcept>
namespace ContainerUtils {
    template<typename T>
    SequenceContainer<T> slice(const SequenceContainer<T>& container, uint32_t from, uint32_t to) {
        return SequenceContainer<T>(container.begin() + from, container.begin() + to + 1);
    }
    // removes 1 instance
    void removeFromUniqueSequenceContainer(SequenceContainer<String>& container, const String& v);
    // adds 1 instance if v not exists already
    void addToUniqueSequenceContainer(SequenceContainer<String>& container, const String& v);
    // adds 1 instance if v not exists already
    bool containsElementInUniqueSequenceContainer(const SequenceContainer<String>& container, const String& v);
}
namespace BitUtils {
    template<typename T>
    uint16_t HIWORD(const T& v)
    {
        constexpr auto size = sizeof(T);
        static_assert(size >= 2, "HIWORD() Requires Type with at least 2 Bytes");
        return (v >> (size - 2 << 3)) & 0xFFFF;
    }
    template<typename T>
    uint16_t LOWORD(const T& v)
    {
        return v & 0xFFFF;
    }
    uint32_t readBitsUp32(uint64_t value, uint32_t offs, uint32_t bitSize);

    template<typename R>
    R read(const SequenceByteContainer& bytes, size_t off)
    {
        auto its = sizeof(R);
        if (its + off > bytes.size())
            throw std::runtime_error("Cannot read byte in Byte Container. Index too far.");
        R v = 0;
        for (int i = 0; i < its; i++)
        {
            v |= bytes.at(i + off) << (i * 8);
        }
        return v;
    }
    template<typename R>
    R read(const SequenceByteContainer::const_iterator& iterator, size_t off)
    {
        auto its = sizeof(R);
        R v = 0;
        for (int i = 0; i < its; i++)
        {
            v |= *(iterator+off+i) << (i * 8);
        }
        return v;
    }
}
namespace StringUtils {
    //blatantly copied from fluentcpp.com
    SequenceContainer<String> split(String s, char delimiter);
    String rotateRight(const String& s, uint32_t rotations);
    String rotateLeft(const String& s, uint32_t rotations);
    String to_lower(const String& s);
    int hexchar2int(char input);
    String string_to_hexstr(const String& s);
    String hexstr_to_string(const String& hs);
    template <typename T>
    String join(const T& v, const String& delim) {
        StringStream s;
        for (const auto& i : v) {
            if (&i != &v[0]) {
                s << delim;
            }
            s << i;
        }
        return s.str();
    }
    bool ends_with(const String& input, const String& ending);

}

namespace File {
    size_t getFileSize(InputFileStream& file);
    void writeFile(String fileName, const SequenceByteContainer& data);
    void writeFile(String fileName, const String& data);
    SequenceByteContainer readFile(String fileName);
    SequenceByteContainer readFile(String fileName, size_t maxSize);
    String readFileString(String fileName);
    SequenceContainer<String> getExtensionsOfPath(const String& path);
}