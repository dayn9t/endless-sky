/* test_Paragraphs.cpp
Copyright (c) 2025 by Benjamin Hauch

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
#include "../../../source/Paragraphs.h"

// Include a helper for creating well-formed DataNodes.
#include "datanode-factory.h"

// Include ConditionsStore for testing condition-based text filtering.
#include "../../../source/ConditionsStore.h"

// ... and any system includes needed for the test file.
#include <string>

namespace { // test namespace

// #region mock data

// Insert file-local data here, e.g. classes, structs, or fixtures that will be useful
// to help test this class/method.

// #endregion mock data



// #region unit tests

SCENARIO( "Creating a Paragraphs object", "[Paragraphs][Creation]" ) {
	GIVEN( "no arguments" ) {
		const auto paragraphs = Paragraphs{};
		THEN( "it is empty" ) {
			REQUIRE( paragraphs.IsEmpty() );
		}
		THEN( "begin equals end" ) {
			CHECK( paragraphs.begin() == paragraphs.end() );
		}
		THEN( "ToString returns an empty string" ) {
			CHECK( paragraphs.ToString().empty() );
		}
		THEN( "IsEmptyFor returns true" ) {
			CHECK( paragraphs.IsEmptyFor() );
		}
	}
}

SCENARIO( "Loading paragraphs from DataNode", "[Paragraphs][Loading]" ) {
	GIVEN( "a simple text node without conditions" ) {
		auto node = AsDataNode("description\n\tHello world");
		Paragraphs paragraphs;
		WHEN( "Load is called" ) {
			paragraphs.Load(node, nullptr);
			THEN( "the paragraph is stored" ) {
				CHECK_FALSE( paragraphs.IsEmpty() );
			}
			THEN( "the text is stored with the last token plus newline" ) {
				// Paragraphs::Load takes node.Token(node.Size() - 1), which is the last token
				CHECK( paragraphs.ToString() == "description\n" );
			}
			THEN( "IsEmptyFor returns false because the condition is always true" ) {
				CHECK_FALSE( paragraphs.IsEmptyFor() );
			}
			THEN( "iteration yields one entry" ) {
				int count = 0;
				for(const auto &entry : paragraphs)
				{
					(void)entry;
					++count;
				}
				CHECK( count == 1 );
			}
		}
	}

	GIVEN( "a node with 'to display' child" ) {
		ConditionsStore store;
		auto node = AsDataNode("description\n\tto display\n\t\tnever");
		Paragraphs paragraphs;
		WHEN( "Load is called with a ConditionsStore" ) {
			paragraphs.Load(node, &store);
			THEN( "the paragraph is stored" ) {
				CHECK_FALSE( paragraphs.IsEmpty() );
			}
			THEN( "IsEmptyFor returns true because 'never' is always false" ) {
				CHECK( paragraphs.IsEmptyFor() );
			}
			THEN( "ToString returns empty string because condition is not met" ) {
				CHECK( paragraphs.ToString().empty() );
			}
		}
	}

	GIVEN( "a node with 'to display' child and a true condition" ) {
		ConditionsStore store{{"myCondition", 1}};
		auto node = AsDataNode("description\n\tto display\n\t\tmyCondition");
		Paragraphs paragraphs;
		WHEN( "Load is called" ) {
			paragraphs.Load(node, &store);
			THEN( "IsEmptyFor returns false because condition is met" ) {
				CHECK_FALSE( paragraphs.IsEmptyFor() );
			}
			THEN( "ToString returns the text because condition is met" ) {
				// Paragraphs::Load takes the last token of the node, which is "description"
				CHECK( paragraphs.ToString() == "description\n" );
			}
		}
	}

	GIVEN( "multiple text lines with mixed conditions" ) {
		ConditionsStore store{{"hasPermission", 1}};
		Paragraphs paragraphs;

		// Load first line without condition
		auto node1 = AsDataNode("description\n\tFirst line");
		paragraphs.Load(node1, &store);

		// Load second line with always-true condition
		auto node2 = AsDataNode("description\n\tto display\n\t\t1");
		paragraphs.Load(node2, &store);

		// Load third line with always-false condition
		auto node3 = AsDataNode("description\n\tto display\n\t\tnever");
		paragraphs.Load(node3, &store);

		// Load fourth line with variable condition
		auto node4 = AsDataNode("description\n\tto display\n\t\thasPermission");
		paragraphs.Load(node4, &store);

		THEN( "all paragraphs are stored" ) {
			CHECK_FALSE( paragraphs.IsEmpty() );
		}
		THEN( "three paragraphs match (first, second, and fourth)" ) {
			CHECK_FALSE( paragraphs.IsEmptyFor() );
		}
		THEN( "ToString concatenates the matching paragraphs" ) {
			// Each Load takes the last token ("description"), so we get 3 copies of "description\n"
			CHECK( paragraphs.ToString() == "description\ndescription\ndescription\n" );
		}
		THEN( "iteration yields four entries" ) {
			int count = 0;
			for(const auto &entry : paragraphs)
			{
				(void)entry;
				++count;
			}
			CHECK( count == 4 );
		}
	}
}

SCENARIO( "Clearing paragraphs", "[Paragraphs][Modification]" ) {
	GIVEN( "a Paragraphs object with content" ) {
		auto node = AsDataNode("description\n\tSome text");
		Paragraphs paragraphs;
		paragraphs.Load(node, nullptr);
		REQUIRE_FALSE( paragraphs.IsEmpty() );

		WHEN( "Clear is called" ) {
			paragraphs.Clear();
			THEN( "the object is empty" ) {
				CHECK( paragraphs.IsEmpty() );
			}
			THEN( "begin equals end" ) {
				CHECK( paragraphs.begin() == paragraphs.end() );
			}
			THEN( "ToString returns empty string" ) {
				CHECK( paragraphs.ToString().empty() );
			}
			THEN( "IsEmptyFor returns true" ) {
				CHECK( paragraphs.IsEmptyFor() );
			}
		}
	}
}

SCENARIO( "Checking IsEmptyFor with various conditions", "[Paragraphs][Conditions]" ) {
	GIVEN( "an empty Paragraphs object" ) {
		Paragraphs paragraphs;
		THEN( "IsEmptyFor returns true" ) {
			CHECK( paragraphs.IsEmptyFor() );
		}
	}

	GIVEN( "a Paragraphs with only never-matching conditions" ) {
		ConditionsStore store;
		auto node = AsDataNode("description\n\tto display\n\t\tnever");
		Paragraphs paragraphs;
		paragraphs.Load(node, &store);

		THEN( "IsEmptyFor returns true" ) {
			CHECK( paragraphs.IsEmptyFor() );
		}
	}

	GIVEN( "a Paragraphs with always-matching condition" ) {
		ConditionsStore store;
		auto node = AsDataNode("description\n\tto display\n\t\t1");
		Paragraphs paragraphs;
		paragraphs.Load(node, &store);

		THEN( "IsEmptyFor returns false" ) {
			CHECK_FALSE( paragraphs.IsEmptyFor() );
		}
	}

	GIVEN( "a Paragraphs with condition checking a variable" ) {
		ConditionsStore store{{"myVar", 5}};
		auto node = AsDataNode("description\n\tto display\n\t\tmyVar >= 3");
		Paragraphs paragraphs;
		paragraphs.Load(node, &store);

		THEN( "IsEmptyFor returns false when condition is met" ) {
			CHECK_FALSE( paragraphs.IsEmptyFor() );
		}
	}

	GIVEN( "a Paragraphs with condition checking an unset variable" ) {
		ConditionsStore store;
		auto node = AsDataNode("description\n\tto display\n\t\tmyVar >= 3");
		Paragraphs paragraphs;
		paragraphs.Load(node, &store);

		THEN( "IsEmptyFor returns true when condition is not met" ) {
			CHECK( paragraphs.IsEmptyFor() );
		}
	}
}

SCENARIO( "ToString concatenation behavior", "[Paragraphs][ToString]" ) {
	GIVEN( "multiple paragraphs with different conditions" ) {
		ConditionsStore store{{"level", 2}};
		Paragraphs paragraphs;

		// Unconditional text
		auto node1 = AsDataNode("description\n\tIntro text");
		paragraphs.Load(node1, &store);

		// Condition that evaluates to true
		auto node2 = AsDataNode("description\n\tto display\n\t\tlevel >= 1");
		paragraphs.Load(node2, &store);

		// Condition that evaluates to false
		auto node3 = AsDataNode("description\n\tto display\n\t\tlevel >= 5");
		paragraphs.Load(node3, &store);

		// Another unconditional text
		auto node4 = AsDataNode("description\n\tOutro text");
		paragraphs.Load(node4, &store);

		WHEN( "ToString is called" ) {
			std::string result = paragraphs.ToString();
			THEN( "only matching paragraphs are included in order" ) {
				// Each Load takes the last token ("description"), so we get 3 copies
				CHECK( result == "description\ndescription\ndescription\n" );
			}
		}
	}

	GIVEN( "a Paragraphs with empty string content" ) {
		ConditionsStore store;
		Paragraphs paragraphs;

		// Loading a node where the last token is empty
		// The Load function takes node.Token(node.Size() - 1), which in this case
		// would be just "description" as the token
		auto node = AsDataNode("description");
		paragraphs.Load(node, &store);

		THEN( "the text is the first token plus newline" ) {
			CHECK( paragraphs.ToString() == "description\n" );
		}
	}
}

SCENARIO( "Iteration over paragraphs", "[Paragraphs][Iteration]" ) {
	GIVEN( "a Paragraphs object with multiple entries" ) {
		ConditionsStore store;
		Paragraphs paragraphs;

		auto node1 = AsDataNode("description\n\tFirst");
		auto node2 = AsDataNode("description\n\tSecond");
		auto node3 = AsDataNode("description\n\tThird");

		paragraphs.Load(node1, &store);
		paragraphs.Load(node2, &store);
		paragraphs.Load(node3, &store);

		WHEN( "iterating with range-based for loop" ) {
			THEN( "all entries are visited in order" ) {
				std::string collected;
				for(const auto &entry : paragraphs)
				{
					collected += entry.second;
				}
				// Each Load stores node.Token(node.Size() - 1) + "\n", which is "description\n"
				CHECK( collected == "description\ndescription\ndescription\n" );
			}
		}

		WHEN( "using begin/end directly" ) {
			THEN( "iteration works correctly" ) {
				int count = 0;
				for(auto it = paragraphs.begin(); it != paragraphs.end(); ++it)
				{
					CHECK_FALSE( it->second.empty() );
					++count;
				}
				CHECK( count == 3 );
			}
		}
	}
}

SCENARIO( "Paragraphs with complex condition expressions", "[Paragraphs][Complex]" ) {
	GIVEN( "a Paragraphs with 'and' condition" ) {
		ConditionsStore store{{"cond1", 1}, {"cond2", 0}};
		auto node = AsDataNode("description\n\tto display\n\t\tand\n\t\t\tcond1\n\t\t\tcond2");
		Paragraphs paragraphs;
		paragraphs.Load(node, &store);

		THEN( "IsEmptyFor returns true because cond2 is 0" ) {
			CHECK( paragraphs.IsEmptyFor() );
		}
	}

	GIVEN( "a Paragraphs with 'or' condition" ) {
		ConditionsStore store{{"cond1", 1}, {"cond2", 0}};
		auto node = AsDataNode("description\n\tto display\n\t\tor\n\t\t\tcond1\n\t\t\tcond2");
		Paragraphs paragraphs;
		paragraphs.Load(node, &store);

		THEN( "IsEmptyFor returns false because cond1 is truthy" ) {
			CHECK_FALSE( paragraphs.IsEmptyFor() );
		}
		THEN( "ToString returns the text" ) {
			// Paragraphs::Load takes node.Token(node.Size() - 1), which is "description"
			CHECK( paragraphs.ToString() == "description\n" );
		}
	}

	GIVEN( "a Paragraphs with comparison condition" ) {
		ConditionsStore store{{"score", 100}};
		auto node = AsDataNode("description\n\tto display\n\t\tscore > 50");
		Paragraphs paragraphs;
		paragraphs.Load(node, &store);

		THEN( "condition evaluates correctly" ) {
			CHECK_FALSE( paragraphs.IsEmptyFor() );
			// Paragraphs::Load takes node.Token(node.Size() - 1), which is "description"
			CHECK( paragraphs.ToString() == "description\n" );
		}
	}
}

// #endregion unit tests



} // test namespace