/* test_plugins.cpp
Copyright (c) 2025 by TomGoodIdea

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "es-test.hpp"

// Include only the tested class's header.
#include "../../../source/Plugins.h"

// ... and any system includes needed for the test file.
#include <filesystem>
#include <fstream>
#include <set>
#include <string>

namespace { // test namespace

// #region mock data

// Helper class to create and manage a temporary directory for tests.
class TempDirectory {
public:
	TempDirectory() : path_(std::filesystem::temp_directory_path() / "es_test_plugins_XXXXXX")
	{
		std::filesystem::create_directories(path_);
	}

	~TempDirectory()
	{
		std::error_code ec;
		std::filesystem::remove_all(path_, ec);
	}

	// No copies or moves.
	TempDirectory(const TempDirectory &) = delete;
	TempDirectory(TempDirectory &&) = delete;
	TempDirectory &operator=(const TempDirectory &) = delete;
	TempDirectory &operator=(TempDirectory &&) = delete;

	const std::filesystem::path &Path() const { return path_; }

	std::filesystem::path operator/(const std::string &subpath) const
	{
		return path_ / subpath;
	}

private:
	std::filesystem::path path_;
};

// Helper to create a file with content.
void CreateFileWithContent(const std::filesystem::path &path, const std::string &content = "")
{
	std::ofstream file(path, std::ios::binary);
	file << content;
}

// Helper to create a minimal plugin directory structure.
void CreatePluginStructure(const std::filesystem::path &pluginPath, const std::string &name,
	const std::string &pluginTxtContent = "")
{
	std::filesystem::create_directories(pluginPath);
	// Create one of the valid asset directories.
	std::filesystem::create_directories(pluginPath / "data");
	if(!pluginTxtContent.empty())
		CreateFileWithContent(pluginPath / "plugin.txt", pluginTxtContent);
}

// Helper to create plugin.txt content.
std::string CreatePluginTxt(const std::string &name,
	const std::string &version = "",
	const std::set<std::string> &authors = {},
	const std::set<std::string> &tags = {},
	const std::string &about = "",
	const std::string &gameVersion = "",
	const std::set<std::string> &required = {},
	const std::set<std::string> &optional = {},
	const std::set<std::string> &conflicts = {})
{
	std::string content;
	content += "name " + name + "\n";
	if(!version.empty())
		content += "version " + version + "\n";
	if(!authors.empty())
	{
		content += "authors\n";
		for(const auto &author : authors)
			content += "\t" + author + "\n";
	}
	if(!tags.empty())
	{
		content += "tags\n";
		for(const auto &tag : tags)
			content += "\t" + tag + "\n";
	}
	if(!about.empty())
		content += "about " + about + "\n";
	if(!gameVersion.empty() || !required.empty() || !optional.empty() || !conflicts.empty())
	{
		content += "dependencies\n";
		if(!gameVersion.empty())
			content += "\tgame version " + gameVersion + "\n";
		if(!required.empty())
		{
			content += "\trequires\n";
			for(const auto &dep : required)
				content += "\t\t" + dep + "\n";
		}
		if(!optional.empty())
		{
			content += "\toptional\n";
			for(const auto &dep : optional)
				content += "\t\t" + dep + "\n";
		}
		if(!conflicts.empty())
		{
			content += "\tconflicts\n";
			for(const auto &dep : conflicts)
				content += "\t\t" + dep + "\n";
		}
	}
	return content;
}

// #endregion mock data



// #region unit tests

// Test Plugin::PluginDependencies::IsEmpty
SCENARIO( "PluginDependencies::IsEmpty checks for any dependencies", "[Plugin][PluginDependencies][IsEmpty]" ) {
	GIVEN( "A PluginDependencies object" ) {
		Plugin::PluginDependencies deps;

		WHEN( "all dependency sets are empty" ) {
			THEN( "IsEmpty returns true" ) {
				CHECK( deps.IsEmpty() );
			}
		}

		WHEN( "required dependencies are present" ) {
			deps.required.insert("other-plugin");
			THEN( "IsEmpty returns false" ) {
				CHECK_FALSE( deps.IsEmpty() );
			}
		}

		WHEN( "optional dependencies are present" ) {
			deps.optional.insert("other-plugin");
			THEN( "IsEmpty returns false" ) {
				CHECK_FALSE( deps.IsEmpty() );
			}
		}

		WHEN( "conflicted dependencies are present" ) {
			deps.conflicted.insert("conflicting-plugin");
			THEN( "IsEmpty returns false" ) {
				CHECK_FALSE( deps.IsEmpty() );
			}
		}

		WHEN( "multiple dependency types are present" ) {
			deps.required.insert("required-plugin");
			deps.optional.insert("optional-plugin");
			deps.conflicted.insert("conflicting-plugin");
			THEN( "IsEmpty returns false" ) {
				CHECK_FALSE( deps.IsEmpty() );
			}
		}
	}
}


// Test Plugin::PluginDependencies::IsValid
SCENARIO( "PluginDependencies::IsValid validates dependencies", "[Plugin][PluginDependencies][IsValid]" ) {
	GIVEN( "A PluginDependencies object" ) {
		Plugin::PluginDependencies deps;

		WHEN( "all dependencies are valid with no overlaps" ) {
			deps.required.insert("plugin-a");
			deps.optional.insert("plugin-b");
			deps.conflicted.insert("plugin-c");

			THEN( "IsValid returns true" ) {
				CHECK( deps.IsValid() );
			}
		}

		WHEN( "a dependency is in both required and optional" ) {
			deps.required.insert("shared-plugin");
			deps.optional.insert("shared-plugin");

			THEN( "IsValid returns true (warning only)" ) {
				// This logs a warning but is still considered valid.
				CHECK( deps.IsValid() );
			}
		}

		WHEN( "a dependency is in both required and conflicted" ) {
			deps.required.insert("conflicted-plugin");
			deps.conflicted.insert("conflicted-plugin");

			THEN( "IsValid returns false" ) {
				CHECK_FALSE( deps.IsValid() );
			}
		}

		WHEN( "a dependency is in both optional and conflicted" ) {
			deps.optional.insert("conflicted-plugin");
			deps.conflicted.insert("conflicted-plugin");

			THEN( "IsValid returns false" ) {
				CHECK_FALSE( deps.IsValid() );
			}
		}

		WHEN( "multiple conflicting dependencies exist" ) {
			deps.required.insert("plugin-a");
			deps.required.insert("plugin-b");
			deps.conflicted.insert("plugin-a");
			deps.conflicted.insert("plugin-b");

			THEN( "IsValid returns false" ) {
				CHECK_FALSE( deps.IsValid() );
			}
		}

		WHEN( "all dependency sets are empty" ) {
			THEN( "IsValid returns true" ) {
				CHECK( deps.IsValid() );
			}
		}
	}
}


// Test Plugin::IsValid
SCENARIO( "Plugin::IsValid checks if plugin is valid", "[Plugin][IsValid]" ) {
	GIVEN( "A Plugin object" ) {
		Plugin plugin;

		WHEN( "plugin has no name" ) {
			plugin.name = "";

			THEN( "IsValid returns false" ) {
				CHECK_FALSE( plugin.IsValid() );
			}
		}

		WHEN( "plugin has a name" ) {
			plugin.name = "test-plugin";

			THEN( "IsValid returns true" ) {
				CHECK( plugin.IsValid() );
			}
		}

		WHEN( "plugin has other fields but no name" ) {
			plugin.path = "/some/path";
			plugin.version = "1.0.0";
			plugin.aboutText = "About text";
			plugin.authors.insert("Author");
			plugin.tags.insert("tag");

			THEN( "IsValid returns false (name is required)" ) {
				CHECK_FALSE( plugin.IsValid() );
			}
		}
	}
}


// Test Plugin::CreateDescription
SCENARIO( "Plugin::CreateDescription generates description string", "[Plugin][CreateDescription]" ) {
	GIVEN( "A Plugin object" ) {
		Plugin plugin;
		plugin.name = "test-plugin";

		WHEN( "plugin has no metadata" ) {
			THEN( "description is empty" ) {
				CHECK( plugin.CreateDescription().empty() );
			}
		}

		WHEN( "plugin has version only" ) {
			plugin.version = "1.2.3";

			THEN( "description contains version" ) {
				std::string desc = plugin.CreateDescription();
				CHECK( desc.find("Version: 1.2.3") != std::string::npos );
			}
		}

		WHEN( "plugin has authors" ) {
			plugin.authors.insert("Alice");
			plugin.authors.insert("Bob");

			THEN( "description contains authors" ) {
				std::string desc = plugin.CreateDescription();
				CHECK( desc.find("Authors:") != std::string::npos );
				CHECK( desc.find("Alice") != std::string::npos );
				CHECK( desc.find("Bob") != std::string::npos );
			}
		}

		WHEN( "plugin has tags" ) {
			plugin.tags.insert("content");
			plugin.tags.insert("utility");

			THEN( "description contains tags" ) {
				std::string desc = plugin.CreateDescription();
				CHECK( desc.find("Tags:") != std::string::npos );
				CHECK( desc.find("content") != std::string::npos );
				CHECK( desc.find("utility") != std::string::npos );
			}
		}

		WHEN( "plugin has about text" ) {
			plugin.aboutText = "This is a test plugin.";

			THEN( "description contains about text" ) {
				std::string desc = plugin.CreateDescription();
				CHECK( desc.find("This is a test plugin.") != std::string::npos );
			}
		}

		WHEN( "plugin has dependencies" ) {
			plugin.dependencies.gameVersion = "0.10.0";
			plugin.dependencies.required.insert("core-plugin");
			plugin.dependencies.optional.insert("optional-addon");
			plugin.dependencies.conflicted.insert("conflicting-mod");

			THEN( "description contains dependency information" ) {
				std::string desc = plugin.CreateDescription();
				CHECK( desc.find("Dependencies:") != std::string::npos );
				CHECK( desc.find("Game Version: 0.10.0") != std::string::npos );
				CHECK( desc.find("Requires:") != std::string::npos );
				CHECK( desc.find("core-plugin") != std::string::npos );
				CHECK( desc.find("Optional:") != std::string::npos );
				CHECK( desc.find("optional-addon") != std::string::npos );
				CHECK( desc.find("Conflicts:") != std::string::npos );
				CHECK( desc.find("conflicting-mod") != std::string::npos );
			}
		}

		WHEN( "plugin has all metadata" ) {
			plugin.version = "2.0.0";
			plugin.authors.insert("Developer");
			plugin.tags.insert("gameplay");
			plugin.dependencies.gameVersion = "0.10.0";
			plugin.aboutText = "Detailed description.";

			THEN( "description contains all information in order" ) {
				std::string desc = plugin.CreateDescription();
				CHECK( desc.find("Version:") < desc.find("Authors:") );
				CHECK( desc.find("Authors:") < desc.find("Tags:") );
				CHECK( desc.find("Tags:") < desc.find("Dependencies:") );
				CHECK( desc.find("Dependencies:") < desc.find("Detailed description.") );
			}
		}
	}
}


// Test Plugins::Get
SCENARIO( "Plugins::Get returns the plugin set", "[Plugins][Get]" ) {
	GIVEN( "The plugin set" ) {
		WHEN( "getting the plugin set" ) {
			const Set<Plugin> &plugins = Plugins::Get();

			THEN( "a reference to the plugin set is returned" ) {
				// Just verify we can call it and it returns a valid reference.
				CHECK( true );
			}
		}
	}
}


// Test Plugins::HasChanged and TogglePlugin
SCENARIO( "Plugins::HasChanged and TogglePlugin work together", "[Plugins][HasChanged][TogglePlugin]" ) {
	GIVEN( "A plugin in the plugin set" ) {
		// Get a reference to the plugin set.
		Set<Plugin> &plugins = const_cast<Set<Plugin>&>(Plugins::Get());
		std::string testPluginName = "test-change-plugin";

		// Create a test plugin.
		Plugin *plugin = plugins.Get(testPluginName);
		plugin->name = testPluginName;
		plugin->enabled = true;
		plugin->currentState = true;

		WHEN( "plugin state matches initial state" ) {
			plugin->enabled = true;
			plugin->currentState = true;

			THEN( "HasChanged returns false" ) {
				CHECK_FALSE( Plugins::HasChanged() );
			}
		}

		WHEN( "TogglePlugin is called once" ) {
			bool originalState = plugin->currentState;
			Plugins::TogglePlugin(testPluginName);

			THEN( "currentState is toggled" ) {
				CHECK( plugin->currentState != originalState );
			}

			AND_THEN( "HasChanged returns true" ) {
				CHECK( Plugins::HasChanged() );
			}
		}

		WHEN( "TogglePlugin is called twice" ) {
			bool originalState = plugin->currentState;
			Plugins::TogglePlugin(testPluginName);
			Plugins::TogglePlugin(testPluginName);

			THEN( "currentState returns to original" ) {
				CHECK( plugin->currentState == originalState );
			}
		}

		// Cleanup: reset the plugin state.
		plugin->enabled = true;
		plugin->currentState = true;
	}
}


// Test Plugins::IsPlugin
SCENARIO( "Plugins::IsPlugin validates plugin directories", "[Plugins][IsPlugin]" ) {
	GIVEN( "A temporary directory" ) {
		TempDirectory tempDir;

		WHEN( "directory contains a 'data' folder" ) {
			auto pluginPath = tempDir / "data-plugin";
			std::filesystem::create_directories(pluginPath / "data");

			THEN( "IsPlugin returns true" ) {
				CHECK( Plugins::IsPlugin(pluginPath) );
			}
		}

		WHEN( "directory contains an 'images' folder" ) {
			auto pluginPath = tempDir / "images-plugin";
			std::filesystem::create_directories(pluginPath / "images");

			THEN( "IsPlugin returns true" ) {
				CHECK( Plugins::IsPlugin(pluginPath) );
			}
		}

		WHEN( "directory contains a 'shaders' folder" ) {
			auto pluginPath = tempDir / "shaders-plugin";
			std::filesystem::create_directories(pluginPath / "shaders");

			THEN( "IsPlugin returns true" ) {
				CHECK( Plugins::IsPlugin(pluginPath) );
			}
		}

		WHEN( "directory contains a 'sounds' folder" ) {
			auto pluginPath = tempDir / "sounds-plugin";
			std::filesystem::create_directories(pluginPath / "sounds");

			THEN( "IsPlugin returns true" ) {
				CHECK( Plugins::IsPlugin(pluginPath) );
			}
		}

		WHEN( "directory contains multiple asset folders" ) {
			auto pluginPath = tempDir / "multi-plugin";
			std::filesystem::create_directories(pluginPath / "data");
			std::filesystem::create_directories(pluginPath / "images");

			THEN( "IsPlugin returns true" ) {
				CHECK( Plugins::IsPlugin(pluginPath) );
			}
		}

		WHEN( "directory contains only a plugin.txt file" ) {
			auto pluginPath = tempDir / "txt-only-plugin";
			std::filesystem::create_directories(pluginPath);
			CreateFileWithContent(pluginPath / "plugin.txt", "name Test Plugin");

			THEN( "IsPlugin returns false (requires asset folder)" ) {
				CHECK_FALSE( Plugins::IsPlugin(pluginPath) );
			}
		}

		WHEN( "directory has no plugin content" ) {
			auto emptyPath = tempDir / "empty-dir";
			std::filesystem::create_directories(emptyPath);

			THEN( "IsPlugin returns false" ) {
				CHECK_FALSE( Plugins::IsPlugin(emptyPath) );
			}
		}

		WHEN( "path does not exist" ) {
			auto nonExistent = tempDir / "does-not-exist";

			THEN( "IsPlugin returns false" ) {
				CHECK_FALSE( Plugins::IsPlugin(nonExistent) );
			}
		}

		WHEN( "asset folders are empty" ) {
			auto pluginPath = tempDir / "empty-assets";
			std::filesystem::create_directories(pluginPath / "data");
			// data folder is empty

			THEN( "IsPlugin returns true (empty folders still count)" ) {
				CHECK( Plugins::IsPlugin(pluginPath) );
			}
		}
	}
}


// Test Plugin default values
SCENARIO( "Plugin has correct default values", "[Plugin][Defaults]" ) {
	GIVEN( "A default-constructed Plugin" ) {
		Plugin plugin;

		WHEN( "checking default values" ) {
			THEN( "enabled and currentState default to true" ) {
				CHECK( plugin.enabled );
				CHECK( plugin.currentState );
			}

			AND_THEN( "string fields are empty" ) {
				CHECK( plugin.name.empty() );
				CHECK( plugin.version.empty() );
				CHECK( plugin.aboutText.empty() );
			}

			AND_THEN( "sets are empty" ) {
				CHECK( plugin.authors.empty() );
				CHECK( plugin.tags.empty() );
				CHECK( plugin.dependencies.required.empty() );
				CHECK( plugin.dependencies.optional.empty() );
				CHECK( plugin.dependencies.conflicted.empty() );
				CHECK( plugin.dependencies.gameVersion.empty() );
			}
		}
	}
}


// Test boundary conditions for PluginDependencies
SCENARIO( "PluginDependencies handles boundary conditions", "[Plugin][PluginDependencies][Boundary]" ) {
	GIVEN( "A PluginDependencies object" ) {
		Plugin::PluginDependencies deps;

		WHEN( "all three sets have the same dependency" ) {
			deps.required.insert("same-plugin");
			deps.optional.insert("same-plugin");
			deps.conflicted.insert("same-plugin");

			THEN( "IsValid returns false due to conflict" ) {
				CHECK_FALSE( deps.IsValid() );
			}
		}

		WHEN( "many dependencies are present" ) {
			for(int i = 0; i < 100; ++i)
			{
				deps.required.insert("required-" + std::to_string(i));
				deps.optional.insert("optional-" + std::to_string(i));
			}

			THEN( "IsValid handles large sets" ) {
				CHECK( deps.IsValid() );
			}

			AND_THEN( "IsEmpty returns false" ) {
				CHECK_FALSE( deps.IsEmpty() );
			}
		}

		WHEN( "dependency names have special characters" ) {
			deps.required.insert("plugin-with-dashes");
			deps.required.insert("plugin_with_underscores");
			deps.required.insert("plugin.with.dots");

			THEN( "dependencies are stored correctly" ) {
				CHECK( deps.required.contains("plugin-with-dashes") );
				CHECK( deps.required.contains("plugin_with_underscores") );
				CHECK( deps.required.contains("plugin.with.dots") );
			}
		}
	}
}


// Test boundary conditions for CreateDescription
SCENARIO( "Plugin::CreateDescription handles boundary conditions", "[Plugin][CreateDescription][Boundary]" ) {
	GIVEN( "A Plugin object" ) {
		Plugin plugin;
		plugin.name = "test";

		WHEN( "authors contains many entries" ) {
			for(int i = 0; i < 50; ++i)
				plugin.authors.insert("Author " + std::to_string(i));

			THEN( "description contains all authors" ) {
				std::string desc = plugin.CreateDescription();
				CHECK( desc.find("Author 0") != std::string::npos );
				CHECK( desc.find("Author 49") != std::string::npos );
			}
		}

		WHEN( "about text is very long" ) {
			plugin.aboutText = std::string(10000, 'a');

			THEN( "description includes the full about text" ) {
				std::string desc = plugin.CreateDescription();
				CHECK( desc.length() >= 10000 );
			}
		}

		WHEN( "about text has multiple lines" ) {
			plugin.aboutText = "Line 1\nLine 2\nLine 3";

			THEN( "all lines are preserved" ) {
				std::string desc = plugin.CreateDescription();
				CHECK( desc.find("Line 1\nLine 2\nLine 3") != std::string::npos );
			}
		}
	}
}


// #endregion unit tests



} // test namespace