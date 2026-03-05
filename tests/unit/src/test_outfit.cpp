/* test_outfit.cpp
Copyright (c) 2025 by the Endless Sky community

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
#include "../../../source/Outfit.h"

// Include Body to avoid incomplete type issues with FlareSprites.
#include "../../../source/Body.h"

// ... and any system includes needed for the test file.
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace { // test namespace

// #region mock data

// #endregion mock data



// #region unit tests

SCENARIO( "Outfit type traits", "[Outfit][TypeTraits]" ) {
	GIVEN( "the Outfit class" ) {
		THEN( "it is default constructible" ) {
			CHECK( std::is_default_constructible_v<Outfit> );
		}
		AND_THEN( "it is copy constructible" ) {
			CHECK( std::is_copy_constructible_v<Outfit> );
		}
		AND_THEN( "it is move constructible" ) {
			CHECK( std::is_move_constructible_v<Outfit> );
		}
		AND_THEN( "it is destructible" ) {
			CHECK( std::is_destructible_v<Outfit> );
		}
	}
}

SCENARIO( "Outfit default construction", "[Outfit]" ) {
	GIVEN( "no parameters" ) {
		WHEN( "an Outfit is default constructed" ) {
			Outfit outfit;
			THEN( "it has default values" ) {
				CHECK( outfit.Mass() == 0.0 );
				CHECK( outfit.Cost() == 0 );
			}
		}
	}
}

// Note: Full testing of Outfit requires:
// 1. Loading outfit data from game files
// 2. Testing Get() for attribute values
// 3. Testing Weapon-related methods
// 4. Testing FlareSprites access
//
// The Outfit class is complex and typically loaded from game data.

// #endregion unit tests



} // test namespace