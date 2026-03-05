/* test_radar.cpp
Copyright (c) 2026 by test contributor

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
#include "../../../source/Radar.h"

// ... and any system includes needed for the test file.
#include "../../../source/Point.h"
#include <type_traits>

namespace { // test namespace

// #region mock data

// Note: The Radar class has significant dependencies:
// - GetColor() requires GameData::Colors().Get() for all color types
// - Draw() requires OpenGL shaders (LineShader, PointerShader, RingShader)
// - Add(), AddPointer(), AddViewportBoundary() all internally call GetColor()
//
// Per the unit test README: "Generally speaking, if a method invokes the GameData
// class, it will not be testable using unit tests."
//
// The following can be tested without GameData or OpenGL:
// - Type constant values (PLAYER, FRIENDLY, etc.)
// - Class type traits
// - Default construction
// - Clear() (does not depend on external state)
// - SetCenter() (does not depend on external state)
//
// Methods that cannot be fully tested without GameData/OpenGL:
// - Add() - calls GetColor() internally
// - AddPointer() - calls GetColor() internally
// - AddViewportBoundary() - calls GetColor() internally
// - Draw() - requires OpenGL shaders
// - GetColor() - requires GameData

// #endregion mock data



// #region unit tests

TEST_CASE( "Radar Class Traits", "[Radar]" ) {
	using T = Radar;
	SECTION( "Class Traits" ) {
		CHECK_FALSE( std::is_trivial_v<T> );
		CHECK( std::is_standard_layout_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
		CHECK( std::is_trivially_destructible_v<T> );
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

SCENARIO( "Radar type constants have expected values", "[Radar][Constants]" ) {
	GIVEN( "the Radar type constants" ) {
		THEN( "they have sequential values starting from 0" ) {
			CHECK( Radar::PLAYER == 0 );
			CHECK( Radar::FRIENDLY == 1 );
			CHECK( Radar::UNFRIENDLY == 2 );
			CHECK( Radar::HOSTILE == 3 );
			CHECK( Radar::INACTIVE == 4 );
			CHECK( Radar::SPECIAL == 5 );
			CHECK( Radar::ANOMALOUS == 6 );
			CHECK( Radar::BLINK == 7 );
			CHECK( Radar::VIEWPORT == 8 );
			CHECK( Radar::STAR == 9 );
		}
	}
}

SCENARIO( "Creating a Radar instance", "[Radar][Construction]" ) {
	GIVEN( "no arguments" ) {
		WHEN( "a Radar is default constructed" ) {
			Radar radar;
			THEN( "it can be cleared without error" ) {
				radar.Clear();
			}
			THEN( "the center can be set" ) {
				radar.SetCenter(Point(100.0, 200.0));
			}
		}
	}
}

SCENARIO( "Clearing the radar", "[Radar][Clear]" ) {
	GIVEN( "a Radar instance" ) {
		Radar radar;
		WHEN( "Clear() is called" ) {
			THEN( "it completes without error" ) {
				radar.Clear();
			}
		}
		WHEN( "Clear() is called multiple times" ) {
			THEN( "all calls succeed" ) {
				radar.Clear();
				radar.Clear();
				radar.Clear();
			}
		}
	}
}

SCENARIO( "Setting the radar center", "[Radar][SetCenter]" ) {
	GIVEN( "a Radar instance" ) {
		Radar radar;
		WHEN( "SetCenter is called with a point" ) {
			THEN( "it completes without error" ) {
				radar.SetCenter(Point(0.0, 0.0));
			}
		}
		WHEN( "SetCenter is called with various points" ) {
			THEN( "all calls succeed" ) {
				radar.SetCenter(Point(100.0, 200.0));
				radar.SetCenter(Point(-50.0, -75.0));
				radar.SetCenter(Point(0.001, 0.002));
			}
		}
		WHEN( "SetCenter is called after Clear" ) {
			radar.Clear();
			THEN( "it still succeeds" ) {
				radar.SetCenter(Point(10.0, 20.0));
			}
		}
	}
}

SCENARIO( "Radar type constants are distinct", "[Radar][Constants]" ) {
	GIVEN( "all Radar type constants" ) {
		THEN( "each constant has a unique value" ) {
			CHECK( Radar::PLAYER != Radar::FRIENDLY );
			CHECK( Radar::PLAYER != Radar::UNFRIENDLY );
			CHECK( Radar::PLAYER != Radar::HOSTILE );
			CHECK( Radar::PLAYER != Radar::INACTIVE );
			CHECK( Radar::PLAYER != Radar::SPECIAL );
			CHECK( Radar::PLAYER != Radar::ANOMALOUS );
			CHECK( Radar::PLAYER != Radar::BLINK );
			CHECK( Radar::PLAYER != Radar::VIEWPORT );
			CHECK( Radar::PLAYER != Radar::STAR );

			CHECK( Radar::FRIENDLY != Radar::UNFRIENDLY );
			CHECK( Radar::FRIENDLY != Radar::HOSTILE );
			CHECK( Radar::FRIENDLY != Radar::INACTIVE );
			CHECK( Radar::FRIENDLY != Radar::SPECIAL );
			CHECK( Radar::FRIENDLY != Radar::ANOMALOUS );
			CHECK( Radar::FRIENDLY != Radar::BLINK );
			CHECK( Radar::FRIENDLY != Radar::VIEWPORT );
			CHECK( Radar::FRIENDLY != Radar::STAR );

			CHECK( Radar::UNFRIENDLY != Radar::HOSTILE );
			CHECK( Radar::UNFRIENDLY != Radar::INACTIVE );
			CHECK( Radar::UNFRIENDLY != Radar::SPECIAL );
			CHECK( Radar::UNFRIENDLY != Radar::ANOMALOUS );
			CHECK( Radar::UNFRIENDLY != Radar::BLINK );
			CHECK( Radar::UNFRIENDLY != Radar::VIEWPORT );
			CHECK( Radar::UNFRIENDLY != Radar::STAR );

			CHECK( Radar::HOSTILE != Radar::INACTIVE );
			CHECK( Radar::HOSTILE != Radar::SPECIAL );
			CHECK( Radar::HOSTILE != Radar::ANOMALOUS );
			CHECK( Radar::HOSTILE != Radar::BLINK );
			CHECK( Radar::HOSTILE != Radar::VIEWPORT );
			CHECK( Radar::HOSTILE != Radar::STAR );

			CHECK( Radar::INACTIVE != Radar::SPECIAL );
			CHECK( Radar::INACTIVE != Radar::ANOMALOUS );
			CHECK( Radar::INACTIVE != Radar::BLINK );
			CHECK( Radar::INACTIVE != Radar::VIEWPORT );
			CHECK( Radar::INACTIVE != Radar::STAR );

			CHECK( Radar::SPECIAL != Radar::ANOMALOUS );
			CHECK( Radar::SPECIAL != Radar::BLINK );
			CHECK( Radar::SPECIAL != Radar::VIEWPORT );
			CHECK( Radar::SPECIAL != Radar::STAR );

			CHECK( Radar::ANOMALOUS != Radar::BLINK );
			CHECK( Radar::ANOMALOUS != Radar::VIEWPORT );
			CHECK( Radar::ANOMALOUS != Radar::STAR );

			CHECK( Radar::BLINK != Radar::VIEWPORT );
			CHECK( Radar::BLINK != Radar::STAR );

			CHECK( Radar::VIEWPORT != Radar::STAR );
		}
	}
}

SCENARIO( "Radar constants represent semantic categories", "[Radar][Constants]" ) {
	GIVEN( "the semantic meaning of each constant" ) {
		THEN( "PLAYER represents the player's ship" ) {
			CHECK( Radar::PLAYER == 0 );
		}
		THEN( "FRIENDLY represents friendly ships" ) {
			CHECK( Radar::FRIENDLY == 1 );
		}
		THEN( "UNFRIENDLY represents neutral ships" ) {
			CHECK( Radar::UNFRIENDLY == 2 );
		}
		THEN( "HOSTILE represents hostile ships" ) {
			CHECK( Radar::HOSTILE == 3 );
		}
		THEN( "INACTIVE represents inactive objects" ) {
			CHECK( Radar::INACTIVE == 4 );
		}
		THEN( "SPECIAL represents special objects" ) {
			CHECK( Radar::SPECIAL == 5 );
		}
		THEN( "ANOMALOUS represents anomalous objects" ) {
			CHECK( Radar::ANOMALOUS == 6 );
		}
		THEN( "BLINK represents blinking objects" ) {
			CHECK( Radar::BLINK == 7 );
		}
		THEN( "VIEWPORT represents viewport boundaries" ) {
			CHECK( Radar::VIEWPORT == 8 );
		}
		THEN( "STAR represents stars" ) {
			CHECK( Radar::STAR == 9 );
		}
	}
}

SCENARIO( "Copying a Radar instance", "[Radar][Copy]" ) {
	GIVEN( "a Radar instance" ) {
		Radar original;
		original.SetCenter(Point(50.0, 100.0));

		WHEN( "copied by constructor" ) {
			Radar copy(original);
			THEN( "the copy can be used" ) {
				copy.Clear();
				copy.SetCenter(Point(0.0, 0.0));
			}
		}

		WHEN( "copied by assignment" ) {
			Radar copy = original;
			THEN( "the copy can be used" ) {
				copy.Clear();
				copy.SetCenter(Point(0.0, 0.0));
			}
		}
	}
}

SCENARIO( "Moving a Radar instance", "[Radar][Move]" ) {
	GIVEN( "a Radar instance" ) {
		Radar original;
		original.SetCenter(Point(50.0, 100.0));

		WHEN( "moved by constructor" ) {
			Radar moved(std::move(original));
			THEN( "the moved instance can be used" ) {
				moved.Clear();
				moved.SetCenter(Point(0.0, 0.0));
			}
		}

		WHEN( "moved by assignment" ) {
			Radar moved = std::move(original);
			THEN( "the moved instance can be used" ) {
				moved.Clear();
				moved.SetCenter(Point(0.0, 0.0));
			}
		}
	}
}

// #endregion unit tests



} // test namespace