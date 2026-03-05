/* test_infoPanelState.cpp
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
#include "../../../source/InfoPanelState.h"

// ... and any system includes needed for the test file.
#include <type_traits>

namespace { // test namespace

// #region mock data

// InfoPanelState has dependencies on:
// - PlayerInfo for ship selection
// - Ship for escort/ship data
// Full testing requires GameData initialization.

// #endregion mock data



// #region unit tests

SCENARIO( "InfoPanelState type traits", "[InfoPanelState][TypeTraits]" ) {
	GIVEN( "the InfoPanelState class" ) {
		THEN( "it is not default constructible (requires PlayerInfo reference)" ) {
			CHECK_FALSE( std::is_default_constructible_v<InfoPanelState> );
		}
		AND_THEN( "it is copy constructible" ) {
			CHECK( std::is_copy_constructible_v<InfoPanelState> );
		}
		AND_THEN( "it is move constructible" ) {
			CHECK( std::is_move_constructible_v<InfoPanelState> );
		}
		AND_THEN( "it is destructible" ) {
			CHECK( std::is_destructible_v<InfoPanelState> );
		}
	}
}

// Note: Full testing of InfoPanelState requires:
// 1. PlayerInfo with ships
// 2. GameData initialized
// 3. Testing selection and navigation
//
// The InfoPanelState class manages the state of info panels.

// #endregion unit tests



} // test namespace