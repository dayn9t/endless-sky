/* Locale.h
Copyright (c) 2024 by ES Translator Contributors

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

#include <map>
#include <string>
#include <vector>



// Locale class provides internationalization (i18n) support for Endless Sky.
// It loads translation files and provides methods to retrieve translated strings.
class Locale {
public:
	// Initialize the locale system with the specified language code.
	// Loads translation files from locales/<language>.json
	static void Init(const std::string &language = "en");

	// Get a translated string by its key (e.g., "outfitter_panel.show_for_sale").
	// Returns the original key if no translation is found.
	static std::string Get(const std::string &key);

	// Check if a translation exists for the given key.
	static bool Has(const std::string &key);

	// Format a translated string with variable substitutions.
	// Example: Format("mission.payment", {{"amount", "1000"}})
	// Replaces {amount} in the translation with "1000".
	static std::string Format(const std::string &key, const std::map<std::string, std::string> &vars);

	// Get the current language code.
	static const std::string &Language();

	// Clear all loaded translations (useful for testing).
	static void Clear();


private:
	// Current language code (e.g., "zh_CN", "en").
	static std::string language;

	// Translation map: key -> translated string.
	static std::map<std::string, std::string> translations;

	// Load translations from a JSON file.
	static void LoadTranslations(const std::string &path);

	// Recursively flatten nested JSON into dot-notation keys.
	// Example: {"panel": {"button": "text"}} -> "panel.button" = "text"
	static void FlattenJson(const std::string &prefix, const std::map<std::string, std::string> &data);
};
