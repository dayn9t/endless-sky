/* Font.h
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

#pragma once

#include "../shader/Shader.h"
#include "../opengl.h"

#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>
#include <unordered_map>

class Color;
class DisplayText;
class Point;



// Class for drawing text in OpenGL using a FreeType glyph atlas.
// Supports full Unicode including CJK characters.
class Font {
public:
	Font() noexcept = default;
	explicit Font(const std::filesystem::path &imagePath);
	~Font();

	void Load(const std::filesystem::path &imagePath);
	void SetPixelSize(int px);

	// Draw a text string, subject to the given layout and truncation strategy.
	void Draw(const DisplayText &text, const Point &point, const Color &color) const;
	void DrawAliased(const DisplayText &text, double x, double y, const Color &color) const;
	// Draw the given text string, e.g. post-formatting (or without regard to formatting).
	void Draw(const std::string &str, const Point &point, const Color &color) const;
	void DrawAliased(const std::string &str, double x, double y, const Color &color) const;

	// Determine the string's width, without considering formatting.
	int Width(const std::string &str, char after = ' ') const;
	// Get the width of the text while accounting for the desired layout and truncation strategy.
	int FormattedWidth(const DisplayText &text, char after = ' ') const;

	int Height() const noexcept;
	int Space() const noexcept;

	static void ShowUnderlines(bool show) noexcept;


private:
	// UTF-8 decode: advance *p past one codepoint, return it.
	static uint32_t NextCodepoint(const char *&p) noexcept;

	struct GlyphInfo {
		int atlasX = 0, atlasY = 0;
		int bitmapW = 0, bitmapH = 0;
		int bearingX = 0, bearingY = 0;
		int advance = 0;
		bool loaded = false;
	};

	const GlyphInfo &GetGlyph(uint32_t codepoint) const;
	void RenderGlyphToAtlas(uint32_t codepoint, GlyphInfo &info) const;
	void SetUpShader();

	std::string TruncateText(const DisplayText &text, int &width) const;
	std::string TruncateBack(const std::string &str, int &width) const;
	std::string TruncateFront(const std::string &str, int &width) const;
	std::string TruncateMiddle(const std::string &str, int &width) const;
	std::string TruncateEndsOrMiddle(const std::string &str, int &width,
		std::function<std::string(const std::string &, int)> getResultString) const;

	int WidthRawString(const char *str, char after = ' ') const noexcept;

private:
	// FreeType handles (void* to avoid pulling ft2build.h into every translation unit).
	mutable void *ftLibrary = nullptr;
	mutable void *ftFace = nullptr;

	// OpenGL glyph atlas.
	mutable GLuint texture = 0;
	static const int ATLAS_SIZE = 2048;
	mutable int atlasX = 0, atlasY = 0, atlasRowH = 0;

	// Per-codepoint glyph cache.
	mutable std::unordered_map<uint32_t, GlyphInfo> glyphCache;

	int height = 0;
	int space = 0;
	mutable int screenWidth = 0;
	mutable int screenHeight = 0;
	mutable GLfloat scale[2]{0.f, 0.f};

	// Shader + GL objects (shared across all Font instances of same size).
	const Shader *shader = nullptr;
	static GLuint vao;
	static GLuint vbo;
	static GLint colorI, scaleI, glyphSizeI, uvRectI, positionI;
	static GLint vertI, cornerI;
};
