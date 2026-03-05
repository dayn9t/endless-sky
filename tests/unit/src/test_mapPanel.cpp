/* test_mapPanel.cpp
Copyright (c) 2026 by Jiang

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
#include "../../../source/MapPanel.h"

// ... and any system includes needed for the test file.
#include "../../../source/Panel.h"
#include <memory>

namespace { // test namespace

// #region mock data

// #endregion mock data



// #region unit tests

TEST_CASE( "MapPanel static constants", "[MapPanel]" ) {
	SECTION( "Show mode constants" ) {
		CHECK( MapPanel::SHOW_SHIPYARD == -1 );
		CHECK( MapPanel::SHOW_OUTFITTER == -2 );
		CHECK( MapPanel::SHOW_VISITED == -3 );
		CHECK( MapPanel::SHOW_SPECIAL == -4 );
		CHECK( MapPanel::SHOW_GOVERNMENT == -5 );
		CHECK( MapPanel::SHOW_REPUTATION == -6 );
		CHECK( MapPanel::SHOW_DANGER == -7 );
		CHECK( MapPanel::SHOW_STARS == -8 );
	}
	SECTION( "Show mode constants are distinct" ) {
		CHECK( MapPanel::SHOW_SHIPYARD != MapPanel::SHOW_OUTFITTER );
		CHECK( MapPanel::SHOW_OUTFITTER != MapPanel::SHOW_VISITED );
		CHECK( MapPanel::SHOW_VISITED != MapPanel::SHOW_SPECIAL );
		CHECK( MapPanel::SHOW_SPECIAL != MapPanel::SHOW_GOVERNMENT );
		CHECK( MapPanel::SHOW_GOVERNMENT != MapPanel::SHOW_REPUTATION );
		CHECK( MapPanel::SHOW_REPUTATION != MapPanel::SHOW_DANGER );
		CHECK( MapPanel::SHOW_DANGER != MapPanel::SHOW_STARS );
	}
	SECTION( "Visual constants" ) {
		CHECK( MapPanel::OUTER > 0.f );
		CHECK( MapPanel::INNER > 0.f );
		CHECK( MapPanel::LINK_WIDTH > 0.f );
		CHECK( MapPanel::LINK_OFFSET >= 0.f );
		CHECK( MapPanel::OUTER > MapPanel::INNER );
	}
}

SCENARIO( "MapPanel class traits", "[MapPanel]" ) {
	GIVEN( "The MapPanel class" ) {
		WHEN( "Checking class hierarchy" ) {
			THEN( "MapPanel inherits from Panel" ) {
				CHECK( std::is_base_of_v<Panel, MapPanel> );
			}
		}
		WHEN( "Checking destructor traits" ) {
			THEN( "MapPanel has a virtual destructor" ) {
				CHECK( std::has_virtual_destructor_v<MapPanel> );
			}
			THEN( "MapPanel is nothrow destructible" ) {
				CHECK( std::is_nothrow_destructible_v<MapPanel> );
			}
		}
		WHEN( "Checking polymorphism" ) {
			THEN( "MapPanel is polymorphic" ) {
				CHECK( std::is_polymorphic_v<MapPanel> );
			}
		}
	}
}

SCENARIO( "MapPanel construction requirements", "[MapPanel][Construction]" ) {
	GIVEN( "The MapPanel class" ) {
		WHEN( "Checking construction traits" ) {
			THEN( "MapPanel is not default constructible" ) {
				// Requires PlayerInfo& and other parameters
				CHECK_FALSE( std::is_default_constructible_v<MapPanel> );
			}
			THEN( "MapPanel is technically copy constructible (type traits allow it despite reference member)" ) {
				// MapPanel has reference members, but type traits report true
				CHECK( std::is_copy_constructible_v<MapPanel> );
			}
			THEN( "MapPanel is technically move constructible (type traits allow it despite reference member)" ) {
				// MapPanel has reference members, but type traits report true
				CHECK( std::is_move_constructible_v<MapPanel> );
			}
		}
	}
}

SCENARIO( "MapPanel SystemTooltipData structure", "[MapPanel][SystemTooltipData]" ) {
	GIVEN( "A SystemTooltipData instance" ) {
		MapPanel::SystemTooltipData data;

		WHEN( "Checking default values" ) {
			THEN( "activeShips defaults to 0" ) {
				CHECK( data.activeShips == 0 );
			}
			THEN( "parkedShips defaults to 0" ) {
				CHECK( data.parkedShips == 0 );
			}
			THEN( "outfits map is empty by default" ) {
				CHECK( data.outfits.empty() );
			}
		}

		WHEN( "Modifying values" ) {
			data.activeShips = 5;
			data.parkedShips = 3;

			THEN( "Values are stored correctly" ) {
				CHECK( data.activeShips == 5 );
				CHECK( data.parkedShips == 3 );
			}
		}
	}
}

SCENARIO( "MapPanel show mode constants usage", "[MapPanel][ShowMode]" ) {
	GIVEN( "The show mode constants" ) {
		WHEN( "Using constants as commodity indices" ) {
			THEN( "All show modes are negative integers" ) {
				CHECK( MapPanel::SHOW_SHIPYARD < 0 );
				CHECK( MapPanel::SHOW_OUTFITTER < 0 );
				CHECK( MapPanel::SHOW_VISITED < 0 );
				CHECK( MapPanel::SHOW_SPECIAL < 0 );
				CHECK( MapPanel::SHOW_GOVERNMENT < 0 );
				CHECK( MapPanel::SHOW_REPUTATION < 0 );
				CHECK( MapPanel::SHOW_DANGER < 0 );
				CHECK( MapPanel::SHOW_STARS < 0 );
			}
		}
		WHEN( "Comparing with regular commodity indices" ) {
			THEN( "Show modes can be distinguished from commodity indices" ) {
				// Commodity indices are typically >= 0
				int normalCommodity = 0;
				CHECK( MapPanel::SHOW_SHIPYARD < normalCommodity );
				CHECK( MapPanel::SHOW_OUTFITTER < normalCommodity );
			}
		}
	}
}

SCENARIO( "MapPanel visual constant relationships", "[MapPanel][Constants]" ) {
	GIVEN( "The visual constants" ) {
		WHEN( "Checking pointer rendering constants" ) {
			THEN( "OUTER is larger than INNER" ) {
				CHECK( MapPanel::OUTER > MapPanel::INNER );
			}
			THEN( "All dimensions are positive" ) {
				CHECK( MapPanel::OUTER > 0.f );
				CHECK( MapPanel::INNER > 0.f );
				CHECK( MapPanel::LINK_WIDTH > 0.f );
			}
		}
		WHEN( "Checking link rendering constants" ) {
			THEN( "LINK_WIDTH is reasonable" ) {
				// Link width should be positive but not too large
				CHECK( MapPanel::LINK_WIDTH > 0.f );
				CHECK( MapPanel::LINK_WIDTH < 100.f );
			}
		}
	}
}

// #endregion unit tests

} // test namespace