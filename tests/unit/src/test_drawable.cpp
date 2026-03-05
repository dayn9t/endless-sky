/* test_drawable.cpp
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
#include "../../../source/Drawable.h"

// ... and any system includes needed for the test file.
#include "../../../source/Point.h"
#include "../../../source/Swizzle.h"

#include <cmath>
#include <type_traits>

namespace { // test namespace

// #region mock data

// #endregion mock data



// #region unit tests

SCENARIO( "Drawable class traits", "[Drawable]" ) {
	GIVEN( "the Drawable class" ) {
		THEN( "it is default constructible" ) {
			CHECK( std::is_default_constructible_v<Drawable> );
		}
		AND_THEN( "it is copy constructible" ) {
			CHECK( std::is_copy_constructible_v<Drawable> );
		}
		AND_THEN( "it is move constructible" ) {
			CHECK( std::is_move_constructible_v<Drawable> );
		}
		AND_THEN( "it is copy assignable" ) {
			CHECK( std::is_copy_assignable_v<Drawable> );
		}
		AND_THEN( "it is move assignable" ) {
			CHECK( std::is_move_assignable_v<Drawable> );
		}
		AND_THEN( "it is destructible" ) {
			CHECK( std::is_destructible_v<Drawable> );
		}
	}
}

SCENARIO( "Drawable default construction", "[Drawable]" ) {
	GIVEN( "no parameters" ) {
		WHEN( "a Drawable is default constructed" ) {
			Drawable d;
			THEN( "sprite is null" ) {
				CHECK( d.GetSprite() == nullptr );
			}
			AND_THEN( "HasSprite returns false" ) {
				CHECK_FALSE( d.HasSprite() );
			}
			AND_THEN( "zoom is 1.0" ) {
				CHECK_THAT( d.Zoom(), Catch::Matchers::WithinAbs(1., 0.0001) );
			}
			AND_THEN( "scale is (1, 1)" ) {
				CHECK_THAT( d.Scale().X(), Catch::Matchers::WithinAbs(1., 0.0001) );
				CHECK_THAT( d.Scale().Y(), Catch::Matchers::WithinAbs(1., 0.0001) );
			}
			AND_THEN( "alpha is 1.0" ) {
				CHECK_THAT( d.Alpha(), Catch::Matchers::WithinAbs(1., 0.0001) );
			}
			AND_THEN( "swizzle is None" ) {
				CHECK( d.GetSwizzle() == Swizzle::None() );
			}
			AND_THEN( "does not inherit parent swizzle" ) {
				CHECK_FALSE( d.InheritsParentSwizzle() );
			}
		}
	}
}

SCENARIO( "Drawable construction with sprite and parameters", "[Drawable]" ) {
	GIVEN( "sprite, zoom, scale, and alpha parameters" ) {
		double zoom = 2.0;
		Point scale(1.5, 2.0);
		double alpha = 0.75;
		WHEN( "a Drawable is constructed with sprite and parameters" ) {
			Drawable d(nullptr, zoom, scale, alpha);
			THEN( "sprite is null (as passed)" ) {
				CHECK( d.GetSprite() == nullptr );
			}
			AND_THEN( "zoom is set correctly" ) {
				CHECK_THAT( d.Zoom(), Catch::Matchers::WithinAbs(zoom, 0.0001) );
			}
			AND_THEN( "scale is set correctly" ) {
				CHECK_THAT( d.Scale().X(), Catch::Matchers::WithinAbs(scale.X(), 0.0001) );
				CHECK_THAT( d.Scale().Y(), Catch::Matchers::WithinAbs(scale.Y(), 0.0001) );
			}
			AND_THEN( "alpha is set correctly" ) {
				CHECK_THAT( d.Alpha(), Catch::Matchers::WithinAbs(alpha, 0.0001) );
			}
		}
	}
}

SCENARIO( "Drawable copy construction with parameters", "[Drawable]" ) {
	GIVEN( "an existing Drawable with custom parameters" ) {
		Point scale(1.5, 2.0);
		Drawable original(nullptr, 2.0, scale, 0.5);
		WHEN( "a Drawable is copy constructed with new parameters" ) {
			Point newScale(2.0, 3.0);
			Drawable copy(original, 3.0, newScale, 0.8);
			THEN( "zoom is set to new value" ) {
				CHECK_THAT( copy.Zoom(), Catch::Matchers::WithinAbs(3., 0.0001) );
			}
			AND_THEN( "scale is multiplied with original scale" ) {
				// scale = newScale * original.Scale() = (2.0 * 1.5, 3.0 * 2.0) = (3.0, 6.0)
				CHECK_THAT( copy.Scale().X(), Catch::Matchers::WithinAbs(3., 0.0001) );
				CHECK_THAT( copy.Scale().Y(), Catch::Matchers::WithinAbs(6., 0.0001) );
			}
			AND_THEN( "alpha is multiplied with original alpha" ) {
				// alpha = 0.8 * 0.5 = 0.4
				CHECK_THAT( copy.Alpha(), Catch::Matchers::WithinAbs(0.4, 0.0001) );
			}
		}
	}
}

SCENARIO( "Drawable HasSprite check", "[Drawable][HasSprite]" ) {
	GIVEN( "a Drawable with null sprite" ) {
		Drawable d;
		WHEN( "HasSprite is called" ) {
			bool hasSprite = d.HasSprite();
			THEN( "returns false" ) {
				CHECK_FALSE( hasSprite );
			}
		}
	}
	GIVEN( "a Drawable constructed with null sprite" ) {
		Drawable d(nullptr);
		WHEN( "HasSprite is called" ) {
			bool hasSprite = d.HasSprite();
			THEN( "returns false" ) {
				CHECK_FALSE( hasSprite );
			}
		}
	}
}

SCENARIO( "Drawable GetSprite", "[Drawable][GetSprite]" ) {
	GIVEN( "a default constructed Drawable" ) {
		Drawable d;
		WHEN( "GetSprite is called" ) {
			const Sprite* sprite = d.GetSprite();
			THEN( "returns nullptr" ) {
				CHECK( sprite == nullptr );
			}
		}
	}
	GIVEN( "a Drawable constructed with nullptr sprite" ) {
		Drawable d(nullptr);
		WHEN( "GetSprite is called" ) {
			const Sprite* sprite = d.GetSprite();
			THEN( "returns nullptr" ) {
				CHECK( sprite == nullptr );
			}
		}
	}
}

SCENARIO( "Drawable Width and Height with null sprite", "[Drawable][Dimensions]" ) {
	GIVEN( "a Drawable with null sprite" ) {
		Drawable d;
		WHEN( "Width is called" ) {
			double width = d.Width();
			THEN( "returns 0" ) {
				CHECK_THAT( width, Catch::Matchers::WithinAbs(0., 0.0001) );
			}
		}
		WHEN( "Height is called" ) {
			double height = d.Height();
			THEN( "returns 0" ) {
				CHECK_THAT( height, Catch::Matchers::WithinAbs(0., 0.0001) );
			}
		}
	}
}

SCENARIO( "Drawable Radius with null sprite", "[Drawable][Radius]" ) {
	GIVEN( "a Drawable with null sprite" ) {
		Drawable d;
		WHEN( "Radius is called" ) {
			double radius = d.Radius();
			THEN( "returns 0" ) {
				CHECK_THAT( radius, Catch::Matchers::WithinAbs(0., 0.0001) );
			}
		}
	}
}

SCENARIO( "Drawable Zoom getter", "[Drawable][Zoom]" ) {
	GIVEN( "a Drawable with default zoom" ) {
		Drawable d;
		WHEN( "Zoom is called" ) {
			double zoom = d.Zoom();
			THEN( "returns 1.0" ) {
				CHECK_THAT( zoom, Catch::Matchers::WithinAbs(1., 0.0001) );
			}
		}
	}
	GIVEN( "a Drawable with custom zoom" ) {
		Drawable d(nullptr, 2.5);
		WHEN( "Zoom is called" ) {
			double zoom = d.Zoom();
			THEN( "returns the custom zoom value" ) {
				CHECK_THAT( zoom, Catch::Matchers::WithinAbs(2.5, 0.0001) );
			}
		}
	}
}

SCENARIO( "Drawable Scale getter", "[Drawable][Scale]" ) {
	GIVEN( "a Drawable with default scale" ) {
		Drawable d;
		WHEN( "Scale is called" ) {
			Point scale = d.Scale();
			THEN( "returns (1, 1)" ) {
				CHECK_THAT( scale.X(), Catch::Matchers::WithinAbs(1., 0.0001) );
				CHECK_THAT( scale.Y(), Catch::Matchers::WithinAbs(1., 0.0001) );
			}
		}
	}
	GIVEN( "a Drawable with custom scale" ) {
		Point customScale(2.0, 3.0);
		Drawable d(nullptr, 1.0, customScale);
		WHEN( "Scale is called" ) {
			Point scale = d.Scale();
			THEN( "returns the custom scale" ) {
				CHECK_THAT( scale.X(), Catch::Matchers::WithinAbs(2., 0.0001) );
				CHECK_THAT( scale.Y(), Catch::Matchers::WithinAbs(3., 0.0001) );
			}
		}
	}
}

SCENARIO( "Drawable Alpha getter", "[Drawable][Alpha]" ) {
	GIVEN( "a Drawable with default alpha" ) {
		Drawable d;
		WHEN( "Alpha is called" ) {
			double alpha = d.Alpha();
			THEN( "returns 1.0" ) {
				CHECK_THAT( alpha, Catch::Matchers::WithinAbs(1., 0.0001) );
			}
		}
	}
	GIVEN( "a Drawable with custom alpha" ) {
		Drawable d(nullptr, 1.0, Point(1., 1.), 0.5);
		WHEN( "Alpha is called" ) {
			double alpha = d.Alpha();
			THEN( "returns the custom alpha value" ) {
				CHECK_THAT( alpha, Catch::Matchers::WithinAbs(0.5, 0.0001) );
			}
		}
	}
}

SCENARIO( "Drawable GetSwizzle", "[Drawable][Swizzle]" ) {
	GIVEN( "a default constructed Drawable" ) {
		Drawable d;
		WHEN( "GetSwizzle is called" ) {
			const Swizzle* swizzle = d.GetSwizzle();
			THEN( "returns Swizzle::None()" ) {
				CHECK( swizzle == Swizzle::None() );
			}
		}
	}
}

SCENARIO( "Drawable SetSwizzle", "[Drawable][SetSwizzle]" ) {
	GIVEN( "a Drawable and a custom swizzle" ) {
		Drawable d;
		Swizzle customSwizzle;
		WHEN( "SetSwizzle is called" ) {
			d.SetSwizzle(&customSwizzle);
			THEN( "GetSwizzle returns the custom swizzle" ) {
				CHECK( d.GetSwizzle() == &customSwizzle );
			}
		}
	}
}

SCENARIO( "Drawable SetSprite", "[Drawable][SetSprite]" ) {
	GIVEN( "a Drawable" ) {
		Drawable d;
		WHEN( "SetSprite is called with nullptr" ) {
			d.SetSprite(nullptr);
			THEN( "GetSprite returns nullptr" ) {
				CHECK( d.GetSprite() == nullptr );
			}
		}
	}
}

SCENARIO( "Drawable GetFrame default", "[Drawable][GetFrame]" ) {
	GIVEN( "a Drawable with null sprite" ) {
		Drawable d;
		WHEN( "GetFrame is called without step" ) {
			float frame = d.GetFrame();
			THEN( "returns 0" ) {
				CHECK_THAT( frame, Catch::Matchers::WithinAbs(0.f, 0.0001f) );
			}
		}
		WHEN( "GetFrame is called with step 0" ) {
			float frame = d.GetFrame(0);
			THEN( "returns 0 because sprite is null" ) {
				CHECK_THAT( frame, Catch::Matchers::WithinAbs(0.f, 0.0001f) );
			}
		}
	}
}

SCENARIO( "Drawable InheritsParentSwizzle default", "[Drawable][InheritsParentSwizzle]" ) {
	GIVEN( "a default constructed Drawable" ) {
		Drawable d;
		WHEN( "InheritsParentSwizzle is called" ) {
			bool inherits = d.InheritsParentSwizzle();
			THEN( "returns false by default" ) {
				CHECK_FALSE( inherits );
			}
		}
	}
}

// Note: SetFrameRate, AddFrameRate, PauseAnimation, and SetStep are protected methods
// that require a derived class with access to private members for testing.
// The public interface is tested through GetFrame() and other public methods.

// #endregion unit tests



} // test namespace