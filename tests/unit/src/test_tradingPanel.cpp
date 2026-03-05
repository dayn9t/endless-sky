/* test_tradingPanel.cpp
Copyright (c) 2026 by Jiang

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
#include "../../../source/TradingPanel.h"

// ... and any system includes needed for the test file.
#include <type_traits>

namespace { // test namespace

// #region mock data

// TradingPanel has significant dependencies on GameData, including:
// - GameData::Commodities() for commodity list
// - GameData::Interfaces() for UI layout
// - GameData::Colors() for rendering
// - PlayerInfo must have a valid System (GetSystem() != nullptr)
//
// Full integration testing requires loading game data.

// #endregion mock data



// #region unit tests

SCENARIO( "TradingPanel type traits", "[TradingPanel][TypeTraits]" ) {
	GIVEN( "the TradingPanel class" ) {
		THEN( "it is not default constructible" ) {
			CHECK_FALSE( std::is_default_constructible_v<TradingPanel> );
		}
		AND_THEN( "it is not copy constructible" ) {
			CHECK_FALSE( std::is_copy_constructible_v<TradingPanel> );
		}
		AND_THEN( "it is not move constructible" ) {
			CHECK_FALSE( std::is_move_constructible_v<TradingPanel> );
		}
		AND_THEN( "it is destructible" ) {
			CHECK( std::is_destructible_v<TradingPanel> );
		}
		AND_THEN( "it inherits from Panel" ) {
			CHECK( std::is_base_of_v<Panel, TradingPanel> );
		}
	}
}

// Note: Full testing of TradingPanel requires:
// 1. A PlayerInfo instance with a valid system
// 2. GameData initialized with commodities
// 3. UI system initialized
//
// The TradingPanel is a UI panel for buying and selling commodities.

// #endregion unit tests



} // test namespace