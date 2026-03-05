/* test_startConditionsPanel.cpp
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
#include "../../../source/StartConditionsPanel.h"

// ... and any system includes needed for the test file.
#include <type_traits>

namespace { // test namespace

// #region mock data

// StartConditionsPanel has heavy dependencies:
// - PlayerInfo (required reference)
// - StartConditions (requires Ship)
// - Audio (static methods)
// - UI (via Panel base class)
//
// Full integration testing requires mocking these dependencies.

// #endregion mock data



// #region unit tests

SCENARIO( "StartConditionsPanel type traits", "[StartConditionsPanel][TypeTraits]" ) {
	GIVEN( "the StartConditionsPanel class" ) {
		THEN( "it is not default constructible" ) {
			CHECK_FALSE( std::is_default_constructible_v<StartConditionsPanel> );
		}
		AND_THEN( "it is not copy constructible" ) {
			CHECK_FALSE( std::is_copy_constructible_v<StartConditionsPanel> );
		}
		AND_THEN( "it is not move constructible" ) {
			CHECK_FALSE( std::is_move_constructible_v<StartConditionsPanel> );
		}
		AND_THEN( "it is destructible" ) {
			CHECK( std::is_destructible_v<StartConditionsPanel> );
		}
		AND_THEN( "it inherits from Panel" ) {
			CHECK( std::is_base_of_v<Panel, StartConditionsPanel> );
		}
	}
}

// Note: Full testing of StartConditionsPanel requires:
// 1. A PlayerInfo instance
// 2. StartConditions loaded from game data
// 3. Audio system initialized
// 4. GameData initialized
//
// The StartConditionsPanel is a UI panel for selecting starting conditions.

// #endregion unit tests



} // test namespace