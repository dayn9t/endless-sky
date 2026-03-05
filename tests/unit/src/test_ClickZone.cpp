/* test_ClickZone.cpp
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
#include "../../../source/ClickZone.h"

// ... and any system includes needed for the test file.
#include <string>
#include <utility>

namespace { // test namespace

// #region mock data
// #endregion mock data



// #region unit tests
SCENARIO( "Creating a ClickZone with int value", "[ClickZone][Creation]" ) {
	GIVEN( "center point and dimensions" ) {
		Point center(100.0, 200.0);
		Point dimensions(50.0, 80.0);
		WHEN( "creating a ClickZone with an int value" ) {
			ClickZone<int> zone(center, dimensions, 42);
			THEN( "the value is stored correctly" ) {
				CHECK( zone.Value() == 42 );
			}
			THEN( "the rectangle properties are correct" ) {
				CHECK( zone.Center() == center );
				CHECK( zone.Dimensions() == dimensions );
			}
		}
	}
}

SCENARIO( "Creating a ClickZone with default value", "[ClickZone][Creation]" ) {
	GIVEN( "center point and dimensions without explicit value" ) {
		Point center(0.0, 0.0);
		Point dimensions(100.0, 100.0);
		WHEN( "creating a ClickZone without a value" ) {
			ClickZone<int> zone(center, dimensions);
			THEN( "the default value is 0" ) {
				CHECK( zone.Value() == 0 );
			}
		}
	}
}

SCENARIO( "Creating a ClickZone from a Rectangle", "[ClickZone][Creation]" ) {
	GIVEN( "a Rectangle" ) {
		Point center(50.0, 75.0);
		Point dimensions(30.0, 40.0);
		Rectangle rect(center, dimensions);
		WHEN( "creating a ClickZone from the Rectangle" ) {
			ClickZone<int> zone(rect, 123);
			THEN( "the value is stored correctly" ) {
				CHECK( zone.Value() == 123 );
			}
			THEN( "the rectangle properties are preserved" ) {
				CHECK( zone.Center() == center );
				CHECK( zone.Dimensions() == dimensions );
			}
		}
	}
}

SCENARIO( "ClickZone with string value type", "[ClickZone][Generics]" ) {
	GIVEN( "a ClickZone with string value" ) {
		Point center(10.0, 20.0);
		Point dimensions(5.0, 10.0);
		std::string testValue = "test_button";
		WHEN( "creating the ClickZone" ) {
			ClickZone<std::string> zone(center, dimensions, testValue);
			THEN( "the string value is stored correctly" ) {
				CHECK( zone.Value() == testValue );
			}
		}
	}
}

SCENARIO( "ClickZone Contains method works correctly", "[ClickZone][Rectangle]" ) {
	GIVEN( "a ClickZone" ) {
		Point center(100.0, 100.0);
		Point dimensions(50.0, 50.0);
		ClickZone<int> zone(center, dimensions, 1);
		WHEN( "checking if a point inside is contained" ) {
			Point inside(105.0, 105.0);
			THEN( "the point is contained" ) {
				CHECK( zone.Contains(inside) );
			}
		}
		WHEN( "checking if a point outside is contained" ) {
			Point outside(200.0, 200.0);
			THEN( "the point is not contained" ) {
				CHECK_FALSE( zone.Contains(outside) );
			}
		}
	}
}

SCENARIO( "ClickZone Rectangle translation", "[ClickZone][Rectangle]" ) {
	GIVEN( "a ClickZone" ) {
		Point center(100.0, 100.0);
		Point dimensions(50.0, 50.0);
		ClickZone<int> zone(center, dimensions, 5);
		WHEN( "getting the underlying Rectangle and translating" ) {
			// ClickZone inherits from Rectangle, so we can use Rectangle methods
			Rectangle rect = zone;
			Point offset(50.0, 50.0);
			Rectangle translated = rect + offset;
			THEN( "the rectangle is translated" ) {
				// Rectangle + Point returns a translated Rectangle
				CHECK( translated.Center() == Point(150.0, 150.0) );
			}
		}
	}
}
// #endregion unit tests



} // test namespace