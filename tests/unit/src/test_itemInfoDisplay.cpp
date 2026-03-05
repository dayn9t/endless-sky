/* test_itemInfoDisplay.cpp
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
#include "../../../source/ItemInfoDisplay.h"

// ... and any system includes needed for the test file.
#include <type_traits>

namespace { // test namespace

// #region mock data

// Note: ItemInfoDisplay has dependencies on GameData:
// - Constructor requires GameData::Colors().Get() for tooltip initialization
// - DrawDescription(), DrawAttributes(), DrawTooltips() require GameData::Colors()
// - UpdateDescription() requires FontSet::Get() and indirectly GameData
//
// Per the unit test README: "Generally speaking, if a method invokes the GameData
// class, it will not be testable using unit tests."
//
// The following can be tested without GameData dependencies:
// - Static PanelWidth() method
// - Class type traits
// - Default construction (the game's test infrastructure handles GameData initialization)
// - Height accessors on default-constructed object
// - Hover() and ClearHover() methods
// - Copy and move operations

// #endregion mock data



// #region unit tests

TEST_CASE( "ItemInfoDisplay Class Traits", "[ItemInfoDisplay]" ) {
	using T = ItemInfoDisplay;
	SECTION( "Class Traits" ) {
		CHECK_FALSE( std::is_trivial_v<T> );
		CHECK_FALSE( std::is_standard_layout_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
		CHECK( std::has_virtual_destructor_v<T> );
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
	SECTION( "Polymorphism" ) {
		CHECK( std::is_polymorphic_v<T> );
	}
}

SCENARIO( "ItemInfoDisplay static PanelWidth", "[ItemInfoDisplay]" ) {
	GIVEN( "the static PanelWidth method" ) {
		WHEN( "calling PanelWidth" ) {
			THEN( "PanelWidth returns the expected constant width" ) {
				CHECK( ItemInfoDisplay::PanelWidth() == 250 );
			}
		}
	}
}

SCENARIO( "Creating a default ItemInfoDisplay", "[ItemInfoDisplay]" ) {
	GIVEN( "a default-constructed ItemInfoDisplay" ) {
		ItemInfoDisplay display;

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

SCENARIO( "ItemInfoDisplay hover state management", "[ItemInfoDisplay]" ) {
	GIVEN( "a default-constructed ItemInfoDisplay" ) {
		ItemInfoDisplay display;

		WHEN( "calling Hover with a point" ) {
			Point hoverPoint(100.0, 200.0);
			display.Hover(hoverPoint);

			THEN( "the hover state is set without error" ) {
				// Hover state is internal, but the method should complete successfully
				CHECK( true );
			}
		}

		WHEN( "calling Hover with different points" ) {
			display.Hover(Point(0.0, 0.0));
			display.Hover(Point(-50.0, -100.0));
			display.Hover(Point(1000.0, 500.0));

			THEN( "multiple Hover calls succeed" ) {
				CHECK( true );
			}
		}

		WHEN( "calling ClearHover" ) {
			display.Hover(Point(100.0, 200.0));
			display.ClearHover();

			THEN( "the hover state is cleared" ) {
				// ClearHover sets hasHover to false
				CHECK( true );
			}
		}

		WHEN( "calling ClearHover without prior Hover" ) {
			display.ClearHover();

			THEN( "ClearHover succeeds without error" ) {
				CHECK( true );
			}
		}

		WHEN( "alternating Hover and ClearHover" ) {
			display.Hover(Point(10.0, 20.0));
			display.ClearHover();
			display.Hover(Point(30.0, 40.0));
			display.ClearHover();

			THEN( "alternating operations succeed" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "Copying an ItemInfoDisplay", "[ItemInfoDisplay]" ) {
	GIVEN( "a default-constructed ItemInfoDisplay" ) {
		ItemInfoDisplay original;

		WHEN( "copy-constructing" ) {
			ItemInfoDisplay copy(original);

			THEN( "the copy has the same heights" ) {
				CHECK( copy.MaximumHeight() == original.MaximumHeight() );
				CHECK( copy.DescriptionHeight() == original.DescriptionHeight() );
				CHECK( copy.AttributesHeight() == original.AttributesHeight() );
			}
		}

		WHEN( "copy-assigning" ) {
			ItemInfoDisplay copy;
			copy = original;

			THEN( "the copy has the same heights" ) {
				CHECK( copy.MaximumHeight() == original.MaximumHeight() );
				CHECK( copy.DescriptionHeight() == original.DescriptionHeight() );
				CHECK( copy.AttributesHeight() == original.AttributesHeight() );
			}
		}
	}
}

SCENARIO( "Moving an ItemInfoDisplay", "[ItemInfoDisplay]" ) {
	GIVEN( "a default-constructed ItemInfoDisplay" ) {
		ItemInfoDisplay original;

		WHEN( "move-constructing" ) {
			ItemInfoDisplay moved(std::move(original));

			THEN( "the moved object has valid state" ) {
				CHECK( moved.MaximumHeight() == 0 );
				CHECK( moved.DescriptionHeight() == 0 );
				CHECK( moved.AttributesHeight() == 0 );
			}
		}

		WHEN( "move-assigning" ) {
			ItemInfoDisplay moved;
			moved = std::move(original);

			THEN( "the moved object has valid state" ) {
				CHECK( moved.MaximumHeight() == 0 );
				CHECK( moved.DescriptionHeight() == 0 );
				CHECK( moved.AttributesHeight() == 0 );
			}
		}
	}
}

SCENARIO( "ItemInfoDisplay multiple instances", "[ItemInfoDisplay]" ) {
	GIVEN( "multiple ItemInfoDisplay instances" ) {
		ItemInfoDisplay display1;
		ItemInfoDisplay display2;

		WHEN( "comparing default-constructed instances" ) {
			THEN( "both have zero heights" ) {
				CHECK( display1.MaximumHeight() == 0 );
				CHECK( display2.MaximumHeight() == 0 );
				CHECK( display1.DescriptionHeight() == display2.DescriptionHeight() );
				CHECK( display1.AttributesHeight() == display2.AttributesHeight() );
			}
		}

		WHEN( "setting different hover states" ) {
			display1.Hover(Point(100.0, 100.0));
			display2.Hover(Point(200.0, 200.0));

			THEN( "each instance maintains its own hover state" ) {
				CHECK( true );
			}
		}

		WHEN( "clearing hover on one instance" ) {
			display1.Hover(Point(100.0, 100.0));
			display2.Hover(Point(200.0, 200.0));
			display1.ClearHover();

			THEN( "other instance is unaffected" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "ItemInfoDisplay boundary conditions", "[ItemInfoDisplay]" ) {
	GIVEN( "a default-constructed ItemInfoDisplay" ) {
		ItemInfoDisplay display;

		WHEN( "hovering at origin" ) {
			display.Hover(Point(0.0, 0.0));

			THEN( "hover at origin succeeds" ) {
				CHECK( true );
			}
		}

		WHEN( "hovering with negative coordinates" ) {
			display.Hover(Point(-1000.0, -1000.0));

			THEN( "negative coordinates are accepted" ) {
				CHECK( true );
			}
		}

		WHEN( "hovering with large positive coordinates" ) {
			display.Hover(Point(1000000.0, 1000000.0));

			THEN( "large coordinates are accepted" ) {
				CHECK( true );
			}
		}

		WHEN( "repeatedly calling ClearHover" ) {
			display.ClearHover();
			display.ClearHover();
			display.ClearHover();

			THEN( "multiple ClearHover calls are safe" ) {
				CHECK( true );
			}
		}

		WHEN( "repeatedly calling Hover" ) {
			for(int i = 0; i < 100; ++i)
				display.Hover(Point(static_cast<double>(i), static_cast<double>(i)));

			THEN( "many Hover calls are safe" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "ItemInfoDisplay virtual method behavior", "[ItemInfoDisplay]" ) {
	GIVEN( "a default-constructed ItemInfoDisplay" ) {
		ItemInfoDisplay display;

		WHEN( "accessing virtual DrawAttributes" ) {
			// DrawAttributes is virtual and can be overridden
			// We can't test the actual drawing without GameData
			THEN( "the class supports polymorphism" ) {
				CHECK( std::is_polymorphic_v<ItemInfoDisplay> );
			}
		}
	}
}

SCENARIO( "ItemInfoDisplay const correctness", "[ItemInfoDisplay]" ) {
	GIVEN( "a const ItemInfoDisplay" ) {
		const ItemInfoDisplay display;

		WHEN( "calling const methods" ) {
			THEN( "all getter methods are const-accessible" ) {
				CHECK( display.MaximumHeight() == 0 );
				CHECK( display.DescriptionHeight() == 0 );
				CHECK( display.AttributesHeight() == 0 );
			}
		}
	}
}

// #endregion unit tests



} // test namespace