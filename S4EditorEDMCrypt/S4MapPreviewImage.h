#pragma once
#include "types.h"
#include "DecodeColorData.h"

#include <png++/png.hpp>

namespace S4MAP::S4MapFile {
	using namespace std::string_literals;
	using namespace S4::Util;
	class CS4MapPreviewImage {
		SequenceByteContainer m_rgb565buf;
		SequenceContainer<uint32_t> m_rgb888buf;
		const size_t CONTENT_WIDTH = 256;
		const size_t CONTENT_HEIGHT = CONTENT_WIDTH;

		const size_t IMAGE_WIDTH = CONTENT_WIDTH * 1.5;
		const size_t IMAGE_HEIGHT = CONTENT_HEIGHT;

		SequenceContainer<uint32_t> DecodeBuf(const SequenceByteContainer& rgb565buf) {
			if (rgb565buf.size() % 2 != 0)
				throw std::runtime_error("RGB565 buffer size is not divisable by 2 (not proper RGB565)");

			SequenceContainer<uint32_t> rgb888buf;

			rgb888buf.resize(rgb565buf.size() / 2);
			for (uint32_t i = 0; i < rgb888buf.size(); i++)
			{
				uint16_t col = rgb565buf.at(i * 2) | (rgb565buf.at(i * 2 + 1) << 8);
				rgb888buf.at(i) = Color::Decode565(col);
			}
			return rgb888buf;
		}
	public:
		CS4MapPreviewImage()
		{

		}
		CS4MapPreviewImage(const CS4MapPreviewImage& cpy)
		{
			this->m_rgb565buf.assign(cpy.m_rgb565buf.begin(), cpy.m_rgb565buf.end());
			this->m_rgb888buf.assign(cpy.m_rgb888buf.begin(), cpy.m_rgb888buf.end());
		}
		CS4MapPreviewImage& operator=(const CS4MapPreviewImage& cpy)
		{
			this->m_rgb565buf.assign(cpy.m_rgb565buf.begin(), cpy.m_rgb565buf.end());
			this->m_rgb888buf.assign(cpy.m_rgb888buf.begin(), cpy.m_rgb888buf.end());
			return *this;
		}
		CS4MapPreviewImage(CS4MapPreviewImage&& mv)
		{
			m_rgb565buf = std::move(mv.m_rgb565buf);
			m_rgb888buf = std::move(mv.m_rgb888buf);
		}

		~CS4MapPreviewImage() {
			m_rgb565buf.clear();
			m_rgb888buf.clear();
		}

		auto& getRGB565Buf() const { return m_rgb565buf; }
		auto& getRGB888Buf() const { return m_rgb888buf; }

		auto getImageWidth() const { return IMAGE_WIDTH; }
		auto getImageHeight() const { return IMAGE_HEIGHT; }

		auto getContentWidth() const { return CONTENT_WIDTH; }
		auto getContentHeight() const { return CONTENT_HEIGHT; }


		void loadImage(const SequenceByteContainer& rgb565buf) {
			if (rgb565buf.size() != 2 * CONTENT_WIDTH * CONTENT_HEIGHT) {
				throw std::runtime_error("Map Preview expected 131072 (0x20000) Bytes size but is "s + std::to_string(m_rgb565buf.size()));
			}
			m_rgb565buf = rgb565buf;
			m_rgb888buf = DecodeBuf(rgb565buf);
		}

		bool savePNG(const String& name) const {
			png::image<png::rgba_pixel> img(CONTENT_WIDTH * 1.5, CONTENT_HEIGHT);
			for (uint32_t y = 0; y < CONTENT_HEIGHT; y++)
			{
				for (uint32_t x = 0; x < CONTENT_WIDTH; x++)
				{
					uint32_t xScew = x + img.get_width() - CONTENT_WIDTH - y / 2;

					uint32_t col = m_rgb888buf.at(y * CONTENT_HEIGHT + x);
					img[y][xScew] = png::rgba_pixel((col >> 16) & 0xFF, (col >> 8) & 0xFF, col & 0xFF);
				}
			}
			img.write(name);
			return true;
		}
	};
}