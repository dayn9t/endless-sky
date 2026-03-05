/* test_shopPanel.cpp
Copyright (c) 2026 by test contributor

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
#include "../../../source/ShopPanel.h"

// ... and any system includes needed for the test file.
#include <type_traits>

namespace { // test namespace

// #region mock data

// ShopPanel is an abstract base class that requires:
// - PlayerInfo reference
// - Derived class implementations for pure virtual methods
//
// Full testing requires integration with GameData and PlayerInfo.

// #endregion mock data



// #region unit tests

SCENARIO( "ShopPanel type traits", "[ShopPanel][TypeTraits]" ) {
	GIVEN( "the ShopPanel class" ) {
		THEN( "it is not default constructible" ) {
			CHECK_FALSE( std::is_default_constructible_v<ShopPanel> );
		}
		AND_THEN( "it is not copy constructible" ) {
			CHECK_FALSE( std::is_copy_constructible_v<ShopPanel> );
		}
		AND_THEN( "it is not move constructible" ) {
			CHECK_FALSE( std::is_move_constructible_v<ShopPanel> );
		}
		AND_THEN( "it is destructible" ) {
			CHECK( std::is_destructible_v<ShopPanel> );
		}
		AND_THEN( "it is an abstract class" ) {
			CHECK( std::is_abstract_v<ShopPanel> );
		}
	}
}

// Note: Full testing of ShopPanel requires:
// 1. A PlayerInfo instance with game state
// 2. Derived classes like OutfitterPanel or ShipyardPanel
// 3. GameData initialized with shop data
//
// The ShopPanel is an abstract base class for shop UI panels.
// Protected members like BUTTON_COL_PAD, ShopPane, Zone, and TransactionResult
// can only be accessed from derived classes.

// #endregion unit tests



} // test namespace