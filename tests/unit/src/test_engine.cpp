/* test_engine.cpp
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
#include "../../../source/Engine.h"

// ... and any system includes needed for the test file.
#include <type_traits>

namespace { // test namespace

// #region mock data

// Engine has very heavy dependencies:
// - PlayerInfo (required reference)
// - GameData, rendering systems
// - Visual, Ship, and many game objects
//
// Full testing requires integration testing with the game engine.

// #endregion mock data



// #region unit tests

SCENARIO( "Engine type traits", "[Engine][TypeTraits]" ) {
	GIVEN( "the Engine class" ) {
		THEN( "it is not default constructible" ) {
			CHECK_FALSE( std::is_default_constructible_v<Engine> );
		}
		AND_THEN( "it is not copy constructible" ) {
			CHECK_FALSE( std::is_copy_constructible_v<Engine> );
		}
		AND_THEN( "it is not move constructible" ) {
			CHECK_FALSE( std::is_move_constructible_v<Engine> );
		}
		AND_THEN( "it is destructible" ) {
			CHECK( std::is_destructible_v<Engine> );
		}
	}
}

// Note: Full testing of Engine requires:
// 1. A PlayerInfo instance with game state
// 2. GameData fully initialized
// 3. Rendering system initialized
//
// The Engine class is the core game loop that handles:
// - Rendering
// - Physics
// - Input handling
// - Game state management

// #endregion unit tests



} // test namespace