/* test_randomEvent.cpp
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
#include "../../../source/RandomEvent.h"

// Include a helper for creating well-formed DataNodes.
#include "datanode-factory.h"

// Include ConditionsStore for testing condition-based triggering.
#include "../../../source/ConditionsStore.h"

// ... and any system includes needed for the test file.
#include <string>

namespace { // test namespace

// #region mock data

// A simple struct to use as the template parameter for RandomEvent testing.
struct TestEvent {
	int id = 0;
	std::string name;
};

// #endregion mock data



// #region unit tests

SCENARIO( "Creating a RandomEvent", "[RandomEvent][Creation]" ) {
	TestEvent testEvent{42, "test"};

	GIVEN( "a valid event pointer and positive period" ) {
		auto node = AsDataNode("event\n\tto spawn");
		ConditionsStore store;

		WHEN( "creating a RandomEvent with positive period" ) {
			RandomEvent<TestEvent> randomEvent(&testEvent, 100, node, &store);

			THEN( "Get returns the event pointer" ) {
				REQUIRE( randomEvent.Get() == &testEvent );
				REQUIRE( randomEvent.Get()->id == 42 );
				REQUIRE( randomEvent.Get()->name == "test" );
			}
			THEN( "Period returns the specified period" ) {
				REQUIRE( randomEvent.Period() == 100 );
			}
		}
	}

	GIVEN( "a null event pointer" ) {
		auto node = AsDataNode("event\n\tto spawn");
		ConditionsStore store;

		WHEN( "creating a RandomEvent with null event" ) {
			RandomEvent<TestEvent> randomEvent(nullptr, 100, node, &store);

			THEN( "Get returns nullptr" ) {
				REQUIRE( randomEvent.Get() == nullptr );
			}
			THEN( "Period still returns the specified period" ) {
				REQUIRE( randomEvent.Period() == 100 );
			}
		}
	}
}

SCENARIO( "RandomEvent period defaulting behavior", "[RandomEvent][Period]" ) {
	TestEvent testEvent{1, "test"};
	auto node = AsDataNode("event\n\tto spawn");
	ConditionsStore store;

	GIVEN( "a period of zero" ) {
		WHEN( "creating a RandomEvent" ) {
			RandomEvent<TestEvent> randomEvent(&testEvent, 0, node, &store);

			THEN( "Period defaults to 200" ) {
				REQUIRE( randomEvent.Period() == 200 );
			}
		}
	}

	GIVEN( "a negative period" ) {
		WHEN( "creating a RandomEvent" ) {
			RandomEvent<TestEvent> randomEvent(&testEvent, -50, node, &store);

			THEN( "Period defaults to 200" ) {
				REQUIRE( randomEvent.Period() == 200 );
			}
		}
	}

	GIVEN( "a positive period" ) {
		WHEN( "creating a RandomEvent" ) {
			RandomEvent<TestEvent> randomEvent(&testEvent, 300, node, &store);

			THEN( "Period returns the specified value" ) {
				REQUIRE( randomEvent.Period() == 300 );
			}
		}
	}

	GIVEN( "a period of 1" ) {
		WHEN( "creating a RandomEvent" ) {
			RandomEvent<TestEvent> randomEvent(&testEvent, 1, node, &store);

			THEN( "Period returns 1" ) {
				REQUIRE( randomEvent.Period() == 1 );
			}
		}
	}
}

SCENARIO( "RandomEvent CanTrigger with conditions", "[RandomEvent][CanTrigger]" ) {
	TestEvent testEvent{1, "test"};

	GIVEN( "a node without spawn conditions" ) {
		auto node = AsDataNode("event");
		ConditionsStore store;

		WHEN( "checking CanTrigger" ) {
			RandomEvent<TestEvent> randomEvent(&testEvent, 100, node, &store);

			THEN( "CanTrigger returns true (no conditions to check)" ) {
				REQUIRE( randomEvent.CanTrigger() );
			}
		}
	}

	GIVEN( "a node with 'to spawn' but no condition expression" ) {
		auto node = AsDataNode("event\n\tto spawn");
		ConditionsStore store;

		WHEN( "checking CanTrigger" ) {
			RandomEvent<TestEvent> randomEvent(&testEvent, 100, node, &store);

			THEN( "CanTrigger returns true (empty condition set)" ) {
				REQUIRE( randomEvent.CanTrigger() );
			}
		}
	}

	GIVEN( "a condition that is always true" ) {
		auto node = AsDataNode("event\n\tto spawn\n\t\tnever");
		ConditionsStore store;

		WHEN( "checking CanTrigger with 'never' condition" ) {
			RandomEvent<TestEvent> randomEvent(&testEvent, 100, node, &store);

			THEN( "CanTrigger returns false" ) {
				REQUIRE_FALSE( randomEvent.CanTrigger() );
			}
		}
	}

	GIVEN( "a condition checking a player condition that exists" ) {
		auto node = AsDataNode("event\n\tto spawn\n\t\tsomeCondition");
		ConditionsStore store;
		store["someCondition"] = 100;

		WHEN( "checking CanTrigger" ) {
			RandomEvent<TestEvent> randomEvent(&testEvent, 100, node, &store);

			THEN( "CanTrigger returns true when condition is non-zero" ) {
				REQUIRE( randomEvent.CanTrigger() );
			}
		}
	}

	GIVEN( "a condition checking a player condition that does not exist" ) {
		auto node = AsDataNode("event\n\tto spawn\n\t\tsomeCondition");
		ConditionsStore store;

		WHEN( "checking CanTrigger" ) {
			RandomEvent<TestEvent> randomEvent(&testEvent, 100, node, &store);

			THEN( "CanTrigger returns false when condition is missing (zero)" ) {
				REQUIRE_FALSE( randomEvent.CanTrigger() );
			}
		}
	}

	GIVEN( "a condition with comparison expression" ) {
		auto node = AsDataNode("event\n\tto spawn\n\t\tscore > 50");
		ConditionsStore store;

		WHEN( "condition is not met (score <= 50)" ) {
			store["score"] = 50;

			RandomEvent<TestEvent> randomEvent(&testEvent, 100, node, &store);

			THEN( "CanTrigger returns false" ) {
				REQUIRE_FALSE( randomEvent.CanTrigger() );
			}
		}

		WHEN( "condition is met (score > 50)" ) {
			store["score"] = 51;

			RandomEvent<TestEvent> randomEvent(&testEvent, 100, node, &store);

			THEN( "CanTrigger returns true" ) {
				REQUIRE( randomEvent.CanTrigger() );
			}
		}
	}

	GIVEN( "multiple conditions with 'and' logic" ) {
		auto node = AsDataNode("event\n\tto spawn\n\t\tcondA\n\t\tcondB");
		ConditionsStore store;

		WHEN( "only one condition is satisfied" ) {
			store["condA"] = 1;
			// condB is missing/zero

			RandomEvent<TestEvent> randomEvent(&testEvent, 100, node, &store);

			THEN( "CanTrigger returns false (both conditions needed)" ) {
				REQUIRE_FALSE( randomEvent.CanTrigger() );
			}
		}

		WHEN( "both conditions are satisfied" ) {
			store["condA"] = 1;
			store["condB"] = 1;

			RandomEvent<TestEvent> randomEvent(&testEvent, 100, node, &store);

			THEN( "CanTrigger returns true" ) {
				REQUIRE( randomEvent.CanTrigger() );
			}
		}
	}
}

SCENARIO( "RandomEvent ignores unrelated child nodes", "[RandomEvent][Parsing]" ) {
	TestEvent testEvent{1, "test"};
	ConditionsStore store;

	GIVEN( "a node with unrelated child tokens" ) {
		auto node = AsDataNode("event\n\tsomething else\n\tto spawn\n\t\tnever");

		WHEN( "creating a RandomEvent" ) {
			RandomEvent<TestEvent> randomEvent(&testEvent, 100, node, &store);

			THEN( "only 'to spawn' children are parsed as conditions" ) {
				REQUIRE_FALSE( randomEvent.CanTrigger() );
			}
		}
	}

	GIVEN( "a node with 'to spawn' child having wrong size" ) {
		auto node = AsDataNode("event\n\tto spawn extra");

		WHEN( "creating a RandomEvent" ) {
			RandomEvent<TestEvent> randomEvent(&testEvent, 100, node, &store);

			THEN( "the condition is not parsed (size != 2)" ) {
				REQUIRE( randomEvent.CanTrigger() );
			}
		}
	}
}

SCENARIO( "RandomEvent with null ConditionsStore", "[RandomEvent][NullStore]" ) {
	TestEvent testEvent{1, "test"};

	GIVEN( "a null ConditionsStore pointer" ) {
		auto node = AsDataNode("event\n\tto spawn\n\t\tsomeCondition");

		WHEN( "creating a RandomEvent with null store" ) {
			RandomEvent<TestEvent> randomEvent(&testEvent, 100, node, nullptr);

			THEN( "the object is still created successfully" ) {
				REQUIRE( randomEvent.Get() == &testEvent );
				REQUIRE( randomEvent.Period() == 100 );
			}
		}
	}
}

SCENARIO( "RandomEvent template instantiation with different types", "[RandomEvent][Template]" ) {
	ConditionsStore store;

	GIVEN( "RandomEvent with primitive type" ) {
		auto node = AsDataNode("event\n\tto spawn");
		int value = 42;

		WHEN( "creating RandomEvent<int>" ) {
			RandomEvent<int> randomEvent(&value, 50, node, &store);

			THEN( "it works correctly" ) {
				REQUIRE( randomEvent.Get() == &value );
				REQUIRE( *randomEvent.Get() == 42 );
				REQUIRE( randomEvent.Period() == 50 );
			}
		}
	}

	GIVEN( "RandomEvent with pointer type" ) {
		auto node = AsDataNode("event\n\tto spawn");
		double value = 3.14;

		WHEN( "creating RandomEvent<double>" ) {
			RandomEvent<double> randomEvent(&value, 75, node, &store);

			THEN( "it works correctly" ) {
				REQUIRE( randomEvent.Get() == &value );
				REQUIRE( *randomEvent.Get() == 3.14 );
				REQUIRE( randomEvent.Period() == 75 );
			}
		}
	}
}

// #endregion unit tests



} // test namespace