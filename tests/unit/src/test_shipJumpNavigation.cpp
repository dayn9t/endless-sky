/* test_shipJumpNavigation.cpp
Copyright (c) 2025 by jiang

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
#include "../../../source/ShipJumpNavigation.h"

// ... and any system includes needed for the test file.
#include <type_traits>

namespace { // test namespace

// #region mock data

// ShipJumpNavigation depends on Ship, Outfit, and System classes which require
// GameData initialization. Full integration tests would be needed to test
// the Calibrate and Recalibrate methods with real ship data.

// #endregion mock data



// #region unit tests

SCENARIO( "ShipJumpNavigation type traits", "[ShipJumpNavigation][TypeTraits]" ) {
	GIVEN( "the ShipJumpNavigation class" ) {
		THEN( "it is default constructible" ) {
			CHECK( std::is_default_constructible_v<ShipJumpNavigation> );
		}
		AND_THEN( "it is copy constructible" ) {
			CHECK( std::is_copy_constructible_v<ShipJumpNavigation> );
		}
		AND_THEN( "it is move constructible" ) {
			CHECK( std::is_move_constructible_v<ShipJumpNavigation> );
		}
		AND_THEN( "it is destructible" ) {
			CHECK( std::is_destructible_v<ShipJumpNavigation> );
		}
	}
}

// Note: Full testing of ShipJumpNavigation requires:
// 1. GameData initialized with systems and outfits
// 2. Ship with jump drives
// 3. Testing Calibrate and Recalibrate methods
//
// The ShipJumpNavigation class manages jump capabilities for ships.

// #endregion unit tests



} // test namespace