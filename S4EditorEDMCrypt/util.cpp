#include "util.h"

#include <numeric>
#include <iostream>
#include <algorithm>
#include <random>
#include <map>
#include <memory>

#include <filesystem>

/*
*
*
*
* STRING UTILS
*
*
*
*
*/
using namespace std::string_literals;

//blatantly copied from fluentcpp.com
SequenceContainer<String> StringUtils::split(String s, char delimiter) {
    SequenceContainer<String> tokens;
    String token;
    InputStringStream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

String StringUtils::rotateRight(const String& s, uint32_t rotations) {
    String rotatedString = s;
    uint32_t rotsMinimized = rotations % s.length();
    if (rotsMinimized != 0)
        std::rotate(rotatedString.rbegin(), rotatedString.rbegin() + rotations, rotatedString.rend());
    return rotatedString;
}
String StringUtils::rotateLeft(const String& s, uint32_t rotations) {
    String rotatedString = s;
    uint32_t rotsMinimized = rotations % s.length();
    if (rotsMinimized != 0)
        std::rotate(rotatedString.begin(), rotatedString.begin() + rotations, rotatedString.end());
    return rotatedString;
}

String StringUtils::to_lower(const String& s)
{
    String sl = s;
    std::transform(sl.begin(), sl.end(), sl.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return sl;
}

int StringUtils::hexchar2int(char input)
{
    if (input >= '0' && input <= '9')
        return input - '0';
    if (input >= 'A' && input <= 'F')
        return input - 'A' + 10;
    if (input >= 'a' && input <= 'f')
        return input - 'a' + 10;
    throw std::invalid_argument("hexchar2int: Invalid character input, not hexchar");
}

String StringUtils::hexstr_to_string(const String& hs)
{
    if (hs.size() % 2)
        throw std::length_error("hexstr needs to be a size of 2n");

    std::stringstream ss;
    for (int i = 0; i < hs.size() / 2; i++)
    {
        char v = hexchar2int(hs.at(i * 2)) * 16
            + hexchar2int(hs.at(i * 2 + 1));
        ss << v;
    }
    return ss.str();
}

bool StringUtils::ends_with(const String& input, const String& ending)
{
    if (input.length() < ending.length())
        return false;
    else
        return input.substr(input.length() - ending.length()) == ending;
}

String StringUtils::string_to_hexstr(const String& s)
{
    StringStream ss;
    for (auto& c : s)
    {
        ss << std::hex << static_cast<int>(c);
    }
    return ss.str();
}

/*
*
*
*
* FILE IO
*
*
*
*
*/

size_t File::getFileSize(InputFileStream& file) {
    auto prePos = file.tellg();
    file.seekg(0, std::ios::end);
    auto fileSize = file.tellg();
    file.seekg(prePos, std::ios::beg);
    return size_t(fileSize);

}

void File::writeFile(String fileName, const SequenceByteContainer& data) {
    OutputFileStream file(fileName, std::ios::binary);
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    file.close();
}
void File::writeFile(String fileName, const String& data) {
    OutputFileStream file(fileName, std::ios::binary);
    file.write(data.data(), data.length());
    file.close();
}
SequenceByteContainer File::readFile(String fileName) {
    InputFileStream file(fileName, std::ios::binary);
    if (file.is_open() == false)
    {
        String err = "Could not open file "s + fileName;
        throw std::runtime_error(err);
    }
    SequenceByteContainer data;
    size_t filesize = File::getFileSize(file);
    data.resize(filesize);
    file.read(reinterpret_cast<char*>(data.data()), filesize);
    file.close();
    return data;
}
SequenceByteContainer File::readFile(String fileName, size_t maxSize)
{
    InputFileStream file(fileName, std::ios::binary);
    if (file.is_open() == false)
    {
        String err = "Could not open file "s + fileName;
        throw std::runtime_error(err);
    }
    SequenceByteContainer data;
    size_t filesize = File::getFileSize(file);
    size_t readFileSize = std::min(maxSize, filesize);
    data.resize(readFileSize);
    file.read(reinterpret_cast<char*>(data.data()), readFileSize);
    file.close();
    return data;
}
String File::readFileString(String fileName) {
    InputFileStream file(fileName);
    if (file.is_open() == false)
    {
        String err = "Could not open file "s + fileName;
        throw std::runtime_error(err);
    }
    StringStream buf;
    buf << file.rdbuf();
    return buf.str();
}

SequenceContainer<String> File::getExtensionsOfPath(const String& path)
{
    SequenceContainer<String> exts;
    for (const auto& file : std::filesystem::directory_iterator(path)) {
        if (file.is_regular_file()) { // sub folders will not be added
            //TODO: test empty extension
            if (file.path().has_extension())
                ContainerUtils::addToUniqueSequenceContainer(exts, StringUtils::to_lower(file.path().extension().string().substr(1)));
        }
    }
    return exts;
}

// removes 1 instance
void ContainerUtils::removeFromUniqueSequenceContainer(SequenceContainer<String>& container, const String& v)
{
    SequenceContainer<String>::iterator position = std::find(container.begin(), container.end(), v);
    if (position != container.end())
        container.erase(position);
}

// adds 1 instance if v not exists already
void ContainerUtils::addToUniqueSequenceContainer(SequenceContainer<String>& container, const String& v)
{
    SequenceContainer<String>::iterator position = std::find(container.begin(), container.end(), v);
    if (position == container.end())
        container.push_back(v);
}

// adds 1 instance if v not exists already
bool ContainerUtils::containsElementInUniqueSequenceContainer(const SequenceContainer<String>& container, const String& v)
{
    SequenceContainer<String>::const_iterator position = std::find(container.begin(), container.end(), v);
    return position != container.end();
}

uint32_t BitUtils::readBitsUp32(uint64_t value, uint32_t offs, uint32_t bitSize) {
    if (bitSize > 32)
        throw ::std::runtime_error("readToDword() only accepts up to 32 bits to read");
    uint64_t res = value;

    res >>= offs;
    res &= (1ULL << bitSize) - 1;

    return static_cast<uint32_t>(res);
}
