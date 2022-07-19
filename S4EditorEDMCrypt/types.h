#pragma once
#include <vector>
#include <array>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>

using SequenceByteContainer = std::vector<uint8_t>;
template <typename T>
using SequenceContainer = std::vector<T>;
template <size_t Size>
using StaticByteArray = std::array<uint8_t, Size>;
template <typename T,size_t Size>
using StaticArray = std::array<T, Size>;
template <size_t Size>
using StaticCharArray = std::array<char, Size>;

using InputFileStream = std::ifstream;
using OutputFileStream = std::ofstream;

using OutputStream = std::ostream;

using InputStringStream = std::istringstream;
using StringStream = std::stringstream;

using String = std::string;

using StringView = std::string_view;

// WCHAR BASED

template <size_t Size>
using StaticWCharArray = std::array<wchar_t, Size>;

using WInputFileStream = std::wifstream;
using WOutputFileStream = std::wofstream;

using WOutputStream = std::wostream;

using WInputStringStream = std::wistringstream;
using WStringStream = std::wstringstream;

using WString = std::wstring;
using WStringView = std::wstring_view;