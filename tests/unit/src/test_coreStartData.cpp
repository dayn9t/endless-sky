/* test_coreStartData.cpp
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
#include "../../../source/CoreStartData.h"

// ... and any system includes needed for the test file.
#include <type_traits>

namespace { // test namespace

// #region mock data

// Note: The CoreStartData class has significant dependencies on GameData:
// - Load() requires GameData::Systems().Get() and GameData::Planets().Get()
// - Save() requires system->TrueName() and planet->TrueName()
// - GetPlanet() requires GameData::Planets().Get("New Boston")
// - GetSystem() requires GameData::Systems().Get("Rutilicus") and planet->GetSystem()
//
// Per the unit test README: "Generally speaking, if a method invokes the GameData
// class, it will not be testable using unit tests."
//
// The following can be tested without GameData:
// - Default construction (all members initialized to default values)
// - GetDate() with default value (16 Nov 3013)
// - GetAccounts() returns a valid Account reference
// - Identifier() returns the identifier string
// - Class type traits

// #endregion mock data



// #region unit tests

TEST_CASE( "CoreStartData Class Traits", "[CoreStartData]" ) {
	using T = CoreStartData;
	SECTION( "Class Traits" ) {
		CHECK_FALSE( std::is_trivial_v<T> );
		// Has std::string member, not standard_layout
		CHECK_FALSE( std::is_standard_layout_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
		// Has std::string member, not trivially destructible
		CHECK_FALSE( std::is_trivially_destructible_v<T> );
	}
	SECTION( "Construction Traits" ) {
		CHECK( std::is_default_constructible_v<T> );
		// Has std::string member, not trivially default constructible
		CHECK_FALSE( std::is_trivially_default_constructible_v<T> );
		CHECK_FALSE( std::is_nothrow_default_constructible_v<T> );
		CHECK( std::is_copy_constructible_v<T> );
		// std::string copy is not trivial
		CHECK_FALSE( std::is_trivially_copy_constructible_v<T> );
		CHECK_FALSE( std::is_nothrow_copy_constructible_v<T> );
		CHECK( std::is_move_constructible_v<T> );
		CHECK_FALSE( std::is_trivially_move_constructible_v<T> );
		CHECK_FALSE( std::is_nothrow_move_constructible_v<T> );
	}
	SECTION( "Copy Traits" ) {
		CHECK( std::is_copy_assignable_v<T> );
		// std::string is not trivially copyable
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

SCENARIO( "Creating a CoreStartData", "[CoreStartData][Creation]" ) {
	GIVEN( "a default-constructed CoreStartData" ) {
		CoreStartData data;

		THEN( "the identifier is empty" ) {
			REQUIRE( data.Identifier().empty() );
		}
	}
}

SCENARIO( "Getting the default date", "[CoreStartData][GetDate]" ) {
	GIVEN( "a default-constructed CoreStartData" ) {
		CoreStartData data;

		WHEN( "calling GetDate()" ) {
			Date date = data.GetDate();

			THEN( "returns the default date 16 Nov 3013" ) {
				REQUIRE( date.Day() == 16 );
				REQUIRE( date.Month() == 11 );
				REQUIRE( date.Year() == 3013 );
			}
		}
	}
}

SCENARIO( "Getting accounts", "[CoreStartData][GetAccounts]" ) {
	GIVEN( "a default-constructed CoreStartData" ) {
		CoreStartData data;

		WHEN( "calling GetAccounts()" ) {
			const Account &accounts = data.GetAccounts();

			THEN( "returns a valid Account reference" ) {
				// Default Account has 0 credits
				REQUIRE( accounts.Credits() == 0 );
				REQUIRE( accounts.TotalDebt() == 0 );
			}
		}
	}
}

SCENARIO( "Getting the identifier", "[CoreStartData][Identifier]" ) {
	GIVEN( "a default-constructed CoreStartData" ) {
		CoreStartData data;

		WHEN( "calling Identifier()" ) {
			const std::string &id = data.Identifier();

			THEN( "returns an empty string" ) {
				REQUIRE( id.empty() );
			}
		}
	}
}

SCENARIO( "Copying a CoreStartData", "[CoreStartData][Copy]" ) {
	GIVEN( "a default-constructed CoreStartData" ) {
		CoreStartData original;

		WHEN( "copy-constructing" ) {
			CoreStartData copy(original);

			THEN( "the copy has the same default values" ) {
				REQUIRE( copy.Identifier().empty() );
				REQUIRE( copy.GetDate().Day() == 16 );
				REQUIRE( copy.GetDate().Month() == 11 );
				REQUIRE( copy.GetDate().Year() == 3013 );
				REQUIRE( copy.GetAccounts().Credits() == 0 );
			}
		}

		WHEN( "copy-assigning" ) {
			CoreStartData copy;
			copy = original;

			THEN( "the copy has the same default values" ) {
				REQUIRE( copy.Identifier().empty() );
				REQUIRE( copy.GetDate().Day() == 16 );
				REQUIRE( copy.GetDate().Month() == 11 );
				REQUIRE( copy.GetDate().Year() == 3013 );
				REQUIRE( copy.GetAccounts().Credits() == 0 );
			}
		}
	}
}

SCENARIO( "Moving a CoreStartData", "[CoreStartData][Move]" ) {
	GIVEN( "a default-constructed CoreStartData" ) {
		CoreStartData original;

		WHEN( "move-constructing" ) {
			CoreStartData moved(std::move(original));

			THEN( "the moved object has the expected default values" ) {
				REQUIRE( moved.Identifier().empty() );
				REQUIRE( moved.GetDate().Day() == 16 );
				REQUIRE( moved.GetDate().Month() == 11 );
				REQUIRE( moved.GetDate().Year() == 3013 );
				REQUIRE( moved.GetAccounts().Credits() == 0 );
			}
		}

		WHEN( "move-assigning" ) {
			CoreStartData moved;
			moved = std::move(original);

			THEN( "the moved object has the expected default values" ) {
				REQUIRE( moved.Identifier().empty() );
				REQUIRE( moved.GetDate().Day() == 16 );
				REQUIRE( moved.GetDate().Month() == 11 );
				REQUIRE( moved.GetDate().Year() == 3013 );
				REQUIRE( moved.GetAccounts().Credits() == 0 );
			}
		}
	}
}

SCENARIO( "GetAccounts returns a reference", "[CoreStartData][GetAccounts]" ) {
	GIVEN( "a default-constructed CoreStartData" ) {
		CoreStartData data;

		WHEN( "calling GetAccounts multiple times" ) {
			const Account &ref1 = data.GetAccounts();
			const Account &ref2 = data.GetAccounts();

			THEN( "returns the same reference" ) {
				REQUIRE( &ref1 == &ref2 );
			}
		}
	}
}

SCENARIO( "Identifier returns a reference", "[CoreStartData][Identifier]" ) {
	GIVEN( "a default-constructed CoreStartData" ) {
		CoreStartData data;

		WHEN( "calling Identifier multiple times" ) {
			const std::string &ref1 = data.Identifier();
			const std::string &ref2 = data.Identifier();

			THEN( "returns the same reference" ) {
				REQUIRE( &ref1 == &ref2 );
			}
		}
	}
}

// #endregion unit tests



} // test namespace