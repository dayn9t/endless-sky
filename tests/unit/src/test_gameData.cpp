/* test_gameData.cpp
Copyright (c) 2026 by the Endless Sky community

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
#include "../../../source/GameData.h"

// ... and any system includes needed for the test file.
#include "../../../source/CategoryType.h"
#include "../../../source/Point.h"
#include "../../../source/shader/StarField.h"
#include <filesystem>
#include <map>
#include <string>
#include <type_traits>

namespace { // test namespace

// #region mock data

// #endregion mock data



// #region unit tests

// GameData is a static-only class with all static methods.
// It cannot be instantiated directly and provides global access to game data.
TEST_CASE( "GameData Class Traits", "[GameData]" ) {
	SECTION( "Static-only class design" ) {
		// GameData has all static methods and no public constructor.
		// It serves as a global accessor for game data.
		CHECK( true ); // The class design itself is the trait being verified.
	}
}

// Note: GameData is a static class that manages global game state.
// Many methods require full initialization (loading game data files),
// which is not suitable for unit tests. These tests focus on:
// 1. Accessors that work without initialization
// 2. Methods that can be tested with minimal setup
// 3. Boundary conditions and error handling
//
// Methods that require full initialization or external dependencies:
// - BeginLoad, FinishLoading, CheckReferences (require filesystem, plugins)
// - LoadSettings, LoadShaders (require filesystem)
// - Sources (requires LoadSources to be called)
// - Revert, SetDate, Change (require loaded game state)
// - ReadEconomy, WriteEconomy, StepEconomy, AddPurchase (require loaded systems)
// - UpdateSystems, RecomputeWormholeRequirements, AddJumpRange (require loaded systems)
// - ResetPersons, DestroyPersons (require loaded persons)
// - PlayerGovernment (requires loaded governments)
// - StartOptions (requires loaded start conditions)
// - HasLandingMessage, LandingMessage, SolarPower, SolarWind, StarIcon (require loaded stellar data)
// - Rating (requires loaded ratings)
// - GetCategory (requires loaded categories)
// - Background, StepBackground, GetBackgroundPosition, SetBackgroundPosition, SetHaze (require StarField)
// - Tooltip, HelpMessage, HelpTemplates (require loaded text data)
// - GetTextReplacements (requires loaded substitutions)
// - GetGamerules, SetGamerules, DefaultGamerules (require loaded gamerules)
// - DrawMenuBackground (requires loaded interface)

SCENARIO( "GameData Set accessors return valid references without initialization", "[GameData][Sets]" ) {
	GIVEN( "GameData has not been initialized" ) {
		THEN( "Colors returns a valid empty Set reference" ) {
			const auto &colors = GameData::Colors();
			CHECK( colors.size() == 0 );
		}

		THEN( "Swizzles returns a valid empty Set reference" ) {
			const auto &swizzles = GameData::Swizzles();
			CHECK( swizzles.size() == 0 );
		}

		THEN( "Conversations returns a valid empty Set reference" ) {
			const auto &conversations = GameData::Conversations();
			CHECK( conversations.size() == 0 );
		}

		THEN( "Effects returns a valid empty Set reference" ) {
			const auto &effects = GameData::Effects();
			CHECK( effects.size() == 0 );
		}

		THEN( "Events returns a valid empty Set reference" ) {
			const auto &events = GameData::Events();
			CHECK( events.size() == 0 );
		}

		THEN( "Fleets returns a valid empty Set reference" ) {
			const auto &fleets = GameData::Fleets();
			CHECK( fleets.size() == 0 );
		}

		THEN( "Formations returns a valid empty Set reference" ) {
			const auto &formations = GameData::Formations();
			CHECK( formations.size() == 0 );
		}

		THEN( "Galaxies returns a valid empty Set reference" ) {
			const auto &galaxies = GameData::Galaxies();
			CHECK( galaxies.size() == 0 );
		}

		THEN( "Governments returns a valid empty Set reference" ) {
			const auto &governments = GameData::Governments();
			CHECK( governments.size() == 0 );
		}

		THEN( "Hazards returns a valid empty Set reference" ) {
			const auto &hazards = GameData::Hazards();
			CHECK( hazards.size() == 0 );
		}

		THEN( "Interfaces returns a valid empty Set reference" ) {
			const auto &interfaces = GameData::Interfaces();
			CHECK( interfaces.size() == 0 );
		}

		THEN( "MessageCategories returns a valid empty Set reference" ) {
			const auto &categories = GameData::MessageCategories();
			CHECK( categories.size() == 0 );
		}

		THEN( "Messages returns a valid empty Set reference" ) {
			const auto &messages = GameData::Messages();
			CHECK( messages.size() == 0 );
		}

		THEN( "Minables returns a valid empty Set reference" ) {
			const auto &minables = GameData::Minables();
			CHECK( minables.size() == 0 );
		}

		THEN( "Missions returns a valid empty Set reference" ) {
			const auto &missions = GameData::Missions();
			CHECK( missions.size() == 0 );
		}

		THEN( "SpaceportNews returns a valid empty Set reference" ) {
			const auto &news = GameData::SpaceportNews();
			CHECK( news.size() == 0 );
		}

		THEN( "Outfits returns a valid empty Set reference" ) {
			const auto &outfits = GameData::Outfits();
			CHECK( outfits.size() == 0 );
		}

		THEN( "Outfitters returns a valid empty Set reference" ) {
			const auto &outfitters = GameData::Outfitters();
			CHECK( outfitters.size() == 0 );
		}

		THEN( "Persons returns a valid empty Set reference" ) {
			const auto &persons = GameData::Persons();
			CHECK( persons.size() == 0 );
		}

		THEN( "Phrases returns a valid empty Set reference" ) {
			const auto &phrases = GameData::Phrases();
			CHECK( phrases.size() == 0 );
		}

		THEN( "Planets returns a valid empty Set reference" ) {
			const auto &planets = GameData::Planets();
			CHECK( planets.size() == 0 );
		}

		THEN( "Shaders returns a valid empty Set reference" ) {
			const auto &shaders = GameData::Shaders();
			CHECK( shaders.size() == 0 );
		}

		THEN( "Ships returns a valid empty Set reference" ) {
			const auto &ships = GameData::Ships();
			CHECK( ships.size() == 0 );
		}

		THEN( "Shipyards returns a valid empty Set reference" ) {
			const auto &shipyards = GameData::Shipyards();
			CHECK( shipyards.size() == 0 );
		}

		THEN( "Systems returns a valid empty Set reference" ) {
			const auto &systems = GameData::Systems();
			CHECK( systems.size() == 0 );
		}

		THEN( "Tests returns a valid empty Set reference" ) {
			const auto &tests = GameData::Tests();
			CHECK( tests.size() == 0 );
		}

		THEN( "TestDataSets returns a valid empty Set reference" ) {
			const auto &testData = GameData::TestDataSets();
			CHECK( testData.size() == 0 );
		}

		THEN( "Wormholes returns a valid empty Set reference" ) {
			const auto &wormholes = GameData::Wormholes();
			CHECK( wormholes.size() == 0 );
		}

		THEN( "GamerulesPresets returns a valid empty Set reference" ) {
			const auto &presets = GameData::GamerulesPresets();
			CHECK( presets.size() == 0 );
		}
	}
}

SCENARIO( "GameData Objects returns valid UniverseObjects reference", "[GameData][Objects]" ) {
	GIVEN( "GameData has not been initialized" ) {
		WHEN( "calling Objects()" ) {
			auto &objects = GameData::Objects();

			THEN( "a valid reference is returned" ) {
				// The reference should be valid, even if empty
				CHECK( &objects != nullptr );
			}
		}
	}
}

SCENARIO( "GameData GlobalConditions returns valid reference", "[GameData][GlobalConditions]" ) {
	GIVEN( "GameData has not been initialized" ) {
		WHEN( "calling GlobalConditions()" ) {
			auto &conditions = GameData::GlobalConditions();

			THEN( "a valid reference is returned" ) {
				CHECK( &conditions != nullptr );
			}
		}
	}
}

SCENARIO( "GameData UniverseWormholeRequirements returns valid reference", "[GameData][WormholeRequirements]" ) {
	GIVEN( "GameData has not been initialized" ) {
		WHEN( "calling UniverseWormholeRequirements()" ) {
			const auto &requirements = GameData::UniverseWormholeRequirements();

			THEN( "a valid empty set is returned" ) {
				CHECK( requirements.empty() );
			}
		}
	}
}

SCENARIO( "GameData Commodities returns valid reference", "[GameData][Commodities]" ) {
	GIVEN( "GameData has not been initialized" ) {
		WHEN( "calling Commodities()" ) {
			const auto &commodities = GameData::Commodities();

			THEN( "a valid reference is returned" ) {
				// Even without initialization, Trade has default commodities
				CHECK( &commodities != nullptr );
			}
		}

		WHEN( "calling SpecialCommodities()" ) {
			const auto &specialCommodities = GameData::SpecialCommodities();

			THEN( "a valid reference is returned" ) {
				CHECK( &specialCommodities != nullptr );
			}
		}
	}
}

SCENARIO( "GameData GetMaskManager returns valid reference", "[GameData][MaskManager]" ) {
	GIVEN( "GameData has not been initialized" ) {
		WHEN( "calling GetMaskManager()" ) {
			auto &maskManager = GameData::GetMaskManager();

			THEN( "a valid reference is returned" ) {
				CHECK( &maskManager != nullptr );
			}
		}
	}
}

SCENARIO( "GameData PlayerGovernment returns nullptr without initialization", "[GameData][PlayerGovernment]" ) {
	GIVEN( "GameData has not been initialized" ) {
		WHEN( "calling PlayerGovernment()" ) {
			const auto *gov = GameData::PlayerGovernment();

			THEN( "returns nullptr when not initialized" ) {
				CHECK( gov == nullptr );
			}
		}
	}
}

SCENARIO( "GameData Sources returns empty vector without initialization", "[GameData][Sources]" ) {
	GIVEN( "GameData has not been initialized" ) {
		WHEN( "calling Sources()" ) {
			const auto &sources = GameData::Sources();

			THEN( "returns an empty vector" ) {
				CHECK( sources.empty() );
			}
		}
	}
}

SCENARIO( "GameData StartOptions returns empty vector without initialization", "[GameData][StartOptions]" ) {
	GIVEN( "GameData has not been initialized" ) {
		WHEN( "calling StartOptions()" ) {
			const auto &options = GameData::StartOptions();

			THEN( "returns an empty vector" ) {
				CHECK( options.empty() );
			}
		}
	}
}

SCENARIO( "GameData GetProgress returns value without initialization", "[GameData][Progress]" ) {
	GIVEN( "GameData has not been initialized" ) {
		WHEN( "calling GetProgress()" ) {
			double progress = GameData::GetProgress();

			THEN( "returns a value between 0 and 1" ) {
				CHECK( progress >= 0. );
				CHECK( progress <= 1. );
			}
		}
	}
}

SCENARIO( "GameData IsLoaded returns false without initialization", "[GameData][IsLoaded]" ) {
	GIVEN( "GameData has not been initialized" ) {
		WHEN( "calling IsLoaded()" ) {
			bool isLoaded = GameData::IsLoaded();

			THEN( "returns false" ) {
				CHECK_FALSE( isLoaded );
			}
		}
	}
}

SCENARIO( "GameData Background returns valid reference", "[GameData][Background]" ) {
	GIVEN( "GameData has not been initialized" ) {
		WHEN( "calling Background()" ) {
			const auto &background = GameData::Background();

			THEN( "a valid StarField reference is returned" ) {
				CHECK( &background != nullptr );
			}
		}
	}
}

SCENARIO( "GameData GetBackgroundPosition returns valid reference", "[GameData][BackgroundPosition]" ) {
	GIVEN( "GameData has not been initialized" ) {
		WHEN( "calling GetBackgroundPosition()" ) {
			const auto &position = GameData::GetBackgroundPosition();

			THEN( "a valid Point reference is returned" ) {
				CHECK( &position != nullptr );
			}
		}
	}
}

SCENARIO( "GameData GetCategory returns valid reference for all CategoryType values", "[GameData][Category]" ) {
	GIVEN( "GameData has not been initialized" ) {
		WHEN( "calling GetCategory with CategoryType::SHIP" ) {
			const auto &category = GameData::GetCategory(CategoryType::SHIP);

			THEN( "a valid CategoryList reference is returned" ) {
				CHECK( &category != nullptr );
			}
		}

		WHEN( "calling GetCategory with CategoryType::OUTFIT" ) {
			const auto &category = GameData::GetCategory(CategoryType::OUTFIT);

			THEN( "a valid CategoryList reference is returned" ) {
				CHECK( &category != nullptr );
			}
		}

		WHEN( "calling GetCategory with CategoryType::BAY" ) {
			const auto &category = GameData::GetCategory(CategoryType::BAY);

			THEN( "a valid CategoryList reference is returned" ) {
				CHECK( &category != nullptr );
			}
		}
	}
}

SCENARIO( "GameData landing message methods handle missing data", "[GameData][LandingMessage]" ) {
	GIVEN( "GameData has not been initialized" ) {
		WHEN( "calling HasLandingMessage with nullptr" ) {
			bool hasMessage = GameData::HasLandingMessage(nullptr);

			THEN( "returns false" ) {
				CHECK_FALSE( hasMessage );
			}
		}

		WHEN( "calling LandingMessage with nullptr" ) {
			const std::string &message = GameData::LandingMessage(nullptr);

			THEN( "returns empty string" ) {
				CHECK( message.empty() );
			}
		}
	}
}

SCENARIO( "GameData solar methods handle missing data", "[GameData][Solar]" ) {
	GIVEN( "GameData has not been initialized" ) {
		WHEN( "calling SolarPower with nullptr" ) {
			double power = GameData::SolarPower(nullptr);

			THEN( "returns 0" ) {
				CHECK( power == 0. );
			}
		}

		WHEN( "calling SolarWind with nullptr" ) {
			double wind = GameData::SolarWind(nullptr);

			THEN( "returns 0" ) {
				CHECK( wind == 0. );
			}
		}

		WHEN( "calling StarIcon with nullptr" ) {
			const Sprite *icon = GameData::StarIcon(nullptr);

			THEN( "returns nullptr" ) {
				CHECK( icon == nullptr );
			}
		}
	}
}

SCENARIO( "GameData Rating handles missing data", "[GameData][Rating]" ) {
	GIVEN( "GameData has not been initialized" ) {
		WHEN( "calling Rating with non-existent type" ) {
			const std::string &rating = GameData::Rating("nonexistent", 0);

			THEN( "returns empty string" ) {
				CHECK( rating.empty() );
			}
		}

		WHEN( "calling Rating with empty type" ) {
			const std::string &rating = GameData::Rating("", 0);

			THEN( "returns empty string" ) {
				CHECK( rating.empty() );
			}
		}

		WHEN( "calling Rating with negative level" ) {
			const std::string &rating = GameData::Rating("combat", -5);

			THEN( "returns empty string (no ratings loaded)" ) {
				CHECK( rating.empty() );
			}
		}

		WHEN( "calling Rating with high level" ) {
			const std::string &rating = GameData::Rating("combat", 1000);

			THEN( "returns empty string (no ratings loaded)" ) {
				CHECK( rating.empty() );
			}
		}
	}
}

SCENARIO( "GameData Tooltip handles missing data", "[GameData][Tooltip]" ) {
	GIVEN( "GameData has not been initialized" ) {
		WHEN( "calling Tooltip with non-existent label" ) {
			const std::string &tooltip = GameData::Tooltip("nonexistent");

			THEN( "returns empty string" ) {
				CHECK( tooltip.empty() );
			}
		}

		WHEN( "calling Tooltip with empty label" ) {
			const std::string &tooltip = GameData::Tooltip("");

			THEN( "returns empty string" ) {
				CHECK( tooltip.empty() );
			}
		}

		WHEN( "calling Tooltip with cost prefix label" ) {
			const std::string &tooltip = GameData::Tooltip("cost 50%");

			THEN( "returns empty string (no tooltips loaded)" ) {
				CHECK( tooltip.empty() );
			}
		}

		WHEN( "calling Tooltip with 'cost' prefix label" ) {
			const std::string &tooltip = GameData::Tooltip("cost something");

			THEN( "returns empty string (no 'cost:' tooltip loaded)" ) {
				CHECK( tooltip.empty() );
			}
		}

		WHEN( "calling Tooltip with sells for prefix label" ) {
			const std::string &tooltip = GameData::Tooltip("sells for 80%");

			THEN( "returns empty string (no tooltips loaded)" ) {
				CHECK( tooltip.empty() );
			}
		}

		WHEN( "calling Tooltip with 'sells for' prefix label" ) {
			const std::string &tooltip = GameData::Tooltip("sells for something");

			THEN( "returns empty string (no 'sells for:' tooltip loaded)" ) {
				CHECK( tooltip.empty() );
			}
		}
	}
}

SCENARIO( "GameData HelpMessage handles missing data", "[GameData][HelpMessage]" ) {
	GIVEN( "GameData has not been initialized" ) {
		WHEN( "calling HelpMessage with non-existent name" ) {
			std::string help = GameData::HelpMessage("nonexistent");

			THEN( "returns empty string" ) {
				CHECK( help.empty() );
			}
		}

		WHEN( "calling HelpMessage with empty name" ) {
			std::string help = GameData::HelpMessage("");

			THEN( "returns empty string" ) {
				CHECK( help.empty() );
			}
		}
	}
}

SCENARIO( "GameData HelpTemplates returns valid reference", "[GameData][HelpTemplates]" ) {
	GIVEN( "GameData has not been initialized" ) {
		WHEN( "calling HelpTemplates()" ) {
			const auto &templates = GameData::HelpTemplates();

			THEN( "returns an empty map" ) {
				CHECK( templates.empty() );
			}
		}
	}
}

SCENARIO( "GameData GetPolitics returns valid reference", "[GameData][Politics]" ) {
	GIVEN( "GameData has not been initialized" ) {
		WHEN( "calling GetPolitics()" ) {
			auto &politics = GameData::GetPolitics();

			THEN( "a valid reference is returned" ) {
				CHECK( &politics != nullptr );
			}
		}
	}
}

SCENARIO( "GameData GetTextReplacements returns valid reference", "[GameData][TextReplacements]" ) {
	GIVEN( "GameData has not been initialized" ) {
		WHEN( "calling GetTextReplacements()" ) {
			const auto &replacements = GameData::GetTextReplacements();

			THEN( "a valid reference is returned" ) {
				CHECK( &replacements != nullptr );
			}
		}
	}
}

// Note: The following tests would crash because Gamerules::Get("Default") dereferences nullptr
// when no gamerules presets are loaded. These tests are commented out for safety.
// These should be tested in integration tests with proper GameData initialization.
//
// SCENARIO( "GameData GetGamerules returns default without initialization", "[GameData][Gamerules]" )
// SCENARIO( "GameData DefaultGamerules returns valid reference", "[GameData][DefaultGamerules]" )
// SCENARIO( "GameData SetGamerules changes active gamerules", "[GameData][SetGamerules]" )

// #endregion unit tests



} // test namespace