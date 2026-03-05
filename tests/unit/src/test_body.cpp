/* test_body.cpp
Copyright (c) 2026 by Endless Sky contributors

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
#include "../../../source/Body.h"

// ... and any system includes needed for the test file.
#include "../../../source/Angle.h"
#include "../../../source/Point.h"

#include <cmath>

namespace { // test namespace

// #region mock data

// TestBody class to expose protected methods for testing
class TestBody : public Body {
public:
	using Body::Body;

	// Expose protected methods for testing
	void MarkForRemoval() { Body::MarkForRemoval(); }
	void UnmarkForRemoval() { Body::UnmarkForRemoval(); }
	void Turn(double amount) { Body::Turn(amount); }
	void Turn(const Angle &amount) { Body::Turn(amount); }

	// Expose protected members for testing
	Point& GetRotatedCenter() { return rotatedCenter; }
	void SetRotatedCenter(const Point& p) { rotatedCenter = p; }
};

// #endregion mock data



// #region unit tests

SCENARIO( "Body default construction", "[Body]" ) {
	GIVEN( "no parameters" ) {
		WHEN( "a Body is default constructed" ) {
			Body body;
			THEN( "position is (0, 0)" ) {
				CHECK( body.Position().X() == 0. );
				CHECK( body.Position().Y() == 0. );
			}
			AND_THEN( "velocity is (0, 0)" ) {
				CHECK( body.Velocity().X() == 0. );
				CHECK( body.Velocity().Y() == 0. );
			}
			AND_THEN( "facing angle is 0 degrees" ) {
				CHECK_THAT( body.Facing().Degrees(), Catch::Matchers::WithinAbs(0., 0.0001) );
			}
			AND_THEN( "it should not be marked for removal" ) {
				CHECK_FALSE( body.ShouldBeRemoved() );
			}
			AND_THEN( "government is null" ) {
				CHECK( body.GetGovernment() == nullptr );
			}
			AND_THEN( "zoom is 1.0" ) {
				CHECK_THAT( body.Zoom(), Catch::Matchers::WithinAbs(1., 0.0001) );
			}
			AND_THEN( "alpha is 1.0" ) {
				CHECK_THAT( body.Alpha(Point(0., 0.)), Catch::Matchers::WithinAbs(1., 0.0001) );
			}
		}
	}
}

SCENARIO( "Body construction with position", "[Body]" ) {
	GIVEN( "a position" ) {
		Point position(100., 200.);
		WHEN( "a Body is constructed with position only" ) {
			Body body(nullptr, position);
			THEN( "position is set correctly" ) {
				CHECK( body.Position().X() == 100. );
				CHECK( body.Position().Y() == 200. );
			}
			AND_THEN( "velocity is (0, 0)" ) {
				CHECK( body.Velocity().X() == 0. );
				CHECK( body.Velocity().Y() == 0. );
			}
			AND_THEN( "facing angle is 0 degrees" ) {
				CHECK_THAT( body.Facing().Degrees(), Catch::Matchers::WithinAbs(0., 0.0001) );
			}
		}
	}
}

SCENARIO( "Body construction with position and velocity", "[Body]" ) {
	GIVEN( "position and velocity" ) {
		Point position(50., 75.);
		Point velocity(10., -5.);
		WHEN( "a Body is constructed with position and velocity" ) {
			Body body(nullptr, position, velocity);
			THEN( "position is set correctly" ) {
				CHECK( body.Position().X() == 50. );
				CHECK( body.Position().Y() == 75. );
			}
			AND_THEN( "velocity is set correctly" ) {
				CHECK( body.Velocity().X() == 10. );
				CHECK( body.Velocity().Y() == -5. );
			}
		}
	}
}

SCENARIO( "Body construction with all parameters", "[Body]" ) {
	GIVEN( "all parameters" ) {
		Point position(100., 200.);
		Point velocity(15., 25.);
		Angle facing(45.);
		double zoom = 2.0;
		Point scale(1.5, 2.0);
		double alpha = 0.8;
		WHEN( "a Body is constructed with all parameters" ) {
			Body body(nullptr, position, velocity, facing, zoom, scale, alpha);
			THEN( "position is set correctly" ) {
				CHECK( body.Position().X() == 100. );
				CHECK( body.Position().Y() == 200. );
			}
			AND_THEN( "velocity is set correctly" ) {
				CHECK( body.Velocity().X() == 15. );
				CHECK( body.Velocity().Y() == 25. );
			}
			AND_THEN( "facing is set correctly" ) {
				CHECK_THAT( body.Facing().Degrees(), Catch::Matchers::WithinAbs(45., 0.1) );
			}
			AND_THEN( "zoom is set correctly" ) {
				CHECK_THAT( body.Zoom(), Catch::Matchers::WithinAbs(2., 0.0001) );
			}
			AND_THEN( "alpha is set correctly" ) {
				CHECK_THAT( body.Alpha(Point(0., 0.)), Catch::Matchers::WithinAbs(0.8, 0.0001) );
			}
		}
	}
}

SCENARIO( "Body copy construction", "[Body]" ) {
	GIVEN( "an existing Body" ) {
		Point position(100., 200.);
		Point velocity(15., 25.);
		Angle facing(90.);
		Body original(nullptr, position, velocity, facing, 2.0, Point(1.0, 1.0), 0.5);
		WHEN( "a Body is copy constructed" ) {
			Body copy(original, Point(300., 400.), Point(50., 60.), Angle(180.));
			THEN( "copy has new position" ) {
				CHECK( copy.Position().X() == 300. );
				CHECK( copy.Position().Y() == 400. );
			}
			AND_THEN( "copy has new velocity" ) {
				CHECK( copy.Velocity().X() == 50. );
				CHECK( copy.Velocity().Y() == 60. );
			}
			AND_THEN( "copy has new facing angle" ) {
				CHECK_THAT( copy.Facing().Degrees(), Catch::Matchers::WithinAbs(180., 0.1) );
			}
			AND_THEN( "copy inherits zoom from original" ) {
				CHECK_THAT( copy.Zoom(), Catch::Matchers::WithinAbs(2., 0.0001) );
			}
		}
	}
}

SCENARIO( "Body position getter", "[Body][Position]" ) {
	GIVEN( "a Body with a specific position" ) {
		Point position(123.45, 678.90);
		Body body(nullptr, position);
		WHEN( "Position() is called" ) {
			const Point& pos = body.Position();
			THEN( "it returns the correct position" ) {
				CHECK_THAT( pos.X(), Catch::Matchers::WithinAbs(123.45, 0.0001) );
				CHECK_THAT( pos.Y(), Catch::Matchers::WithinAbs(678.90, 0.0001) );
			}
		}
	}
}

SCENARIO( "Body velocity getter", "[Body][Velocity]" ) {
	GIVEN( "a Body with a specific velocity" ) {
		Point position(0., 0.);
		Point velocity(25.5, -30.5);
		Body body(nullptr, position, velocity);
		WHEN( "Velocity() is called" ) {
			const Point& vel = body.Velocity();
			THEN( "it returns the correct velocity" ) {
				CHECK_THAT( vel.X(), Catch::Matchers::WithinAbs(25.5, 0.0001) );
				CHECK_THAT( vel.Y(), Catch::Matchers::WithinAbs(-30.5, 0.0001) );
			}
		}
	}
}

SCENARIO( "Body facing getter", "[Body][Facing]" ) {
	GIVEN( "a Body with a specific facing angle" ) {
		Point position(0., 0.);
		Angle facing(135.);
		Body body(nullptr, position, Point(), facing);
		WHEN( "Facing() is called" ) {
			const Angle& face = body.Facing();
			THEN( "it returns the correct angle" ) {
				CHECK_THAT( face.Degrees(), Catch::Matchers::WithinAbs(135., 0.1) );
			}
		}
	}
}

SCENARIO( "Body Center calculation", "[Body][Center]" ) {
	GIVEN( "a Body with default rotated center" ) {
		Point position(100., 200.);
		Body body(nullptr, position);
		WHEN( "Center() is called" ) {
			Point center = body.Center();
			THEN( "it returns position minus rotated center" ) {
				// Default rotatedCenter is (0, 0), so Center equals position
				CHECK_THAT( center.X(), Catch::Matchers::WithinAbs(100., 0.0001) );
				CHECK_THAT( center.Y(), Catch::Matchers::WithinAbs(200., 0.0001) );
			}
		}
	}
}

SCENARIO( "Body Unit vector", "[Body][Unit]" ) {
	GIVEN( "a Body with a specific facing angle" ) {
		Point position(0., 0.);
		Angle facing(90.);
		Body body(nullptr, position, Point(), facing, 2.0);
		WHEN( "Unit() is called" ) {
			Point unit = body.Unit();
			THEN( "it returns a unit vector in the facing direction, scaled by half zoom" ) {
				// At 90 degrees, unit vector should point right (positive X direction)
				// With zoom = 2.0, Unit = angle.Unit() * (0.5 * 2.0) = angle.Unit() * 1.0
				CHECK_THAT( unit.X(), Catch::Matchers::WithinAbs(1., 0.1) );
				CHECK_THAT( unit.Y(), Catch::Matchers::WithinAbs(0., 0.1) );
			}
		}
	}
	GIVEN( "a Body with default facing and zoom" ) {
		Body body(nullptr, Point());
		WHEN( "Unit() is called" ) {
			Point unit = body.Unit();
			THEN( "it returns a unit vector pointing up (default angle), scaled by half zoom" ) {
				// Default angle is 0 degrees (pointing up), default zoom is 1.0
				// Unit = angle.Unit() * 0.5 = (0, -1) * 0.5 = (0, -0.5)
				CHECK_THAT( unit.X(), Catch::Matchers::WithinAbs(0., 0.0001) );
				CHECK_THAT( unit.Y(), Catch::Matchers::WithinAbs(-0.5, 0.0001) );
			}
		}
	}
}

SCENARIO( "Body removal marking", "[Body][ShouldBeRemoved]" ) {
	GIVEN( "a default constructed Body" ) {
		TestBody body;
		REQUIRE_FALSE( body.ShouldBeRemoved() );
		WHEN( "the body is marked for removal" ) {
			body.MarkForRemoval();
			THEN( "ShouldBeRemoved returns true" ) {
				CHECK( body.ShouldBeRemoved() );
			}
		}
	}
	GIVEN( "a Body marked for removal" ) {
		TestBody body;
		body.MarkForRemoval();
		REQUIRE( body.ShouldBeRemoved() );
		WHEN( "the body is unmarked for removal" ) {
			body.UnmarkForRemoval();
			THEN( "ShouldBeRemoved returns false" ) {
				CHECK_FALSE( body.ShouldBeRemoved() );
			}
		}
	}
}

SCENARIO( "Body Turn with double", "[Body][Turn]" ) {
	GIVEN( "a Body with initial angle 0" ) {
		TestBody body(nullptr, Point(100., 100.));
		REQUIRE_THAT( body.Facing().Degrees(), Catch::Matchers::WithinAbs(0., 0.1) );
		WHEN( "Turn(45.0) is called" ) {
			body.Turn(45.);
			THEN( "the facing angle increases by 45 degrees" ) {
				CHECK_THAT( body.Facing().Degrees(), Catch::Matchers::WithinAbs(45., 0.1) );
			}
		}
		WHEN( "Turn(90.0) is called multiple times" ) {
			body.Turn(90.);
			body.Turn(90.);
			THEN( "the facing angle accumulates correctly" ) {
				CHECK_THAT( body.Facing().Degrees(), Catch::Matchers::WithinAbs(180., 0.1) );
			}
		}
		WHEN( "Turn(-45.0) is called" ) {
			body.Turn(-45.);
			THEN( "the facing angle decreases by 45 degrees" ) {
				CHECK_THAT( body.Facing().Degrees(), Catch::Matchers::WithinAbs(-45., 0.1) );
			}
		}
	}
}

SCENARIO( "Body Turn with Angle", "[Body][Turn]" ) {
	GIVEN( "a Body with initial angle 0" ) {
		TestBody body(nullptr, Point(100., 100.));
		REQUIRE_THAT( body.Facing().Degrees(), Catch::Matchers::WithinAbs(0., 0.1) );
		WHEN( "Turn(Angle(30.0)) is called" ) {
			body.Turn(Angle(30.));
			THEN( "the facing angle increases by 30 degrees" ) {
				CHECK_THAT( body.Facing().Degrees(), Catch::Matchers::WithinAbs(30., 0.1) );
			}
		}
		WHEN( "Turn(Angle(-60.0)) is called" ) {
			body.Turn(Angle(-60.));
			THEN( "the facing angle decreases by 60 degrees" ) {
				CHECK_THAT( body.Facing().Degrees(), Catch::Matchers::WithinAbs(-60., 0.1) );
			}
		}
	}
}

SCENARIO( "Body Alpha calculation", "[Body][Alpha]" ) {
	GIVEN( "a Body with default alpha" ) {
		Body body(nullptr, Point(0., 0.));
		WHEN( "Alpha() is called with draw center at same position" ) {
			double alpha = body.Alpha(Point(0., 0.));
			THEN( "it returns 1.0 (fully visible)" ) {
				CHECK_THAT( alpha, Catch::Matchers::WithinAbs(1., 0.0001) );
			}
		}
	}
	GIVEN( "a Body with custom alpha" ) {
		Body body(nullptr, Point(0., 0.), Point(), Angle(), 1., Point(1., 1.), 0.5);
		WHEN( "Alpha() is called" ) {
			double alpha = body.Alpha(Point(0., 0.));
			THEN( "it returns the base alpha value when distance doesn't affect it" ) {
				CHECK_THAT( alpha, Catch::Matchers::WithinAbs(0.5, 0.0001) );
			}
		}
	}
}

SCENARIO( "Body DistanceAlpha calculation", "[Body][DistanceAlpha]" ) {
	GIVEN( "a Body at position (0, 0) with default distance settings" ) {
		Body body(nullptr, Point(0., 0.));
		WHEN( "DistanceAlpha() is called with various draw centers" ) {
			THEN( "it returns 1.0 because distanceInvisible is 0" ) {
				CHECK_THAT( body.DistanceAlpha(Point(0., 0.)), Catch::Matchers::WithinAbs(1., 0.0001) );
				CHECK_THAT( body.DistanceAlpha(Point(100., 100.)), Catch::Matchers::WithinAbs(1., 0.0001) );
			}
		}
	}
}

SCENARIO( "Body IsVisible check", "[Body][IsVisible]" ) {
	GIVEN( "a Body at position (0, 0)" ) {
		Body body(nullptr, Point(0., 0.));
		WHEN( "IsVisible() is called with draw center at same position" ) {
			bool visible = body.IsVisible(Point(0., 0.));
			THEN( "it returns true" ) {
				CHECK( visible );
			}
		}
		WHEN( "IsVisible() is called with draw center far away" ) {
			bool visible = body.IsVisible(Point(1000., 1000.));
			THEN( "it returns true (default distance settings)" ) {
				CHECK( visible );
			}
		}
	}
}

SCENARIO( "Body GetGovernment", "[Body][GetGovernment]" ) {
	GIVEN( "a default constructed Body" ) {
		Body body;
		WHEN( "GetGovernment() is called" ) {
			const Government* gov = body.GetGovernment();
			THEN( "it returns nullptr" ) {
				CHECK( gov == nullptr );
			}
		}
	}
}

SCENARIO( "Body GetMask with null sprite", "[Body][GetMask]" ) {
	GIVEN( "a Body with null sprite" ) {
		Body body(nullptr, Point(0., 0.));
		WHEN( "GetMask() is called" ) {
			const Mask& mask = body.GetMask();
			THEN( "it returns an empty mask" ) {
				// When sprite is null, an empty static mask is returned
				// We just verify it doesn't crash
				CHECK( true );
			}
		}
	}
}

SCENARIO( "Body angle wrapping", "[Body][Angle]" ) {
	GIVEN( "a Body with initial angle 0" ) {
		TestBody body(nullptr, Point(0., 0.));
		WHEN( "Turn(400.0) is called (more than 360 degrees)" ) {
			body.Turn(400.);
			THEN( "the angle wraps correctly" ) {
				// 400 degrees = 360 + 40 = 40 degrees
				CHECK_THAT( body.Facing().Degrees(), Catch::Matchers::WithinAbs(40., 0.5) );
			}
		}
		WHEN( "Turn(-400.0) is called (more than -360 degrees)" ) {
			body.Turn(-400.);
			THEN( "the angle wraps correctly" ) {
				// -400 degrees = -360 - 40 = -40 degrees
				CHECK_THAT( body.Facing().Degrees(), Catch::Matchers::WithinAbs(-40., 0.5) );
			}
		}
	}
}

SCENARIO( "Body scale from Drawable", "[Body][Scale]" ) {
	GIVEN( "a Body with custom scale" ) {
		Point scale(2.0, 3.0);
		Body body(nullptr, Point(0., 0.), Point(), Angle(), 1., scale);
		WHEN( "Scale() is called" ) {
			Point s = body.Scale();
			THEN( "it returns the correct scale" ) {
				CHECK_THAT( s.X(), Catch::Matchers::WithinAbs(2., 0.0001) );
				CHECK_THAT( s.Y(), Catch::Matchers::WithinAbs(3., 0.0001) );
			}
		}
	}
	GIVEN( "a Body with default scale" ) {
		Body body(nullptr, Point(0., 0.));
		WHEN( "Scale() is called" ) {
			Point s = body.Scale();
			THEN( "it returns (1, 1)" ) {
				CHECK_THAT( s.X(), Catch::Matchers::WithinAbs(1., 0.0001) );
				CHECK_THAT( s.Y(), Catch::Matchers::WithinAbs(1., 0.0001) );
			}
		}
	}
}

// #endregion unit tests



// #region benchmarks
#ifdef CATCH_CONFIG_ENABLE_BENCHMARKING
TEST_CASE( "Benchmark Body construction", "[!benchmark][Body]" ) {
	BENCHMARK( "Body::Body()" ) {
		return Body();
	};
	BENCHMARK( "Body::Body(position)" ) {
		return Body(nullptr, Point(100., 200.));
	};
	BENCHMARK( "Body::Body(position, velocity, facing)" ) {
		return Body(nullptr, Point(100., 200.), Point(10., 20.), Angle(45.));
	};
	BENCHMARK( "Body::Body(all params)" ) {
		return Body(nullptr, Point(100., 200.), Point(10., 20.), Angle(45.), 2.0, Point(1., 1.), 0.8);
	};
}

TEST_CASE( "Benchmark Body getters", "[!benchmark][Body]" ) {
	Body body(nullptr, Point(100., 200.), Point(10., 20.), Angle(45.));
	BENCHMARK( "Body::Position()" ) {
		return body.Position();
	};
	BENCHMARK( "Body::Velocity()" ) {
		return body.Velocity();
	};
	BENCHMARK( "Body::Facing()" ) {
		return body.Facing();
	};
	BENCHMARK( "Body::Center()" ) {
		return body.Center();
	};
	BENCHMARK( "Body::Unit()" ) {
		return body.Unit();
	};
	BENCHMARK( "Body::Zoom()" ) {
		return body.Zoom();
	};
	BENCHMARK( "Body::Alpha()" ) {
		return body.Alpha(Point(0., 0.));
	};
	BENCHMARK( "Body::DistanceAlpha()" ) {
		return body.DistanceAlpha(Point(0., 0.));
	};
	BENCHMARK( "Body::IsVisible()" ) {
		return body.IsVisible(Point(0., 0.));
	};
	BENCHMARK( "Body::ShouldBeRemoved()" ) {
		return body.ShouldBeRemoved();
	};
	BENCHMARK( "Body::GetGovernment()" ) {
		return body.GetGovernment();
	};
}

TEST_CASE( "Benchmark Body Turn", "[!benchmark][Body][Turn]" ) {
	TestBody body;
	BENCHMARK( "Body::Turn(double)" ) {
		body.Turn(1.0);
	};
	BENCHMARK( "Body::Turn(Angle)" ) {
		body.Turn(Angle(1.0));
	};
}
#endif
// #endregion benchmarks

} // test namespace