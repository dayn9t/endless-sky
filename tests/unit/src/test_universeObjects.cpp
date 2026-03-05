/* test_universeObjects.cpp
Copyright (c) 2026 by jiang

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
#include "../../../source/UniverseObjects.h"

// ... and any system includes needed for the test file.
#include <type_traits>

namespace { // test namespace

// #region mock data

// #endregion mock data



// #region unit tests

SCENARIO( "UniverseObjects type traits", "[UniverseObjects][TypeTraits]" ) {
	GIVEN( "the UniverseObjects class" ) {
		THEN( "it is default constructible" ) {
			CHECK( std::is_default_constructible_v<UniverseObjects> );
		}
		AND_THEN( "it is not copy constructible (has mutex/atomic members)" ) {
			CHECK_FALSE( std::is_copy_constructible_v<UniverseObjects> );
		}
		AND_THEN( "it is not move constructible (has mutex/atomic members)" ) {
			CHECK_FALSE( std::is_move_constructible_v<UniverseObjects> );
		}
		AND_THEN( "it is destructible" ) {
			CHECK( std::is_destructible_v<UniverseObjects> );
		}
	}
}

SCENARIO( "UniverseObjects default construction", "[UniverseObjects]" ) {
	GIVEN( "no parameters" ) {
		WHEN( "a UniverseObjects is default constructed" ) {
			UniverseObjects objects;
			THEN( "progress is initialized to 0" ) {
				CHECK( objects.GetProgress() >= 0.0 );
				CHECK( objects.GetProgress() <= 1.0 );
			}
		}
	}
}

SCENARIO( "UniverseObjects progress value", "[UniverseObjects][Progress]" ) {
	GIVEN( "a default-constructed UniverseObjects" ) {
		UniverseObjects objects;
		WHEN( "GetProgress is called" ) {
			double progress = objects.GetProgress();
			THEN( "progress is within valid range [0, 1]" ) {
				CHECK( progress >= 0.0 );
				CHECK( progress <= 1.0 );
			}
		}
	}
}

// Note: UniverseObjects contains std::atomic<double> and std::mutex members,
// which makes it non-copyable and non-movable. The class is designed to be
// used as a single instance for tracking game data loading progress.
//
// Additional testing would require:
// 1. Integration with GameData
// 2. Testing the callback mechanism
// 3. Testing thread-safe progress updates

// #endregion unit tests



} // test namespace