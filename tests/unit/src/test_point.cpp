/* test_point.cpp
Copyright (c) 2020 by Benjamin Hauch

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
#include "../../../source/Point.h"

// ... and any system includes needed for the test file.
#include <cmath>
#include <limits>



namespace { // test namespace
// #region mock data
// #endregion mock data



// #region unit tests
TEST_CASE( "Point Basics", "[Point]" ) {
	using T = Point;
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

SCENARIO( "A position or other geometric vector must be expressed", "[Point]" ) {
	GIVEN( "No initial values" ) {
		Point a;
		WHEN( "the point is created" ) {
			THEN( "it represents (0, 0)" ) {
				CHECK( a.X() == 0. );
				CHECK( a.Y() == 0. );
			}
		}
		WHEN( "Set is called" ) {
			a.Set(1, 3);
			THEN( "X and Y are updated" ) {
				CHECK( a.X() == 1. );
				CHECK( a.Y() == 3. );
			}
		}
	}

	GIVEN( "any Point" ) {
		auto a = Point();
		WHEN( "the point represents (0, 0)" ) {
			a.X() = 0.;
			a.Y() = 0.;
			THEN( "it can be converted to boolean FALSE" ) {
				CHECK( static_cast<bool>(a) == false );
			}

			THEN( "it is equal to the default point" ) {
				CHECK( a == Point() );
			}
		}
		WHEN( "the point has non-zero X" ) {
			a.X() = 0.00001;
			REQUIRE( a.Y() == 0. );
			THEN( "it can be converted to boolean TRUE" ) {
				CHECK( static_cast<bool>(a) == true );
			}

			THEN( "it isn't equal to the default point" ) {
				CHECK( a != Point() );
			}
		}
		WHEN( "the point has non-zero Y") {
			a.Y() = 0.00001;
			REQUIRE( a.X() == 0. );
			THEN( "it can be converted to boolean TRUE" ) {
				CHECK( static_cast<bool>(a) == true );
			}
		}
	}

	GIVEN( "two points" ) {
		auto a = Point();
		auto b = Point();

		WHEN( "both points are (0, 0)" ) {
			THEN( "they are equal" ) {
				CHECK( a == b );
			}
		}

		WHEN( "one point is different" ) {
			a.X() = 0.0001;
			THEN( "they are not equal" ) {
				CHECK( a != b );
			}
		}
	}
}

SCENARIO( "Copying Points", "[Point]" ) {
	GIVEN( "any Point" ) {
		auto source = Point(5.4321, 10.987654321);
		WHEN( "copied by constructor" ) {
			Point copy(source);
			THEN( "the copy has the correct values" ) {
				CHECK( copy.X() == source.X() );
				CHECK( copy.Y() == source.Y() );
			}
		}
		WHEN( "copied by assignment" ) {
			Point copy = source;
			THEN( "the copy has the correct values" ) {
				CHECK( copy.X() == source.X() );
				CHECK( copy.Y() == source.Y() );
			}
		}
	}
}

SCENARIO( "Adding points", "[Point][operator+]" ) {
	GIVEN( "any Point" ) {
		Point first = Point(5.4321, 10.987654321);
		WHEN( "Nothing is added" ) {
			Point second;
			Point third;
			THEN( "The sum is the same as the original" ) {
				CHECK( first == (first + second) );
				CHECK( first == (second + first) );
				third = first;
				third += second;
				CHECK( first == third );
				third = second;
				third += first;
				CHECK( first == third );
			}
		}
		WHEN( "A value is added" ) {
			Point second = Point(25.4321, 10.487254321);
			Point third;
			Point expected = Point(first.X() + second.X(), first.Y() + second.Y());
			THEN( "The sum is has the expected value" ) {
				CHECK( expected == (first + second) );
				CHECK( expected == (second + first) );
				third = first;
				third += second;
				CHECK( expected == third );
				third = second;
				third += first;
				CHECK( expected == third );
			}
		}
	}
}

SCENARIO( "Subtracting points", "[Point][operator-]" ) {
	GIVEN( "any Point" ) {
		Point first = Point(5.4321, 10.987654321);
		WHEN( "Nothing is subtracted" ) {
			Point second;
			Point third;
			THEN( "The result is the same as the original" ) {
				CHECK( first == (first - second) );
				CHECK( -first == (second - first) );
				third = first;
				third -= second;
				CHECK( first == third );
				third = second;
				third -= first;
				CHECK( -first == third );
			}
		}
		WHEN( "A value is subtracted" ) {
			Point second = Point(25.4321, 10.487254321);
			Point third;
			Point expected = Point(first.X() - second.X(), first.Y() - second.Y());
			THEN( "The sum is has the expected value" ) {
				CHECK( expected == (first - second) );
				CHECK( -expected == (second - first) );
				third = first;
				third -= second;
				CHECK( expected == third );
				third = second;
				third -= first;
				CHECK( -expected == third );
			}
		}
	}
}

SCENARIO( "Multiplying points", "[Point][operator*]" ) {
	GIVEN( "any Point" ) {
		Point first = Point(5.4321, 10.987654321);
		WHEN( "Multiplied with 1" ) {
			THEN( "The result is the same as the original" ) {
				CHECK( first == (first * 1.) );
				CHECK( first == (1. * first) );
			}
		}
		WHEN( "Multiplied with 0" ) {
			THEN( "The result is 0" ) {
				CHECK( Point() == (first * 0.) );
				CHECK( Point() == (0. * first) );
			}
		}
		WHEN( "Multiplied with a number" )
		{
			double mult = 25.25406;
			Point expected(first.X() * mult, first.Y() * mult);
			THEN( "The result is correct" ) {
				CHECK( expected == first * mult );
				CHECK( expected == mult * first );
			}
		}
	}
}

SCENARIO( "Dividing points", "[Point][operator/]" ) {
	GIVEN( "any Point" ) {
		Point first = Point(5.4321, 10.987654321);
		WHEN( "Divided by 1" ) {
			THEN( "The result is the same as the original" ) {
				CHECK( first == (first / 1.) );
			}
		}
		WHEN( "Divided by 0" ) {
			THEN( "The result is infinity" ) {
				CHECK( std::numeric_limits<double>::infinity() == (first / 0.).X() );
				CHECK( std::numeric_limits<double>::infinity() == (first / 0.).Y() );
			}
		}
		WHEN( "Divided by a number" )
		{
			double div = 25.25406;
			Point expected(first.X() / div, first.Y() / div);
			THEN( "The result is correct" ) {
				CHECK( expected == first / div );
			}
		}
	}
}

SCENARIO( "Multiplying points with each other", "[Point][operator*]" ) {
	GIVEN( "any two Points" ) {
		Point first = Point(5.4321, 10.987654321);
		Point second = Point(63.57151, 0.156123);
		WHEN( "Multiplied with each other" ) {
			Point expected(first.X() * second.X(), first.Y() * second.Y());
			THEN( "The result is correct" ) {
				CHECK( expected == first * second );
				CHECK( expected == second * first );
			}
		}
	}
}

SCENARIO( "Calculating dot product", "[Point][Dot]" ) {
	GIVEN( "any two Points" ) {
		Point first = Point(5.4321, 10.987654321);
		Point second = Point(63.57151, 0.156123);
		WHEN( "Multiplied with each other" ) {
			double expected = first.X() * second.X() + first.Y() * second.Y();
			THEN( "The result is correct" ) {
				CHECK( expected == first.Dot(second) );
				CHECK( expected == second.Dot(first) );
			}
		}
	}
}

SCENARIO( "Calculating cross product", "[Point][Dot]" ) {
	GIVEN( "any two Points" ) {
		Point first = Point(5.4321, 10.987654321);
		Point second = Point(63.57151, 0.156123);
		WHEN( "Multiplied with each other" ) {
			double expected = first.X() * second.Y() - first.Y() * second.X();
			THEN( "The result is correct" ) {
				CHECK( expected == first.Cross(second) );
				CHECK( -expected == second.Cross(first) );
			}
		}
	}
}

SCENARIO( "Calculating length", "[Point][Length]" ) {
	GIVEN( "any Point" ) {
		Point first = Point(5.4321, 10.987654321);
		WHEN( "Checking length" ) {
			double expectedSq = first.X() * first.X() + first.Y() * first.Y();
			double expected = sqrt(expectedSq);
			THEN( "The result is correct" ) {
				CHECK_THAT( first.Length(), Catch::Matchers::WithinAbs(expected, 0.0001) );
				CHECK_THAT( first.LengthSquared(), Catch::Matchers::WithinAbs(expectedSq, 0.0001) );
			}
		}
	}
}

SCENARIO( "Calculating unit vector", "[Point][Unit]" ) {
	GIVEN( "any Point" ) {
		Point first = Point(5.4321, 10.987654321);
		WHEN( "Calculating unit" ) {
			Point expected = first / first.Length();
			THEN( "The result is correct" ) {
				CHECK_THAT( first.Unit().X(), Catch::Matchers::WithinAbs(expected.X(), 0.0001) );
				CHECK_THAT( first.Unit().Y(), Catch::Matchers::WithinAbs(expected.Y(), 0.0001) );
			}
		}
	}
}

SCENARIO( "Calculating distance", "[Point][Distance]" ) {
	GIVEN( "any two Points" ) {
		Point first = Point(5.4321, 10.987654321);
		Point second = Point(63.57151, 0.156123);
		WHEN( "Calculating distance" ) {
			Point delta = first - second;
			double expected = delta.Length();
			double expectedSq = delta.LengthSquared();
			THEN( "The result is correct" ) {
				CHECK_THAT( first.Distance(second), Catch::Matchers::WithinAbs(expected, 0.0001) );
				CHECK_THAT( second.Distance(first), Catch::Matchers::WithinAbs(expected, 0.0001) );
				CHECK_THAT( first.DistanceSquared(second), Catch::Matchers::WithinAbs(expectedSq, 0.0001) );
				CHECK_THAT( second.DistanceSquared(first), Catch::Matchers::WithinAbs(expectedSq, 0.0001) );
			}
		}
	}
}

SCENARIO( "Linear interpolation", "[Point][Lerp]" ) {
	GIVEN( "any two Points" ) {
		Point first = Point(5.4321, 10.987654321);
		Point second = Point(63.57151, 0.156123);
		WHEN( "Interpolating the first position" ) {
			THEN( "The result is the first point" ) {
				CHECK_THAT( first.Distance(first.Lerp(second, 0.)), Catch::Matchers::WithinAbs(0., 0.0001) );
			}
		}
		WHEN( "Interpolating the second position" ) {
			THEN( "The result is the second point" ) {
				CHECK_THAT( second.Distance(first.Lerp(second, 1.)), Catch::Matchers::WithinAbs(0., 0.0001) );
			}
		}
		WHEN( "Interpolating between them" ) {
			double c = 0.2637;
			Point delta = second - first;
			Point offset = delta * c;
			Point expected = first + offset;
			THEN( "The result is correct" ) {
				Point result = first.Lerp(second, c);
				CHECK_THAT( expected.Distance(result), Catch::Matchers::WithinAbs(0., 0.0001) );
			}
			THEN( "The inverse result is correct" ) {
				Point result = second.Lerp(first, 1. - c);
				CHECK_THAT( expected.Distance(result), Catch::Matchers::WithinAbs(0., 0.0001) );
			}
		}
	}
}

SCENARIO( "Calculating absolute value", "[Point][abs]" ) {
	GIVEN( "any Point" ) {
		Point first = Point(5.4321, -10.987654321);
		WHEN( "Calculating abs" ) {
			THEN( "The result is correct" ) {
				CHECK( std::abs(first.X()) == abs(first).X() );
				CHECK( std::abs(first.Y()) == abs(first).Y() );
			}
		}
	}
}

SCENARIO( "Calculating min/max", "[Point][min]" ) {
	GIVEN( "any two Points" ) {
		Point first = Point(5.4321, 10.987654321);
		Point second = Point(-63.57151, 0.156123);
		WHEN( "Calculating min" ) {
			THEN( "The result is the minimum" ) {
				CHECK( std::min(first.X(), second.X()) == min(first, second).X() );
				CHECK( std::min(first.Y(), second.Y()) == min(first, second).Y() );
			}
		}
		WHEN( "Calculating max" ) {
			THEN( "The result is the maximum" ) {
				CHECK( std::max(first.X(), second.X()) == max(first, second).X() );
				CHECK( std::max(first.Y(), second.Y()) == max(first, second).Y() );
			}
		}
	}
}

// ============================================================================
// Enhanced Tests: Boundary Conditions and Edge Cases
// ============================================================================

SCENARIO( "Unary negation operator", "[Point][operator-]" ) {
	GIVEN( "any Point" ) {
		Point p = Point(5.4321, -10.987654321);
		WHEN( "Negating the point" ) {
			Point negated = -p;
			THEN( "The result has negated coordinates" ) {
				CHECK( negated.X() == -p.X() );
				CHECK( negated.Y() == -p.Y() );
			}
			AND_THEN( "Double negation returns the original" ) {
				Point doubleNeg = -negated;
				CHECK( doubleNeg.X() == p.X() );
				CHECK( doubleNeg.Y() == p.Y() );
			}
		}
	}
	GIVEN( "a zero point" ) {
		Point p = Point(0., 0.);
		WHEN( "Negating the zero point" ) {
			Point negated = -p;
			THEN( "The result is still zero" ) {
				CHECK( negated.X() == 0. );
				CHECK( negated.Y() == 0. );
			}
		}
	}
}

SCENARIO( "Logical NOT operator", "[Point][operator!]" ) {
	GIVEN( "a zero point" ) {
		Point zero;
		WHEN( "Applying NOT operator" ) {
			THEN( "The result is true" ) {
				CHECK( !zero == true );
			}
		}
	}
	GIVEN( "a non-zero point" ) {
		Point p(1., 0.);
		WHEN( "Applying NOT operator" ) {
			THEN( "The result is false" ) {
				CHECK( !p == false );
			}
		}
	}
	GIVEN( "a point with only non-zero Y" ) {
		Point p(0., 1.);
		WHEN( "Applying NOT operator" ) {
			THEN( "The result is false" ) {
				CHECK( !p == false );
			}
		}
	}
}

SCENARIO( "Compound assignment operators", "[Point][operator+=][operator-=][operator*=][operator/=]" ) {
	GIVEN( "any Point" ) {
		Point original(5.4321, 10.987654321);
		WHEN( "Using += operator" ) {
			Point p = original;
			Point delta(1.5, -2.5);
			p += delta;
			THEN( "The result is correct" ) {
				CHECK_THAT( p.X(), Catch::Matchers::WithinAbs(original.X() + delta.X(), 0.0001) );
				CHECK_THAT( p.Y(), Catch::Matchers::WithinAbs(original.Y() + delta.Y(), 0.0001) );
			}
		}
		WHEN( "Using -= operator" ) {
			Point p = original;
			Point delta(1.5, -2.5);
			p -= delta;
			THEN( "The result is correct" ) {
				CHECK_THAT( p.X(), Catch::Matchers::WithinAbs(original.X() - delta.X(), 0.0001) );
				CHECK_THAT( p.Y(), Catch::Matchers::WithinAbs(original.Y() - delta.Y(), 0.0001) );
			}
		}
		WHEN( "Using *= operator with scalar" ) {
			Point p = original;
			double scalar = 2.5;
			p *= scalar;
			THEN( "The result is correct" ) {
				CHECK_THAT( p.X(), Catch::Matchers::WithinAbs(original.X() * scalar, 0.0001) );
				CHECK_THAT( p.Y(), Catch::Matchers::WithinAbs(original.Y() * scalar, 0.0001) );
			}
		}
		WHEN( "Using /= operator with scalar" ) {
			Point p = original;
			double scalar = 2.5;
			p /= scalar;
			THEN( "The result is correct" ) {
				CHECK_THAT( p.X(), Catch::Matchers::WithinAbs(original.X() / scalar, 0.0001) );
				CHECK_THAT( p.Y(), Catch::Matchers::WithinAbs(original.Y() / scalar, 0.0001) );
			}
		}
		WHEN( "Using *= operator with another Point" ) {
			Point p = original;
			Point other(2.0, 3.0);
			p *= other;
			THEN( "The result is correct (component-wise)" ) {
				CHECK_THAT( p.X(), Catch::Matchers::WithinAbs(original.X() * other.X(), 0.0001) );
				CHECK_THAT( p.Y(), Catch::Matchers::WithinAbs(original.Y() * other.Y(), 0.0001) );
			}
		}
	}
}

SCENARIO( "Zero point edge cases", "[Point][edge]" ) {
	GIVEN( "a zero point" ) {
		Point zero;
		WHEN( "Adding zero to any point" ) {
			Point p(5., 10.);
			Point result = p + zero;
			THEN( "The result is the same as the original" ) {
				CHECK( result == p );
			}
		}
		WHEN( "Subtracting zero from any point" ) {
			Point p(5., 10.);
			Point result = p - zero;
			THEN( "The result is the same as the original" ) {
				CHECK( result == p );
			}
		}
		WHEN( "Multiplying zero point by a scalar" ) {
			double scalar = 5.5;
			Point result = zero * scalar;
			THEN( "The result is still zero" ) {
				CHECK( result.X() == 0. );
				CHECK( result.Y() == 0. );
			}
		}
		WHEN( "Multiplying any point by zero scalar" ) {
			Point p(5., 10.);
			Point result = p * 0.;
			THEN( "The result is zero" ) {
				CHECK( result.X() == 0. );
				CHECK( result.Y() == 0. );
			}
		}
		WHEN( "Calculating dot product with zero" ) {
			Point p(5., 10.);
			double dot = p.Dot(zero);
			THEN( "The result is zero" ) {
				CHECK( dot == 0. );
			}
		}
		WHEN( "Calculating cross product with zero" ) {
			Point p(5., 10.);
			double cross = p.Cross(zero);
			THEN( "The result is zero" ) {
				CHECK( cross == 0. );
			}
		}
		WHEN( "Calculating length of zero" ) {
			THEN( "The length is zero" ) {
				CHECK( zero.Length() == 0. );
				CHECK( zero.LengthSquared() == 0. );
			}
		}
		WHEN( "Calculating distance between zero points" ) {
			THEN( "The distance is zero" ) {
				CHECK( zero.Distance(zero) == 0. );
				CHECK( zero.DistanceSquared(zero) == 0. );
			}
		}
		WHEN( "Negating zero point" ) {
			Point negated = -zero;
			THEN( "The result is still zero" ) {
				CHECK( negated.X() == 0. );
				CHECK( negated.Y() == 0. );
			}
		}
	}
}

SCENARIO( "Negative coordinates", "[Point][negative]" ) {
	GIVEN( "a point with negative coordinates" ) {
		Point neg(-5., -10.);
		WHEN( "Calculating length" ) {
			double length = neg.Length();
			THEN( "The length is positive" ) {
				CHECK( length > 0. );
				CHECK_THAT( length, Catch::Matchers::WithinAbs(std::sqrt(25. + 100.), 0.0001) );
			}
		}
		WHEN( "Calculating unit vector" ) {
			Point unit = neg.Unit();
			THEN( "The unit vector points in the same direction" ) {
				CHECK_THAT( unit.X(), Catch::Matchers::WithinAbs(neg.X() / neg.Length(), 0.0001) );
				CHECK_THAT( unit.Y(), Catch::Matchers::WithinAbs(neg.Y() / neg.Length(), 0.0001) );
				CHECK_THAT( unit.Length(), Catch::Matchers::WithinAbs(1., 0.0001) );
			}
		}
		WHEN( "Taking absolute value" ) {
			Point absVal = abs(neg);
			THEN( "Both coordinates are positive" ) {
				CHECK( absVal.X() == std::abs(neg.X()) );
				CHECK( absVal.Y() == std::abs(neg.Y()) );
			}
		}
	}
	GIVEN( "a point with mixed sign coordinates" ) {
		Point mixed(5., -10.);
		WHEN( "Taking absolute value" ) {
			Point absVal = abs(mixed);
			THEN( "Both coordinates are positive" ) {
				CHECK( absVal.X() == std::abs(mixed.X()) );
				CHECK( absVal.Y() == std::abs(mixed.Y()) );
			}
		}
	}
}

SCENARIO( "Large values", "[Point][edge]" ) {
	GIVEN( "points with large coordinates" ) {
		Point large(1e10, 2e10);
		Point larger(3e10, 4e10);
		WHEN( "Adding large points" ) {
			Point result = large + larger;
			THEN( "The result is correct" ) {
				CHECK_THAT( result.X(), Catch::Matchers::WithinAbs(4e10, 1e5) );
				CHECK_THAT( result.Y(), Catch::Matchers::WithinAbs(6e10, 1e5) );
			}
		}
		WHEN( "Calculating distance between large points" ) {
			double dist = large.Distance(larger);
			double expected = std::sqrt(std::pow(2e10, 2) + std::pow(2e10, 2));
			THEN( "The distance is calculated correctly" ) {
				CHECK_THAT( dist, Catch::Matchers::WithinAbs(expected, 1e5) );
			}
		}
		WHEN( "Calculating unit vector of large point" ) {
			Point unit = large.Unit();
			THEN( "The unit vector has length 1" ) {
				CHECK_THAT( unit.Length(), Catch::Matchers::WithinAbs(1., 0.0001) );
			}
		}
	}
}

SCENARIO( "Small values", "[Point][edge]" ) {
	GIVEN( "points with very small coordinates" ) {
		Point tiny(1e-10, 2e-10);
		Point tinier(3e-10, 4e-10);
		WHEN( "Adding tiny points" ) {
			Point result = tiny + tinier;
			THEN( "The result is correct" ) {
				CHECK_THAT( result.X(), Catch::Matchers::WithinAbs(4e-10, 1e-15) );
				CHECK_THAT( result.Y(), Catch::Matchers::WithinAbs(6e-10, 1e-15) );
			}
		}
		WHEN( "Calculating length of tiny point" ) {
			double length = tiny.Length();
			double expected = std::sqrt(1e-20 + 4e-20);
			THEN( "The length is calculated correctly" ) {
				CHECK_THAT( length, Catch::Matchers::WithinAbs(expected, 1e-20) );
			}
		}
	}
}

SCENARIO( "Division by zero", "[Point][edge]" ) {
	GIVEN( "any non-zero point" ) {
		Point p(5., 10.);
		WHEN( "Dividing by zero" ) {
			Point result = p / 0.;
			THEN( "The result is infinity" ) {
				CHECK( std::isinf(result.X()) );
				CHECK( std::isinf(result.Y()) );
			}
		}
	}
	GIVEN( "a zero point" ) {
		Point zero;
		WHEN( "Dividing by zero" ) {
			Point result = zero / 0.;
			THEN( "The result is NaN" ) {
				CHECK( std::isnan(result.X()) );
				CHECK( std::isnan(result.Y()) );
			}
		}
	}
}

SCENARIO( "Special floating-point values", "[Point][edge]" ) {
	GIVEN( "a point with infinity coordinates" ) {
		Point inf(std::numeric_limits<double>::infinity(), 10.);
		WHEN( "Checking length" ) {
			THEN( "The length is infinity" ) {
				CHECK( std::isinf(inf.Length()) );
			}
		}
		WHEN( "Negating" ) {
			Point neg = -inf;
			THEN( "Infinity becomes negative infinity" ) {
				CHECK( std::isinf(neg.X()) );
				CHECK( neg.X() < 0 );
			}
		}
	}
	GIVEN( "a point with NaN coordinates" ) {
		Point nanPoint(std::numeric_limits<double>::quiet_NaN(), 10.);
		WHEN( "Checking length" ) {
			THEN( "The length is NaN" ) {
				CHECK( std::isnan(nanPoint.Length()) );
			}
		}
		WHEN( "Comparing with another point" ) {
			Point other(5., 10.);
			THEN( "NaN comparisons are false" ) {
				CHECK_FALSE( nanPoint == other );
				CHECK_FALSE( nanPoint == nanPoint ); // NaN != NaN
			}
		}
	}
}

SCENARIO( "Lerp boundary conditions", "[Point][Lerp][edge]" ) {
	GIVEN( "two points" ) {
		Point a(5., 10.);
		Point b(15., 20.);
		WHEN( "Interpolating at t=0" ) {
			Point result = a.Lerp(b, 0.);
			THEN( "The result is the first point" ) {
				CHECK_THAT( result.X(), Catch::Matchers::WithinAbs(a.X(), 0.0001) );
				CHECK_THAT( result.Y(), Catch::Matchers::WithinAbs(a.Y(), 0.0001) );
			}
		}
		WHEN( "Interpolating at t=1" ) {
			Point result = a.Lerp(b, 1.);
			THEN( "The result is the second point" ) {
				CHECK_THAT( result.X(), Catch::Matchers::WithinAbs(b.X(), 0.0001) );
				CHECK_THAT( result.Y(), Catch::Matchers::WithinAbs(b.Y(), 0.0001) );
			}
		}
		WHEN( "Interpolating at t=0.5" ) {
			Point result = a.Lerp(b, 0.5);
			Point expected((a.X() + b.X()) / 2., (a.Y() + b.Y()) / 2.);
			THEN( "The result is the midpoint" ) {
				CHECK_THAT( result.X(), Catch::Matchers::WithinAbs(expected.X(), 0.0001) );
				CHECK_THAT( result.Y(), Catch::Matchers::WithinAbs(expected.Y(), 0.0001) );
			}
		}
		WHEN( "Interpolating at t<0 (extrapolation)" ) {
			Point result = a.Lerp(b, -0.5);
			Point expected(a.X() + (b.X() - a.X()) * -0.5, a.Y() + (b.Y() - a.Y()) * -0.5);
			THEN( "The result extrapolates backward" ) {
				CHECK_THAT( result.X(), Catch::Matchers::WithinAbs(expected.X(), 0.0001) );
				CHECK_THAT( result.Y(), Catch::Matchers::WithinAbs(expected.Y(), 0.0001) );
			}
		}
		WHEN( "Interpolating at t>1 (extrapolation)" ) {
			Point result = a.Lerp(b, 1.5);
			Point expected(a.X() + (b.X() - a.X()) * 1.5, a.Y() + (b.Y() - a.Y()) * 1.5);
			THEN( "The result extrapolates forward" ) {
				CHECK_THAT( result.X(), Catch::Matchers::WithinAbs(expected.X(), 0.0001) );
				CHECK_THAT( result.Y(), Catch::Matchers::WithinAbs(expected.Y(), 0.0001) );
			}
		}
		WHEN( "Interpolating with the same point" ) {
			Point result = a.Lerp(a, 0.5);
			THEN( "The result is the same point" ) {
				CHECK_THAT( result.X(), Catch::Matchers::WithinAbs(a.X(), 0.0001) );
				CHECK_THAT( result.Y(), Catch::Matchers::WithinAbs(a.Y(), 0.0001) );
			}
		}
	}
}

SCENARIO( "Same point operations", "[Point][edge]" ) {
	GIVEN( "a point" ) {
		Point p(5., 10.);
		WHEN( "Calculating distance to itself" ) {
			double dist = p.Distance(p);
			THEN( "The distance is zero" ) {
				CHECK( dist == 0. );
			}
		}
		WHEN( "Adding the point to itself" ) {
			Point result = p + p;
			THEN( "The result is doubled" ) {
				CHECK( result.X() == p.X() * 2 );
				CHECK( result.Y() == p.Y() * 2 );
			}
		}
		WHEN( "Subtracting the point from itself" ) {
			Point result = p - p;
			THEN( "The result is zero" ) {
				CHECK( result.X() == 0. );
				CHECK( result.Y() == 0. );
			}
		}
		WHEN( "Multiplying by itself (component-wise)" ) {
			Point result = p * p;
			THEN( "Each component is squared" ) {
				CHECK( result.X() == p.X() * p.X() );
				CHECK( result.Y() == p.Y() * p.Y() );
			}
		}
		WHEN( "Calculating dot product with itself" ) {
			double dot = p.Dot(p);
			THEN( "The result is the squared length" ) {
				CHECK( dot == p.LengthSquared() );
			}
		}
		WHEN( "Calculating cross product with itself" ) {
			double cross = p.Cross(p);
			THEN( "The result is zero" ) {
				CHECK( cross == 0. );
			}
		}
	}
}

SCENARIO( "Orthogonal vectors", "[Point][Dot][Cross]" ) {
	GIVEN( "orthogonal vectors" ) {
		Point horizontal(10., 0.);
		Point vertical(0., 5.);
		WHEN( "Calculating dot product" ) {
			double dot = horizontal.Dot(vertical);
			THEN( "The dot product is zero" ) {
				CHECK( dot == 0. );
			}
		}
		WHEN( "Calculating cross product" ) {
			double cross = horizontal.Cross(vertical);
			THEN( "The cross product is non-zero (z-component of 3D cross)" ) {
				CHECK_THAT( cross, Catch::Matchers::WithinAbs(horizontal.X() * vertical.Y(), 0.0001) );
			}
		}
		WHEN( "Calculating cross product in reverse order" ) {
			double cross = vertical.Cross(horizontal);
			THEN( "The cross product is negated" ) {
				CHECK_THAT( cross, Catch::Matchers::WithinAbs(-horizontal.X() * vertical.Y(), 0.0001) );
			}
		}
	}
}

SCENARIO( "Unit vector edge cases", "[Point][Unit][edge]" ) {
	GIVEN( "a zero point" ) {
		Point zero;
		WHEN( "Calculating unit vector" ) {
			Point unit = zero.Unit();
			THEN( "The result is NaN" ) {
				CHECK( std::isnan(unit.X()) );
				CHECK( std::isnan(unit.Y()) );
			}
		}
	}
	GIVEN( "a point on X axis" ) {
		Point px(5., 0.);
		WHEN( "Calculating unit vector" ) {
			Point unit = px.Unit();
			THEN( "The unit vector points along X axis" ) {
				CHECK_THAT( unit.X(), Catch::Matchers::WithinAbs(1., 0.0001) );
				CHECK_THAT( unit.Y(), Catch::Matchers::WithinAbs(0., 0.0001) );
			}
		}
	}
	GIVEN( "a point on Y axis" ) {
		Point py(0., 5.);
		WHEN( "Calculating unit vector" ) {
			Point unit = py.Unit();
			THEN( "The unit vector points along Y axis" ) {
				CHECK_THAT( unit.X(), Catch::Matchers::WithinAbs(0., 0.0001) );
				CHECK_THAT( unit.Y(), Catch::Matchers::WithinAbs(1., 0.0001) );
			}
		}
	}
}

SCENARIO( "Min/max edge cases", "[Point][min][max][edge]" ) {
	GIVEN( "two identical points" ) {
		Point a(5., 10.);
		Point b(5., 10.);
		WHEN( "Calculating min" ) {
			Point result = min(a, b);
			THEN( "The result is the same point" ) {
				CHECK( result.X() == a.X() );
				CHECK( result.Y() == a.Y() );
			}
		}
		WHEN( "Calculating max" ) {
			Point result = max(a, b);
			THEN( "The result is the same point" ) {
				CHECK( result.X() == a.X() );
				CHECK( result.Y() == a.Y() );
			}
		}
	}
	GIVEN( "points where one has larger X and smaller Y" ) {
		Point a(10., 5.);
		Point b(5., 10.);
		WHEN( "Calculating min" ) {
			Point result = min(a, b);
			THEN( "Each component is the minimum independently" ) {
				CHECK( result.X() == 5. );
				CHECK( result.Y() == 5. );
			}
		}
		WHEN( "Calculating max" ) {
			Point result = max(a, b);
			THEN( "Each component is the maximum independently" ) {
				CHECK( result.X() == 10. );
				CHECK( result.Y() == 10. );
			}
		}
	}
}

SCENARIO( "Set method", "[Point][Set]" ) {
	GIVEN( "a point" ) {
		Point p;
		WHEN( "Setting new values" ) {
			p.Set(3.14, 2.71);
			THEN( "The values are updated correctly" ) {
				CHECK_THAT( p.X(), Catch::Matchers::WithinAbs(3.14, 0.0001) );
				CHECK_THAT( p.Y(), Catch::Matchers::WithinAbs(2.71, 0.0001) );
			}
		}
	}
	GIVEN( "an existing point" ) {
		Point p(10., 20.);
		WHEN( "Overwriting with Set" ) {
			p.Set(30., 40.);
			THEN( "The old values are replaced" ) {
				CHECK_THAT( p.X(), Catch::Matchers::WithinAbs(30., 0.0001) );
				CHECK_THAT( p.Y(), Catch::Matchers::WithinAbs(40., 0.0001) );
			}
		}
	}
}

SCENARIO( "Reference accessors", "[Point][X][Y]" ) {
	GIVEN( "a point" ) {
		Point p(1., 2.);
		WHEN( "Modifying X through reference" ) {
			p.X() = 10.;
			THEN( "X is updated" ) {
				CHECK( p.X() == 10. );
				CHECK( p.Y() == 2. );
			}
		}
		WHEN( "Modifying Y through reference" ) {
			p.Y() = 20.;
			THEN( "Y is updated" ) {
				CHECK( p.X() == 1. );
				CHECK( p.Y() == 20. );
			}
		}
		WHEN( "Modifying both through references" ) {
			p.X() = 100.;
			p.Y() = 200.;
			THEN( "Both are updated" ) {
				CHECK( p.X() == 100. );
				CHECK( p.Y() == 200. );
			}
		}
	}
}

SCENARIO( "Equality with floating-point precision", "[Point][equality]" ) {
	GIVEN( "two points with very small differences" ) {
		Point a(1.0000000001, 2.);
		Point b(1.0000000002, 2.);
		WHEN( "Comparing for equality" ) {
			THEN( "They may not be equal due to exact comparison" ) {
				// Point uses exact comparison, so very small differences matter
				CHECK_FALSE( a == b );
			}
		}
	}
	GIVEN( "two exactly equal points" ) {
		Point a(1.5, 2.5);
		Point b(1.5, 2.5);
		WHEN( "Comparing for equality" ) {
			THEN( "They are equal" ) {
				CHECK( a == b );
				CHECK_FALSE( a != b );
			}
		}
	}
	GIVEN( "two points differing only in X" ) {
		Point a(1., 2.);
		Point b(1.5, 2.);
		WHEN( "Comparing for equality" ) {
			THEN( "They are not equal" ) {
				CHECK_FALSE( a == b );
				CHECK( a != b );
			}
		}
	}
	GIVEN( "two points differing only in Y" ) {
		Point a(1., 2.);
		Point b(1., 2.5);
		WHEN( "Comparing for equality" ) {
			THEN( "They are not equal" ) {
				CHECK_FALSE( a == b );
				CHECK( a != b );
			}
		}
	}
}

SCENARIO( "Scalar multiplication commutativity", "[Point][operator*]" ) {
	GIVEN( "a point and a scalar" ) {
		Point p(5., 10.);
		double s = 2.5;
		WHEN( "Multiplying point by scalar" ) {
			Point result1 = p * s;
			THEN( "The result is correct" ) {
				CHECK_THAT( result1.X(), Catch::Matchers::WithinAbs(12.5, 0.0001) );
				CHECK_THAT( result1.Y(), Catch::Matchers::WithinAbs(25., 0.0001) );
			}
		}
		WHEN( "Multiplying scalar by point" ) {
			Point result2 = s * p;
			THEN( "The result is the same (commutative)" ) {
				CHECK_THAT( result2.X(), Catch::Matchers::WithinAbs(12.5, 0.0001) );
				CHECK_THAT( result2.Y(), Catch::Matchers::WithinAbs(25., 0.0001) );
			}
		}
		WHEN( "Comparing both results" ) {
			Point result1 = p * s;
			Point result2 = s * p;
			THEN( "They are equal" ) {
				CHECK( result1 == result2 );
			}
		}
	}
}

SCENARIO( "Negative scalar operations", "[Point][operator*][operator/]" ) {
	GIVEN( "a point and a negative scalar" ) {
		Point p(5., 10.);
		double s = -2.;
		WHEN( "Multiplying by negative scalar" ) {
			Point result = p * s;
			THEN( "The result has negated coordinates scaled" ) {
				CHECK_THAT( result.X(), Catch::Matchers::WithinAbs(-10., 0.0001) );
				CHECK_THAT( result.Y(), Catch::Matchers::WithinAbs(-20., 0.0001) );
			}
		}
		WHEN( "Dividing by negative scalar" ) {
			Point result = p / s;
			THEN( "The result has negated coordinates divided" ) {
				CHECK_THAT( result.X(), Catch::Matchers::WithinAbs(-2.5, 0.0001) );
				CHECK_THAT( result.Y(), Catch::Matchers::WithinAbs(-5., 0.0001) );
			}
		}
	}
}

SCENARIO( "Cross product direction", "[Point][Cross]" ) {
	GIVEN( "two perpendicular vectors" ) {
		Point a(1., 0.);
		Point b(0., 1.);
		WHEN( "Calculating cross product a x b" ) {
			double cross = a.Cross(b);
			THEN( "The result is positive (counter-clockwise)" ) {
				CHECK( cross > 0 );
			}
		}
		WHEN( "Calculating cross product b x a" ) {
			double cross = b.Cross(a);
			THEN( "The result is negative (clockwise)" ) {
				CHECK( cross < 0 );
			}
		}
	}
	GIVEN( "parallel vectors" ) {
		Point a(2., 3.);
		Point b(4., 6.);  // b = 2 * a
		WHEN( "Calculating cross product" ) {
			double cross = a.Cross(b);
			THEN( "The result is zero" ) {
				CHECK( cross == 0. );
			}
		}
	}
	GIVEN( "anti-parallel vectors" ) {
		Point a(2., 3.);
		Point b(-4., -6.);  // b = -2 * a
		WHEN( "Calculating cross product" ) {
			double cross = a.Cross(b);
			THEN( "The result is zero" ) {
				CHECK( cross == 0. );
			}
		}
	}
}
// #endregion unit tests

// #region benchmarks
#ifdef CATCH_CONFIG_ENABLE_BENCHMARKING
TEST_CASE( "Benchmark Point::Point()", "[!benchmark][point][point]" ) {
	BENCHMARK( "Point::Point()" ) {
		return Point();
	};
	BENCHMARK( "Point::Point(x, y)" ) {
		return Point(31261.0501, 1.16026123);
	};

	Point toCopy(31261.0501, 1.16026123);
	BENCHMARK( "Point::Point(Point)" ) {
		return Point(toCopy);
	};
}

TEST_CASE( "Benchmark Point-boolean conversions", "[!benchmark][point][bool]" ) {
	Point zero;
	BENCHMARK( "Zero point" ) {
		return zero.operator bool();
	};
	BENCHMARK( "Negate zero point" ) {
		return !zero;
	};
	Point point(31261.0501, 1.16026123);
	BENCHMARK( "Nonzero point" ) {
		return point.operator bool();
	};
	BENCHMARK( "Negate nonzero point" ) {
		return !point;
	};
}

TEST_CASE( "Benchmark Point arithmetics", "[!benchmark][point][arithmetics]" ) {
	Point first = Point(5.4321, 10.987654321);
	Point second = Point(-63.57151, 0.156123);
	BENCHMARK( "Addition" ) {
		return first + second;
	};
	BENCHMARK( "Subtraction" ) {
		return first - second;
	};
	BENCHMARK( "Lane-wise multiplication" ) {
		return first * second;
	};
	double op = 36.61376183;
	BENCHMARK( "Scalar multiplication" ) {
		return first * op;
	};
	BENCHMARK( "Scalar division" ) {
		return first / op;
	};
}

TEST_CASE( "Benchmark Point vector arithmetics", "[!benchmark][point][vector arithmetics]" ) {
	Point first = Point(5.4321, 10.987654321);
	Point second = Point(-63.57151, 0.156123);
	BENCHMARK( "Dot product" ) {
		return first.Dot(second);
	};
	BENCHMARK( "Cross product" ) {
		return first.Cross(second);
	};
	BENCHMARK( "Length" ) {
		return first.Length();
	};
	BENCHMARK( "LengthSquared" ) {
		return first.LengthSquared();
	};
	BENCHMARK( "Unit" ) {
		return first.Unit();
	};
	BENCHMARK( "Distance" ) {
		return first.Distance(second);
	};
	BENCHMARK( "DistanceSquared" ) {
		return first.DistanceSquared(second);
	};
	BENCHMARK( "Lerp" ) {
		return first.Lerp(second, 0.3167116);
	};
}

TEST_CASE( "Benchmark Point helpers", "[!benchmark][point][helpers]" )
{
	Point first = Point(5.4321, 10.987654321);
	Point second = Point(-63.57151, 0.156123);
	BENCHMARK( "abs" ) {
		return abs(first);
	};
	BENCHMARK( "min" ) {
		return min(first, second);
	};
	BENCHMARK( "max" ) {
		return max(first, second);
	};
}
#endif
// #endregion benchmarks


} // test namespace
