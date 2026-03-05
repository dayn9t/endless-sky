/* test_camera.cpp
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
#include "../../../source/Camera.h"

// ... and any system includes needed for the test file.
#include "../../../source/Point.h"
#include "../../../source/Preferences.h"

#include <cmath>

namespace { // test namespace

// #region mock data

// Helper function to check if two points are approximately equal
bool ApproxEqual(const Point &a, const Point &b, double epsilon = 0.0001)
{
	return std::abs(a.X() - b.X()) < epsilon && std::abs(a.Y() - b.Y()) < epsilon;
}

// #endregion mock data



// #region unit tests
TEST_CASE( "Camera Basics", "[Camera]" ) {
	using T = Camera;
	SECTION( "Class Traits" ) {
		CHECK( std::is_default_constructible_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
		CHECK( std::is_trivially_destructible_v<T> );
	}
	SECTION( "Construction Traits" ) {
		CHECK( std::is_default_constructible_v<T> );
		CHECK( std::is_copy_constructible_v<T> );
		CHECK( std::is_move_constructible_v<T> );
	}
	SECTION( "Copy Traits" ) {
		CHECK( std::is_copy_assignable_v<T> );
		CHECK( std::is_move_assignable_v<T> );
	}
}

SCENARIO( "Camera default construction", "[Camera][Construction]" ) {
	GIVEN( "a default constructed Camera" ) {
		Camera camera;

		WHEN( "checking initial state" ) {
			THEN( "center is at origin" ) {
				CHECK( camera.Center().X() == 0. );
				CHECK( camera.Center().Y() == 0. );
			}
			AND_THEN( "velocity is zero" ) {
				CHECK( camera.Velocity().X() == 0. );
				CHECK( camera.Velocity().Y() == 0. );
			}
		}
	}
}

SCENARIO( "Camera SnapTo operation", "[Camera][SnapTo]" ) {
	GIVEN( "a default constructed Camera" ) {
		Camera camera;

		WHEN( "snapping to a target without keeping velocity" ) {
			Point target(100., 200.);
			camera.SnapTo(target, false);

			THEN( "camera center is at the target" ) {
				CHECK( camera.Center().X() == 100. );
				CHECK( camera.Center().Y() == 200. );
			}
			AND_THEN( "velocity is zero" ) {
				CHECK( camera.Velocity().X() == 0. );
				CHECK( camera.Velocity().Y() == 0. );
			}
		}

		WHEN( "snapping to a target with default keepVelocity (false)" ) {
			Point target(150., 250.);
			camera.SnapTo(target);

			THEN( "camera center is at the target" ) {
				CHECK( camera.Center().X() == 150. );
				CHECK( camera.Center().Y() == 250. );
			}
			AND_THEN( "velocity is zero" ) {
				CHECK( camera.Velocity().X() == 0. );
				CHECK( camera.Velocity().Y() == 0. );
			}
		}

		WHEN( "snapping to origin" ) {
			Point target(0., 0.);
			camera.SnapTo(target);

			THEN( "camera center is at origin" ) {
				CHECK( camera.Center().X() == 0. );
				CHECK( camera.Center().Y() == 0. );
			}
		}

		WHEN( "snapping to negative coordinates" ) {
			Point target(-100., -200.);
			camera.SnapTo(target);

			THEN( "camera center is at negative coordinates" ) {
				CHECK( camera.Center().X() == -100. );
				CHECK( camera.Center().Y() == -200. );
			}
		}
	}

	GIVEN( "a Camera that has been moved" ) {
		Camera camera;
		Point initialTarget(100., 100.);
		camera.SnapTo(initialTarget);

		// Simulate movement by calling MoveTo
		Point moveTarget(200., 200.);
		camera.MoveTo(moveTarget, 0.);

		WHEN( "snapping to a new target without keeping velocity" ) {
			Point newTarget(300., 400.);
			camera.SnapTo(newTarget, false);

			THEN( "camera center is at the new target" ) {
				CHECK( camera.Center().X() == 300. );
				CHECK( camera.Center().Y() == 400. );
			}
			AND_THEN( "velocity is reset to zero" ) {
				CHECK( camera.Velocity().X() == 0. );
				CHECK( camera.Velocity().Y() == 0. );
			}
		}

		WHEN( "snapping to a new target with keepVelocity = true" ) {
			// Get current velocity
			Point currentVelocity = camera.Velocity();
			Point newTarget(300., 400.);
			camera.SnapTo(newTarget, true);

			THEN( "camera center is offset by velocity" ) {
				Point expectedCenter = newTarget - currentVelocity;
				CHECK_THAT( camera.Center().X(), Catch::Matchers::WithinAbs(expectedCenter.X(), 0.0001) );
				CHECK_THAT( camera.Center().Y(), Catch::Matchers::WithinAbs(expectedCenter.Y(), 0.0001) );
			}
			AND_THEN( "velocity is zero (will be recalculated on next MoveTo)" ) {
				CHECK( camera.Velocity().X() == 0. );
				CHECK( camera.Velocity().Y() == 0. );
			}
		}
	}
}

SCENARIO( "Camera MoveTo operation with acceleration OFF", "[Camera][MoveTo]" ) {
	GIVEN( "a default constructed Camera with acceleration OFF" ) {
		Camera camera;
		// Default Preferences::CameraAcceleration() is OFF

		WHEN( "moving to a target for the first time" ) {
			Point target(100., 200.);
			camera.MoveTo(target, 0.);

			THEN( "camera center is at the target" ) {
				CHECK( camera.Center().X() == 100. );
				CHECK( camera.Center().Y() == 200. );
			}
			AND_THEN( "velocity equals target minus oldTarget (which was origin)" ) {
				CHECK( camera.Velocity().X() == 100. );
				CHECK( camera.Velocity().Y() == 200. );
			}
		}

		WHEN( "moving to multiple targets sequentially" ) {
			Point target1(100., 200.);
			camera.MoveTo(target1, 0.);

			Point target2(150., 250.);
			camera.MoveTo(target2, 0.);

			THEN( "camera center follows the target" ) {
				CHECK( camera.Center().X() == 150. );
				CHECK( camera.Center().Y() == 250. );
			}
			AND_THEN( "velocity is the delta between targets" ) {
				CHECK( camera.Velocity().X() == 50. );
				CHECK( camera.Velocity().Y() == 50. );
			}
		}

		WHEN( "moving to the same target twice" ) {
			Point target(100., 200.);
			camera.MoveTo(target, 0.);
			camera.MoveTo(target, 0.);

			THEN( "velocity is zero" ) {
				CHECK( camera.Velocity().X() == 0. );
				CHECK( camera.Velocity().Y() == 0. );
			}
		}

		WHEN( "moving with hyperspace influence" ) {
			Point target(100., 200.);
			camera.MoveTo(target, 0.5);

			THEN( "camera center is at the target" ) {
				CHECK( camera.Center().X() == 100. );
				CHECK( camera.Center().Y() == 200. );
			}
		}
	}
}

SCENARIO( "Camera Center and Velocity accessors", "[Camera][Accessors]" ) {
	GIVEN( "a Camera at a known position" ) {
		Camera camera;
		Point target(500., -300.);
		camera.SnapTo(target);

		WHEN( "reading Center" ) {
			const Point &center = camera.Center();

			THEN( "center matches the snap target" ) {
				CHECK( center.X() == 500. );
				CHECK( center.Y() == -300. );
			}
		}

		WHEN( "reading Velocity after snap" ) {
			const Point &velocity = camera.Velocity();

			THEN( "velocity is zero" ) {
				CHECK( velocity.X() == 0. );
				CHECK( velocity.Y() == 0. );
			}
		}

		WHEN( "reading Velocity after move" ) {
			Point newTarget(600., -200.);
			camera.MoveTo(newTarget, 0.);
			const Point &velocity = camera.Velocity();

			THEN( "velocity is the delta" ) {
				CHECK( velocity.X() == 100. );
				CHECK( velocity.Y() == 100. );
			}
		}
	}
}

SCENARIO( "Camera with large coordinates", "[Camera][Edge]" ) {
	GIVEN( "a default constructed Camera" ) {
		Camera camera;

		WHEN( "snapping to very large coordinates" ) {
			Point target(1e10, 2e10);
			camera.SnapTo(target);

			THEN( "camera handles large values correctly" ) {
				CHECK_THAT( camera.Center().X(), Catch::Matchers::WithinAbs(1e10, 1e5) );
				CHECK_THAT( camera.Center().Y(), Catch::Matchers::WithinAbs(2e10, 1e5) );
			}
		}

		WHEN( "snapping to very small coordinates" ) {
			Point target(1e-10, 2e-10);
			camera.SnapTo(target);

			THEN( "camera handles small values correctly" ) {
				CHECK_THAT( camera.Center().X(), Catch::Matchers::WithinAbs(1e-10, 1e-15) );
				CHECK_THAT( camera.Center().Y(), Catch::Matchers::WithinAbs(2e-10, 1e-15) );
			}
		}

		WHEN( "moving to large coordinates" ) {
			Point target(1e10, 2e10);
			camera.MoveTo(target, 0.);

			THEN( "camera handles large values correctly" ) {
				CHECK_THAT( camera.Center().X(), Catch::Matchers::WithinAbs(1e10, 1e5) );
				CHECK_THAT( camera.Center().Y(), Catch::Matchers::WithinAbs(2e10, 1e5) );
			}
		}
	}
}

SCENARIO( "Camera with negative coordinates", "[Camera][Negative]" ) {
	GIVEN( "a default constructed Camera" ) {
		Camera camera;

		WHEN( "snapping to negative coordinates" ) {
			Point target(-100., -200.);
			camera.SnapTo(target);

			THEN( "camera center is at negative coordinates" ) {
				CHECK( camera.Center().X() == -100. );
				CHECK( camera.Center().Y() == -200. );
			}
		}

		WHEN( "moving to negative coordinates" ) {
			Point target(-100., -200.);
			camera.MoveTo(target, 0.);

			THEN( "camera center is at negative coordinates" ) {
				CHECK( camera.Center().X() == -100. );
				CHECK( camera.Center().Y() == -200. );
			}
			AND_THEN( "velocity is negative" ) {
				CHECK( camera.Velocity().X() == -100. );
				CHECK( camera.Velocity().Y() == -200. );
			}
		}

		WHEN( "moving from negative to positive" ) {
			camera.SnapTo(Point(-100., -100.));
			Point target(100., 100.);
			camera.MoveTo(target, 0.);

			THEN( "velocity is positive" ) {
				CHECK( camera.Velocity().X() == 200. );
				CHECK( camera.Velocity().Y() == 200. );
			}
		}

		WHEN( "moving from positive to negative" ) {
			camera.SnapTo(Point(100., 100.));
			Point target(-100., -100.);
			camera.MoveTo(target, 0.);

			THEN( "velocity is negative" ) {
				CHECK( camera.Velocity().X() == -200. );
				CHECK( camera.Velocity().Y() == -200. );
			}
		}
	}
}

SCENARIO( "Camera with mixed sign coordinates", "[Camera][MixedSign]" ) {
	GIVEN( "a default constructed Camera" ) {
		Camera camera;

		WHEN( "snapping to mixed sign coordinates (positive X, negative Y)" ) {
			Point target(100., -200.);
			camera.SnapTo(target);

			THEN( "camera center has correct signs" ) {
				CHECK( camera.Center().X() == 100. );
				CHECK( camera.Center().Y() == -200. );
			}
		}

		WHEN( "snapping to mixed sign coordinates (negative X, positive Y)" ) {
			Point target(-100., 200.);
			camera.SnapTo(target);

			THEN( "camera center has correct signs" ) {
				CHECK( camera.Center().X() == -100. );
				CHECK( camera.Center().Y() == 200. );
			}
		}
	}
}

SCENARIO( "Camera copy and assignment", "[Camera][Copy]" ) {
	GIVEN( "a Camera at a specific position" ) {
		Camera original;
		original.SnapTo(Point(100., 200.));
		original.MoveTo(Point(150., 250.), 0.);

		WHEN( "copy constructing a new Camera" ) {
			Camera copy(original);

			THEN( "copy has the same center" ) {
				CHECK( copy.Center().X() == original.Center().X() );
				CHECK( copy.Center().Y() == original.Center().Y() );
			}
			AND_THEN( "copy has the same velocity" ) {
				CHECK( copy.Velocity().X() == original.Velocity().X() );
				CHECK( copy.Velocity().Y() == original.Velocity().Y() );
			}
		}

		WHEN( "copy assigning a Camera" ) {
			Camera assigned;
			assigned.SnapTo(Point(0., 0.));
			assigned = original;

			THEN( "assigned has the same center" ) {
				CHECK( assigned.Center().X() == original.Center().X() );
				CHECK( assigned.Center().Y() == original.Center().Y() );
			}
			AND_THEN( "assigned has the same velocity" ) {
				CHECK( assigned.Velocity().X() == original.Velocity().X() );
				CHECK( assigned.Velocity().Y() == original.Velocity().Y() );
			}
		}
	}
}

SCENARIO( "Camera hyperspace influence", "[Camera][Hyperspace]" ) {
	GIVEN( "a Camera with acceleration OFF" ) {
		Camera camera;

		WHEN( "moving with zero hyperspace influence" ) {
			Point target(100., 200.);
			camera.MoveTo(target, 0.);

			THEN( "camera center is at target" ) {
				CHECK( camera.Center().X() == 100. );
				CHECK( camera.Center().Y() == 200. );
			}
		}

		WHEN( "moving with partial hyperspace influence" ) {
			Point target(100., 200.);
			camera.MoveTo(target, 0.5);

			THEN( "camera center is at target (with acceleration OFF, hyperspace has no extra effect)" ) {
				CHECK( camera.Center().X() == 100. );
				CHECK( camera.Center().Y() == 200. );
			}
		}

		WHEN( "moving with full hyperspace influence" ) {
			Point target(100., 200.);
			camera.MoveTo(target, 1.0);

			THEN( "camera center is at target" ) {
				CHECK( camera.Center().X() == 100. );
				CHECK( camera.Center().Y() == 200. );
			}
		}
	}
}

SCENARIO( "Camera sequential operations", "[Camera][Sequence]" ) {
	GIVEN( "a default constructed Camera" ) {
		Camera camera;

		WHEN( "performing a series of snap and move operations" ) {
			// Snap to initial position
			camera.SnapTo(Point(0., 0.));
			REQUIRE( camera.Center().X() == 0. );
			REQUIRE( camera.Center().Y() == 0. );

			// Move to first target
			camera.MoveTo(Point(100., 100.), 0.);
			REQUIRE( camera.Center().X() == 100. );
			REQUIRE( camera.Center().Y() == 100. );

			// Move to second target
			camera.MoveTo(Point(200., 200.), 0.);
			REQUIRE( camera.Center().X() == 200. );
			REQUIRE( camera.Center().Y() == 200. );

			// Snap to new position
			camera.SnapTo(Point(500., 500.));

			THEN( "camera state is consistent after operations" ) {
				CHECK( camera.Center().X() == 500. );
				CHECK( camera.Center().Y() == 500. );
				CHECK( camera.Velocity().X() == 0. );
				CHECK( camera.Velocity().Y() == 0. );
			}
		}

		WHEN( "moving to oscillating positions" ) {
			// Move back and forth
			for(int i = 0; i < 5; ++i)
			{
				camera.MoveTo(Point(100., 100.), 0.);
				camera.MoveTo(Point(-100., -100.), 0.);
			}

			THEN( "camera handles oscillating movement" ) {
				CHECK( camera.Center().X() == -100. );
				CHECK( camera.Center().Y() == -100. );
				// Last move was from 100,100 to -100,-100, so velocity should be -200,-200
				CHECK( camera.Velocity().X() == -200. );
				CHECK( camera.Velocity().Y() == -200. );
			}
		}
	}
}

SCENARIO( "Camera velocity calculation", "[Camera][Velocity]" ) {
	GIVEN( "a Camera with acceleration OFF" ) {
		Camera camera;

		WHEN( "moving from origin to target" ) {
			Point target(100., 200.);
			camera.MoveTo(target, 0.);

			THEN( "velocity equals target position" ) {
				CHECK( camera.Velocity().X() == 100. );
				CHECK( camera.Velocity().Y() == 200. );
			}
		}

		WHEN( "moving between two positions" ) {
			camera.SnapTo(Point(100., 200.));
			Point newTarget(200., 400.);
			camera.MoveTo(newTarget, 0.);

			THEN( "velocity equals the difference" ) {
				CHECK( camera.Velocity().X() == 100. );
				CHECK( camera.Velocity().Y() == 200. );
			}
		}

		WHEN( "staying at the same position" ) {
			camera.SnapTo(Point(100., 100.));
			camera.MoveTo(Point(100., 100.), 0.);

			THEN( "velocity is zero" ) {
				CHECK( camera.Velocity().X() == 0. );
				CHECK( camera.Velocity().Y() == 0. );
			}
		}

		WHEN( "moving with zero delta" ) {
			camera.MoveTo(Point(0., 0.), 0.);
			camera.MoveTo(Point(0., 0.), 0.);

			THEN( "velocity is zero" ) {
				CHECK( camera.Velocity().X() == 0. );
				CHECK( camera.Velocity().Y() == 0. );
			}
		}
	}
}

SCENARIO( "Camera with diagonal movement", "[Camera][Diagonal]" ) {
	GIVEN( "a default constructed Camera" ) {
		Camera camera;

		WHEN( "moving diagonally at 45 degrees" ) {
			Point target(100., 100.);
			camera.MoveTo(target, 0.);

			THEN( "camera handles diagonal movement correctly" ) {
				CHECK( camera.Center().X() == 100. );
				CHECK( camera.Center().Y() == 100. );
				CHECK_THAT( camera.Velocity().Length(), Catch::Matchers::WithinAbs(std::sqrt(20000.), 0.01) );
			}
		}

		WHEN( "moving in a square pattern" ) {
			camera.MoveTo(Point(100., 0.), 0.);
			camera.MoveTo(Point(100., 100.), 0.);
			camera.MoveTo(Point(0., 100.), 0.);
			camera.MoveTo(Point(0., 0.), 0.);

			THEN( "camera returns to origin" ) {
				CHECK( camera.Center().X() == 0. );
				CHECK( camera.Center().Y() == 0. );
			}
		}
	}
}

SCENARIO( "Camera reference stability", "[Camera][Reference]" ) {
	GIVEN( "a Camera" ) {
		Camera camera;
		camera.SnapTo(Point(100., 200.));

		WHEN( "getting const references to Center and Velocity" ) {
			const Point &centerRef = camera.Center();
			const Point &velocityRef = camera.Velocity();

			THEN( "references are valid and can be read multiple times" ) {
				CHECK( centerRef.X() == 100. );
				CHECK( centerRef.X() == camera.Center().X() );
				CHECK( velocityRef.Y() == 0. );
				CHECK( velocityRef.Y() == camera.Velocity().Y() );
			}
		}
	}
}

SCENARIO( "Camera state after multiple operations", "[Camera][State]" ) {
	GIVEN( "a Camera with a complex history" ) {
		Camera camera;

		// Build up some state
		camera.SnapTo(Point(0., 0.));
		for(int i = 1; i <= 10; ++i)
			camera.MoveTo(Point(i * 10., i * 20.), 0.);

		WHEN( "checking final state" ) {
			THEN( "camera is at the last position" ) {
				CHECK( camera.Center().X() == 100. );
				CHECK( camera.Center().Y() == 200. );
			}
			AND_THEN( "velocity reflects the last move" ) {
				CHECK( camera.Velocity().X() == 10. );
				CHECK( camera.Velocity().Y() == 20. );
			}
		}

		WHEN( "resetting with SnapTo after complex history" ) {
			camera.SnapTo(Point(500., 600.));

			THEN( "camera is at the snap target" ) {
				CHECK( camera.Center().X() == 500. );
				CHECK( camera.Center().Y() == 600. );
			}
			AND_THEN( "velocity is reset to zero" ) {
				CHECK( camera.Velocity().X() == 0. );
				CHECK( camera.Velocity().Y() == 0. );
			}
		}
	}
}

SCENARIO( "Camera with extreme velocity changes", "[Camera][Extreme]" ) {
	GIVEN( "a Camera at rest" ) {
		Camera camera;
		camera.SnapTo(Point(0., 0.));

		WHEN( "moving to a very distant target" ) {
			Point target(10000., 20000.);
			camera.MoveTo(target, 0.);

			THEN( "camera handles large velocity" ) {
				CHECK( camera.Center().X() == 10000. );
				CHECK( camera.Center().Y() == 20000. );
			}
		}

		WHEN( "moving from distant to near" ) {
			camera.SnapTo(Point(10000., 10000.));
			Point nearTarget(10., 10.);
			camera.MoveTo(nearTarget, 0.);

			THEN( "camera handles the transition" ) {
				CHECK( camera.Center().X() == 10. );
				CHECK( camera.Center().Y() == 10. );
			}
		}
	}
}
// #endregion unit tests



} // test namespace