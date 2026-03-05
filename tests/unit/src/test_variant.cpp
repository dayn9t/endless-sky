/* test_variant.cpp
Copyright (c) 2026 by Amazinite

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
#include "../../../source/Variant.h"

// ... and any system includes needed for the test file.
#include <type_traits>
#include <vector>

namespace { // test namespace

// #region mock data

// Note: The Variant class depends on GameData for loading ship data.
// Per the unit test README, methods that invoke GameData are not easily testable
// without integration testing. This test file focuses on:
// - Default construction and class traits
// - Getter methods for default state
// - Comparison operators (which do not directly use GameData)

// #endregion mock data



// #region unit tests
TEST_CASE( "Variant Class Traits", "[Variant]" ) {
	using T = Variant;
	SECTION( "Class Layout" ) {
		CHECK_FALSE( std::is_trivial_v<T> );
		CHECK( std::is_standard_layout_v<T> );
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

SCENARIO( "Creating a Variant", "[Variant][Creation]" ) {
	GIVEN( "a default-constructed Variant" ) {
		Variant variant;

		THEN( "it has the correct default properties" ) {
			CHECK( variant.Weight() == 1 );
			CHECK( variant.Ships().empty() );
		}

		THEN( "the ships vector is empty" ) {
			CHECK( variant.Ships().size() == 0 );
			CHECK( variant.Ships().begin() == variant.Ships().end() );
		}
	}
}

SCENARIO( "Comparing Variants", "[Variant][Comparison]" ) {
	GIVEN( "two default-constructed Variants" ) {
		Variant variant1;
		Variant variant2;

		WHEN( "comparing them for equality" ) {
			THEN( "they are equal" ) {
				CHECK( variant1 == variant2 );
				CHECK_FALSE( variant1 != variant2 );
			}
		}
	}

	GIVEN( "a Variant compared with itself" ) {
		Variant variant;

		WHEN( "comparing for equality" ) {
			THEN( "it equals itself" ) {
				CHECK( variant == variant );
				CHECK_FALSE( variant != variant );
			}
		}
	}

	GIVEN( "multiple Variants" ) {
		Variant variant1;
		Variant variant2;
		Variant variant3;

		WHEN( "checking equality transitivity" ) {
			THEN( "equality is transitive" ) {
				CHECK( variant1 == variant2 );
				CHECK( variant2 == variant3 );
				CHECK( variant1 == variant3 );
			}
		}
	}
}

SCENARIO( "Variant Ships Accessor", "[Variant][Ships]" ) {
	GIVEN( "a default-constructed Variant" ) {
		Variant variant;

		WHEN( "accessing the ships vector" ) {
			const auto &ships = variant.Ships();

			THEN( "an empty vector is returned" ) {
				CHECK( ships.empty() );
				CHECK( ships.size() == 0 );
			}
		}
	}
}

SCENARIO( "Variant Weight Accessor", "[Variant][Weight]" ) {
	GIVEN( "a default-constructed Variant" ) {
		Variant variant;

		WHEN( "accessing the weight" ) {
			int weight = variant.Weight();

			THEN( "the default weight is 1" ) {
				CHECK( weight == 1 );
			}
		}
	}
}

SCENARIO( "Variant Copy and Move Operations", "[Variant][CopyMove]" ) {
	GIVEN( "a default-constructed Variant" ) {
		Variant original;

		WHEN( "copy-constructing another Variant" ) {
			Variant copy(original);

			THEN( "the copy has the same properties" ) {
				CHECK( copy.Weight() == original.Weight() );
				CHECK( copy.Ships().size() == original.Ships().size() );
				CHECK( copy == original );
			}
		}

		WHEN( "copy-assigning to another Variant" ) {
			Variant copy;
			copy = original;

			THEN( "the copy has the same properties" ) {
				CHECK( copy.Weight() == original.Weight() );
				CHECK( copy.Ships().size() == original.Ships().size() );
				CHECK( copy == original );
			}
		}

		WHEN( "move-constructing another Variant" ) {
			Variant moved(std::move(original));

			THEN( "the moved Variant has the expected properties" ) {
				CHECK( moved.Weight() == 1 );
				CHECK( moved.Ships().empty() );
			}
		}

		WHEN( "move-assigning to another Variant" ) {
			Variant moved;
			moved = std::move(original);

			THEN( "the moved Variant has the expected properties" ) {
				CHECK( moved.Weight() == 1 );
				CHECK( moved.Ships().empty() );
			}
		}
	}
}

// Note: The following methods cannot be fully tested with unit tests
// because they depend on GameData:
// - Variant(const DataNode &node) - calls Load()
// - Load(const DataNode &node) - uses GameData::Ships().Get()
// - IsValid() - requires Ship objects from GameData
// - Strength() - requires Ship objects with valid Strength()
//
// These methods should be tested via integration tests that have
// proper GameData initialization.
// #endregion unit tests



} // test namespace