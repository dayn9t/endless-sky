/* test_color.cpp
Copyright (c) 2024 by test contributor

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
#include "../../../source/Color.h"

// ... and any system includes needed for the test file.
#include <cmath>

namespace { // test namespace

// #region mock data

// Helper function to compare floats with tolerance
bool floatEquals(float a, float b, float epsilon = 0.0001f)
{
	return std::fabs(a - b) < epsilon;
}

// #endregion mock data



// #region unit tests
SCENARIO( "Creating a Color with grayscale constructor", "[Color][Creation]" ) {
	GIVEN( "default color with no arguments" ) {
		Color color;
		WHEN( "checking RGBA values" ) {
			THEN( "default values are white (1, 1, 1, 1)" ) {
				CHECK( color.Get()[0] == 1.f );
				CHECK( color.Get()[1] == 1.f );
				CHECK( color.Get()[2] == 1.f );
				CHECK( color.Get()[3] == 1.f );
			}
			THEN( "IsLoaded is false" ) {
				CHECK( color.IsLoaded() == false );
			}
		}
	}

	GIVEN( "a grayscale color with intensity only" ) {
		Color color(0.5f);
		WHEN( "checking RGBA values" ) {
			THEN( "RGB are all the same intensity, alpha is 1" ) {
				CHECK( color.Get()[0] == 0.5f );
				CHECK( color.Get()[1] == 0.5f );
				CHECK( color.Get()[2] == 0.5f );
				CHECK( color.Get()[3] == 1.f );
			}
			THEN( "IsLoaded is false" ) {
				CHECK( color.IsLoaded() == false );
			}
		}
	}

	GIVEN( "a grayscale color with intensity and alpha" ) {
		Color color(0.7f, 0.3f);
		WHEN( "checking RGBA values" ) {
			THEN( "RGB are all the same intensity, alpha is set" ) {
				CHECK( color.Get()[0] == 0.7f );
				CHECK( color.Get()[1] == 0.7f );
				CHECK( color.Get()[2] == 0.7f );
				CHECK( color.Get()[3] == 0.3f );
			}
			THEN( "IsLoaded is false" ) {
				CHECK( color.IsLoaded() == false );
			}
		}
	}
}

SCENARIO( "Creating a Color with RGB constructor", "[Color][Creation]" ) {
	GIVEN( "an RGB color with no alpha" ) {
		Color color(0.2f, 0.4f, 0.6f);
		WHEN( "checking RGBA values" ) {
			THEN( "RGB values are set, alpha defaults to 1" ) {
				CHECK( color.Get()[0] == 0.2f );
				CHECK( color.Get()[1] == 0.4f );
				CHECK( color.Get()[2] == 0.6f );
				CHECK( color.Get()[3] == 1.f );
			}
			THEN( "IsLoaded is true" ) {
				CHECK( color.IsLoaded() == true );
			}
		}
	}

	GIVEN( "an RGBA color with explicit alpha" ) {
		Color color(0.1f, 0.2f, 0.3f, 0.5f);
		WHEN( "checking RGBA values" ) {
			THEN( "all values are set correctly" ) {
				CHECK( color.Get()[0] == 0.1f );
				CHECK( color.Get()[1] == 0.2f );
				CHECK( color.Get()[2] == 0.3f );
				CHECK( color.Get()[3] == 0.5f );
			}
			THEN( "IsLoaded is true" ) {
				CHECK( color.IsLoaded() == true );
			}
		}
	}

	GIVEN( "a fully transparent RGBA color" ) {
		Color color(0.5f, 0.5f, 0.5f, 0.f);
		WHEN( "checking RGBA values" ) {
			THEN( "alpha is zero" ) {
				CHECK( color.Get()[3] == 0.f );
			}
		}
	}
}

SCENARIO( "Comparing colors with equality operators", "[Color][Comparison]" ) {
	GIVEN( "two identical colors" ) {
		Color c1(0.5f, 0.3f, 0.7f, 0.9f);
		Color c2(0.5f, 0.3f, 0.7f, 0.9f);
		WHEN( "comparing with ==" ) {
			THEN( "they are equal" ) {
				CHECK( c1 == c2 );
				CHECK( c2 == c1 );
			}
		}
		WHEN( "comparing with !=" ) {
			THEN( "they are not different" ) {
				CHECK_FALSE( c1 != c2 );
			}
		}
	}

	GIVEN( "two colors with different R values" ) {
		Color c1(0.5f, 0.3f, 0.7f, 0.9f);
		Color c2(0.6f, 0.3f, 0.7f, 0.9f);
		WHEN( "comparing" ) {
			THEN( "they are not equal" ) {
				CHECK( c1 != c2 );
				CHECK_FALSE( c1 == c2 );
			}
		}
	}

	GIVEN( "two colors with different G values" ) {
		Color c1(0.5f, 0.3f, 0.7f, 0.9f);
		Color c2(0.5f, 0.4f, 0.7f, 0.9f);
		WHEN( "comparing" ) {
			THEN( "they are not equal" ) {
				CHECK( c1 != c2 );
			}
		}
	}

	GIVEN( "two colors with different B values" ) {
		Color c1(0.5f, 0.3f, 0.7f, 0.9f);
		Color c2(0.5f, 0.3f, 0.8f, 0.9f);
		WHEN( "comparing" ) {
			THEN( "they are not equal" ) {
				CHECK( c1 != c2 );
			}
		}
	}

	GIVEN( "two colors with different alpha values" ) {
		Color c1(0.5f, 0.3f, 0.7f, 0.9f);
		Color c2(0.5f, 0.3f, 0.7f, 0.8f);
		WHEN( "comparing" ) {
			THEN( "they are not equal" ) {
				CHECK( c1 != c2 );
			}
		}
	}

	GIVEN( "two grayscale colors with same values" ) {
		Color c1(0.5f, 0.8f);
		Color c2(0.5f, 0.8f);
		WHEN( "comparing" ) {
			THEN( "they are equal" ) {
				CHECK( c1 == c2 );
			}
		}
	}
}

SCENARIO( "Loading color values", "[Color][Load]" ) {
	GIVEN( "a default constructed color" ) {
		Color color;
		REQUIRE( color.IsLoaded() == false );
		WHEN( "Load() is called" ) {
			color.Load(0.25, 0.5, 0.75, 0.9);
			THEN( "values are set correctly" ) {
				CHECK( floatEquals(color.Get()[0], 0.25f) );
				CHECK( floatEquals(color.Get()[1], 0.5f) );
				CHECK( floatEquals(color.Get()[2], 0.75f) );
				CHECK( floatEquals(color.Get()[3], 0.9f) );
			}
			THEN( "IsLoaded is true" ) {
				CHECK( color.IsLoaded() == true );
			}
		}
	}

	GIVEN( "an already loaded color" ) {
		Color color(1.f, 0.f, 0.f, 1.f);
		REQUIRE( color.IsLoaded() == true );
		WHEN( "Load() is called with new values" ) {
			color.Load(0.1, 0.2, 0.3, 0.4);
			THEN( "values are updated" ) {
				CHECK( floatEquals(color.Get()[0], 0.1f) );
				CHECK( floatEquals(color.Get()[1], 0.2f) );
				CHECK( floatEquals(color.Get()[2], 0.3f) );
				CHECK( floatEquals(color.Get()[3], 0.4f) );
			}
		}
	}

	GIVEN( "a grayscale color (not loaded)" ) {
		Color color(0.5f);
		REQUIRE( color.IsLoaded() == false );
		WHEN( "Load() is called" ) {
			color.Load(0.1, 0.2, 0.3, 0.4);
			THEN( "IsLoaded becomes true" ) {
				CHECK( color.IsLoaded() == true );
			}
		}
	}
}

SCENARIO( "Using Opaque() method", "[Color][Manipulation]" ) {
	GIVEN( "a semi-transparent color" ) {
		Color color(0.5f, 0.6f, 0.7f, 0.4f);
		WHEN( "calling Opaque()" ) {
			Color opaque = color.Opaque();
			THEN( "alpha becomes 1, RGB unchanged" ) {
				CHECK( opaque.Get()[0] == 0.5f );
				CHECK( opaque.Get()[1] == 0.6f );
				CHECK( opaque.Get()[2] == 0.7f );
				CHECK( opaque.Get()[3] == 1.f );
			}
			THEN( "original color is unchanged" ) {
				CHECK( color.Get()[3] == 0.4f );
			}
		}
	}

	GIVEN( "a fully opaque color" ) {
		Color color(1.f, 0.5f, 0.f, 1.f);
		WHEN( "calling Opaque()" ) {
			Color opaque = color.Opaque();
			THEN( "alpha remains 1" ) {
				CHECK( opaque.Get()[3] == 1.f );
			}
		}
	}

	GIVEN( "a fully transparent color" ) {
		Color color(0.f, 0.f, 0.f, 0.f);
		WHEN( "calling Opaque()" ) {
			Color opaque = color.Opaque();
			THEN( "alpha becomes 1" ) {
				CHECK( opaque.Get()[3] == 1.f );
			}
		}
	}
}

SCENARIO( "Using Transparent() method", "[Color][Manipulation]" ) {
	GIVEN( "an opaque color" ) {
		Color color(1.f, 0.8f, 0.6f, 1.f);
		WHEN( "calling Transparent(0.5)" ) {
			Color transparent = color.Transparent(0.5f);
			THEN( "RGB are multiplied by alpha (premultiplied)" ) {
				CHECK( transparent.Get()[0] == 0.5f );
				CHECK( transparent.Get()[1] == 0.4f );
				CHECK( transparent.Get()[2] == 0.3f );
				CHECK( transparent.Get()[3] == 0.5f );
			}
			THEN( "original color is unchanged" ) {
				CHECK( color.Get()[0] == 1.f );
				CHECK( color.Get()[3] == 1.f );
			}
		}
	}

	GIVEN( "an opaque white color" ) {
		Color color(1.f, 1.f, 1.f, 1.f);
		WHEN( "calling Transparent(0.3)" ) {
			Color transparent = color.Transparent(0.3f);
			THEN( "result is semi-transparent white" ) {
				CHECK( transparent.Get()[0] == 0.3f );
				CHECK( transparent.Get()[1] == 0.3f );
				CHECK( transparent.Get()[2] == 0.3f );
				CHECK( transparent.Get()[3] == 0.3f );
			}
		}
	}

	GIVEN( "a color with Transparent(0)" ) {
		Color color(0.5f, 0.5f, 0.5f, 1.f);
		WHEN( "calling Transparent(0)" ) {
			Color transparent = color.Transparent(0.f);
			THEN( "result is fully transparent" ) {
				CHECK( transparent.Get()[0] == 0.f );
				CHECK( transparent.Get()[1] == 0.f );
				CHECK( transparent.Get()[2] == 0.f );
				CHECK( transparent.Get()[3] == 0.f );
			}
		}
	}
}

SCENARIO( "Using Additive() method", "[Color][Manipulation]" ) {
	GIVEN( "an opaque color" ) {
		Color color(1.f, 0.5f, 0.25f, 1.f);
		WHEN( "calling Additive(0.6)" ) {
			Color additive = color.Additive(0.6f);
			THEN( "RGB are multiplied by alpha, alpha becomes 0" ) {
				CHECK( floatEquals(additive.Get()[0], 0.6f) );
				CHECK( floatEquals(additive.Get()[1], 0.3f) );
				CHECK( floatEquals(additive.Get()[2], 0.15f) );
				CHECK( additive.Get()[3] == 0.f );
			}
		}
	}

	GIVEN( "a white color for additive blending" ) {
		Color color(1.f, 1.f, 1.f, 1.f);
		WHEN( "calling Additive(0.8)" ) {
			Color additive = color.Additive(0.8f);
			THEN( "result is additive white" ) {
				CHECK( additive.Get()[0] == 0.8f );
				CHECK( additive.Get()[1] == 0.8f );
				CHECK( additive.Get()[2] == 0.8f );
				CHECK( additive.Get()[3] == 0.f );
			}
		}
	}

	GIVEN( "a color with Additive(0)" ) {
		Color color(1.f, 1.f, 1.f, 1.f);
		WHEN( "calling Additive(0)" ) {
			Color additive = color.Additive(0.f);
			THEN( "result is fully black with alpha 0" ) {
				CHECK( additive.Get()[0] == 0.f );
				CHECK( additive.Get()[1] == 0.f );
				CHECK( additive.Get()[2] == 0.f );
				CHECK( additive.Get()[3] == 0.f );
			}
		}
	}
}

SCENARIO( "Using static Combine() method", "[Color][Static]" ) {
	GIVEN( "two colors to combine" ) {
		Color c1(1.f, 0.5f, 0.25f, 1.f);
		Color c2(0.f, 0.5f, 0.75f, 0.5f);
		WHEN( "combining with equal weights" ) {
			Color result = Color::Combine(0.5f, c1, 0.5f, c2);
			THEN( "result is average of both colors" ) {
				CHECK( floatEquals(result.Get()[0], 0.5f) );
				CHECK( floatEquals(result.Get()[1], 0.5f) );
				CHECK( floatEquals(result.Get()[2], 0.5f) );
				CHECK( floatEquals(result.Get()[3], 0.75f) );
			}
		}
	}

	GIVEN( "two colors with different weights" ) {
		Color c1(1.f, 1.f, 1.f, 1.f);
		Color c2(0.f, 0.f, 0.f, 0.f);
		WHEN( "combining with 75% first, 25% second" ) {
			Color result = Color::Combine(0.75f, c1, 0.25f, c2);
			THEN( "result is weighted toward first color" ) {
				CHECK( floatEquals(result.Get()[0], 0.75f) );
				CHECK( floatEquals(result.Get()[1], 0.75f) );
				CHECK( floatEquals(result.Get()[2], 0.75f) );
				CHECK( floatEquals(result.Get()[3], 0.75f) );
			}
		}
	}

	GIVEN( "only first color contributes" ) {
		Color c1(0.8f, 0.6f, 0.4f, 0.2f);
		Color c2(1.f, 1.f, 1.f, 1.f);
		WHEN( "combining with weight 1 for first, 0 for second" ) {
			Color result = Color::Combine(1.f, c1, 0.f, c2);
			THEN( "result equals first color" ) {
				CHECK( result.Get()[0] == 0.8f );
				CHECK( result.Get()[1] == 0.6f );
				CHECK( result.Get()[2] == 0.4f );
				CHECK( result.Get()[3] == 0.2f );
			}
		}
	}

	GIVEN( "only second color contributes" ) {
		Color c1(1.f, 1.f, 1.f, 1.f);
		Color c2(0.3f, 0.5f, 0.7f, 0.9f);
		WHEN( "combining with weight 0 for first, 1 for second" ) {
			Color result = Color::Combine(0.f, c1, 1.f, c2);
			THEN( "result equals second color" ) {
				CHECK( result.Get()[0] == 0.3f );
				CHECK( result.Get()[1] == 0.5f );
				CHECK( result.Get()[2] == 0.7f );
				CHECK( result.Get()[3] == 0.9f );
			}
		}
	}
}

SCENARIO( "Using static Multiply() method", "[Color][Static]" ) {
	GIVEN( "a color and a scalar" ) {
		Color base(1.f, 0.8f, 0.6f, 0.4f);
		WHEN( "multiplying by 0.5" ) {
			Color result = Color::Multiply(0.5f, base);
			THEN( "all components are halved" ) {
				CHECK( result.Get()[0] == 0.5f );
				CHECK( result.Get()[1] == 0.4f );
				CHECK( result.Get()[2] == 0.3f );
				CHECK( result.Get()[3] == 0.2f );
			}
			THEN( "original color is unchanged" ) {
				CHECK( base.Get()[0] == 1.f );
				CHECK( base.Get()[1] == 0.8f );
			}
		}
	}

	GIVEN( "a color and scalar 2.0" ) {
		Color base(0.5f, 0.5f, 0.5f, 0.5f);
		WHEN( "multiplying by 2" ) {
			Color result = Color::Multiply(2.f, base);
			THEN( "all components are doubled" ) {
				CHECK( result.Get()[0] == 1.f );
				CHECK( result.Get()[1] == 1.f );
				CHECK( result.Get()[2] == 1.f );
				CHECK( result.Get()[3] == 1.f );
			}
		}
	}

	GIVEN( "a color and scalar 0" ) {
		Color base(1.f, 1.f, 1.f, 1.f);
		WHEN( "multiplying by 0" ) {
			Color result = Color::Multiply(0.f, base);
			THEN( "all components become 0" ) {
				CHECK( result.Get()[0] == 0.f );
				CHECK( result.Get()[1] == 0.f );
				CHECK( result.Get()[2] == 0.f );
				CHECK( result.Get()[3] == 0.f );
			}
		}
	}

	GIVEN( "a color and scalar 1.0" ) {
		Color base(0.7f, 0.5f, 0.3f, 0.9f);
		WHEN( "multiplying by 1" ) {
			Color result = Color::Multiply(1.f, base);
			THEN( "color is unchanged" ) {
				CHECK( result.Get()[0] == 0.7f );
				CHECK( result.Get()[1] == 0.5f );
				CHECK( result.Get()[2] == 0.3f );
				CHECK( result.Get()[3] == 0.9f );
			}
		}
	}
}

SCENARIO( "Using Get() method", "[Color][Access]" ) {
	GIVEN( "a color" ) {
		Color color(0.1f, 0.2f, 0.3f, 0.4f);
		WHEN( "calling Get()" ) {
			const float* values = color.Get();
			THEN( "pointer to internal array is returned" ) {
				CHECK( values[0] == 0.1f );
				CHECK( values[1] == 0.2f );
				CHECK( values[2] == 0.3f );
				CHECK( values[3] == 0.4f );
			}
		}
	}
}

SCENARIO( "Using TrueName methods", "[Color][TrueName]" ) {
	GIVEN( "a color with no true name" ) {
		Color color;
		WHEN( "checking TrueName" ) {
			THEN( "returns empty string" ) {
				CHECK( color.TrueName().empty() );
			}
		}
		WHEN( "setting a true name" ) {
			color.SetTrueName("myColor");
			THEN( "TrueName returns the set name" ) {
				CHECK( color.TrueName() == "myColor" );
			}
		}
	}

	GIVEN( "a color with true name set" ) {
		Color color;
		color.SetTrueName("testColor");
		WHEN( "setting a different true name" ) {
			color.SetTrueName("newColor");
			THEN( "true name is updated" ) {
				CHECK( color.TrueName() == "newColor" );
			}
		}
	}
}
// #endregion unit tests



} // test namespace