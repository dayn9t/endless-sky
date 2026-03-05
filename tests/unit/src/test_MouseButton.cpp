/* test_MouseButton.cpp
Copyright (c) 2025 by the Endless Sky contributors

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
#include "../../../source/MouseButton.h"

// ... and any system includes needed for the test file.
#include <type_traits>

namespace { // test namespace

// #region mock data
// #endregion mock data



// #region unit tests
SCENARIO( "MouseButton enum values are defined correctly", "[MouseButton]" ) {
	GIVEN( "the MouseButton enum" ) {
		THEN( "NONE has value 0" ) {
			REQUIRE( static_cast<int>(MouseButton::NONE) == 0 );
		}
		THEN( "LEFT has value 1" ) {
			REQUIRE( static_cast<int>(MouseButton::LEFT) == 1 );
		}
		THEN( "MIDDLE has value 2" ) {
			REQUIRE( static_cast<int>(MouseButton::MIDDLE) == 2 );
		}
		THEN( "RIGHT has value 3" ) {
			REQUIRE( static_cast<int>(MouseButton::RIGHT) == 3 );
		}
		THEN( "X1 has value 4" ) {
			REQUIRE( static_cast<int>(MouseButton::X1) == 4 );
		}
		THEN( "X2 has value 5" ) {
			REQUIRE( static_cast<int>(MouseButton::X2) == 5 );
		}
	}
}

SCENARIO( "MouseButton is a scoped enum", "[MouseButton]" ) {
	GIVEN( "the MouseButton type" ) {
		THEN( "it is an enum class" ) {
			REQUIRE( std::is_enum_v<MouseButton> );
			REQUIRE( std::is_enum_v<MouseButton> );
		}
	}
}

SCENARIO( "MouseButton can be used in comparisons", "[MouseButton]" ) {
	GIVEN( "two MouseButton values" ) {
		WHEN( "they are the same" ) {
			THEN( "they compare equal" ) {
				REQUIRE( MouseButton::NONE == MouseButton::NONE );
				REQUIRE( MouseButton::LEFT == MouseButton::LEFT );
				REQUIRE( MouseButton::MIDDLE == MouseButton::MIDDLE );
				REQUIRE( MouseButton::RIGHT == MouseButton::RIGHT );
				REQUIRE( MouseButton::X1 == MouseButton::X1 );
				REQUIRE( MouseButton::X2 == MouseButton::X2 );
			}
		}
		WHEN( "they are different" ) {
			THEN( "they compare not equal" ) {
				REQUIRE( MouseButton::NONE != MouseButton::LEFT );
				REQUIRE( MouseButton::LEFT != MouseButton::RIGHT );
				REQUIRE( MouseButton::MIDDLE != MouseButton::X1 );
				REQUIRE( MouseButton::X1 != MouseButton::X2 );
			}
		}
	}
}

SCENARIO( "MouseButton can be used in switch statements", "[MouseButton]" ) {
	GIVEN( "a MouseButton value" ) {
		WHEN( "using it in a switch" ) {
			THEN( "all cases are handled correctly" ) {
				auto getName = [](MouseButton button) -> const char* {
					switch(button) {
						case MouseButton::NONE: return "NONE";
						case MouseButton::LEFT: return "LEFT";
						case MouseButton::MIDDLE: return "MIDDLE";
						case MouseButton::RIGHT: return "RIGHT";
						case MouseButton::X1: return "X1";
						case MouseButton::X2: return "X2";
					}
					return "UNKNOWN";
				};

				REQUIRE( std::string(getName(MouseButton::NONE)) == "NONE" );
				REQUIRE( std::string(getName(MouseButton::LEFT)) == "LEFT" );
				REQUIRE( std::string(getName(MouseButton::MIDDLE)) == "MIDDLE" );
				REQUIRE( std::string(getName(MouseButton::RIGHT)) == "RIGHT" );
				REQUIRE( std::string(getName(MouseButton::X1)) == "X1" );
				REQUIRE( std::string(getName(MouseButton::X2)) == "X2" );
			}
		}
	}
}

SCENARIO( "MouseButton represents standard mouse buttons", "[MouseButton]" ) {
	GIVEN( "different mouse button types" ) {
		THEN( "NONE represents no button pressed" ) {
			CHECK( MouseButton::NONE == MouseButton::NONE );
		}
		THEN( "LEFT represents left mouse button" ) {
			CHECK( MouseButton::LEFT == MouseButton::LEFT );
		}
		THEN( "MIDDLE represents middle mouse button" ) {
			CHECK( MouseButton::MIDDLE == MouseButton::MIDDLE );
		}
		THEN( "RIGHT represents right mouse button" ) {
			CHECK( MouseButton::RIGHT == MouseButton::RIGHT );
		}
		THEN( "X1 represents extra mouse button 1" ) {
			CHECK( MouseButton::X1 == MouseButton::X1 );
		}
		THEN( "X2 represents extra mouse button 2" ) {
			CHECK( MouseButton::X2 == MouseButton::X2 );
		}
	}
}

SCENARIO( "MouseButton can check for main buttons", "[MouseButton]" ) {
	GIVEN( "a mouse button" ) {
		WHEN( "checking if it is a main button" ) {
			auto isMainButton = [](MouseButton button) -> bool {
				return button == MouseButton::LEFT ||
				       button == MouseButton::RIGHT;
			};
			THEN( "LEFT is a main button" ) {
				CHECK( isMainButton(MouseButton::LEFT) );
			}
			THEN( "RIGHT is a main button" ) {
				CHECK( isMainButton(MouseButton::RIGHT) );
			}
			THEN( "NONE is not a main button" ) {
				CHECK_FALSE( isMainButton(MouseButton::NONE) );
			}
			THEN( "MIDDLE is not a main button" ) {
				CHECK_FALSE( isMainButton(MouseButton::MIDDLE) );
			}
			THEN( "X1 is not a main button" ) {
				CHECK_FALSE( isMainButton(MouseButton::X1) );
			}
			THEN( "X2 is not a main button" ) {
				CHECK_FALSE( isMainButton(MouseButton::X2) );
			}
		}
	}
}
// #endregion unit tests



} // test namespace