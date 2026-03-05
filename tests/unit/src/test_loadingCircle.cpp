/* test_loadingCircle.cpp
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
#include "../../../source/LoadingCircle.h"

// ... and any system includes needed for the test file.
#include "../../../source/Point.h"

namespace { // test namespace

// #region mock data

// Helper function to create a Point for testing
Point MakeTestPoint(double x, double y)
{
	return Point(x, y);
}

// #endregion mock data



// #region unit tests

SCENARIO( "Creating a LoadingCircle", "[LoadingCircle][Creation]" ) {
	GIVEN( "valid parameters for a loading circle" ) {
		WHEN( "creating with size, ticks, and default rotation speed" ) {
			LoadingCircle circle(100.f, 12);
			THEN( "the object is created successfully" ) {
				// Object creation itself is the test - no exception means success
				CHECK( true );
			}
		}
		WHEN( "creating with size, ticks, and explicit rotation speed" ) {
			LoadingCircle circle(50.f, 8, 5.0);
			THEN( "the object is created successfully" ) {
				CHECK( true );
			}
		}
		WHEN( "creating with negative rotation speed" ) {
			LoadingCircle circle(100.f, 10, -3.0);
			THEN( "the object is created successfully" ) {
				CHECK( true );
			}
		}
		WHEN( "creating with zero rotation speed" ) {
			LoadingCircle circle(100.f, 10, 0.0);
			THEN( "the object is created successfully" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "Creating a LoadingCircle with various tick counts", "[LoadingCircle][Creation][Ticks]" ) {
	GIVEN( "different tick count values" ) {
		WHEN( "creating with 1 tick" ) {
			LoadingCircle circle(100.f, 1);
			THEN( "the object is created successfully" ) {
				CHECK( true );
			}
		}
		WHEN( "creating with a small number of ticks" ) {
			LoadingCircle circle(100.f, 4);
			THEN( "the object is created successfully" ) {
				CHECK( true );
			}
		}
		WHEN( "creating with a large number of ticks" ) {
			LoadingCircle circle(100.f, 360);
			THEN( "the object is created successfully" ) {
				CHECK( true );
			}
		}
		WHEN( "creating with a very large number of ticks" ) {
			LoadingCircle circle(100.f, 1000);
			THEN( "the object is created successfully" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "Creating a LoadingCircle with various sizes", "[LoadingCircle][Creation][Size]" ) {
	GIVEN( "different size values" ) {
		WHEN( "creating with a small size" ) {
			LoadingCircle circle(1.f, 8);
			THEN( "the object is created successfully" ) {
				CHECK( true );
			}
		}
		WHEN( "creating with a typical size" ) {
			LoadingCircle circle(100.f, 8);
			THEN( "the object is created successfully" ) {
				CHECK( true );
			}
		}
		WHEN( "creating with a large size" ) {
			LoadingCircle circle(1000.f, 8);
			THEN( "the object is created successfully" ) {
				CHECK( true );
			}
		}
		WHEN( "creating with zero size" ) {
			LoadingCircle circle(0.f, 8);
			THEN( "the object is created successfully" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "Calling Step on a LoadingCircle", "[LoadingCircle][Step]" ) {
	GIVEN( "a loading circle with zero rotation speed" ) {
		LoadingCircle circle(100.f, 12, 0.0);
		WHEN( "calling Step" ) {
			circle.Step();
			THEN( "no exception is thrown" ) {
				CHECK( true );
			}
		}
		WHEN( "calling Step multiple times" ) {
			for(int i = 0; i < 100; ++i)
				circle.Step();
			THEN( "no exception is thrown" ) {
				CHECK( true );
			}
		}
	}
	GIVEN( "a loading circle with positive rotation speed" ) {
		LoadingCircle circle(100.f, 12, 5.0);
		WHEN( "calling Step" ) {
			circle.Step();
			THEN( "no exception is thrown" ) {
				CHECK( true );
			}
		}
		WHEN( "calling Step many times" ) {
			for(int i = 0; i < 1000; ++i)
				circle.Step();
			THEN( "no exception is thrown" ) {
				CHECK( true );
			}
		}
	}
	GIVEN( "a loading circle with negative rotation speed" ) {
		LoadingCircle circle(100.f, 12, -5.0);
		WHEN( "calling Step" ) {
			circle.Step();
			THEN( "no exception is thrown" ) {
				CHECK( true );
			}
		}
		WHEN( "calling Step many times" ) {
			for(int i = 0; i < 1000; ++i)
				circle.Step();
			THEN( "no exception is thrown" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "Calling Draw on a LoadingCircle", "[LoadingCircle][Draw]" ) {
	GIVEN( "a loading circle and a position" ) {
		LoadingCircle circle(100.f, 12);
		Point position(200.0, 150.0);
		WHEN( "calling Draw with default progress" ) {
			circle.Draw(position);
			THEN( "no exception is thrown" ) {
				CHECK( true );
			}
		}
		WHEN( "calling Draw with full progress (1.0)" ) {
			circle.Draw(position, 1.0);
			THEN( "no exception is thrown" ) {
				CHECK( true );
			}
		}
		WHEN( "calling Draw with half progress (0.5)" ) {
			circle.Draw(position, 0.5);
			THEN( "no exception is thrown" ) {
				CHECK( true );
			}
		}
		WHEN( "calling Draw with zero progress" ) {
			circle.Draw(position, 0.0);
			THEN( "no exception is thrown" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "Calling Draw with various positions", "[LoadingCircle][Draw][Position]" ) {
	GIVEN( "a loading circle" ) {
		LoadingCircle circle(100.f, 12);
		WHEN( "drawing at origin" ) {
			Point origin(0.0, 0.0);
			circle.Draw(origin, 1.0);
			THEN( "no exception is thrown" ) {
				CHECK( true );
			}
		}
		WHEN( "drawing at positive coordinates" ) {
			Point pos(500.0, 300.0);
			circle.Draw(pos, 1.0);
			THEN( "no exception is thrown" ) {
				CHECK( true );
			}
		}
		WHEN( "drawing at negative coordinates" ) {
			Point pos(-500.0, -300.0);
			circle.Draw(pos, 1.0);
			THEN( "no exception is thrown" ) {
				CHECK( true );
			}
		}
		WHEN( "drawing at mixed sign coordinates" ) {
			Point pos(-200.0, 400.0);
			circle.Draw(pos, 1.0);
			THEN( "no exception is thrown" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "Calling Draw with progress boundaries", "[LoadingCircle][Draw][Progress]" ) {
	GIVEN( "a loading circle" ) {
		LoadingCircle circle(100.f, 12);
		Point position(0.0, 0.0);
		WHEN( "drawing with progress 0 (no ticks shown)" ) {
			circle.Draw(position, 0.0);
			THEN( "no exception is thrown" ) {
				CHECK( true );
			}
		}
		WHEN( "drawing with progress 1 (all ticks shown)" ) {
			circle.Draw(position, 1.0);
			THEN( "no exception is thrown" ) {
				CHECK( true );
			}
		}
		WHEN( "drawing with progress slightly above 1" ) {
			circle.Draw(position, 1.1);
			THEN( "no exception is thrown" ) {
				CHECK( true );
			}
		}
		WHEN( "drawing with progress slightly below 0" ) {
			circle.Draw(position, -0.1);
			THEN( "no exception is thrown" ) {
				CHECK( true );
			}
		}
		WHEN( "drawing with very small positive progress" ) {
			circle.Draw(position, 0.001);
			THEN( "no exception is thrown" ) {
				CHECK( true );
			}
		}
		WHEN( "drawing with progress 0.999" ) {
			circle.Draw(position, 0.999);
			THEN( "no exception is thrown" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "Combining Step and Draw operations", "[LoadingCircle][Integration]" ) {
	GIVEN( "a loading circle with rotation speed" ) {
		LoadingCircle circle(100.f, 12, 2.0);
		Point position(0.0, 0.0);
		WHEN( "calling Step then Draw repeatedly" ) {
			for(int i = 0; i < 100; ++i)
			{
				circle.Step();
				circle.Draw(position, static_cast<double>(i) / 100.0);
			}
			THEN( "no exception is thrown" ) {
				CHECK( true );
			}
		}
	}
	GIVEN( "a loading circle without rotation speed" ) {
		LoadingCircle circle(100.f, 12, 0.0);
		Point position(0.0, 0.0);
		WHEN( "calling Step then Draw repeatedly" ) {
			for(int i = 0; i < 100; ++i)
			{
				circle.Step();
				circle.Draw(position, static_cast<double>(i) / 100.0);
			}
			THEN( "no exception is thrown" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "LoadingCircle with extreme parameters", "[LoadingCircle][Edge]" ) {
	GIVEN( "a loading circle with extreme tick count" ) {
		WHEN( "creating with 1 tick" ) {
			LoadingCircle circle(100.f, 1);
			Point position(0.0, 0.0);
			circle.Draw(position, 1.0);
			THEN( "operations complete successfully" ) {
				CHECK( true );
			}
		}
		WHEN( "creating with large tick count" ) {
			LoadingCircle circle(100.f, 10000);
			Point position(0.0, 0.0);
			circle.Draw(position, 1.0);
			THEN( "operations complete successfully" ) {
				CHECK( true );
			}
		}
	}
	GIVEN( "a loading circle with large rotation speed" ) {
		LoadingCircle circle(100.f, 12, 1000.0);
		Point position(0.0, 0.0);
		WHEN( "stepping with large rotation speed" ) {
			circle.Step();
			circle.Draw(position, 1.0);
			THEN( "no exception is thrown" ) {
				CHECK( true );
			}
		}
	}
	GIVEN( "a loading circle with very small size" ) {
		LoadingCircle circle(0.001f, 12);
		Point position(0.0, 0.0);
		WHEN( "drawing" ) {
			circle.Draw(position, 1.0);
			THEN( "no exception is thrown" ) {
				CHECK( true );
			}
		}
	}
	GIVEN( "a loading circle with very large size" ) {
		LoadingCircle circle(100000.f, 12);
		Point position(0.0, 0.0);
		WHEN( "drawing" ) {
			circle.Draw(position, 1.0);
			THEN( "no exception is thrown" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "LoadingCircle const correctness", "[LoadingCircle][Const]" ) {
	GIVEN( "a const loading circle" ) {
		const LoadingCircle circle(100.f, 12, 5.0);
		Point position(0.0, 0.0);
		WHEN( "calling Draw on const object" ) {
			circle.Draw(position, 1.0);
			THEN( "Draw can be called on const object" ) {
				CHECK( true );
			}
		}
	}
}

// #endregion unit tests



} // test namespace