/* test_swizzle.cpp
Copyright (c) 2025 by test contributor

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
#include "../../../source/Swizzle.h"

// ... and any system includes needed for the test file.
#include "../../../source/Color.h"
#include "datanode-factory.h"

#include <array>
#include <cmath>
#include <string>

namespace { // test namespace

// #region mock data

// Helper function to compare floats with tolerance
bool floatEquals(float a, float b, float epsilon = 0.0001f)
{
	return std::fabs(a - b) < epsilon;
}

// Helper function to compare two 4x4 matrices
bool matrixEquals(const float* m1, const float* m2, float epsilon = 0.0001f)
{
	for(int i = 0; i < 16; ++i)
	{
		if(!floatEquals(m1[i], m2[i], epsilon))
			return false;
	}
	return true;
}

// Identity matrix for comparison
constexpr std::array<float, 16> IDENTITY_MATRIX = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

// #endregion mock data



// #region unit tests

SCENARIO( "Creating a default Swizzle", "[Swizzle][Creation]" ) {
	GIVEN( "a default constructed Swizzle" ) {
		Swizzle swizzle;
		WHEN( "checking initial state" ) {
			THEN( "IsLoaded returns false" ) {
				CHECK( swizzle.IsLoaded() == false );
			}
			THEN( "IsIdentity returns true (default is identity)" ) {
				CHECK( swizzle.IsIdentity() == true );
			}
			THEN( "Name returns empty string" ) {
				CHECK( swizzle.Name().empty() );
			}
			THEN( "OverrideMask returns false" ) {
				CHECK( swizzle.OverrideMask() == false );
			}
			THEN( "MatrixPtr returns identity matrix" ) {
				const float* matrix = swizzle.MatrixPtr();
				CHECK( matrixEquals(matrix, IDENTITY_MATRIX.data()) );
			}
		}
	}
}

SCENARIO( "Loading a Swizzle from DataNode", "[Swizzle][Load]" ) {
	GIVEN( "a default constructed Swizzle" ) {
		Swizzle swizzle;
		REQUIRE( swizzle.IsLoaded() == false );

		WHEN( "loading with just a name" ) {
			auto node = AsDataNode("swizzle \"testSwizzle\"");
			swizzle.Load(node);

			THEN( "IsLoaded becomes true" ) {
				CHECK( swizzle.IsLoaded() == true );
			}
			THEN( "Name is set correctly" ) {
				CHECK( swizzle.Name() == "testSwizzle" );
			}
			THEN( "IsIdentity remains true (no channel modifications)" ) {
				CHECK( swizzle.IsIdentity() == true );
			}
			THEN( "OverrideMask remains false" ) {
				CHECK( swizzle.OverrideMask() == false );
			}
		}
	}

	GIVEN( "a swizzle with red channel modification" ) {
		auto node = AsDataNode(R"(swizzle "redModify"
	red 2 0 0 0)");
		Swizzle swizzle;
		swizzle.Load(node);

		WHEN( "checking the state after load" ) {
			THEN( "Name is set correctly" ) {
				CHECK( swizzle.Name() == "redModify" );
			}
			THEN( "IsIdentity is false (matrix was modified)" ) {
				CHECK( swizzle.IsIdentity() == false );
			}
			THEN( "Red channel row is modified" ) {
				const float* matrix = swizzle.MatrixPtr();
				CHECK( matrix[0] == 2.0f );  // red row, column 0
				CHECK( matrix[1] == 0.0f );  // red row, column 1
				CHECK( matrix[2] == 0.0f );  // red row, column 2
				CHECK( matrix[3] == 0.0f );  // red row, column 3
			}
			THEN( "Other channels remain identity" ) {
				const float* matrix = swizzle.MatrixPtr();
				CHECK( matrix[4] == 0.0f );  // green row, column 0
				CHECK( matrix[5] == 1.0f );  // green row, column 1
				CHECK( matrix[6] == 0.0f );  // green row, column 2
				CHECK( matrix[7] == 0.0f );  // green row, column 3
			}
		}
	}

	GIVEN( "a swizzle with multiple channel modifications" ) {
		auto node = AsDataNode(R"(swizzle "multiModify"
	red 0 1 0 0
	green 0 0 1 0
	blue 0 0 0 1
	alpha 1 0 0 0)");
		Swizzle swizzle;
		swizzle.Load(node);

		WHEN( "checking the state after load" ) {
			THEN( "Red channel reads from green" ) {
				const float* matrix = swizzle.MatrixPtr();
				CHECK( matrix[0] == 0.0f );
				CHECK( matrix[1] == 1.0f );
				CHECK( matrix[2] == 0.0f );
				CHECK( matrix[3] == 0.0f );
			}
			THEN( "Green channel reads from blue" ) {
				const float* matrix = swizzle.MatrixPtr();
				CHECK( matrix[4] == 0.0f );
				CHECK( matrix[5] == 0.0f );
				CHECK( matrix[6] == 1.0f );
				CHECK( matrix[7] == 0.0f );
			}
			THEN( "Blue channel reads from alpha" ) {
				const float* matrix = swizzle.MatrixPtr();
				CHECK( matrix[8] == 0.0f );
				CHECK( matrix[9] == 0.0f );
				CHECK( matrix[10] == 0.0f );
				CHECK( matrix[11] == 1.0f );
			}
			THEN( "Alpha channel reads from red" ) {
				const float* matrix = swizzle.MatrixPtr();
				CHECK( matrix[12] == 1.0f );
				CHECK( matrix[13] == 0.0f );
				CHECK( matrix[14] == 0.0f );
				CHECK( matrix[15] == 0.0f );
			}
		}
	}

	GIVEN( "a swizzle with override flag" ) {
		auto node = AsDataNode(R"(swizzle "overrideTest"
	override
	red 0.5 0.5 0 0)");
		Swizzle swizzle;
		swizzle.Load(node);

		WHEN( "checking the state after load" ) {
			THEN( "OverrideMask is true" ) {
				CHECK( swizzle.OverrideMask() == true );
			}
		}
	}

	GIVEN( "a swizzle with partial channel specification" ) {
		auto node = AsDataNode(R"(swizzle "partialChannel"
	red 0.5)");
		Swizzle swizzle;
		swizzle.Load(node);

		WHEN( "checking the matrix" ) {
			THEN( "only specified values are set, rest remain from identity" ) {
				const float* matrix = swizzle.MatrixPtr();
				CHECK( matrix[0] == 0.5f );  // set
				CHECK( matrix[1] == 0.0f );  // not set, remains 0
				CHECK( matrix[2] == 0.0f );  // not set, remains 0
				CHECK( matrix[3] == 0.0f );  // not set, remains 0
			}
		}
	}
}

SCENARIO( "Testing Swizzle::IsIdentity", "[Swizzle][IsIdentity]" ) {
	GIVEN( "a default Swizzle" ) {
		Swizzle swizzle;
		WHEN( "checking IsIdentity" ) {
			THEN( "returns true for default constructed" ) {
				CHECK( swizzle.IsIdentity() == true );
			}
		}
	}

	GIVEN( "a Swizzle loaded with identity matrix values" ) {
		auto node = AsDataNode(R"(swizzle "stillIdentity"
	red 1 0 0 0
	green 0 1 0 0
	blue 0 0 1 0
	alpha 0 0 0 1)");
		Swizzle swizzle;
		swizzle.Load(node);

		WHEN( "checking IsIdentity" ) {
			THEN( "returns true because matrix equals identity" ) {
				CHECK( swizzle.IsIdentity() == true );
			}
		}
	}

	GIVEN( "a Swizzle with modified matrix" ) {
		auto node = AsDataNode(R"(swizzle "notIdentity"
	red 0.5 0.5 0 0)");
		Swizzle swizzle;
		swizzle.Load(node);

		WHEN( "checking IsIdentity" ) {
			THEN( "returns false because matrix differs from identity" ) {
				CHECK( swizzle.IsIdentity() == false );
			}
		}
	}
}

SCENARIO( "Testing Swizzle::MatrixPtr", "[Swizzle][MatrixPtr]" ) {
	GIVEN( "a loaded Swizzle" ) {
		auto node = AsDataNode(R"(swizzle "matrixTest"
	red 0.1 0.2 0.3 0.4
	green 0.5 0.6 0.7 0.8
	blue 0.9 1.0 0.0 0.1
	alpha 0.2 0.3 0.4 0.5)");
		Swizzle swizzle;
		swizzle.Load(node);

		WHEN( "getting the matrix pointer" ) {
			const float* matrix = swizzle.MatrixPtr();

			THEN( "red row is correct" ) {
				CHECK( floatEquals(matrix[0], 0.1f) );
				CHECK( floatEquals(matrix[1], 0.2f) );
				CHECK( floatEquals(matrix[2], 0.3f) );
				CHECK( floatEquals(matrix[3], 0.4f) );
			}
			THEN( "green row is correct" ) {
				CHECK( floatEquals(matrix[4], 0.5f) );
				CHECK( floatEquals(matrix[5], 0.6f) );
				CHECK( floatEquals(matrix[6], 0.7f) );
				CHECK( floatEquals(matrix[7], 0.8f) );
			}
			THEN( "blue row is correct" ) {
				CHECK( floatEquals(matrix[8], 0.9f) );
				CHECK( floatEquals(matrix[9], 1.0f) );
				CHECK( floatEquals(matrix[10], 0.0f) );
				CHECK( floatEquals(matrix[11], 0.1f) );
			}
			THEN( "alpha row is correct" ) {
				CHECK( floatEquals(matrix[12], 0.2f) );
				CHECK( floatEquals(matrix[13], 0.3f) );
				CHECK( floatEquals(matrix[14], 0.4f) );
				CHECK( floatEquals(matrix[15], 0.5f) );
			}
		}
	}
}

SCENARIO( "Testing Swizzle::Apply with identity matrix", "[Swizzle][Apply]" ) {
	GIVEN( "a default Swizzle (identity)" ) {
		Swizzle swizzle;
		REQUIRE( swizzle.IsIdentity() == true );

		WHEN( "applying to a color" ) {
			Color original(0.5f, 0.3f, 0.7f, 0.9f);
			Color result = swizzle.Apply(original);

			THEN( "color is unchanged" ) {
				const float* orig = original.Get();
				const float* res = result.Get();
				CHECK( floatEquals(res[0], orig[0]) );
				CHECK( floatEquals(res[1], orig[1]) );
				CHECK( floatEquals(res[2], orig[2]) );
				CHECK( floatEquals(res[3], orig[3]) );
			}
		}

		WHEN( "applying to white color" ) {
			Color white(1.0f, 1.0f, 1.0f, 1.0f);
			Color result = swizzle.Apply(white);

			THEN( "white is unchanged" ) {
				CHECK( result.Get()[0] == 1.0f );
				CHECK( result.Get()[1] == 1.0f );
				CHECK( result.Get()[2] == 1.0f );
				CHECK( result.Get()[3] == 1.0f );
			}
		}

		WHEN( "applying to black color" ) {
			Color black(0.0f, 0.0f, 0.0f, 1.0f);
			Color result = swizzle.Apply(black);

			THEN( "black is unchanged" ) {
				CHECK( result.Get()[0] == 0.0f );
				CHECK( result.Get()[1] == 0.0f );
				CHECK( result.Get()[2] == 0.0f );
				CHECK( result.Get()[3] == 1.0f );
			}
		}
	}
}

SCENARIO( "Testing Swizzle::Apply with channel swap", "[Swizzle][Apply]" ) {
	GIVEN( "a Swizzle that swaps red and blue channels" ) {
		auto node = AsDataNode(R"(swizzle "swapRedBlue"
	red 0 0 1 0
	blue 1 0 0 0)");
		Swizzle swizzle;
		swizzle.Load(node);

		WHEN( "applying to a color with distinct RGB values" ) {
			Color original(0.1f, 0.2f, 0.3f, 0.4f);
			Color result = swizzle.Apply(original);

			THEN( "red and blue are swapped, green unchanged" ) {
				CHECK( floatEquals(result.Get()[0], 0.3f) );  // red gets blue
				CHECK( floatEquals(result.Get()[1], 0.2f) );  // green unchanged
				CHECK( floatEquals(result.Get()[2], 0.1f) );  // blue gets red
				CHECK( floatEquals(result.Get()[3], 0.4f) );  // alpha unchanged
			}
		}
	}

	GIVEN( "a Swizzle that rotates all RGB channels" ) {
		auto node = AsDataNode(R"(swizzle "rotateRGB"
	red 0 0 1 0
	green 1 0 0 0
	blue 0 1 0 0)");
		Swizzle swizzle;
		swizzle.Load(node);

		WHEN( "applying to a color" ) {
			Color original(0.1f, 0.2f, 0.3f, 0.5f);
			Color result = swizzle.Apply(original);

			THEN( "channels are rotated: R<-B, G<-R, B<-G" ) {
				CHECK( floatEquals(result.Get()[0], 0.3f) );  // red <- blue
				CHECK( floatEquals(result.Get()[1], 0.1f) );  // green <- red
				CHECK( floatEquals(result.Get()[2], 0.2f) );  // blue <- green
				CHECK( floatEquals(result.Get()[3], 0.5f) );  // alpha unchanged
			}
		}
	}
}

SCENARIO( "Testing Swizzle::Apply with scaling", "[Swizzle][Apply]" ) {
	GIVEN( "a Swizzle that scales red channel" ) {
		auto node = AsDataNode(R"(swizzle "scaleRed"
	red 2 0 0 0)");
		Swizzle swizzle;
		swizzle.Load(node);

		WHEN( "applying to a color" ) {
			Color original(0.3f, 0.4f, 0.5f, 0.6f);
			Color result = swizzle.Apply(original);

			THEN( "red is doubled, other channels unchanged" ) {
				CHECK( floatEquals(result.Get()[0], 0.6f) );  // red * 2
				CHECK( floatEquals(result.Get()[1], 0.4f) );  // green unchanged
				CHECK( floatEquals(result.Get()[2], 0.5f) );  // blue unchanged
				CHECK( floatEquals(result.Get()[3], 0.6f) );  // alpha unchanged
			}
		}
	}

	GIVEN( "a Swizzle that zeroes out all channels" ) {
		auto node = AsDataNode(R"(swizzle "zeroAll"
	red 0 0 0 0
	green 0 0 0 0
	blue 0 0 0 0
	alpha 0 0 0 0)");
		Swizzle swizzle;
		swizzle.Load(node);

		WHEN( "applying to any color" ) {
			Color original(1.0f, 1.0f, 1.0f, 1.0f);
			Color result = swizzle.Apply(original);

			THEN( "all channels become zero" ) {
				CHECK( floatEquals(result.Get()[0], 0.0f) );
				CHECK( floatEquals(result.Get()[1], 0.0f) );
				CHECK( floatEquals(result.Get()[2], 0.0f) );
				CHECK( floatEquals(result.Get()[3], 0.0f) );
			}
		}
	}
}

SCENARIO( "Testing Swizzle::Apply with additive operations", "[Swizzle][Apply]" ) {
	GIVEN( "a Swizzle that adds channels together" ) {
		auto node = AsDataNode(R"(swizzle "addChannels"
	red 0.33 0.33 0.33 0)");
		Swizzle swizzle;
		swizzle.Load(node);

		WHEN( "applying to a color" ) {
			Color original(1.0f, 1.0f, 1.0f, 1.0f);
			Color result = swizzle.Apply(original);

			THEN( "red becomes average of RGB" ) {
				CHECK( floatEquals(result.Get()[0], 0.99f) );  // 0.33*(1+1+1)
				CHECK( floatEquals(result.Get()[1], 1.0f) );  // green unchanged
				CHECK( floatEquals(result.Get()[2], 1.0f) );  // blue unchanged
				CHECK( floatEquals(result.Get()[3], 1.0f) );  // alpha unchanged
			}
		}
	}

	GIVEN( "a Swizzle that sets alpha from RGB average" ) {
		auto node = AsDataNode(R"(swizzle "alphaFromLuminance"
	alpha 0.299 0.587 0.114 0)");
		Swizzle swizzle;
		swizzle.Load(node);

		WHEN( "applying to a color" ) {
			Color original(1.0f, 1.0f, 1.0f, 0.5f);
			Color result = swizzle.Apply(original);

			THEN( "alpha becomes luminance value" ) {
				CHECK( floatEquals(result.Get()[0], 1.0f) );  // red unchanged
				CHECK( floatEquals(result.Get()[1], 1.0f) );  // green unchanged
				CHECK( floatEquals(result.Get()[2], 1.0f) );  // blue unchanged
				CHECK( floatEquals(result.Get()[3], 1.0f) );  // luminance of white
			}
		}
	}
}

SCENARIO( "Testing Swizzle::None static method", "[Swizzle][None]" ) {
	GIVEN( "the static None swizzle" ) {
		const Swizzle* none = Swizzle::None();

		WHEN( "checking its properties" ) {
			THEN( "it is not null" ) {
				CHECK( none != nullptr );
			}
			THEN( "IsIdentity returns true" ) {
				CHECK( none->IsIdentity() == true );
			}
			THEN( "IsLoaded returns true" ) {
				CHECK( none->IsLoaded() == true );
			}
			THEN( "OverrideMask returns true" ) {
				CHECK( none->OverrideMask() == true );
			}
			THEN( "MatrixPtr returns identity matrix" ) {
				CHECK( matrixEquals(none->MatrixPtr(), IDENTITY_MATRIX.data()) );
			}
		}

		WHEN( "applying to a color" ) {
			Color original(0.5f, 0.3f, 0.7f, 0.9f);
			Color result = none->Apply(original);

			THEN( "color is unchanged" ) {
				CHECK( floatEquals(result.Get()[0], 0.5f) );
				CHECK( floatEquals(result.Get()[1], 0.3f) );
				CHECK( floatEquals(result.Get()[2], 0.7f) );
				CHECK( floatEquals(result.Get()[3], 0.9f) );
			}
		}

		WHEN( "calling None() multiple times" ) {
			const Swizzle* none2 = Swizzle::None();

			THEN( "returns the same pointer (singleton)" ) {
				CHECK( none == none2 );
			}
		}
	}
}

SCENARIO( "Testing Swizzle::OverrideMask", "[Swizzle][OverrideMask]" ) {
	GIVEN( "a default Swizzle" ) {
		Swizzle swizzle;
		WHEN( "checking OverrideMask" ) {
			THEN( "returns false by default" ) {
				CHECK( swizzle.OverrideMask() == false );
			}
		}
	}

	GIVEN( "a Swizzle loaded with override keyword" ) {
		auto node = AsDataNode(R"(swizzle "withOverride"
	override)");
		Swizzle swizzle;
		swizzle.Load(node);

		WHEN( "checking OverrideMask" ) {
			THEN( "returns true" ) {
				CHECK( swizzle.OverrideMask() == true );
			}
		}
	}

	GIVEN( "a Swizzle loaded without override keyword" ) {
		auto node = AsDataNode(R"(swizzle "noOverride"
	red 0.5 0.5 0 0)");
		Swizzle swizzle;
		swizzle.Load(node);

		WHEN( "checking OverrideMask" ) {
			THEN( "returns false" ) {
				CHECK( swizzle.OverrideMask() == false );
			}
		}
	}
}

SCENARIO( "Testing Swizzle::Name", "[Swizzle][Name]" ) {
	GIVEN( "a default Swizzle" ) {
		Swizzle swizzle;
		WHEN( "checking Name" ) {
			THEN( "returns empty string" ) {
				CHECK( swizzle.Name().empty() );
			}
		}
	}

	GIVEN( "a Swizzle loaded with a name" ) {
		auto node = AsDataNode("swizzle \"mySwizzle\"");
		Swizzle swizzle;
		swizzle.Load(node);

		WHEN( "checking Name" ) {
			THEN( "returns the name from the DataNode" ) {
				CHECK( swizzle.Name() == "mySwizzle" );
			}
		}
	}

	GIVEN( "a Swizzle loaded with a different name" ) {
		auto node = AsDataNode("swizzle \"anotherName\"");
		Swizzle swizzle;
		swizzle.Load(node);

		WHEN( "checking Name" ) {
			THEN( "returns the correct name" ) {
				CHECK( swizzle.Name() == "anotherName" );
			}
		}
	}
}

SCENARIO( "Testing Swizzle::IsLoaded", "[Swizzle][IsLoaded]" ) {
	GIVEN( "a default Swizzle" ) {
		Swizzle swizzle;
		WHEN( "checking IsLoaded" ) {
			THEN( "returns false before Load is called" ) {
				CHECK( swizzle.IsLoaded() == false );
			}
		}
	}

	GIVEN( "a Swizzle after Load is called" ) {
		auto node = AsDataNode("swizzle \"loaded\"");
		Swizzle swizzle;
		swizzle.Load(node);

		WHEN( "checking IsLoaded" ) {
			THEN( "returns true after Load is called" ) {
				CHECK( swizzle.IsLoaded() == true );
			}
		}
	}
}

// #endregion unit tests



} // test namespace