/* test_escortDisplay.cpp
Copyright (c) 2026 by jiang

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
#include "../../../source/EscortDisplay.h"

// ... and any system includes needed for the test file.
#include <type_traits>

namespace { // test namespace

// #region mock data

// EscortDisplay has dependencies on:
// - Ship for escort information
// - PlayerInfo for flagship data
// Full testing requires GameData initialization.

// #endregion mock data



// #region unit tests

SCENARIO( "EscortDisplay type traits", "[EscortDisplay][TypeTraits]" ) {
	GIVEN( "the EscortDisplay class" ) {
		THEN( "it is default constructible" ) {
			CHECK( std::is_default_constructible_v<EscortDisplay> );
		}
		AND_THEN( "it is destructible" ) {
			CHECK( std::is_destructible_v<EscortDisplay> );
		}
	}
}

// Note: Full testing of EscortDisplay requires:
// 1. PlayerInfo with flagship and escorts
// 2. GameData with ship data
// 3. Testing the Draw method
//
// The EscortDisplay class shows the status of escort ships in the HUD.

// #endregion unit tests



} // test namespace