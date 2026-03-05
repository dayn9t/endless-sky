/* test_ai.cpp
Copyright (c) 2026 by the Endless Sky community

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
#include "../../../source/AI.h"

// Include system headers for testing.
#include <list>
#include <memory>
#include <type_traits>

namespace { // test namespace

// #region mock data

// #endregion mock data



// #region unit tests

SCENARIO( "AI class design constraints", "[AI]" ) {
	GIVEN( "the AI class" ) {
		THEN( "it is not default constructible" ) {
			CHECK_FALSE( std::is_default_constructible_v<AI> );
		}
		AND_THEN( "it requires PlayerInfo reference" ) {
			// AI has a reference member to PlayerInfo, so it cannot be default constructed
			// and must be constructed with valid references
			CHECK_FALSE( std::is_trivially_constructible_v<AI> );
		}
		AND_THEN( "it is technically copy constructible (type traits allow it despite reference member)" ) {
			// AI contains reference members, but type traits report copy constructible as true
			// because the compiler doesn't check implicit constructor validity until instantiation.
			CHECK( std::is_copy_constructible_v<AI> );
		}
		AND_THEN( "it is not copy assignable" ) {
			// Reference members prevent copy assignment
			CHECK_FALSE( std::is_copy_assignable_v<AI> );
		}
		AND_THEN( "it is technically move constructible (type traits allow it despite reference member)" ) {
			// AI has reference members, but type traits report move constructible as true
			CHECK( std::is_move_constructible_v<AI> );
		}
		AND_THEN( "it is not move assignable" ) {
			// Reference members prevent move assignment
			CHECK_FALSE( std::is_move_assignable_v<AI> );
		}
		AND_THEN( "it is destructible" ) {
			CHECK( std::is_destructible_v<AI> );
		}
	}
}

SCENARIO( "AI::List template alias", "[AI][List]" ) {
	GIVEN( "the AI::List template alias" ) {
		THEN( "it is equivalent to std::list of shared_ptr" ) {
			using TestList = AI::List<int>;
			CHECK( std::is_same_v<TestList, std::list<std::shared_ptr<int>>> );
		}
		AND_THEN( "it can be used with Ship type" ) {
			using ShipList = AI::List<Ship>;
			CHECK( std::is_same_v<ShipList, std::list<std::shared_ptr<Ship>>> );
		}
	}
}

// Note: The AI class constructor requires PlayerInfo, List<Ship>, List<Minable>, List<Flotsam>
// which all depend on full GameData initialization. Testing the AI class directly
// would require extensive mocking or integration testing setup.
//
// The FindLandingLocation static method can be tested but requires a valid Ship with
// system information.

// #endregion unit tests



} // test namespace