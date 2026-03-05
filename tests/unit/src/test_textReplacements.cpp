/* test_textReplacements.cpp
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
#include "../../../source/TextReplacements.h"

// Include a helper for creating well-formed DataNodes.
#include "datanode-factory.h"
// Include ConditionsStore for testing conditional substitutions.
#include "../../../source/ConditionsStore.h"
// Include ConditionSet for creating conditions.
#include "../../../source/ConditionSet.h"

// ... and any system includes needed for the test file.
#include <map>
#include <string>

namespace { // test namespace

// #region mock data



// #endregion mock data



// #region unit tests
SCENARIO( "Creating a TextReplacements object", "[TextReplacements][Creation]" )
{
	GIVEN( "no arguments" )
	{
		const auto replacements = TextReplacements{};

		WHEN( "getting substitutions" )
		{
			std::map<std::string, std::string> subs;
			replacements.Substitutions(subs);

			THEN( "no substitutions are added" )
			{
				REQUIRE( subs.empty() );
			}
		}
	}
}

SCENARIO( "Loading text replacements", "[TextReplacements][Load]" )
{
	GIVEN( "an empty substitutions node" )
	{
		ConditionsStore store;
		auto node = AsDataNode("substitutions");
		auto replacements = TextReplacements{};

		WHEN( "loading the node" )
		{
			replacements.Load(node, &store);

			THEN( "no substitutions are added" )
			{
				std::map<std::string, std::string> subs;
				replacements.Substitutions(subs);
				REQUIRE( subs.empty() );
			}
		}
	}

	GIVEN( "a node with simple key-value pairs" )
	{
		ConditionsStore store;
		auto node = AsDataNode("substitutions\n\t<name> value1\n\t<title> value2");
		auto replacements = TextReplacements{};

		WHEN( "loading the node" )
		{
			replacements.Load(node, &store);

			THEN( "the substitutions are added" )
			{
				std::map<std::string, std::string> subs;
				replacements.Substitutions(subs);
				REQUIRE( subs.size() == 2 );
				REQUIRE( subs["<name>"] == "value1" );
				REQUIRE( subs["<title>"] == "value2" );
			}
		}
	}

	GIVEN( "a node with keys missing angle brackets" )
	{
		ConditionsStore store;
		auto node = AsDataNode("substitutions\n\tname value1\ntitle value2");
		auto replacements = TextReplacements{};

		WHEN( "loading the node" )
		{
			replacements.Load(node, &store);

			THEN( "the angle brackets are added automatically" )
			{
				std::map<std::string, std::string> subs;
				replacements.Substitutions(subs);
				REQUIRE( subs.size() == 2 );
				REQUIRE( subs["<name>"] == "value1" );
				REQUIRE( subs["<title>"] == "value2" );
			}
		}
	}

	GIVEN( "a node with reserved keys" )
	{
		ConditionsStore store;
		auto node = AsDataNode("substitutions\n\t<first> value1\n\t<last> value2\n\t<ship> value3\n\t<model> value4\n\t<flagship> value5\n\t<flagship model> value6");
		auto replacements = TextReplacements{};

		WHEN( "loading the node" )
		{
			replacements.Load(node, &store);

			THEN( "reserved keys are skipped" )
			{
				std::map<std::string, std::string> subs;
				replacements.Substitutions(subs);
				REQUIRE( subs.empty() );
			}
		}
	}

	GIVEN( "a node with a mix of reserved and non-reserved keys" )
	{
		ConditionsStore store;
		auto node = AsDataNode("substitutions\n\t<first> value1\n\t<custom> value2\n\t<ship> value3");
		auto replacements = TextReplacements{};

		WHEN( "loading the node" )
		{
			replacements.Load(node, &store);

			THEN( "only non-reserved keys are added" )
			{
				std::map<std::string, std::string> subs;
				replacements.Substitutions(subs);
				REQUIRE( subs.size() == 1 );
				REQUIRE( subs["<custom>"] == "value2" );
			}
		}
	}

	GIVEN( "a node with a conditional substitution that passes" )
	{
		ConditionsStore store;
		store.Set("testCondition", 1);
		auto node = AsDataNode("substitutions\n\t<name> value1\n\t\ttestCondition");
		auto replacements = TextReplacements{};

		WHEN( "loading the node and getting substitutions" )
		{
			replacements.Load(node, &store);
			std::map<std::string, std::string> subs;
			replacements.Substitutions(subs);

			THEN( "the substitution is added because the condition passes" )
			{
				REQUIRE( subs.size() == 1 );
				REQUIRE( subs["<name>"] == "value1" );
			}
		}
	}

	GIVEN( "a node with a conditional substitution that fails" )
	{
		ConditionsStore store;
		store.Set("testCondition", 0);
		auto node = AsDataNode("substitutions\n\t<name> value1\n\t\ttestCondition");
		auto replacements = TextReplacements{};

		WHEN( "loading the node and getting substitutions" )
		{
			replacements.Load(node, &store);
			std::map<std::string, std::string> subs;
			replacements.Substitutions(subs);

			THEN( "the substitution is not added because the condition fails" )
			{
				REQUIRE( subs.empty() );
			}
		}
	}

	GIVEN( "multiple substitutions for the same key" )
	{
		ConditionsStore store;
		store.Set("condition1", 1);
		store.Set("condition2", 1);
		auto node = AsDataNode("substitutions\n\t<name> firstValue\n\t\tcondition1\n\t<name> secondValue\n\t\tcondition2");
		auto replacements = TextReplacements{};

		WHEN( "loading the node and getting substitutions" )
		{
			replacements.Load(node, &store);
			std::map<std::string, std::string> subs;
			replacements.Substitutions(subs);

			THEN( "all passing substitutions are evaluated and the last one wins" )
			{
				REQUIRE( subs.size() == 1 );
				REQUIRE( subs["<name>"] == "secondValue" );
			}
		}
	}

	GIVEN( "a node with entries missing replacement values" )
	{
		ConditionsStore store;
		auto node = AsDataNode("substitutions\n\t<name>\n\t<title> valid");
		auto replacements = TextReplacements{};

		WHEN( "loading the node" )
		{
			replacements.Load(node, &store);

			THEN( "entries without replacement values are skipped" )
			{
				std::map<std::string, std::string> subs;
				replacements.Substitutions(subs);
				REQUIRE( subs.size() == 1 );
				REQUIRE( subs["<title>"] == "valid" );
			}
		}
	}
}

SCENARIO( "Reverting text replacements", "[TextReplacements][Revert]" )
{
	GIVEN( "two TextReplacements objects with different content" )
	{
		ConditionsStore store;
		auto node1 = AsDataNode("substitutions\n\t<name> value1\n\t<title> title1");
		auto node2 = AsDataNode("substitutions\n\t<name> value2\n\t<other> otherValue");

		auto replacements1 = TextReplacements{};
		auto replacements2 = TextReplacements{};

		replacements1.Load(node1, &store);
		replacements2.Load(node2, &store);

		std::map<std::string, std::string> subs1;
		std::map<std::string, std::string> subs2;
		replacements1.Substitutions(subs1);
		replacements2.Substitutions(subs2);

		REQUIRE( subs1.size() == 2 );
		REQUIRE( subs2.size() == 2 );
		REQUIRE( subs1["<name>"] == "value1" );
		REQUIRE( subs2["<name>"] == "value2" );

		WHEN( "reverting one with the other" )
		{
			replacements1.Revert(replacements2);

			THEN( "the first object has the content of the second" )
			{
				std::map<std::string, std::string> subsAfter;
				replacements1.Substitutions(subsAfter);
				REQUIRE( subsAfter.size() == 2 );
				REQUIRE( subsAfter["<name>"] == "value2" );
				REQUIRE( subsAfter["<other>"] == "otherValue" );
			}
		}
	}

	GIVEN( "a TextReplacements with content and an empty one" )
	{
		ConditionsStore store;
		auto node = AsDataNode("substitutions\n\t<name> value1");
		auto replacements = TextReplacements{};
		replacements.Load(node, &store);

		auto emptyReplacements = TextReplacements{};

		std::map<std::string, std::string> subs;
		replacements.Substitutions(subs);
		REQUIRE( subs.size() == 1 );

		WHEN( "reverting with an empty object" )
		{
			replacements.Revert(emptyReplacements);

			THEN( "the object becomes empty" )
			{
				std::map<std::string, std::string> subsAfter;
				replacements.Substitutions(subsAfter);
				REQUIRE( subsAfter.empty() );
			}
		}
	}
}

SCENARIO( "Getting substitutions", "[TextReplacements][Substitutions]" )
{
	GIVEN( "a TextReplacements with multiple substitutions" )
	{
		ConditionsStore store;
		auto node = AsDataNode("substitutions\n\t<name> value1\n\t<title> value2\n\t<place> value3");
		auto replacements = TextReplacements{};
		replacements.Load(node, &store);

		WHEN( "calling Substitutions with an empty map" )
		{
			std::map<std::string, std::string> subs;
			replacements.Substitutions(subs);

			THEN( "all substitutions are added to the map" )
			{
				REQUIRE( subs.size() == 3 );
				REQUIRE( subs["<name>"] == "value1" );
				REQUIRE( subs["<title>"] == "value2" );
				REQUIRE( subs["<place>"] == "value3" );
			}
		}

		WHEN( "calling Substitutions with a non-empty map" )
		{
			std::map<std::string, std::string> subs;
			subs["<existing>"] = "existingValue";
			subs["<name>"] = "oldValue";
			replacements.Substitutions(subs);

			THEN( "the map contains both existing and new values, with overlaps overwritten" )
			{
				REQUIRE( subs.size() == 4 );
				REQUIRE( subs["<existing>"] == "existingValue" );
				REQUIRE( subs["<name>"] == "value1" );
				REQUIRE( subs["<title>"] == "value2" );
				REQUIRE( subs["<place>"] == "value3" );
			}
		}
	}

	GIVEN( "a TextReplacements with conditional substitutions" )
	{
		ConditionsStore store;
		store.Set("enabled", 1);
		store.Set("disabled", 0);

		auto node = AsDataNode("substitutions\n\t<enabledKey> enabledValue\n\t\tenabled\n\t<disabledKey> disabledValue\n\t\tdisabled\n\t<unconditionalKey> unconditionalValue");
		auto replacements = TextReplacements{};
		replacements.Load(node, &store);

		WHEN( "calling Substitutions" )
		{
			std::map<std::string, std::string> subs;
			replacements.Substitutions(subs);

			THEN( "only substitutions with passing conditions are added" )
			{
				REQUIRE( subs.size() == 2 );
				REQUIRE( subs["<enabledKey>"] == "enabledValue" );
				REQUIRE( subs["<unconditionalKey>"] == "unconditionalValue" );
				REQUIRE( subs.count("<disabledKey>") == 0 );
			}
		}
	}

	GIVEN( "multiple TextReplacements objects to merge" )
	{
		ConditionsStore store;
		auto node1 = AsDataNode("substitutions\n\t<name> value1\n\t<title> title1");
		auto node2 = AsDataNode("substitutions\n\t<name> value2\n\t<place> place2");

		auto replacements1 = TextReplacements{};
		auto replacements2 = TextReplacements{};
		replacements1.Load(node1, &store);
		replacements2.Load(node2, &store);

		WHEN( "merging substitutions from multiple sources" )
		{
			std::map<std::string, std::string> subs;
			replacements1.Substitutions(subs);
			replacements2.Substitutions(subs);

			THEN( "the map contains all unique keys with later values overwriting earlier ones" )
			{
				REQUIRE( subs.size() == 3 );
				REQUIRE( subs["<name>"] == "value2" );
				REQUIRE( subs["<title>"] == "title1" );
				REQUIRE( subs["<place>"] == "place2" );
			}
		}
	}
}

// #endregion unit tests



} // test namespace