/* test_weapon.cpp
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
#include "../../../source/Weapon.h"

// ... and any system includes needed for the test file.
#include <cstdint>
#include <memory>
#include <type_traits>

namespace { // test namespace

// #region mock data

// Weapon has heavy dependencies:
// - Body for sprites
// - Distribution for inaccuracy
// - DataNode for loading
//
// The Submunition struct has a reference member, making Weapon non-copy-assignable.
// Full testing requires loading weapon data from game files.

// #endregion mock data



// #region unit tests

SCENARIO( "Weapon type traits", "[Weapon][TypeTraits]" ) {
	GIVEN( "the Weapon class" ) {
		THEN( "it is default constructible" ) {
			CHECK( std::is_default_constructible_v<Weapon> );
		}
		AND_THEN( "it is copy constructible" ) {
			CHECK( std::is_copy_constructible_v<Weapon> );
		}
		AND_THEN( "it is move constructible" ) {
			CHECK( std::is_move_constructible_v<Weapon> );
		}
		AND_THEN( "it is destructible" ) {
			CHECK( std::is_destructible_v<Weapon> );
		}
	}
}

SCENARIO( "Weapon default construction", "[Weapon]" ) {
	GIVEN( "no parameters" ) {
		WHEN( "a Weapon is default constructed" ) {
			Weapon weapon;
			THEN( "it has default values" ) {
				CHECK( weapon.Velocity() == 0.0 );
				CHECK( weapon.Reload() == 1.0 );
				CHECK( weapon.Lifetime() == 0 );
			}
		}
	}
}

// Note: Full testing of Weapon requires:
// 1. Loading weapon data from game files
// 2. Testing damage values
// 3. Testing Submunition behavior
//
// The Weapon class is complex and typically loaded from game data.

// #endregion unit tests



} // test namespace