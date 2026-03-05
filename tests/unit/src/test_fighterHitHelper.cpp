/* test_fighterHitHelper.cpp
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
#include "../../../source/FighterHitHelper.h"

// ... and any system includes needed for the test file.
#include <type_traits>

namespace { // test namespace

// #region mock data

// Insert file-local data here, e.g. classes, structs, or fixtures that will be useful
// to help test this class/method.

// #endregion mock data



// #region unit tests

SCENARIO( "FighterHitHelper class traits", "[FighterHitHelper]" ) {
	GIVEN( "the FighterHitHelper class" ) {
		THEN( "it has the expected construction traits" ) {
			// FighterHitHelper is a utility class with only static methods.
			// It should not be instantiated.
			CHECK( std::is_default_constructible_v<FighterHitHelper> );
			CHECK( std::is_trivially_default_constructible_v<FighterHitHelper> );
		}
		THEN( "it has the expected destructor traits" ) {
			CHECK( std::is_destructible_v<FighterHitHelper> );
			CHECK( std::is_trivially_destructible_v<FighterHitHelper> );
		}
		THEN( "it has the expected copy traits" ) {
			CHECK( std::is_copy_constructible_v<FighterHitHelper> );
			CHECK( std::is_trivially_copy_constructible_v<FighterHitHelper> );
			CHECK( std::is_copy_assignable_v<FighterHitHelper> );
			CHECK( std::is_trivially_copy_assignable_v<FighterHitHelper> );
		}
		THEN( "it has the expected move traits" ) {
			CHECK( std::is_move_constructible_v<FighterHitHelper> );
			CHECK( std::is_trivially_move_constructible_v<FighterHitHelper> );
			CHECK( std::is_move_assignable_v<FighterHitHelper> );
			CHECK( std::is_trivially_move_assignable_v<FighterHitHelper> );
		}
		THEN( "it is a trivial type" ) {
			CHECK( std::is_trivial_v<FighterHitHelper> );
		}
	}
}

// NOTE: The FighterHitHelper::IsValidTarget() method cannot be unit tested because it
// depends on global GameData::GetGamerules() singleton. The method logic is:
//
//   static inline bool IsValidTarget(const Ship *ship) {
//       if(!ship->CanBeCarried() || !ship->IsDisabled())
//           return true;
//       switch(GameData::GetGamerules().FightersHitWhenDisabled()) {
//           case Gamerules::FighterDodgePolicy::ALL: return false;
//           case Gamerules::FighterDodgePolicy::NONE: return true;
//           case Gamerules::FighterDodgePolicy::ONLY_PLAYER: return !ship->IsYours();
//       }
//       return false;
//   }
//
// Testing this would require:
// 1. Mocking or faking the GameData singleton
// 2. Creating a Ship instance with mocked CanBeCarried(), IsDisabled(), and IsYours() methods
//
// The expected behavior based on the source code:
// - If ship cannot be carried OR is not disabled: always returns true (valid target)
// - If ship can be carried AND is disabled:
//   - ALL policy: returns false (fighter dodges all projectiles)
//   - NONE policy: returns true (fighter is always hit)
//   - ONLY_PLAYER policy: returns true for NPC ships, false for player ships
//
// This should be tested via integration tests with proper GameData setup.

// #endregion unit tests



} // test namespace