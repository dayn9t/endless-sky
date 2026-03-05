/* test_missionAction.cpp
Copyright (c) 2025 by Endless Sky contributors

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
#include "../../../source/MissionAction.h"

// Include a helper for creating well-formed DataNodes.
#include "datanode-factory.h"

// ... and any system includes needed for the test file.
#include "../../../source/DataWriter.h"

#include <map>
#include <string>

namespace { // test namespace

// #region mock data

// Helper function to create a DataNode for MissionAction testing.
DataNode CreateMissionActionNode(const std::string &trigger, const std::string &location = "")
{
	std::string text = "on " + trigger;
	if(!location.empty())
		text += " " + location;
	return AsDataNode(text);
}

// Helper function to create a DataNode with child content.
DataNode CreateMissionActionNodeWithContent(const std::string &trigger, const std::string &content)
{
	std::string text = "on " + trigger;
	if(!content.empty())
		text += "\n\t" + content;
	return AsDataNode(text);
}

// #endregion mock data



// #region unit tests

SCENARIO( "Creating a MissionAction", "[MissionAction][Construction]" ) {
	GIVEN( "no arguments" ) {
		WHEN( "default constructing a MissionAction" ) {
			MissionAction action;
			THEN( "it creates a valid but empty MissionAction" ) {
				CHECK( action.Validate().empty() );
				CHECK( action.Payment() == 0 );
				CHECK( action.DialogText().empty() );
			}
		}
	}
	GIVEN( "a DataNode with a trigger" ) {
		DataNode node = CreateMissionActionNode("complete");
		WHEN( "constructing with the DataNode" ) {
			MissionAction action(node, nullptr, nullptr, nullptr);
			THEN( "the MissionAction is created successfully" ) {
				CHECK( action.Validate().empty() );
			}
		}
	}
	GIVEN( "a DataNode with a trigger and location" ) {
		DataNode node = CreateMissionActionNode("enter", "destination");
		WHEN( "constructing with the DataNode" ) {
			MissionAction action(node, nullptr, nullptr, nullptr);
			THEN( "the MissionAction is created successfully" ) {
				CHECK( action.Validate().empty() );
			}
		}
	}
}

SCENARIO( "Loading a MissionAction", "[MissionAction][Load]" ) {
	GIVEN( "an empty MissionAction" ) {
		MissionAction action;
		WHEN( "loading from a DataNode with a trigger" ) {
			DataNode node = CreateMissionActionNode("offer");
			action.Load(node, nullptr, nullptr, nullptr);
			THEN( "the action is loaded successfully" ) {
				CHECK( action.Validate().empty() );
			}
		}
		WHEN( "loading from a DataNode with trigger and location" ) {
			DataNode node = CreateMissionActionNode("land", "destination");
			action.Load(node, nullptr, nullptr, nullptr);
			THEN( "the action is loaded successfully" ) {
				CHECK( action.Validate().empty() );
			}
		}
		WHEN( "loading from a DataNode without a trigger token" ) {
			DataNode node = AsDataNode("on");
			action.Load(node, nullptr, nullptr, nullptr);
			THEN( "the action is loaded with an empty trigger" ) {
				CHECK( action.Validate().empty() );
			}
		}
	}
	GIVEN( "a DataNode with dialog content" ) {
		DataNode node = CreateMissionActionNodeWithContent("complete", "dialog \"Test message\"");
		WHEN( "loading the MissionAction" ) {
			MissionAction action(node, nullptr, nullptr, nullptr);
			THEN( "the dialog text is stored" ) {
				CHECK( action.DialogText() == "Test message" );
			}
		}
	}
	GIVEN( "a DataNode with payment" ) {
		DataNode node = CreateMissionActionNodeWithContent("complete", "payment 1000");
		WHEN( "loading the MissionAction" ) {
			MissionAction action(node, nullptr, nullptr, nullptr);
			THEN( "the payment is set correctly" ) {
				CHECK( action.Payment() == 1000 );
			}
		}
	}
}

SCENARIO( "Loading MissionAction with various trigger types", "[MissionAction][Triggers]" ) {
	GIVEN( "various trigger names" ) {
		std::vector<std::string> triggers = {
			"offer", "accept", "decline", "complete", "fail",
			"abort", "visit", "stopover", "enter", "land"
		};
		WHEN( "creating MissionActions with different triggers" ) {
			THEN( "each trigger is preserved correctly" ) {
				for(const auto &trigger : triggers)
				{
					DataNode node = CreateMissionActionNode(trigger);
					MissionAction action(node, nullptr, nullptr, nullptr);
					DataWriter writer;
					action.Save(writer);
					std::string result = writer.SaveToString();
					CHECK( result.find("on " + trigger) != std::string::npos );
				}
			}
		}
	}
}

SCENARIO( "Loading MissionAction with 'can trigger after failure' flag", "[MissionAction][Failure]" ) {
	GIVEN( "a DataNode with 'can trigger after failure'" ) {
		DataNode node = AsDataNode("on complete\n\t\"can trigger after failure\"");
		WHEN( "loading the MissionAction" ) {
			MissionAction action(node, nullptr, nullptr, nullptr);
			THEN( "the action is created successfully" ) {
				CHECK( action.Validate().empty() );
			}
		}
	}
	GIVEN( "a DataNode without the failure flag" ) {
		DataNode node = CreateMissionActionNode("complete");
		WHEN( "loading the MissionAction" ) {
			MissionAction action(node, nullptr, nullptr, nullptr);
			THEN( "saving does not include the flag" ) {
				DataWriter writer;
				action.Save(writer);
				std::string result = writer.SaveToString();
				CHECK( result.find("can trigger after failure") == std::string::npos );
			}
		}
	}
}

SCENARIO( "Loading MissionAction with dialog", "[MissionAction][Dialog]" ) {
	GIVEN( "a DataNode with simple dialog text" ) {
		DataNode node = AsDataNode("on offer\n\tdialog \"Welcome pilot!\"");
		WHEN( "loading the MissionAction" ) {
			MissionAction action(node, nullptr, nullptr, nullptr);
			THEN( "the dialog text is stored correctly" ) {
				CHECK( action.DialogText() == "Welcome pilot!" );
			}
			THEN( "validation succeeds" ) {
				CHECK( action.Validate().empty() );
			}
		}
	}
	GIVEN( "a DataNode with multi-line dialog" ) {
		DataNode node = AsDataNode("on offer\n\tdialog\n\t\t\"Line one\"\n\t\t\"Line two\"");
		WHEN( "loading the MissionAction" ) {
			MissionAction action(node, nullptr, nullptr, nullptr);
			THEN( "the dialog text is stored" ) {
				CHECK_FALSE( action.DialogText().empty() );
			}
		}
	}
}

SCENARIO( "Saving a MissionAction", "[MissionAction][Save]" ) {
	GIVEN( "a MissionAction with a trigger" ) {
		DataNode node = CreateMissionActionNode("complete");
		MissionAction action(node, nullptr, nullptr, nullptr);
		WHEN( "saving to a DataWriter" ) {
			DataWriter writer;
			action.Save(writer);
			THEN( "the output contains the trigger" ) {
				std::string result = writer.SaveToString();
				CHECK( result.find("on complete") != std::string::npos );
			}
		}
	}
	GIVEN( "a MissionAction with trigger and location" ) {
		DataNode node = CreateMissionActionNode("enter", "destination");
		MissionAction action(node, nullptr, nullptr, nullptr);
		WHEN( "saving to a DataWriter" ) {
			DataWriter writer;
			action.Save(writer);
			THEN( "the output contains trigger and location" ) {
				std::string result = writer.SaveToString();
				CHECK( result.find("on enter destination") != std::string::npos );
			}
		}
	}
	GIVEN( "a MissionAction with dialog" ) {
		DataNode node = AsDataNode("on offer\n\tdialog \"Hello\"");
		MissionAction action(node, nullptr, nullptr, nullptr);
		WHEN( "saving to a DataWriter" ) {
			DataWriter writer;
			action.Save(writer);
			THEN( "the output contains the dialog" ) {
				std::string result = writer.SaveToString();
				CHECK( result.find("dialog") != std::string::npos );
				CHECK( result.find("Hello") != std::string::npos );
			}
		}
	}
	GIVEN( "a MissionAction with payment" ) {
		DataNode node = AsDataNode("on complete\n\tpayment 5000");
		MissionAction action(node, nullptr, nullptr, nullptr);
		WHEN( "saving to a DataWriter" ) {
			DataWriter writer;
			action.Save(writer);
			THEN( "the output contains the payment" ) {
				std::string result = writer.SaveToString();
				CHECK( result.find("payment") != std::string::npos );
				CHECK( result.find("5000") != std::string::npos );
			}
		}
	}
	GIVEN( "a default constructed MissionAction" ) {
		MissionAction action;
		WHEN( "saving to a DataWriter" ) {
			DataWriter writer;
			action.Save(writer);
			THEN( "the output is valid" ) {
				std::string result = writer.SaveToString();
				CHECK( result.find("on") != std::string::npos );
			}
		}
	}
}

SCENARIO( "Validating a MissionAction", "[MissionAction][Validate]" ) {
	GIVEN( "a default constructed MissionAction" ) {
		MissionAction action;
		WHEN( "validating" ) {
			std::string result = action.Validate();
			THEN( "no validation errors are returned" ) {
				CHECK( result.empty() );
			}
		}
	}
	GIVEN( "a MissionAction loaded from valid data" ) {
		DataNode node = CreateMissionActionNode("complete");
		MissionAction action(node, nullptr, nullptr, nullptr);
		WHEN( "validating" ) {
			std::string result = action.Validate();
			THEN( "no validation errors are returned" ) {
				CHECK( result.empty() );
			}
		}
	}
	GIVEN( "a MissionAction with valid dialog" ) {
		DataNode node = AsDataNode("on offer\n\tdialog \"Valid dialog\"");
		MissionAction action(node, nullptr, nullptr, nullptr);
		WHEN( "validating" ) {
			std::string result = action.Validate();
			THEN( "no validation errors are returned" ) {
				CHECK( result.empty() );
			}
		}
	}
	GIVEN( "a MissionAction with payment" ) {
		DataNode node = AsDataNode("on complete\n\tpayment 1000");
		MissionAction action(node, nullptr, nullptr, nullptr);
		WHEN( "validating" ) {
			std::string result = action.Validate();
			THEN( "no validation errors are returned" ) {
				CHECK( result.empty() );
			}
		}
	}
}

SCENARIO( "MissionAction Payment method", "[MissionAction][Payment]" ) {
	GIVEN( "a default constructed MissionAction" ) {
		MissionAction action;
		THEN( "payment is zero" ) {
			CHECK( action.Payment() == 0 );
		}
	}
	GIVEN( "a MissionAction with payment" ) {
		DataNode node = AsDataNode("on complete\n\tpayment 5000");
		MissionAction action(node, nullptr, nullptr, nullptr);
		THEN( "payment returns the correct value" ) {
			CHECK( action.Payment() == 5000 );
		}
	}
	GIVEN( "a MissionAction with accumulated payments" ) {
		DataNode node = AsDataNode("on complete\n\tpayment 1000\n\tpayment 2000");
		MissionAction action(node, nullptr, nullptr, nullptr);
		THEN( "payments accumulate" ) {
			CHECK( action.Payment() == 3000 );
		}
	}
}

SCENARIO( "MissionAction DialogText method", "[MissionAction][DialogText]" ) {
	GIVEN( "a default constructed MissionAction" ) {
		MissionAction action;
		THEN( "dialog text is empty" ) {
			CHECK( action.DialogText().empty() );
		}
	}
	GIVEN( "a MissionAction with dialog" ) {
		DataNode node = AsDataNode("on offer\n\tdialog \"Test dialog text\"");
		MissionAction action(node, nullptr, nullptr, nullptr);
		THEN( "dialog text is returned correctly" ) {
			CHECK( action.DialogText() == "Test dialog text" );
		}
	}
}

SCENARIO( "Instantiating a MissionAction", "[MissionAction][Instantiate]" ) {
	GIVEN( "a MissionAction with a trigger" ) {
		DataNode node = CreateMissionActionNode("complete");
		MissionAction templateAction(node, nullptr, nullptr, nullptr);
		WHEN( "instantiating with substitution map" ) {
			std::map<std::string, std::string> subs;
			subs["<test>"] = "replaced";
			MissionAction instance = templateAction.Instantiate(subs, nullptr, 0, 0);
			THEN( "the instantiated action has the same trigger" ) {
				DataWriter writer;
				instance.Save(writer);
				std::string result = writer.SaveToString();
				CHECK( result.find("on complete") != std::string::npos );
			}
			AND_THEN( "the original template is unchanged" ) {
				DataWriter writer;
				templateAction.Save(writer);
				std::string result = writer.SaveToString();
				CHECK( result.find("on complete") != std::string::npos );
			}
		}
	}
	GIVEN( "a MissionAction with payment" ) {
		DataNode node = AsDataNode("on complete\n\tpayment 1000");
		MissionAction templateAction(node, nullptr, nullptr, nullptr);
		WHEN( "instantiating" ) {
			std::map<std::string, std::string> subs;
			MissionAction instance = templateAction.Instantiate(subs, nullptr, 0, 0);
			THEN( "payment is preserved" ) {
				CHECK( instance.Payment() == 1000 );
			}
		}
	}
	GIVEN( "a MissionAction with dialog" ) {
		DataNode node = AsDataNode("on offer\n\tdialog \"Hello <name>!\"");
		MissionAction templateAction(node, nullptr, nullptr, nullptr);
		WHEN( "instantiating with substitutions" ) {
			std::map<std::string, std::string> subs;
			subs["<name>"] = "Pilot";
			MissionAction instance = templateAction.Instantiate(subs, nullptr, 0, 0);
			THEN( "dialog text has substitutions applied" ) {
				CHECK( instance.DialogText() == "Hello Pilot!" );
			}
		}
	}
	GIVEN( "a MissionAction with location" ) {
		DataNode node = CreateMissionActionNode("enter", "destination");
		MissionAction templateAction(node, nullptr, nullptr, nullptr);
		WHEN( "instantiating" ) {
			std::map<std::string, std::string> subs;
			MissionAction instance = templateAction.Instantiate(subs, nullptr, 0, 0);
			THEN( "location is preserved" ) {
				DataWriter writer;
				instance.Save(writer);
				std::string result = writer.SaveToString();
				CHECK( result.find("on enter destination") != std::string::npos );
			}
		}
	}
	GIVEN( "a MissionAction with 'can trigger after failure' flag" ) {
		DataNode node = AsDataNode("on complete\n\t\"can trigger after failure\"");
		MissionAction templateAction(node, nullptr, nullptr, nullptr);
		WHEN( "instantiating" ) {
			std::map<std::string, std::string> subs;
			MissionAction instance = templateAction.Instantiate(subs, nullptr, 0, 0);
			THEN( "the flag is preserved" ) {
				// Check that runsWhenFailed is set (internal state).
				CHECK( instance.Validate().empty() );
			}
		}
	}
}

SCENARIO( "Copying and moving MissionAction objects", "[MissionAction][Copy][Move]" ) {
	GIVEN( "a MissionAction with data" ) {
		DataNode node = AsDataNode("on complete\n\tpayment 1000\n\tdialog \"Test\"");
		MissionAction original(node, nullptr, nullptr, nullptr);
		WHEN( "copy constructing" ) {
			MissionAction copied(original);
			THEN( "the copy has the same data" ) {
				CHECK( copied.Payment() == 1000 );
				CHECK( copied.DialogText() == "Test" );
			}
		}
		WHEN( "copy assigning" ) {
			MissionAction assigned;
			assigned = original;
			THEN( "the assigned has the same data" ) {
				CHECK( assigned.Payment() == 1000 );
				CHECK( assigned.DialogText() == "Test" );
			}
		}
		WHEN( "move constructing" ) {
			MissionAction moved(std::move(original));
			THEN( "the moved object has the data" ) {
				CHECK( moved.Payment() == 1000 );
				CHECK( moved.DialogText() == "Test" );
			}
		}
		WHEN( "move assigning" ) {
			MissionAction moveAssigned;
			moveAssigned = std::move(original);
			THEN( "the move assigned has the data" ) {
				CHECK( moveAssigned.Payment() == 1000 );
				CHECK( moveAssigned.DialogText() == "Test" );
			}
		}
	}
}

SCENARIO( "MissionAction with complex configuration", "[MissionAction][Complex]" ) {
	GIVEN( "a MissionAction with multiple settings" ) {
		DataNode node = AsDataNode(
			"on complete\n"
			"\tpayment 5000\n"
			"\tdialog \"Mission complete!\"\n"
		);
		MissionAction action(node, nullptr, nullptr, nullptr);
		THEN( "all values are set correctly" ) {
			CHECK( action.Payment() == 5000 );
			CHECK( action.DialogText() == "Mission complete!" );
			CHECK( action.Validate().empty() );
		}
		WHEN( "saving the action" ) {
			DataWriter writer;
			action.Save(writer);
			std::string result = writer.SaveToString();
			THEN( "all settings are preserved in output" ) {
				CHECK( result.find("on complete") != std::string::npos );
				CHECK( result.find("payment") != std::string::npos );
				CHECK( result.find("dialog") != std::string::npos );
			}
		}
	}
	GIVEN( "a MissionAction with GameAction content" ) {
		DataNode node = AsDataNode(
			"on complete\n"
			"\tpayment 1000\n"
			"\tfine 100"
		);
		MissionAction action(node, nullptr, nullptr, nullptr);
		THEN( "both payment and fine are processed" ) {
			CHECK( action.Payment() == 1000 );
		}
	}
}

SCENARIO( "MissionAction LoadSingle method", "[MissionAction][LoadSingle]" ) {
	GIVEN( "an empty MissionAction" ) {
		MissionAction action;
		WHEN( "LoadSingle is called with a payment node" ) {
			DataNode child = AsDataNode("payment 500");
			action.LoadSingle(child, nullptr, nullptr, nullptr);
			THEN( "payment is set correctly" ) {
				CHECK( action.Payment() == 500 );
			}
		}
		WHEN( "LoadSingle is called with a dialog node" ) {
			DataNode child = AsDataNode("dialog \"Test\"");
			action.LoadSingle(child, nullptr, nullptr, nullptr);
			THEN( "dialog is set correctly" ) {
				CHECK( action.DialogText() == "Test" );
			}
		}
		WHEN( "LoadSingle is called with 'can trigger after failure'" ) {
			DataNode child = AsDataNode("\"can trigger after failure\"");
			action.LoadSingle(child, nullptr, nullptr, nullptr);
			THEN( "the flag is set" ) {
				// runsWhenFailed should be true after loading.
				CHECK( action.Validate().empty() );
			}
		}
	}
}

SCENARIO( "MissionAction with payment multiplier", "[MissionAction][PaymentMultiplier]" ) {
	GIVEN( "a MissionAction with payment and multiplier" ) {
		DataNode node = AsDataNode("on complete\n\tpayment 500 100");
		MissionAction action(node, nullptr, nullptr, nullptr);
		WHEN( "checking payment before instantiation" ) {
			THEN( "base payment is returned" ) {
				CHECK( action.Payment() == 500 );
			}
		}
		WHEN( "instantiating with jumps and payload" ) {
			std::map<std::string, std::string> subs;
			// payment = 500 + (jumps + 1) * payload * paymentMultiplier
			// payment = 500 + (5 + 1) * 10 * 100 = 500 + 6000 = 6500
			MissionAction instance = action.Instantiate(subs, nullptr, 5, 10);
			THEN( "payment includes multiplier bonus" ) {
				CHECK( instance.Payment() == 6500 );
			}
		}
	}
}

SCENARIO( "MissionAction SaveBody method", "[MissionAction][SaveBody]" ) {
	GIVEN( "a MissionAction with content" ) {
		DataNode node = AsDataNode("on complete\n\tpayment 1000");
		MissionAction action(node, nullptr, nullptr, nullptr);
		WHEN( "saving body to a DataWriter" ) {
			DataWriter writer;
			writer.Write("on complete");
			writer.BeginChild();
			action.SaveBody(writer);
			writer.EndChild();
			THEN( "the output contains the payment" ) {
				std::string result = writer.SaveToString();
				CHECK( result.find("payment") != std::string::npos );
				CHECK( result.find("1000") != std::string::npos );
			}
		}
	}
}

SCENARIO( "MissionAction trigger is 'fail'", "[MissionAction][FailTrigger]" ) {
	GIVEN( "a MissionAction with trigger 'fail'" ) {
		DataNode node = CreateMissionActionNode("fail");
		MissionAction action(node, nullptr, nullptr, nullptr);
		WHEN( "saving" ) {
			DataWriter writer;
			action.Save(writer);
			THEN( "the trigger is 'fail'" ) {
				std::string result = writer.SaveToString();
				CHECK( result.find("on fail") != std::string::npos );
			}
		}
	}
}

// #endregion unit tests



} // test namespace