# Localization Usage Guide

## Overview

This guide explains how to use the Locale system to add Chinese language support to Endless Sky.

## Quick Start

### 1. Initialize Locale System

Add to `source/main.cpp` or `source/GameData.cpp`:

```cpp
#include "Locale.h"

// In main() or GameData::BeginLoad()
Locale::Init("zh_CN");  // or "en" for English
```

### 2. Replace Hardcoded Strings

**Before:**
```cpp
font.Draw("Show outfits for sale", pos, color);
```

**After:**
```cpp
#include "Locale.h"

font.Draw(Locale::Get("outfits.show_for_sale"), pos, color);
```

### 3. Format Strings with Parameters

**Before:**
```cpp
return "You need a further " + Format::Credits(needed) + " credits.";
```

**After:**
```cpp
return Locale::Format("error.not_enough_money", Format::Credits(needed));
```

## Translation Keys

Translation keys use dot notation for organization:

| Pattern | Example | Usage |
|---------|---------|-------|
| `category.key` | `outfits.show_for_sale` | UI labels |
| `category.subcategory.key` | `error.payment.insufficient` | Error messages |
| `file.function.key` | `tradingpanel.buy` | Component-specific |

## Available Translations

See `locales/zh_CN.json` for all available translations:

```json
{
  "tradingpanel": {
    "buy": "[购买]",
    "sell": "[出售]",
    "commodity": "货物",
    "price": "价格"
  },
  "mapdetailpanel": {
    "friendly": "友好",
    "hostile": "敌对",
    "dominated": "被控制"
  }
}
```

## Adding New Translations

### 1. Extract New Strings

```bash
cd es-translator
cargo run -- extract --source ui --output pending_ui.json
```

### 2. Translate

```bash
source ~/cc/.env
cargo run -- translate --input pending_ui.json --output translated_ui.json
```

### 3. Generate Translation File

```bash
cargo run -- generate --type ui --input translated_ui.json
```

### 4. Update Game Code

Replace hardcoded strings with `Locale::Get()` calls.

## Language Switching

To support runtime language switching:

```cpp
// In settings menu
void SetLanguage(const string &lang)
{
    Locale::Init(lang);
    // Reload UI elements
}
```

## Fallback Behavior

If a translation key is not found:
- Returns the key itself as fallback
- Logs a warning (in debug builds)
- Game continues to function

Example:
```cpp
Locale::Get("missing.key");  // Returns "missing.key"
```

## Best Practices

1. **Use descriptive keys**: `error.insufficient_funds` not `err1`
2. **Group by component**: `tradingpanel.*`, `mapdetail.*`
3. **Keep placeholders**: `{0}`, `{1}` for formatted strings
4. **Test fallbacks**: Ensure game works with missing translations

## File Structure

```
endless-sky/
├── locales/
│   ├── en.json          # English (default)
│   └── zh_CN.json       # Simplified Chinese
├── source/
│   ├── Locale.h         # Locale API
│   └── Locale.cpp       # Implementation
└── es-translator/       # Translation tool
    └── ...
```

## Troubleshooting

### Translation not showing

1. Check `Locale::Init()` is called
2. Verify key exists in `locales/zh_CN.json`
3. Check file encoding is UTF-8

### Build errors

1. Ensure `Locale.cpp` is in `CMakeLists.txt`
2. Include `Locale.h` in source files
3. Link against required libraries

### API errors (translation tool)

1. Check `DASHSCOPE_API_KEY` in `~/cc/.env`
2. Verify network connectivity
3. Check API quota/limits

## Examples

### Simple Label

```cpp
// Before
font.Draw("Cargo Free:", point, color);

// After
font.Draw(Locale::Get("menu.cargo_free"), point, color);
```

### Error Message with Parameter

```cpp
// Before
return "You don't have enough money. You need " + to_string(needed) + " more.";

// After
return Locale::Format("error.insufficient_funds", to_string(needed));
```

### Conditional Text

```cpp
// Before
string status = isHostile ? "Hostile" : "Friendly";

// After
string status = isHostile
    ? Locale::Get("status.hostile")
    : Locale::Get("status.friendly");
```

## Performance

- Translations loaded once at startup
- Lookup is O(1) hash map access
- Minimal runtime overhead
- No impact on game performance

## Future Enhancements

- [ ] Runtime language switching UI
- [ ] Plural forms support
- [ ] Context-aware translations
- [ ] Translation validation tool
- [ ] Community translation platform
