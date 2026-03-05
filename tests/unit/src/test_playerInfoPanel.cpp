/* test_playerInfoPanel.cpp
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
#include "../../../source/PlayerInfoPanel.h"

// ... and any system includes needed for the test file.
#include <type_traits>

namespace { // test namespace

// #region mock data

// PlayerInfoPanel has extensive dependencies:
// - PlayerInfo (with ships, accounts, conditions)
// - InfoPanelState (for maintaining state across panels)
// - Ship class (requires GameData for model, outfits)
// Full testing requires GameData initialization.

// #endregion mock data



// #region unit tests

SCENARIO( "PlayerInfoPanel type traits", "[PlayerInfoPanel][TypeTraits]" ) {
	GIVEN( "the PlayerInfoPanel class" ) {
		THEN( "it is not default constructible" ) {
			CHECK_FALSE( std::is_default_constructible_v<PlayerInfoPanel> );
		}
		AND_THEN( "it is technically copy constructible (type traits allow it despite reference member)" ) {
			// PlayerInfoPanel has reference members, but type traits report true
			CHECK( std::is_copy_constructible_v<PlayerInfoPanel> );
		}
		AND_THEN( "it is technically move constructible (type traits allow it despite reference member)" ) {
			// PlayerInfoPanel has reference members, but type traits report true
			CHECK( std::is_move_constructible_v<PlayerInfoPanel> );
		}
		AND_THEN( "it is destructible" ) {
			CHECK( std::is_destructible_v<PlayerInfoPanel> );
		}
		AND_THEN( "it inherits from Panel" ) {
			CHECK( std::is_base_of_v<Panel, PlayerInfoPanel> );
		}
	}
}

// Note: Full testing of PlayerInfoPanel requires:
// 1. PlayerInfo with ships
// 2. GameData initialized
// 3. Font system initialized
// 4. Screen dimensions set
//
// The PlayerInfoPanel class shows player information in the UI.

// #endregion unit tests



} // test namespace