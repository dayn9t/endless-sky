/* test_mapPlanetCard.cpp
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
#include "../../../source/MapPlanetCard.h"

// ... and any system includes needed for the test file.
#include "../../../source/MapPanel.h"

namespace { // test namespace

// #region mock data

// #endregion mock data



// #region unit tests

TEST_CASE( "MapPlanetCard::ClickAction enum values", "[MapPlanetCard]" ) {
	SECTION( "ClickAction enum values match MapPanel constants" ) {
		// ClickAction values should align with MapPanel::SHOW_* constants
		CHECK( static_cast<int>(MapPlanetCard::ClickAction::SHOW_GOVERNMENT)
			== MapPanel::SHOW_GOVERNMENT );
		CHECK( static_cast<int>(MapPlanetCard::ClickAction::SHOW_REPUTATION)
			== MapPanel::SHOW_REPUTATION );
		CHECK( static_cast<int>(MapPlanetCard::ClickAction::SHOW_SHIPYARD)
			== MapPanel::SHOW_SHIPYARD );
		CHECK( static_cast<int>(MapPlanetCard::ClickAction::SHOW_OUTFITTER)
			== MapPanel::SHOW_OUTFITTER );
		CHECK( static_cast<int>(MapPlanetCard::ClickAction::SHOW_VISITED)
			== MapPanel::SHOW_VISITED );
	}
	SECTION( "Navigation actions have positive values" ) {
		CHECK( static_cast<int>(MapPlanetCard::ClickAction::GOTO_SHIPYARD) == 3 );
		CHECK( static_cast<int>(MapPlanetCard::ClickAction::GOTO_OUTFITTER) == 2 );
		CHECK( static_cast<int>(MapPlanetCard::ClickAction::SELECTED) == 1 );
		CHECK( static_cast<int>(MapPlanetCard::ClickAction::NONE) == 0 );
	}
	SECTION( "All ClickAction values are distinct" ) {
		auto values = {
			static_cast<int>(MapPlanetCard::ClickAction::SHOW_GOVERNMENT),
			static_cast<int>(MapPlanetCard::ClickAction::SHOW_REPUTATION),
			static_cast<int>(MapPlanetCard::ClickAction::SHOW_SHIPYARD),
			static_cast<int>(MapPlanetCard::ClickAction::SHOW_OUTFITTER),
			static_cast<int>(MapPlanetCard::ClickAction::SHOW_VISITED),
			static_cast<int>(MapPlanetCard::ClickAction::GOTO_SHIPYARD),
			static_cast<int>(MapPlanetCard::ClickAction::GOTO_OUTFITTER),
			static_cast<int>(MapPlanetCard::ClickAction::SELECTED),
			static_cast<int>(MapPlanetCard::ClickAction::NONE)
		};
		// Check that all values are unique
		std::vector<int> vec(values);
		std::sort(vec.begin(), vec.end());
		auto last = std::unique(vec.begin(), vec.end());
		CHECK( last == vec.end() );
	}
}

SCENARIO( "MapPlanetCard class traits", "[MapPlanetCard]" ) {
	GIVEN( "The MapPlanetCard class" ) {
		WHEN( "Checking destructor traits" ) {
			THEN( "MapPlanetCard is nothrow destructible" ) {
				CHECK( std::is_nothrow_destructible_v<MapPlanetCard> );
			}
		}
		WHEN( "Checking construction traits" ) {
			THEN( "MapPlanetCard is not default constructible" ) {
				// Requires StellarObject&, number, hasVisited, parent
				CHECK_FALSE( std::is_default_constructible_v<MapPlanetCard> );
			}
			THEN( "MapPlanetCard is technically copy constructible (type traits allow it despite reference member)" ) {
				// MapPlanetCard has const std::string& planetName reference member
				// but type traits report true because the compiler doesn't validate
				// implicit constructor definitions until instantiation
				CHECK( std::is_copy_constructible_v<MapPlanetCard> );
			}
			THEN( "MapPlanetCard is technically move constructible (type traits allow it despite reference member)" ) {
				// MapPlanetCard has const std::string& planetName reference member
				CHECK( std::is_move_constructible_v<MapPlanetCard> );
			}
		}
		WHEN( "Checking class type traits" ) {
			THEN( "MapPlanetCard is not trivial" ) {
				CHECK_FALSE( std::is_trivial_v<MapPlanetCard> );
			}
			THEN( "MapPlanetCard has standard layout" ) {
				CHECK( std::is_standard_layout_v<MapPlanetCard> );
			}
		}
	}
}

SCENARIO( "MapPlanetCard ClickAction navigation actions", "[MapPlanetCard][ClickAction]" ) {
	GIVEN( "Navigation action values" ) {
		WHEN( "Checking GOTO actions" ) {
			THEN( "GOTO_SHIPYARD has higher value than GOTO_OUTFITTER" ) {
				CHECK( static_cast<int>(MapPlanetCard::ClickAction::GOTO_SHIPYARD)
					> static_cast<int>(MapPlanetCard::ClickAction::GOTO_OUTFITTER) );
			}
			THEN( "GOTO actions are positive" ) {
				CHECK( static_cast<int>(MapPlanetCard::ClickAction::GOTO_SHIPYARD) > 0 );
				CHECK( static_cast<int>(MapPlanetCard::ClickAction::GOTO_OUTFITTER) > 0 );
			}
		}
		WHEN( "Checking SELECTED and NONE" ) {
			THEN( "SELECTED is greater than NONE" ) {
				CHECK( static_cast<int>(MapPlanetCard::ClickAction::SELECTED)
					> static_cast<int>(MapPlanetCard::ClickAction::NONE) );
			}
			THEN( "NONE is zero" ) {
				CHECK( static_cast<int>(MapPlanetCard::ClickAction::NONE) == 0 );
			}
		}
	}
}

SCENARIO( "MapPlanetCard ClickAction show mode actions", "[MapPlanetCard][ShowMode]" ) {
	GIVEN( "Show mode ClickAction values" ) {
		WHEN( "Comparing show mode values with MapPanel" ) {
			THEN( "SHOW_GOVERNMENT matches MapPanel constant" ) {
				CHECK( static_cast<int>(MapPlanetCard::ClickAction::SHOW_GOVERNMENT)
					== MapPanel::SHOW_GOVERNMENT );
			}
			THEN( "SHOW_REPUTATION matches MapPanel constant" ) {
				CHECK( static_cast<int>(MapPlanetCard::ClickAction::SHOW_REPUTATION)
					== MapPanel::SHOW_REPUTATION );
			}
			THEN( "SHOW_SHIPYARD matches MapPanel constant" ) {
				CHECK( static_cast<int>(MapPlanetCard::ClickAction::SHOW_SHIPYARD)
					== MapPanel::SHOW_SHIPYARD );
			}
			THEN( "SHOW_OUTFITTER matches MapPanel constant" ) {
				CHECK( static_cast<int>(MapPlanetCard::ClickAction::SHOW_OUTFITTER)
					== MapPanel::SHOW_OUTFITTER );
			}
			THEN( "SHOW_VISITED matches MapPanel constant" ) {
				CHECK( static_cast<int>(MapPlanetCard::ClickAction::SHOW_VISITED)
					== MapPanel::SHOW_VISITED );
			}
		}
		WHEN( "Using show mode actions in switch" ) {
			THEN( "All show modes are distinct and handled" ) {
				int distinctCount = 0;
				// Test show mode values
				for( int mode : { MapPanel::SHOW_GOVERNMENT, MapPanel::SHOW_REPUTATION,
					MapPanel::SHOW_SHIPYARD, MapPanel::SHOW_OUTFITTER, MapPanel::SHOW_VISITED } )
				{
					switch( mode )
					{
						case MapPanel::SHOW_GOVERNMENT:
						case MapPanel::SHOW_REPUTATION:
						case MapPanel::SHOW_SHIPYARD:
						case MapPanel::SHOW_OUTFITTER:
						case MapPanel::SHOW_VISITED:
							++distinctCount;
							break;
						default:
							break;
					}
				}
				CHECK( distinctCount == 5 );
			}
		}
	}
}

SCENARIO( "MapPlanetCard ClickAction action categories", "[MapPlanetCard][Categories]" ) {
	GIVEN( "Different types of ClickAction values" ) {
		WHEN( "Categorizing show mode actions" ) {
			THEN( "Show mode actions have negative values" ) {
				CHECK( static_cast<int>(MapPlanetCard::ClickAction::SHOW_GOVERNMENT) < 0 );
				CHECK( static_cast<int>(MapPlanetCard::ClickAction::SHOW_REPUTATION) < 0 );
				CHECK( static_cast<int>(MapPlanetCard::ClickAction::SHOW_SHIPYARD) < 0 );
				CHECK( static_cast<int>(MapPlanetCard::ClickAction::SHOW_OUTFITTER) < 0 );
				CHECK( static_cast<int>(MapPlanetCard::ClickAction::SHOW_VISITED) < 0 );
			}
		}
		WHEN( "Categorizing navigation actions" ) {
			THEN( "Navigation actions have positive values" ) {
				CHECK( static_cast<int>(MapPlanetCard::ClickAction::GOTO_SHIPYARD) > 0 );
				CHECK( static_cast<int>(MapPlanetCard::ClickAction::GOTO_OUTFITTER) > 0 );
				CHECK( static_cast<int>(MapPlanetCard::ClickAction::SELECTED) > 0 );
			}
		}
		WHEN( "Categorizing NONE action" ) {
			THEN( "NONE is zero" ) {
				CHECK( static_cast<int>(MapPlanetCard::ClickAction::NONE) == 0 );
			}
		}
	}
}

SCENARIO( "MapPlanetCard ClickAction comparison operations", "[MapPlanetCard][Comparison]" ) {
	GIVEN( "ClickAction enum values" ) {
		WHEN( "Comparing enum values directly" ) {
			THEN( "Different values are not equal" ) {
				CHECK( MapPlanetCard::ClickAction::NONE != MapPlanetCard::ClickAction::SELECTED );
				CHECK( MapPlanetCard::ClickAction::SELECTED != MapPlanetCard::ClickAction::GOTO_SHIPYARD );
				CHECK( MapPlanetCard::ClickAction::GOTO_SHIPYARD != MapPlanetCard::ClickAction::GOTO_OUTFITTER );
			}
		}
		WHEN( "Comparing same values" ) {
			THEN( "Same values are equal" ) {
				CHECK( MapPlanetCard::ClickAction::NONE == MapPlanetCard::ClickAction::NONE );
				CHECK( MapPlanetCard::ClickAction::SELECTED == MapPlanetCard::ClickAction::SELECTED );
			}
		}
	}
}

// #endregion unit tests

} // test namespace