/* test_CategoryType.cpp
Copyright (c) 2025 by the Endless Sky contributors

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
#include "../../../source/CategoryType.h"

// ... and any system includes needed for the test file.
#include <type_traits>

namespace { // test namespace

// #region mock data
// #endregion mock data



// #region unit tests
SCENARIO( "CategoryType enum values are defined correctly", "[CategoryType]" ) {
	GIVEN( "the CategoryType enum" ) {
		THEN( "SHIP has value 0" ) {
			REQUIRE( static_cast<int>(CategoryType::SHIP) == 0 );
		}
		THEN( "BAY has value 1" ) {
			REQUIRE( static_cast<int>(CategoryType::BAY) == 1 );
		}
		THEN( "OUTFIT has value 2" ) {
			REQUIRE( static_cast<int>(CategoryType::OUTFIT) == 2 );
		}
		THEN( "SERIES has value 3" ) {
			REQUIRE( static_cast<int>(CategoryType::SERIES) == 3 );
		}
	}
}

SCENARIO( "CategoryType is a scoped enum", "[CategoryType]" ) {
	GIVEN( "the CategoryType type" ) {
		THEN( "it is an enum class" ) {
			REQUIRE( std::is_enum_v<CategoryType> );
			REQUIRE( std::is_enum_v<CategoryType> );
		}
		THEN( "it has int as underlying type" ) {
			REQUIRE( std::is_same_v<std::underlying_type_t<CategoryType>, int> );
		}
	}
}

SCENARIO( "CategoryType can be used in comparisons", "[CategoryType]" ) {
	GIVEN( "two CategoryType values" ) {
		WHEN( "they are the same" ) {
			THEN( "they compare equal" ) {
				REQUIRE( CategoryType::SHIP == CategoryType::SHIP );
				REQUIRE( CategoryType::BAY == CategoryType::BAY );
				REQUIRE( CategoryType::OUTFIT == CategoryType::OUTFIT );
				REQUIRE( CategoryType::SERIES == CategoryType::SERIES );
			}
		}
		WHEN( "they are different" ) {
			THEN( "they compare not equal" ) {
				REQUIRE( CategoryType::SHIP != CategoryType::BAY );
				REQUIRE( CategoryType::BAY != CategoryType::OUTFIT );
				REQUIRE( CategoryType::OUTFIT != CategoryType::SERIES );
				REQUIRE( CategoryType::SERIES != CategoryType::SHIP );
			}
		}
	}
}

SCENARIO( "CategoryType can be used in switch statements", "[CategoryType]" ) {
	GIVEN( "a CategoryType value" ) {
		WHEN( "using it in a switch" ) {
			THEN( "all cases are handled correctly" ) {
				auto getValue = [](CategoryType type) -> int {
					switch(type) {
						case CategoryType::SHIP: return 1;
						case CategoryType::BAY: return 2;
						case CategoryType::OUTFIT: return 3;
						case CategoryType::SERIES: return 4;
					}
					return 0;
				};

				REQUIRE( getValue(CategoryType::SHIP) == 1 );
				REQUIRE( getValue(CategoryType::BAY) == 2 );
				REQUIRE( getValue(CategoryType::OUTFIT) == 3 );
				REQUIRE( getValue(CategoryType::SERIES) == 4 );
			}
		}
	}
}
// #endregion unit tests



} // test namespace