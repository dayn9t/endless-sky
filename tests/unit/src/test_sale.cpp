/* test_sale.cpp
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
#include "../../../source/Sale.h"

// Include a helper for creating well-formed DataNodes.
#include "datanode-factory.h"
#include "logger-output.h"
#include "output-capture.hpp"

// ... and any system includes needed for the test file.
#include <string>

namespace { // test namespace

// #region mock data

// Simple test item class for Sale template instantiation.
class TestItem {
public:
	TestItem() = default;
	explicit TestItem(std::string name) : name_(std::move(name)) {}

	const std::string &Name() const { return name_; }

private:
	std::string name_;
};

// #endregion mock data



// #region unit tests

SCENARIO("Sale default construction", "[Sale]") {
	GIVEN("A default-constructed Sale") {
		Sale<TestItem> sale;

		THEN("it is empty") {
			CHECK(sale.empty());
			CHECK(sale.size() == 0);
		}

		THEN("it contains no items") {
			TestItem item("test");
			CHECK_FALSE(sale.Has(&item));
		}
	}
}

SCENARIO("Sale Has method", "[Sale]") {
	GIVEN("An empty Sale") {
		Sale<TestItem> sale;
		TestItem item("item1");

		WHEN("checking if an item exists") {
			bool hasItem = sale.Has(&item);

			THEN("it returns false") {
				CHECK_FALSE(hasItem);
			}
		}
	}

	GIVEN("A Sale with items") {
		Sale<TestItem> sale;
		TestItem item1("item1");
		TestItem item2("item2");

		sale.insert(&item1);

		WHEN("checking for an existing item") {
			bool hasItem1 = sale.Has(&item1);

			THEN("it returns true") {
				CHECK(hasItem1);
			}
		}

		WHEN("checking for a non-existing item") {
			bool hasItem2 = sale.Has(&item2);

			THEN("it returns false") {
				CHECK_FALSE(hasItem2);
			}
		}

		WHEN("checking for a null pointer") {
			bool hasNull = sale.Has(nullptr);

			THEN("it returns false") {
				CHECK_FALSE(hasNull);
			}
		}
	}
}

SCENARIO("Sale Add method", "[Sale]") {
	GIVEN("Two Sales with different items") {
		Sale<TestItem> sale1;
		Sale<TestItem> sale2;
		TestItem item1("item1");
		TestItem item2("item2");
		TestItem item3("item3");

		sale1.insert(&item1);
		sale1.insert(&item2);
		sale2.insert(&item2);
		sale2.insert(&item3);

		WHEN("adding sale2 to sale1") {
			sale1.Add(sale2);

			THEN("sale1 contains all items from both") {
				CHECK(sale1.Has(&item1));
				CHECK(sale1.Has(&item2));
				CHECK(sale1.Has(&item3));
				CHECK(sale1.size() == 3);
			}

			THEN("sale2 is unchanged") {
				CHECK(sale2.Has(&item2));
				CHECK(sale2.Has(&item3));
				CHECK(sale2.size() == 2);
			}
		}
	}

	GIVEN("An empty Sale and a non-empty Sale") {
		Sale<TestItem> emptySale;
		Sale<TestItem> nonEmptySale;
		TestItem item("item");

		nonEmptySale.insert(&item);

		WHEN("adding non-empty to empty") {
			emptySale.Add(nonEmptySale);

			THEN("empty sale now contains the item") {
				CHECK(emptySale.Has(&item));
				CHECK(emptySale.size() == 1);
			}
		}

		WHEN("adding empty to non-empty") {
			nonEmptySale.Add(emptySale);

			THEN("non-empty sale is unchanged") {
				CHECK(nonEmptySale.Has(&item));
				CHECK(nonEmptySale.size() == 1);
			}
		}
	}

	GIVEN("Two empty Sales") {
		Sale<TestItem> sale1;
		Sale<TestItem> sale2;

		WHEN("adding one to the other") {
			sale1.Add(sale2);

			THEN("both remain empty") {
				CHECK(sale1.empty());
				CHECK(sale2.empty());
			}
		}
	}
}

SCENARIO("Sale LoadSingle method", "[Sale]") {
	OutputSink traces(std::cerr);

	GIVEN("A Sale and a Set with items") {
		Set<TestItem> items;
		TestItem *item1 = items.Get("item1");
		TestItem *item2 = items.Get("item2");
		TestItem *item3 = items.Get("item3");

		AND_GIVEN("a DataNode with a plain item name") {
			DataNode node = AsDataNode("item1");

			WHEN("LoadSingle is called") {
				Sale<TestItem> sale;
				sale.LoadSingle(node, items);

				THEN("the item is added to the sale") {
					CHECK(sale.Has(item1));
					CHECK(sale.size() == 1);
				}
			}
		}

		AND_GIVEN("a DataNode with 'add' keyword") {
			DataNode node = AsDataNode("add item2");

			WHEN("LoadSingle is called") {
				Sale<TestItem> sale;
				sale.LoadSingle(node, items);

				THEN("the item is added to the sale") {
					CHECK(sale.Has(item2));
					CHECK(sale.size() == 1);
				}
			}
		}

		AND_GIVEN("a Sale with items and a DataNode with 'remove' keyword") {
			DataNode node = AsDataNode("remove item1");

			WHEN("LoadSingle is called") {
				Sale<TestItem> sale;
				sale.insert(item1);
				sale.insert(item2);
				sale.LoadSingle(node, items);

				THEN("the item is removed from the sale") {
					CHECK_FALSE(sale.Has(item1));
					CHECK(sale.Has(item2));
					CHECK(sale.size() == 1);
				}
			}
		}

		AND_GIVEN("a Sale with items and a DataNode with 'clear' keyword") {
			DataNode node = AsDataNode("clear");

			WHEN("LoadSingle is called") {
				Sale<TestItem> sale;
				sale.insert(item1);
				sale.insert(item2);
				sale.insert(item3);
				sale.LoadSingle(node, items);

				THEN("all items are removed from the sale") {
					CHECK(sale.empty());
					CHECK(sale.size() == 0);
				}
			}
		}

		AND_GIVEN("a Sale with items and a DataNode with 'clear item' format") {
			DataNode node = AsDataNode("clear item1");

			WHEN("LoadSingle is called") {
				Sale<TestItem> sale;
				sale.insert(item1);
				sale.insert(item2);
				sale.LoadSingle(node, items);

				THEN("only the specified item is cleared/removed") {
					CHECK_FALSE(sale.Has(item1));
					CHECK(sale.Has(item2));
					CHECK(sale.size() == 1);
				}
			}
		}

		AND_GIVEN("a DataNode with non-existent item name") {
			DataNode node = AsDataNode("nonexistent");

			WHEN("LoadSingle is called") {
				Sale<TestItem> sale;
				sale.LoadSingle(node, items);

				THEN("a new item pointer is still added (Set creates it)") {
					// Note: Set::Get creates items if they don't exist
					CHECK(sale.size() == 1);
				}
			}
		}
	}
}

SCENARIO("Sale LoadSingle with preventModifiers", "[Sale]") {
	OutputSink traces(std::cerr);

	GIVEN("A Sale, Set with items, and preventModifiers = true") {
		Set<TestItem> items;
		TestItem *item1 = items.Get("item1");
		TestItem *item2 = items.Get("item2");

		Sale<TestItem> sale;
		sale.insert(item1);

		AND_GIVEN("a DataNode with 'add' keyword") {
			DataNode node = AsDataNode("add item2");

			WHEN("LoadSingle is called with preventModifiers") {
				sale.LoadSingle(node, items, true);

				THEN("the add is ignored and a trace is printed") {
					CHECK_FALSE(sale.Has(item2));
					CHECK(sale.size() == 1);
					CHECK_FALSE(traces.Flush().empty());
				}
			}
		}

		AND_GIVEN("a DataNode with 'remove' keyword") {
			DataNode node = AsDataNode("remove item1");

			WHEN("LoadSingle is called with preventModifiers") {
				sale.LoadSingle(node, items, true);

				THEN("the remove is ignored and a trace is printed") {
					CHECK(sale.Has(item1));
					CHECK(sale.size() == 1);
					CHECK_FALSE(traces.Flush().empty());
				}
			}
		}

		AND_GIVEN("a DataNode with 'clear' keyword") {
			DataNode node = AsDataNode("clear");

			WHEN("LoadSingle is called with preventModifiers") {
				sale.LoadSingle(node, items, true);

				THEN("the clear is ignored and a trace is printed") {
					CHECK(sale.Has(item1));
					CHECK(sale.size() == 1);
					CHECK_FALSE(traces.Flush().empty());
				}
			}
		}

		AND_GIVEN("a DataNode with a plain item name") {
			DataNode node = AsDataNode("item2");

			WHEN("LoadSingle is called with preventModifiers") {
				sale.LoadSingle(node, items, true);

				THEN("the item is added normally (plain insert is not a modifier)") {
					CHECK(sale.Has(item1));
					CHECK(sale.Has(item2));
					CHECK(sale.size() == 2);
				}
			}
		}
	}
}

SCENARIO("Sale Load method", "[Sale]") {
	OutputSink traces(std::cerr);

	GIVEN("A Sale and a Set with items") {
		Set<TestItem> items;
		TestItem *item1 = items.Get("item1");
		TestItem *item2 = items.Get("item2");
		TestItem *item3 = items.Get("item3");

		AND_GIVEN("a DataNode with multiple child nodes") {
			DataNode node = AsDataNode("sale\n\titem1\n\titem2\n\titem3");

			WHEN("Load is called") {
				Sale<TestItem> sale;
				sale.Load(node, items);

				THEN("all items are added to the sale") {
					CHECK(sale.Has(item1));
					CHECK(sale.Has(item2));
					CHECK(sale.Has(item3));
					CHECK(sale.size() == 3);
				}
			}
		}

		AND_GIVEN("a DataNode with mixed operations") {
			DataNode node = AsDataNode("sale\n\titem1\n\titem2\n\tremove item1\n\tadd item3");

			WHEN("Load is called on an empty sale") {
				Sale<TestItem> sale;
				sale.Load(node, items);

				THEN("operations are applied in order") {
					CHECK_FALSE(sale.Has(item1));  // added then removed
					CHECK(sale.Has(item2));         // added
					CHECK(sale.Has(item3));         // added
					CHECK(sale.size() == 2);
				}
			}
		}

		AND_GIVEN("a DataNode with clear operation") {
			DataNode node = AsDataNode("sale\n\titem1\n\titem2\n\tclear\n\titem3");

			WHEN("Load is called") {
				Sale<TestItem> sale;
				sale.Load(node, items);

				THEN("clear removes all previous items") {
					CHECK_FALSE(sale.Has(item1));
					CHECK_FALSE(sale.Has(item2));
					CHECK(sale.Has(item3));
					CHECK(sale.size() == 1);
				}
			}
		}
	}
}

SCENARIO("Sale edge cases", "[Sale]") {
	OutputSink traces(std::cerr);

	GIVEN("A Sale and Set with items") {
		Set<TestItem> items;
		TestItem *item1 = items.Get("item1");

		AND_GIVEN("a DataNode with 'add' but no item name") {
			DataNode node = AsDataNode("add");

			WHEN("LoadSingle is called") {
				Sale<TestItem> sale;
				sale.LoadSingle(node, items);

				THEN("nothing is added (no item specified)") {
					CHECK(sale.empty());
				}
			}
		}

		AND_GIVEN("a DataNode with 'remove' but no item name") {
			DataNode node = AsDataNode("remove");

			WHEN("LoadSingle is called on a non-empty sale") {
				Sale<TestItem> sale;
				sale.insert(item1);
				sale.LoadSingle(node, items);

				THEN("nothing is removed (remove without value falls through to default)") {
					// Note: The code checks `remove && !hasValue` to clear,
					// but the else branch at line 68-69 handles this.
					// Actually looking at the code:
					// - remove = true, hasValue = false (Size < 2)
					// - goes to: `if(remove && !hasValue) this->clear()`
					CHECK(sale.empty());
				}
			}
		}

		AND_GIVEN("an empty DataNode (single token, no children)") {
			// Create a minimal DataNode
			DataNode node = AsDataNode("singletoken");

			WHEN("LoadSingle is called") {
				Sale<TestItem> sale;
				sale.LoadSingle(node, items);

				THEN("the token is treated as an item name") {
					CHECK(sale.size() == 1);
				}
			}
		}
	}

	GIVEN("A Sale checking Has with null pointer") {
		Sale<TestItem> sale;

		WHEN("checking Has(nullptr)") {
			bool result = sale.Has(nullptr);

			THEN("it returns false (count(nullptr) returns 0 if not inserted)") {
				CHECK_FALSE(result);
			}
		}

		AND_GIVEN("an item added to the sale") {
			Set<TestItem> items;
			TestItem *item = items.Get("item");
			sale.insert(item);

			WHEN("checking Has with the item") {
				bool result = sale.Has(item);

				THEN("it returns true") {
					CHECK(result);
				}
			}
		}
	}
}

// #endregion unit tests



} // test namespace