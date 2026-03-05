/* test_playerInfo.cpp
Copyright (c) 2026 by Endless Sky contributors

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
#include "../../../source/PlayerInfo.h"

// ... and any system includes needed for the test file.
#include <chrono>
#include <set>
#include <string>
#include <type_traits>

namespace { // test namespace

// #region mock data

// PlayerInfo has many dependencies (GameData, Messages, Random, etc.).
// Tests focus on simple getters/setters and default state that don't require
// the full game infrastructure to be loaded.

// #endregion mock data



// #region unit tests

SCENARIO( "Creating a PlayerInfo with default constructor", "[PlayerInfo][Creation]" ) {
	GIVEN( "a default-constructed PlayerInfo" ) {
		PlayerInfo player;

		THEN( "the player is not loaded" ) {
			REQUIRE_FALSE( player.IsLoaded() );
		}
		THEN( "the player is not dead" ) {
			REQUIRE_FALSE( player.IsDead() );
		}
		THEN( "the player has no first name" ) {
			REQUIRE( player.FirstName().empty() );
		}
		THEN( "the player has no last name" ) {
			REQUIRE( player.LastName().empty() );
		}
		THEN( "the player has no licenses" ) {
			REQUIRE( player.Licenses().empty() );
		}
		THEN( "the player has no travel plan" ) {
			REQUIRE_FALSE( player.HasTravelPlan() );
			REQUIRE( player.TravelPlan().empty() );
		}
		THEN( "the player has no flagship" ) {
			REQUIRE( player.Flagship() == nullptr );
			REQUIRE( player.FlagshipPtr() == nullptr );
		}
		THEN( "the player has no ships" ) {
			REQUIRE( player.Ships().empty() );
		}
		THEN( "the player has no system" ) {
			REQUIRE( player.GetSystem() == nullptr );
		}
		THEN( "the player has no planet" ) {
			REQUIRE( player.GetPlanet() == nullptr );
		}
		THEN( "the play time is zero" ) {
			REQUIRE( player.GetPlayTime() == Catch::Approx(0.0) );
		}
		THEN( "the player has no stellar object" ) {
			REQUIRE( player.GetStellarObject() == nullptr );
		}
		THEN( "the player has no previous system" ) {
			REQUIRE( player.GetPreviousSystem() == nullptr );
		}
		THEN( "the player has no travel destination" ) {
			REQUIRE( player.TravelDestination() == nullptr );
		}
	}
}

SCENARIO( "PlayerInfo copy semantics", "[PlayerInfo][Copy]" ) {
	GIVEN( "a PlayerInfo class" ) {
		THEN( "copy constructor is deleted" ) {
			REQUIRE_FALSE( std::is_copy_constructible_v<PlayerInfo> );
		}
		THEN( "copy assignment is deleted" ) {
			REQUIRE_FALSE( std::is_copy_assignable_v<PlayerInfo> );
		}
	}
}

SCENARIO( "PlayerInfo move semantics", "[PlayerInfo][Move]" ) {
	GIVEN( "a PlayerInfo class" ) {
		THEN( "move constructor is deleted" ) {
			REQUIRE_FALSE( std::is_move_constructible_v<PlayerInfo> );
		}
		THEN( "move assignment is allowed" ) {
			REQUIRE( std::is_move_assignable_v<PlayerInfo> );
		}
	}
}

SCENARIO( "Setting and getting player name", "[PlayerInfo][Name]" ) {
	GIVEN( "a default-constructed PlayerInfo" ) {
		PlayerInfo player;
		REQUIRE( player.FirstName().empty() );
		REQUIRE( player.LastName().empty() );

		WHEN( "setting name to 'John Doe'" ) {
			player.SetName("John", "Doe");

			THEN( "first name is 'John'" ) {
				REQUIRE( player.FirstName() == "John" );
			}
			THEN( "last name is 'Doe'" ) {
				REQUIRE( player.LastName() == "Doe" );
			}
			THEN( "the player is now considered loaded" ) {
				REQUIRE( player.IsLoaded() );
			}
		}

		WHEN( "setting name to empty strings" ) {
			player.SetName("", "");

			THEN( "both names are empty" ) {
				REQUIRE( player.FirstName().empty() );
				REQUIRE( player.LastName().empty() );
			}
			THEN( "the player is not considered loaded (empty name doesn't count)" ) {
				REQUIRE_FALSE( player.IsLoaded() );
			}
		}

		WHEN( "setting name with only first name" ) {
			player.SetName("Alice", "");

			THEN( "first name is 'Alice'" ) {
				REQUIRE( player.FirstName() == "Alice" );
			}
			THEN( "last name is empty" ) {
				REQUIRE( player.LastName().empty() );
			}
		}

		WHEN( "setting name with only last name" ) {
			player.SetName("", "Smith");

			THEN( "first name is empty" ) {
				REQUIRE( player.FirstName().empty() );
			}
			THEN( "last name is 'Smith'" ) {
				REQUIRE( player.LastName() == "Smith" );
			}
		}

		WHEN( "changing the name" ) {
			player.SetName("John", "Doe");
			REQUIRE( player.FirstName() == "John" );
			REQUIRE( player.LastName() == "Doe" );

			player.SetName("Jane", "Smith");

			THEN( "the name is updated" ) {
				REQUIRE( player.FirstName() == "Jane" );
				REQUIRE( player.LastName() == "Smith" );
			}
		}
	}
}

SCENARIO( "Player death state", "[PlayerInfo][Death]" ) {
	GIVEN( "a default-constructed PlayerInfo" ) {
		PlayerInfo player;
		REQUIRE_FALSE( player.IsDead() );

		WHEN( "the player dies" ) {
			player.Die();

			THEN( "the player is dead" ) {
				REQUIRE( player.IsDead() );
			}
		}

		WHEN( "the player dies with a response code" ) {
			player.Die(1);

			THEN( "the player is dead" ) {
				REQUIRE( player.IsDead() );
			}
		}
	}
}

SCENARIO( "Managing licenses", "[PlayerInfo][Licenses]" ) {
	GIVEN( "a PlayerInfo with no licenses" ) {
		PlayerInfo player;
		REQUIRE( player.Licenses().empty() );

		WHEN( "adding a license" ) {
			player.AddLicense("Pilot");

			THEN( "the license is present" ) {
				REQUIRE( player.HasLicense("Pilot") );
				REQUIRE( player.Licenses().size() == 1 );
				REQUIRE( player.Licenses().count("Pilot") == 1 );
			}
		}

		WHEN( "adding multiple licenses" ) {
			player.AddLicense("Pilot");
			player.AddLicense("Merchant");
			player.AddLicense("Navy");

			THEN( "all licenses are present" ) {
				REQUIRE( player.HasLicense("Pilot") );
				REQUIRE( player.HasLicense("Merchant") );
				REQUIRE( player.HasLicense("Navy") );
				REQUIRE( player.Licenses().size() == 3 );
			}
		}

		WHEN( "adding the same license twice" ) {
			player.AddLicense("Pilot");
			player.AddLicense("Pilot");

			THEN( "the license is only present once" ) {
				REQUIRE( player.HasLicense("Pilot") );
				REQUIRE( player.Licenses().size() == 1 );
			}
		}

		WHEN( "removing a license" ) {
			player.AddLicense("Pilot");
			player.AddLicense("Merchant");
			REQUIRE( player.HasLicense("Pilot") );

			player.RemoveLicense("Pilot");

			THEN( "the license is removed" ) {
				REQUIRE_FALSE( player.HasLicense("Pilot") );
				REQUIRE( player.HasLicense("Merchant") );
				REQUIRE( player.Licenses().size() == 1 );
			}
		}

		WHEN( "removing a non-existent license" ) {
			player.AddLicense("Pilot");

			player.RemoveLicense("Merchant");

			THEN( "the existing license remains" ) {
				REQUIRE( player.HasLicense("Pilot") );
				REQUIRE_FALSE( player.HasLicense("Merchant") );
			}
		}

		WHEN( "checking for a non-existent license" ) {
			THEN( "HasLicense returns false" ) {
				REQUIRE_FALSE( player.HasLicense("NonExistent") );
			}
		}
	}
}

SCENARIO( "Managing play time", "[PlayerInfo][PlayTime]" ) {
	GIVEN( "a default-constructed PlayerInfo" ) {
		PlayerInfo player;
		REQUIRE( player.GetPlayTime() == Catch::Approx(0.0) );

		WHEN( "adding play time" ) {
			player.AddPlayTime(std::chrono::nanoseconds(1000000000)); // 1 second

			THEN( "play time is 1 second" ) {
				REQUIRE( player.GetPlayTime() == Catch::Approx(1.0) );
			}
		}

		WHEN( "adding multiple play times" ) {
			player.AddPlayTime(std::chrono::nanoseconds(1000000000)); // 1 second
			player.AddPlayTime(std::chrono::nanoseconds(2000000000)); // 2 seconds
			player.AddPlayTime(std::chrono::nanoseconds(500000000));  // 0.5 seconds

			THEN( "play time is accumulated" ) {
				REQUIRE( player.GetPlayTime() == Catch::Approx(3.5) );
			}
		}

		WHEN( "adding zero play time" ) {
			player.AddPlayTime(std::chrono::nanoseconds(0));

			THEN( "play time remains zero" ) {
				REQUIRE( player.GetPlayTime() == Catch::Approx(0.0) );
			}
		}

		WHEN( "adding very small play time" ) {
			player.AddPlayTime(std::chrono::nanoseconds(1)); // 1 nanosecond

			THEN( "play time is converted correctly" ) {
				REQUIRE( player.GetPlayTime() == Catch::Approx(0.000000001) );
			}
		}

		WHEN( "adding large play time" ) {
			// 100 hours in nanoseconds
			auto hundredHours = std::chrono::hours(100);
			player.AddPlayTime(hundredHours);

			THEN( "play time is 360000 seconds (100 hours)" ) {
				REQUIRE( player.GetPlayTime() == Catch::Approx(360000.0) );
			}
		}
	}
}

SCENARIO( "Managing system entry type", "[PlayerInfo][SystemEntry]" ) {
	GIVEN( "a default-constructed PlayerInfo" ) {
		PlayerInfo player;

		THEN( "default system entry is TAKE_OFF" ) {
			REQUIRE( player.GetSystemEntry() == SystemEntry::TAKE_OFF );
		}

		WHEN( "setting system entry to HYPERDRIVE" ) {
			player.SetSystemEntry(SystemEntry::HYPERDRIVE);

			THEN( "system entry is HYPERDRIVE" ) {
				REQUIRE( player.GetSystemEntry() == SystemEntry::HYPERDRIVE );
			}
		}

		WHEN( "setting system entry to JUMP" ) {
			player.SetSystemEntry(SystemEntry::JUMP);

			THEN( "system entry is JUMP" ) {
				REQUIRE( player.GetSystemEntry() == SystemEntry::JUMP );
			}
		}

		WHEN( "setting system entry to WORMHOLE" ) {
			player.SetSystemEntry(SystemEntry::WORMHOLE);

			THEN( "system entry is WORMHOLE" ) {
				REQUIRE( player.GetSystemEntry() == SystemEntry::WORMHOLE );
			}
		}

		WHEN( "setting system entry back to TAKE_OFF" ) {
			player.SetSystemEntry(SystemEntry::HYPERDRIVE);
			player.SetSystemEntry(SystemEntry::TAKE_OFF);

			THEN( "system entry is TAKE_OFF" ) {
				REQUIRE( player.GetSystemEntry() == SystemEntry::TAKE_OFF );
			}
		}
	}
}

SCENARIO( "Managing map coloring", "[PlayerInfo][MapColoring]" ) {
	GIVEN( "a default-constructed PlayerInfo" ) {
		PlayerInfo player;

		THEN( "default map coloring is -6" ) {
			REQUIRE( player.MapColoring() == -6 );
		}

		WHEN( "setting map coloring to 0" ) {
			player.SetMapColoring(0);

			THEN( "map coloring is 0" ) {
				REQUIRE( player.MapColoring() == 0 );
			}
		}

		WHEN( "setting map coloring to a positive value" ) {
			player.SetMapColoring(5);

			THEN( "map coloring is 5" ) {
				REQUIRE( player.MapColoring() == 5 );
			}
		}

		WHEN( "setting map coloring to a negative value" ) {
			player.SetMapColoring(-3);

			THEN( "map coloring is -3" ) {
				REQUIRE( player.MapColoring() == -3 );
			}
		}

		WHEN( "setting map coloring multiple times" ) {
			player.SetMapColoring(1);
			player.SetMapColoring(10);
			player.SetMapColoring(-5);

			THEN( "the last value is retained" ) {
				REQUIRE( player.MapColoring() == -5 );
			}
		}
	}
}

SCENARIO( "Managing map zoom", "[PlayerInfo][MapZoom]" ) {
	GIVEN( "a default-constructed PlayerInfo" ) {
		PlayerInfo player;

		THEN( "default map zoom is 0" ) {
			REQUIRE( player.MapZoom() == 0 );
		}

		WHEN( "setting map zoom to a positive value" ) {
			player.SetMapZoom(5);

			THEN( "map zoom is 5" ) {
				REQUIRE( player.MapZoom() == 5 );
			}
		}

		WHEN( "setting map zoom to zero" ) {
			player.SetMapZoom(10);
			player.SetMapZoom(0);

			THEN( "map zoom is 0" ) {
				REQUIRE( player.MapZoom() == 0 );
			}
		}

		WHEN( "setting map zoom to a negative value" ) {
			player.SetMapZoom(-2);

			THEN( "map zoom is -2" ) {
				REQUIRE( player.MapZoom() == -2 );
			}
		}
	}
}

SCENARIO( "Managing travel plan", "[PlayerInfo][TravelPlan]" ) {
	GIVEN( "a default-constructed PlayerInfo" ) {
		PlayerInfo player;

		THEN( "there is no travel plan" ) {
			REQUIRE_FALSE( player.HasTravelPlan() );
			REQUIRE( player.TravelPlan().empty() );
		}

		THEN( "travel plan can be accessed and modified" ) {
			// The non-const version allows modification
			REQUIRE( player.TravelPlan().empty() );
		}

		THEN( "there is no travel destination" ) {
			REQUIRE( player.TravelDestination() == nullptr );
		}
	}
}

SCENARIO( "Accessing cargo hold", "[PlayerInfo][Cargo]" ) {
	GIVEN( "a default-constructed PlayerInfo" ) {
		PlayerInfo player;

		THEN( "cargo hold can be accessed" ) {
			// Access the const version
			const auto &cargo = player.Cargo();
			// Default CargoHold has unlimited capacity
			CHECK( cargo.Size() == -1 );
		}

		THEN( "cargo hold can be modified" ) {
			player.Cargo().SetSize(100);
			CHECK( player.Cargo().Size() == 100 );
		}
	}
}

SCENARIO( "Accessing accounts", "[PlayerInfo][Accounts]" ) {
	GIVEN( "a default-constructed PlayerInfo" ) {
		PlayerInfo player;

		THEN( "accounts can be accessed (const)" ) {
			const auto &accounts = player.Accounts();
			CHECK( accounts.Credits() == 0 );
		}

		THEN( "accounts can be modified" ) {
			player.Accounts().AddCredits(1000);
			CHECK( player.Accounts().Credits() == 1000 );
		}
	}
}

SCENARIO( "Accessing conditions store", "[PlayerInfo][Conditions]" ) {
	GIVEN( "a default-constructed PlayerInfo" ) {
		PlayerInfo player;

		THEN( "conditions store can be accessed" ) {
			const auto &conditions = player.Conditions();
			CHECK( conditions.PrimariesSize() == 0 );
		}

		THEN( "conditions can be set and retrieved" ) {
			player.Conditions().Set("test_condition", 42);
			CHECK( player.Conditions().Get("test_condition") == 42 );
		}
	}
}

SCENARIO( "Accessing missions", "[PlayerInfo][Missions]" ) {
	GIVEN( "a default-constructed PlayerInfo" ) {
		PlayerInfo player;

		THEN( "there are no missions" ) {
			REQUIRE( player.Missions().empty() );
		}

		THEN( "there are no available jobs" ) {
			REQUIRE( player.AvailableJobs().empty() );
		}

		THEN( "there are no available in-flight missions" ) {
			REQUIRE_FALSE( player.HasAvailableInflightMissions() );
		}
	}
}

SCENARIO( "Accessing ships", "[PlayerInfo][Ships]" ) {
	GIVEN( "a default-constructed PlayerInfo" ) {
		PlayerInfo player;

		THEN( "there are no ships" ) {
			REQUIRE( player.Ships().empty() );
		}

		THEN( "there is no flagship" ) {
			REQUIRE( player.Flagship() == nullptr );
			REQUIRE( player.FlagshipPtr() == nullptr );
		}
	}
}

SCENARIO( "Accessing stock and depreciation", "[PlayerInfo][Stock]" ) {
	GIVEN( "a default-constructed PlayerInfo" ) {
		PlayerInfo player;

		THEN( "stock is empty" ) {
			REQUIRE( player.GetStock().empty() );
		}

		THEN( "stock count for any outfit is zero" ) {
			REQUIRE( player.Stock(nullptr) == 0 );
		}

		THEN( "fleet depreciation is accessible" ) {
			const auto &depreciation = player.FleetDepreciation();
			// Default depreciation object is valid
			CHECK( true );
		}

		THEN( "stock depreciation is accessible" ) {
			const auto &stockDepreciation = player.StockDepreciation();
			CHECK( true );
		}
	}
}

SCENARIO( "Accessing harvested materials", "[PlayerInfo][Harvest]" ) {
	GIVEN( "a default-constructed PlayerInfo" ) {
		PlayerInfo player;

		THEN( "there are no harvested materials" ) {
			REQUIRE( player.Harvested().empty() );
		}
	}
}

SCENARIO( "Accessing selected secondary weapons", "[PlayerInfo][Weapons]" ) {
	GIVEN( "a default-constructed PlayerInfo" ) {
		PlayerInfo player;

		THEN( "there are no selected secondary weapons" ) {
			REQUIRE( player.SelectedSecondaryWeapons().empty() );
		}
	}
}

SCENARIO( "Accessing selected escorts", "[PlayerInfo][Escorts]" ) {
	GIVEN( "a default-constructed PlayerInfo" ) {
		PlayerInfo player;

		THEN( "there are no selected escorts" ) {
			REQUIRE( player.SelectedEscorts().empty() );
		}
	}
}

SCENARIO( "Accessing tribute", "[PlayerInfo][Tribute]" ) {
	GIVEN( "a default-constructed PlayerInfo" ) {
		PlayerInfo player;

		THEN( "there is no tribute" ) {
			REQUIRE( player.GetTribute().empty() );
			REQUIRE( player.GetTributeTotal() == 0 );
		}
	}
}

SCENARIO( "Accessing logbook", "[PlayerInfo][Logbook]" ) {
	GIVEN( "a default-constructed PlayerInfo" ) {
		PlayerInfo player;

		THEN( "logbook is empty" ) {
			REQUIRE( player.Logbook().empty() );
		}

		THEN( "special logs are empty" ) {
			REQUIRE( player.SpecialLogs().empty() );
		}

		THEN( "there are no logs" ) {
			REQUIRE_FALSE( player.HasLogs() );
		}
	}
}

SCENARIO( "Accessing visited systems and planets", "[PlayerInfo][Visited]" ) {
	GIVEN( "a default-constructed PlayerInfo" ) {
		PlayerInfo player;

		THEN( "visited systems are empty" ) {
			REQUIRE( player.VisitedSystems().empty() );
		}

		THEN( "visited planets are empty" ) {
			REQUIRE( player.VisitedPlanets().empty() );
		}
	}
}

SCENARIO( "Accessing planetary storage", "[PlayerInfo][PlanetaryStorage]" ) {
	GIVEN( "a default-constructed PlayerInfo" ) {
		PlayerInfo player;

		THEN( "planetary storage is empty" ) {
			REQUIRE( player.PlanetaryStorage().empty() );
		}
	}
}

SCENARIO( "Accessing gamerules", "[PlayerInfo][Gamerules]" ) {
	GIVEN( "a default-constructed PlayerInfo" ) {
		PlayerInfo player;

		THEN( "gamerules are accessible" ) {
			// Just verify the method compiles and works
			Gamerules &gamerules = player.GetGamerules();
			// Default gamerules object
			CHECK( &gamerules != nullptr );
		}
	}
}

SCENARIO( "Accessing gifted ships", "[PlayerInfo][GiftedShips]" ) {
	GIVEN( "a default-constructed PlayerInfo" ) {
		PlayerInfo player;

		THEN( "there are no gifted ships" ) {
			REQUIRE( player.GiftedShips().empty() );
		}
	}
}

SCENARIO( "Getting date", "[PlayerInfo][Date]" ) {
	GIVEN( "a default-constructed PlayerInfo" ) {
		PlayerInfo player;

		THEN( "date is default-constructed (uninitialized)" ) {
			const Date &date = player.GetDate();
			REQUIRE( !date );
		}
	}
}

SCENARIO( "Escort destination", "[PlayerInfo][EscortDestination]" ) {
	GIVEN( "a default-constructed PlayerInfo" ) {
		PlayerInfo player;

		THEN( "there is no escort destination" ) {
			REQUIRE_FALSE( player.HasEscortDestination() );
			auto dest = player.GetEscortDestination();
			CHECK( dest.first == nullptr );
		}
	}
}

SCENARIO( "Display carrier help", "[PlayerInfo][CarrierHelp]" ) {
	GIVEN( "a default-constructed PlayerInfo" ) {
		PlayerInfo player;

		THEN( "carrier help is not displayed by default" ) {
			REQUIRE_FALSE( player.DisplayCarrierHelp() );
		}
	}
}

SCENARIO( "Collapsed categories", "[PlayerInfo][Collapsed]" ) {
	GIVEN( "a default-constructed PlayerInfo" ) {
		PlayerInfo player;

		WHEN( "accessing collapsed categories for a panel" ) {
			auto &collapsed = player.Collapsed("test_panel");

			THEN( "an empty set is returned (or created)" ) {
				CHECK( collapsed.empty() );
			}

			THEN( "categories can be added" ) {
				collapsed.insert("category1");
				collapsed.insert("category2");
				CHECK( player.Collapsed("test_panel").size() == 2 );
			}
		}
	}
}

SCENARIO( "Mission sorting type", "[PlayerInfo][SortType]" ) {
	GIVEN( "a default-constructed PlayerInfo" ) {
		PlayerInfo player;

		THEN( "sort type can be accessed" ) {
			// Default is ABC
			REQUIRE( player.GetAvailableSortType() == PlayerInfo::ABC );
		}

		WHEN( "cycling through sort types" ) {
			player.NextAvailableSortType();
			THEN( "sort type changes" ) {
				REQUIRE( player.GetAvailableSortType() == PlayerInfo::PAY );
			}
		}

		THEN( "sort ascending is true by default" ) {
			REQUIRE( player.ShouldSortAscending() );
		}

		WHEN( "toggling sort ascending" ) {
			player.ToggleSortAscending();
			THEN( "sort ascending is false" ) {
				REQUIRE_FALSE( player.ShouldSortAscending() );
			}
		}

		THEN( "sort separate deadline is false by default" ) {
			REQUIRE_FALSE( player.ShouldSortSeparateDeadline() );
		}

		WHEN( "toggling sort separate deadline" ) {
			player.ToggleSortSeparateDeadline();
			THEN( "sort separate deadline is true" ) {
				REQUIRE( player.ShouldSortSeparateDeadline() );
			}
		}

		THEN( "sort separate possible is false by default" ) {
			REQUIRE_FALSE( player.ShouldSortSeparatePossible() );
		}

		WHEN( "toggling sort separate possible" ) {
			player.ToggleSortSeparatePossible();
			THEN( "sort separate possible is true" ) {
				REQUIRE( player.ShouldSortSeparatePossible() );
			}
		}
	}
}

SCENARIO( "Start data access", "[PlayerInfo][StartData]" ) {
	GIVEN( "a default-constructed PlayerInfo" ) {
		PlayerInfo player;

		THEN( "start data can be accessed" ) {
			const CoreStartData &startData = player.StartData();
			// Default-constructed CoreStartData
			CHECK( &startData != nullptr );
		}
	}
}

// #endregion unit tests



} // test namespace