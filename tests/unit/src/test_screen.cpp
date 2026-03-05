/* test_screen.cpp
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
#include "../../../source/Screen.h"

// ... and any system includes needed for the test file.

namespace { // test namespace

// #region mock data

// Helper function to set up screen dimensions without triggering SDL events.
// The noResizeEvent parameter suppresses SDL event dispatching during tests.
void SetupScreen(int width, int height)
{
	Screen::SetRaw(width, height, true);
}

// Helper function to set zoom without triggering SDL events.
void SetupZoom(int percent)
{
	Screen::SetZoom(percent, true);
}

// #endregion mock data



// #region unit tests
SCENARIO( "Screen dimensions are queried", "[Screen]" ) {
	GIVEN( "a screen with specific dimensions" ) {
		SetupScreen(1920, 1080);

		WHEN( "querying Width" ) {
			THEN( "it returns the effective width" ) {
				CHECK( Screen::Width() == 1920 );
			}
		}

		WHEN( "querying Height" ) {
			THEN( "it returns the effective height" ) {
				CHECK( Screen::Height() == 1080 );
			}
		}

		WHEN( "querying Dimensions" ) {
			THEN( "it returns a Point with width and height" ) {
				auto dims = Screen::Dimensions();
				CHECK( dims.X() == 1920 );
				CHECK( dims.Y() == 1080 );
			}
		}
	}
}

SCENARIO( "Screen raw dimensions are queried", "[Screen]" ) {
	GIVEN( "a screen with specific raw dimensions" ) {
		SetupScreen(1920, 1080);

		WHEN( "querying RawWidth" ) {
			THEN( "it returns the raw width" ) {
				CHECK( Screen::RawWidth() == 1920 );
			}
		}

		WHEN( "querying RawHeight" ) {
			THEN( "it returns the raw height" ) {
				CHECK( Screen::RawHeight() == 1080 );
			}
		}
	}
}

SCENARIO( "Screen edge positions are calculated", "[Screen]" ) {
	GIVEN( "a screen with dimensions 1920x1080" ) {
		SetupScreen(1920, 1080);

		WHEN( "querying Left" ) {
			THEN( "it returns negative half of width" ) {
				CHECK( Screen::Left() == -960 );
			}
		}

		WHEN( "querying Top" ) {
			THEN( "it returns negative half of height" ) {
				CHECK( Screen::Top() == -540 );
			}
		}

		WHEN( "querying Right" ) {
			THEN( "it returns positive half of width" ) {
				CHECK( Screen::Right() == 960 );
			}
		}

		WHEN( "querying Bottom" ) {
			THEN( "it returns positive half of height" ) {
				CHECK( Screen::Bottom() == 540 );
			}
		}
	}

	GIVEN( "a screen with odd dimensions" ) {
		SetupScreen(1921, 1081);

		WHEN( "querying edges with odd width" ) {
			THEN( "Left is -960 (integer division)" ) {
				CHECK( Screen::Left() == -960 );
			}
			THEN( "Right is 960 (integer division)" ) {
				CHECK( Screen::Right() == 960 );
			}
		}

		WHEN( "querying edges with odd height" ) {
			THEN( "Top is -540 (integer division)" ) {
				CHECK( Screen::Top() == -540 );
			}
			THEN( "Bottom is 540 (integer division)" ) {
				CHECK( Screen::Bottom() == 540 );
			}
		}
	}
}

SCENARIO( "Screen corner positions are calculated", "[Screen]" ) {
	GIVEN( "a screen with dimensions 1920x1080" ) {
		SetupScreen(1920, 1080);

		WHEN( "querying TopLeft" ) {
			THEN( "it returns (-width/2, -height/2)" ) {
				auto corner = Screen::TopLeft();
				CHECK( corner.X() == -960.0 );
				CHECK( corner.Y() == -540.0 );
			}
		}

		WHEN( "querying TopRight" ) {
			THEN( "it returns (width/2, -height/2)" ) {
				auto corner = Screen::TopRight();
				CHECK( corner.X() == 960.0 );
				CHECK( corner.Y() == -540.0 );
			}
		}

		WHEN( "querying BottomLeft" ) {
			THEN( "it returns (-width/2, height/2)" ) {
				auto corner = Screen::BottomLeft();
				CHECK( corner.X() == -960.0 );
				CHECK( corner.Y() == 540.0 );
			}
		}

		WHEN( "querying BottomRight" ) {
			THEN( "it returns (width/2, height/2)" ) {
				auto corner = Screen::BottomRight();
				CHECK( corner.X() == 960.0 );
				CHECK( corner.Y() == 540.0 );
			}
		}
	}
}

SCENARIO( "Screen zoom is set", "[Screen][Zoom]" ) {
	GIVEN( "a screen with large dimensions" ) {
		SetupScreen(2000, 1000);
		SetupZoom(100);

		WHEN( "setting zoom to 150%" ) {
			SetupZoom(150);

			THEN( "UserZoom returns the set value" ) {
				CHECK( Screen::UserZoom() == 150 );
			}

			THEN( "effective dimensions are scaled down" ) {
				CHECK( Screen::Width() == 1333 );  // 2000 * 100 / 150
				CHECK( Screen::Height() == 666 );  // 1000 * 100 / 150
			}
		}

		WHEN( "setting zoom below 100%" ) {
			SetupZoom(50);

			THEN( "UserZoom is clamped to 100" ) {
				CHECK( Screen::UserZoom() == 100 );
			}
		}

		WHEN( "setting zoom above 200%" ) {
			SetupZoom(300);

			THEN( "UserZoom is clamped to 200" ) {
				CHECK( Screen::UserZoom() == 200 );
			}
		}
	}
}

SCENARIO( "Screen zoom respects minimum UI requirements", "[Screen][Zoom]" ) {
	GIVEN( "a small screen (1000x500)" ) {
		SetupScreen(1000, 500);

		WHEN( "setting zoom to 200%" ) {
			SetupZoom(200);

			THEN( "effective zoom is limited because UI needs 1000x500 minimum" ) {
				// minZoomX = 100 * 1000 / 1000 = 100
				// minZoomY = 100 * 500 / 500 = 100
				// minZoom = min(100, 100) = 100, rounded down to 100
				CHECK( Screen::Zoom() == 100 );
			}
		}
	}

	GIVEN( "a medium screen (1500x750)" ) {
		SetupScreen(1500, 750);

		WHEN( "setting zoom to 200%" ) {
			SetupZoom(200);

			THEN( "effective zoom is limited" ) {
				// minZoomX = 100 * 1500 / 1000 = 150
				// minZoomY = 100 * 750 / 500 = 150
				// minZoom = min(150, 150) = 150, rounded down to 150
				CHECK( Screen::Zoom() == 150 );
			}
		}
	}

	GIVEN( "a large screen (2000x1000)" ) {
		SetupScreen(2000, 1000);

		WHEN( "setting zoom to 200%" ) {
			SetupZoom(200);

			THEN( "user zoom is allowed" ) {
				CHECK( Screen::Zoom() == 200 );
			}
		}
	}
}

SCENARIO( "ScreenDimensionsGuard manages temporary dimension changes", "[Screen][ScreenDimensionsGuard]" ) {
	GIVEN( "initial screen dimensions" ) {
		SetupScreen(1920, 1080);

		WHEN( "creating a ScreenDimensionsGuard" ) {
			{
				Screen::ScreenDimensionsGuard guard(800, 600);

				THEN( "dimensions are temporarily changed" ) {
					CHECK( Screen::Width() == 800 );
					CHECK( Screen::Height() == 600 );
				}
			}

			THEN( "dimensions are restored when guard goes out of scope" ) {
				CHECK( Screen::Width() == 1920 );
				CHECK( Screen::Height() == 1080 );
			}
		}

		WHEN( "calling Deactivate explicitly" ) {
			Screen::ScreenDimensionsGuard guard(800, 600);
			CHECK( Screen::Width() == 800 );

			guard.Deactivate();

			THEN( "dimensions are restored immediately" ) {
				CHECK( Screen::Width() == 1920 );
				CHECK( Screen::Height() == 1080 );
			}

			AND_THEN( "calling Deactivate again has no effect" ) {
				guard.Deactivate();
				CHECK( Screen::Width() == 1920 );
				CHECK( Screen::Height() == 1080 );
			}
		}

		WHEN( "creating nested guards" ) {
			{
				Screen::ScreenDimensionsGuard guard1(1280, 720);
				CHECK( Screen::Width() == 1280 );
				CHECK( Screen::Height() == 720 );

				{
					Screen::ScreenDimensionsGuard guard2(640, 480);
					CHECK( Screen::Width() == 640 );
					CHECK( Screen::Height() == 480 );
				}

				THEN( "inner guard restores to outer guard's dimensions" ) {
					CHECK( Screen::Width() == 1280 );
					CHECK( Screen::Height() == 720 );
				}
			}

			THEN( "outer guard restores to original dimensions" ) {
				CHECK( Screen::Width() == 1920 );
				CHECK( Screen::Height() == 1080 );
			}
		}
	}
}

SCENARIO( "SetRaw updates raw and effective dimensions", "[Screen]" ) {
	GIVEN( "any initial state" ) {
		WHEN( "calling SetRaw with new dimensions" ) {
			SetupScreen(2560, 1440);

			THEN( "raw dimensions are updated" ) {
				CHECK( Screen::RawWidth() == 2560 );
				CHECK( Screen::RawHeight() == 1440 );
			}

			THEN( "effective dimensions match raw at 100% zoom" ) {
				CHECK( Screen::Width() == 2560 );
				CHECK( Screen::Height() == 1440 );
			}
		}

		WHEN( "calling SetRaw after changing zoom" ) {
			SetupScreen(1920, 1080);
			SetupZoom(200);
			CHECK( Screen::Width() == 960 );  // 1920 * 100 / 200

			SetupScreen(2560, 1440);

			THEN( "user zoom is preserved" ) {
				CHECK( Screen::UserZoom() == 200 );
			}

			THEN( "effective dimensions reflect the new raw dimensions" ) {
				CHECK( Screen::Width() == 1280 );  // 2560 * 100 / 200
				CHECK( Screen::Height() == 720 );  // 1440 * 100 / 200
			}
		}
	}
}

// #endregion unit tests



} // test namespace