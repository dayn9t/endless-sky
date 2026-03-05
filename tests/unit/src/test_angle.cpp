/* test_angle.cpp
Copyright (c) 2021 by quyykk

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
#include "../../../source/Angle.h"

// ... and any system includes needed for the test file.
#include <cmath>

namespace { // test namespace

// #region mock data
// #endregion mock data



// #region unit tests
TEST_CASE( "Angle::Angle", "[angle]") {
	Angle defaultAngle;
	CHECK_THAT( defaultAngle.Degrees(), Catch::Matchers::WithinAbs(0., 0.0001) );
	Point defaultUnit = defaultAngle.Unit();
	CHECK_THAT( defaultUnit.X(), Catch::Matchers::WithinAbs(0., 0.0001) );
	CHECK_THAT( defaultUnit.Y(), Catch::Matchers::WithinAbs(-1., 0.0001) );
	CHECK_THAT( Angle(defaultUnit).Degrees(), Catch::Matchers::WithinAbs(defaultAngle.Degrees(), 0.0001) );

	Angle halfAngle = 180.;
	CHECK_THAT( halfAngle.Degrees(), Catch::Matchers::WithinAbs(-180., 0.0001) );
	Point halfUnit = halfAngle.Unit();
	CHECK_THAT( halfUnit.X(), Catch::Matchers::WithinAbs(0., 0.01) || Catch::Matchers::WithinRel(0.0001) );
	CHECK_THAT( halfUnit.Y(), Catch::Matchers::WithinAbs(1., 0.0001) );
	CHECK_THAT( Angle(halfUnit).Degrees(), Catch::Matchers::WithinAbs(halfAngle.Degrees(), 0.0001) );

	Angle fullAngle = 360.;
	CHECK_THAT( fullAngle.Degrees(), Catch::Matchers::WithinAbs(0., 0.0001) );
	Point fullUnit = fullAngle.Unit();
	CHECK_THAT( fullUnit.X(), Catch::Matchers::WithinAbs(0., 0.0001) );
	CHECK_THAT( fullUnit.Y(), Catch::Matchers::WithinAbs(-1., 0.0001) );
	CHECK_THAT( Angle(fullUnit).Degrees(), Catch::Matchers::WithinAbs(fullAngle.Degrees(), 0.0001) );
}

TEST_CASE( "Angle::Angle cardinal directions", "[angle][constructor]" ) {
	// Test 0 degrees (up/north)
	Angle up(0.);
	CHECK_THAT( up.Degrees(), Catch::Matchers::WithinAbs(0., 0.01) );
	Point upUnit = up.Unit();
	CHECK_THAT( upUnit.X(), Catch::Matchers::WithinAbs(0., 0.01) );
	CHECK_THAT( upUnit.Y(), Catch::Matchers::WithinAbs(-1., 0.01) );

	// Test 90 degrees (right/east)
	Angle right(90.);
	CHECK_THAT( right.Degrees(), Catch::Matchers::WithinAbs(90., 0.01) );
	Point rightUnit = right.Unit();
	CHECK_THAT( rightUnit.X(), Catch::Matchers::WithinAbs(1., 0.01) );
	CHECK_THAT( rightUnit.Y(), Catch::Matchers::WithinAbs(0., 0.01) );

	// Test 180 degrees (down/south)
	Angle down(180.);
	CHECK_THAT( down.Degrees(), Catch::Matchers::WithinAbs(-180., 0.01) );
	Point downUnit = down.Unit();
	CHECK_THAT( downUnit.X(), Catch::Matchers::WithinAbs(0., 0.01) );
	CHECK_THAT( downUnit.Y(), Catch::Matchers::WithinAbs(1., 0.01) );

	// Test 270 degrees (left/west)
	Angle left(270.);
	CHECK_THAT( left.Degrees(), Catch::Matchers::WithinAbs(-90., 0.01) );
	Point leftUnit = left.Unit();
	CHECK_THAT( leftUnit.X(), Catch::Matchers::WithinAbs(-1., 0.01) );
	CHECK_THAT( leftUnit.Y(), Catch::Matchers::WithinAbs(0., 0.01) );
}

TEST_CASE( "Angle::Angle wrapping", "[angle][constructor][wrapping]" ) {
	// Angles > 360 should wrap around
	Angle over360(450.);
	CHECK_THAT( over360.Degrees(), Catch::Matchers::WithinAbs(90., 0.01) );

	Angle over720(810.);
	CHECK_THAT( over720.Degrees(), Catch::Matchers::WithinAbs(90., 0.01) );

	// Angles < -360 should wrap around
	Angle under360(-450.);
	CHECK_THAT( under360.Degrees(), Catch::Matchers::WithinAbs(-90., 0.01) );

	Angle under720(-810.);
	CHECK_THAT( under720.Degrees(), Catch::Matchers::WithinAbs(-90., 0.01) );

	// Test boundary at 360
	Angle justOver360(360.5);
	CHECK_THAT( justOver360.Degrees(), Catch::Matchers::WithinAbs(0.5, 0.01) );

	Angle justUnder360(359.5);
	CHECK_THAT( justUnder360.Degrees(), Catch::Matchers::WithinAbs(-0.5, 0.01) );
}

TEST_CASE( "Angle::Angle from Point", "[angle][constructor][point]" ) {
	// Point pointing up (0 degrees)
	Angle fromUp(Point(0., -1.));
	CHECK_THAT( fromUp.Degrees(), Catch::Matchers::WithinAbs(0., 0.01) );

	// Point pointing right (90 degrees)
	Angle fromRight(Point(1., 0.));
	CHECK_THAT( fromRight.Degrees(), Catch::Matchers::WithinAbs(90., 0.01) );

	// Point pointing down (180 degrees)
	Angle fromDown(Point(0., 1.));
	CHECK_THAT( fromDown.Degrees(), Catch::Matchers::WithinAbs(-180., 0.01) );

	// Point pointing left (270 degrees / -90 degrees)
	Angle fromLeft(Point(-1., 0.));
	CHECK_THAT( fromLeft.Degrees(), Catch::Matchers::WithinAbs(-90., 0.01) );

	// Non-unit vectors should give same angles
	Angle fromScaled(Point(10., 0.));
	CHECK_THAT( fromScaled.Degrees(), Catch::Matchers::WithinAbs(90., 0.01) );

	Angle fromScaled2(Point(0., 100.));
	CHECK_THAT( fromScaled2.Degrees(), Catch::Matchers::WithinAbs(-180., 0.01) );

	// Diagonal direction (45 degrees)
	Angle fromDiagonal(Point(1., -1.));
	CHECK_THAT( fromDiagonal.Degrees(), Catch::Matchers::WithinAbs(45., 0.01) );
}

TEST_CASE( "Angle::AbsDegrees", "[angle][absdegrees]" ) {
	// Positive angles
	CHECK_THAT( Angle(0.).AbsDegrees(), Catch::Matchers::WithinAbs(0., 0.01) );
	CHECK_THAT( Angle(90.).AbsDegrees(), Catch::Matchers::WithinAbs(90., 0.01) );
	CHECK_THAT( Angle(180.).AbsDegrees(), Catch::Matchers::WithinAbs(180., 0.01) );
	CHECK_THAT( Angle(270.).AbsDegrees(), Catch::Matchers::WithinAbs(270., 0.01) );
	CHECK_THAT( Angle(359.).AbsDegrees(), Catch::Matchers::WithinAbs(359., 0.01) );

	// Negative angles should be in 0-360 range
	CHECK_THAT( Angle(-90.).AbsDegrees(), Catch::Matchers::WithinAbs(270., 0.01) );
	CHECK_THAT( Angle(-180.).AbsDegrees(), Catch::Matchers::WithinAbs(180., 0.01) );
	CHECK_THAT( Angle(-1.).AbsDegrees(), Catch::Matchers::WithinAbs(359., 0.01) );
	CHECK_THAT( Angle(-270.).AbsDegrees(), Catch::Matchers::WithinAbs(90., 0.01) );
	CHECK_THAT( Angle(-359.).AbsDegrees(), Catch::Matchers::WithinAbs(1., 0.01) );

	// Wrapped angles
	CHECK_THAT( Angle(450.).AbsDegrees(), Catch::Matchers::WithinAbs(90., 0.01) );
	CHECK_THAT( Angle(-450.).AbsDegrees(), Catch::Matchers::WithinAbs(270., 0.01) );
}

TEST_CASE( "Angle::Rotate", "[angle][rotate]" ) {
	Angle angle = 180.;
	REQUIRE_THAT( angle.Degrees(), Catch::Matchers::WithinAbs(-180., 0.0001) );

	auto rotate1 = angle.Rotate(Point(0., 1.));
	CHECK_THAT( rotate1.X(), Catch::Matchers::WithinAbs(0., 0.01) || Catch::Matchers::WithinRel(0.0001) );
	CHECK_THAT( rotate1.Y(), Catch::Matchers::WithinAbs(-1., 0.0001) );

	auto rotate2 = angle.Rotate(Point(1., -1.));
	CHECK_THAT( rotate2.X(), Catch::Matchers::WithinAbs(-1., 0.0001) );
	CHECK_THAT( rotate2.Y(), Catch::Matchers::WithinAbs(1., 0.0001) );
}

TEST_CASE( "Angle::Rotate cardinal directions", "[angle][rotate]" ) {
	Point testPoint(1., 0.);

	// Rotate by 0 degrees - should be identity
	Angle zeroRot(0.);
	Point rotated0 = zeroRot.Rotate(testPoint);
	CHECK_THAT( rotated0.X(), Catch::Matchers::WithinAbs(1., 0.01) );
	CHECK_THAT( rotated0.Y(), Catch::Matchers::WithinAbs(0., 0.01) );

	// Rotate by 90 degrees clockwise
	Angle rot90(90.);
	Point rotated90 = rot90.Rotate(testPoint);
	CHECK_THAT( rotated90.X(), Catch::Matchers::WithinAbs(0., 0.01) );
	CHECK_THAT( rotated90.Y(), Catch::Matchers::WithinAbs(1., 0.01) );

	// Rotate by 180 degrees
	Angle rot180(180.);
	Point rotated180 = rot180.Rotate(testPoint);
	CHECK_THAT( rotated180.X(), Catch::Matchers::WithinAbs(-1., 0.01) );
	CHECK_THAT( rotated180.Y(), Catch::Matchers::WithinAbs(0., 0.01) );

	// Rotate by 270 degrees (or -90 degrees)
	Angle rot270(270.);
	Point rotated270 = rot270.Rotate(testPoint);
	CHECK_THAT( rotated270.X(), Catch::Matchers::WithinAbs(0., 0.01) );
	CHECK_THAT( rotated270.Y(), Catch::Matchers::WithinAbs(-1., 0.01) );
}

TEST_CASE( "Angle::Rotate special cases", "[angle][rotate]" ) {
	// Rotating the origin should always give origin
	Angle anyAngle(123.45);
	Point origin(0., 0.);
	Point rotatedOrigin = anyAngle.Rotate(origin);
	CHECK_THAT( rotatedOrigin.X(), Catch::Matchers::WithinAbs(0., 0.0001) );
	CHECK_THAT( rotatedOrigin.Y(), Catch::Matchers::WithinAbs(0., 0.0001) );

	// Rotating negative coordinates
	Angle rot90(90.);
	Point negPoint(-1., -1.);
	Point rotatedNeg = rot90.Rotate(negPoint);
	CHECK_THAT( rotatedNeg.X(), Catch::Matchers::WithinAbs(-1., 0.01) );
	CHECK_THAT( rotatedNeg.Y(), Catch::Matchers::WithinAbs(1., 0.01) );

	// Full rotation should return to original
	Angle rot360(360.);
	Point testPoint(3., 4.);
	Point rotated360 = rot360.Rotate(testPoint);
	CHECK_THAT( rotated360.X(), Catch::Matchers::WithinAbs(3., 0.01) );
	CHECK_THAT( rotated360.Y(), Catch::Matchers::WithinAbs(4., 0.01) );
}

TEST_CASE( "Angle arithmetic", "[angle][arithmetic]") {
	Angle angle = 60.;
	REQUIRE_THAT( angle.Degrees(), Catch::Matchers::WithinAbs(60., 0.05) || Catch::Matchers::WithinRel(0.0001) );

	angle += 45.;
	REQUIRE_THAT( angle.Degrees(), Catch::Matchers::WithinAbs(105., 0.05) || Catch::Matchers::WithinRel(0.0001) );

	angle = angle + 100.;
	REQUIRE_THAT( angle.Degrees(), Catch::Matchers::WithinAbs(-155., 0.05) || Catch::Matchers::WithinRel(0.0001) );

	angle -= 50.;
	REQUIRE_THAT( angle.Degrees(), Catch::Matchers::WithinAbs(155., 0.05) || Catch::Matchers::WithinRel(0.0001) );

	angle = angle - 25.;
	REQUIRE_THAT( angle.Degrees(), Catch::Matchers::WithinAbs(130., 0.05) || Catch::Matchers::WithinRel(0.0001) );

	angle = -angle;
	REQUIRE_THAT( angle.Degrees(), Catch::Matchers::WithinAbs(-130., 0.05) || Catch::Matchers::WithinRel(0.0001) );
}

TEST_CASE( "Angle::operator plus", "[angle][arithmetic][plus]" ) {
	// Basic addition
	Angle a1(45.);
	Angle a2(45.);
	Angle sum = a1 + a2;
	CHECK_THAT( sum.Degrees(), Catch::Matchers::WithinAbs(90., 0.01) );

	// Addition with wrapping over 360
	Angle a3(300.);
	Angle a4(100.);
	Angle sumWrap = a3 + a4;
	CHECK_THAT( sumWrap.Degrees(), Catch::Matchers::WithinAbs(40., 0.01) );

	// Addition with negative angles
	Angle a5(-45.);
	Angle a6(90.);
	Angle sumNeg = a5 + a6;
	CHECK_THAT( sumNeg.Degrees(), Catch::Matchers::WithinAbs(45., 0.01) );

	// Multiple additions
	Angle a7(100.);
	a7 += Angle(50.);
	a7 += Angle(50.);
	a7 += Angle(50.);
	CHECK_THAT( a7.Degrees(), Catch::Matchers::WithinAbs(-110., 0.01) );
}

TEST_CASE( "Angle::operator minus", "[angle][arithmetic][minus]" ) {
	// Basic subtraction
	Angle a1(90.);
	Angle a2(45.);
	Angle diff = a1 - a2;
	CHECK_THAT( diff.Degrees(), Catch::Matchers::WithinAbs(45., 0.01) );

	// Subtraction resulting in negative
	Angle a3(45.);
	Angle a4(90.);
	Angle diffNeg = a3 - a4;
	CHECK_THAT( diffNeg.Degrees(), Catch::Matchers::WithinAbs(-45., 0.01) );

	// Subtraction with wrapping
	Angle a5(10.);
	Angle a6(350.);
	Angle diffWrap = a5 - a6;
	CHECK_THAT( diffWrap.Degrees(), Catch::Matchers::WithinAbs(20., 0.01) );

	// Multiple subtractions
	Angle a7(100.);
	a7 -= Angle(30.);
	a7 -= Angle(30.);
	a7 -= Angle(30.);
	CHECK_THAT( a7.Degrees(), Catch::Matchers::WithinAbs(10., 0.01) );
}

TEST_CASE( "Angle::operator negate", "[angle][arithmetic][negate]" ) {
	// Negate positive angle
	Angle a1(90.);
	Angle neg1 = -a1;
	CHECK_THAT( neg1.Degrees(), Catch::Matchers::WithinAbs(-90., 0.01) );

	// Negate negative angle
	Angle a2(-90.);
	Angle neg2 = -a2;
	CHECK_THAT( neg2.Degrees(), Catch::Matchers::WithinAbs(90., 0.01) );

	// Negate zero
	Angle a3(0.);
	Angle neg3 = -a3;
	CHECK_THAT( neg3.Degrees(), Catch::Matchers::WithinAbs(0., 0.01) );

	// Negate 180 degrees
	Angle a4(180.);
	Angle neg4 = -a4;
	CHECK_THAT( neg4.Degrees(), Catch::Matchers::WithinAbs(-180., 0.01) );

	// Double negation should return original
	Angle a5(45.);
	Angle doubleNeg = -(-a5);
	CHECK_THAT( doubleNeg.Degrees(), Catch::Matchers::WithinAbs(45., 0.01) );
}

TEST_CASE( "Angle::operator equality", "[angle][comparison]" ) {
	// Equal angles
	Angle a1(90.);
	Angle a2(90.);
	CHECK( a1 == a2 );
	CHECK_FALSE( a1 != a2 );

	// Angles that differ by 360 degrees are equal
	Angle a3(90.);
	Angle a4(450.);
	CHECK( a3 == a4 );
	CHECK_FALSE( a3 != a4 );

	// Angles that differ by -360 degrees are equal
	Angle a5(90.);
	Angle a6(-270.);
	CHECK( a5 == a6 );

	// Zero and 360 are equal
	Angle zero(0.);
	Angle fullCircle(360.);
	CHECK( zero == fullCircle );

	// Different angles
	Angle a7(90.);
	Angle a8(91.);
	CHECK( a7 != a8 );
	CHECK_FALSE( a7 == a8 );
}

TEST_CASE( "Angle::Unit roundtrip", "[angle][unit]" ) {
	// Test that Angle -> Unit -> Angle preserves the angle
	auto testRoundtrip = [](double degrees) {
		Angle original(degrees);
		Point unit = original.Unit();
		Angle reconstructed(unit);
		CHECK_THAT( reconstructed.Degrees(), Catch::Matchers::WithinAbs(original.Degrees(), 0.01) );
	};

	testRoundtrip(0.);
	testRoundtrip(45.);
	testRoundtrip(90.);
	testRoundtrip(135.);
	testRoundtrip(180.);
	testRoundtrip(225.);
	testRoundtrip(270.);
	testRoundtrip(315.);
	testRoundtrip(-45.);
	testRoundtrip(-90.);
}

TEST_CASE( "Angle::Random", "[angle][random]") {
	auto value = GENERATE(10, 100, 1000, 10000, 100000, 1000000, 3600000);

	for(int i = 0; i < 10; ++i)
	{
		auto random = Angle::Random(value);
		CHECK_THAT( random.Degrees(), Catch::Matchers::WithinAbs(0, 180) );

		auto unit = random.Unit();
		CHECK_THAT( unit.X(), Catch::Matchers::WithinAbs(0, 1) );
		CHECK_THAT( unit.Y(), Catch::Matchers::WithinAbs(0, 1) );
	}
}

TEST_CASE( "Angle::Random no range", "[angle][random]" ) {
	// Random with no parameter should give any angle
	for(int i = 0; i < 100; ++i)
	{
		Angle random = Angle::Random();
		// Check that the angle is valid (in range -180 to 180)
		CHECK_THAT( random.Degrees(), Catch::Matchers::WithinAbs(0., 180.) );
		// Check that unit vector is normalized
		Point unit = random.Unit();
		double length = std::sqrt(unit.X() * unit.X() + unit.Y() * unit.Y());
		CHECK_THAT( length, Catch::Matchers::WithinAbs(1., 0.001) );
	}
}

TEST_CASE( "Angle::Random with range", "[angle][random]" ) {
	// Random with 0 range should always give 0
	for(int i = 0; i < 10; ++i)
	{
		Angle random = Angle::Random(0.);
		CHECK_THAT( random.Degrees(), Catch::Matchers::WithinAbs(0., 0.01) );
	}

	// Random with 360 range can give any angle
	for(int i = 0; i < 100; ++i)
	{
		Angle random = Angle::Random(360.);
		CHECK_THAT( random.Degrees(), Catch::Matchers::WithinAbs(0., 180.) );
	}

	// Random with negative range (should handle gracefully)
	Angle randomNeg = Angle::Random(-90.);
	CHECK_THAT( randomNeg.Degrees(), Catch::Matchers::WithinAbs(0., 180.) );
}

TEST_CASE( "Angle::IsInRange", "[angle][isInRange]" ) {
	Angle base = 0.;
	Angle limit = 359.;
	REQUIRE( Angle(0.).IsInRange(base, limit) );
	REQUIRE( Angle(359.).IsInRange(base, limit) );
	REQUIRE( Angle(180.).IsInRange(base, limit) );

	base = -20.;
	limit = 20.;
	REQUIRE( Angle(0.).IsInRange(base, limit) );
	REQUIRE( Angle(20.).IsInRange(base, limit) );
	REQUIRE( Angle(-20.).IsInRange(base, limit) );
	REQUIRE_FALSE( Angle(21.).IsInRange(base, limit) );
	REQUIRE_FALSE( Angle(-21.).IsInRange(base, limit) );
	REQUIRE_FALSE( Angle(180.).IsInRange(base, limit) );
}

TEST_CASE( "Angle::IsInRange edge cases", "[angle][isInRange]" ) {
	// Range where base equals limit (empty range, only that exact angle is valid)
	Angle base(45.);
	Angle limit(45.);
	CHECK( Angle(45.).IsInRange(base, limit) );
	CHECK_FALSE( Angle(46.).IsInRange(base, limit) );
	CHECK_FALSE( Angle(44.).IsInRange(base, limit) );

	// Small range
	Angle baseSmall(10.);
	Angle limitSmall(20.);
	CHECK( Angle(10.).IsInRange(baseSmall, limitSmall) );
	CHECK( Angle(15.).IsInRange(baseSmall, limitSmall) );
	CHECK( Angle(20.).IsInRange(baseSmall, limitSmall) );
	CHECK_FALSE( Angle(9.).IsInRange(baseSmall, limitSmall) );
	CHECK_FALSE( Angle(21.).IsInRange(baseSmall, limitSmall) );

	// Range wrapping through 0/360
	Angle baseWrap(350.);
	Angle limitWrap(10.);
	CHECK( Angle(350.).IsInRange(baseWrap, limitWrap) );
	CHECK( Angle(355.).IsInRange(baseWrap, limitWrap) );
	CHECK( Angle(0.).IsInRange(baseWrap, limitWrap) );
	CHECK( Angle(5.).IsInRange(baseWrap, limitWrap) );
	CHECK( Angle(10.).IsInRange(baseWrap, limitWrap) );
	CHECK_FALSE( Angle(180.).IsInRange(baseWrap, limitWrap) );
	CHECK_FALSE( Angle(340.).IsInRange(baseWrap, limitWrap) );

	// Full circle range
	Angle baseFull(0.);
	Angle limitFull(359.99);
	CHECK( Angle(0.).IsInRange(baseFull, limitFull) );
	CHECK( Angle(180.).IsInRange(baseFull, limitFull) );
	CHECK( Angle(359.).IsInRange(baseFull, limitFull) );
}

TEST_CASE( "Angle::IsInRange with negative angles", "[angle][isInRange]" ) {
	// Range with negative base
	Angle base(-45.);
	Angle limit(45.);
	CHECK( Angle(-45.).IsInRange(base, limit) );
	CHECK( Angle(0.).IsInRange(base, limit) );
	CHECK( Angle(45.).IsInRange(base, limit) );
	CHECK( Angle(-30.).IsInRange(base, limit) );
	CHECK( Angle(30.).IsInRange(base, limit) );
	CHECK_FALSE( Angle(-46.).IsInRange(base, limit) );
	CHECK_FALSE( Angle(46.).IsInRange(base, limit) );
	CHECK_FALSE( Angle(180.).IsInRange(base, limit) );
}

TEST_CASE( "Angle precision", "[angle][precision]" ) {
	// Test that small angle differences are preserved
	Angle small1(0.01);
	Angle small2(0.02);
	CHECK( small1 != small2 );

	// Test that very small angles round appropriately
	Angle tiny(0.005);
	CHECK_THAT( tiny.Degrees(), Catch::Matchers::WithinAbs(0., 0.01) );

	// Test addition precision
	Angle a(1.);
	Angle b(1.);
	Angle c = a + b;
	CHECK_THAT( c.Degrees(), Catch::Matchers::WithinAbs(2., 0.01) );

	// Test that precision is maintained through operations
	Angle start(123.456);
	Angle rotated = start + Angle(0.) - Angle(0.);
	CHECK_THAT( rotated.Degrees(), Catch::Matchers::WithinAbs(start.Degrees(), 0.01) );
}
// Test code goes here. Preferably, use scenario-driven language making use of the SCENARIO, GIVEN,
// WHEN, and THEN macros. (There will be cases where the more traditional TEST_CASE and SECTION macros
// are better suited to declaration of the public API.)

// When writing assertions, prefer the CHECK and CHECK_FALSE macros when probing the scenario, and prefer
// the REQUIRE / REQUIRE_FALSE macros for fundamental / "validity" assertions. If a CHECK fails, the rest
// of the block's statements will still be evaluated, but a REQUIRE failure will exit the current block.

// #endregion unit tests

// #region benchmarks
#ifdef CATCH_CONFIG_ENABLE_BENCHMARKING
TEST_CASE( "Benchmark Angle::Random", "[!benchmark][angle][random]" ) {
	BENCHMARK( "Angle::Random()" ) {
		return Angle::Random();
	};
	BENCHMARK( "Angle::Random(60.)" ) {
		return Angle::Random(60.);
	};
	BENCHMARK( "Angle::Random(180.)" ) {
		return Angle::Random(180.);
	};
	BENCHMARK( "Angle::Random(360.)" ) {
		return Angle::Random(360.);
	};
}

TEST_CASE( "Benchmark Angle operations", "[!benchmark][angle]" ) {
	Angle a(45.);
	Angle b(90.);
	Point p(1., 2.);

	BENCHMARK( "Angle::Unit()" ) {
		return a.Unit();
	};

	BENCHMARK( "Angle::Degrees()" ) {
		return a.Degrees();
	};

	BENCHMARK( "Angle::AbsDegrees()" ) {
		return a.AbsDegrees();
	};

	BENCHMARK( "Angle::Rotate(Point)" ) {
		return a.Rotate(p);
	};

	BENCHMARK( "Angle::IsInRange" ) {
		return a.IsInRange(b, Angle(180.));
	};

	BENCHMARK( "Angle operator+" ) {
		return a + b;
	};

	BENCHMARK( "Angle operator-" ) {
		return a - b;
	};

	BENCHMARK( "Angle unary negate" ) {
		return -a;
	};
}
#endif
// #endregion benchmarks



} // test namespace
