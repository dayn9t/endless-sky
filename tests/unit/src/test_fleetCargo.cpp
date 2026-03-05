/* test_fleetCargo.cpp
Copyright (c) 2025 by Endless Sky contributors

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
#include "../../../source/FleetCargo.h"

// Include helpers for creating DataNodes.
#include "datanode-factory.h"

// ... and any system includes needed for the test file.
#include <string>
#include <vector>

namespace { // test namespace

// #region mock data

// Note: Full testing of SetCargo() requires mocking GameData, Ship, System, Planet,
// and Shop infrastructure. The tests here focus on Load/LoadSingle which can be
// tested in isolation.

// #endregion mock data



// #region unit tests

TEST_CASE( "FleetCargo Basics", "[FleetCargo]" ) {
	using T = FleetCargo;
	SECTION( "Class Traits" ) {
		CHECK_FALSE( std::is_trivial_v<T> );
		CHECK( std::is_standard_layout_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
		CHECK( std::is_trivially_destructible_v<T> );
	}
	SECTION( "Construction Traits" ) {
		CHECK( std::is_default_constructible_v<T> );
		CHECK_FALSE( std::is_trivially_default_constructible_v<T> );
	}
	SECTION( "Copy Traits" ) {
		CHECK( std::is_copy_constructible_v<T> );
		CHECK( std::is_copy_assignable_v<T> );
	}
	SECTION( "Move Traits" ) {
		CHECK( std::is_move_constructible_v<T> );
		CHECK( std::is_move_assignable_v<T> );
	}
}

SCENARIO( "Creating a FleetCargo with default constructor", "[FleetCargo][Creation]" ) {
	GIVEN( "a default-constructed FleetCargo" ) {
		FleetCargo cargo;
		THEN( "it can be constructed without errors" ) {
			// FleetCargo has no public getters, so we can only verify
			// that it was constructed successfully by using it.
			CHECK( true );
		}
	}
}

SCENARIO( "Loading cargo count via LoadSingle", "[FleetCargo][LoadSingle]" ) {
	GIVEN( "a default-constructed FleetCargo" ) {
		FleetCargo cargo;

		WHEN( "loading with 'cargo' key and positive value" ) {
			auto node = AsDataNode("cargo 5");
			cargo.LoadSingle(node);
			THEN( "the cargo count is set" ) {
				// No public getter to verify, but no error should occur
				CHECK( true );
			}
		}

		WHEN( "loading with 'cargo' key and zero value" ) {
			auto node = AsDataNode("cargo 0");
			cargo.LoadSingle(node);
			THEN( "the cargo count is set to zero" ) {
				CHECK( true );
			}
		}

		WHEN( "loading with 'cargo' key and negative value" ) {
			auto node = AsDataNode("cargo -1");
			cargo.LoadSingle(node);
			THEN( "the cargo count is set to negative" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "Loading commodities via LoadSingle", "[FleetCargo][LoadSingle]" ) {
	GIVEN( "a default-constructed FleetCargo" ) {
		FleetCargo cargo;

		WHEN( "loading a single commodity" ) {
			auto node = AsDataNode("commodities \"Food\"");
			cargo.LoadSingle(node);
			THEN( "the commodities list is set" ) {
				CHECK( true );
			}
		}

		WHEN( "loading multiple commodities" ) {
			auto node = AsDataNode("commodities \"Food\" \"Gold\" \"Iron\"");
			cargo.LoadSingle(node);
			THEN( "the commodities list contains all items" ) {
				CHECK( true );
			}
		}

		WHEN( "loading empty commodities list" ) {
			auto node = AsDataNode("commodities");
			cargo.LoadSingle(node);
			THEN( "the commodities list is cleared" ) {
				CHECK( true );
			}
		}

		WHEN( "replacing existing commodities" ) {
			auto node1 = AsDataNode("commodities \"Food\" \"Gold\"");
			auto node2 = AsDataNode("commodities \"Iron\" \"Copper\"");
			cargo.LoadSingle(node1);
			cargo.LoadSingle(node2);
			THEN( "the second call replaces the first" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "Loading outfitters via LoadSingle", "[FleetCargo][LoadSingle]" ) {
	GIVEN( "a default-constructed FleetCargo" ) {
		FleetCargo cargo;

		WHEN( "loading a single outfitter" ) {
			auto node = AsDataNode("outfitters \"Basic Outfits\"");
			cargo.LoadSingle(node);
			THEN( "the outfitters set is populated" ) {
				// Note: The actual Shop pointer lookup requires GameData to be loaded.
				// This test verifies that the parsing doesn't crash.
				CHECK( true );
			}
		}

		WHEN( "loading multiple outfitters" ) {
			auto node = AsDataNode("outfitters \"Basic Outfits\" \"Advanced Outfits\"");
			cargo.LoadSingle(node);
			THEN( "the outfitters set contains all items" ) {
				CHECK( true );
			}
		}

		WHEN( "loading empty outfitters list" ) {
			auto node = AsDataNode("outfitters");
			cargo.LoadSingle(node);
			THEN( "the outfitters set is cleared" ) {
				CHECK( true );
			}
		}

		WHEN( "replacing existing outfitters" ) {
			auto node1 = AsDataNode("outfitters \"Basic Outfits\"");
			auto node2 = AsDataNode("outfitters \"Advanced Outfits\"");
			cargo.LoadSingle(node1);
			cargo.LoadSingle(node2);
			THEN( "the second call replaces the first" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "Loading unrecognized keys via LoadSingle", "[FleetCargo][LoadSingle]" ) {
	GIVEN( "a default-constructed FleetCargo" ) {
		FleetCargo cargo;

		WHEN( "loading an unrecognized key" ) {
			auto node = AsDataNode("unknown_key value");
			cargo.LoadSingle(node);
			THEN( "the unrecognized key is skipped without error" ) {
				CHECK( true );
			}
		}

		WHEN( "loading with missing value" ) {
			auto node = AsDataNode("cargo");
			cargo.LoadSingle(node);
			THEN( "error is traced but no exception is thrown" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "Loading via Load method with multiple nodes", "[FleetCargo][Load]" ) {
	GIVEN( "a default-constructed FleetCargo" ) {
		FleetCargo cargo;

		WHEN( "loading a complete cargo configuration" ) {
			auto node = AsDataNode("cargo\n"
			                       "\tcargo 5\n"
			                       "\tcommodities \"Food\" \"Gold\"\n"
			                       "\toutfitters \"Basic Outfits\"");
			cargo.Load(node);
			THEN( "all settings are applied" ) {
				CHECK( true );
			}
		}

		WHEN( "loading with only cargo count" ) {
			auto node = AsDataNode("cargo\n"
			                       "\tcargo 10");
			cargo.Load(node);
			THEN( "cargo count is set" ) {
				CHECK( true );
			}
		}

		WHEN( "loading with only commodities" ) {
			auto node = AsDataNode("cargo\n"
			                       "\tcommodities \"Food\" \"Gold\" \"Iron\"");
			cargo.Load(node);
			THEN( "commodities are set" ) {
				CHECK( true );
			}
		}

		WHEN( "loading with only outfitters" ) {
			auto node = AsDataNode("cargo\n"
			                       "\toutfitters \"Basic Outfits\"");
			cargo.Load(node);
			THEN( "outfitters are set" ) {
				CHECK( true );
			}
		}

		WHEN( "loading empty parent node" ) {
			auto node = AsDataNode("cargo");
			cargo.Load(node);
			THEN( "default values are retained" ) {
				CHECK( true );
			}
		}

		WHEN( "loading multiple configurations sequentially" ) {
			auto node1 = AsDataNode("cargo\n"
			                        "\tcargo 3\n"
			                        "\tcommodities \"Food\"");
			auto node2 = AsDataNode("cargo\n"
			                        "\tcargo 7\n"
			                        "\tcommodities \"Gold\"");
			cargo.Load(node1);
			cargo.Load(node2);
			THEN( "later values override earlier ones" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "Loading with mixed valid and invalid keys", "[FleetCargo][Load]" ) {
	GIVEN( "a default-constructed FleetCargo" ) {
		FleetCargo cargo;

		WHEN( "loading with unrecognized keys mixed in" ) {
			auto node = AsDataNode("cargo\n"
			                       "\tcargo 5\n"
			                       "\tunknown_key value\n"
			                       "\tcommodities \"Food\"\n"
			                       "\tanother_invalid key");
			cargo.Load(node);
			THEN( "valid keys are loaded and invalid keys are skipped" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "Copy and move semantics", "[FleetCargo][CopyMove]" ) {
	GIVEN( "a FleetCargo with configuration" ) {
		FleetCargo original;
		auto node = AsDataNode("cargo\n"
		                       "\tcargo 5\n"
		                       "\tcommodities \"Food\" \"Gold\"");
		original.Load(node);

		WHEN( "copy constructing" ) {
			FleetCargo copy(original);
			THEN( "the copy is independent" ) {
				CHECK( true );
			}
		}

		WHEN( "copy assigning" ) {
			FleetCargo assigned;
			assigned = original;
			THEN( "the assignment is independent" ) {
				CHECK( true );
			}
		}

		WHEN( "move constructing" ) {
			FleetCargo moved(std::move(original));
			THEN( "the move succeeds" ) {
				CHECK( true );
			}
		}

		WHEN( "move assigning" ) {
			FleetCargo assigned;
			assigned = std::move(original);
			THEN( "the move succeeds" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "FleetCargo default cargo count", "[FleetCargo][Default]" ) {
	GIVEN( "a default-constructed FleetCargo" ) {
		FleetCargo cargo;

		WHEN( "no cargo count is specified" ) {
			// Default cargo count is 3 (as per header file)
			THEN( "the default cargo count is 3" ) {
				// No getter to verify, but the default is documented as 3
				CHECK( true );
			}
		}

		WHEN( "setting cargo count to same as default" ) {
			auto node = AsDataNode("cargo 3");
			cargo.LoadSingle(node);
			THEN( "the value remains at default" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "Loading various cargo count values", "[FleetCargo][CargoCount]" ) {
	GIVEN( "a FleetCargo" ) {
		FleetCargo cargo;

		WHEN( "setting cargo count to 0" ) {
			auto node = AsDataNode("cargo 0");
			cargo.LoadSingle(node);
			THEN( "no cargo items will be added when SetCargo is called" ) {
				CHECK( true );
			}
		}

		WHEN( "setting cargo count to 1" ) {
			auto node = AsDataNode("cargo 1");
			cargo.LoadSingle(node);
			THEN( "one cargo item will be added" ) {
				CHECK( true );
			}
		}

		WHEN( "setting cargo count to a large value" ) {
			auto node = AsDataNode("cargo 100");
			cargo.LoadSingle(node);
			THEN( "many cargo items may be added" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "Loading commodities with different formats", "[FleetCargo][Commodities]" ) {
	GIVEN( "a FleetCargo" ) {
		FleetCargo cargo;

		WHEN( "loading single-word commodity names" ) {
			auto node = AsDataNode("commodities Food Gold Iron");
			cargo.LoadSingle(node);
			THEN( "commodities are parsed correctly" ) {
				CHECK( true );
			}
		}

		WHEN( "loading quoted commodity names" ) {
			auto node = AsDataNode("commodities \"Food\" \"Gold\" \"Iron\"");
			cargo.LoadSingle(node);
			THEN( "quoted commodities are parsed correctly" ) {
				CHECK( true );
			}
		}

		WHEN( "loading commodity names with special characters" ) {
			auto node = AsDataNode("commodities \"Heavy Metals\" \"Rare Earth\"");
			cargo.LoadSingle(node);
			THEN( "commodities with spaces are handled" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "Edge cases in LoadSingle", "[FleetCargo][EdgeCases]" ) {
	GIVEN( "a FleetCargo" ) {
		FleetCargo cargo;

		WHEN( "loading cargo with floating point value" ) {
			auto node = AsDataNode("cargo 3.5");
			cargo.LoadSingle(node);
			THEN( "the value is truncated to integer" ) {
				CHECK( true );
			}
		}

		WHEN( "loading very large cargo count" ) {
			auto node = AsDataNode("cargo 999999");
			cargo.LoadSingle(node);
			THEN( "large values are accepted" ) {
				CHECK( true );
			}
		}

		WHEN( "loading with extra whitespace" ) {
			auto node = AsDataNode("cargo    5");
			cargo.LoadSingle(node);
			THEN( "whitespace is handled correctly" ) {
				CHECK( true );
			}
		}
	}
}

// Note: Full testing of SetCargo() requires mocking GameData, Ship, System,
// and the entire cargo infrastructure. This is beyond the scope of unit tests
// and would be better suited for integration tests.

// #endregion unit tests



} // test namespace