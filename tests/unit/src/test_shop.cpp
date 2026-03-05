/* test_shop.cpp
Copyright (c) 2025 by TomGoodIdea

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
#include "../../../source/Shop.h"

// Include helper for creating well-formed DataNodes.
#include "datanode-factory.h"
// Include a helper for handling logger output.
#include "logger-output.h"
// Include a helper for capturing & asserting on logged output.
#include "output-capture.hpp"

// Include dependencies needed for Shop.
#include "../../../source/ConditionsStore.h"
#include "../../../source/Set.h"

// ... and any system includes needed for the test file.
#include <set>
#include <string>

namespace { // test namespace

// #region mock data

// A minimal item class for testing the Shop template.
class MockItem {
public:
	std::string name;
	MockItem() = default;
	explicit MockItem(const std::string &n) : name(n) {}
};

// A simple Planet mock for testing CanStock.
// Since Planet is a complex class, we only need a pointer for CanStock testing.
// The actual location filter matching would require a full Planet object.

// #endregion mock data



// #region unit tests

SCENARIO( "Creating a Shop", "[Shop][Creation]" ) {

	OutputSink warnings(std::cerr);
	ConditionsStore store;

	GIVEN( "no arguments" ) {
		Shop<MockItem> shop;

		THEN( "the shop has default values" ) {
			CHECK( shop.Name().empty() );
			CHECK( shop.Stock().empty() );
		}
	}

	GIVEN( "a DataNode with only a name" ) {
		auto node = AsDataNode("shop Test Shop");
		Set<MockItem> items;
		std::set<const System *> visitedSystems;
		std::set<const Planet *> visitedPlanets;

		Shop<MockItem> shop(node, items, &store, &visitedSystems, &visitedPlanets);

		THEN( "the shop has the correct name" ) {
			CHECK( shop.Name() == "Test Shop" );
		}
		THEN( "the shop has empty stock" ) {
			CHECK( shop.Stock().empty() );
		}
	}

	GIVEN( "a DataNode with stock items" ) {
		auto node = AsDataNode("shop Outfitter\n"
			"\tstock\n"
			"\t\titem1\n"
			"\t\titem2");
		Set<MockItem> items;
		items.Get("item1");
		items.Get("item2");
		std::set<const System *> visitedSystems;
		std::set<const Planet *> visitedPlanets;

		Shop<MockItem> shop(node, items, &store, &visitedSystems, &visitedPlanets);

		THEN( "the shop contains the stock items" ) {
			CHECK( shop.Name() == "Outfitter" );
			CHECK( shop.Stock().size() == 2 );
			CHECK( shop.Stock().Has(items.Find("item1")) );
			CHECK( shop.Stock().Has(items.Find("item2")) );
		}
	}

	GIVEN( "a DataNode with inline stock items" ) {
		auto node = AsDataNode("shop Shipyard\n"
			"\tship1\n"
			"\tship2");
		Set<MockItem> items;
		items.Get("ship1");
		items.Get("ship2");
		std::set<const System *> visitedSystems;
		std::set<const Planet *> visitedPlanets;

		Shop<MockItem> shop(node, items, &store, &visitedSystems, &visitedPlanets);

		THEN( "the shop contains the inline stock items" ) {
			CHECK( shop.Name() == "Shipyard" );
			CHECK( shop.Stock().size() == 2 );
			CHECK( shop.Stock().Has(items.Find("ship1")) );
			CHECK( shop.Stock().Has(items.Find("ship2")) );
		}
	}
}

SCENARIO( "Loading a Shop with conditions and location filters", "[Shop][Loading]" ) {

	OutputSink warnings(std::cerr);
	ConditionsStore store;

	GIVEN( "a DataNode with a 'to sell' condition" ) {
		store.Set("reputation", 50);
		auto node = AsDataNode("shop Special Shop\n"
			"\tto sell\n"
			"\t\treputation >= 40");
		Set<MockItem> items;
		std::set<const System *> visitedSystems;
		std::set<const Planet *> visitedPlanets;

		Shop<MockItem> shop(node, items, &store, &visitedSystems, &visitedPlanets);

		THEN( "the shop can stock matching planets" ) {
			CHECK( shop.Name() == "Special Shop" );
			// CanStock returns true when condition passes and no location filter
			CHECK( shop.CanStock(nullptr) );
		}
	}

	GIVEN( "a DataNode with a 'never' condition" ) {
		auto node = AsDataNode("shop Never Shop\n"
			"\tto sell\n"
			"\t\tnever");
		Set<MockItem> items;
		std::set<const System *> visitedSystems;
		std::set<const Planet *> visitedPlanets;

		Shop<MockItem> shop(node, items, &store, &visitedSystems, &visitedPlanets);

		THEN( "the shop cannot stock any planet" ) {
			CHECK( shop.Name() == "Never Shop" );
			CHECK_FALSE( shop.CanStock(nullptr) );
		}
	}

	GIVEN( "a DataNode with both conditions and stock" ) {
		store.Set("license", 1);
		auto node = AsDataNode("shop Licensed Shop\n"
			"\tto sell\n"
			"\t\tlicense\n"
			"\tstock\n"
			"\t\titem1");
		Set<MockItem> items;
		items.Get("item1");
		std::set<const System *> visitedSystems;
		std::set<const Planet *> visitedPlanets;

		Shop<MockItem> shop(node, items, &store, &visitedSystems, &visitedPlanets);

		THEN( "the shop has both conditions and stock" ) {
			CHECK( shop.Name() == "Licensed Shop" );
			CHECK( shop.Stock().size() == 1 );
			CHECK( shop.CanStock(nullptr) );
		}
	}
}

SCENARIO( "Using CanStock to determine shop availability", "[Shop][CanStock]" ) {

	OutputSink warnings(std::cerr);
	ConditionsStore store;

	GIVEN( "a shop with no conditions or location filter" ) {
		auto node = AsDataNode("shop Basic Shop\n"
			"\tstock\n"
			"\t\titem1");
		Set<MockItem> items;
		items.Get("item1");
		std::set<const System *> visitedSystems;
		std::set<const Planet *> visitedPlanets;

		Shop<MockItem> shop(node, items, &store, &visitedSystems, &visitedPlanets);

		THEN( "CanStock returns false because it must be explicitly assigned" ) {
			CHECK_FALSE( shop.CanStock(nullptr) );
		}
	}

	GIVEN( "a shop with only a condition set" ) {
		store.Set("mission complete", 1);
		auto node = AsDataNode("shop Reward Shop\n"
			"\tto sell\n"
			"\t\tmission complete");
		Set<MockItem> items;
		std::set<const System *> visitedSystems;
		std::set<const Planet *> visitedPlanets;

		Shop<MockItem> shop(node, items, &store, &visitedSystems, &visitedPlanets);

		THEN( "CanStock returns true when condition passes" ) {
			CHECK( shop.CanStock(nullptr) );
		}

		WHEN( "the condition does not pass" ) {
			store.Set("mission complete", 0);

			THEN( "CanStock returns false" ) {
				CHECK_FALSE( shop.CanStock(nullptr) );
			}
		}
	}

	GIVEN( "a shop with a complex condition expression" ) {
		store.Set("reputation", 75);
		store.Set("combat rating", 500);
		auto node = AsDataNode("shop Elite Shop\n"
			"\tto sell\n"
			"\t\treputation >= 50\n"
			"\t\tcombat rating >= 300");
		Set<MockItem> items;
		std::set<const System *> visitedSystems;
		std::set<const Planet *> visitedPlanets;

		Shop<MockItem> shop(node, items, &store, &visitedSystems, &visitedPlanets);

		THEN( "CanStock returns true when all conditions pass" ) {
			CHECK( shop.CanStock(nullptr) );
		}

		WHEN( "only one condition passes" ) {
			store.Set("reputation", 30);

			THEN( "CanStock returns false" ) {
				CHECK_FALSE( shop.CanStock(nullptr) );
			}
		}
	}
}

SCENARIO( "Loading a Shop multiple times", "[Shop][Reloading]" ) {

	OutputSink warnings(std::cerr);
	ConditionsStore store;

	GIVEN( "a shop that is loaded twice" ) {
		Set<MockItem> items;
		items.Get("item1");
		items.Get("item2");
		items.Get("item3");
		std::set<const System *> visitedSystems;
		std::set<const Planet *> visitedPlanets;

		Shop<MockItem> shop;

		auto node1 = AsDataNode("shop Reload Shop\n"
			"\tstock\n"
			"\t\titem1\n"
			"\t\titem2");
		shop.Load(node1, items, &store, &visitedSystems, &visitedPlanets);

		REQUIRE( shop.Stock().size() == 2 );

		WHEN( "reloaded with a new stock node" ) {
			auto node2 = AsDataNode("shop Reload Shop\n"
				"\tstock\n"
				"\t\titem3");
			shop.Load(node2, items, &store, &visitedSystems, &visitedPlanets);

			THEN( "the stock is replaced" ) {
				CHECK( shop.Stock().size() == 1 );
				CHECK( shop.Stock().Has(items.Find("item3")) );
				CHECK_FALSE( shop.Stock().Has(items.Find("item1")) );
			}
		}

		WHEN( "reloaded with 'add' modifier" ) {
			auto node2 = AsDataNode("shop Reload Shop\n"
				"\tadd stock\n"
				"\t\titem3");
			shop.Load(node2, items, &store, &visitedSystems, &visitedPlanets);

			THEN( "the stock is added to existing items" ) {
				CHECK( shop.Stock().size() == 3 );
				CHECK( shop.Stock().Has(items.Find("item1")) );
				CHECK( shop.Stock().Has(items.Find("item2")) );
				CHECK( shop.Stock().Has(items.Find("item3")) );
			}
		}

		WHEN( "reloaded with 'remove' modifier" ) {
			auto node2 = AsDataNode("shop Reload Shop\n"
				"\tremove stock");
			shop.Load(node2, items, &store, &visitedSystems, &visitedPlanets);

			THEN( "the stock is cleared" ) {
				CHECK( shop.Stock().empty() );
			}
		}
	}
}

SCENARIO( "Removing shop conditions and location filters", "[Shop][Removal]" ) {

	OutputSink warnings(std::cerr);
	ConditionsStore store;
	store.Set("condition", 1);

	GIVEN( "a shop with conditions" ) {
		Set<MockItem> items;
		std::set<const System *> visitedSystems;
		std::set<const Planet *> visitedPlanets;

		auto node1 = AsDataNode("shop Conditional Shop\n"
			"\tto sell\n"
			"\t\tcondition");
		Shop<MockItem> shop(node1, items, &store, &visitedSystems, &visitedPlanets);

		REQUIRE( shop.CanStock(nullptr) );

		WHEN( "conditions are removed" ) {
			auto node2 = AsDataNode("shop Conditional Shop\n"
				"\tremove to sell");
			shop.Load(node2, items, &store, &visitedSystems, &visitedPlanets);

			THEN( "the shop can no longer stock dynamically" ) {
				CHECK_FALSE( shop.CanStock(nullptr) );
			}
		}
	}
}

SCENARIO( "Shop with empty DataNode children", "[Shop][EdgeCases]" ) {

	OutputSink warnings(std::cerr);
	ConditionsStore store;

	GIVEN( "a shop with an empty stock node" ) {
		auto node = AsDataNode("shop Empty Stock Shop\n"
			"\tstock");
		Set<MockItem> items;
		std::set<const System *> visitedSystems;
		std::set<const Planet *> visitedPlanets;

		Shop<MockItem> shop(node, items, &store, &visitedSystems, &visitedPlanets);

		THEN( "the shop has empty stock" ) {
			CHECK( shop.Name() == "Empty Stock Shop" );
			CHECK( shop.Stock().empty() );
		}
	}

	GIVEN( "a shop with an empty 'to sell' node" ) {
		auto node = AsDataNode("shop Empty Condition Shop\n"
			"\tto sell");
		Set<MockItem> items;
		std::set<const System *> visitedSystems;
		std::set<const Planet *> visitedPlanets;

		Shop<MockItem> shop(node, items, &store, &visitedSystems, &visitedPlanets);

		THEN( "the condition set is invalid" ) {
			// Empty condition set doesn't pass
			CHECK_FALSE( shop.CanStock(nullptr) );
		}
	}
}

SCENARIO( "Shop with nonexistent items in stock", "[Shop][NonexistentItems]" ) {

	OutputSink warnings(std::cerr);
	ConditionsStore store;

	GIVEN( "a shop that references items not in the Set" ) {
		auto node = AsDataNode("shop Missing Items Shop\n"
			"\tstock\n"
			"\t\tnonexistent1\n"
			"\t\tnonexistent2");
		Set<MockItem> items;
		// Note: We don't add the items to the set
		std::set<const System *> visitedSystems;
		std::set<const Planet *> visitedPlanets;

		Shop<MockItem> shop(node, items, &store, &visitedSystems, &visitedPlanets);

		THEN( "the shop still creates entries in the Set" ) {
			CHECK( shop.Name() == "Missing Items Shop" );
			// Set::Get creates items if they don't exist
			CHECK( shop.Stock().size() == 2 );
			CHECK( items.Has("nonexistent1") );
			CHECK( items.Has("nonexistent2") );
		}
	}
}

SCENARIO( "Shop class traits", "[Shop][Traits]" ) {
	using T = Shop<MockItem>;

	SECTION( "Class construction traits" ) {
		CHECK( std::is_default_constructible_v<T> );
		CHECK( std::is_nothrow_default_constructible_v<T> );
		CHECK( std::is_copy_constructible_v<T> );
		CHECK( std::is_move_constructible_v<T> );
	}

	SECTION( "Class assignment traits" ) {
		CHECK( std::is_copy_assignable_v<T> );
		CHECK( std::is_move_assignable_v<T> );
	}

	SECTION( "Class destructor traits" ) {
		CHECK( std::is_destructible_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
	}
}

// #endregion unit tests



} // test namespace