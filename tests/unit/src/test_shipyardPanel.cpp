/* test_shipyardPanel.cpp
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
#include "../../../source/ShipyardPanel.h"

// Include related class headers for testing.
#include "../../../source/ShopPanel.h"

// ... and any system includes needed for the test file.
#include <type_traits>

namespace { // test namespace

// #region mock data

// ShipyardPanel has heavy dependencies:
// - PlayerInfo (required reference)
// - GameData with ship data
// - UI system
//
// Full integration testing requires mocking these dependencies.

// #endregion mock data



// #region unit tests

SCENARIO( "ShipyardPanel type traits", "[ShipyardPanel][TypeTraits]" ) {
	GIVEN( "the ShipyardPanel class" ) {
		THEN( "it is not default constructible" ) {
			CHECK_FALSE( std::is_default_constructible_v<ShipyardPanel> );
		}
		AND_THEN( "it is not copy constructible" ) {
			CHECK_FALSE( std::is_copy_constructible_v<ShipyardPanel> );
		}
		AND_THEN( "it is not move constructible" ) {
			CHECK_FALSE( std::is_move_constructible_v<ShipyardPanel> );
		}
		AND_THEN( "it is destructible" ) {
			CHECK( std::is_destructible_v<ShipyardPanel> );
		}
		AND_THEN( "it inherits from ShopPanel" ) {
			CHECK( std::is_base_of_v<ShopPanel, ShipyardPanel> );
		}
		AND_THEN( "it is polymorphic" ) {
			CHECK( std::is_polymorphic_v<ShipyardPanel> );
		}
	}
}

// Note: Full testing of ShipyardPanel requires:
// 1. A PlayerInfo instance
// 2. GameData initialized with ship data
// 3. UI system initialized
//
// The ShipyardPanel is a UI panel for buying and selling ships.

// #endregion unit tests



} // test namespace