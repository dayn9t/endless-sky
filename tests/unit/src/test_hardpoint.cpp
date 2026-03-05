/* test_hardpoint.cpp
Copyright (c) 2026 by jiang

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
#include "../../../source/Hardpoint.h"

// ... and any system includes needed for the test file.
#include "../../../source/Angle.h"
#include "../../../source/Point.h"

namespace { // test namespace

// #region mock data

// Helper function to create default BaseAttributes for testing.
Hardpoint::BaseAttributes makeDefaultAttributes()
{
	Hardpoint::BaseAttributes attrs;
	attrs.baseAngle = Angle();
	attrs.isParallel = false;
	attrs.isOmnidirectional = true;
	attrs.side = Hardpoint::Side::OVER;
	attrs.minArc = Angle(-180.);
	attrs.maxArc = Angle(180.);
	attrs.turnMultiplier = 0.;
	return attrs;
}

// Helper function to create custom BaseAttributes.
Hardpoint::BaseAttributes makeAttributes(Angle baseAngle, bool isParallel = false,
	bool isOmnidirectional = true, Hardpoint::Side side = Hardpoint::Side::OVER,
	double turnMultiplier = 0.)
{
	Hardpoint::BaseAttributes attrs;
	attrs.baseAngle = baseAngle;
	attrs.isParallel = isParallel;
	attrs.isOmnidirectional = isOmnidirectional;
	attrs.side = side;
	attrs.minArc = Angle(-180.);
	attrs.maxArc = Angle(180.);
	attrs.turnMultiplier = turnMultiplier;
	return attrs;
}

// #endregion mock data



// #region unit tests
SCENARIO( "Creating a Hardpoint instance", "[hardpoint][constructor]" ) {
	GIVEN( "default attributes and a point" ) {
		auto attrs = makeDefaultAttributes();
		Point point(100., 200.);

		WHEN( "a gun hardpoint is created without an outfit" ) {
			Hardpoint hp(point, attrs, false);

			THEN( "the point is scaled by 50%" ) {
				CHECK_THAT( hp.GetPoint().X(), Catch::Matchers::WithinAbs(50., 0.0001) );
				CHECK_THAT( hp.GetPoint().Y(), Catch::Matchers::WithinAbs(100., 0.0001) );
			}
			THEN( "no outfit is installed" ) {
				CHECK( hp.GetOutfit() == nullptr );
				CHECK( hp.GetWeapon() == nullptr );
			}
			THEN( "it is identified as a gun hardpoint" ) {
				CHECK_FALSE( hp.IsTurret() );
			}
			THEN( "default attributes are preserved" ) {
				CHECK_FALSE( hp.IsParallel() );
				CHECK( hp.IsOmnidirectional() );
				CHECK( hp.GetSide() == Hardpoint::Side::OVER );
			}
			THEN( "the angle is set to the base angle" ) {
				CHECK_THAT( hp.GetAngle().Degrees(), Catch::Matchers::WithinAbs(0., 0.0001) );
				CHECK_THAT( hp.GetIdleAngle().Degrees(), Catch::Matchers::WithinAbs(0., 0.0001) );
			}
		}

		WHEN( "a turret hardpoint is created without an outfit" ) {
			Hardpoint hp(point, attrs, true);

			THEN( "the point is scaled by 50%" ) {
				CHECK_THAT( hp.GetPoint().X(), Catch::Matchers::WithinAbs(50., 0.0001) );
				CHECK_THAT( hp.GetPoint().Y(), Catch::Matchers::WithinAbs(100., 0.0001) );
			}
			THEN( "no outfit is installed" ) {
				CHECK( hp.GetOutfit() == nullptr );
				CHECK( hp.GetWeapon() == nullptr );
			}
			THEN( "it is identified as a turret hardpoint" ) {
				CHECK( hp.IsTurret() );
			}
		}

		WHEN( "a hardpoint is created at the origin" ) {
			Point origin(0., 0.);
			Hardpoint hp(origin, attrs, false);

			THEN( "the point remains at origin" ) {
				CHECK_THAT( hp.GetPoint().X(), Catch::Matchers::WithinAbs(0., 0.0001) );
				CHECK_THAT( hp.GetPoint().Y(), Catch::Matchers::WithinAbs(0., 0.0001) );
			}
		}
	}

	GIVEN( "attributes with a non-zero base angle" ) {
		auto attrs = makeAttributes(Angle(45.));
		Point point(0., 0.);

		WHEN( "a hardpoint is created" ) {
			Hardpoint hp(point, attrs, false);

			THEN( "the idle angle matches the base angle" ) {
				CHECK_THAT( hp.GetIdleAngle().Degrees(), Catch::Matchers::WithinAbs(45., 0.05) );
			}
		}
	}

	GIVEN( "attributes with parallel flag set" ) {
		auto attrs = makeAttributes(Angle(), true);
		Point point(0., 0.);

		WHEN( "a hardpoint is created" ) {
			Hardpoint hp(point, attrs, false);

			THEN( "the hardpoint is marked as parallel" ) {
				CHECK( hp.IsParallel() );
			}
		}
	}

	GIVEN( "attributes with directional turret settings" ) {
		Hardpoint::BaseAttributes attrs;
		attrs.baseAngle = Angle(0.);
		attrs.isParallel = false;
		attrs.isOmnidirectional = false;
		attrs.side = Hardpoint::Side::UNDER;
		attrs.minArc = Angle(-90.);
		attrs.maxArc = Angle(90.);
		attrs.turnMultiplier = 0.;
		Point point(0., 0.);

		WHEN( "a turret hardpoint is created" ) {
			Hardpoint hp(point, attrs, true);

			THEN( "the hardpoint is not omnidirectional" ) {
				CHECK_FALSE( hp.IsOmnidirectional() );
			}
			THEN( "the side is set to UNDER" ) {
				CHECK( hp.GetSide() == Hardpoint::Side::UNDER );
			}
			THEN( "the arc limits are set" ) {
				// Arc values are set but may be modified by UpdateArc
				CHECK_THAT( hp.GetMinArc().Degrees(), Catch::Matchers::WithinAbs(-90., 1.) );
				CHECK_THAT( hp.GetMaxArc().Degrees(), Catch::Matchers::WithinAbs(90., 1.) );
			}
		}
	}

	GIVEN( "different side values" ) {
		Point point(0., 0.);

		WHEN( "side is set to OVER" ) {
			auto attrs = makeAttributes(Angle(), false, true, Hardpoint::Side::OVER);
			Hardpoint hp(point, attrs, false);

			THEN( "GetSide returns OVER" ) {
				CHECK( hp.GetSide() == Hardpoint::Side::OVER );
			}
		}

		WHEN( "side is set to INSIDE" ) {
			auto attrs = makeAttributes(Angle(), false, true, Hardpoint::Side::INSIDE);
			Hardpoint hp(point, attrs, false);

			THEN( "GetSide returns INSIDE" ) {
				CHECK( hp.GetSide() == Hardpoint::Side::INSIDE );
			}
		}

		WHEN( "side is set to UNDER" ) {
			auto attrs = makeAttributes(Angle(), false, true, Hardpoint::Side::UNDER);
			Hardpoint hp(point, attrs, false);

			THEN( "GetSide returns UNDER" ) {
				CHECK( hp.GetSide() == Hardpoint::Side::UNDER );
			}
		}
	}
}

SCENARIO( "Getting hardpoint attributes", "[hardpoint][accessors]" ) {
	GIVEN( "a hardpoint with specific attributes" ) {
		auto attrs = makeAttributes(Angle(30.), true, false, Hardpoint::Side::INSIDE, 0.5);
		Point point(200., 400.);
		Hardpoint hp(point, attrs, true);

		WHEN( "querying base attributes" ) {
			const auto& baseAttrs = hp.GetBaseAttributes();

			THEN( "the base attributes are returned correctly" ) {
				CHECK_THAT( baseAttrs.baseAngle.Degrees(), Catch::Matchers::WithinAbs(30., 0.05) );
				CHECK( baseAttrs.isParallel == true );
				CHECK( baseAttrs.isOmnidirectional == false );
				CHECK( baseAttrs.side == Hardpoint::Side::INSIDE );
				CHECK_THAT( baseAttrs.turnMultiplier, Catch::Matchers::WithinAbs(0.5, 0.0001) );
			}
		}

		WHEN( "querying if homing" ) {
			THEN( "returns false without an outfit" ) {
				CHECK_FALSE( hp.IsHoming() );
			}
		}

		WHEN( "querying if special" ) {
			THEN( "returns false without an outfit" ) {
				CHECK_FALSE( hp.IsSpecial() );
			}
		}

		WHEN( "querying if ready" ) {
			THEN( "returns false without an outfit" ) {
				CHECK_FALSE( hp.IsReady() );
			}
		}

		WHEN( "querying burst remaining" ) {
			THEN( "returns 0 without an outfit" ) {
				CHECK( hp.BurstRemaining() == 0 );
			}
		}
	}
}

SCENARIO( "Uninstalling an outfit from a hardpoint", "[hardpoint][uninstall]" ) {
	GIVEN( "an empty hardpoint" ) {
		auto attrs = makeDefaultAttributes();
		Point point(100., 100.);
		Hardpoint hp(point, attrs, false);

		WHEN( "uninstall is called on an empty hardpoint" ) {
			hp.Uninstall();

			THEN( "the hardpoint remains empty" ) {
				CHECK( hp.GetOutfit() == nullptr );
				CHECK( hp.GetWeapon() == nullptr );
			}
		}
	}
}

SCENARIO( "Reloading a hardpoint", "[hardpoint][reload]" ) {
	GIVEN( "an empty hardpoint" ) {
		auto attrs = makeDefaultAttributes();
		Point point(100., 100.);
		Hardpoint hp(point, attrs, false);

		WHEN( "reload is called" ) {
			hp.Reload();

			THEN( "the hardpoint remains empty and no exception is thrown" ) {
				CHECK( hp.GetOutfit() == nullptr );
				CHECK_FALSE( hp.IsReady() );
				CHECK( hp.BurstRemaining() == 0 );
			}
		}
	}
}

SCENARIO( "Stepping a hardpoint", "[hardpoint][step]" ) {
	GIVEN( "an empty hardpoint" ) {
		auto attrs = makeDefaultAttributes();
		Point point(100., 100.);
		Hardpoint hp(point, attrs, false);

		WHEN( "step is called multiple times" ) {
			for(int i = 0; i < 10; ++i)
				hp.Step();

			THEN( "no exception is thrown and hardpoint remains valid" ) {
				CHECK( hp.GetOutfit() == nullptr );
				CHECK_FALSE( hp.IsReady() );
				CHECK_FALSE( hp.WasFiring() );
			}
		}
	}
}

SCENARIO( "Checking blind status", "[hardpoint][blind]" ) {
	GIVEN( "an empty hardpoint with no blindspots" ) {
		auto attrs = makeDefaultAttributes();
		Point point(100., 100.);
		Hardpoint hp(point, attrs, false);

		WHEN( "checking if blind" ) {
			THEN( "returns false without blindspots" ) {
				CHECK_FALSE( hp.IsBlind() );
			}
		}
	}

	GIVEN( "an empty hardpoint with blindspots" ) {
		Hardpoint::BaseAttributes attrs;
		attrs.baseAngle = Angle(0.);
		attrs.blindspots.emplace_back(Angle(-10.), Angle(10.));
		Point point(100., 100.);
		Hardpoint hp(point, attrs, false);

		WHEN( "checking if blind at default angle" ) {
			THEN( "the blindspot is configured" ) {
				// Without an outfit, the angle is set to baseAngle
				// The blindspot check depends on the current angle
				CHECK_FALSE( hp.IsBlind() );
			}
		}
	}
}

SCENARIO( "WasFiring state", "[hardpoint][wasfiring]" ) {
	GIVEN( "a new hardpoint" ) {
		auto attrs = makeDefaultAttributes();
		Point point(100., 100.);
		Hardpoint hp(point, attrs, false);

		WHEN( "checking WasFiring initially" ) {
			THEN( "returns false" ) {
				CHECK_FALSE( hp.WasFiring() );
			}
		}

		WHEN( "step is called" ) {
			hp.Step();

			THEN( "WasFiring still returns false" ) {
				CHECK_FALSE( hp.WasFiring() );
			}
		}
	}
}

SCENARIO( "Hardpoint copy semantics", "[hardpoint][copy]" ) {
	GIVEN( "a hardpoint with specific configuration" ) {
		auto attrs = makeAttributes(Angle(45.), true, false, Hardpoint::Side::INSIDE);
		Point point(100., 200.);
		Hardpoint original(point, attrs, true);

		WHEN( "the hardpoint is copied via copy constructor" ) {
			Hardpoint copy(original);

			THEN( "the copy has the same point" ) {
				CHECK_THAT( copy.GetPoint().X(), Catch::Matchers::WithinAbs(original.GetPoint().X(), 0.0001) );
				CHECK_THAT( copy.GetPoint().Y(), Catch::Matchers::WithinAbs(original.GetPoint().Y(), 0.0001) );
			}
			THEN( "the copy has the same turret status" ) {
				CHECK( copy.IsTurret() == original.IsTurret() );
			}
			THEN( "the copy has the same angle" ) {
				CHECK_THAT( copy.GetAngle().Degrees(), Catch::Matchers::WithinAbs(original.GetAngle().Degrees(), 0.05) );
			}
			THEN( "the copy has the same base attributes" ) {
				const auto& origAttrs = original.GetBaseAttributes();
				const auto& copyAttrs = copy.GetBaseAttributes();
				CHECK_THAT( origAttrs.baseAngle.Degrees(), Catch::Matchers::WithinAbs(copyAttrs.baseAngle.Degrees(), 0.05) );
				CHECK( origAttrs.isParallel == copyAttrs.isParallel );
				CHECK( origAttrs.isOmnidirectional == copyAttrs.isOmnidirectional );
				CHECK( origAttrs.side == copyAttrs.side );
			}
		}
	}
}

SCENARIO( "Hardpoint with negative coordinates", "[hardpoint][negative]" ) {
	GIVEN( "a point with negative coordinates" ) {
		auto attrs = makeDefaultAttributes();
		Point point(-100., -200.);

		WHEN( "a hardpoint is created" ) {
			Hardpoint hp(point, attrs, false);

			THEN( "the point is scaled correctly" ) {
				CHECK_THAT( hp.GetPoint().X(), Catch::Matchers::WithinAbs(-50., 0.0001) );
				CHECK_THAT( hp.GetPoint().Y(), Catch::Matchers::WithinAbs(-100., 0.0001) );
			}
		}
	}
}

SCENARIO( "Hardpoint with large coordinates", "[hardpoint][large]" ) {
	GIVEN( "a point with large coordinates" ) {
		auto attrs = makeDefaultAttributes();
		Point point(10000., 20000.);

		WHEN( "a hardpoint is created" ) {
			Hardpoint hp(point, attrs, false);

			THEN( "the point is scaled correctly" ) {
				CHECK_THAT( hp.GetPoint().X(), Catch::Matchers::WithinAbs(5000., 0.0001) );
				CHECK_THAT( hp.GetPoint().Y(), Catch::Matchers::WithinAbs(10000., 0.0001) );
			}
		}
	}
}

SCENARIO( "Hardpoint with fractional coordinates", "[hardpoint][fractional]" ) {
	GIVEN( "a point with fractional coordinates" ) {
		auto attrs = makeDefaultAttributes();
		Point point(33.5, 67.25);

		WHEN( "a hardpoint is created" ) {
			Hardpoint hp(point, attrs, false);

			THEN( "the point is scaled correctly" ) {
				CHECK_THAT( hp.GetPoint().X(), Catch::Matchers::WithinAbs(16.75, 0.0001) );
				CHECK_THAT( hp.GetPoint().Y(), Catch::Matchers::WithinAbs(33.625, 0.0001) );
			}
		}
	}
}

SCENARIO( "Hardpoint IsTurret classification", "[hardpoint][isturret]" ) {
	GIVEN( "default attributes" ) {
		auto attrs = makeDefaultAttributes();
		Point point(0., 0.);

		WHEN( "creating a gun hardpoint" ) {
			Hardpoint hp(point, attrs, false);

			THEN( "IsTurret returns false" ) {
				CHECK_FALSE( hp.IsTurret() );
			}
		}

		WHEN( "creating a turret hardpoint" ) {
			Hardpoint hp(point, attrs, true);

			THEN( "IsTurret returns true" ) {
				CHECK( hp.IsTurret() );
			}
		}
	}
}

SCENARIO( "Hardpoint IsParallel flag", "[hardpoint][isparallel]" ) {
	GIVEN( "attributes with different isParallel values" ) {
		Point point(0., 0.);

		WHEN( "isParallel is false" ) {
			auto attrs = makeAttributes(Angle(), false);
			Hardpoint hp(point, attrs, false);

			THEN( "IsParallel returns false" ) {
				CHECK_FALSE( hp.IsParallel() );
			}
		}

		WHEN( "isParallel is true" ) {
			auto attrs = makeAttributes(Angle(), true);
			Hardpoint hp(point, attrs, false);

			THEN( "IsParallel returns true" ) {
				CHECK( hp.IsParallel() );
			}
		}
	}
}

SCENARIO( "Hardpoint IsOmnidirectional flag", "[hardpoint][isomnidirectional]" ) {
	GIVEN( "attributes with different isOmnidirectional values" ) {
		Point point(0., 0.);

		WHEN( "isOmnidirectional is true" ) {
			auto attrs = makeAttributes(Angle(), false, true);
			Hardpoint hp(point, attrs, true);

			THEN( "IsOmnidirectional returns true" ) {
				CHECK( hp.IsOmnidirectional() );
			}
		}

		WHEN( "isOmnidirectional is false" ) {
			Hardpoint::BaseAttributes attrs;
			attrs.baseAngle = Angle(0.);
			attrs.isParallel = false;
			attrs.isOmnidirectional = false;
			attrs.minArc = Angle(-45.);
			attrs.maxArc = Angle(45.);
			Hardpoint hp(point, attrs, true);

			THEN( "IsOmnidirectional returns false" ) {
				CHECK_FALSE( hp.IsOmnidirectional() );
			}
		}
	}
}

SCENARIO( "Hardpoint GetAngle and GetIdleAngle", "[hardpoint][angle]" ) {
	GIVEN( "attributes with various base angles" ) {
		Point point(0., 0.);

		WHEN( "base angle is 0 degrees" ) {
			auto attrs = makeAttributes(Angle(0.));
			Hardpoint hp(point, attrs, false);

			THEN( "both angles are near 0" ) {
				CHECK_THAT( hp.GetAngle().Degrees(), Catch::Matchers::WithinAbs(0., 0.05) );
				CHECK_THAT( hp.GetIdleAngle().Degrees(), Catch::Matchers::WithinAbs(0., 0.05) );
			}
		}

		WHEN( "base angle is 90 degrees" ) {
			auto attrs = makeAttributes(Angle(90.));
			Hardpoint hp(point, attrs, false);

			THEN( "idle angle is 90 degrees" ) {
				CHECK_THAT( hp.GetIdleAngle().Degrees(), Catch::Matchers::WithinAbs(90., 0.05) );
			}
		}

		WHEN( "base angle is -45 degrees" ) {
			auto attrs = makeAttributes(Angle(-45.));
			Hardpoint hp(point, attrs, false);

			THEN( "idle angle is -45 degrees" ) {
				CHECK_THAT( hp.GetIdleAngle().Degrees(), Catch::Matchers::WithinAbs(-45., 0.05) );
			}
		}

		WHEN( "base angle is 180 degrees" ) {
			auto attrs = makeAttributes(Angle(180.));
			Hardpoint hp(point, attrs, false);

			THEN( "idle angle is near +/-180 degrees" ) {
				CHECK_THAT( std::abs(hp.GetIdleAngle().Degrees()), Catch::Matchers::WithinAbs(180., 0.05) );
			}
		}
	}
}

SCENARIO( "Hardpoint GetMinArc and GetMaxArc for omnidirectional turret", "[hardpoint][arc]" ) {
	GIVEN( "an omnidirectional turret hardpoint" ) {
		auto attrs = makeDefaultAttributes();
		Point point(0., 0.);
		Hardpoint hp(point, attrs, true);

		WHEN( "querying arc limits" ) {
			THEN( "the arcs span the full circle" ) {
				// For omnidirectional turrets, min and max arc are opposite points
				CHECK_THAT( hp.GetMinArc().Degrees(), Catch::Matchers::WithinAbs(180., 0.05) );
				CHECK_THAT( hp.GetMaxArc().Degrees(), Catch::Matchers::WithinAbs(180., 0.05) );
			}
		}
	}
}

// Note: Testing Install() with actual Outfit objects requires GameData infrastructure.
// The following tests would require mocking Outfit and Weapon classes.
// These are placeholder scenarios documenting expected behavior:

/*
SCENARIO( "Installing an outfit in a hardpoint", "[hardpoint][install]" ) {
	// Requires: Mock Outfit with Weapon
	// Tests: Install(), GetOutfit(), GetWeapon(), IsReady(), BurstRemaining()
}

SCENARIO( "Firing a weapon from a hardpoint", "[hardpoint][fire]" ) {
	// Requires: Mock Ship, Outfit, Weapon
	// Tests: Fire(), IsReady(), BurstRemaining(), WasFiring()
}

SCENARIO( "Aiming a turret", "[hardpoint][aim]" ) {
	// Requires: Mock Ship with attributes
	// Tests: Aim(), GetAngle(), TurnRate(), CanAim()
}

SCENARIO( "Harmonized angle calculation", "[hardpoint][harmonize]" ) {
	// Requires: Mock Outfit with Weapon that has Range()
	// Tests: HarmonizedAngle()
}
*/

// #endregion unit tests



} // test namespace