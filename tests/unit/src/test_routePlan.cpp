/* test_routePlan.cpp
Copyright (c) 2025 by jiang

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
#include "../../../source/RoutePlan.h"

// ... and any system includes needed for the test file.
#include <vector>

namespace { // test namespace

// #region mock data

// RoutePlan depends on System, Ship, PlayerInfo, and DistanceMap.
// These are complex game classes that require GameData to be initialized
// with actual game data. Full integration tests would be needed to test
// the route planning functionality with real systems.
// Here we test the default behavior and API contract.

// #endregion mock data



// #region unit tests

SCENARIO( "Creating a default RoutePlan", "[RoutePlan][Creation]" ) {
	GIVEN( "a default constructed RoutePlan" ) {
		RoutePlan plan;

		THEN( "it has no route" ) {
			CHECK_FALSE( plan.HasRoute() );
		}
		THEN( "FirstStep returns nullptr" ) {
			CHECK( plan.FirstStep() == nullptr );
		}
		THEN( "Days returns -1" ) {
			CHECK( plan.Days() == -1 );
		}
		THEN( "RequiredFuel returns -1" ) {
			CHECK( plan.RequiredFuel() == -1 );
		}
		THEN( "Plan returns an empty vector" ) {
			CHECK( plan.Plan().empty() );
		}
		THEN( "FuelCosts returns an empty vector" ) {
			CHECK( plan.FuelCosts().empty() );
		}
	}
}

SCENARIO( "RoutePlan reports no route correctly", "[RoutePlan]" ) {
	GIVEN( "a RoutePlan with no route available" ) {
		RoutePlan plan;

		WHEN( "checking if route exists" ) {
			bool hasRoute = plan.HasRoute();

			THEN( "the result is false" ) {
				CHECK_FALSE( hasRoute );
			}
		}

		WHEN( "getting the first step" ) {
			auto firstStep = plan.FirstStep();

			THEN( "nullptr is returned" ) {
				CHECK( firstStep == nullptr );
			}
		}

		WHEN( "getting the number of days" ) {
			int days = plan.Days();

			THEN( "-1 is returned indicating no route" ) {
				CHECK( days == -1 );
			}
		}

		WHEN( "getting the required fuel" ) {
			int fuel = plan.RequiredFuel();

			THEN( "-1 is returned indicating no route" ) {
				CHECK( fuel == -1 );
			}
		}

		WHEN( "getting the plan steps" ) {
			auto steps = plan.Plan();

			THEN( "an empty vector is returned" ) {
				CHECK( steps.empty() );
			}
		}

		WHEN( "getting the fuel costs" ) {
			auto fuelCosts = plan.FuelCosts();

			THEN( "an empty vector is returned" ) {
				CHECK( fuelCosts.empty() );
			}
		}
	}
}

SCENARIO( "RoutePlan API contract", "[RoutePlan][API]" ) {
	GIVEN( "a RoutePlan instance" ) {
		RoutePlan plan;

		WHEN( "querying route status without valid systems" ) {
			THEN( "HasRoute returns a boolean" ) {
				CHECK_FALSE( plan.HasRoute() );
			}
		}

		WHEN( "querying route details without a valid route" ) {
			THEN( "Days returns a valid integer" ) {
				CHECK( plan.Days() == -1 );
			}
			THEN( "RequiredFuel returns a valid integer" ) {
				CHECK( plan.RequiredFuel() == -1 );
			}
		}

		WHEN( "querying route steps without a valid route" ) {
			THEN( "Plan returns a vector of System pointers" ) {
				std::vector<const System *> steps = plan.Plan();
				CHECK( steps.empty() );
			}
			THEN( "FuelCosts returns a vector of pairs" ) {
				std::vector<std::pair<const System *, int>> costs = plan.FuelCosts();
				CHECK( costs.empty() );
			}
		}
	}
}

// Note: Testing RoutePlan with actual routes would require:
// 1. A fully initialized GameData with system definitions
// 2. Systems with hyperspace links or jumpable distances
// 3. A PlayerInfo object with visited systems
// 4. A Ship with appropriate jump capabilities
//
// Such tests would be better suited as integration tests in
// the tests/integration directory rather than unit tests.

// #endregion unit tests



} // test namespace