/* test_rectangle.cpp
Copyright (c) 2026 by Benjamin Hauch

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
#include "../../../source/Rectangle.h"

// ... and any system includes needed for the test file.
#include <cmath>

namespace { // test namespace

// #region mock data
// #endregion mock data



// #region unit tests
TEST_CASE( "Rectangle Basics", "[Rectangle]" ) {
	using T = Rectangle;
	SECTION( "Class Traits" ) {
		CHECK_FALSE( std::is_trivial_v<T> );
		CHECK( std::is_standard_layout_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
		CHECK( std::is_trivially_destructible_v<T> );
	}
	SECTION( "Construction Traits" ) {
		CHECK( std::is_default_constructible_v<T> );
		CHECK_FALSE( std::is_trivially_default_constructible_v<T> );
		CHECK( std::is_nothrow_default_constructible_v<T> );
		CHECK( std::is_copy_constructible_v<T> );
		CHECK( std::is_trivially_copy_constructible_v<T> );
		CHECK( std::is_nothrow_copy_constructible_v<T> );
		CHECK( std::is_move_constructible_v<T> );
		CHECK( std::is_trivially_move_constructible_v<T> );
		CHECK( std::is_nothrow_move_constructible_v<T> );
	}
	SECTION( "Copy Traits" ) {
		CHECK( std::is_copy_assignable_v<T> );
		CHECK( std::is_trivially_copyable_v<T> );
		CHECK( std::is_trivially_copy_assignable_v<T> );
		CHECK( std::is_nothrow_copy_assignable_v<T> );
	}
	SECTION( "Move Traits" ) {
		CHECK( std::is_move_assignable_v<T> );
		CHECK( std::is_trivially_move_assignable_v<T> );
		CHECK( std::is_nothrow_move_assignable_v<T> );
	}
}

SCENARIO( "A rectangle must be constructed", "[Rectangle]" ) {
	GIVEN( "No initial values" ) {
		WHEN( "a default-constructed rectangle is created" ) {
			Rectangle r;
			THEN( "it has zero center and dimensions" ) {
				CHECK( r.Center() == Point() );
				CHECK( r.Dimensions() == Point() );
				CHECK( r.Width() == 0. );
				CHECK( r.Height() == 0. );
			}
		}
	}

	GIVEN( "A center point and dimensions" ) {
		Point center(10., 20.);
		Point dimensions(100., 50.);
		WHEN( "a rectangle is constructed" ) {
			Rectangle r(center, dimensions);
			THEN( "the center and dimensions are correct" ) {
				CHECK( r.Center() == center );
				CHECK( r.Dimensions() == dimensions );
				CHECK( r.Width() == dimensions.X() );
				CHECK( r.Height() == dimensions.Y() );
			}
		}
	}

	GIVEN( "Negative dimensions" ) {
		Point center(10., 20.);
		Point dimensions(-100., -50.);
		WHEN( "a rectangle is constructed" ) {
			Rectangle r(center, dimensions);
			THEN( "dimensions are stored as absolute values" ) {
				CHECK( r.Center() == center );
				CHECK( r.Dimensions() == Point(100., 50.) );
				CHECK( r.Width() == 100. );
				CHECK( r.Height() == 50. );
			}
		}
	}
}

SCENARIO( "Rectangles can be created with WithCorners", "[Rectangle][WithCorners]" ) {
	GIVEN( "Two corner points in normal order (top-left to bottom-right)" ) {
		Point topLeft(0., 0.);
		Point bottomRight(100., 50.);
		WHEN( "a rectangle is created with WithCorners" ) {
			Rectangle r = Rectangle::WithCorners(topLeft, bottomRight);
			THEN( "the rectangle has the correct center and dimensions" ) {
				CHECK( r.Center() == Point(50., 25.) );
				CHECK( r.Dimensions() == Point(100., 50.) );
				CHECK( r.Left() == 0. );
				CHECK( r.Top() == 0. );
				CHECK( r.Right() == 100. );
				CHECK( r.Bottom() == 50. );
			}
		}
	}

	GIVEN( "Two corner points in reverse order (bottom-right to top-left)" ) {
		Point bottomRight(100., 50.);
		Point topLeft(0., 0.);
		WHEN( "a rectangle is created with WithCorners" ) {
			Rectangle r = Rectangle::WithCorners(bottomRight, topLeft);
			THEN( "the rectangle has the correct center and dimensions" ) {
				CHECK( r.Center() == Point(50., 25.) );
				CHECK( r.Dimensions() == Point(100., 50.) );
				CHECK( r.Left() == 0. );
				CHECK( r.Top() == 0. );
				CHECK( r.Right() == 100. );
				CHECK( r.Bottom() == 50. );
			}
		}
	}

	GIVEN( "Two corner points with negative coordinates" ) {
		Point corner1(-50., -25.);
		Point corner2(50., 25.);
		WHEN( "a rectangle is created with WithCorners" ) {
			Rectangle r = Rectangle::WithCorners(corner1, corner2);
			THEN( "the rectangle has the correct center and dimensions" ) {
				CHECK( r.Center() == Point(0., 0.) );
				CHECK( r.Dimensions() == Point(100., 50.) );
				CHECK( r.Left() == -50. );
				CHECK( r.Top() == -25. );
				CHECK( r.Right() == 50. );
				CHECK( r.Bottom() == 25. );
			}
		}
	}
}

SCENARIO( "Rectangles can be created with FromCorner", "[Rectangle][FromCorner]" ) {
	GIVEN( "A top-left corner and positive dimensions" ) {
		Point corner(0., 0.);
		Point dimensions(100., 50.);
		WHEN( "a rectangle is created with FromCorner" ) {
			Rectangle r = Rectangle::FromCorner(corner, dimensions);
			THEN( "the rectangle has the correct center and dimensions" ) {
				CHECK( r.Center() == Point(50., 25.) );
				CHECK( r.Dimensions() == Point(100., 50.) );
				CHECK( r.Left() == 0. );
				CHECK( r.Top() == 0. );
				CHECK( r.Right() == 100. );
				CHECK( r.Bottom() == 50. );
			}
		}
	}

	GIVEN( "A corner and negative dimensions" ) {
		Point corner(100., 50.);
		Point dimensions(-100., -50.);
		WHEN( "a rectangle is created with FromCorner" ) {
			Rectangle r = Rectangle::FromCorner(corner, dimensions);
			THEN( "the rectangle extends in the opposite direction" ) {
				CHECK( r.Center() == Point(50., 25.) );
				CHECK( r.Dimensions() == Point(100., 50.) );
				CHECK( r.Left() == 0. );
				CHECK( r.Top() == 0. );
				CHECK( r.Right() == 100. );
				CHECK( r.Bottom() == 50. );
			}
		}
	}

	GIVEN( "A corner with mixed sign dimensions" ) {
		Point corner(50., 25.);
		Point dimensions(-50., 50.);
		WHEN( "a rectangle is created with FromCorner" ) {
			Rectangle r = Rectangle::FromCorner(corner, dimensions);
			THEN( "the rectangle handles the mixed signs correctly" ) {
				CHECK( r.Center() == Point(25., 50.) );
				CHECK( r.Dimensions() == Point(50., 50.) );
				CHECK( r.Left() == 0. );
				CHECK( r.Top() == 25. );
				CHECK( r.Right() == 50. );
				CHECK( r.Bottom() == 75. );
			}
		}
	}
}

SCENARIO( "Rectangle corners are accessible", "[Rectangle]" ) {
	GIVEN( "A rectangle with known bounds" ) {
		Rectangle r(Point(50., 50.), Point(100., 60.));
		WHEN( "querying corners" ) {
			THEN( "all four corners are correct" ) {
				CHECK( r.TopLeft() == Point(0., 20.) );
				CHECK( r.TopRight() == Point(100., 20.) );
				CHECK( r.BottomLeft() == Point(0., 80.) );
				CHECK( r.BottomRight() == Point(100., 80.) );
			}
		}
	}
}

SCENARIO( "Rectangles can be translated", "[Rectangle]" ) {
	GIVEN( "A rectangle at a known position" ) {
		Rectangle original(Point(50., 50.), Point(100., 60.));
		WHEN( "adding a point offset" ) {
			Point offset(10., 15.);
			Rectangle shifted = original + offset;
			THEN( "the center is shifted but dimensions remain" ) {
				CHECK( shifted.Center() == Point(60., 65.) );
				CHECK( shifted.Dimensions() == original.Dimensions() );
			}
		}
		WHEN( "subtracting a point offset" ) {
			Point offset(10., 15.);
			Rectangle shifted = original - offset;
			THEN( "the center is shifted in the opposite direction" ) {
				CHECK( shifted.Center() == Point(40., 35.) );
				CHECK( shifted.Dimensions() == original.Dimensions() );
			}
		}
	}

	GIVEN( "A rectangle to modify in-place" ) {
		Rectangle r(Point(50., 50.), Point(100., 60.));
		Point offset(10., 15.);
		WHEN( "using += operator" ) {
			r += offset;
			THEN( "the center is modified" ) {
				CHECK( r.Center() == Point(60., 65.) );
			}
		}
		WHEN( "using -= operator" ) {
			r -= offset;
			THEN( "the center is modified" ) {
				CHECK( r.Center() == Point(40., 35.) );
			}
		}
	}
}

SCENARIO( "Rectangle contains point checks", "[Rectangle][Contains]" ) {
	GIVEN( "A rectangle at origin with dimensions 100x50" ) {
		Rectangle r(Point(0., 0.), Point(100., 50.));
		WHEN( "checking a point inside the rectangle" ) {
			THEN( "the point is contained" ) {
				CHECK( r.Contains(Point(0., 0.)) );
				CHECK( r.Contains(Point(50., 25.)) );
				CHECK( r.Contains(Point(-49., -24.)) );
				CHECK( r.Contains(Point(49., 24.)) );
			}
		}
		WHEN( "checking a point on the edge" ) {
			THEN( "the point is contained" ) {
				CHECK( r.Contains(Point(-50., -25.)) );
				CHECK( r.Contains(Point(50., 25.)) );
				CHECK( r.Contains(Point(50., -25.)) );
				CHECK( r.Contains(Point(-50., 25.)) );
			}
		}
		WHEN( "checking a point outside the rectangle" ) {
			THEN( "the point is not contained" ) {
				CHECK_FALSE( r.Contains(Point(51., 0.)) );
				CHECK_FALSE( r.Contains(Point(0., 26.)) );
				CHECK_FALSE( r.Contains(Point(-51., 0.)) );
				CHECK_FALSE( r.Contains(Point(0., -26.)) );
				CHECK_FALSE( r.Contains(Point(100., 100.)) );
			}
		}
	}
}

SCENARIO( "Rectangle contains rectangle checks", "[Rectangle][Contains]" ) {
	GIVEN( "An outer rectangle" ) {
		Rectangle outer(Point(0., 0.), Point(100., 100.));
		WHEN( "checking a smaller rectangle inside" ) {
			Rectangle inner(Point(0., 0.), Point(50., 50.));
			THEN( "the inner rectangle is contained" ) {
				CHECK( outer.Contains(inner) );
			}
		}
		WHEN( "checking an equal rectangle" ) {
			Rectangle equal(Point(0., 0.), Point(100., 100.));
			THEN( "the equal rectangle is contained" ) {
				CHECK( outer.Contains(equal) );
			}
		}
		WHEN( "checking a rectangle that extends outside" ) {
			Rectangle partial(Point(0., 0.), Point(150., 50.));
			THEN( "the rectangle is not contained" ) {
				CHECK_FALSE( outer.Contains(partial) );
			}
		}
		WHEN( "checking a rectangle completely outside" ) {
			Rectangle outside(Point(200., 200.), Point(50., 50.));
			THEN( "the rectangle is not contained" ) {
				CHECK_FALSE( outer.Contains(outside) );
			}
		}
		WHEN( "checking a rectangle touching the edge" ) {
			Rectangle touching(Point(50., 0.), Point(50., 100.));
			THEN( "the rectangle is contained" ) {
				CHECK( outer.Contains(touching) );
			}
		}
	}
}

SCENARIO( "Rectangle overlap checks with other rectangles", "[Rectangle][Overlaps]" ) {
	GIVEN( "A reference rectangle" ) {
		Rectangle r(Point(0., 0.), Point(100., 100.));
		WHEN( "checking a completely overlapping rectangle" ) {
			Rectangle overlap(Point(0., 0.), Point(50., 50.));
			THEN( "they overlap" ) {
				CHECK( r.Overlaps(overlap) );
				CHECK( overlap.Overlaps(r) );
			}
		}
		WHEN( "checking a partially overlapping rectangle" ) {
			Rectangle partial(Point(50., 50.), Point(100., 100.));
			THEN( "they overlap" ) {
				CHECK( r.Overlaps(partial) );
				CHECK( partial.Overlaps(r) );
			}
		}
		WHEN( "checking a rectangle to the right, not overlapping" ) {
			Rectangle right(Point(150., 0.), Point(50., 50.));
			THEN( "they do not overlap" ) {
				CHECK_FALSE( r.Overlaps(right) );
				CHECK_FALSE( right.Overlaps(r) );
			}
		}
		WHEN( "checking a rectangle above, not overlapping" ) {
			Rectangle above(Point(0., 100.), Point(50., 50.));
			THEN( "they do not overlap" ) {
				CHECK_FALSE( r.Overlaps(above) );
				CHECK_FALSE( above.Overlaps(r) );
			}
		}
		WHEN( "checking a rectangle touching at the edge" ) {
			Rectangle touching(Point(100., 0.), Point(50., 50.));
			THEN( "they overlap (edge case)" ) {
				CHECK( r.Overlaps(touching) );
				CHECK( touching.Overlaps(r) );
			}
		}
		WHEN( "checking a rectangle completely inside" ) {
			Rectangle inside(Point(0., 0.), Point(20., 20.));
			THEN( "they overlap" ) {
				CHECK( r.Overlaps(inside) );
				CHECK( inside.Overlaps(r) );
			}
		}
	}
}

SCENARIO( "Rectangle overlap checks with circles", "[Rectangle][Overlaps]" ) {
	GIVEN( "A rectangle centered at origin" ) {
		Rectangle r(Point(0., 0.), Point(100., 100.));
		WHEN( "checking a circle centered inside the rectangle" ) {
			THEN( "they overlap" ) {
				CHECK( r.Overlaps(Point(0., 0.), 10.) );
				CHECK( r.Overlaps(Point(40., 40.), 5.) );
			}
		}
		WHEN( "checking a circle completely outside" ) {
			THEN( "they do not overlap" ) {
				CHECK_FALSE( r.Overlaps(Point(200., 0.), 10.) );
				CHECK_FALSE( r.Overlaps(Point(0., 200.), 10.) );
			}
		}
		WHEN( "checking a circle near the edge that overlaps" ) {
			THEN( "they overlap" ) {
				CHECK( r.Overlaps(Point(60., 0.), 20.) );
				CHECK( r.Overlaps(Point(0., 60.), 20.) );
			}
		}
		WHEN( "checking a circle near the corner that overlaps" ) {
			THEN( "they overlap" ) {
				CHECK( r.Overlaps(Point(60., 60.), 20.) );
			}
		}
		WHEN( "checking a circle near the corner that does not overlap" ) {
			THEN( "they do not overlap" ) {
				CHECK_FALSE( r.Overlaps(Point(80., 80.), 10.) );
			}
		}
		WHEN( "checking a circle centered on the edge" ) {
			THEN( "they overlap" ) {
				CHECK( r.Overlaps(Point(50., 0.), 10.) );
				CHECK( r.Overlaps(Point(0., 50.), 10.) );
			}
		}
		WHEN( "checking a circle with zero radius" ) {
			THEN( "overlap depends on whether center is inside" ) {
				CHECK( r.Overlaps(Point(0., 0.), 0.) );
				CHECK_FALSE( r.Overlaps(Point(60., 0.), 0.) );
			}
		}
	}
}

SCENARIO( "Copying Rectangles", "[Rectangle]" ) {
	GIVEN( "any Rectangle" ) {
		auto source = Rectangle(Point(25.5, -30.75), Point(100., 200.));
		WHEN( "copied by constructor" ) {
			Rectangle copy(source);
			THEN( "the copy has the correct values" ) {
				CHECK( copy.Center() == source.Center() );
				CHECK( copy.Dimensions() == source.Dimensions() );
			}
		}
		WHEN( "copied by assignment" ) {
			Rectangle copy = source;
			THEN( "the copy has the correct values" ) {
				CHECK( copy.Center() == source.Center() );
				CHECK( copy.Dimensions() == source.Dimensions() );
			}
		}
	}
}

SCENARIO( "Rectangle edge coordinate accessors", "[Rectangle]" ) {
	GIVEN( "A rectangle with known bounds" ) {
		Rectangle r(Point(100., 50.), Point(50., 30.));
		WHEN( "querying edge coordinates" ) {
			THEN( "the edge coordinates are correct" ) {
				CHECK( r.Left() == 75. );
				CHECK( r.Right() == 125. );
				CHECK( r.Top() == 35. );
				CHECK( r.Bottom() == 65. );
			}
		}
	}
}

SCENARIO( "Rectangle with zero dimensions", "[Rectangle]" ) {
	GIVEN( "A rectangle with zero width and height" ) {
		Rectangle r(Point(10., 20.), Point(0., 0.));
		WHEN( "checking its properties" ) {
			THEN( "it represents a single point" ) {
				CHECK( r.Width() == 0. );
				CHECK( r.Height() == 0. );
				CHECK( r.Left() == 10. );
				CHECK( r.Right() == 10. );
				CHECK( r.Top() == 20. );
				CHECK( r.Bottom() == 20. );
			}
		}
		WHEN( "checking containment" ) {
			THEN( "only the center point is contained" ) {
				CHECK( r.Contains(Point(10., 20.)) );
				CHECK_FALSE( r.Contains(Point(10.1, 20.)) );
				CHECK_FALSE( r.Contains(Point(10., 20.1)) );
			}
		}
	}
}
// #endregion unit tests

} // test namespace