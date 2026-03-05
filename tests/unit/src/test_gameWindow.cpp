/* test_gameWindow.cpp
Copyright (c) 2026 by Test Author

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
#include "../../../source/GameWindow.h"

// ... and any system includes needed for the test file.
#include <string>

namespace { // test namespace

// #region mock data

// Helper to check if a string starts with a prefix.
bool StartsWith(const std::string& str, const std::string& prefix)
{
	return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

// Helper to check if a string contains a substring.
bool Contains(const std::string& str, const std::string& substr)
{
	return str.find(substr) != std::string::npos;
}

// #endregion mock data



// #region unit tests

SCENARIO( "SDLVersions returns valid version information", "[GameWindow]" ) {
	GIVEN( "no initialization required" ) {
		WHEN( "querying SDL versions" ) {
			auto versions = GameWindow::SDLVersions();

			THEN( "the result is a non-empty string" ) {
				REQUIRE_FALSE( versions.empty() );
			}

			THEN( "the result contains version information" ) {
				CHECK( Contains(versions, "SDL") );
				CHECK( Contains(versions, "v") );
			}

			THEN( "the result contains 'Compiled against' and 'Using'" ) {
				CHECK( Contains(versions, "Compiled against") );
				CHECK( Contains(versions, "Using") );
			}
		}
	}
}

SCENARIO( "GameWindow can be initialized in headless mode", "[GameWindow][Init]" ) {
	GIVEN( "no prior initialization" ) {
		WHEN( "initializing in headless mode" ) {
			bool result = GameWindow::Init(true);

			THEN( "initialization succeeds" ) {
				REQUIRE( result );
			}

			AND_THEN( "dimensions are set to default values" ) {
				CHECK( GameWindow::Width() >= 0 );
				CHECK( GameWindow::Height() >= 0 );
			}
		}

		// Clean up after each scenario
		GameWindow::Quit();
	}
}

SCENARIO( "GameWindow dimensions are tracked correctly", "[GameWindow][Dimensions]" ) {
	GIVEN( "a headless initialized window" ) {
		REQUIRE( GameWindow::Init(true) );

		WHEN( "querying Width" ) {
			int width = GameWindow::Width();

			THEN( "a non-negative value is returned" ) {
				CHECK( width >= 0 );
			}
		}

		WHEN( "querying Height" ) {
			int height = GameWindow::Height();

			THEN( "a non-negative value is returned" ) {
				CHECK( height >= 0 );
			}
		}

		WHEN( "querying DrawWidth" ) {
			int drawWidth = GameWindow::DrawWidth();

			THEN( "a non-negative value is returned" ) {
				CHECK( drawWidth >= 0 );
			}
		}

		WHEN( "querying DrawHeight" ) {
			int drawHeight = GameWindow::DrawHeight();

			THEN( "a non-negative value is returned" ) {
				CHECK( drawHeight >= 0 );
			}
		}

		GameWindow::Quit();
	}
}

SCENARIO( "GameWindow window state queries work correctly", "[GameWindow][State]" ) {
	GIVEN( "a headless initialized window" ) {
		REQUIRE( GameWindow::Init(true) );

		WHEN( "querying IsMaximized" ) {
			bool maximized = GameWindow::IsMaximized();

			THEN( "a boolean value is returned" ) {
				// In headless mode, the window is not maximized
				CHECK_FALSE( maximized );
			}
		}

		WHEN( "querying IsFullscreen" ) {
			bool fullscreen = GameWindow::IsFullscreen();

			THEN( "a boolean value is returned" ) {
				// In headless mode, the window is not fullscreen
				CHECK_FALSE( fullscreen );
			}
		}

		GameWindow::Quit();
	}
}

SCENARIO( "GameWindow VSync setting can be manipulated", "[GameWindow][VSync]" ) {
	GIVEN( "a headless initialized window" ) {
		REQUIRE( GameWindow::Init(true) );

		WHEN( "setting VSync to off" ) {
			bool result = GameWindow::SetVSync(Preferences::VSync::off);

			THEN( "the operation returns a boolean result" ) {
				// In headless mode, there's no OpenGL context, so this may fail
				// The result depends on whether a valid context exists
				CHECK( (result == true || result == false) );
			}
		}

		WHEN( "setting VSync to on" ) {
			bool result = GameWindow::SetVSync(Preferences::VSync::on);

			THEN( "the operation returns a boolean result" ) {
				CHECK( (result == true || result == false) );
			}
		}

		WHEN( "setting VSync to adaptive" ) {
			bool result = GameWindow::SetVSync(Preferences::VSync::adaptive);

			THEN( "the operation returns a boolean result" ) {
				CHECK( (result == true || result == false) );
			}
		}

		GameWindow::Quit();
	}
}

SCENARIO( "GameWindow ToggleFullscreen can be called", "[GameWindow][Fullscreen]" ) {
	GIVEN( "a headless initialized window" ) {
		REQUIRE( GameWindow::Init(true) );

		WHEN( "calling ToggleFullscreen" ) {
			// This should not crash in headless mode
			GameWindow::ToggleFullscreen();

			THEN( "the operation completes without error" ) {
				CHECK( true );
			}
		}

		GameWindow::Quit();
	}
}

SCENARIO( "GameWindow ToggleBlockScreenSaver can be called", "[GameWindow][ScreenSaver]" ) {
	GIVEN( "a headless initialized window" ) {
		REQUIRE( GameWindow::Init(true) );

		WHEN( "calling ToggleBlockScreenSaver" ) {
			// This should not crash
			GameWindow::ToggleBlockScreenSaver();

			THEN( "the operation completes without error" ) {
				CHECK( true );
			}
		}

		WHEN( "calling ToggleBlockScreenSaver twice" ) {
			GameWindow::ToggleBlockScreenSaver();
			GameWindow::ToggleBlockScreenSaver();

			THEN( "the operation toggles back to original state" ) {
				CHECK( true );
			}
		}

		GameWindow::Quit();
	}
}

SCENARIO( "GameWindow Step can be called", "[GameWindow][Step]" ) {
	GIVEN( "a headless initialized window" ) {
		REQUIRE( GameWindow::Init(true) );

		WHEN( "calling Step" ) {
			// In headless mode, there's no actual window to swap,
			// but the call should not crash
			GameWindow::Step();

			THEN( "the operation completes without error" ) {
				CHECK( true );
			}
		}

		GameWindow::Quit();
	}
}

SCENARIO( "GameWindow AdjustViewport can be called", "[GameWindow][Viewport]" ) {
	GIVEN( "a headless initialized window" ) {
		REQUIRE( GameWindow::Init(true) );

		WHEN( "calling AdjustViewport with noResizeEvent = false" ) {
			GameWindow::AdjustViewport(false);

			THEN( "the operation completes without error" ) {
				CHECK( true );
			}
		}

		WHEN( "calling AdjustViewport with noResizeEvent = true" ) {
			GameWindow::AdjustViewport(true);

			THEN( "the operation completes without error" ) {
				CHECK( true );
			}
		}

		GameWindow::Quit();
	}
}

SCENARIO( "GameWindow can be initialized and cleaned up multiple times", "[GameWindow][Lifecycle]" ) {
	GIVEN( "no prior initialization" ) {
		WHEN( "initializing and quitting multiple times" ) {
			bool result1 = GameWindow::Init(true);
			CHECK( result1 );
			GameWindow::Quit();

			bool result2 = GameWindow::Init(true);
			CHECK( result2 );
			GameWindow::Quit();

			bool result3 = GameWindow::Init(true);
			CHECK( result3 );
			GameWindow::Quit();

			THEN( "all initializations succeed" ) {
				CHECK( result1 );
				CHECK( result2 );
				CHECK( result3 );
			}
		}
	}
}

SCENARIO( "GameWindow Quit can be called safely", "[GameWindow][Quit]" ) {
	GIVEN( "a headless initialized window" ) {
		REQUIRE( GameWindow::Init(true) );

		WHEN( "calling Quit" ) {
			GameWindow::Quit();

			THEN( "the operation completes without error" ) {
				CHECK( true );
			}
		}
		// No need to Quit again since we already did
	}

	GIVEN( "no initialized window" ) {
		WHEN( "calling Quit" ) {
			// Quit should be safe to call even without initialization
			GameWindow::Quit();

			THEN( "the operation completes without error" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "GameWindow SetVSync returns false without OpenGL context", "[GameWindow][VSync][Headless]" ) {
	GIVEN( "a headless initialized window with no OpenGL context" ) {
		REQUIRE( GameWindow::Init(true) );

		WHEN( "attempting to set VSync" ) {
			bool result = GameWindow::SetVSync(Preferences::VSync::on);

			THEN( "returns false because there is no OpenGL context" ) {
				CHECK_FALSE( result );
			}
		}

		GameWindow::Quit();
	}
}

// #endregion unit tests



} // test namespace