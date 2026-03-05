/* test_gameEvent.cpp
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
#include "../../../source/GameEvent.h"

// Include helper for creating well-formed DataNodes.
#include "datanode-factory.h"
// Include helper for handling logger output.
#include "logger-output.h"
// Include helper for capturing & asserting on logged output.
#include "output-capture.hpp"

// Include related headers needed for testing.
#include "../../../source/ConditionsStore.h"
#include "../../../source/DataWriter.h"
#include "../../../source/Date.h"

// ... and any system includes needed for the test file.
#include <list>
#include <map>
#include <set>
#include <string>

namespace { // test namespace

// #region mock data

// #endregion mock data



// #region unit tests

SCENARIO( "Creating a GameEvent", "[GameEvent][Creation]" ) {
	GIVEN( "no arguments" ) {
		WHEN( "default-constructing a GameEvent" ) {
			GameEvent event;
			THEN( "the event has default values" ) {
				CHECK( event.TrueName().empty() );
				CHECK( event.GetDate() == Date{} );
				CHECK_FALSE( event.SaveRawChanges() );
				CHECK( event.Changes().empty() );
			}
			THEN( "the event is not defined" ) {
				CHECK( event.IsValid() == "not defined" );
			}
		}
	}
}

SCENARIO( "Constructing GameEvent with DataNode", "[GameEvent][Creation]" ) {
	ConditionsStore store;
	OutputSink warnings(std::cerr);

	GIVEN( "a DataNode with a name" ) {
		WHEN( "creating a GameEvent with a valid name" ) {
			auto node = AsDataNode("event testEvent\n\tdate 15 3 3013");
			GameEvent event(node, &store);

			THEN( "the event is properly initialized" ) {
				CHECK( event.TrueName() == "testEvent" );
				CHECK( event.GetDate().Day() == 15 );
				CHECK( event.GetDate().Month() == 3 );
				CHECK( event.GetDate().Year() == 3013 );
			}
			THEN( "the event is valid" ) {
				CHECK( event.IsValid().empty() );
			}
		}
	}
	GIVEN( "a DataNode without a name" ) {
		WHEN( "creating a GameEvent with just 'event'" ) {
			auto node = AsDataNode("event\n\tdate 1 1 3013");
			GameEvent event(node, &store);

			THEN( "the event has no name" ) {
				CHECK( event.TrueName().empty() );
			}
			THEN( "the event is still valid" ) {
				CHECK( event.IsValid().empty() );
			}
		}
	}
	GIVEN( "a DataNode with condition assignments" ) {
		WHEN( "creating a GameEvent with conditions" ) {
			auto node = AsDataNode("event myEvent\n\tsomeCondition = 42");
			GameEvent event(node, &store);

			THEN( "the conditions are stored" ) {
				CHECK_FALSE( event.Conditions().IsEmpty() );
			}
		}
	}
	GIVEN( "a DataNode with 'save raw changes' flag" ) {
		WHEN( "creating a GameEvent with the flag" ) {
			// Note: The "save raw changes" is a single token that must be quoted
			auto node = AsDataNode("event myEvent\n\t`save raw changes`");
			GameEvent event(node, &store);

			THEN( "the flag is set" ) {
				CHECK( event.SaveRawChanges() );
			}
		}
	}
	GIVEN( "a DataNode without 'save raw changes' flag" ) {
		WHEN( "creating a GameEvent without the flag" ) {
			auto node = AsDataNode("event myEvent\n\tdate 1 1 3013");
			GameEvent event(node, &store);

			THEN( "the flag is not set" ) {
				CHECK_FALSE( event.SaveRawChanges() );
			}
		}
	}
}

SCENARIO( "Loading GameEvent from DataNode", "[GameEvent][Loading]" ) {
	ConditionsStore store;
	OutputSink warnings(std::cerr);

	GIVEN( "an empty GameEvent" ) {
		GameEvent event;

		WHEN( "loading from a DataNode with a date" ) {
			auto node = AsDataNode("event datedEvent\n\tdate 25 12 3015");
			event.Load(node, &store);

			THEN( "the date is set correctly" ) {
				CHECK( event.GetDate().Day() == 25 );
				CHECK( event.GetDate().Month() == 12 );
				CHECK( event.GetDate().Year() == 3015 );
			}
			THEN( "the name is set" ) {
				CHECK( event.TrueName() == "datedEvent" );
			}
		}
		WHEN( "loading from a DataNode with data changes" ) {
			auto node = AsDataNode("event changeEvent\n\tfleet newFleet\n\t\tsome data");
			event.Load(node, &store);

			THEN( "the changes are stored" ) {
				CHECK_FALSE( event.Changes().empty() );
			}
		}
	}
}

SCENARIO( "Setting and getting the event name", "[GameEvent][Name]" ) {
	GIVEN( "a GameEvent" ) {
		GameEvent event;

		WHEN( "setting the true name" ) {
			event.SetTrueName("customName");

			THEN( "the name can be retrieved" ) {
				CHECK( event.TrueName() == "customName" );
			}
		}
		WHEN( "setting an empty name" ) {
			event.SetTrueName("");

			THEN( "the name is empty" ) {
				CHECK( event.TrueName().empty() );
			}
		}
	}
	GIVEN( "a GameEvent with an existing name" ) {
		ConditionsStore store;
		auto node = AsDataNode("event originalName");
		GameEvent event(node, &store);
		REQUIRE( event.TrueName() == "originalName" );

		WHEN( "changing the name" ) {
			event.SetTrueName("newName");

			THEN( "the new name is returned" ) {
				CHECK( event.TrueName() == "newName" );
			}
		}
	}
}

SCENARIO( "Setting and getting the event date", "[GameEvent][Date]" ) {
	GIVEN( "a GameEvent" ) {
		GameEvent event;

		WHEN( "setting a specific date" ) {
			Date newDate(15, 6, 3020);
			event.SetDate(newDate);

			THEN( "the date can be retrieved" ) {
				CHECK( event.GetDate().Day() == 15 );
				CHECK( event.GetDate().Month() == 6 );
				CHECK( event.GetDate().Year() == 3020 );
			}
		}
		WHEN( "setting a date and then another" ) {
			event.SetDate(Date(1, 1, 3010));
			event.SetDate(Date(31, 12, 3025));

			THEN( "the last date is stored" ) {
				CHECK( event.GetDate().Day() == 31 );
				CHECK( event.GetDate().Month() == 12 );
				CHECK( event.GetDate().Year() == 3025 );
			}
		}
	}
	GIVEN( "a GameEvent with an existing date" ) {
		ConditionsStore store;
		auto node = AsDataNode("event datedEvent\n\tdate 10 5 3015");
		GameEvent event(node, &store);
		REQUIRE( event.GetDate().Day() == 10 );

		WHEN( "updating the date" ) {
			event.SetDate(Date(20, 8, 3020));

			THEN( "the new date is returned" ) {
				CHECK( event.GetDate().Day() == 20 );
				CHECK( event.GetDate().Month() == 8 );
				CHECK( event.GetDate().Year() == 3020 );
			}
		}
	}
}

SCENARIO( "Disabling a GameEvent", "[GameEvent][Disable]" ) {
	ConditionsStore store;
	OutputSink warnings(std::cerr);

	GIVEN( "an enabled GameEvent" ) {
		auto node = AsDataNode("event myEvent\n\tdate 1 1 3013");
		GameEvent event(node, &store);
		REQUIRE( event.IsValid().empty() );

		WHEN( "disabling the event" ) {
			event.Disable();

			THEN( "the event is still valid but won't save" ) {
				CHECK( event.IsValid().empty() );
			}
			THEN( "Save produces no output" ) {
				DataWriter writer;
				event.Save(writer);
				CHECK( writer.SaveToString().empty() );
			}
		}
	}
}

SCENARIO( "Saving a GameEvent", "[GameEvent][Save]" ) {
	ConditionsStore store;
	OutputSink warnings(std::cerr);

	GIVEN( "a GameEvent with date and name" ) {
		auto node = AsDataNode("event testEvent\n\tdate 15 3 3013");
		GameEvent event(node, &store);

		WHEN( "saving the event" ) {
			DataWriter writer;
			event.Save(writer);
			std::string output = writer.SaveToString();

			THEN( "the output contains the event structure" ) {
				CHECK_FALSE( output.empty() );
				CHECK( output.find("event") != std::string::npos );
				CHECK( output.find("date") != std::string::npos );
			}
		}
	}
	GIVEN( "a disabled GameEvent" ) {
		auto node = AsDataNode("event disabledEvent\n\tdate 1 1 3013");
		GameEvent event(node, &store);
		event.Disable();

		WHEN( "saving the event" ) {
			DataWriter writer;
			event.Save(writer);

			THEN( "nothing is written" ) {
				CHECK( writer.SaveToString().empty() );
			}
		}
	}
	GIVEN( "a GameEvent with condition assignments" ) {
		auto node = AsDataNode("event condEvent\n\tmyVar = 100");
		GameEvent event(node, &store);

		WHEN( "saving the event" ) {
			DataWriter writer;
			event.Save(writer);
			std::string output = writer.SaveToString();

			THEN( "the output is non-empty" ) {
				CHECK_FALSE( output.empty() );
			}
		}
	}
}

SCENARIO( "Comparing GameEvents by date", "[GameEvent][Comparison]" ) {
	GIVEN( "two GameEvents with different dates" ) {
		ConditionsStore store;
		auto node1 = AsDataNode("event earlier\n\tdate 1 1 3010");
		auto node2 = AsDataNode("event later\n\tdate 1 1 3020");
		GameEvent event1(node1, &store);
		GameEvent event2(node2, &store);

		WHEN( "comparing events" ) {
			THEN( "earlier event is less than later event" ) {
				CHECK( event1 < event2 );
				CHECK_FALSE( event2 < event1 );
			}
		}
	}
	GIVEN( "two GameEvents with the same date" ) {
		ConditionsStore store;
		auto node1 = AsDataNode("event first\n\tdate 15 6 3015");
		auto node2 = AsDataNode("event second\n\tdate 15 6 3015");
		GameEvent event1(node1, &store);
		GameEvent event2(node2, &store);

		WHEN( "comparing events" ) {
			THEN( "neither is less than the other" ) {
				CHECK_FALSE( event1 < event2 );
				CHECK_FALSE( event2 < event1 );
			}
		}
	}
	GIVEN( "GameEvents with default dates" ) {
		GameEvent event1;
		GameEvent event2;

		WHEN( "comparing events with no dates" ) {
			THEN( "both have equal default dates" ) {
				CHECK_FALSE( event1 < event2 );
				CHECK_FALSE( event2 < event1 );
			}
		}
	}
}

SCENARIO( "Validating a GameEvent", "[GameEvent][Validation]" ) {
	ConditionsStore store;
	OutputSink warnings(std::cerr);

	GIVEN( "a GameEvent loaded from DataNode" ) {
		auto node = AsDataNode("event validEvent\n\tdate 1 1 3013");
		GameEvent event(node, &store);

		WHEN( "checking validity" ) {
			THEN( "the event is valid" ) {
				CHECK( event.IsValid().empty() );
			}
		}
	}
	GIVEN( "a default-constructed GameEvent" ) {
		GameEvent event;

		WHEN( "checking validity" ) {
			THEN( "the event is not defined" ) {
				CHECK( event.IsValid() == "not defined" );
			}
		}
		WHEN( "loading the event from a DataNode" ) {
			auto node = AsDataNode("event nowDefined\n\tdate 1 1 3013");
			event.Load(node, &store);

			THEN( "the event becomes valid" ) {
				CHECK( event.IsValid().empty() );
			}
		}
	}
}

SCENARIO( "Getting changes from a GameEvent", "[GameEvent][Changes]" ) {
	ConditionsStore store;
	OutputSink warnings(std::cerr);

	GIVEN( "a GameEvent with data changes" ) {
		auto node = AsDataNode("event changeEvent\n\tfleet testFleet\n\t\tpos 100 200");
		GameEvent event(node, &store);

		WHEN( "getting the changes" ) {
			const auto& changes = event.Changes();

			THEN( "the changes list is not empty" ) {
				CHECK_FALSE( changes.empty() );
			}
			THEN( "the changes contain the fleet definition" ) {
				bool foundFleet = false;
				for(const auto& change : changes) {
					if(change.Size() >= 2 && change.Token(0) == "fleet") {
						foundFleet = true;
						break;
					}
				}
				CHECK( foundFleet );
			}
		}
	}
	GIVEN( "a GameEvent without data changes" ) {
		auto node = AsDataNode("event noChangeEvent\n\tdate 1 1 3013");
		GameEvent event(node, &store);

		WHEN( "getting the changes" ) {
			THEN( "the changes list is empty" ) {
				CHECK( event.Changes().empty() );
			}
		}
	}
}

SCENARIO( "Getting conditions from a GameEvent", "[GameEvent][Conditions]" ) {
	ConditionsStore store;
	OutputSink warnings(std::cerr);

	GIVEN( "a GameEvent with condition assignments" ) {
		auto node = AsDataNode("event condEvent\n\tmyCondition = 42");
		GameEvent event(node, &store);

		WHEN( "getting the conditions" ) {
			THEN( "the conditions are not empty" ) {
				CHECK_FALSE( event.Conditions().IsEmpty() );
			}
		}
	}
	GIVEN( "a GameEvent without condition assignments" ) {
		auto node = AsDataNode("event noCondEvent\n\tdate 1 1 3013");
		GameEvent event(node, &store);

		WHEN( "getting the conditions" ) {
			THEN( "the conditions may have the auto-set condition" ) {
				// Named events auto-add "event: <name>" condition
				CHECK_FALSE( event.Conditions().IsEmpty() );
			}
		}
	}
	GIVEN( "a GameEvent with a name (auto-sets condition)" ) {
		auto node = AsDataNode("event namedEvent");
		GameEvent event(node, &store);

		WHEN( "getting the conditions" ) {
			THEN( "the auto-set event condition is present" ) {
				CHECK_FALSE( event.Conditions().IsEmpty() );
			}
		}
	}
	GIVEN( "a GameEvent without a name" ) {
		auto node = AsDataNode("event\n\tdate 1 1 3013");
		GameEvent event(node, &store);

		WHEN( "getting the conditions" ) {
			THEN( "no auto-set condition is added" ) {
				CHECK( event.Conditions().IsEmpty() );
			}
		}
	}
}

SCENARIO( "Static DeferredDefinitions method", "[GameEvent][DeferredDefinitions]" ) {
	GIVEN( "an empty list of changes" ) {
		std::list<DataNode> changes;

		WHEN( "calling DeferredDefinitions" ) {
			auto definitions = GameEvent::DeferredDefinitions(changes);

			THEN( "no definitions are returned" ) {
				CHECK( definitions.empty() );
			}
		}
	}
	GIVEN( "changes with fleet definition" ) {
		auto nodes = AsDataNodes("fleet myFleet\n\tship \"Stock Ship\"");
		std::list<DataNode> changes(nodes.begin(), nodes.end());

		WHEN( "calling DeferredDefinitions" ) {
			auto definitions = GameEvent::DeferredDefinitions(changes);

			THEN( "the fleet definition is found" ) {
				CHECK( definitions.count("fleet") == 1 );
				CHECK( definitions["fleet"].count("myFleet") == 1 );
			}
		}
	}
	GIVEN( "changes with system definition including pos" ) {
		auto nodes = AsDataNodes("system mySystem\n\tpos 100 200");
		std::list<DataNode> changes(nodes.begin(), nodes.end());

		WHEN( "calling DeferredDefinitions" ) {
			auto definitions = GameEvent::DeferredDefinitions(changes);

			THEN( "the system definition is found" ) {
				CHECK( definitions.count("system") == 1 );
				CHECK( definitions["system"].count("mySystem") == 1 );
			}
		}
	}
	GIVEN( "changes with system definition without pos" ) {
		auto nodes = AsDataNodes("system mySystem\n\tgovernment Republic");
		std::list<DataNode> changes(nodes.begin(), nodes.end());

		WHEN( "calling DeferredDefinitions" ) {
			auto definitions = GameEvent::DeferredDefinitions(changes);

			THEN( "the system is not defined (no pos)" ) {
				CHECK( definitions.count("system") == 0 );
			}
		}
	}
	GIVEN( "changes with multiple definitions" ) {
		auto nodes = AsDataNodes(
			"fleet fleet1\n\tship ship1\n"
			"planet planet1\n\tspaceport description\n"
			"government testGov\n\thue 0.5"
		);
		std::list<DataNode> changes(nodes.begin(), nodes.end());

		WHEN( "calling DeferredDefinitions" ) {
			auto definitions = GameEvent::DeferredDefinitions(changes);

			THEN( "all definition types are found" ) {
				CHECK( definitions.count("fleet") == 1 );
				CHECK( definitions.count("planet") == 1 );
				CHECK( definitions.count("government") == 1 );
			}
		}
	}
	GIVEN( "changes with non-definition nodes" ) {
		auto nodes = AsDataNodes(
			"link system1 system2\n"
			"unlink system3 system4"
		);
		std::list<DataNode> changes(nodes.begin(), nodes.end());

		WHEN( "calling DeferredDefinitions" ) {
			auto definitions = GameEvent::DeferredDefinitions(changes);

			THEN( "no definitions are returned (link/unlink don't define)" ) {
				CHECK( definitions.empty() );
			}
		}
	}
}

// #endregion unit tests



} // test namespace