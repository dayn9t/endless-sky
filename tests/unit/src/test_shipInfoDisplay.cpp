/* test_shipInfoDisplay.cpp
Copyright (c) 2026 by the Endless Sky community

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
#include "../../../source/ShipInfoDisplay.h"

// Include base class for inheritance testing.
#include "../../../source/ItemInfoDisplay.h"

// ... and any system includes needed for the test file.
#include <type_traits>

namespace { // test namespace

// #region mock data

// Note: ShipInfoDisplay has significant dependencies on GameData through Ship and PlayerInfo:
// - Constructor requires Ship and PlayerInfo, which require GameData initialization
// - Update() requires Ship, PlayerInfo, and accesses GameData::Ships(), GameData::Outfits(),
//   GameData::Colors(), GameData::GetCategory()
// - DrawAttributes() requires GameData::Colors()
// - GetAttributesHeight() and OutfitsHeight() can be called on default-constructed objects
//
// Per the unit test README: "Generally speaking, if a method invokes the GameData
// class, it will not be testable using unit tests."
//
// The following can be tested without GameData:
// - Default construction
// - Class type traits
// - Inheritance relationship with ItemInfoDisplay
// - GetAttributesHeight() on default-constructed object
// - OutfitsHeight() on default-constructed object
// - Inherited methods from ItemInfoDisplay (MaximumHeight, DescriptionHeight, AttributesHeight)

// #endregion mock data



// #region unit tests

TEST_CASE( "ShipInfoDisplay Class Traits", "[ShipInfoDisplay]" ) {
	using T = ShipInfoDisplay;
	SECTION( "Class Traits" ) {
		CHECK_FALSE( std::is_trivial_v<T> );
		CHECK( std::is_standard_layout_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
		CHECK( std::is_trivially_destructible_v<T> );
	}
	SECTION( "Construction Traits" ) {
		CHECK( std::is_default_constructible_v<T> );
		CHECK( std::is_nothrow_default_constructible_v<T> );
		CHECK( std::is_copy_constructible_v<T> );
		CHECK( std::is_move_constructible_v<T> );
		CHECK( std::is_nothrow_move_constructible_v<T> );
	}
	SECTION( "Copy Traits" ) {
		CHECK( std::is_copy_assignable_v<T> );
		CHECK( std::is_move_assignable_v<T> );
		CHECK( std::is_nothrow_move_assignable_v<T> );
	}
}

TEST_CASE( "ShipInfoDisplay Inheritance", "[ShipInfoDisplay]" ) {
	SECTION( "Inherits from ItemInfoDisplay" ) {
		CHECK( std::is_base_of_v<ItemInfoDisplay, ShipInfoDisplay> );
	}
	SECTION( "Can be used polymorphically" ) {
		CHECK( std::is_polymorphic_v<ShipInfoDisplay> );
	}
	SECTION( "Has virtual destructor through base class" ) {
		CHECK( std::has_virtual_destructor_v<ShipInfoDisplay> );
	}
}

SCENARIO( "Creating a default ShipInfoDisplay", "[ShipInfoDisplay]" ) {
	GIVEN( "a default-constructed ShipInfoDisplay" ) {
		ShipInfoDisplay display;

		THEN( "the attributes height is zero" ) {
			CHECK( display.AttributesHeight() == 0 );
		}

		THEN( "the outfits height is zero" ) {
			CHECK( display.OutfitsHeight() == 0 );
		}

		THEN( "the maximum height is zero" ) {
			CHECK( display.MaximumHeight() == 0 );
		}

		THEN( "the description height is zero" ) {
			CHECK( display.DescriptionHeight() == 0 );
		}
	}
}

SCENARIO( "Getting attributes height with sale parameter", "[ShipInfoDisplay]" ) {
	GIVEN( "a default-constructed ShipInfoDisplay" ) {
		ShipInfoDisplay display;

		WHEN( "getting attributes height without sale" ) {
			int height = display.GetAttributesHeight(false);

			THEN( "height equals base attributes height" ) {
				CHECK( height == display.AttributesHeight() );
			}
		}

		WHEN( "getting attributes height with sale" ) {
			int height = display.GetAttributesHeight(true);

			THEN( "height equals base attributes height plus sale height" ) {
				// When no Update() has been called, sale height is 0
				// so height should equal attributes height
				CHECK( height == display.AttributesHeight() );
			}
		}
	}
}

SCENARIO( "ShipInfoDisplay panel width", "[ShipInfoDisplay]" ) {
	GIVEN( "the static PanelWidth method from base class" ) {
		THEN( "PanelWidth returns the expected width" ) {
			CHECK( ItemInfoDisplay::PanelWidth() == 250 );
		}
	}
}

SCENARIO( "Copying a ShipInfoDisplay", "[ShipInfoDisplay]" ) {
	GIVEN( "a default-constructed ShipInfoDisplay" ) {
		ShipInfoDisplay original;

		WHEN( "copy-constructing" ) {
			ShipInfoDisplay copy(original);

			THEN( "the copy has the same heights" ) {
				CHECK( copy.AttributesHeight() == original.AttributesHeight() );
				CHECK( copy.OutfitsHeight() == original.OutfitsHeight() );
				CHECK( copy.MaximumHeight() == original.MaximumHeight() );
			}
		}

		WHEN( "copy-assigning" ) {
			ShipInfoDisplay copy;
			copy = original;

			THEN( "the copy has the same heights" ) {
				CHECK( copy.AttributesHeight() == original.AttributesHeight() );
				CHECK( copy.OutfitsHeight() == original.OutfitsHeight() );
				CHECK( copy.MaximumHeight() == original.MaximumHeight() );
			}
		}
	}
}

SCENARIO( "Moving a ShipInfoDisplay", "[ShipInfoDisplay]" ) {
	GIVEN( "a default-constructed ShipInfoDisplay" ) {
		ShipInfoDisplay original;

		WHEN( "move-constructing" ) {
			ShipInfoDisplay moved(std::move(original));

			THEN( "the moved object has valid state" ) {
				CHECK( moved.AttributesHeight() == 0 );
				CHECK( moved.OutfitsHeight() == 0 );
				CHECK( moved.MaximumHeight() == 0 );
			}
		}

		WHEN( "move-assigning" ) {
			ShipInfoDisplay moved;
			moved = std::move(original);

			THEN( "the moved object has valid state" ) {
				CHECK( moved.AttributesHeight() == 0 );
				CHECK( moved.OutfitsHeight() == 0 );
				CHECK( moved.MaximumHeight() == 0 );
			}
		}
	}
}

SCENARIO( "ShipInfoDisplay base class method access", "[ShipInfoDisplay]" ) {
	GIVEN( "a default-constructed ShipInfoDisplay" ) {
		ShipInfoDisplay display;

		WHEN( "accessing base class methods" ) {
			THEN( "all inherited methods are accessible and return zero" ) {
				CHECK( display.MaximumHeight() == 0 );
				CHECK( display.DescriptionHeight() == 0 );
				CHECK( display.AttributesHeight() == 0 );
			}
		}

		WHEN( "accessing static PanelWidth" ) {
			THEN( "PanelWidth returns the expected value" ) {
				CHECK( ShipInfoDisplay::PanelWidth() == 250 );
			}
		}
	}
}

SCENARIO( "ShipInfoDisplay GetAttributesHeight edge cases", "[ShipInfoDisplay]" ) {
	GIVEN( "a default-constructed ShipInfoDisplay" ) {
		ShipInfoDisplay display;

		WHEN( "calling GetAttributesHeight with different sale values" ) {
			int heightNoSale = display.GetAttributesHeight(false);
			int heightWithSale = display.GetAttributesHeight(true);

			THEN( "both return the same value when no Update was called" ) {
				CHECK( heightNoSale == heightWithSale );
				CHECK( heightNoSale == 0 );
			}
		}
	}
}

// #endregion unit tests



} // test namespace