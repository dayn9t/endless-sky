/* test_damageProfile.cpp
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
#include "../../../source/DamageProfile.h"

// ... and any system includes needed for the test file.
#include "../../../source/DamageDealt.h"
#include "../../../source/MinableDamageDealt.h"
#include "../../../source/Point.h"
#include "../../../source/Projectile.h"
#include "../../../source/Weather.h"
#include "../../../source/Weapon.h"

#include <type_traits>

namespace { // test namespace

// #region mock data

// Note: The Weapon class requires data to be loaded from game files.
// Testing DamageProfile's actual damage calculation requires a fully loaded Weapon.
// We can only test the class structure and basic construction here.

// #endregion mock data



// #region unit tests

SCENARIO( "DamageProfile class traits", "[DamageProfile]" ) {
	GIVEN( "the DamageProfile class" ) {
		THEN( "it is not default constructible" ) {
			CHECK_FALSE( std::is_default_constructible_v<DamageProfile> );
		}
		AND_THEN( "it is technically copy constructible (type traits allow it despite const Weapon& member)" ) {
			// DamageProfile has const Weapon& member, but type traits report true
			CHECK( std::is_copy_constructible_v<DamageProfile> );
		}
		AND_THEN( "it is technically move constructible (type traits allow it despite const Weapon& member)" ) {
			// DamageProfile has const Weapon& member, but type traits report true
			CHECK( std::is_move_constructible_v<DamageProfile> );
		}
		AND_THEN( "it is not copy assignable" ) {
			CHECK_FALSE( std::is_copy_assignable_v<DamageProfile> );
		}
		AND_THEN( "it is not move assignable" ) {
			CHECK_FALSE( std::is_move_assignable_v<DamageProfile> );
		}
		AND_THEN( "it is destructible" ) {
			CHECK( std::is_destructible_v<DamageProfile> );
		}
	}
}

SCENARIO( "DamageDealt class traits", "[DamageDealt]" ) {
	GIVEN( "the DamageDealt class" ) {
		THEN( "it is default constructible" ) {
			CHECK( std::is_default_constructible_v<DamageDealt> );
		}
		AND_THEN( "it is copy constructible" ) {
			CHECK( std::is_copy_constructible_v<DamageDealt> );
		}
		AND_THEN( "it is move constructible" ) {
			CHECK( std::is_move_constructible_v<DamageDealt> );
		}
		AND_THEN( "it is destructible" ) {
			CHECK( std::is_destructible_v<DamageDealt> );
		}
	}
}

// Note: MinableDamageDealt is tested in test_minableDamageDealt.cpp

SCENARIO( "Projectile::ImpactInfo class traits", "[Projectile][ImpactInfo]" ) {
	GIVEN( "the Projectile::ImpactInfo class" ) {
		THEN( "it is not default constructible" ) {
			CHECK_FALSE( std::is_default_constructible_v<Projectile::ImpactInfo> );
		}
		AND_THEN( "it is copy constructible" ) {
			CHECK( std::is_copy_constructible_v<Projectile::ImpactInfo> );
		}
		AND_THEN( "it is destructible" ) {
			CHECK( std::is_destructible_v<Projectile::ImpactInfo> );
		}
	}
}

SCENARIO( "Weather::ImpactInfo class traits", "[Weather][ImpactInfo]" ) {
	GIVEN( "the Weather::ImpactInfo class" ) {
		THEN( "it is not default constructible" ) {
			CHECK_FALSE( std::is_default_constructible_v<Weather::ImpactInfo> );
		}
		AND_THEN( "it is copy constructible" ) {
			CHECK( std::is_copy_constructible_v<Weather::ImpactInfo> );
		}
		AND_THEN( "it is destructible" ) {
			CHECK( std::is_destructible_v<Weather::ImpactInfo> );
		}
	}
}

// Note: Full DamageProfile testing requires Weapon instances loaded from game data.
// The DamageProfile constructor uses Weapon's public accessor methods to get
// blast radius, trigger radius, damage scaling, and damage values.
//
// To properly test DamageProfile, one would need:
// 1. A valid Weapon loaded from game data files
// 2. A valid Ship with shields, hull, and energy attributes
// 3. Optionally, a Minable object for minable damage testing
//
// The following scenarios document the expected behavior:
//
// DamageProfile(Projectile::ImpactInfo):
// - Takes weapon, position, and distance traveled
// - Calculates blast parameters if weapon has blast radius and damage scaling
// - Applies damage dropoff if weapon has damage dropoff enabled
//
// DamageProfile(Weather::ImpactInfo):
// - Takes weapon, position, and scale factor
// - Sets isHazard flag to true
// - Blast calculations similar to projectile path
//
// CalculateDamage(Body&, Ship&):
// - Returns DamageDealt with scaled damage values
// - Applies blast radius falloff if applicable
// - Applies damage dropoff for hazards based on ship position

// #endregion unit tests



} // test namespace