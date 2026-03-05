/* test_fleet.cpp
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
#include "../../../source/Fleet.h"

// ... and any system includes needed for the test file.
#include <list>
#include <memory>
#include <type_traits>

namespace { // test namespace

// #region mock data

// Note: The Fleet class depends heavily on GameData for loading government,
// phrase, formation, and variant data. Per the unit test README, methods that
// invoke GameData are not easily testable without integration testing.
// This test file focuses on:
// - Default construction and class traits
// - Getter methods for default state
// - IsValid() with default-constructed fleet
// - Strength() with empty variants
// - RemoveInvalidVariants() with empty fleet

// #endregion mock data



// #region unit tests
TEST_CASE( "Fleet Class Traits", "[Fleet]" ) {
	using T = Fleet;
	SECTION( "Class Layout" ) {
		CHECK_FALSE( std::is_trivial_v<T> );
		CHECK_FALSE( std::is_standard_layout_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
	}
	SECTION( "Construction Traits" ) {
		CHECK( std::is_default_constructible_v<T> );
		CHECK( std::is_nothrow_default_constructible_v<T> );
		CHECK( std::is_copy_constructible_v<T> );
		CHECK( std::is_nothrow_move_constructible_v<T> );
	}
	SECTION( "Copy and Move Traits" ) {
		CHECK( std::is_copy_assignable_v<T> );
		CHECK( std::is_move_assignable_v<T> );
		CHECK( std::is_nothrow_move_assignable_v<T> );
	}
}

SCENARIO( "Creating a Fleet with default constructor", "[Fleet][Creation]" ) {
	GIVEN( "a default-constructed Fleet" ) {
		Fleet fleet;

		THEN( "it has no government" ) {
			CHECK( fleet.GetGovernment() == nullptr );
		}

		THEN( "it is not valid when requiring government" ) {
			CHECK_FALSE( fleet.IsValid() );
		}

		THEN( "it is valid when not requiring government" ) {
			CHECK( fleet.IsValid(false) );
		}

		THEN( "it has zero strength" ) {
			CHECK( fleet.Strength() == 0 );
		}
	}
}

SCENARIO( "Checking Fleet validity", "[Fleet][Validity]" ) {
	GIVEN( "a default-constructed Fleet" ) {
		Fleet fleet;

		WHEN( "checking validity with requireGovernment = true (default)" ) {
			THEN( "it is invalid because government is null" ) {
				CHECK_FALSE( fleet.IsValid() );
				CHECK_FALSE( fleet.IsValid(true) );
			}
		}

		WHEN( "checking validity with requireGovernment = false" ) {
			THEN( "it is valid because no government is required" ) {
				CHECK( fleet.IsValid(false) );
			}
		}
	}
}

SCENARIO( "Fleet government accessor", "[Fleet][Government]" ) {
	GIVEN( "a default-constructed Fleet" ) {
		Fleet fleet;

		WHEN( "getting the government" ) {
			const Government *gov = fleet.GetGovernment();

			THEN( "the government is null" ) {
				CHECK( gov == nullptr );
			}
		}
	}
}

SCENARIO( "Fleet strength calculation", "[Fleet][Strength]" ) {
	GIVEN( "a default-constructed Fleet" ) {
		Fleet fleet;

		WHEN( "getting the strength" ) {
			int64_t strength = fleet.Strength();

			THEN( "strength is zero for empty variants" ) {
				CHECK( strength == 0 );
			}
		}
	}
}

SCENARIO( "Removing invalid variants from Fleet", "[Fleet][Variants]" ) {
	GIVEN( "a default-constructed Fleet with no variants" ) {
		Fleet fleet;

		WHEN( "removing invalid variants" ) {
			// This should not throw or crash
			fleet.RemoveInvalidVariants();

			THEN( "the operation completes without error" ) {
				CHECK( true );
			}

			THEN( "strength remains zero" ) {
				CHECK( fleet.Strength() == 0 );
			}
		}
	}
}

SCENARIO( "Fleet copy semantics", "[Fleet][Copy]" ) {
	GIVEN( "a default-constructed Fleet" ) {
		Fleet original;

		WHEN( "copy-constructing another Fleet" ) {
			Fleet copy(original);

			THEN( "the copy has the same default properties" ) {
				CHECK( copy.GetGovernment() == nullptr );
				CHECK_FALSE( copy.IsValid() );
				CHECK( copy.IsValid(false) );
				CHECK( copy.Strength() == 0 );
			}
		}

		WHEN( "copy-assigning to another Fleet" ) {
			Fleet assigned;
			assigned = original;

			THEN( "the assigned Fleet has the same default properties" ) {
				CHECK( assigned.GetGovernment() == nullptr );
				CHECK_FALSE( assigned.IsValid() );
				CHECK( assigned.IsValid(false) );
				CHECK( assigned.Strength() == 0 );
			}
		}
	}
}

SCENARIO( "Fleet move semantics", "[Fleet][Move]" ) {
	GIVEN( "a default-constructed Fleet" ) {
		Fleet original;

		WHEN( "move-constructing another Fleet" ) {
			Fleet moved(std::move(original));

			THEN( "the moved Fleet has the expected properties" ) {
				CHECK( moved.GetGovernment() == nullptr );
				CHECK_FALSE( moved.IsValid() );
				CHECK( moved.IsValid(false) );
				CHECK( moved.Strength() == 0 );
			}
		}

		WHEN( "move-assigning to another Fleet" ) {
			Fleet assigned;
			assigned = std::move(original);

			THEN( "the assigned Fleet has the expected properties" ) {
				CHECK( assigned.GetGovernment() == nullptr );
				CHECK_FALSE( assigned.IsValid() );
				CHECK( assigned.IsValid(false) );
				CHECK( assigned.Strength() == 0 );
			}
		}
	}
}

SCENARIO( "Fleet with multiple copy and move operations", "[Fleet][CopyMove]" ) {
	GIVEN( "a default-constructed Fleet" ) {
		Fleet original;

		WHEN( "performing multiple copies" ) {
			Fleet firstCopy(original);
			Fleet secondCopy(firstCopy);

			THEN( "all copies have consistent properties" ) {
				CHECK( original.GetGovernment() == nullptr );
				CHECK( firstCopy.GetGovernment() == nullptr );
				CHECK( secondCopy.GetGovernment() == nullptr );
				CHECK( original.Strength() == 0 );
				CHECK( firstCopy.Strength() == 0 );
				CHECK( secondCopy.Strength() == 0 );
			}
		}

		WHEN( "performing copy then move" ) {
			Fleet copied(original);
			Fleet moved(std::move(copied));

			THEN( "the final Fleet has expected properties" ) {
				CHECK( moved.GetGovernment() == nullptr );
				CHECK( moved.Strength() == 0 );
			}
		}
	}
}

SCENARIO( "Fleet default state consistency", "[Fleet][DefaultState]" ) {
	GIVEN( "multiple default-constructed Fleets" ) {
		Fleet fleet1;
		Fleet fleet2;
		Fleet fleet3;

		THEN( "they all have consistent default state" ) {
			CHECK( fleet1.GetGovernment() == fleet2.GetGovernment() );
			CHECK( fleet2.GetGovernment() == fleet3.GetGovernment() );
			CHECK( fleet1.GetGovernment() == nullptr );

			CHECK( fleet1.IsValid() == fleet2.IsValid() );
			CHECK( fleet2.IsValid() == fleet3.IsValid() );
			CHECK_FALSE( fleet1.IsValid() );

			CHECK( fleet1.IsValid(false) == fleet2.IsValid(false) );
			CHECK( fleet2.IsValid(false) == fleet3.IsValid(false) );
			CHECK( fleet1.IsValid(false) );

			CHECK( fleet1.Strength() == fleet2.Strength() );
			CHECK( fleet2.Strength() == fleet3.Strength() );
			CHECK( fleet1.Strength() == 0 );
		}
	}
}

SCENARIO( "Fleet IsValid parameter variations", "[Fleet][IsValid]" ) {
	GIVEN( "a default-constructed Fleet" ) {
		Fleet fleet;

		WHEN( "checking validity with requireGovernment = true" ) {
			bool valid = fleet.IsValid(true);

			THEN( "it is invalid because government is null" ) {
				CHECK_FALSE( valid );
			}
		}

		WHEN( "checking validity with requireGovernment = false" ) {
			bool valid = fleet.IsValid(false);

			THEN( "it is valid" ) {
				CHECK( valid );
			}
		}

		WHEN( "checking validity with default parameter" ) {
			bool valid = fleet.IsValid();

			THEN( "default is requireGovernment = true, so invalid" ) {
				CHECK_FALSE( valid );
			}
		}
	}
}

SCENARIO( "Fleet RemoveInvalidVariants idempotency", "[Fleet][RemoveInvalidVariants]" ) {
	GIVEN( "a default-constructed Fleet" ) {
		Fleet fleet;

		WHEN( "calling RemoveInvalidVariants multiple times" ) {
			fleet.RemoveInvalidVariants();
			fleet.RemoveInvalidVariants();
			fleet.RemoveInvalidVariants();

			THEN( "the operation is idempotent" ) {
				CHECK( fleet.Strength() == 0 );
				CHECK( fleet.GetGovernment() == nullptr );
				CHECK_FALSE( fleet.IsValid() );
			}
		}
	}
}

SCENARIO( "Fleet after self-assignment", "[Fleet][SelfAssignment]" ) {
	GIVEN( "a default-constructed Fleet" ) {
		Fleet fleet;

		WHEN( "self-copy-assigning" ) {
			fleet = fleet;

			THEN( "the Fleet remains in valid state" ) {
				CHECK( fleet.GetGovernment() == nullptr );
				CHECK_FALSE( fleet.IsValid() );
				CHECK( fleet.Strength() == 0 );
			}
		}

		WHEN( "self-move-assigning" ) {
			fleet = std::move(fleet);

			THEN( "the Fleet remains in valid state" ) {
				CHECK( fleet.GetGovernment() == nullptr );
				CHECK_FALSE( fleet.IsValid() );
				CHECK( fleet.Strength() == 0 );
			}
		}
	}
}

SCENARIO( "Fleet Strength return type", "[Fleet][Strength]" ) {
	GIVEN( "a default-constructed Fleet" ) {
		Fleet fleet;

		WHEN( "getting strength" ) {
			auto strength = fleet.Strength();

			THEN( "strength is of type int64_t" ) {
				CHECK( (std::is_same_v<decltype(strength), int64_t>) );
			}

			THEN( "strength is zero" ) {
				CHECK( strength == 0 );
			}
		}
	}
}

SCENARIO( "Fleet GetGovernment return type", "[Fleet][Government]" ) {
	GIVEN( "a default-constructed Fleet" ) {
		Fleet fleet;

		WHEN( "getting government" ) {
			auto gov = fleet.GetGovernment();

			THEN( "government is of type const Government*" ) {
				CHECK( (std::is_same_v<decltype(gov), const Government*>) );
			}

			THEN( "government is null" ) {
				CHECK( gov == nullptr );
			}
		}
	}
}

// Note: The following methods cannot be fully tested with unit tests
// because they depend on GameData:
// - Fleet(const DataNode &node) - calls Load()
// - Load(const DataNode &node) - uses GameData::Governments().Get(), etc.
// - Enter() methods - require System, Ship, Planet, and GameData
// - Place() methods - require System, Ship, and GameData
//
// These methods should be tested via integration tests that have
// proper GameData initialization.

// #endregion unit tests



} // test namespace