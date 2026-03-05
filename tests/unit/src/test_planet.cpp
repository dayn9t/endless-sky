/* test_planet.cpp
Copyright (c) 2025 by test contributor

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
#include "../../../source/Planet.h"

// ... and any system includes needed for the test file.
#include <type_traits>
#include <string>
#include <set>

namespace { // test namespace

// #region mock data

// Insert file-local data here, e.g. classes, structs, or fixtures that will be useful
// to help test this class/method.

// #endregion mock data



// #region unit tests

// Test Class Traits
TEST_CASE( "Planet Class Traits", "[Planet]" ) {
	using T = Planet;
	SECTION( "Class Properties" ) {
		// Planet has member types that prevent it from being trivial.
		CHECK_FALSE( std::is_trivial_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
		CHECK_FALSE( std::is_trivially_destructible_v<T> );
	}
	SECTION( "Construction Traits" ) {
		CHECK( std::is_default_constructible_v<T> );
		CHECK_FALSE( std::is_trivially_default_constructible_v<T> );
		CHECK( std::is_copy_constructible_v<T> );
		CHECK( std::is_move_constructible_v<T> );
	}
	SECTION( "Copy/Move Traits" ) {
		CHECK( std::is_copy_assignable_v<T> );
		CHECK( std::is_move_assignable_v<T> );
		CHECK_FALSE( std::is_trivially_copyable_v<T> );
	}
}

// Test the Friendliness enum values.
SCENARIO( "Planet::Friendliness enum values", "[Planet][Friendliness]" ) {
	GIVEN( "the Friendliness enum" ) {
		THEN( "enum values are defined in the expected order" ) {
			CHECK( static_cast<int>(Planet::Friendliness::FRIENDLY) == 0 );
			CHECK( static_cast<int>(Planet::Friendliness::RESTRICTED) == 1 );
			CHECK( static_cast<int>(Planet::Friendliness::HOSTILE) == 2 );
			CHECK( static_cast<int>(Planet::Friendliness::DOMINATED) == 3 );
		}
	}
}

// Test default construction.
SCENARIO( "Creating a Planet with default constructor", "[Planet][Creation]" ) {
	GIVEN( "a default-constructed Planet" ) {
		Planet planet;

		WHEN( "checking initial state" ) {
			THEN( "it is not valid by default" ) {
				CHECK_FALSE( planet.IsValid() );
			}
			THEN( "it has no true name set" ) {
				CHECK( planet.TrueName().empty() );
			}
			THEN( "it has no display name set" ) {
				CHECK( planet.DisplayName().empty() );
			}
			THEN( "it has no description" ) {
				CHECK( planet.Description().IsEmpty() );
			}
			THEN( "it has no landscape sprite" ) {
				CHECK( planet.Landscape() == nullptr );
			}
			THEN( "it has no music name" ) {
				CHECK( planet.MusicName().empty() );
			}
			THEN( "it has no attributes" ) {
				CHECK( planet.Attributes().empty() );
			}
			THEN( "it has no required attributes" ) {
				CHECK( planet.RequiredAttributes().empty() );
			}
			THEN( "it is unrestricted by default" ) {
				CHECK( planet.IsUnrestricted() );
			}
			THEN( "it is not inhabited by default" ) {
				CHECK_FALSE( planet.IsInhabited() );
			}
			THEN( "it has no custom security by default" ) {
				CHECK_FALSE( planet.HasCustomSecurity() );
			}
			THEN( "it has no shipyard by default" ) {
				CHECK_FALSE( planet.HasShipyard() );
			}
			THEN( "it has no outfitter by default" ) {
				CHECK_FALSE( planet.HasOutfitter() );
			}
			THEN( "it has no government by default" ) {
				CHECK( planet.GetGovernment() == nullptr );
			}
			THEN( "it has zero required reputation by default" ) {
				CHECK( planet.RequiredReputation() == 0. );
			}
			THEN( "it has default bribe fraction" ) {
				CHECK( planet.GetBribeFraction() == Catch::Approx(0.01) );
			}
			THEN( "it has default security value" ) {
				CHECK( planet.Security() == Catch::Approx(0.25) );
			}
			THEN( "it has no system by default" ) {
				CHECK( planet.GetSystem() == nullptr );
			}
			THEN( "it has an empty systems list" ) {
				CHECK( planet.Systems().empty() );
			}
			THEN( "it is not a wormhole by default" ) {
				CHECK_FALSE( planet.IsWormhole() );
			}
			THEN( "it has no wormhole by default" ) {
				CHECK( planet.GetWormhole() == nullptr );
			}
			THEN( "it has no named port by default" ) {
				CHECK_FALSE( planet.HasNamedPort() );
			}
			THEN( "it has no services by default" ) {
				CHECK_FALSE( planet.HasServices() );
			}
			THEN( "noun defaults to 'planet'" ) {
				CHECK( planet.Noun() == "planet" );
			}
			THEN( "it is not defending by default" ) {
				CHECK_FALSE( planet.IsDefending() );
			}
			THEN( "daily tribute penalty is zero by default" ) {
				CHECK( planet.DailyTributePenalty() == 0. );
			}
		}
	}
}

// Test TrueName and SetTrueName.
SCENARIO( "Setting and getting TrueName", "[Planet][TrueName]" ) {
	GIVEN( "a default-constructed Planet" ) {
		Planet planet;
		REQUIRE( planet.TrueName().empty() );
		REQUIRE( planet.DisplayName().empty() );

		WHEN( "setting the true name" ) {
			planet.SetTrueName("Test Planet");

			THEN( "the true name is set correctly" ) {
				CHECK( planet.TrueName() == "Test Planet" );
			}
			THEN( "the display name is also set to true name by default" ) {
				CHECK( planet.DisplayName() == "Test Planet" );
			}
		}

		WHEN( "setting an empty true name" ) {
			planet.SetTrueName("");

			THEN( "the true name remains empty" ) {
				CHECK( planet.TrueName().empty() );
			}
		}
	}
}

// Test DisplayName behavior.
SCENARIO( "DisplayName returns appropriate name", "[Planet][DisplayName]" ) {
	GIVEN( "a planet with a true name set" ) {
		Planet planet;
		planet.SetTrueName("Original Name");

		WHEN( "querying display name without wormhole" ) {
			THEN( "display name matches true name" ) {
				CHECK( planet.DisplayName() == "Original Name" );
			}
		}
		// Note: When planet is a wormhole, DisplayName() returns wormhole->DisplayName()
		// Testing wormhole behavior requires GameData initialization
	}
}

// Test system operations.
SCENARIO( "Setting and getting system associations", "[Planet][System]" ) {
	GIVEN( "a default-constructed Planet" ) {
		Planet planet;
		REQUIRE( planet.GetSystem() == nullptr );
		REQUIRE( planet.Systems().empty() );

		// Note: SetSystem requires a valid System pointer.
		// Testing with actual System instances requires GameData initialization.
		// This test documents the expected behavior pattern.
		WHEN( "no system is set" ) {
			THEN( "GetSystem returns nullptr" ) {
				CHECK( planet.GetSystem() == nullptr );
			}
			THEN( "Systems returns empty vector" ) {
				CHECK( planet.Systems().empty() );
			}
		}

		// Note: IsInSystem checks if the planet is in a given system.
		// This is meaningful for wormholes which can be in multiple systems.
	}
}

// Test wormhole status.
SCENARIO( "Checking wormhole status", "[Planet][Wormhole]" ) {
	GIVEN( "a default-constructed Planet" ) {
		Planet planet;

		WHEN( "checking if it is a wormhole" ) {
			THEN( "it is not a wormhole by default" ) {
				CHECK_FALSE( planet.IsWormhole() );
			}
		}

		WHEN( "getting the wormhole pointer" ) {
			THEN( "it returns nullptr by default" ) {
				CHECK( planet.GetWormhole() == nullptr );
			}
		}
	}
}

// Test validity checking.
SCENARIO( "Planet validity checking", "[Planet][Validity]" ) {
	GIVEN( "a default-constructed Planet" ) {
		Planet planet;

		WHEN( "checking IsValid without any setup" ) {
			THEN( "it returns false because isDefined is false" ) {
				CHECK_FALSE( planet.IsValid() );
			}
		}

		// Note: Full IsValid testing requires:
		// 1. isDefined = true (set via Load())
		// 2. At least one system in the systems vector
		// 3. All systems must be valid
		// These dependencies require GameData initialization.
	}
}

// Test IsUnrestricted.
SCENARIO( "Checking if planet is unrestricted", "[Planet][Access]" ) {
	GIVEN( "a default-constructed Planet" ) {
		Planet planet;

		WHEN( "checking IsUnrestricted" ) {
			THEN( "it is unrestricted by default (no required attributes)" ) {
				CHECK( planet.IsUnrestricted() );
			}
		}
		// Note: Required attributes are populated via Load() with "requires: X" attributes
	}
}

// Test noun descriptor.
SCENARIO( "Getting planet's noun descriptor", "[Planet][Noun]" ) {
	GIVEN( "a default-constructed Planet" ) {
		Planet planet;

		WHEN( "no attributes are set" ) {
			THEN( "noun defaults to 'planet'" ) {
				CHECK( planet.Noun() == "planet" );
			}
		}
		// Note: When attributes contain "moon" or "station", Noun() returns that.
		// When planet is a wormhole, Noun() returns "wormhole".
		// Testing these requires Load() with proper attributes.
	}
}

// Test HasNamedPort.
SCENARIO( "Checking if planet has a named port", "[Planet][Port]" ) {
	GIVEN( "a default-constructed Planet" ) {
		Planet planet;

		WHEN( "checking HasNamedPort" ) {
			THEN( "it has no named port by default" ) {
				CHECK_FALSE( planet.HasNamedPort() );
			}
		}

		WHEN( "getting the port" ) {
			THEN( "a valid port object is returned" ) {
				const Port &port = planet.GetPort();
				CHECK_FALSE( port.CustomLoaded() );
				CHECK( port.DisplayName().empty() );
			}
		}
	}
}

// Test HasServices.
SCENARIO( "Checking planet services", "[Planet][Services]" ) {
	GIVEN( "a default-constructed Planet" ) {
		Planet planet;

		WHEN( "checking HasServices" ) {
			THEN( "no services are available by default" ) {
				CHECK_FALSE( planet.HasServices() );
				CHECK_FALSE( planet.HasServices(true) );
				CHECK_FALSE( planet.HasServices(false) );
			}
		}
	}
}

// Test security-related getters.
SCENARIO( "Getting planet security values", "[Planet][Security]" ) {
	GIVEN( "a default-constructed Planet" ) {
		Planet planet;

		WHEN( "checking security values" ) {
			THEN( "required reputation is zero by default" ) {
				CHECK( planet.RequiredReputation() == Catch::Approx(0.) );
			}
			THEN( "bribe fraction has a default of 0.01" ) {
				CHECK( planet.GetBribeFraction() == Catch::Approx(0.01) );
			}
			THEN( "security has a default of 0.25" ) {
				CHECK( planet.Security() == Catch::Approx(0.25) );
			}
			THEN( "custom security is false by default" ) {
				CHECK_FALSE( planet.HasCustomSecurity() );
			}
		}
		// Note: These values can be changed via Load() with appropriate attributes
	}
}

// Test shipyard and outfitter.
SCENARIO( "Checking shipyard and outfitter availability", "[Planet][Shops]" ) {
	GIVEN( "a default-constructed Planet" ) {
		Planet planet;

		WHEN( "checking shipyard availability" ) {
			THEN( "no shipyard by default" ) {
				CHECK_FALSE( planet.HasShipyard() );
			}
			THEN( "shipyard stock is empty" ) {
				const auto &stock = planet.ShipyardStock();
				CHECK( stock.empty() );
			}
			THEN( "shipyards set is empty" ) {
				CHECK( planet.Shipyards().empty() );
			}
		}

		WHEN( "checking outfitter availability" ) {
			THEN( "no outfitter by default" ) {
				CHECK_FALSE( planet.HasOutfitter() );
			}
			THEN( "outfitter stock is empty" ) {
				const auto &stock = planet.OutfitterStock();
				CHECK( stock.empty() );
			}
			THEN( "outfitters set is empty" ) {
				CHECK( planet.Outfitters().empty() );
			}
		}
	}
}

// Test government getter.
SCENARIO( "Getting planet's government", "[Planet][Government]" ) {
	GIVEN( "a default-constructed Planet" ) {
		Planet planet;

		WHEN( "no government is set" ) {
			THEN( "GetGovernment returns nullptr" ) {
				CHECK( planet.GetGovernment() == nullptr );
			}
		}
		// Note: When government is nullptr but planet has a system,
		// GetGovernment() returns the system's government.
	}
}

// Test defense-related methods.
SCENARIO( "Checking defense state", "[Planet][Defense]" ) {
	GIVEN( "a default-constructed Planet" ) {
		Planet planet;

		WHEN( "checking defense status" ) {
			THEN( "planet is not defending by default" ) {
				CHECK_FALSE( planet.IsDefending() );
			}
			THEN( "daily tribute penalty is zero" ) {
				CHECK( planet.DailyTributePenalty() == Catch::Approx(0.) );
			}
		}

		WHEN( "calling ResetDefense" ) {
			planet.ResetDefense();

			THEN( "planet is not defending after reset" ) {
				CHECK_FALSE( planet.IsDefending() );
			}
		}
	}
}

// Test copying and moving Planet instances.
SCENARIO( "Copying and moving Planet instances", "[Planet][CopyMove]" ) {
	GIVEN( "two Planet instances" ) {
		Planet original;
		original.SetTrueName("Original Planet");
		Planet copy;

		WHEN( "copy-assigning one to another" ) {
			copy = original;

			THEN( "both have the same state" ) {
				CHECK( copy.TrueName() == original.TrueName() );
				CHECK( copy.DisplayName() == original.DisplayName() );
				CHECK( copy.IsValid() == original.IsValid() );
				CHECK( copy.IsWormhole() == original.IsWormhole() );
				CHECK( copy.IsInhabited() == original.IsInhabited() );
				CHECK( copy.IsUnrestricted() == original.IsUnrestricted() );
				CHECK( copy.HasShipyard() == original.HasShipyard() );
				CHECK( copy.HasOutfitter() == original.HasOutfitter() );
				CHECK( copy.Security() == Catch::Approx(original.Security()) );
				CHECK( copy.GetBribeFraction() == Catch::Approx(original.GetBribeFraction()) );
				CHECK( copy.RequiredReputation() == Catch::Approx(original.RequiredReputation()) );
			}
		}
	}

	GIVEN( "a Planet to move" ) {
		Planet original;
		original.SetTrueName("Moved Planet");
		Planet moved;

		WHEN( "move-assigning" ) {
			moved = std::move(original);

			THEN( "the moved-to object has the expected state" ) {
				CHECK( moved.TrueName() == "Moved Planet" );
				CHECK_FALSE( moved.IsValid() );
				CHECK_FALSE( moved.IsWormhole() );
				CHECK_FALSE( moved.IsInhabited() );
			}
		}
	}

	GIVEN( "a Planet instance for copy construction" ) {
		Planet original;
		original.SetTrueName("Copied Planet");

		WHEN( "copy-constructing" ) {
			Planet copy(original);

			THEN( "the copy has the same state as original" ) {
				CHECK( copy.TrueName() == original.TrueName() );
				CHECK( copy.DisplayName() == original.DisplayName() );
				CHECK( copy.IsValid() == original.IsValid() );
				CHECK( copy.IsWormhole() == original.IsWormhole() );
				CHECK( copy.Systems().size() == original.Systems().size() );
			}
		}
	}

	GIVEN( "a Planet instance for move construction" ) {
		Planet original;
		original.SetTrueName("Move Constructed Planet");

		WHEN( "move-constructing" ) {
			Planet moved(std::move(original));

			THEN( "the moved instance has valid state" ) {
				CHECK( moved.TrueName() == "Move Constructed Planet" );
				CHECK_FALSE( moved.IsValid() );
				CHECK_FALSE( moved.IsWormhole() );
				CHECK_FALSE( moved.IsInhabited() );
				CHECK_FALSE( moved.IsDefending() );
			}
		}
	}
}

// Test attributes access.
SCENARIO( "Accessing planet attributes", "[Planet][Attributes]" ) {
	GIVEN( "a default-constructed Planet" ) {
		Planet planet;

		WHEN( "getting attributes" ) {
			THEN( "attributes set is empty by default" ) {
				CHECK( planet.Attributes().empty() );
			}
		}

		WHEN( "getting required attributes" ) {
			THEN( "required attributes set is empty by default" ) {
				CHECK( planet.RequiredAttributes().empty() );
			}
		}
		// Note: Attributes are populated via Load() method
	}
}

// Test IsAccessible.
SCENARIO( "Checking ship accessibility", "[Planet][Access]" ) {
	GIVEN( "a default-constructed Planet" ) {
		Planet planet;

		WHEN( "checking IsAccessible with nullptr ship" ) {
			THEN( "returns true for unrestricted planet with nullptr" ) {
				// IsUnrestricted() is true, so any ship can land
				// When ship is nullptr and IsUnrestricted is true, returns true
				CHECK( planet.IsAccessible(nullptr) );
			}
		}
		// Note: Full IsAccessible testing with Ship requires Ship instance
		// which requires GameData initialization for loading ship models
	}
}

// Test description.
SCENARIO( "Getting planet description", "[Planet][Description]" ) {
	GIVEN( "a default-constructed Planet" ) {
		Planet planet;

		WHEN( "querying description" ) {
			THEN( "description is empty by default" ) {
				CHECK( planet.Description().IsEmpty() );
			}
		}
		// Note: Description is loaded via Load() with "description" nodes
	}
}

// Test landscape and music.
SCENARIO( "Getting planet landscape and music", "[Planet][Media]" ) {
	GIVEN( "a default-constructed Planet" ) {
		Planet planet;

		WHEN( "querying landscape" ) {
			THEN( "landscape is nullptr by default" ) {
				CHECK( planet.Landscape() == nullptr );
			}
		}

		WHEN( "querying music name" ) {
			THEN( "music name is empty by default" ) {
				CHECK( planet.MusicName().empty() );
			}
		}
		// Note: These are loaded via Load() with "landscape" and "music" attributes
	}
}

// ============================================================================
// Integration Test Notes
// ============================================================================
// The following Planet functionality requires integration tests with full
// GameData initialization:
//
// 1. Load() method:
//    - Requires DataNode with properly formatted planet data
//    - Requires Set<Wormhole> for wormhole resolution
//    - Requires ConditionsStore for player conditions
//    - Tests: loading all planet attributes, government, shipyard, outfitter
//    - Tests: "add" and "remove" operations
//    - Tests: tribute and defense fleet configuration
//
// 2. FinishLoading() method:
//    - Requires Set<Wormhole> for legacy wormhole generation
//    - Tests: automatic wormhole creation for multi-system planets
//
// 3. IsValid() with valid data:
//    - Test: returns true when all components are valid
//    - Test: returns false when no systems assigned
//    - Test: returns false when a linked system is invalid
//
// 4. SetSystem() and RemoveSystem():
//    - Require valid System pointers
//    - Test: adding systems, checking IsInSystem()
//    - Test: removing systems, wormhole behavior with multiple systems
//
// 5. IsAccessible() with Ship:
//    - Requires a Ship instance with proper attributes
//    - Test: checking required attributes against ship attributes
//    - Test: wormhole destination accessibility
//
// 6. Politics-dependent methods (require GameData::GetPolitics()):
//    - HasFuelFor(), CanBribe(), CanLand(), GetFriendliness(), CanUseServices()
//    - Bribe(), DemandTribute(), DeployDefense()
//
// 7. Shop-related methods with real data:
//    - Shipyards(), Outfitters() require GameData::Shipyards()/Outfitters()
//    - ShipyardStock(), OutfitterStock() with loaded shop data
// ============================================================================

// #endregion unit tests



} // test namespace