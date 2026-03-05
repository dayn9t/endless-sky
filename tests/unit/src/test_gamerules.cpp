/* test_gamerules.cpp
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
#include "../../../source/Gamerules.h"

// ... and any system includes needed for the test file.
#include <optional>
#include <string>
#include <type_traits>

namespace { // test namespace

// #region mock data

// Insert file-local data here, e.g. classes, structs, or fixtures that will be useful
// to help test this class/method.

// #endregion mock data



// #region unit tests
TEST_CASE( "Gamerules Basics", "[Gamerules]" ) {
	using T = Gamerules;
	SECTION( "Class Traits" ) {
		CHECK_FALSE( std::is_trivial_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
		CHECK_FALSE( std::is_trivially_destructible_v<T> );
	}
	SECTION( "Construction Traits" ) {
		CHECK( std::is_default_constructible_v<T> );
		CHECK( std::is_nothrow_default_constructible_v<T> );
		CHECK( std::is_copy_constructible_v<T> );
		CHECK( std::is_move_constructible_v<T> );
	}
	SECTION( "Copy Traits" ) {
		CHECK( std::is_copy_assignable_v<T> );
		CHECK( std::is_move_assignable_v<T> );
	}
}

SCENARIO( "Creating a Gamerules instance with default values", "[Gamerules]" ) {
	GIVEN( "a default-constructed Gamerules" ) {
		Gamerules rules;

		THEN( "it has the correct default name" ) {
			CHECK( rules.Name().empty() );
		}
		THEN( "it has the correct default description" ) {
			CHECK( rules.Description().empty() );
		}
		THEN( "it has a null thumbnail" ) {
			CHECK( rules.Thumbnail() == nullptr );
		}
		THEN( "default gamerule values are set correctly" ) {
			CHECK( rules.LockGamerules() == true );
			CHECK( rules.UniversalRamscoopActive() == true );
			CHECK( rules.PersonSpawnPeriod() == 36000 );
			CHECK( rules.NoPersonSpawnWeight() == 1000 );
			CHECK( rules.NPCMaxMiningTime() == 3600 );
			CHECK( rules.UniversalFrugalThreshold() == 0.75 );
			CHECK( rules.DepreciationMin() == 0.25 );
			CHECK( rules.DepreciationDaily() == 0.997 );
			CHECK( rules.DepreciationGracePeriod() == 7 );
			CHECK( rules.DepreciationMaxAge() == 1000 );
			CHECK( rules.FightersHitWhenDisabled() == Gamerules::FighterDodgePolicy::ALL );
			CHECK( rules.SystemDepartureMin() == 0.0 );
			CHECK( rules.SystemArrivalMin() == std::nullopt );
			CHECK( rules.FleetMultiplier() == 1.0 );
		}
	}
}

SCENARIO( "Setting and getting boolean gamerule values", "[Gamerules]" ) {
	GIVEN( "a default-constructed Gamerules" ) {
		Gamerules rules;

		WHEN( "LockGamerules is set to false" ) {
			rules.SetLockGamerules(false);
			THEN( "the value is updated" ) {
				CHECK_FALSE( rules.LockGamerules() );
			}
		}

		WHEN( "LockGamerules is set to true" ) {
			rules.SetLockGamerules(true);
			THEN( "the value is true" ) {
				CHECK( rules.LockGamerules() );
			}
		}

		WHEN( "UniversalRamscoopActive is set to false" ) {
			rules.SetUniversalRamscoopActive(false);
			THEN( "the value is updated" ) {
				CHECK_FALSE( rules.UniversalRamscoopActive() );
			}
		}

		WHEN( "UniversalRamscoopActive is set to true" ) {
			rules.SetUniversalRamscoopActive(true);
			THEN( "the value is true" ) {
				CHECK( rules.UniversalRamscoopActive() );
			}
		}
	}
}

SCENARIO( "Setting and getting integer gamerule values", "[Gamerules]" ) {
	GIVEN( "a default-constructed Gamerules" ) {
		Gamerules rules;

		WHEN( "PersonSpawnPeriod is set" ) {
			THEN( "positive values are stored as-is" ) {
				rules.SetPersonSpawnPeriod(100);
				CHECK( rules.PersonSpawnPeriod() == 100 );
			}
			THEN( "values below 1 are clamped to 1" ) {
				rules.SetPersonSpawnPeriod(0);
				CHECK( rules.PersonSpawnPeriod() == 1 );
				rules.SetPersonSpawnPeriod(-50);
				CHECK( rules.PersonSpawnPeriod() == 1 );
			}
		}

		WHEN( "NoPersonSpawnWeight is set" ) {
			THEN( "non-negative values are stored as-is" ) {
				rules.SetNoPersonSpawnWeight(500);
				CHECK( rules.NoPersonSpawnWeight() == 500 );
			}
			THEN( "negative values are clamped to 0" ) {
				rules.SetNoPersonSpawnWeight(-10);
				CHECK( rules.NoPersonSpawnWeight() == 0 );
			}
		}

		WHEN( "NPCMaxMiningTime is set" ) {
			THEN( "non-negative values are stored as-is" ) {
				rules.SetNPCMaxMiningTime(7200);
				CHECK( rules.NPCMaxMiningTime() == 7200 );
			}
			THEN( "negative values are clamped to 0" ) {
				rules.SetNPCMaxMiningTime(-100);
				CHECK( rules.NPCMaxMiningTime() == 0 );
			}
		}

		WHEN( "DepreciationGracePeriod is set" ) {
			THEN( "non-negative values are stored as-is" ) {
				rules.SetDepreciationGracePeriod(14);
				CHECK( rules.DepreciationGracePeriod() == 14 );
			}
			THEN( "negative values are clamped to 0" ) {
				rules.SetDepreciationGracePeriod(-5);
				CHECK( rules.DepreciationGracePeriod() == 0 );
			}
		}

		WHEN( "DepreciationMaxAge is set" ) {
			THEN( "non-negative values are stored as-is" ) {
				rules.SetDepreciationMaxAge(500);
				CHECK( rules.DepreciationMaxAge() == 500 );
			}
			THEN( "negative values are clamped to 0" ) {
				rules.SetDepreciationMaxAge(-30);
				CHECK( rules.DepreciationMaxAge() == 0 );
			}
		}
	}
}

SCENARIO( "Setting and getting double gamerule values", "[Gamerules]" ) {
	GIVEN( "a default-constructed Gamerules" ) {
		Gamerules rules;

		WHEN( "UniversalFrugalThreshold is set" ) {
			THEN( "values in range [0,1] are stored as-is" ) {
				rules.SetUniversalFrugalThreshold(0.5);
				CHECK( rules.UniversalFrugalThreshold() == 0.5 );
				rules.SetUniversalFrugalThreshold(0.0);
				CHECK( rules.UniversalFrugalThreshold() == 0.0 );
				rules.SetUniversalFrugalThreshold(1.0);
				CHECK( rules.UniversalFrugalThreshold() == 1.0 );
			}
			THEN( "values below 0 are clamped to 0" ) {
				rules.SetUniversalFrugalThreshold(-0.5);
				CHECK( rules.UniversalFrugalThreshold() == 0.0 );
			}
			THEN( "values above 1 are clamped to 1" ) {
				rules.SetUniversalFrugalThreshold(1.5);
				CHECK( rules.UniversalFrugalThreshold() == 1.0 );
			}
		}

		WHEN( "DepreciationMin is set" ) {
			THEN( "values in range [0,1] are stored as-is" ) {
				rules.SetDepreciationMin(0.3);
				CHECK( rules.DepreciationMin() == 0.3 );
			}
			THEN( "values below 0 are clamped to 0" ) {
				rules.SetDepreciationMin(-0.2);
				CHECK( rules.DepreciationMin() == 0.0 );
			}
			THEN( "values above 1 are clamped to 1" ) {
				rules.SetDepreciationMin(2.0);
				CHECK( rules.DepreciationMin() == 1.0 );
			}
		}

		WHEN( "DepreciationDaily is set" ) {
			THEN( "values in range [0,1] are stored as-is" ) {
				rules.SetDepreciationDaily(0.99);
				CHECK( rules.DepreciationDaily() == 0.99 );
			}
			THEN( "values below 0 are clamped to 0" ) {
				rules.SetDepreciationDaily(-0.1);
				CHECK( rules.DepreciationDaily() == 0.0 );
			}
			THEN( "values above 1 are clamped to 1" ) {
				rules.SetDepreciationDaily(1.5);
				CHECK( rules.DepreciationDaily() == 1.0 );
			}
		}

		WHEN( "SystemDepartureMin is set" ) {
			THEN( "non-negative values are stored as-is" ) {
				rules.SetSystemDepartureMin(5.5);
				CHECK( rules.SystemDepartureMin() == 5.5 );
			}
			THEN( "negative values are clamped to 0" ) {
				rules.SetSystemDepartureMin(-10.0);
				CHECK( rules.SystemDepartureMin() == 0.0 );
			}
		}

		WHEN( "FleetMultiplier is set" ) {
			THEN( "non-negative values are stored as-is" ) {
				rules.SetFleetMultiplier(2.5);
				CHECK( rules.FleetMultiplier() == 2.5 );
			}
			THEN( "negative values are clamped to 0" ) {
				rules.SetFleetMultiplier(-1.0);
				CHECK( rules.FleetMultiplier() == 0.0 );
			}
		}
	}
}

SCENARIO( "Setting and getting optional double gamerule values", "[Gamerules]" ) {
	GIVEN( "a default-constructed Gamerules" ) {
		Gamerules rules;

		WHEN( "SystemArrivalMin is set" ) {
			THEN( "a valid optional value is stored" ) {
				rules.SetSystemArrivalMin(10.5);
				REQUIRE( rules.SystemArrivalMin().has_value() );
				CHECK( rules.SystemArrivalMin().value() == 10.5 );
			}
			THEN( "nullopt clears the value" ) {
				rules.SetSystemArrivalMin(5.0);
				REQUIRE( rules.SystemArrivalMin().has_value() );
				rules.SetSystemArrivalMin(std::nullopt);
				CHECK_FALSE( rules.SystemArrivalMin().has_value() );
			}
		}
	}
}

SCENARIO( "Setting and getting FighterDodgePolicy", "[Gamerules]" ) {
	GIVEN( "a default-constructed Gamerules" ) {
		Gamerules rules;

		THEN( "default policy is ALL" ) {
			CHECK( rules.FightersHitWhenDisabled() == Gamerules::FighterDodgePolicy::ALL );
		}

		WHEN( "FighterDodgePolicy is set to NONE" ) {
			rules.SetFighterDodgePolicy(Gamerules::FighterDodgePolicy::NONE);
			THEN( "the policy is NONE" ) {
				CHECK( rules.FightersHitWhenDisabled() == Gamerules::FighterDodgePolicy::NONE );
			}
		}

		WHEN( "FighterDodgePolicy is set to ONLY_PLAYER" ) {
			rules.SetFighterDodgePolicy(Gamerules::FighterDodgePolicy::ONLY_PLAYER);
			THEN( "the policy is ONLY_PLAYER" ) {
				CHECK( rules.FightersHitWhenDisabled() == Gamerules::FighterDodgePolicy::ONLY_PLAYER );
			}
		}

		WHEN( "FighterDodgePolicy is set to ALL" ) {
			rules.SetFighterDodgePolicy(Gamerules::FighterDodgePolicy::NONE);
			rules.SetFighterDodgePolicy(Gamerules::FighterDodgePolicy::ALL);
			THEN( "the policy is ALL" ) {
				CHECK( rules.FightersHitWhenDisabled() == Gamerules::FighterDodgePolicy::ALL );
			}
		}
	}
}

SCENARIO( "Setting and getting misc values", "[Gamerules]" ) {
	GIVEN( "a default-constructed Gamerules" ) {
		Gamerules rules;

		WHEN( "a misc value is set" ) {
			rules.SetMiscValue("custom rule", 42);
			THEN( "GetValue returns the set value" ) {
				CHECK( rules.GetValue("custom rule") == 42 );
			}
		}

		WHEN( "a misc value is updated" ) {
			rules.SetMiscValue("custom rule", 10);
			rules.SetMiscValue("custom rule", 20);
			THEN( "GetValue returns the updated value" ) {
				CHECK( rules.GetValue("custom rule") == 20 );
			}
		}

		WHEN( "getting a non-existent misc value" ) {
			THEN( "GetValue returns 0" ) {
				CHECK( rules.GetValue("nonexistent rule") == 0 );
			}
		}
	}
}

SCENARIO( "Using GetValue for known rules", "[Gamerules]" ) {
	GIVEN( "a Gamerules with modified values" ) {
		Gamerules rules;
		rules.SetLockGamerules(false);
		rules.SetUniversalRamscoopActive(false);
		rules.SetPersonSpawnPeriod(1000);
		rules.SetNoPersonSpawnWeight(500);
		rules.SetNPCMaxMiningTime(5000);
		rules.SetUniversalFrugalThreshold(0.5);
		rules.SetDepreciationMin(0.3);
		rules.SetDepreciationDaily(0.8);
		rules.SetDepreciationGracePeriod(10);
		rules.SetDepreciationMaxAge(200);
		rules.SetFighterDodgePolicy(Gamerules::FighterDodgePolicy::ONLY_PLAYER);
		rules.SetSystemDepartureMin(5.0);
		rules.SetSystemArrivalMin(10.0);
		rules.SetFleetMultiplier(2.0);

		THEN( "GetValue returns the correct values (booleans as int)" ) {
			CHECK( rules.GetValue("lock gamerules") == 0 );
			CHECK( rules.GetValue("universal ramscoop") == 0 );
		}
		THEN( "GetValue returns correct int values" ) {
			CHECK( rules.GetValue("person spawn period") == 1000 );
			CHECK( rules.GetValue("no person spawn weight") == 500 );
			CHECK( rules.GetValue("npc max mining time") == 5000 );
			CHECK( rules.GetValue("depreciation grace period") == 10 );
			CHECK( rules.GetValue("depreciation max age") == 200 );
		}
		THEN( "GetValue returns double values scaled by 1000" ) {
			CHECK( rules.GetValue("universal frugal threshold") == 500 );
			CHECK( rules.GetValue("depreciation min") == 300 );
			CHECK( rules.GetValue("depreciation daily") == 800 );
			CHECK( rules.GetValue("system departure min") == 5000 );
			CHECK( rules.GetValue("system arrival min") == 10000 );
			CHECK( rules.GetValue("fleet multiplier") == 2000 );
		}
		THEN( "GetValue returns FighterDodgePolicy as int" ) {
			CHECK( rules.GetValue("disabled fighters avoid projectiles") == 1 );
		}
	}

	GIVEN( "a Gamerules with SystemArrivalMin unset" ) {
		Gamerules rules;
		rules.SetSystemArrivalMin(std::nullopt);

		THEN( "GetValue returns 0 for unset SystemArrivalMin" ) {
			CHECK( rules.GetValue("system arrival min") == 0 );
		}
	}
}

SCENARIO( "Comparing Gamerules instances", "[Gamerules]" ) {
	GIVEN( "two default-constructed Gamerules" ) {
		Gamerules rules1;
		Gamerules rules2;

		THEN( "they are equal" ) {
			CHECK( rules1 == rules2 );
		}
	}

	GIVEN( "two Gamerules with different values" ) {
		Gamerules rules1;
		Gamerules rules2;
		rules2.SetLockGamerules(false);

		THEN( "they are not equal" ) {
			CHECK_FALSE( rules1 == rules2 );
		}
	}

	GIVEN( "two Gamerules with same custom values" ) {
		Gamerules rules1;
		Gamerules rules2;
		rules1.SetPersonSpawnPeriod(5000);
		rules1.SetFleetMultiplier(3.0);
		rules2.SetPersonSpawnPeriod(5000);
		rules2.SetFleetMultiplier(3.0);

		THEN( "they are equal" ) {
			CHECK( rules1 == rules2 );
		}
	}
}

SCENARIO( "Replacing gamerules", "[Gamerules]" ) {
	GIVEN( "two Gamerules with different values" ) {
		Gamerules rules1;
		Gamerules rules2;
		rules1.SetPersonSpawnPeriod(1000);
		rules1.SetFleetMultiplier(2.0);
		rules2.SetPersonSpawnPeriod(5000);
		rules2.SetFleetMultiplier(3.0);
		rules2.SetLockGamerules(false);

		WHEN( "Replace is called" ) {
			rules1.Replace(rules2);

			THEN( "rules1 has rules2's values" ) {
				CHECK( rules1.PersonSpawnPeriod() == 5000 );
				CHECK( rules1.FleetMultiplier() == 3.0 );
				CHECK_FALSE( rules1.LockGamerules() );
				CHECK( rules1 == rules2 );
			}
		}
	}
}

SCENARIO( "Resetting individual gamerules", "[Gamerules]" ) {
	GIVEN( "a modified Gamerules and a preset" ) {
		Gamerules rules;
		Gamerules preset;
		rules.SetPersonSpawnPeriod(1000);
		rules.SetFleetMultiplier(2.0);
		rules.SetLockGamerules(false);
		rules.SetMiscValue("custom rule", 42);

		WHEN( "Reset is called for person spawn period" ) {
			rules.Reset("person spawn period", preset);
			THEN( "person spawn period is reset to default" ) {
				CHECK( rules.PersonSpawnPeriod() == 36000 );
			}
		}

		WHEN( "Reset is called for fleet multiplier" ) {
			rules.Reset("fleet multiplier", preset);
			THEN( "fleet multiplier is reset to default" ) {
				CHECK( rules.FleetMultiplier() == 1.0 );
			}
		}

		WHEN( "Reset is called for lock gamerules" ) {
			rules.Reset("lock gamerules", preset);
			THEN( "lock gamerules is reset to default" ) {
				CHECK( rules.LockGamerules() == true );
			}
		}

		WHEN( "Reset is called for universal ramscoop" ) {
			rules.SetUniversalRamscoopActive(false);
			rules.Reset("universal ramscoop", preset);
			THEN( "universal ramscoop is reset to default" ) {
				CHECK( rules.UniversalRamscoopActive() == true );
			}
		}

		WHEN( "Reset is called for depreciation min" ) {
			rules.SetDepreciationMin(0.5);
			rules.Reset("depreciation min", preset);
			THEN( "depreciation min is reset to default" ) {
				CHECK( rules.DepreciationMin() == 0.25 );
			}
		}
	}
}

SCENARIO( "FighterDodgePolicy enum values", "[Gamerules][FighterDodgePolicy]" ) {
	THEN( "enum values are as expected" ) {
		CHECK( static_cast<int>(Gamerules::FighterDodgePolicy::NONE) == 0 );
		CHECK( static_cast<int>(Gamerules::FighterDodgePolicy::ONLY_PLAYER) == 1 );
		CHECK( static_cast<int>(Gamerules::FighterDodgePolicy::ALL) == 2 );
	}
}

// #endregion unit tests



} // test namespace