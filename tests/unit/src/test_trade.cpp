/* test_trade.cpp
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
#include "../../../source/Trade.h"

// Include a helper function.
#include "datanode-factory.h"

// ... and any system includes needed for the test file.
#include <string>
#include <vector>

namespace { // test namespace

// #region mock data

// Insert file-local data here, e.g. classes, structs, or fixtures that will be useful
// to help test this class/method.

// #endregion mock data



// #region unit tests

SCENARIO( "Creating a Trade instance", "[Trade][Creation]" ) {
	GIVEN( "a default constructed Trade" ) {
		Trade trade;
		THEN( "it has no commodities" ) {
			CHECK( trade.Commodities().empty() );
		}
		AND_THEN( "it has no special commodities" ) {
			CHECK( trade.SpecialCommodities().empty() );
		}
	}
}

SCENARIO( "Loading commodities into Trade", "[Trade][Load]" ) {
	GIVEN( "an empty Trade instance" ) {
		Trade trade;
		WHEN( "loading a regular commodity with price range" ) {
			auto node = AsDataNode(R"(
trade
	commodity "Food" 50 150
		"eggs"
		"frozen meat"
)");
			trade.Load(node);
			THEN( "the commodity is added to commodities with correct values" ) {
				REQUIRE( trade.Commodities().size() == 1 );
				const auto &commodity = trade.Commodities()[0];
				CHECK( commodity.name == "Food" );
				CHECK( commodity.low == 50 );
				CHECK( commodity.high == 150 );
				AND_THEN( "items are populated from child nodes" ) {
					REQUIRE( commodity.items.size() == 2 );
					CHECK( commodity.items[0] == "eggs" );
					CHECK( commodity.items[1] == "frozen meat" );
				}
			}
			AND_THEN( "special commodities remain empty" ) {
				CHECK( trade.SpecialCommodities().empty() );
			}
		}
		WHEN( "loading a special commodity without price range" ) {
			auto node = AsDataNode(R"(
trade
	commodity "Mission Cargo"
		"secret documents"
		"prototype weapon"
)");
			trade.Load(node);
			THEN( "the commodity is added to special commodities" ) {
				REQUIRE( trade.SpecialCommodities().size() == 1 );
				const auto &commodity = trade.SpecialCommodities()[0];
				CHECK( commodity.name == "Mission Cargo" );
				CHECK( commodity.low == 0 );
				CHECK( commodity.high == 0 );
				AND_THEN( "items are populated from child nodes" ) {
					REQUIRE( commodity.items.size() == 2 );
					CHECK( commodity.items[0] == "secret documents" );
					CHECK( commodity.items[1] == "prototype weapon" );
				}
			}
			AND_THEN( "regular commodities remain empty" ) {
				CHECK( trade.Commodities().empty() );
			}
		}
		WHEN( "loading multiple commodities" ) {
			auto node = AsDataNode(R"(
trade
	commodity "Food" 50 150
		"eggs"
	commodity "Industrial" 100 300
		"steel"
	commodity "Luxury" 200 500
)");
			trade.Load(node);
			THEN( "all commodities are added in order" ) {
				REQUIRE( trade.Commodities().size() == 3 );
				CHECK( trade.Commodities()[0].name == "Food" );
				CHECK( trade.Commodities()[1].name == "Industrial" );
				CHECK( trade.Commodities()[2].name == "Luxury" );
			}
		}
		WHEN( "loading a commodity that already exists" ) {
			auto node = AsDataNode(R"(
trade
	commodity "Food" 50 150
		"eggs"
	commodity "Food" 60 160
		"bread"
)");
			trade.Load(node);
			THEN( "the existing commodity is updated" ) {
				REQUIRE( trade.Commodities().size() == 1 );
				const auto &commodity = trade.Commodities()[0];
				CHECK( commodity.name == "Food" );
				CHECK( commodity.low == 60 );
				CHECK( commodity.high == 160 );
				AND_THEN( "items from both loads are combined" ) {
					REQUIRE( commodity.items.size() == 2 );
					CHECK( commodity.items[0] == "eggs" );
					CHECK( commodity.items[1] == "bread" );
				}
			}
		}
	}
}

SCENARIO( "Loading special commodities into Trade", "[Trade][Load][Special]" ) {
	GIVEN( "an empty Trade instance" ) {
		Trade trade;
		WHEN( "loading multiple special commodities" ) {
			auto node = AsDataNode(R"(
trade
	commodity "Mission Cargo"
		"documents"
	commodity "Contraband"
		"illegal goods"
)");
			trade.Load(node);
			THEN( "all special commodities are added in order" ) {
				REQUIRE( trade.SpecialCommodities().size() == 2 );
				CHECK( trade.SpecialCommodities()[0].name == "Mission Cargo" );
				CHECK( trade.SpecialCommodities()[1].name == "Contraband" );
			}
		}
		WHEN( "loading a special commodity that already exists" ) {
			auto node = AsDataNode(R"(
trade
	commodity "Mission Cargo"
		"documents"
	commodity "Mission Cargo"
		"packages"
)");
			trade.Load(node);
			THEN( "the existing special commodity is updated" ) {
				REQUIRE( trade.SpecialCommodities().size() == 1 );
				const auto &commodity = trade.SpecialCommodities()[0];
				CHECK( commodity.name == "Mission Cargo" );
				AND_THEN( "items from both loads are combined" ) {
					REQUIRE( commodity.items.size() == 2 );
					CHECK( commodity.items[0] == "documents" );
					CHECK( commodity.items[1] == "packages" );
				}
			}
		}
	}
}

SCENARIO( "Clearing commodities in Trade", "[Trade][Load][Clear]" ) {
	GIVEN( "a Trade instance with existing commodities" ) {
		Trade trade;
		auto node = AsDataNode(R"(
trade
	commodity "Food" 50 150
	commodity "Industrial" 100 300
)");
		trade.Load(node);
		REQUIRE( trade.Commodities().size() == 2 );
		WHEN( "clearing commodities" ) {
			auto clearNode = AsDataNode(R"(
trade
	clear
)");
			trade.Load(clearNode);
			THEN( "all commodities are removed" ) {
				CHECK( trade.Commodities().empty() );
			}
			AND_THEN( "special commodities are not affected" ) {
				CHECK( trade.SpecialCommodities().empty() );
			}
		}
		WHEN( "clearing and then adding new commodities" ) {
			auto clearAndAddNode = AsDataNode(R"(
trade
	clear
	commodity "Luxury" 200 500
)");
			trade.Load(clearAndAddNode);
			THEN( "only the new commodity exists" ) {
				REQUIRE( trade.Commodities().size() == 1 );
				CHECK( trade.Commodities()[0].name == "Luxury" );
			}
		}
	}
}

SCENARIO( "Loading mixed commodity types", "[Trade][Load][Mixed]" ) {
	GIVEN( "an empty Trade instance" ) {
		Trade trade;
		WHEN( "loading both regular and special commodities" ) {
			auto node = AsDataNode(R"(
trade
	commodity "Food" 50 150
		"eggs"
	commodity "Mission Cargo"
		"documents"
	commodity "Industrial" 100 300
	commodity "Contraband"
		"illegal goods"
)");
			trade.Load(node);
			THEN( "regular commodities are stored separately" ) {
				REQUIRE( trade.Commodities().size() == 2 );
				CHECK( trade.Commodities()[0].name == "Food" );
				CHECK( trade.Commodities()[1].name == "Industrial" );
			}
			AND_THEN( "special commodities are stored separately" ) {
				REQUIRE( trade.SpecialCommodities().size() == 2 );
				CHECK( trade.SpecialCommodities()[0].name == "Mission Cargo" );
				CHECK( trade.SpecialCommodities()[1].name == "Contraband" );
			}
		}
	}
}

SCENARIO( "Loading commodity with edge cases", "[Trade][Load][Edge]" ) {
	GIVEN( "an empty Trade instance" ) {
		Trade trade;
		WHEN( "loading a commodity without items" ) {
			auto node = AsDataNode(R"(
trade
	commodity "Food" 50 150
)");
			trade.Load(node);
			THEN( "the commodity is added with empty items" ) {
				REQUIRE( trade.Commodities().size() == 1 );
				const auto &commodity = trade.Commodities()[0];
				CHECK( commodity.name == "Food" );
				CHECK( commodity.low == 50 );
				CHECK( commodity.high == 150 );
				CHECK( commodity.items.empty() );
			}
		}
		WHEN( "loading a special commodity without items" ) {
			auto node = AsDataNode(R"(
trade
	commodity "Mission Cargo"
)");
			trade.Load(node);
			THEN( "the special commodity is added with empty items" ) {
				REQUIRE( trade.SpecialCommodities().size() == 1 );
				const auto &commodity = trade.SpecialCommodities()[0];
				CHECK( commodity.name == "Mission Cargo" );
				CHECK( commodity.items.empty() );
			}
		}
		WHEN( "loading with empty trade node" ) {
			auto node = AsDataNode("trade");
			trade.Load(node);
			THEN( "no commodities are added" ) {
				CHECK( trade.Commodities().empty() );
				CHECK( trade.SpecialCommodities().empty() );
			}
		}
	}
}

SCENARIO( "Trade::Commodities returns const reference", "[Trade][Commodities]" ) {
	GIVEN( "a Trade instance with commodities" ) {
		Trade trade;
		auto node = AsDataNode(R"(
trade
	commodity "Food" 50 150
)");
		trade.Load(node);
		WHEN( "accessing Commodities" ) {
			const auto &commodities = trade.Commodities();
			THEN( "the reference is valid and contains expected data" ) {
				REQUIRE( commodities.size() == 1 );
				CHECK( commodities[0].name == "Food" );
			}
		}
	}
}

SCENARIO( "Trade::SpecialCommodities returns const reference", "[Trade][SpecialCommodities]" ) {
	GIVEN( "a Trade instance with special commodities" ) {
		Trade trade;
		auto node = AsDataNode(R"(
trade
	commodity "Mission Cargo"
		"documents"
)");
		trade.Load(node);
		WHEN( "accessing SpecialCommodities" ) {
			const auto &specialCommodities = trade.SpecialCommodities();
			THEN( "the reference is valid and contains expected data" ) {
				REQUIRE( specialCommodities.size() == 1 );
				CHECK( specialCommodities[0].name == "Mission Cargo" );
			}
		}
	}
}

// #endregion unit tests



} // test namespace