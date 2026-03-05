/* test_mission.cpp
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
#include "../../../source/Mission.h"

// Include a helper for creating well-formed DataNodes.
#include "datanode-factory.h"

// ... and any system includes needed for the test file.
#include "../../../source/DataWriter.h"
#include "../../../source/Date.h"

#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <type_traits>

namespace { // test namespace

// #region mock data

// #endregion mock data



// #region unit tests

SCENARIO( "Creating a Mission", "[Mission][Creation]" ) {
	GIVEN( "a default-constructed Mission" ) {
		Mission mission;

			THEN( "it has default state" ) {
			CHECK_FALSE( mission.IsFailed() );
			CHECK( mission.IsVisible() );
            CHECK_FALSE( mission.HasPriority() );
            CHECK_FALSE( mission.IsNonBlocking() );
            CHECK_FALSE( mission.IsMinor() );
            CHECK( mission.OfferPrecedence() == 0 );
            CHECK_FALSE( mission.RecommendsAutosave() );
            // IsUnique returns true because repeat defaults to 1
            CHECK( mission.IsUnique() );
            // IsValid returns false because no destination is set
            CHECK_FALSE( mission.IsValid() );
        }
		THEN( "string properties are empty" ) {
			CHECK( mission.TrueName().empty() );
			CHECK( mission.DisplayName().empty() );
			CHECK( mission.Description().empty() );
			CHECK( mission.Cargo().empty() );
			CHECK( mission.ClearanceMessage().empty() );
		}
		THEN( "numeric properties are zero" ) {
			CHECK( mission.CargoSize() == 0 );
			CHECK( mission.Fine() == 0 );
			CHECK( mission.Passengers() == 0 );
			CHECK( mission.DisplayedPayment() == 0 );
			CHECK( mission.ExpectedJumps() == 0 );
		}
		THEN( "destination and source are null" ) {
			CHECK( mission.Destination() == nullptr );
			CHECK( mission.SourceShip() == nullptr );
		}
		THEN( "collections are empty" ) {
			CHECK( mission.Waypoints().empty() );
			CHECK( mission.VisitedWaypoints().empty() );
			CHECK( mission.Stopovers().empty() );
			CHECK( mission.VisitedStopovers().empty() );
			CHECK( mission.MarkedSystems().empty() );
			CHECK( mission.UnmarkedSystems().empty() );
			CHECK( mission.TrackedSystems().empty() );
			CHECK( mission.NPCs().empty() );
		}
		THEN( "clearance is default" ) {
			CHECK_FALSE( mission.HasClearance(nullptr) );
			CHECK( mission.HasFullClearance() );
		}
		THEN( "deadline is not set" ) {
			CHECK_FALSE( mission.Deadline() );
		}
		THEN( "it has a valid UUID" ) {
			CHECK_FALSE( mission.UUID().ToString().empty() );
		}
	}
}

SCENARIO( "Mission class traits", "[Mission][Traits]" ) {
	using T = Mission;
	SECTION( "Class Traits" ) {
		CHECK( std::is_nothrow_destructible_v<T> );
		CHECK_FALSE( std::is_trivially_destructible_v<T> );
	}
	SECTION( "Construction Traits" ) {
		CHECK( std::is_default_constructible_v<T> );
		CHECK_FALSE( std::is_trivially_default_constructible_v<T> );
		// Mission's default constructor may throw due to EsUuid
		CHECK_FALSE( std::is_nothrow_default_constructible_v<T> );
		CHECK_FALSE( std::is_copy_constructible_v<T> );
		CHECK( std::is_move_constructible_v<T> );
		// Mission's move constructor may throw due to various members
		CHECK_FALSE( std::is_nothrow_move_constructible_v<T> );
	}
	SECTION( "Assignment Traits" ) {
		CHECK_FALSE( std::is_copy_assignable_v<T> );
		CHECK( std::is_move_assignable_v<T> );
		CHECK( std::is_nothrow_move_assignable_v<T> );
	}
}

SCENARIO( "Mission UUID handling", "[Mission][UUID]" ) {
	GIVEN( "two default-constructed Missions" ) {
		Mission mission1;
		Mission mission2;

		THEN( "they have different UUIDs" ) {
			CHECK( mission1.UUID() != mission2.UUID() );
			CHECK( mission1.UUID().ToString() != mission2.UUID().ToString() );
		}
		THEN( "each UUID is consistent across calls" ) {
			CHECK( mission1.UUID() == mission1.UUID() );
			CHECK( mission1.UUID().ToString() == mission1.UUID().ToString() );
		}
	}

	GIVEN( "a moved Mission" ) {
		Mission original;
		std::string originalUuid = original.UUID().ToString();

		WHEN( "the mission is move-constructed" ) {
			Mission moved(std::move(original));

			THEN( "the moved mission retains the UUID" ) {
				CHECK( moved.UUID().ToString() == originalUuid );
			}
		}
	}
}

SCENARIO( "Mission location types", "[Mission][Location]" ) {
	GIVEN( "a default-constructed Mission" ) {
		Mission mission;

		THEN( "default location is SPACEPORT" ) {
			CHECK( mission.IsAtLocation(Mission::SPACEPORT) );
			CHECK_FALSE( mission.IsAtLocation(Mission::LANDING) );
			CHECK_FALSE( mission.IsAtLocation(Mission::JOB) );
			CHECK_FALSE( mission.IsAtLocation(Mission::ASSISTING) );
			CHECK_FALSE( mission.IsAtLocation(Mission::BOARDING) );
			CHECK_FALSE( mission.IsAtLocation(Mission::SHIPYARD) );
			CHECK_FALSE( mission.IsAtLocation(Mission::OUTFITTER) );
			CHECK_FALSE( mission.IsAtLocation(Mission::JOB_BOARD) );
			CHECK_FALSE( mission.IsAtLocation(Mission::ENTERING) );
			CHECK_FALSE( mission.IsAtLocation(Mission::TRANSITION) );
		}
	}
}

SCENARIO( "Mission failure state", "[Mission][Failure]" ) {
	GIVEN( "a default-constructed Mission" ) {
		Mission mission;

		THEN( "it is not failed initially" ) {
			CHECK_FALSE( mission.IsFailed() );
		}

		WHEN( "Fail() is called" ) {
			mission.Fail();

			THEN( "it becomes failed" ) {
				CHECK( mission.IsFailed() );
			}
		}
	}
}

SCENARIO( "Mission deadline handling", "[Mission][Deadline]" ) {
	GIVEN( "a default-constructed Mission" ) {
		Mission mission;

		THEN( "deadline is not set" ) {
			CHECK_FALSE( mission.Deadline() );
		}
	}

	GIVEN( "a date before the deadline" ) {
		Date today(1, 1, 2024);
		Date deadline(31, 12, 2024);

		THEN( "deadline is in the future" ) {
			CHECK( deadline > today );
		}
	}

	GIVEN( "a date after the deadline" ) {
		Date today(31, 12, 2024);
		Date deadline(1, 1, 2024);

		THEN( "deadline has passed" ) {
			CHECK( deadline < today );
		}
	}
}

SCENARIO( "Mission clearance handling", "[Mission][Clearance]" ) {
	GIVEN( "a default-constructed Mission" ) {
		Mission mission;

		THEN( "has full clearance by default" ) {
			CHECK( mission.HasFullClearance() );
		}

		THEN( "no planet has clearance by default" ) {
			CHECK_FALSE( mission.HasClearance(nullptr) );
		}

		THEN( "clearance message is empty" ) {
			CHECK( mission.ClearanceMessage().empty() );
		}
	}
}

SCENARIO( "Mission waypoint and stopover handling", "[Mission][Waypoints]" ) {
	GIVEN( "a default-constructed Mission" ) {
		Mission mission;

		THEN( "all waypoint collections are empty" ) {
			CHECK( mission.Waypoints().empty() );
			CHECK( mission.VisitedWaypoints().empty() );
		}

		THEN( "all stopover collections are empty" ) {
			CHECK( mission.Stopovers().empty() );
			CHECK( mission.VisitedStopovers().empty() );
		}
	}
}

SCENARIO( "Mission system marking", "[Mission][Marking]" ) {
	GIVEN( "a default-constructed Mission" ) {
		Mission mission;

		THEN( "no systems are marked" ) {
			CHECK( mission.MarkedSystems().empty() );
			CHECK( mission.UnmarkedSystems().empty() );
			CHECK( mission.TrackedSystems().empty() );
		}

		WHEN( "Mark is called with an empty set" ) {
			std::set<const System *> systems;
			mission.Mark(systems);

			THEN( "marked systems remain empty" ) {
				CHECK( mission.MarkedSystems().empty() );
			}
		}

		WHEN( "Unmark is called with an empty set" ) {
			std::set<const System *> systems;
			mission.Unmark(systems);

			THEN( "unmarked systems remain empty" ) {
				CHECK( mission.UnmarkedSystems().empty() );
			}
		}
	}
}

SCENARIO( "Mission cargo and passengers", "[Mission][Cargo]" ) {
	GIVEN( "a default-constructed Mission" ) {
		Mission mission;

		THEN( "no cargo by default" ) {
			CHECK( mission.Cargo().empty() );
			CHECK( mission.CargoSize() == 0 );
		}

		THEN( "no passengers by default" ) {
			CHECK( mission.Passengers() == 0 );
		}

		THEN( "no fine by default" ) {
			CHECK( mission.Fine() == 0 );
			CHECK( mission.FineMessage().empty() );
		}

		THEN( "failIfDiscovered is false" ) {
			CHECK_FALSE( mission.FailIfDiscovered() );
		}
	}
}

SCENARIO( "Mission payment", "[Mission][Payment]" ) {
	GIVEN( "a default-constructed Mission" ) {
		Mission mission;

		THEN( "no payment by default" ) {
			CHECK( mission.DisplayedPayment() == 0 );
		}
	}
}

SCENARIO( "Mission NPCs", "[Mission][NPCs]" ) {
	GIVEN( "a default-constructed Mission" ) {
		Mission mission;

		THEN( "no NPCs by default" ) {
			CHECK( mission.NPCs().empty() );
		}

		THEN( "HasShip returns false for null ship" ) {
			auto nullShip = std::shared_ptr<Ship>();
			CHECK_FALSE( mission.HasShip(nullShip) );
		}
	}
}

SCENARIO( "Mission Trigger enum values", "[Mission][Trigger]" ) {
	GIVEN( "the Trigger enum" ) {
		THEN( "it has all expected values" ) {
			CHECK( static_cast<int>(Mission::COMPLETE) == 0 );
			CHECK( static_cast<int>(Mission::OFFER) == 1 );
			CHECK( static_cast<int>(Mission::ACCEPT) == 2 );
			CHECK( static_cast<int>(Mission::DECLINE) == 3 );
			CHECK( static_cast<int>(Mission::FAIL) == 4 );
			CHECK( static_cast<int>(Mission::ABORT) == 5 );
			CHECK( static_cast<int>(Mission::DEFER) == 6 );
			CHECK( static_cast<int>(Mission::VISIT) == 7 );
			CHECK( static_cast<int>(Mission::STOPOVER) == 8 );
			CHECK( static_cast<int>(Mission::WAYPOINT) == 9 );
		 CHECK( static_cast<int>(Mission::DAILY) == 10 );
            CHECK( static_cast<int>(Mission::DISABLED) == 11 );
		}
	}
}

SCENARIO( "Mission GetAction", "[Mission][Action]" ) {
	GIVEN( "a default-constructed Mission" ) {
		Mission mission;

		WHEN( "getting an action for any trigger" ) {
			THEN( "returns an action with zero payment for COMPLETE" ) {
				auto action = mission.GetAction(Mission::COMPLETE);
				CHECK( action.Payment() == 0 );
			}
			THEN( "returns an action with zero payment for OFFER" ) {
				auto action = mission.GetAction(Mission::OFFER);
				CHECK( action.Payment() == 0 );
			}
			THEN( "returns an action with zero payment for ACCEPT" ) {
				auto action = mission.GetAction(Mission::ACCEPT);
				CHECK( action.Payment() == 0 );
			}
			THEN( "returns an action with zero payment for FAIL" ) {
				auto action = mission.GetAction(Mission::FAIL);
				CHECK( action.Payment() == 0 );
			}
		}
	}
}

SCENARIO( "Mission move semantics", "[Mission][Move]" ) {
	GIVEN( "a default-constructed Mission" ) {
		Mission original;
		original.Fail();  // Set a non-default state

		WHEN( "move-constructing a new Mission" ) {
			Mission moved(std::move(original));

			THEN( "the moved Mission has the failed state" ) {
				CHECK( moved.IsFailed() );
			}
		}
	}

	GIVEN( "two Missions for move assignment" ) {
		Mission original;
		original.Fail();

		Mission target;

		WHEN( "move-assigning" ) {
			target = std::move(original);

			THEN( "the target has the original's failed state" ) {
				CHECK( target.IsFailed() );
			}
		}
	}
}

SCENARIO( "Mission color accessors", "[Mission][Color]" ) {
	GIVEN( "a default-constructed Mission" ) {
		Mission mission;

		THEN( "color accessors return valid Color objects" ) {
			// Colors should be valid Color objects even if not explicitly set
			CHECK_FALSE( mission.Unavailable().IsLoaded() );
			CHECK_FALSE( mission.Unselected().IsLoaded() );
			CHECK_FALSE( mission.Selected().IsLoaded() );
		}
	}
}

SCENARIO( "Mission RecalculateTrackedSystems", "[Mission][TrackedSystems]" ) {
	GIVEN( "a default-constructed Mission" ) {
		Mission mission;

		WHEN( "RecalculateTrackedSystems is called" ) {
			mission.RecalculateTrackedSystems();

			THEN( "tracked systems remain empty" ) {
				CHECK( mission.TrackedSystems().empty() );
			}
		}
	}
}

SCENARIO( "Mission expected jumps", "[Mission][Jumps]" ) {
	GIVEN( "a default-constructed Mission" ) {
		Mission mission;

		THEN( "expected jumps is zero" ) {
			CHECK( mission.ExpectedJumps() == 0 );
		}
	}
}

SCENARIO( "Mission overrides capture", "[Mission][Capture]" ) {
	GIVEN( "a default-constructed Mission" ) {
		Mission mission;

		THEN( "does not override capture by default" ) {
			CHECK_FALSE( mission.OverridesCapture() );
		}
	}
}

SCENARIO( "Mission NeverOffer", "[Mission][NeverOffer]" ) {
	GIVEN( "a default-constructed Mission" ) {
		Mission mission;

		WHEN( "NeverOffer is called" ) {
			mission.NeverOffer();

			THEN( "mission state remains invalid (no destination)" ) {
			 // IsValid returns false because no destination is set
                CHECK_FALSE( mission.IsValid() );
            }
        }
    }
}

// #endregion unit tests



} // test namespace