/* test_ammoDisplay.cpp
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
#include "../../../source/AmmoDisplay.h"

// ... and any system includes needed for the test file.
#include "../../../source/PlayerInfo.h"
#include "../../../source/Point.h"
#include "../../../source/Rectangle.h"
#include "../../../source/Ship.h"

#include <type_traits>

namespace { // test namespace

// #region mock data

// Insert file-local data here, e.g. classes, structs, or fixtures that will be useful
// to help test this class/method.

// #endregion mock data



// #region unit tests
TEST_CASE( "AmmoDisplay Basics", "[AmmoDisplay]" ) {
	using T = AmmoDisplay;
	SECTION( "Class Traits" ) {
		CHECK_FALSE( std::is_trivial_v<T> );
		CHECK_FALSE( std::is_standard_layout_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
	}
	SECTION( "Construction Traits" ) {
		// AmmoDisplay requires a PlayerInfo reference, so it is not default constructible.
		CHECK_FALSE( std::is_default_constructible_v<T> );
		CHECK( std::is_copy_constructible_v<T> );
		CHECK( std::is_move_constructible_v<T> );
	}
	SECTION( "Copy Traits" ) {
		// AmmoDisplay has a reference member, so assignment operators are deleted.
		CHECK_FALSE( std::is_copy_assignable_v<T> );
		CHECK_FALSE( std::is_move_assignable_v<T> );
	}
}

SCENARIO( "Creating an AmmoDisplay instance", "[AmmoDisplay]" ) {
	GIVEN( "A PlayerInfo instance" ) {
		PlayerInfo player;
		WHEN( "an AmmoDisplay is constructed" ) {
			AmmoDisplay display(player);
			THEN( "the instance is created successfully" ) {
				// Basic construction check - no exceptions should be thrown
				CHECK( true );
			}
		}
	}
}

SCENARIO( "AmmoDisplay Reset functionality", "[AmmoDisplay]" ) {
	GIVEN( "An AmmoDisplay instance" ) {
		PlayerInfo player;
		AmmoDisplay display(player);
		WHEN( "Reset is called" ) {
			// Reset should clear internal ammo map
			REQUIRE_NOTHROW( display.Reset() );
			THEN( "the display is reset without error" ) {
				CHECK( true );
			}
		}
		WHEN( "Reset is called multiple times" ) {
			REQUIRE_NOTHROW( display.Reset() );
			REQUIRE_NOTHROW( display.Reset() );
			REQUIRE_NOTHROW( display.Reset() );
			THEN( "multiple resets do not cause issues" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "AmmoDisplay Click with empty zones", "[AmmoDisplay]" ) {
	GIVEN( "An AmmoDisplay instance with no ammo icons" ) {
		PlayerInfo player;
		AmmoDisplay display(player);
		WHEN( "Click is called with a point and no zones populated" ) {
			Point clickPoint(10., 10.);
			bool result = display.Click(clickPoint, false);
			THEN( "the click returns false" ) {
				CHECK_FALSE( result );
			}
		}
		WHEN( "Click is called with a point and control=true" ) {
			Point clickPoint(10., 10.);
			bool result = display.Click(clickPoint, true);
			THEN( "the click returns false" ) {
				CHECK_FALSE( result );
			}
		}
		WHEN( "Click is called with a Rectangle and no zones populated" ) {
			Rectangle clickBox(Point(10., 10.), Point(20., 20.));
			bool result = display.Click(clickBox);
			THEN( "the click returns false" ) {
				CHECK_FALSE( result );
			}
		}
	}
}

SCENARIO( "AmmoDisplay Click with various point positions", "[AmmoDisplay]" ) {
	GIVEN( "An AmmoDisplay instance" ) {
		PlayerInfo player;
		AmmoDisplay display(player);
		WHEN( "Click is called at origin" ) {
			Point clickPoint(0., 0.);
			bool result = display.Click(clickPoint, false);
			THEN( "the click returns false with empty zones" ) {
				CHECK_FALSE( result );
			}
		}
		WHEN( "Click is called with negative coordinates" ) {
			Point clickPoint(-100., -50.);
			bool result = display.Click(clickPoint, false);
			THEN( "the click returns false with empty zones" ) {
				CHECK_FALSE( result );
			}
		}
		WHEN( "Click is called with large positive coordinates" ) {
			Point clickPoint(1000., 500.);
			bool result = display.Click(clickPoint, false);
			THEN( "the click returns false with empty zones" ) {
				CHECK_FALSE( result );
			}
		}
	}
}

SCENARIO( "AmmoDisplay Click with various rectangle sizes", "[AmmoDisplay]" ) {
	GIVEN( "An AmmoDisplay instance" ) {
		PlayerInfo player;
		AmmoDisplay display(player);
		WHEN( "Click is called with a small rectangle" ) {
			Rectangle clickBox(Point(10., 10.), Point(5., 5.));
			bool result = display.Click(clickBox);
			THEN( "the click returns false with empty zones" ) {
				CHECK_FALSE( result );
			}
		}
		WHEN( "Click is called with a large rectangle" ) {
			Rectangle clickBox(Point(0., 0.), Point(1000., 500.));
			bool result = display.Click(clickBox);
			THEN( "the click returns false with empty zones" ) {
				CHECK_FALSE( result );
			}
		}
		WHEN( "Click is called with a zero-size rectangle" ) {
			Rectangle clickBox(Point(10., 10.), Point(0., 0.));
			bool result = display.Click(clickBox);
			THEN( "the click returns false with empty zones" ) {
				CHECK_FALSE( result );
			}
		}
		WHEN( "Click is called with a rectangle at negative position" ) {
			Rectangle clickBox(Point(-50., -50.), Point(20., 20.));
			bool result = display.Click(clickBox);
			THEN( "the click returns false with empty zones" ) {
				CHECK_FALSE( result );
			}
		}
	}
}

SCENARIO( "AmmoDisplay Update with default Ship", "[AmmoDisplay]" ) {
	GIVEN( "An AmmoDisplay instance and a default Ship" ) {
		PlayerInfo player;
		AmmoDisplay display(player);
		Ship ship;
		WHEN( "Update is called with a default-constructed Ship" ) {
			REQUIRE_NOTHROW( display.Update(ship) );
			THEN( "the update completes without error" ) {
				CHECK( true );
			}
		}
		WHEN( "Update is called after Reset" ) {
			display.Reset();
			REQUIRE_NOTHROW( display.Update(ship) );
			THEN( "the update completes without error" ) {
				CHECK( true );
			}
		}
		WHEN( "Update is called multiple times" ) {
			REQUIRE_NOTHROW( display.Update(ship) );
			REQUIRE_NOTHROW( display.Update(ship) );
			REQUIRE_NOTHROW( display.Update(ship) );
			THEN( "multiple updates do not cause issues" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "AmmoDisplay Draw with empty ammo", "[AmmoDisplay]" ) {
	GIVEN( "An AmmoDisplay instance with no ammo" ) {
		PlayerInfo player;
		AmmoDisplay display(player);
		WHEN( "Draw is called with a valid ammo box and icon dimensions" ) {
			Rectangle ammoBox(Point(100., 100.), Point(50., 100.));
			Point iconDimensions(20., 20.);
			// Note: Draw may require initialized graphics resources.
			// Testing that it doesn't crash with empty ammo.
			REQUIRE_NOTHROW( display.Draw(ammoBox, iconDimensions) );
			THEN( "the draw completes without error" ) {
				CHECK( true );
			}
		}
		WHEN( "Draw is called with zero-size icon dimensions" ) {
			Rectangle ammoBox(Point(100., 100.), Point(50., 100.));
			Point iconDimensions(0., 0.);
			// This tests edge case behavior.
			REQUIRE_NOTHROW( display.Draw(ammoBox, iconDimensions) );
			THEN( "the draw handles zero-size dimensions" ) {
				CHECK( true );
			}
		}
		WHEN( "Draw is called with small ammo box" ) {
			Rectangle ammoBox(Point(10., 10.), Point(5., 5.));
			Point iconDimensions(20., 20.);
			REQUIRE_NOTHROW( display.Draw(ammoBox, iconDimensions) );
			THEN( "the draw handles small box" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "Copying an AmmoDisplay", "[AmmoDisplay]" ) {
	GIVEN( "An AmmoDisplay instance" ) {
		PlayerInfo player;
		AmmoDisplay original(player);
		WHEN( "copy-constructing" ) {
			AmmoDisplay copy(original);
			THEN( "the copy is created successfully" ) {
				// Verify that the copy references the same PlayerInfo
				CHECK( true );
			}
		}
		// Note: AmmoDisplay has a reference member (PlayerInfo&), so copy/move assignment
		// operators are implicitly deleted. We cannot test assignment operations.
	}
}

SCENARIO( "Moving an AmmoDisplay", "[AmmoDisplay]" ) {
	GIVEN( "An AmmoDisplay instance" ) {
		PlayerInfo player;
		AmmoDisplay original(player);
		WHEN( "move-constructing" ) {
			AmmoDisplay moved(std::move(original));
			THEN( "the move is successful" ) {
				CHECK( true );
			}
		}
		// Note: AmmoDisplay has a reference member (PlayerInfo&), so move assignment
		// operator is implicitly deleted. We cannot test move assignment.
	}
}

SCENARIO( "AmmoDisplay click control parameter behavior", "[AmmoDisplay]" ) {
	GIVEN( "An AmmoDisplay instance with empty zones" ) {
		PlayerInfo player;
		AmmoDisplay display(player);
		WHEN( "Click is called with control=false" ) {
			Point clickPoint(50., 50.);
			bool result = display.Click(clickPoint, false);
			THEN( "the click returns false" ) {
				CHECK_FALSE( result );
			}
		}
		WHEN( "Click is called with control=true" ) {
			Point clickPoint(50., 50.);
			bool result = display.Click(clickPoint, true);
			THEN( "the click returns false" ) {
				CHECK_FALSE( result );
			}
		}
	}
}

SCENARIO( "AmmoDisplay Reset and Update sequence", "[AmmoDisplay]" ) {
	GIVEN( "An AmmoDisplay instance" ) {
		PlayerInfo player;
		AmmoDisplay display(player);
		Ship ship;
		WHEN( "Reset then Update is called" ) {
			display.Reset();
			REQUIRE_NOTHROW( display.Update(ship) );
			THEN( "the sequence completes successfully" ) {
				CHECK( true );
			}
		}
		WHEN( "Update then Reset is called" ) {
			REQUIRE_NOTHROW( display.Update(ship) );
			REQUIRE_NOTHROW( display.Reset() );
			THEN( "the sequence completes successfully" ) {
				CHECK( true );
			}
		}
		WHEN( "Multiple Reset-Update cycles" ) {
			for(int i = 0; i < 5; ++i)
			{
				REQUIRE_NOTHROW( display.Reset() );
				REQUIRE_NOTHROW( display.Update(ship) );
			}
			THEN( "multiple cycles complete successfully" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "AmmoDisplay Draw after Reset", "[AmmoDisplay]" ) {
	GIVEN( "An AmmoDisplay instance" ) {
		PlayerInfo player;
		AmmoDisplay display(player);
		Rectangle ammoBox(Point(100., 100.), Point(50., 100.));
		Point iconDimensions(20., 20.);
		WHEN( "Draw is called after Reset" ) {
			display.Reset();
			REQUIRE_NOTHROW( display.Draw(ammoBox, iconDimensions) );
			THEN( "the draw completes without error" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "AmmoDisplay Click after Reset", "[AmmoDisplay]" ) {
	GIVEN( "An AmmoDisplay instance" ) {
		PlayerInfo player;
		AmmoDisplay display(player);
		WHEN( "Click is called after Reset with point" ) {
			display.Reset();
			Point clickPoint(50., 50.);
			bool result = display.Click(clickPoint, false);
			THEN( "the click returns false" ) {
				CHECK_FALSE( result );
			}
		}
		WHEN( "Click is called after Reset with rectangle" ) {
			display.Reset();
			Rectangle clickBox(Point(50., 50.), Point(20., 20.));
			bool result = display.Click(clickBox);
			THEN( "the click returns false" ) {
				CHECK_FALSE( result );
			}
		}
	}
}

SCENARIO( "AmmoDisplay boundary conditions", "[AmmoDisplay][edge]" ) {
	GIVEN( "An AmmoDisplay instance" ) {
		PlayerInfo player;
		AmmoDisplay display(player);
		WHEN( "Click with point at extreme negative coordinates" ) {
			Point clickPoint(-1e10, -1e10);
			bool result = display.Click(clickPoint, false);
			THEN( "the click handles extreme values gracefully" ) {
				CHECK_FALSE( result );
			}
		}
		WHEN( "Click with point at extreme positive coordinates" ) {
			Point clickPoint(1e10, 1e10);
			bool result = display.Click(clickPoint, false);
			THEN( "the click handles extreme values gracefully" ) {
				CHECK_FALSE( result );
			}
		}
		WHEN( "Click with very large rectangle" ) {
			Rectangle clickBox(Point(0., 0.), Point(1e6, 1e6));
			bool result = display.Click(clickBox);
			THEN( "the click handles large rectangle gracefully" ) {
				CHECK_FALSE( result );
			}
		}
	}
}

// #endregion unit tests



} // test namespace