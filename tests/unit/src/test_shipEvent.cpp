/* test_shipEvent.cpp
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
#include "../../../source/ShipEvent.h"

// ... and any system includes needed for the test file.
#include <memory>
#include <type_traits>

namespace { // test namespace

// #region mock data

// Insert file-local data here, e.g. classes, structs, or fixtures that will be useful
// to help test this class/method.

// #endregion mock data



// #region unit tests
TEST_CASE( "ShipEvent Class Traits", "[ShipEvent]" ) {
	using T = ShipEvent;
	SECTION( "Class Traits" ) {
		CHECK_FALSE( std::is_trivial_v<T> );
		CHECK_FALSE( std::is_trivially_destructible_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
	}
	SECTION( "Construction Traits" ) {
		CHECK_FALSE( std::is_default_constructible_v<T> );
		CHECK( std::is_copy_constructible_v<T> );
		CHECK( std::is_move_constructible_v<T> );
	}
	SECTION( "Copy Traits" ) {
		CHECK( std::is_copy_assignable_v<T> );
		CHECK( std::is_move_assignable_v<T> );
	}
}

SCENARIO( "Creating a ShipEvent with Government as actor", "[ShipEvent]" ) {
	GIVEN( "a Government pointer and a null target ship" ) {
		const Government* actorGov = nullptr;
		std::shared_ptr<Ship> targetShip;
		int eventType = ShipEvent::ASSIST;

		WHEN( "creating a ShipEvent" ) {
			ShipEvent event(actorGov, targetShip, eventType);

			THEN( "the actor ship is null" ) {
				CHECK_FALSE( event.Actor() );
			}
			THEN( "the actor government is the provided pointer" ) {
				CHECK( event.ActorGovernment() == actorGov );
			}
			THEN( "the target ship is null" ) {
				CHECK_FALSE( event.Target() );
			}
			THEN( "the target government is null" ) {
				CHECK_FALSE( event.TargetGovernment() );
			}
			THEN( "the type is the provided event type" ) {
				CHECK( event.Type() == eventType );
			}
		}
	}
}

SCENARIO( "Creating a ShipEvent with Ship as actor", "[ShipEvent]" ) {
	GIVEN( "null actor and target ships" ) {
		std::shared_ptr<Ship> actorShip;
		std::shared_ptr<Ship> targetShip;
		int eventType = ShipEvent::PROVOKE;

		WHEN( "creating a ShipEvent" ) {
			ShipEvent event(actorShip, targetShip, eventType);

			THEN( "the actor ship is null" ) {
				CHECK_FALSE( event.Actor() );
			}
			THEN( "the actor government is null" ) {
				CHECK_FALSE( event.ActorGovernment() );
			}
			THEN( "the target ship is null" ) {
				CHECK_FALSE( event.Target() );
			}
			THEN( "the target government is null" ) {
				CHECK_FALSE( event.TargetGovernment() );
			}
			THEN( "the type is the provided event type" ) {
				CHECK( event.Type() == eventType );
			}
		}
	}
}

SCENARIO( "ShipEvent type values", "[ShipEvent]" ) {
	GIVEN( "the ShipEvent type constants" ) {
		THEN( "NONE is 0" ) {
			CHECK( ShipEvent::NONE == 0 );
		}
		THEN( "ASSIST is bit 0" ) {
			CHECK( ShipEvent::ASSIST == (1 << 0) );
		}
		THEN( "SCAN_CARGO is bit 1" ) {
			CHECK( ShipEvent::SCAN_CARGO == (1 << 1) );
		}
		THEN( "SCAN_OUTFITS is bit 2" ) {
			CHECK( ShipEvent::SCAN_OUTFITS == (1 << 2) );
		}
		THEN( "PROVOKE is bit 3" ) {
			CHECK( ShipEvent::PROVOKE == (1 << 3) );
		}
		THEN( "DISABLE is bit 4" ) {
			CHECK( ShipEvent::DISABLE == (1 << 4) );
		}
		THEN( "BOARD is bit 5" ) {
			CHECK( ShipEvent::BOARD == (1 << 5) );
		}
		THEN( "CAPTURE is bit 6" ) {
			CHECK( ShipEvent::CAPTURE == (1 << 6) );
		}
		THEN( "DESTROY is bit 7" ) {
			CHECK( ShipEvent::DESTROY == (1 << 7) );
		}
		THEN( "ATROCITY is bit 8" ) {
			CHECK( ShipEvent::ATROCITY == (1 << 8) );
		}
		THEN( "JUMP is bit 9" ) {
			CHECK( ShipEvent::JUMP == (1 << 9) );
		}
		THEN( "ENCOUNTER is bit 10" ) {
			CHECK( ShipEvent::ENCOUNTER == (1 << 10) );
		}
	}
}

SCENARIO( "ShipEvent type flags can be combined", "[ShipEvent]" ) {
	GIVEN( "multiple ShipEvent types" ) {
		WHEN( "combining SCAN_CARGO and SCAN_OUTFITS" ) {
			int combinedType = ShipEvent::SCAN_CARGO | ShipEvent::SCAN_OUTFITS;

			THEN( "the combined value has both flags set" ) {
				CHECK( (combinedType & ShipEvent::SCAN_CARGO) != 0 );
				CHECK( (combinedType & ShipEvent::SCAN_OUTFITS) != 0 );
			}
			THEN( "other flags are not set" ) {
				CHECK( (combinedType & ShipEvent::ASSIST) == 0 );
				CHECK( (combinedType & ShipEvent::PROVOKE) == 0 );
			}
		}

		WHEN( "combining DISABLE and BOARD and CAPTURE" ) {
			int combinedType = ShipEvent::DISABLE | ShipEvent::BOARD | ShipEvent::CAPTURE;

			THEN( "all three flags are set" ) {
				CHECK( (combinedType & ShipEvent::DISABLE) != 0 );
				CHECK( (combinedType & ShipEvent::BOARD) != 0 );
				CHECK( (combinedType & ShipEvent::CAPTURE) != 0 );
			}
		}

		WHEN( "creating a ShipEvent with combined types" ) {
			int combinedType = ShipEvent::ASSIST | ShipEvent::SCAN_CARGO;
			ShipEvent event(nullptr, nullptr, combinedType);

			THEN( "the event type contains both flags" ) {
				CHECK( (event.Type() & ShipEvent::ASSIST) != 0 );
				CHECK( (event.Type() & ShipEvent::SCAN_CARGO) != 0 );
			}
		}
	}
}

SCENARIO( "ShipEvent with different type values", "[ShipEvent]" ) {
	GIVEN( "a ShipEvent creator" ) {
		std::shared_ptr<Ship> emptyShip;

		WHEN( "creating a ShipEvent with NONE type" ) {
			ShipEvent event(nullptr, emptyShip, ShipEvent::NONE);

			THEN( "the type is NONE" ) {
				CHECK( event.Type() == ShipEvent::NONE );
				CHECK( event.Type() == 0 );
			}
		}

		WHEN( "creating a ShipEvent with DESTROY type" ) {
			ShipEvent event(nullptr, emptyShip, ShipEvent::DESTROY);

			THEN( "the type is DESTROY" ) {
				CHECK( event.Type() == ShipEvent::DESTROY );
			}
		}

		WHEN( "creating a ShipEvent with ATROCITY type" ) {
			ShipEvent event(nullptr, emptyShip, ShipEvent::ATROCITY);

			THEN( "the type is ATROCITY" ) {
				CHECK( event.Type() == ShipEvent::ATROCITY );
			}
		}

		WHEN( "creating a ShipEvent with JUMP type" ) {
			ShipEvent event(nullptr, emptyShip, ShipEvent::JUMP);

			THEN( "the type is JUMP" ) {
				CHECK( event.Type() == ShipEvent::JUMP );
			}
		}

		WHEN( "creating a ShipEvent with ENCOUNTER type" ) {
			ShipEvent event(nullptr, emptyShip, ShipEvent::ENCOUNTER);

			THEN( "the type is ENCOUNTER" ) {
				CHECK( event.Type() == ShipEvent::ENCOUNTER );
			}
		}
	}
}

SCENARIO( "ShipEvent copy semantics", "[ShipEvent]" ) {
	GIVEN( "an existing ShipEvent" ) {
		int eventType = ShipEvent::BOARD | ShipEvent::CAPTURE;
		ShipEvent original(nullptr, nullptr, eventType);

		WHEN( "copy-constructing a new ShipEvent" ) {
			ShipEvent copy(original);

			THEN( "the copy has the same type" ) {
				CHECK( copy.Type() == original.Type() );
			}
			THEN( "the copy has the same actor" ) {
				CHECK( copy.Actor() == original.Actor() );
			}
			THEN( "the copy has the same target" ) {
				CHECK( copy.Target() == original.Target() );
			}
			THEN( "the copy has the same actor government" ) {
				CHECK( copy.ActorGovernment() == original.ActorGovernment() );
			}
			THEN( "the copy has the same target government" ) {
				CHECK( copy.TargetGovernment() == original.TargetGovernment() );
			}
		}
	}
}

SCENARIO( "ShipEvent move semantics", "[ShipEvent]" ) {
	GIVEN( "an existing ShipEvent" ) {
		int eventType = ShipEvent::DISABLE;
		ShipEvent original(nullptr, nullptr, eventType);

		WHEN( "move-constructing a new ShipEvent" ) {
			ShipEvent moved(std::move(original));

			THEN( "the moved-to object has the correct type" ) {
				CHECK( moved.Type() == eventType );
			}
		}
	}
}

SCENARIO( "ShipEvent with non-null pointers", "[ShipEvent]" ) {
	GIVEN( "a ShipEvent with Government actor constructor" ) {
		// Create a dummy Government pointer (not a valid government, but a non-null pointer)
		int dummyData = 0;
		const Government* dummyGov = reinterpret_cast<const Government*>(&dummyData);
		std::shared_ptr<Ship> emptyShip;
		int eventType = ShipEvent::ASSIST;

		WHEN( "creating a ShipEvent with a non-null government pointer" ) {
			ShipEvent event(dummyGov, emptyShip, eventType);

			THEN( "the actor government is the provided pointer" ) {
				CHECK( event.ActorGovernment() == dummyGov );
			}
			THEN( "the actor ship remains null" ) {
				CHECK_FALSE( event.Actor() );
			}
		}
	}
}

// #endregion unit tests



} // test namespace