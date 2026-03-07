# FreeType Font Rendering Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Replace the ASCII-only bitmap font system with FreeType-based rendering to support Chinese (and all Unicode) characters.

**Architecture:** Add a FreeType glyph atlas to `Font.cpp` — a 2048×2048 OpenGL texture that glyphs are rendered into on demand. Replace the fixed-98-slot vertex shader with a UV-rect based approach. Keep `Font::Draw(string, point, color)` interface unchanged so all 168 call sites need no modification.

**Tech Stack:** FreeType 2 (already installed at `/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc`), OpenGL, CMake PkgConfig

---

### Task 1: Add FreeType to CMakeLists.txt

**Files:**
- Modify: `CMakeLists.txt`

**Step 1: Add FreeType find_package after line 112 (after ZLIB)**

```cmake
find_package(PkgConfig REQUIRED)
pkg_check_modules(FREETYPE REQUIRED freetype2)
```

**Step 2: Add FreeType to ExternalLibraries link (line 173 block)**

After the existing `target_link_libraries(ExternalLibraries INTERFACE SDL2::SDL2 ...)` line, add:

```cmake
target_link_libraries(ExternalLibraries INTERFACE ${FREETYPE_LIBRARIES})
target_include_directories(ExternalLibraries INTERFACE ${FREETYPE_INCLUDE_DIRS})
```

**Step 3: Build to verify FreeType links**

```bash
cd /home/jiang/cc/games/endless-sky
cmake --build build --parallel 2>&1 | grep -E "error:|freetype|FreeType" | head -20
```

Expected: build succeeds (no new errors).

**Step 4: Commit**

```bash
git add CMakeLists.txt
git commit -m "build: add FreeType2 dependency"
```

---

### Task 2: Replace font.vert shader with UV-rect approach

**Files:**
- Modify: `shaders/font.vert`

The current shader uses `uniform int glyph` to index into a fixed 98-slot strip texture. Replace with `uniform vec4 uvRect` (x, y, w, h in atlas UV space).

**Step 1: Replace font.vert entirely**

```glsl
/* font.vert */
uniform vec2 scale;
uniform vec2 position;
// UV rectangle in atlas: (u0, v0, u1, v1)
uniform vec4 uvRect;
uniform vec2 glyphSize;

in vec2 vert;
in vec2 corner;

out vec2 texCoord;

void main() {
    texCoord = vec2(
        uvRect.x + corner.x * (uvRect.z - uvRect.x),
        uvRect.y + corner.y * (uvRect.w - uvRect.y)
    );
    vec2 pos = vert * glyphSize;
    gl_Position = vec4(
        (pos.x + position.x) * scale.x,
        (pos.y + position.y) * scale.y,
        0.f, 1.f
    );
}
```

Note: removed `aspect` uniform (no longer needed — glyph size is set directly via `glyphSize`).

**Step 2: No build yet** — Font.cpp must be updated first or the shader uniform names won't match.

---

### Task 3: Rewrite Font.h for FreeType atlas

**Files:**
- Modify: `source/text/Font.h`

**Step 1: Replace private section of Font.h**

Keep the entire public interface unchanged. Replace the private section (lines 63–94) with:

```cpp
private:
    // UTF-8 decode: advance *p past one codepoint, return it.
    static uint32_t NextCodepoint(const char *&p) noexcept;

    struct GlyphInfo {
        // Position in atlas texture (pixels).
        int atlasX = 0, atlasY = 0;
        int bitmapW = 0, bitmapH = 0;
        // Bearing (offset from pen position to top-left of bitmap).
        int bearingX = 0, bearingY = 0;
        // How far to advance pen after this glyph.
        int advance = 0;
        bool loaded = false;
    };

    const GlyphInfo &GetGlyph(uint32_t codepoint) const;
    void RenderGlyphToAtlas(uint32_t codepoint, GlyphInfo &info) const;
    void SetUpShader();

private:
    // FreeType
    mutable void *ftLibrary = nullptr;   // FT_Library (void* to avoid header in .h)
    mutable void *ftFace = nullptr;      // FT_Face

    // OpenGL atlas
    mutable GLuint texture = 0;
    static const int ATLAS_SIZE = 2048;
    mutable int atlasX = 0, atlasY = 0, atlasRowH = 0;

    // Glyph cache
    mutable std::unordered_map<uint32_t, GlyphInfo> glyphCache;

    int height = 0;
    int space = 0;
    mutable int screenWidth = 0;
    mutable int screenHeight = 0;
    mutable GLfloat scale[2]{0.f, 0.f};

    // Shader uniforms
    const Shader *shader = nullptr;
    static GLuint vao;
    static GLuint vbo;
    static GLint colorI, scaleI, glyphSizeI, uvRectI, positionI;
    static GLint vertI, cornerI;
};
```

**Step 2: Add `#include <unordered_map>` and `#include <cstdint>` to Font.h includes**

---

### Task 4: Rewrite Font.cpp with FreeType atlas rendering

**Files:**
- Modify: `source/text/Font.cpp`

This is the main implementation. Replace the entire file.

**Step 1: Replace includes at top of Font.cpp**

```cpp
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
#include <stdexcept>

using namespace std;
```

**Step 2: Replace anonymous namespace and static definitions**

```cpp
namespace {
    bool showUnderlines = false;
    const int KERN = 1;
    // Path to the TTF/TTC font used for all text rendering.
    const char *FONT_PATH = "/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc";
}

GLuint Font::vao = 0;
GLuint Font::vbo = 0;
GLint Font::colorI = 0;
GLint Font::scaleI = 0;
GLint Font::glyphSizeI = 0;
GLint Font::uvRectI = 0;
GLint Font::positionI = 0;
GLint Font::vertI = 0;
GLint Font::cornerI = 0;
```

**Step 3: Replace Font::Load()**

```cpp
Font::Font(const filesystem::path &) { /* path ignored, TTF used instead */ Load({}); }

void Font::Load(const filesystem::path &)
{
    FT_Library lib;
    if(FT_Init_FreeType(&lib))
        throw runtime_error("FreeType init failed");
    ftLibrary = lib;

    FT_Face face;
    if(FT_New_Face(lib, FONT_PATH, 0, &face))
        throw runtime_error(string("Cannot load font: ") + FONT_PATH);
    ftFace = face;

    // Set pixel size matching the original 14px bitmap font.
    FT_Set_Pixel_Sizes(face, 0, height > 0 ? height : 14);
    height = (face->size->metrics.height >> 6);
    space = height / 2;

    // Create atlas texture (single-channel, GL_RED).
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // Allocate empty atlas.
    vector<uint8_t> blank(ATLAS_SIZE * ATLAS_SIZE, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, ATLAS_SIZE, ATLAS_SIZE, 0,
        GL_RED, GL_UNSIGNED_BYTE, blank.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    SetUpShader();
}
```

**Step 4: Replace DrawAliased(string)**

```cpp
void Font::DrawAliased(const string &str, double x, double y, const Color &color) const
{
    glUseProgram(shader->Object());
    glBindTexture(GL_TEXTURE_2D, texture);
    if(OpenGL::HasVaoSupport())
        glBindVertexArray(vao);
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        // enable attrib arrays inline
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

    GLfloat penX = static_cast<float>(x);
    GLfloat penY = static_cast<float>(y);

    const char *p = str.c_str();
    while(*p)
    {
        if(*p == '_') { ++p; continue; }

        uint32_t cp = NextCodepoint(p);
        if(cp == ' ' || cp == '\t') { penX += space; continue; }

        const GlyphInfo &g = GetGlyph(cp);
        if(!g.loaded) { penX += space; continue; }

        // Position: top-left of bitmap relative to baseline.
        GLfloat gx = penX + g.bearingX;
        GLfloat gy = penY - g.bearingY;
        GLfloat gw = static_cast<GLfloat>(g.bitmapW);
        GLfloat gh = static_cast<GLfloat>(g.bitmapH);

        glUniform2f(glyphSizeI, gw * .5f, gh * .5f);
        GLfloat pos[2] = {gx, gy};
        glUniform2fv(positionI, 1, pos);

        // UV rect in atlas.
        GLfloat u0 = static_cast<GLfloat>(g.atlasX) / ATLAS_SIZE;
        GLfloat v0 = static_cast<GLfloat>(g.atlasY) / ATLAS_SIZE;
        GLfloat u1 = static_cast<GLfloat>(g.atlasX + g.bitmapW) / ATLAS_SIZE;
        GLfloat v1 = static_cast<GLfloat>(g.atlasY + g.bitmapH) / ATLAS_SIZE;
        glUniform4f(uvRectI, u0, v0, u1, v1);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        penX += g.advance + KERN;
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
```

**Step 5: Add WidthRawString replacement**

```cpp
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
```

**Step 6: Add helper implementations**

```cpp
// UTF-8 decode one codepoint, advance pointer.
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
    int bw = slot->bitmap.width;
    int bh = slot->bitmap.rows;

    // Advance to next row if needed.
    if(atlasX + bw > ATLAS_SIZE)
    {
        atlasX = 0;
        atlasY += atlasRowH + 1;
        atlasRowH = 0;
    }
    if(atlasY + bh > ATLAS_SIZE)
        return; // Atlas full — skip glyph.

    // Upload bitmap to atlas.
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0,
        atlasX, atlasY, bw, bh,
        GL_RED, GL_UNSIGNED_BYTE, slot->bitmap.buffer);
    glBindTexture(GL_TEXTURE_2D, 0);

    info.atlasX   = atlasX;
    info.atlasY   = atlasY;
    info.bitmapW  = bw;
    info.bitmapH  = bh;
    info.bearingX = slot->bitmap_left;
    info.bearingY = slot->bitmap_top;
    info.advance  = slot->advance.x >> 6;
    info.loaded   = true;

    atlasX += bw + 1;
    atlasRowH = max(atlasRowH, bh);
}
```

**Step 7: Replace SetUpShader()**

```cpp
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

        colorI    = shader->Uniform("color");
        scaleI    = shader->Uniform("scale");
        glyphSizeI = shader->Uniform("glyphSize");
        uvRectI   = shader->Uniform("uvRect");
        positionI = shader->Uniform("position");
    }
    screenWidth = 0;
    screenHeight = 0;
}
```

**Step 8: Update font.frag to handle GL_R8 atlas (single channel)**

The fragment shader currently does `texture(...).a * color`. With GL_R8 the value is in `.r`:

```glsl
void main() {
    finalColor = vec4(color.rgb, color.a * texture(tex, texCoord).r);
}
```

**Step 9: Update GameData.cpp — change font loading**

At lines 257–258, replace PNG paths with pixel sizes only:

```cpp
FontSet::Add(14);
FontSet::Add(18);
```

Then update `FontSet::Add` signature (Task 5).

---

### Task 5: Update FontSet to pass pixel size instead of path

**Files:**
- Modify: `source/text/FontSet.h`
- Modify: `source/text/FontSet.cpp`

**Step 1: Read current FontSet**

```bash
cat source/text/FontSet.h
cat source/text/FontSet.cpp
```

**Step 2: Change Add() signature**

In `FontSet.h`, change:
```cpp
static void Add(const std::filesystem::path &path, int size);
```
to:
```cpp
static void Add(int size);
```

In `FontSet.cpp`, change the `Add` implementation to call `font.Load({})` (path ignored) and set the pixel size via the FreeType face inside `Font::Load`.

The `Font::Load` already sets `FT_Set_Pixel_Sizes(face, 0, 14)` as default. We need to pass the size. Add a `Font::Load(int pixelSize)` overload, or pass size to constructor.

Simplest: keep `Font::Load(const filesystem::path &path)` but use the `path` string as the pixel size hint — no, that's ugly.

Better: add `void Font::SetPixelSize(int px)` called from FontSet::Add after Load:

In `Font.h` public section add:
```cpp
void SetPixelSize(int px);
```

In `Font.cpp`:
```cpp
void Font::SetPixelSize(int px)
{
    FT_Face face = static_cast<FT_Face>(ftFace);
    FT_Set_Pixel_Sizes(face, 0, px);
    height = (face->size->metrics.height >> 6);
    space = height / 2;
    glyphCache.clear(); // invalidate cache for new size
}
```

In `FontSet.cpp`, `Add(int size)`:
```cpp
void FontSet::Add(int size)
{
    Font font;
    font.Load({});
    font.SetPixelSize(size);
    fonts[size] = move(font);
}
```

**Step 3: Build**

```bash
cd /home/jiang/cc/games/endless-sky
cmake --build build --parallel 2>&1 | grep -E "error:" | head -30
```

Expected: no errors.

**Step 4: Commit**

```bash
git add source/text/Font.h source/text/Font.cpp source/text/FontSet.h source/text/FontSet.cpp \
        shaders/font.vert shaders/font.frag source/GameData.cpp CMakeLists.txt
git commit -m "feat: replace bitmap font with FreeType atlas for Unicode/CJK support"
```

---

### Task 6: Run game and verify Chinese text

**Step 1: Launch game**

```bash
cd /home/jiang/cc/games/endless-sky
./build/endless-sky &
sleep 8
cat ~/.local/share/endless-sky/errors.txt | tail -20
```

**Step 2: Check UI shows Chinese**

- Main menu buttons should show English (UI keys not yet wired to Chinese labels — that's OK)
- Open outfitter → "显示待售装备" should appear
- Open map → planet status labels should be Chinese

**Step 3: Check data text**

Start new game → intro conversation should show Chinese text.

**Step 4: If text shows as boxes or blank**

Check errors.txt for shader compile errors:
```bash
grep -i "shader\|glsl\|compile" ~/.local/share/endless-sky/errors.txt
```

**Step 5: Commit any fixes, then done**

```bash
git add -A && git commit -m "fix: font rendering adjustments"
```

---

## Quick Reference

```bash
# Build
cmake --build /home/jiang/cc/games/endless-sky/build --parallel

# Run
cd /home/jiang/cc/games/endless-sky && ./build/endless-sky

# Check errors
cat ~/.local/share/endless-sky/errors.txt | tail -20

# Font file
/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc
```
