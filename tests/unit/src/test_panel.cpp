/* test_panel.cpp
Copyright (c) 2026 by TomGoodIdea

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
#include "../../../source/Panel.h"

// ... and any system includes needed for the test file.
#include <memory>
#include <type_traits>

namespace { // test namespace

// #region mock data

// Concrete implementation of Panel for testing purposes.
class TestPanel : public Panel {
public:
	// Implement the pure virtual Draw method.
	void Draw() override {}
};

// #endregion mock data



// #region unit tests

SCENARIO( "Panel class traits", "[Panel]" ) {
	GIVEN( "the Panel class" ) {
		THEN( "it is not default constructible" ) {
			CHECK_FALSE( std::is_default_constructible_v<Panel> );
		}
		AND_THEN( "it is not copy constructible" ) {
			CHECK_FALSE( std::is_copy_constructible_v<Panel> );
		}
		AND_THEN( "it is not move constructible" ) {
			CHECK_FALSE( std::is_move_constructible_v<Panel> );
		}
		AND_THEN( "it is destructible" ) {
			CHECK( std::is_destructible_v<Panel> );
		}
	}
}

SCENARIO( "TestPanel default construction", "[Panel]" ) {
	GIVEN( "no parameters" ) {
		WHEN( "a TestPanel is default constructed" ) {
			TestPanel panel;
			THEN( "it is created successfully" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "TestPanel IsFullScreen", "[Panel]" ) {
	GIVEN( "a TestPanel" ) {
		TestPanel panel;
		WHEN( "IsFullScreen is called" ) {
			bool isFullScreen = panel.IsFullScreen();
			THEN( "it returns false by default" ) {
				CHECK_FALSE( isFullScreen );
			}
		}
	}
}

SCENARIO( "TestPanel TrapAllEvents", "[Panel]" ) {
	GIVEN( "a TestPanel" ) {
		TestPanel panel;
		WHEN( "TrapAllEvents is called" ) {
			bool trapsAll = panel.TrapAllEvents();
			THEN( "it returns true by default (panels trap all events by default)" ) {
				CHECK( trapsAll );
			}
		}
	}
}

SCENARIO( "TestPanel IsInterruptible", "[Panel]" ) {
	GIVEN( "a TestPanel" ) {
		TestPanel panel;
		WHEN( "IsInterruptible is called" ) {
			bool interruptible = panel.IsInterruptible();
			THEN( "it returns true by default" ) {
				CHECK( interruptible );
			}
		}
	}
}

// Note: Many Panel methods are protected and require a derived class to access.
// Protected methods like SetIsFullScreen, SetTrapAllEvents, SetInterruptible,
// KeyDown, Click, Hover, Drag, Release, Scroll, etc. can only be tested
// through a derived class with using declarations.
//
// The Panel class is designed as an abstract base for UI components.
// Full testing would require integration with the UI system.

// #endregion unit tests



} // test namespace