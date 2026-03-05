/* test_outfitInfoDisplay.cpp
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
#include "../../../source/OutfitInfoDisplay.h"

// Include base class for inheritance testing.
#include "../../../source/ItemInfoDisplay.h"

// ... and any system includes needed for the test file.
#include <type_traits>

namespace { // test namespace

// #region mock data

// Note: OutfitInfoDisplay has significant dependencies on GameData through Outfit and PlayerInfo:
// - Constructor requires Outfit and PlayerInfo, which require GameData initialization
// - Update() requires Outfit, PlayerInfo, and accesses GameData::Outfits().Find(),
//   PlayerInfo::GetDate(), PlayerInfo::StockDepreciation(), PlayerInfo::FleetDepreciation()
// - UpdateRequirements() requires GameData::Outfits().Find()
// - UpdateAttributes() requires Outfit::GetWeapon() and Weapon methods
// - DrawRequirements() requires initialized requirementLabels and requirementValues
//
// Per the unit test README: "Generally speaking, if a method invokes the GameData
// class, it will not be testable using unit tests."
//
// The following can be tested without GameData:
// - Default construction
// - Class type traits
// - Inheritance relationship with ItemInfoDisplay
// - RequirementsHeight() on default-constructed object (returns 0)
// - Inherited methods from ItemInfoDisplay (MaximumHeight, DescriptionHeight, AttributesHeight)

// #endregion mock data



// #region unit tests

TEST_CASE( "OutfitInfoDisplay Class Traits", "[OutfitInfoDisplay]" ) {
	using T = OutfitInfoDisplay;
	SECTION( "Class Traits" ) {
		CHECK_FALSE( std::is_trivial_v<T> );
		// OutfitInfoDisplay has virtual functions (polymorphic), so it's not standard_layout
		CHECK_FALSE( std::is_standard_layout_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
		// Has virtual destructor, not trivially destructible
		CHECK_FALSE( std::is_trivially_destructible_v<T> );
	}
	SECTION( "Construction Traits" ) {
		CHECK( std::is_default_constructible_v<T> );
		// Has non-trivial base class, not trivially default constructible
		CHECK_FALSE( std::is_trivially_default_constructible_v<T> );
		CHECK_FALSE( std::is_nothrow_default_constructible_v<T> );
		CHECK( std::is_copy_constructible_v<T> );
		// Has non-trivial copy/move operations due to inheritance
		CHECK_FALSE( std::is_trivially_copy_constructible_v<T> );
		CHECK_FALSE( std::is_nothrow_copy_constructible_v<T> );
		CHECK( std::is_move_constructible_v<T> );
		CHECK_FALSE( std::is_trivially_move_constructible_v<T> );
		CHECK_FALSE( std::is_nothrow_move_constructible_v<T> );
	}
	SECTION( "Copy Traits" ) {
		CHECK( std::is_copy_assignable_v<T> );
		// Not trivially copyable due to virtual functions
		CHECK_FALSE( std::is_trivially_copyable_v<T> );
		CHECK_FALSE( std::is_trivially_copy_assignable_v<T> );
		CHECK_FALSE( std::is_nothrow_copy_assignable_v<T> );
	}
	SECTION( "Move Traits" ) {
		CHECK( std::is_move_assignable_v<T> );
		CHECK_FALSE( std::is_trivially_move_assignable_v<T> );
		CHECK_FALSE( std::is_nothrow_move_assignable_v<T> );
	}
}

TEST_CASE( "OutfitInfoDisplay Inheritance", "[OutfitInfoDisplay]" ) {
	SECTION( "Inherits from ItemInfoDisplay" ) {
		CHECK( std::is_base_of_v<ItemInfoDisplay, OutfitInfoDisplay> );
	}
	SECTION( "Can be used polymorphically" ) {
		CHECK( std::is_polymorphic_v<OutfitInfoDisplay> );
	}
	SECTION( "Has virtual destructor through base class" ) {
		CHECK( std::has_virtual_destructor_v<OutfitInfoDisplay> );
	}
}

SCENARIO( "Creating a default OutfitInfoDisplay", "[OutfitInfoDisplay]" ) {
	GIVEN( "a default-constructed OutfitInfoDisplay" ) {
		OutfitInfoDisplay display;

		THEN( "the requirements height is zero" ) {
			CHECK( display.RequirementsHeight() == 0 );
		}

		THEN( "the maximum height is zero" ) {
			CHECK( display.MaximumHeight() == 0 );
		}

		THEN( "the description height is zero" ) {
			CHECK( display.DescriptionHeight() == 0 );
		}

		THEN( "the attributes height is zero" ) {
			CHECK( display.AttributesHeight() == 0 );
		}
	}
}

SCENARIO( "OutfitInfoDisplay panel width", "[OutfitInfoDisplay]" ) {
	GIVEN( "the static PanelWidth method from base class" ) {
		THEN( "PanelWidth returns the expected width" ) {
			CHECK( ItemInfoDisplay::PanelWidth() == 250 );
		}
	}
}

SCENARIO( "Copying an OutfitInfoDisplay", "[OutfitInfoDisplay]" ) {
	GIVEN( "a default-constructed OutfitInfoDisplay" ) {
		OutfitInfoDisplay original;

		WHEN( "copy-constructing" ) {
			OutfitInfoDisplay copy(original);

			THEN( "the copy has the same heights" ) {
				CHECK( copy.RequirementsHeight() == original.RequirementsHeight() );
				CHECK( copy.MaximumHeight() == original.MaximumHeight() );
				CHECK( copy.DescriptionHeight() == original.DescriptionHeight() );
				CHECK( copy.AttributesHeight() == original.AttributesHeight() );
			}
		}

		WHEN( "copy-assigning" ) {
			OutfitInfoDisplay copy;
			copy = original;

			THEN( "the copy has the same heights" ) {
				CHECK( copy.RequirementsHeight() == original.RequirementsHeight() );
				CHECK( copy.MaximumHeight() == original.MaximumHeight() );
				CHECK( copy.DescriptionHeight() == original.DescriptionHeight() );
				CHECK( copy.AttributesHeight() == original.AttributesHeight() );
			}
		}
	}
}

SCENARIO( "Moving an OutfitInfoDisplay", "[OutfitInfoDisplay]" ) {
	GIVEN( "a default-constructed OutfitInfoDisplay" ) {
		OutfitInfoDisplay original;

		WHEN( "move-constructing" ) {
			OutfitInfoDisplay moved(std::move(original));

			THEN( "the moved object has valid state" ) {
				CHECK( moved.RequirementsHeight() == 0 );
				CHECK( moved.MaximumHeight() == 0 );
				CHECK( moved.DescriptionHeight() == 0 );
				CHECK( moved.AttributesHeight() == 0 );
			}
		}

		WHEN( "move-assigning" ) {
			OutfitInfoDisplay moved;
			moved = std::move(original);

			THEN( "the moved object has valid state" ) {
				CHECK( moved.RequirementsHeight() == 0 );
				CHECK( moved.MaximumHeight() == 0 );
				CHECK( moved.DescriptionHeight() == 0 );
				CHECK( moved.AttributesHeight() == 0 );
			}
		}
	}
}

SCENARIO( "OutfitInfoDisplay base class method access", "[OutfitInfoDisplay]" ) {
	GIVEN( "a default-constructed OutfitInfoDisplay" ) {
		OutfitInfoDisplay display;

		WHEN( "accessing base class methods" ) {
			THEN( "all inherited methods are accessible and return zero" ) {
				CHECK( display.MaximumHeight() == 0 );
				CHECK( display.DescriptionHeight() == 0 );
				CHECK( display.AttributesHeight() == 0 );
			}
		}

		WHEN( "accessing static PanelWidth" ) {
			THEN( "PanelWidth returns the expected value" ) {
				CHECK( OutfitInfoDisplay::PanelWidth() == 250 );
			}
		}
	}
}

// #endregion unit tests



} // test namespace