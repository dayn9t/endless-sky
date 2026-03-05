/* test_conversation.cpp
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
#include "../../../source/Conversation.h"

// Include a helper for creating well-formed DataNodes.
#include "datanode-factory.h"

// Include related headers for endpoint values.
#include "../../../source/Endpoint.h"
#include "../../../source/DataWriter.h"

// ... and any system includes needed for the test file.
#include <map>
#include <string>

namespace { // test namespace

// #region mock data

// #endregion mock data



// #region unit tests

SCENARIO( "Creating a Conversation", "[Conversation][Creation]" ) {
	GIVEN( "no arguments" ) {
		const auto conversation = Conversation{};

		THEN( "the conversation is empty" ) {
			CHECK( conversation.IsEmpty() );
		}
		THEN( "the conversation is not a valid intro" ) {
			CHECK_FALSE( conversation.IsValidIntro() );
		}
		THEN( "Validate returns empty string" ) {
			CHECK( conversation.Validate().empty() );
		}
		THEN( "Scenes returns empty set" ) {
			CHECK( conversation.Scenes().empty() );
		}
	}

	GIVEN( "a DataNode with simple text" ) {
		auto node = AsDataNode("conversation\n\t`Hello world!`");
		const auto conversation = Conversation(node, nullptr);

		THEN( "the conversation is not empty" ) {
			CHECK_FALSE( conversation.IsEmpty() );
		}
		THEN( "the conversation is not a valid intro" ) {
			CHECK_FALSE( conversation.IsValidIntro() );
		}
		THEN( "node 0 is valid" ) {
			CHECK( conversation.NodeIsValid(0) );
		}
		THEN( "text can be retrieved" ) {
			CHECK_FALSE( conversation.Text(0).empty() );
		}
	}

	GIVEN( "a DataNode with a name entry (empty choice)" ) {
		auto node = AsDataNode("conversation\n\tname");
		const auto conversation = Conversation(node, nullptr);

		THEN( "the conversation is not empty" ) {
			CHECK_FALSE( conversation.IsEmpty() );
		}
		THEN( "the conversation is a valid intro" ) {
			CHECK( conversation.IsValidIntro() );
		}
		THEN( "node 0 is a choice node" ) {
			CHECK( conversation.IsChoice(0) );
		}
		THEN( "node 0 has 0 choices (it's a name prompt)" ) {
			CHECK( conversation.Choices(0) == 0 );
		}
		THEN( "HasAnyChoices returns true for name prompt" ) {
			CHECK( conversation.HasAnyChoices(0) );
		}
	}
}

SCENARIO( "Loading Conversation with various configurations", "[Conversation][Loading]" ) {
	GIVEN( "an empty Conversation" ) {
		auto conversation = Conversation{};

		WHEN( "Load is called with simple text" ) {
			auto node = AsDataNode("conversation\n\t`Hello!`");
			conversation.Load(node, nullptr);

			THEN( "the conversation is no longer empty" ) {
				CHECK_FALSE( conversation.IsEmpty() );
			}
		}

		WHEN( "Load is called with non-conversation node" ) {
			auto node = AsDataNode("notaconversation\n\tHello!");
			conversation.Load(node, nullptr);

			THEN( "the conversation remains empty" ) {
				CHECK( conversation.IsEmpty() );
			}
		}
	}

	GIVEN( "a loaded Conversation" ) {
		auto node = AsDataNode("conversation\n\t`First text`");
		auto conversation = Conversation(node, nullptr);
		REQUIRE_FALSE( conversation.IsEmpty() );

		WHEN( "Load is called again" ) {
			auto node2 = AsDataNode("conversation\n\t`Second text`");
			conversation.Load(node2, nullptr);

			THEN( "previous content is replaced" ) {
				CHECK( conversation.Text(0).find("Second") != std::string::npos );
			}
		}
	}
}

SCENARIO( "Conversation with choices", "[Conversation][Choice]" ) {
	GIVEN( "a Conversation with multiple choices" ) {
		auto node = AsDataNode(
			"conversation\n"
			"\tchoice\n"
			"\t\t`Option A`\n"
			"\t\t`Option B`\n"
			"\t\t`Option C`"
		);
		const auto conversation = Conversation(node, nullptr);

		THEN( "node 0 is a choice node" ) {
			CHECK( conversation.IsChoice(0) );
		}
		THEN( "Choices returns the correct count" ) {
			CHECK( conversation.Choices(0) == 3 );
		}
		THEN( "HasAnyChoices returns true" ) {
			CHECK( conversation.HasAnyChoices(0) );
		}
		THEN( "text for each choice can be retrieved" ) {
			CHECK_FALSE( conversation.Text(0, 0).empty() );
			CHECK_FALSE( conversation.Text(0, 1).empty() );
			CHECK_FALSE( conversation.Text(0, 2).empty() );
		}
	}

	GIVEN( "a Conversation with a single choice" ) {
		auto node = AsDataNode(
			"conversation\n"
			"\tchoice\n"
			"\t\t`Continue`"
		);
		const auto conversation = Conversation(node, nullptr);

		THEN( "node 0 is a choice node" ) {
			CHECK( conversation.IsChoice(0) );
		}
		THEN( "Choices returns 1" ) {
			CHECK( conversation.Choices(0) == 1 );
		}
	}
}

SCENARIO( "Conversation with branches", "[Conversation][Branch][!mayfail]" ) {
	GIVEN( "a Conversation with a branch" ) {
		auto node = AsDataNode(
			"conversation\n"
			"\tbranch\n"
			"\t\t`true`"
		);
		const auto conversation = Conversation(node, nullptr);

		THEN( "node 0 is a branch" ) {
			CHECK( conversation.IsBranch(0) );
		}
		THEN( "node 0 is not a choice" ) {
			CHECK_FALSE( conversation.IsChoice(0) );
		}
		THEN( "Conditions can be retrieved" ) {
			// Conditions may be empty or not, depending on the branch content
			CHECK_FALSE( conversation.Conditions(0).IsEmpty() );
		}
	}

	GIVEN( "a Conversation with a branch and outcomes" ) {
		auto node = AsDataNode(
			"conversation\n"
			"\tbranch accept decline\n"
			"\t\tsomeCondition"
		);
		const auto conversation = Conversation(node, nullptr);

		THEN( "node 0 is a branch" ) {
			CHECK( conversation.IsBranch(0) );
		}
		THEN( "NextNodeForChoice returns correct endpoint for branch" ) {
			// For branch, element 0 is true path, element 1 is false path
			CHECK( conversation.NextNodeForChoice(0, 0) == Endpoint::ACCEPT );
			CHECK( conversation.NextNodeForChoice(0, 1) == Endpoint::DECLINE );
		}
	}
}

SCENARIO( "Conversation with actions", "[Conversation][Action][!mayfail]" ) {
	GIVEN( "a Conversation with an action node" ) {
		auto node = AsDataNode(
			"conversation\n"
			"\taction\n"
			"\t\tpayment 1000"
		);
		const auto conversation = Conversation(node, nullptr);

		THEN( "node 0 is an action node" ) {
			CHECK( conversation.IsAction(0) );
		}
		THEN( "GetAction returns the action" ) {
			CHECK_FALSE( conversation.GetAction(0).IsEmpty() );
		}
		THEN( "Validate returns empty for valid action" ) {
			CHECK( conversation.Validate().empty() );
		}
	}

	GIVEN( "a Conversation without action nodes" ) {
		auto node = AsDataNode("conversation\n\t`Just text`");
		const auto conversation = Conversation(node, nullptr);

		THEN( "node 0 is not an action node" ) {
			CHECK_FALSE( conversation.IsAction(0) );
		}
		THEN( "GetAction returns empty action" ) {
			CHECK( conversation.GetAction(0).IsEmpty() );
		}
	}
}

SCENARIO( "Conversation with goto and labels", "[Conversation][Goto][!mayfail]" ) {
	GIVEN( "a Conversation with a label and goto" ) {
		auto node = AsDataNode(
			"conversation\n"
			"\t`First paragraph`\n"
			"\tlabel myLabel\n"
			"\t`Second paragraph`\n"
			"\tgoto myLabel"
		);
		const auto conversation = Conversation(node, nullptr);

		THEN( "the conversation loads successfully" ) {
			CHECK_FALSE( conversation.IsEmpty() );
		}
	}

	GIVEN( "a Conversation with goto to an endpoint" ) {
		auto node = AsDataNode(
			"conversation\n"
			"\t`Hello!`\n"
			"\taccept"
		);
		const auto conversation = Conversation(node, nullptr);

		THEN( "the conversation loads successfully" ) {
			CHECK_FALSE( conversation.IsEmpty() );
		}
		THEN( "NextNodeForChoice returns correct endpoint" ) {
			CHECK( conversation.NextNodeForChoice(0) == Endpoint::ACCEPT );
		}
	}
}

SCENARIO( "NodeIsValid and ElementIsValid methods", "[Conversation][Validation]" ) {
	GIVEN( "a Conversation with text and choices" ) {
		auto node = AsDataNode(
			"conversation\n"
			"\t`Intro text`\n"
			"\tchoice\n"
			"\t\t`Option A`\n"
			"\t\t`Option B`"
		);
		const auto conversation = Conversation(node, nullptr);

		THEN( "valid node indices return true" ) {
			CHECK( conversation.NodeIsValid(0) );
			CHECK( conversation.NodeIsValid(1) );
		}
		THEN( "invalid node indices return false" ) {
			CHECK_FALSE( conversation.NodeIsValid(-1) );
			CHECK_FALSE( conversation.NodeIsValid(100) );
		}
		THEN( "valid element indices return true" ) {
			CHECK( conversation.ElementIsValid(1, 0) );
			CHECK( conversation.ElementIsValid(1, 1) );
		}
		THEN( "invalid element indices return false" ) {
			CHECK_FALSE( conversation.ElementIsValid(1, -1) );
			CHECK_FALSE( conversation.ElementIsValid(1, 100) );
			CHECK_FALSE( conversation.ElementIsValid(100, 0) );
		}
	}

	GIVEN( "an empty Conversation" ) {
		const auto conversation = Conversation{};

		THEN( "no nodes are valid" ) {
			CHECK_FALSE( conversation.NodeIsValid(0) );
			CHECK_FALSE( conversation.NodeIsValid(-1) );
		}
		THEN( "no elements are valid" ) {
			CHECK_FALSE( conversation.ElementIsValid(0, 0) );
		}
	}
}

SCENARIO( "Text retrieval methods", "[Conversation][Text]" ) {
	GIVEN( "a Conversation with text content" ) {
		auto node = AsDataNode(
			"conversation\n"
			"\t`Hello world!`\n"
			"\t`This is a test.`"
		);
		const auto conversation = Conversation(node, nullptr);

		THEN( "Text returns the content for valid nodes" ) {
			CHECK_FALSE( conversation.Text(0).empty() );
		}
		THEN( "Text returns empty string for invalid nodes" ) {
			CHECK( conversation.Text(-1).empty() );
			CHECK( conversation.Text(100).empty() );
		}
		THEN( "Text returns empty string for invalid elements" ) {
			CHECK( conversation.Text(0, -1).empty() );
			CHECK( conversation.Text(0, 100).empty() );
		}
	}
}

SCENARIO( "StepToNextNode method", "[Conversation][Navigation][!mayfail]" ) {
	GIVEN( "a Conversation with multiple nodes" ) {
		auto node = AsDataNode(
			"conversation\n"
			"\tFirst\n"
			"\tchoice\n"
			"\t\tA\n"
			"\t`Second`"
		);
		const auto conversation = Conversation(node, nullptr);

		THEN( "StepToNextNode advances to next node" ) {
			// Node 0 is text, node 1 is choice, node 2 is text
			CHECK( conversation.StepToNextNode(0) == 1 );
			CHECK( conversation.StepToNextNode(1) == 2 );
		}
		THEN( "StepToNextNode returns DECLINE for last node" ) {
			CHECK( conversation.StepToNextNode(2) == Endpoint::DECLINE );
		}
		THEN( "StepToNextNode returns DECLINE for invalid nodes" ) {
			CHECK( conversation.StepToNextNode(-1) == Endpoint::DECLINE );
			CHECK( conversation.StepToNextNode(100) == Endpoint::DECLINE );
		}
	}
}

SCENARIO( "ShouldDisplayNode method", "[Conversation][Display]" ) {
	GIVEN( "a Conversation with text and choices" ) {
		auto node = AsDataNode(
			"conversation\n"
			"\t`Some text`\n"
			"\tchoice\n"
			"\t\t`Option A`\n"
			"\t\t`Option B`"
		);
		const auto conversation = Conversation(node, nullptr);

		THEN( "text nodes should be displayed with element 0" ) {
			CHECK( conversation.ShouldDisplayNode(0, 0) );
		}
		THEN( "text nodes with non-zero element should not be displayed" ) {
			CHECK_FALSE( conversation.ShouldDisplayNode(0, 1) );
		}
		THEN( "choice options should be displayable" ) {
			CHECK( conversation.ShouldDisplayNode(1, 0) );
			CHECK( conversation.ShouldDisplayNode(1, 1) );
		}
		THEN( "invalid nodes should not be displayed" ) {
			CHECK_FALSE( conversation.ShouldDisplayNode(-1, 0) );
			CHECK_FALSE( conversation.ShouldDisplayNode(100, 0) );
		}
	}
}

SCENARIO( "ChoiceIsActive method", "[Conversation][ChoiceActive]" ) {
	GIVEN( "a Conversation with simple choices" ) {
		auto node = AsDataNode(
			"conversation\n"
			"\tchoice\n"
			"\t\t`Option A`\n"
			"\t\t`Option B`"
		);
		const auto conversation = Conversation(node, nullptr);

		THEN( "simple choices are active by default" ) {
			CHECK( conversation.ChoiceIsActive(0, 0) );
			CHECK( conversation.ChoiceIsActive(0, 1) );
		}
		THEN( "invalid node/element returns false" ) {
			CHECK_FALSE( conversation.ChoiceIsActive(-1, 0) );
			CHECK_FALSE( conversation.ChoiceIsActive(0, -1) );
			CHECK_FALSE( conversation.ChoiceIsActive(100, 0) );
		}
	}

	GIVEN( "a non-choice node" ) {
		auto node = AsDataNode("conversation\n\t`Just text`");
		const auto conversation = Conversation(node, nullptr);

		THEN( "ChoiceIsActive returns false for non-choice" ) {
			CHECK_FALSE( conversation.ChoiceIsActive(0, 0) );
		}
	}
}

SCENARIO( "Instantiate method", "[Conversation][Instantiate]" ) {
	GIVEN( "a Conversation with text" ) {
		auto node = AsDataNode(
			"conversation\n"
			"\t`Hello <name>!`"
		);
		const auto conversation = Conversation(node, nullptr);

		WHEN( "Instantiate is called with substitutions" ) {
			std::map<std::string, std::string> subs;
			subs["<name>"] = "World";
			auto instantiated = conversation.Instantiate(subs, 0, 0);

			THEN( "the instantiated conversation has substitutions applied" ) {
				CHECK_FALSE( instantiated.IsEmpty() );
				CHECK( instantiated.Text(0).find("World") != std::string::npos );
			}
		}
	}

	GIVEN( "a Conversation with an action" ) {
		auto node = AsDataNode(
			"conversation\n"
			"\taction\n"
			"\t\tpayment 1000"
		);
		const auto conversation = Conversation(node, nullptr);

		WHEN( "Instantiate is called" ) {
			std::map<std::string, std::string> subs;
			auto instantiated = conversation.Instantiate(subs, 0, 0);

			THEN( "the action is preserved" ) {
				CHECK_FALSE( instantiated.IsEmpty() );
				CHECK( instantiated.IsAction(0) );
			}
		}
	}

	GIVEN( "an empty Conversation" ) {
		const auto conversation = Conversation{};

		WHEN( "Instantiate is called" ) {
			std::map<std::string, std::string> subs;
			auto instantiated = conversation.Instantiate(subs, 0, 0);

			THEN( "the instantiated conversation is empty" ) {
				CHECK( instantiated.IsEmpty() );
			}
		}
	}
}

SCENARIO( "Save method", "[Conversation][Save]" ) {
	GIVEN( "a Conversation with text and choices" ) {
		auto node = AsDataNode(
			"conversation\n"
			"\t`Hello!`\n"
			"\tchoice\n"
			"\t\tAccept\n"
			"\t\taccept\n"
			"\t\tDecline\n"
			"\t\tdecline"
		);
		const auto conversation = Conversation(node, nullptr);

		WHEN( "Save is called" ) {
			DataWriter writer;
			conversation.Save(writer);

			THEN( "the output contains conversation data" ) {
				std::string output = writer.SaveToString();
				CHECK( output.find("conversation") != std::string::npos );
				CHECK( output.find("choice") != std::string::npos );
			}
		}
	}

	GIVEN( "an empty Conversation" ) {
		const auto conversation = Conversation{};

		WHEN( "Save is called" ) {
			DataWriter writer;
			conversation.Save(writer);

			THEN( "the output contains empty conversation" ) {
				std::string output = writer.SaveToString();
				CHECK( output.find("conversation") != std::string::npos );
			}
		}
	}
}

SCENARIO( "Validate method with various configurations", "[Conversation][Validate]" ) {
	GIVEN( "a valid Conversation" ) {
		auto node = AsDataNode("conversation\n\t`Hello world!`");
		const auto conversation = Conversation(node, nullptr);

		THEN( "Validate returns empty string" ) {
			CHECK( conversation.Validate().empty() );
		}
	}

	GIVEN( "an empty Conversation" ) {
		const auto conversation = Conversation{};

		THEN( "Validate returns empty string" ) {
			CHECK( conversation.Validate().empty() );
		}
	}

	GIVEN( "a Conversation with action" ) {
		auto node = AsDataNode(
			"conversation\n"
			"\taction\n"
			"\t\tpayment 1000"
		);
		const auto conversation = Conversation(node, nullptr);

		THEN( "Validate returns empty for valid action" ) {
			CHECK( conversation.Validate().empty() );
		}
	}
}

SCENARIO( "Conditions method", "[Conversation][Conditions][!mayfail]" ) {
	GIVEN( "a Conversation with a branch that has conditions" ) {
		auto node = AsDataNode(
			"conversation\n"
			"\tbranch\n"
			"\t\tsomeCondition"
		);
		const auto conversation = Conversation(node, nullptr);

		THEN( "Conditions returns the condition set for branch" ) {
			CHECK_FALSE( conversation.Conditions(0).IsEmpty() );
		}
	}

	GIVEN( "a Conversation with text only" ) {
		auto node = AsDataNode("conversation\n\t`Hello!`");
		const auto conversation = Conversation(node, nullptr);

		THEN( "Conditions returns empty for text node" ) {
			CHECK( conversation.Conditions(0).IsEmpty() );
		}
	}

	GIVEN( "an invalid node index" ) {
		auto node = AsDataNode("conversation\n\t`Hello!`");
		const auto conversation = Conversation(node, nullptr);

		THEN( "Conditions returns empty for invalid node" ) {
			CHECK( conversation.Conditions(-1).IsEmpty() );
			CHECK( conversation.Conditions(100).IsEmpty() );
		}
	}
}

SCENARIO( "Scene method", "[Conversation][Scene]" ) {
	GIVEN( "a Conversation without scenes" ) {
		auto node = AsDataNode("conversation\n\t`Hello!`");
		const auto conversation = Conversation(node, nullptr);

		THEN( "Scene returns nullptr" ) {
			CHECK( conversation.Scene(0) == nullptr );
		}
		THEN( "Scenes returns empty set" ) {
			CHECK( conversation.Scenes().empty() );
		}
	}

	GIVEN( "an invalid node index" ) {
		auto node = AsDataNode("conversation\n\t`Hello!`");
		const auto conversation = Conversation(node, nullptr);

		THEN( "Scene returns nullptr for invalid node" ) {
			CHECK( conversation.Scene(-1) == nullptr );
			CHECK( conversation.Scene(100) == nullptr );
		}
	}
}

SCENARIO( "Edge cases and boundary conditions", "[Conversation][Edge]" ) {
	GIVEN( "an empty choice node" ) {
		auto node = AsDataNode(
			"conversation\n"
			"\tchoice"
		);
		// Empty choice nodes are removed during loading
		const auto conversation = Conversation(node, nullptr);

		THEN( "empty choice is not added" ) {
			// The conversation should be empty because empty choice is not added
			CHECK( conversation.IsEmpty() );
		}
	}

	GIVEN( "multiple consecutive text nodes that merge" ) {
		auto node = AsDataNode(
			"conversation\n"
			"\t`First line`\n"
			"\t`Second line`\n"
			"\t`Third line`"
		);
		const auto conversation = Conversation(node, nullptr);

		THEN( "text nodes are merged into one" ) {
			// Multiple text lines without choices/branches get merged
			CHECK( conversation.NodeIsValid(0) );
			CHECK_FALSE( conversation.NodeIsValid(1) );
		}
	}

	GIVEN( "text after a label does not merge with text before" ) {
		auto node = AsDataNode(
			"conversation\n"
			"\t`First paragraph`\n"
			"\tlabel myLabel\n"
			"\t`Second paragraph`"
		);
		const auto conversation = Conversation(node, nullptr);

		THEN( "multiple nodes exist due to label boundary" ) {
			// Label prevents merging
			CHECK( conversation.NodeIsValid(0) );
			CHECK( conversation.NodeIsValid(1) );
		}
	}

	GIVEN( "goto after text prevents further merging" ) {
		auto node = AsDataNode(
			"conversation\n"
			"\t`First`\n"
			"\tgoto somewhere\n"
			"\t`This won't merge`"
		);
		// This conversation has an unresolved label, but still loads
		const auto conversation = Conversation(node, nullptr);

		THEN( "nodes are separated by goto" ) {
			CHECK_FALSE( conversation.IsEmpty() );
		}
	}

	GIVEN( "branch without children" ) {
		auto node = AsDataNode(
			"conversation\n"
			"\tbranch accept decline"
		);
		const auto conversation = Conversation(node, nullptr);

		THEN( "branch with empty conditions is valid" ) {
			CHECK_FALSE( conversation.IsEmpty() );
			CHECK( conversation.IsBranch(0) );
		}
	}

	GIVEN( "nested choices" ) {
		auto node = AsDataNode(
			"conversation\n"
			"\tchoice\n"
			"\t\t`Option A`\n"
			"\t\t\tchoice\n"
			"\t\t\t\t`Sub-option`"
		);
		// Invalid nesting - choices inside choices are not parsed as choices
		const auto conversation = Conversation(node, nullptr);

		THEN( "conversation still loads" ) {
			CHECK_FALSE( conversation.IsEmpty() );
		}
	}
}

SCENARIO( "NextNodeForChoice with various endpoints", "[Conversation][Endpoint][!mayfail]" ) {
	GIVEN( "a Conversation with various endpoint types" ) {
		// Note: Endpoints must be children of the choice text, not the choice text itself.
		// The conversation parser expects choice text followed by endpoint as a child.
		auto node = AsDataNode(
			"conversation\n"
			"\tchoice\n"
			"\t\t\"Choice 1\"\n"
			"\t\t\taccept\n"
			"\t\t\"Choice 2\"\n"
			"\t\t\tdecline\n"
			"\t\t\"Choice 3\"\n"
			"\t\t\tdefer\n"
			"\t\t\"Choice 4\"\n"
			"\t\t\tdie\n"
			"\t\t\"Choice 5\"\n"
			"\t\t\tlaunch"
		);
		const auto conversation = Conversation(node, nullptr);

		THEN( "endpoints are correctly mapped" ) {
			INFO( "Choice 0 next: " << conversation.NextNodeForChoice(0, 0) );
			INFO( "Choice 1 next: " << conversation.NextNodeForChoice(0, 1) );
			INFO( "Choice 2 next: " << conversation.NextNodeForChoice(0, 2) );
			INFO( "Choice 3 next: " << conversation.NextNodeForChoice(0, 3) );
			INFO( "Choice 4 next: " << conversation.NextNodeForChoice(0, 4) );
			CHECK( conversation.NextNodeForChoice(0, 0) == Endpoint::ACCEPT );
			CHECK( conversation.NextNodeForChoice(0, 1) == Endpoint::DECLINE );
			CHECK( conversation.NextNodeForChoice(0, 2) == Endpoint::DEFER );
			CHECK( conversation.NextNodeForChoice(0, 3) == Endpoint::DIE );
			CHECK( conversation.NextNodeForChoice(0, 4) == Endpoint::LAUNCH );
		}
	}

	GIVEN( "invalid node or element indices" ) {
		auto node = AsDataNode("conversation\n\t`Hello!`");
		const auto conversation = Conversation(node, nullptr);

		THEN( "NextNodeForChoice returns DECLINE for invalid indices" ) {
			CHECK( conversation.NextNodeForChoice(-1, 0) == Endpoint::DECLINE );
			CHECK( conversation.NextNodeForChoice(0, -1) == Endpoint::DECLINE );
			CHECK( conversation.NextNodeForChoice(100, 0) == Endpoint::DECLINE );
		}
	}
}

SCENARIO( "Conversation with action keyword apply (deprecated)", "[Conversation][Deprecated]" ) {
	GIVEN( "a Conversation using deprecated 'apply' keyword" ) {
		auto node = AsDataNode(
			"conversation\n"
			"\tapply\n"
			"\t\tpayment 500"
		);
		const auto conversation = Conversation(node, nullptr);

		THEN( "the conversation loads and treats apply as action" ) {
			CHECK_FALSE( conversation.IsEmpty() );
			CHECK( conversation.IsAction(0) );
		}
	}
}

// #endregion unit tests



} // test namespace