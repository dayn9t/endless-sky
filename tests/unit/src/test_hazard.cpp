/* test_hazard.cpp
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
#include "../../../source/Hazard.h"

// ... and any system includes needed for the test file.
#include <type_traits>

namespace { // test namespace

// #region mock data

// Hazard depends on Weapon which contains non-copyable Submunition struct.
// Full testing requires GameData initialization.

// #endregion mock data



// #region unit tests

SCENARIO( "Hazard type traits", "[Hazard][TypeTraits]" ) {
	GIVEN( "the Hazard class" ) {
		THEN( "it is default constructible" ) {
			CHECK( std::is_default_constructible_v<Hazard> );
		}
		AND_THEN( "it is destructible" ) {
			CHECK( std::is_destructible_v<Hazard> );
		}
	}
}

// Note: Full testing of Hazard requires:
// 1. GameData initialized with hazards and weapons
// 2. Testing hazard damage and effects
// 3. Testing period and deviating behavior
//
// The Hazard class represents environmental hazards in the game.

// #endregion unit tests



} // test namespace