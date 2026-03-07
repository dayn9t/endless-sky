# Endless Sky Chinese Localization Design

**Date:** 2026-03-05
**Author:** AI-assisted design
**Status:** Approved

---

## Overview

This document describes the design for adding complete Chinese (Simplified) language support to Endless Sky, including:

1. **Source code internationalization** - i18n system for UI strings
2. **Data file translation** - Plugin-based content translation
3. **AI translation tool** - Automated translation using Qwen 3.5 API
4. **Workflow skill** - Claude Code skill to automate the translation process

---

## Architecture

```
+--------------------------------------------------------------+
|                Endless Sky Chinese Support                   |
+--------------------------------------------------------------+
|                                                              |
|  +------------------+    +--------------------------------+  |
|  |   Source Code    |    |        Data Layer              |  |
|  |                  |    |                                |  |
|  |  UI strings      |    |  data/*.txt                   |  |
|  |  Error messages  |    |  (missions, dialog, news)     |  |
|  |  Menu text       |    |                                |  |
|  +--------+---------+    +---------------+----------------+  |
|           |                              |                   |
|           v                              v                   |
|  +------------------+    +--------------------------------+  |
|  |  Locale Class    |    |  Plugin Mechanism              |  |
|  |  (NEW)           |    |  (Existing)                    |  |
|  |                  |    |                                |  |
|  |  Load JSON       |    |  plugins/chinese/              |  |
|  |  Query trans.    |    |  +-- data/*.txt (Chinese)      |  |
|  +------------------+    +--------------------------------+  |
|                                                              |
+--------------------------------------------------------------+
|                    AI Translation Toolchain                  |
|                                                              |
|  es-translator/                                              |
|  +-- extractor/    # Extract text to translate               |
|  +-- translator/   # Call Qwen API                           |
|  +-- generator/    # Generate translation files              |
|  +-- context/      # Game lore context                       |
+--------------------------------------------------------------+
```

---

## Component 1: Source Code Internationalization

### Locale Class

New files to be created:

- `source/Locale.h`
- `source/Locale.cpp`

```cpp
// source/Locale.h
#pragma once

#include <string>
#include <unordered_map>
#include <string_view>

class Locale {
public:
    static void Init(const std::string &lang = "en");
    static const std::string& CurrentLanguage();
    static std::string_view Get(std::string_view key);

    template<typename... Args>
    static std::string Format(std::string_view key, Args&&... args);

private:
    static std::string currentLang;
    static std::unordered_map<std::string, std::string> translations;
};
```

### Usage Pattern

```cpp
// Before
font.Draw("Show outfits for sale", pos, color);

// After
font.Draw(Locale::Get("outfits.show_for_sale"), pos, color);
```

### Translation File Format

`locales/zh_CN.json`:

```json
{
  "outfits": {
    "show_for_sale": "显示出售的装备",
    "show_installed": "显示已安装的装备"
  },
  "error": {
    "not_enough_money": "你没有足够的积分。还需要 {0} 积分。"
  }
}
```

---

## Component 2: Data File Translation

### Plugin-Based Approach

Endless Sky's plugin system allows data files to be **overridden**. We create a Chinese plugin with translated data files.

### File Structure

```
plugins/
+-- chinese/
    +-- about.txt
    +-- data/
        +-- _ui/
        |   +-- messages.txt
        |   +-- tooltips.txt
        |   +-- help.txt
        +-- human/
        |   +-- culture conversations.txt
        |   +-- free worlds 0 prologue.txt
        +-- hai/
        |   +-- hai missions.txt
        +-- ... (other content)
```

### about.txt

```
name: "简体中文语言包"
version: "1.0.0"
description: "Endless Sky 简体中文翻译插件"
author: "AI Translation"
```

### Key Points

1. **Same structure** - File paths must match original
2. **Text only** - Keywords (mission, conversation) stay in English
3. **Preserve placeholders** - `{0}`, `<player>` remain unchanged

---

## Component 3: AI Translation Tool (es-translator)

### Commands

```bash
# Extract text
es-translator extract ui                    # -> pending_ui.json
es-translator extract data [path]           # -> pending_data.json

# Translate
es-translator translate pending_ui.json     # -> translated_ui.json
es-translator translate pending_data.json   # -> translated_data.json

# Generate
es-translator generate ui translated_ui.json      # -> locales/zh_CN.json
es-translator generate data translated_data.json  # -> plugins/chinese/

# Full pipeline
es-translator run --all
```

### Qwen API Integration

```rust
const SYSTEM_PROMPT: &str = r#"
You are a professional translator for the game Endless Sky.

## Game Background
Endless Sky is an open-source space exploration game. The player is a
starship captain, trading, fighting, and exploring across the galaxy.
Humanity is divided into factions: Republic, Syndicate, Free Worlds, etc.
Alien races include: Hai, Korath, Quarg, and more.

## Terminology
| English | Chinese | Notes |
|---------|---------|-------|
| ship | 飞船 | |
| outfit | 装备 | Weapons, engines, etc. |
| planet | 行星 | |
| system | 星系 | Contains planets |
| hyperdrive | 跃迁引擎 | Interstellar travel |
| credits | 积分 | Currency |

## Rules
1. Keep sci-fi style
2. Use terminology table
3. Preserve {0} placeholders
4. Maintain character voice in dialog
"#;
```

### Batch Strategy

- Batch size: 30 items per API call
- Retry: 3 attempts with exponential backoff
- Cache: Skip already translated items

---

## Component 4: es-translate Skill

### Location

`~/.claude/skills/es-translate/SKILL.md`

### Workflow

1. Check environment (API key, tool)
2. Extract text (UI + data)
3. Translate via Qwen API
4. Generate translation files
5. Verify (build + test)

### Triggers

- "翻译 endless sky"
- "更新中文翻译"
- "/es-translate"

---

## Implementation Plan

### Phase 1: AI Translation Tool

| Task | Description |
|------|-------------|
| 1.1 | Project init (Rust + dependencies) |
| 1.2 | Text extractor (UI + data files) |
| 1.3 | Qwen API integration |
| 1.4 | Translation file generator |
| 1.5 | CLI commands |

### Phase 2: Source Code i18n

| Task | Description |
|------|-------------|
| 2.1 | Locale class implementation |
| 2.2 | JSON loader |
| 2.3 | UI string wrapping (~100+ files) |
| 2.4 | Language settings UI |

### Phase 3: Translation Execution

| Task | Description |
|------|-------------|
| 3.1 | Extract all translatable text |
| 3.2 | Batch translation (Qwen API) |
| 3.3 | Generate translation files |
| 3.4 | Verification and fixes |

### Phase 4: Skill & Documentation

| Task | Description |
|------|-------------|
| 4.1 | Create es-translate skill |
| 4.2 | Write usage documentation |
| 4.3 | Build and test game |

---

## File Summary

### New Files

```
source/Locale.h
source/Locale.cpp
locales/zh_CN.json
plugins/chinese/about.txt
plugins/chinese/data/**/*.txt
```

### New Project

```
es-translator/
+-- Cargo.toml
+-- src/
|   +-- main.rs
|   +-- extractor.rs
|   +-- translator.rs
|   +-- generator.rs
+-- prompts/
    +-- system.md
```

### New Skill

```
~/.claude/skills/es-translate/SKILL.md
```

### Modified Files

```
source/**/*.cpp    # Wrap UI strings with Locale::Get()
```

---

## Environment

- **API Key:** `DASHSCOPE_API_KEY` in `~/cc/.env`
- **Base URL:** `https://dashscope.aliyuncs.com/compatible-mode/v1`
- **Model:** Qwen 3.5 (qwen-plus or qwen-turbo)

---

## References

- [Endless Sky GitHub](https://github.com/endless-sky/endless-sky)
- [Endless Sky Wiki - CreatingPlugins](https://endless-sky.github.io)
- [Qwen API Documentation](https://help.aliyun.com/document_detail/2712195.html)
