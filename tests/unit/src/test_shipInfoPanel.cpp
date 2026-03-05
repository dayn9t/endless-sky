/* test_shipInfoPanel.cpp
Copyright (c) 2025 by Endless Sky contributors

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
#include "../../../source/ShipInfoPanel.h"

// ... and any system includes needed for the test file.
#include <memory>
#include <type_traits>

namespace { // test namespace

// #region mock data

// ShipInfoPanel has heavy dependencies on:
// - PlayerInfo (required reference)
// - InfoPanelState (required for second constructor)
// - Audio (static methods called in ctor/dtor)
// - GameData (static methods called in Draw)
// - UI (via Panel base class)
//
// Full integration testing requires mocking these dependencies.
// Below we test what can be tested without full mocking infrastructure.

// #endregion mock data



// #region unit tests

// Test type traits and class properties
SCENARIO( "ShipInfoPanel type traits", "[ShipInfoPanel][TypeTraits]" ) {
	GIVEN( "the ShipInfoPanel class" ) {
		THEN( "it is not default constructible" ) {
			CHECK_FALSE( std::is_default_constructible_v<ShipInfoPanel> );
		}
		AND_THEN( "it is not copy constructible" ) {
			CHECK_FALSE( std::is_copy_constructible_v<ShipInfoPanel> );
		}
		AND_THEN( "it is not move constructible" ) {
			CHECK_FALSE( std::is_move_constructible_v<ShipInfoPanel> );
		}
		AND_THEN( "it is destructible" ) {
			CHECK( std::is_destructible_v<ShipInfoPanel> );
		}
	}
}

// Note: Full testing of ShipInfoPanel requires:
// 1. A PlayerInfo instance with ships
// 2. InfoPanelState for the second constructor
// 3. GameData initialized with ship data
// 4. Audio system initialized
//
// The ShipInfoPanel is a complex UI panel that displays detailed ship information.
// It inherits from Panel and has protected virtual methods for interaction.
//
// Key methods that would need integration testing:
// - Constructor with PlayerInfo&
// - Constructor with PlayerInfo& and InfoPanelState
// - Draw() method
// - KeyDown(), Click(), Hover(), Drag(), Release() event handlers (protected)

// #endregion unit tests



} // test namespace