/* test_visual.cpp
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
#include "../../../source/Visual.h"

// ... and any system includes needed for the test file.
#include <type_traits>

namespace { // test namespace

// #region mock data

// Visual has heavy dependencies:
// - Effect for construction
// - Body for position/angle
// - Point for coordinates
//
// Full testing requires integration with the game engine.

// #endregion mock data



// #region unit tests

SCENARIO( "Visual type traits", "[Visual][TypeTraits]" ) {
	GIVEN( "the Visual class" ) {
		THEN( "it is not default constructible" ) {
			CHECK_FALSE( std::is_default_constructible_v<Visual> );
		}
		AND_THEN( "it is destructible" ) {
			CHECK( std::is_destructible_v<Visual> );
		}
	}
}

// Note: Full testing of Visual requires:
// 1. An Effect object with sprite data
// 2. Position and angle information
// 3. Integration with the rendering system
//
// The Visual class is a simple effect object that displays a sprite.

// #endregion unit tests



} // test namespace