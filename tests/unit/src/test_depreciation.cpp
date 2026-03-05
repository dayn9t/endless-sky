/* test_depreciation.cpp
Copyright (c) 2026 by Claude

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
#include "../../../source/Depreciation.h"

// ... and any system includes needed for the test file.
#include <cstdint>
#include <type_traits>

namespace { // test namespace

// #region mock data

// Note: The Depreciation class has significant dependencies on GameData:
// - Full() requires GameData::GetGamerules().DepreciationMin()
// - Load() requires GameData::Ships().Get() and GameData::Outfits().Get()
// - Save() requires GameData through WriteSorted
// - Init() requires GameData::Ships().Get()
// - Buy() requires GameData::Ships().Get() and Outfit::Get("installable")
// - Value() requires GameData::Ships().Get(), Ship::ChassisCost(), Outfit::Cost()
// - Internal depreciation calculations require Gamerules values
//
// Per the unit test README: "Generally speaking, if a method invokes the GameData
// class, it will not be testable using unit tests."
//
// The following can be tested without GameData:
// - Default construction (isStock = true, isLoaded = false)
// - IsLoaded() method
// - Class type traits

// #endregion mock data



// #region unit tests

TEST_CASE( "Depreciation Class Traits", "[Depreciation]" ) {
	using T = Depreciation;
	SECTION( "Class Traits" ) {
		CHECK_FALSE( std::is_trivial_v<T> );
		// Has std::map members, not standard_layout
		CHECK_FALSE( std::is_standard_layout_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
		// Has std::map members, not trivially destructible
		CHECK_FALSE( std::is_trivially_destructible_v<T> );
	}
	SECTION( "Construction Traits" ) {
		CHECK( std::is_default_constructible_v<T> );
		// Has std::map members, not trivially default constructible
		CHECK_FALSE( std::is_trivially_default_constructible_v<T> );
		CHECK_FALSE( std::is_nothrow_default_constructible_v<T> );
		CHECK( std::is_copy_constructible_v<T> );
		// std::map copy is not trivial
		CHECK_FALSE( std::is_trivially_copy_constructible_v<T> );
		CHECK_FALSE( std::is_nothrow_copy_constructible_v<T> );
		CHECK( std::is_move_constructible_v<T> );
		CHECK_FALSE( std::is_trivially_move_constructible_v<T> );
		CHECK_FALSE( std::is_nothrow_move_constructible_v<T> );
	}
	SECTION( "Copy Traits" ) {
		CHECK( std::is_copy_assignable_v<T> );
		// std::map is not trivially copyable
		CHECK_FALSE( std::is_trivially_copyable_v<T> );
		CHECK_FALSE( std::is_trivially_copy_assignable_v<T> );
		CHECK_FALSE( std::is_nothrow_copy_assignable_v<T> );
	}
	SECTION( "Move Traits" ) {
		CHECK( std::is_move_assignable_v<T> );
		CHECK_FALSE( std::is_trivially_move_assignable_v<T> );
		CHECK_FALSE( std::is_nothrow_move_assignable_v<T> );
	}
}

SCENARIO( "Creating a Depreciation record", "[Depreciation]" ) {
	GIVEN( "a default-constructed Depreciation" ) {
		Depreciation depreciation;

		THEN( "it is not loaded" ) {
			CHECK_FALSE( depreciation.IsLoaded() );
		}
	}
}

SCENARIO( "Checking load status", "[Depreciation]" ) {
	GIVEN( "a default-constructed Depreciation" ) {
		Depreciation depreciation;

		WHEN( "checking if loaded" ) {
			THEN( "IsLoaded returns false" ) {
				CHECK_FALSE( depreciation.IsLoaded() );
			}
		}
	}
}

SCENARIO( "Copying a Depreciation record", "[Depreciation]" ) {
	GIVEN( "a default-constructed Depreciation" ) {
		Depreciation original;

		WHEN( "copy-constructing" ) {
			Depreciation copy(original);
			THEN( "the copy has the same load status" ) {
				CHECK_FALSE( copy.IsLoaded() );
			}
		}

		WHEN( "copy-assigning" ) {
			Depreciation copy;
			copy = original;
			THEN( "the copy has the same load status" ) {
				CHECK_FALSE( copy.IsLoaded() );
			}
		}
	}
}

SCENARIO( "Moving a Depreciation record", "[Depreciation]" ) {
	GIVEN( "a default-constructed Depreciation" ) {
		Depreciation original;

		WHEN( "move-constructing" ) {
			Depreciation moved(std::move(original));
			THEN( "the moved object has the expected state" ) {
				CHECK_FALSE( moved.IsLoaded() );
			}
		}

		WHEN( "move-assigning" ) {
			Depreciation moved;
			moved = std::move(original);
			THEN( "the moved object has the expected state" ) {
				CHECK_FALSE( moved.IsLoaded() );
			}
		}
	}
}

// #endregion unit tests



} // test namespace