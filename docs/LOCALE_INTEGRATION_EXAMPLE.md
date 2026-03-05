# Locale Integration Example

This document shows how to integrate the Locale system into Endless Sky.

## Step 1: Initialize Locale in main.cpp

Add `#include "Locale.h"` and initialize the locale system early in main():

```cpp
// In main.cpp, after includes
#include "Locale.h"

// In main() function, after Files::Init()
int main(int argc, char *argv[])
{
    // ... existing code ...

    Files::Init(argv[0]);

    // Initialize localization system
    // TODO: Read language preference from config file
    Locale::Init("zh_CN");  // or "en" for English

    // ... rest of main() ...
}
```

## Step 2: Example - TradingPanel.cpp

Replace hardcoded strings with Locale::Get() calls:

```cpp
// Add include at top
#include "Locale.h"

// Before:
const string TRADE_LEVEL[] = {
    "(very low)",
    "(low)",
    "(medium)",
    "(high)",
    "(very high)"
};

// After:
const string TRADE_LEVEL[] = {
    Locale::Get("trading.level.very_low"),
    Locale::Get("trading.level.low"),
    Locale::Get("trading.level.medium"),
    Locale::Get("trading.level.high"),
    Locale::Get("trading.level.very_high")
};

// Before:
string message = "You sold " + Format::CargoString(tonsSold, "cargo ");
if(profit < 0)
    message += "at a loss of " + Format::CreditString(-profit) + ".";
else
    message += "for a total profit of " + Format::CreditString(profit) + ".";

// After:
string message = Locale::Format("trading.sold_cargo",
    Format::CargoString(tonsSold, "cargo "));
if(profit < 0)
    message += Locale::Format("trading.loss", Format::CreditString(-profit));
else
    message += Locale::Format("trading.profit", Format::CreditString(profit));
```

## Step 3: Add Missing Translations

Update `locales/zh_CN.json`:

```json
{
  "trading": {
    "level": {
      "very_low": "(非常低)",
      "low": "(低)",
      "medium": "(中等)",
      "high": "(高)",
      "very_high": "(非常高)"
    },
    "sold_cargo": "你出售了 {0}",
    "loss": "亏损 {0}。",
    "profit": "总利润 {0}。"
  }
}
```

## Step 4: Build and Test

```bash
# Build the game
cmake --build build

# Run and verify Chinese text displays correctly
./build/EndlessSky
```

## Files to Modify (Priority Order)

Based on UI translation keys, these files likely need updates:

1. **TradingPanel.cpp** - Trading interface
2. **MapDetailPanel.cpp** - Map details (friendly, hostile, etc.)
3. **ConversationPanel.cpp** - Dialog system (first name, last name)
4. **OutfitterPanel.cpp** - Outfit shop (not in current translations)
5. **MenuPanel.cpp** - Main menu (not in current translations)

## Current Translation Coverage

From `locales/zh_CN.json`, we have translations for:

- `missiontimer.*` - Mission timer events
- `conversationpanel.*` - Dialog panel (first_name, last_name)
- `mapdetailpanel.*` - Map status (dominated, friendly, hostile, etc.)
- `tradingpanel.*` - Trading interface (buy, sell, commodity, etc.)
- `text_format.*` - Number/date formatting
- `planet.*` - Planet interaction messages

## Next Steps

1. Wait for data file translation to complete (~14 minutes remaining)
2. Modify source files to use Locale::Get()
3. Add Locale::Init() to main.cpp
4. Build and test
5. Generate Chinese plugin from translated data files

## Notes

- The Locale system uses fallback: if a key is not found, it returns the key itself
- This ensures the game remains playable even with incomplete translations
- Translation keys use dot notation for organization (e.g., "trading.buy")
