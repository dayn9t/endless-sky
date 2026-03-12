/* Font.cpp
Copyright (c) 2014-2020 by Michael Zahniser

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Font.h"

#include "Alignment.h"
#include "../Color.h"
#include "DisplayText.h"
#include "../GameData.h"
#include "../Point.h"
#include "../Preferences.h"
#include "../Screen.h"
#include "Truncate.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <algorithm>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <functional>
#include <stdexcept>
#include <vector>

using namespace std;

namespace {
	bool showUnderlines = false;
	const int KERN = 1;

	// Find a CJK-capable font on the current platform.
	// Returns empty string if none found.
	string FindCJKFont()
	{
		// Candidate paths in priority order.
		static const char *candidates[] = {
			// Linux: Noto CJK (most common)
			"/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc",
			"/usr/share/fonts/noto-cjk/NotoSansCJK-Regular.ttc",
			"/usr/share/fonts/noto/NotoSansCJK-Regular.ttc",
			// Linux: WenQuanYi fallback
			"/usr/share/fonts/truetype/wqy/wqy-microhei.ttc",
			"/usr/share/fonts/wqy-microhei/wqy-microhei.ttc",
			// macOS: system CJK fonts
			"/System/Library/Fonts/PingFang.ttc",
			"/Library/Fonts/Arial Unicode MS.ttf",
			// Windows: system CJK fonts
			"C:\\Windows\\Fonts\\msyh.ttc",
			"C:\\Windows\\Fonts\\simsun.ttc",
			nullptr
		};
		for(int i = 0; candidates[i]; ++i)
			if(filesystem::exists(candidates[i]))
				return candidates[i];
		return {};
	}
}

// Static members.
GLuint Font::vao = 0;
GLuint Font::vbo = 0;
GLint Font::colorI = 0;
GLint Font::scaleI = 0;
GLint Font::glyphSizeI = 0;
GLint Font::uvRectI = 0;
GLint Font::positionI = 0;
GLint Font::vertI = 0;
GLint Font::cornerI = 0;



Font::Font(const filesystem::path &)
{
	Load({});
}



Font::~Font()
{
	if(ftFace)
		FT_Done_Face(static_cast<FT_Face>(ftFace));
	if(ftLibrary)
		FT_Done_FreeType(static_cast<FT_Library>(ftLibrary));
}



void Font::Load(const filesystem::path &)
{
	FT_Library lib;
	if(FT_Init_FreeType(&lib))
		throw runtime_error("FreeType init failed");
	ftLibrary = lib;

	string fontPath = FindCJKFont();
	if(fontPath.empty())
		throw runtime_error("No CJK-capable font found. Please install a Noto CJK font.");

	FT_Face face;
	if(FT_New_Face(lib, fontPath.c_str(), 0, &face))
		throw runtime_error("Cannot load font: " + fontPath);
	ftFace = face;

	FT_Set_Pixel_Sizes(face, 0, 14);
	height = static_cast<int>(face->size->metrics.height >> 6);
	ascender = static_cast<int>(face->size->metrics.ascender >> 6);
	space = height / 2;

	// Create atlas texture (single-channel GL_R8).
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	vector<uint8_t> blank(ATLAS_SIZE * ATLAS_SIZE, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, ATLAS_SIZE, ATLAS_SIZE, 0,
		GL_RED, GL_UNSIGNED_BYTE, blank.data());
	glBindTexture(GL_TEXTURE_2D, 0);

	SetUpShader();
}



void Font::SetPixelSize(int px)
{
	FT_Face face = static_cast<FT_Face>(ftFace);
	FT_Set_Pixel_Sizes(face, 0, px);
	height = static_cast<int>(face->size->metrics.height >> 6);
	ascender = static_cast<int>(face->size->metrics.ascender >> 6);
	space = height / 2;
	glyphCache.clear();
	atlasX = atlasY = atlasRowH = 0;
	// Clear atlas texture.
	vector<uint8_t> blank(ATLAS_SIZE * ATLAS_SIZE, 0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, ATLAS_SIZE, ATLAS_SIZE,
		GL_RED, GL_UNSIGNED_BYTE, blank.data());
	glBindTexture(GL_TEXTURE_2D, 0);
}



void Font::Draw(const DisplayText &text, const Point &point, const Color &color) const
{
	DrawAliased(text, round(point.X()), round(point.Y()), color);
}



void Font::DrawAliased(const DisplayText &text, double x, double y, const Color &color) const
{
	int width = -1;
	const string truncText = TruncateText(text, width);
	const auto &layout = text.GetLayout();
	if(width >= 0)
	{
		if(layout.align == Alignment::CENTER)
			x += (layout.width - width) / 2;
		else if(layout.align == Alignment::RIGHT)
			x += layout.width - width;
	}
	DrawAliased(truncText, x, y, color);
}



void Font::Draw(const string &str, const Point &point, const Color &color) const
{
	DrawAliased(str, round(point.X()), round(point.Y()), color);
}



void Font::DrawAliased(const string &str, double x, double y, const Color &color) const
{
	glUseProgram(shader->Object());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(shader->Uniform("tex"), 0);

	if(OpenGL::HasVaoSupport())
		glBindVertexArray(vao);
	else
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		constexpr auto stride = 4 * sizeof(GLfloat);
		glEnableVertexAttribArray(vertI);
		glVertexAttribPointer(vertI, 2, GL_FLOAT, GL_FALSE, stride, nullptr);
		glEnableVertexAttribArray(cornerI);
		glVertexAttribPointer(cornerI, 2, GL_FLOAT, GL_FALSE, stride,
			reinterpret_cast<const GLvoid *>(2 * sizeof(GLfloat)));
	}

	glUniform4fv(colorI, 1, color.Get());

	if(Screen::Width() != screenWidth || Screen::Height() != screenHeight)
	{
		screenWidth = Screen::Width();
		screenHeight = Screen::Height();
		scale[0] = 2.f / screenWidth;
		scale[1] = -2.f / screenHeight;
	}
	glUniform2fv(scaleI, 1, scale);

	GLfloat penX = static_cast<GLfloat>(x);
	// y is the top of the text line; convert to baseline for FreeType bearing math.
	GLfloat penY = static_cast<GLfloat>(y) + ascender;

	const char *p = str.c_str();
	while(*p)
	{
		if(*p == '_')
		{
			if(showUnderlines)
			{
				// Draw underline as underscore glyph under next char — skip for now.
			}
			++p;
			continue;
		}

		uint32_t cp = NextCodepoint(p);
		if(cp == ' ' || cp == '\t' || cp == '\n') { penX += space; continue; }

		const GlyphInfo &g = GetGlyph(cp);
		if(!g.loaded) { penX += space; continue; }

		GLfloat gx = penX + static_cast<GLfloat>(g.bearingX);
		GLfloat gy = penY - static_cast<GLfloat>(g.bearingY);
		GLfloat gw = static_cast<GLfloat>(g.bitmapW);
		GLfloat gh = static_cast<GLfloat>(g.bitmapH);

		glUniform2f(glyphSizeI, gw, gh);
		GLfloat pos[2] = {gx, gy};
		glUniform2fv(positionI, 1, pos);

		GLfloat u0 = static_cast<GLfloat>(g.atlasX) / ATLAS_SIZE;
		GLfloat v0 = static_cast<GLfloat>(g.atlasY) / ATLAS_SIZE;
		GLfloat u1 = static_cast<GLfloat>(g.atlasX + g.bitmapW) / ATLAS_SIZE;
		GLfloat v1 = static_cast<GLfloat>(g.atlasY + g.bitmapH) / ATLAS_SIZE;
		glUniform4f(uvRectI, u0, v0, u1, v1);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		penX += static_cast<GLfloat>(g.advance + KERN);
	}

	if(OpenGL::HasVaoSupport())
		glBindVertexArray(0);
	else
	{
		glDisableVertexAttribArray(vertI);
		glDisableVertexAttribArray(cornerI);
	}
	glUseProgram(0);
}



int Font::Width(const string &str, char after) const
{
	return WidthRawString(str.c_str(), after);
}



int Font::FormattedWidth(const DisplayText &text, char after) const
{
	int width = -1;
	const string truncText = TruncateText(text, width);
	return width < 0 ? WidthRawString(truncText.c_str(), after) : width;
}



int Font::Height() const noexcept
{
	return height;
}



int Font::Space() const noexcept
{
	return space;
}



void Font::ShowUnderlines(bool show) noexcept
{
	showUnderlines = show || Preferences::Has("Always underline shortcuts");
}



uint32_t Font::NextCodepoint(const char *&p) noexcept
{
	unsigned char c = static_cast<unsigned char>(*p++);
	if(c < 0x80) return c;
	uint32_t cp;
	int extra;
	if((c & 0xE0) == 0xC0)      { cp = c & 0x1F; extra = 1; }
	else if((c & 0xF0) == 0xE0) { cp = c & 0x0F; extra = 2; }
	else if((c & 0xF8) == 0xF0) { cp = c & 0x07; extra = 3; }
	else return '?';
	while(extra-- > 0 && (*p & 0xC0) == 0x80)
		cp = (cp << 6) | (static_cast<unsigned char>(*p++) & 0x3F);
	return cp;
}



const Font::GlyphInfo &Font::GetGlyph(uint32_t cp) const
{
	auto it = glyphCache.find(cp);
	if(it != glyphCache.end())
		return it->second;
	GlyphInfo &info = glyphCache[cp];
	RenderGlyphToAtlas(cp, info);
	return info;
}



void Font::RenderGlyphToAtlas(uint32_t cp, GlyphInfo &info) const
{
	FT_Face face = static_cast<FT_Face>(ftFace);
	if(FT_Load_Char(face, cp, FT_LOAD_RENDER))
		return;

	FT_GlyphSlot slot = face->glyph;
	int bw = static_cast<int>(slot->bitmap.width);
	int bh = static_cast<int>(slot->bitmap.rows);

	if(bw == 0 || bh == 0)
	{
		// Whitespace or invisible glyph — store advance only.
		info.advance = (slot->advance.x >> 6);
		info.loaded = true;
		return;
	}

	// Advance to next row if needed.
	if(atlasX + bw > ATLAS_SIZE)
	{
		atlasX = 0;
		atlasY += atlasRowH + 1;
		atlasRowH = 0;
	}
	if(atlasY + bh > ATLAS_SIZE)
		return; // Atlas full.

	glBindTexture(GL_TEXTURE_2D, texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// FreeType bitmap may have padding; handle pitch if different from width.
	if(slot->bitmap.pitch == bw)
	{
		glTexSubImage2D(GL_TEXTURE_2D, 0,
			atlasX, atlasY, bw, bh,
			GL_RED, GL_UNSIGNED_BYTE, slot->bitmap.buffer);
	}
	else
	{
		// Upload row by row to handle pitch.
		for(int row = 0; row < bh; ++row)
		{
			const uint8_t *rowPtr = slot->bitmap.buffer + row * slot->bitmap.pitch;
			glTexSubImage2D(GL_TEXTURE_2D, 0,
				atlasX, atlasY + row, bw, 1,
				GL_RED, GL_UNSIGNED_BYTE, rowPtr);
		}
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	info.atlasX   = atlasX;
	info.atlasY   = atlasY;
	info.bitmapW  = bw;
	info.bitmapH  = bh;
	info.bearingX = slot->bitmap_left;
	info.bearingY = slot->bitmap_top;
	info.advance  = (slot->advance.x >> 6);
	info.loaded   = true;

	atlasX += bw + 1;
	atlasRowH = max(atlasRowH, bh);
}



void Font::SetUpShader()
{
	shader = GameData::Shaders().Get("font");
	if(!vbo)
	{
		vertI   = shader->Attrib("vert");
		cornerI = shader->Attrib("corner");

		glUseProgram(shader->Object());
		glUniform1i(shader->Uniform("tex"), 0);
		glUseProgram(0);

		if(OpenGL::HasVaoSupport())
		{
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);
		}
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		GLfloat vertices[] = {
			0.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 1.f,
			1.f, 0.f, 1.f, 0.f,
			1.f, 1.f, 1.f, 1.f
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		if(OpenGL::HasVaoSupport())
		{
			constexpr auto stride = 4 * sizeof(GLfloat);
			glEnableVertexAttribArray(vertI);
			glVertexAttribPointer(vertI, 2, GL_FLOAT, GL_FALSE, stride, nullptr);
			glEnableVertexAttribArray(cornerI);
			glVertexAttribPointer(cornerI, 2, GL_FLOAT, GL_FALSE, stride,
				reinterpret_cast<const GLvoid *>(2 * sizeof(GLfloat)));
			glBindVertexArray(0);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		colorI     = shader->Uniform("color");
		scaleI     = shader->Uniform("scale");
		glyphSizeI = shader->Uniform("glyphSize");
		uvRectI    = shader->Uniform("uvRect");
		positionI  = shader->Uniform("position");
	}
	screenWidth = 0;
	screenHeight = 0;
}



int Font::WidthRawString(const char *str, char) const noexcept
{
	int width = 0;
	const char *p = str;
	while(*p)
	{
		if(*p == '_') { ++p; continue; }
		uint32_t cp = NextCodepoint(p);
		if(cp == ' ' || cp == '\t') { width += space; continue; }
		const GlyphInfo &g = GetGlyph(cp);
		width += (g.loaded ? g.advance : space) + KERN;
	}
	return width > 0 ? width - KERN : 0;
}



// Param width will be set to the width of the return value, unless the layout width is negative.
string Font::TruncateText(const DisplayText &text, int &width) const
{
	width = -1;
	const auto &layout = text.GetLayout();
	const string &str = text.GetText();
	if(layout.width < 0 || (layout.align == Alignment::LEFT && layout.truncate == Truncate::NONE))
		return str;
	width = layout.width;
	switch(layout.truncate)
	{
		case Truncate::NONE:
			width = WidthRawString(str.c_str());
			return str;
		case Truncate::FRONT:
			return TruncateFront(str, width);
		case Truncate::MIDDLE:
			return TruncateMiddle(str, width);
		case Truncate::BACK:
		default:
			return TruncateBack(str, width);
	}
}



string Font::TruncateBack(const string &str, int &width) const
{
	return TruncateEndsOrMiddle(str, width,
		[](const string &str, int charCount)
		{
			return str.substr(0, charCount) + "...";
		});
}



string Font::TruncateFront(const string &str, int &width) const
{
	return TruncateEndsOrMiddle(str, width,
		[](const string &str, int charCount)
		{
			return "..." + str.substr(str.size() - charCount);
		});
}



string Font::TruncateMiddle(const string &str, int &width) const
{
	return TruncateEndsOrMiddle(str, width,
		[](const string &str, int charCount)
		{
			return str.substr(0, (charCount + 1) / 2) + "..." + str.substr(str.size() - charCount / 2);
		});
}



string Font::TruncateEndsOrMiddle(const string &str, int &width,
	function<string(const string &, int)> getResultString) const
{
	int firstWidth = WidthRawString(str.c_str());
	if(firstWidth <= width)
	{
		width = firstWidth;
		return str;
	}

	int workingChars = 0;
	int workingWidth = 0;

	int low = 0, high = static_cast<int>(str.size()) - 1;
	while(low <= high)
	{
		int nextChars = (low + high) / 2;
		int nextWidth = WidthRawString(getResultString(str, nextChars).c_str());
		if(nextWidth <= width)
		{
			if(nextChars > workingChars)
			{
				workingChars = nextChars;
				workingWidth = nextWidth;
			}
			low = nextChars + (nextChars == low);
		}
		else
			high = nextChars - 1;
	}
	width = workingWidth;
	return getResultString(str, workingChars);
}
