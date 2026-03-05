/* test_information.cpp
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
#include "../../../source/Information.h"

// ... and any system includes needed for the test file.
#include "../../../source/Color.h"
#include "../../../source/Point.h"
#include "../../../source/Rectangle.h"
#include "../../../source/Swizzle.h"

namespace { // test namespace

// #region mock data

// #endregion mock data



// #region unit tests

TEST_CASE( "Information Basics", "[Information]" ) {
	using T = Information;
	SECTION( "Class Traits" ) {
		CHECK_FALSE( std::is_trivial_v<T> );
		// Information has std::map and std::set members, not standard_layout
		CHECK_FALSE( std::is_standard_layout_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
		// Has std::map and std::set members with non-trivial destructors
		CHECK_FALSE( std::is_trivially_destructible_v<T> );
	}
	SECTION( "Construction Traits" ) {
		CHECK( std::is_default_constructible_v<T> );
		// Has std::map members, not nothrow default constructible
		CHECK_FALSE( std::is_nothrow_default_constructible_v<T> );
		CHECK( std::is_copy_constructible_v<T> );
		// std::map copy is not trivial
		CHECK_FALSE( std::is_trivially_copy_constructible_v<T> );
		CHECK_FALSE( std::is_nothrow_copy_constructible_v<T> );
		CHECK( std::is_move_constructible_v<T> );
		CHECK_FALSE( std::is_trivially_move_constructible_v<T> );
		// std::map and std::set have noexcept move constructors
		CHECK( std::is_nothrow_move_constructible_v<T> );
	}
	SECTION( "Copy Traits" ) {
		CHECK( std::is_copy_assignable_v<T> );
		// std::map is not trivially copyable
		CHECK_FALSE( std::is_trivially_copyable_v<T> );
		CHECK_FALSE( std::is_trivially_copy_assignable_v<T> );
		CHECK_FALSE( std::is_nothrow_copy_assignable_v<T> );
	}
	SECTION( "Move Traits" ) {
		CHECK( std::is_move_assignable_v<T> );
		CHECK_FALSE( std::is_trivially_move_assignable_v<T> );
		// std::map and std::set have noexcept move assignment
		CHECK( std::is_nothrow_move_assignable_v<T> );
	}
}

SCENARIO( "Information default construction", "[Information][Default]" ) {
	GIVEN( "a default-constructed Information object" ) {
		Information info;
		WHEN( "checking initial state" ) {
			THEN( "no custom region is set" ) {
				CHECK_FALSE( info.HasCustomRegion() );
			}
			THEN( "strings are empty when not set" ) {
				CHECK( info.GetString("nonexistent").empty() );
			}
			THEN( "bar values are 0 when not set" ) {
				CHECK( info.BarValue("nonexistent") == 0. );
			}
			THEN( "bar segments are 1 when not set" ) {
				CHECK( info.BarSegments("nonexistent") == 1. );
			}
			THEN( "conditions are not set" ) {
				CHECK_FALSE( info.HasCondition("nonexistent") );
			}
			THEN( "sprite frame is 0 when not set" ) {
				CHECK( info.GetSpriteFrame("nonexistent") == 0.f );
			}
			THEN( "swizzle is null when not set" ) {
				CHECK( info.GetSwizzle("nonexistent") == nullptr );
			}
		}
	}
}

SCENARIO( "Information region handling", "[Information][Region]" ) {
	GIVEN( "an Information object" ) {
		Information info;
		REQUIRE_FALSE( info.HasCustomRegion() );

		WHEN( "a custom region is set" ) {
			Point center(100., 50.);
			Point dimensions(200., 100.);
			Rectangle rect(center, dimensions);
			info.SetRegion(rect);

			THEN( "HasCustomRegion returns true" ) {
				CHECK( info.HasCustomRegion() );
			}
			THEN( "GetCustomRegion returns the set region" ) {
				CHECK( info.GetCustomRegion().Center() == center );
				CHECK( info.GetCustomRegion().Dimensions() == dimensions );
			}
		}

		WHEN( "setting a region with zero dimensions" ) {
			Rectangle rect(Point(0., 0.), Point(0., 0.));
			info.SetRegion(rect);

			THEN( "HasCustomRegion returns true" ) {
				CHECK( info.HasCustomRegion() );
			}
		}
	}
}

SCENARIO( "Information string handling", "[Information][String]" ) {
	GIVEN( "an Information object" ) {
		Information info;

		WHEN( "setting a string" ) {
			info.SetString("name", "test value");

			THEN( "the string can be retrieved" ) {
				CHECK( info.GetString("name") == "test value" );
			}
		}

		WHEN( "setting multiple strings" ) {
			info.SetString("first", "value1");
			info.SetString("second", "value2");

			THEN( "each string is stored separately" ) {
				CHECK( info.GetString("first") == "value1" );
				CHECK( info.GetString("second") == "value2" );
			}
		}

		WHEN( "overwriting a string" ) {
			info.SetString("name", "original");
			info.SetString("name", "updated");

			THEN( "the updated value is returned" ) {
				CHECK( info.GetString("name") == "updated" );
			}
		}

		WHEN( "setting an empty string" ) {
			info.SetString("empty", "");

			THEN( "the empty string is stored" ) {
				CHECK( info.GetString("empty").empty() );
			}
		}

		WHEN( "getting a non-existent string" ) {
			THEN( "an empty string is returned" ) {
				CHECK( info.GetString("nonexistent").empty() );
			}
		}
	}
}

SCENARIO( "Information bar handling", "[Information][Bar]" ) {
	GIVEN( "an Information object" ) {
		Information info;

		WHEN( "setting a bar value with default segments" ) {
			info.SetBar("health", 0.75);

			THEN( "the bar value can be retrieved" ) {
				CHECK( info.BarValue("health") == 0.75 );
			}
			THEN( "segments default to 0" ) {
				CHECK( info.BarSegments("health") == 0. );
			}
		}

		WHEN( "setting a bar value with segments" ) {
			info.SetBar("shield", 0.5, 5.);

			THEN( "both value and segments are stored" ) {
				CHECK( info.BarValue("shield") == 0.5 );
				CHECK( info.BarSegments("shield") == 5. );
			}
		}

		WHEN( "setting multiple bars" ) {
			info.SetBar("health", 0.8, 4.);
			info.SetBar("fuel", 0.3, 10.);

			THEN( "each bar is stored separately" ) {
				CHECK( info.BarValue("health") == 0.8 );
				CHECK( info.BarSegments("health") == 4. );
				CHECK( info.BarValue("fuel") == 0.3 );
				CHECK( info.BarSegments("fuel") == 10. );
			}
		}

		WHEN( "overwriting a bar" ) {
			info.SetBar("energy", 0.5, 2.);
			info.SetBar("energy", 0.9, 3.);

			THEN( "the updated values are returned" ) {
				CHECK( info.BarValue("energy") == 0.9 );
				CHECK( info.BarSegments("energy") == 3. );
			}
		}

		WHEN( "setting bar value to zero" ) {
			info.SetBar("empty", 0., 0.);

			THEN( "zero values are stored" ) {
				CHECK( info.BarValue("empty") == 0. );
				CHECK( info.BarSegments("empty") == 0. );
			}
		}

		WHEN( "setting bar value to negative" ) {
			info.SetBar("negative", -0.5, -1.);

			THEN( "negative values are stored as-is" ) {
				CHECK( info.BarValue("negative") == -0.5 );
				CHECK( info.BarSegments("negative") == -1. );
			}
		}

		WHEN( "setting bar value greater than 1" ) {
			info.SetBar("overflow", 1.5, 10.);

			THEN( "the value is stored as-is" ) {
				CHECK( info.BarValue("overflow") == 1.5 );
			}
		}

		WHEN( "getting a non-existent bar" ) {
			THEN( "value returns 0" ) {
				CHECK( info.BarValue("nonexistent") == 0. );
			}
			THEN( "segments returns 1" ) {
				CHECK( info.BarSegments("nonexistent") == 1. );
			}
		}
	}
}

SCENARIO( "Information condition handling", "[Information][Condition]" ) {
	GIVEN( "an Information object" ) {
		Information info;

		WHEN( "setting a condition" ) {
			info.SetCondition("active");

			THEN( "the condition is present" ) {
				CHECK( info.HasCondition("active") );
			}
		}

		WHEN( "setting multiple conditions" ) {
			info.SetCondition("condition1");
			info.SetCondition("condition2");
			info.SetCondition("condition3");

			THEN( "all conditions are present" ) {
				CHECK( info.HasCondition("condition1") );
				CHECK( info.HasCondition("condition2") );
				CHECK( info.HasCondition("condition3") );
			}
		}

		WHEN( "setting the same condition twice" ) {
			info.SetCondition("duplicate");
			info.SetCondition("duplicate");

			THEN( "the condition is still present" ) {
				CHECK( info.HasCondition("duplicate") );
			}
		}

		WHEN( "checking a non-existent condition" ) {
			THEN( "the condition is not present" ) {
				CHECK_FALSE( info.HasCondition("nonexistent") );
			}
		}

		WHEN( "checking an empty condition" ) {
			THEN( "returns true (special case)" ) {
				CHECK( info.HasCondition("") );
			}
		}

		WHEN( "checking a negated condition that is not set" ) {
			THEN( "!nonexistent returns true" ) {
				CHECK( info.HasCondition("!nonexistent") );
			}
		}

		WHEN( "checking a negated condition that is set" ) {
			info.SetCondition("active");

			THEN( "!active returns false" ) {
				CHECK_FALSE( info.HasCondition("!active") );
			}
		}

		WHEN( "checking nested negation" ) {
			info.SetCondition("condition");

			THEN( "!condition is false" ) {
				CHECK_FALSE( info.HasCondition("!condition") );
			}
		}
	}
}

SCENARIO( "Information sprite handling", "[Information][Sprite]" ) {
	GIVEN( "an Information object" ) {
		Information info;

		WHEN( "setting a sprite with default parameters" ) {
			info.SetSprite("ship", nullptr);

			THEN( "the sprite can be retrieved" ) {
				CHECK( info.GetSprite("ship") == nullptr );
			}
			THEN( "default unit is (0, -1)" ) {
				CHECK( info.GetSpriteUnit("ship") == Point(0., -1.) );
			}
			THEN( "default frame is 0" ) {
				CHECK( info.GetSpriteFrame("ship") == 0.f );
			}
			THEN( "default swizzle is None" ) {
				CHECK( info.GetSwizzle("ship") == Swizzle::None() );
			}
		}

		WHEN( "setting a sprite with custom unit" ) {
			Point customUnit(1., 0.);
			info.SetSprite("facing", nullptr, customUnit);

			THEN( "the custom unit is returned" ) {
				CHECK( info.GetSpriteUnit("facing") == customUnit );
			}
		}

		WHEN( "setting a sprite with custom frame" ) {
			info.SetSprite("animated", nullptr, Point(0., -1.), 0.5f);

			THEN( "the custom frame is returned" ) {
				CHECK( info.GetSpriteFrame("animated") == 0.5f );
			}
		}

		WHEN( "setting a sprite with custom swizzle" ) {
			const Swizzle* customSwizzle = Swizzle::None();
			info.SetSprite("swizzled", nullptr, Point(0., -1.), 0.f, customSwizzle);

			THEN( "the custom swizzle is returned" ) {
				CHECK( info.GetSwizzle("swizzled") == customSwizzle );
			}
		}

		WHEN( "setting multiple sprites" ) {
			Point unit1(1., 0.);
			Point unit2(0., 1.);
			info.SetSprite("sprite1", nullptr, unit1, 0.1f);
			info.SetSprite("sprite2", nullptr, unit2, 0.2f);

			THEN( "each sprite has its own data" ) {
				CHECK( info.GetSpriteUnit("sprite1") == unit1 );
				CHECK( info.GetSpriteFrame("sprite1") == 0.1f );
				CHECK( info.GetSpriteUnit("sprite2") == unit2 );
				CHECK( info.GetSpriteFrame("sprite2") == 0.2f );
			}
		}

		WHEN( "overwriting a sprite" ) {
			Point original(1., 0.);
			Point updated(0., 1.);
			info.SetSprite("overlay", nullptr, original, 0.5f);
			info.SetSprite("overlay", nullptr, updated, 0.8f);

			THEN( "the updated values are returned" ) {
				CHECK( info.GetSpriteUnit("overlay") == updated );
				CHECK( info.GetSpriteFrame("overlay") == 0.8f );
			}
		}

		WHEN( "getting a non-existent sprite" ) {
			THEN( "returns pointer to empty sprite (not nullptr)" ) {
				// GetSprite returns a pointer to a static empty Sprite for non-existent sprites
				CHECK( info.GetSprite("nonexistent") != nullptr );
			}
			THEN( "returns default unit (0, -1)" ) {
				CHECK( info.GetSpriteUnit("nonexistent") == Point(0., -1.) );
			}
			THEN( "returns default frame 0" ) {
				CHECK( info.GetSpriteFrame("nonexistent") == 0.f );
			}
			THEN( "returns null swizzle" ) {
				CHECK( info.GetSwizzle("nonexistent") == nullptr );
			}
		}
	}
}

SCENARIO( "Information outline color handling", "[Information][Color]" ) {
	GIVEN( "an Information object" ) {
		Information info;

		WHEN( "setting an outline color" ) {
			Color color(0.5f, 0.3f, 0.8f, 1.f);
			info.SetOutlineColor(color);

			THEN( "the color can be retrieved" ) {
				CHECK( info.GetOutlineColor().Get()[0] == 0.5f );
				CHECK( info.GetOutlineColor().Get()[1] == 0.3f );
				CHECK( info.GetOutlineColor().Get()[2] == 0.8f );
				CHECK( info.GetOutlineColor().Get()[3] == 1.f );
			}
		}

		WHEN( "setting different colors" ) {
			Color first(1.f, 0.f, 0.f, 1.f);
			Color second(0.f, 1.f, 0.f, 0.5f);
			info.SetOutlineColor(first);
			info.SetOutlineColor(second);

			THEN( "the last color is stored" ) {
				CHECK( info.GetOutlineColor().Get()[1] == 1.f );
				CHECK( info.GetOutlineColor().Get()[3] == 0.5f );
			}
		}
	}
}

SCENARIO( "Information copying", "[Information][Copy]" ) {
	GIVEN( "an Information object with all data types" ) {
		Information original;
		original.SetRegion(Rectangle(Point(100., 50.), Point(200., 100.)));
		original.SetString("name", "test");
		original.SetBar("health", 0.75, 4.);
		original.SetCondition("active");
		original.SetSprite("ship", nullptr, Point(1., 0.), 0.5f);
		original.SetOutlineColor(Color(0.5f, 0.5f, 0.5f, 1.f));

		WHEN( "copied by constructor" ) {
			Information copy(original);

			THEN( "all data is copied correctly" ) {
				CHECK( copy.HasCustomRegion() );
				CHECK( copy.GetCustomRegion().Center() == Point(100., 50.) );
				CHECK( copy.GetString("name") == "test" );
				CHECK( copy.BarValue("health") == 0.75 );
				CHECK( copy.BarSegments("health") == 4. );
				CHECK( copy.HasCondition("active") );
				CHECK( copy.GetSpriteUnit("ship") == Point(1., 0.) );
				CHECK( copy.GetSpriteFrame("ship") == 0.5f );
				CHECK( copy.GetOutlineColor().Get()[0] == 0.5f );
			}
		}

		WHEN( "copied by assignment" ) {
			Information copy;
			copy = original;

			THEN( "all data is copied correctly" ) {
				CHECK( copy.HasCustomRegion() );
				CHECK( copy.GetString("name") == "test" );
				CHECK( copy.BarValue("health") == 0.75 );
				CHECK( copy.HasCondition("active") );
				CHECK( copy.GetSpriteUnit("ship") == Point(1., 0.) );
			}
		}
	}
}

SCENARIO( "Information edge cases", "[Information][Edge]" ) {
	GIVEN( "an Information object" ) {
		Information info;

		WHEN( "setting condition with special characters" ) {
			info.SetCondition("condition-with-dash");
			info.SetCondition("condition_with_underscore");
			info.SetCondition("condition.with.dot");

			THEN( "all conditions are stored correctly" ) {
				CHECK( info.HasCondition("condition-with-dash") );
				CHECK( info.HasCondition("condition_with_underscore") );
				CHECK( info.HasCondition("condition.with.dot") );
			}
		}

		WHEN( "setting bar with extreme values" ) {
			info.SetBar("max", 1e10, 1e10);

			THEN( "extreme values are stored" ) {
				CHECK( info.BarValue("max") == 1e10 );
				CHECK( info.BarSegments("max") == 1e10 );
			}
		}

		WHEN( "setting bar with very small values" ) {
			info.SetBar("min", 1e-10, 1e-10);

			THEN( "small values are stored" ) {
				CHECK( info.BarValue("min") == 1e-10 );
				CHECK( info.BarSegments("min") == 1e-10 );
			}
		}
	}
}

// #endregion unit tests



} // test namespace