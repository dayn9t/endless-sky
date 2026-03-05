/* test_npcAction.cpp
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
#include "../../../source/NPCAction.h"

// ... and any system includes needed for the test file.
#include "../../../source/DataNode.h"
#include "../../../source/DataWriter.h"

#include "datanode-factory.h"

#include <map>
#include <string>

namespace { // test namespace

// #region mock data

// Helper function to create a DataNode for NPCAction testing.
DataNode CreateNPCActionNode(const std::string &trigger, bool withTriggered = false)
{
	std::string text = "on " + trigger;
	if(withTriggered)
		text += "\n\ttriggered";
	return AsDataNode(text);
}

// #endregion mock data



// #region unit tests

SCENARIO( "Creating an NPCAction", "[NPCAction][Construction]" ) {
	GIVEN( "no arguments" ) {
		WHEN( "default constructing an NPCAction" ) {
			NPCAction action;
			THEN( "it creates a valid but empty NPCAction" ) {
				// Default constructed NPCAction should be usable
				// Validate on an empty action should return empty string
				CHECK( action.Validate().empty() );
			}
		}
	}
	GIVEN( "a DataNode with a trigger" ) {
		DataNode node = CreateNPCActionNode("complete");
		WHEN( "constructing with the DataNode" ) {
			NPCAction action(node, nullptr, nullptr, nullptr);
			THEN( "the NPCAction is created successfully" ) {
				CHECK( action.Validate().empty() );
			}
		}
	}
	GIVEN( "a DataNode with a trigger and triggered flag" ) {
		DataNode node = CreateNPCActionNode("complete", true);
		WHEN( "constructing with the DataNode" ) {
			NPCAction action(node, nullptr, nullptr, nullptr);
			THEN( "the NPCAction is created successfully" ) {
				CHECK( action.Validate().empty() );
			}
		}
	}
}

SCENARIO( "Loading an NPCAction", "[NPCAction][Load]" ) {
	GIVEN( "an empty NPCAction" ) {
		NPCAction action;
		WHEN( "loading from a DataNode with a trigger" ) {
			DataNode node = CreateNPCActionNode("offer");
			action.Load(node, nullptr, nullptr, nullptr);
			THEN( "the action is loaded successfully" ) {
				CHECK( action.Validate().empty() );
			}
		}
		WHEN( "loading from a DataNode with triggered flag" ) {
			DataNode node = CreateNPCActionNode("accept", true);
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
	GIVEN( "a DataNode with multiple child elements" ) {
		DataNode node = AsDataNode("on complete\n\ttriggered\n\tdialog \"Test message\"");
		WHEN( "loading the NPCAction" ) {
			NPCAction action(node, nullptr, nullptr, nullptr);
			THEN( "all child elements are processed" ) {
				CHECK( action.Validate().empty() );
			}
		}
	}
}

SCENARIO( "Saving an NPCAction", "[NPCAction][Save]" ) {
	GIVEN( "an NPCAction with a trigger" ) {
		DataNode node = CreateNPCActionNode("complete");
		NPCAction action(node, nullptr, nullptr, nullptr);
		WHEN( "saving to a DataWriter" ) {
			DataWriter writer;
			action.Save(writer);
			THEN( "the output contains the trigger" ) {
				std::string result = writer.SaveToString();
				CHECK( result.find("on complete") != std::string::npos );
			}
		}
	}
	GIVEN( "an NPCAction with triggered flag set" ) {
		DataNode node = CreateNPCActionNode("fail", true);
		NPCAction action(node, nullptr, nullptr, nullptr);
		WHEN( "saving to a DataWriter" ) {
			DataWriter writer;
			action.Save(writer);
			THEN( "the output contains triggered marker" ) {
				std::string result = writer.SaveToString();
				CHECK( result.find("on fail") != std::string::npos );
				CHECK( result.find("triggered") != std::string::npos );
			}
		}
	}
	GIVEN( "a default constructed NPCAction" ) {
		NPCAction action;
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

SCENARIO( "Validating an NPCAction", "[NPCAction][Validate]" ) {
	GIVEN( "a default constructed NPCAction" ) {
		NPCAction action;
		WHEN( "validating" ) {
			std::string result = action.Validate();
			THEN( "no validation errors are returned" ) {
				CHECK( result.empty() );
			}
		}
	}
	GIVEN( "an NPCAction loaded from valid data" ) {
		DataNode node = CreateNPCActionNode("complete");
		NPCAction action(node, nullptr, nullptr, nullptr);
		WHEN( "validating" ) {
			std::string result = action.Validate();
			THEN( "no validation errors are returned" ) {
				CHECK( result.empty() );
			}
		}
	}
}

SCENARIO( "Instantiating an NPCAction", "[NPCAction][Instantiate]" ) {
	GIVEN( "an NPCAction with a trigger" ) {
		DataNode node = CreateNPCActionNode("complete");
		NPCAction templateAction(node, nullptr, nullptr, nullptr);
		WHEN( "instantiating with substitution map" ) {
			std::map<std::string, std::string> subs;
			subs["<test>"] = "replaced";
			NPCAction instance = templateAction.Instantiate(subs, nullptr, 0, 0);
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
	GIVEN( "an NPCAction with triggered flag" ) {
		DataNode node = CreateNPCActionNode("offer", true);
		NPCAction templateAction(node, nullptr, nullptr, nullptr);
		WHEN( "instantiating" ) {
			std::map<std::string, std::string> subs;
			NPCAction instance = templateAction.Instantiate(subs, nullptr, 0, 0);
			THEN( "the instantiated action has the trigger" ) {
				DataWriter writer;
				instance.Save(writer);
				std::string result = writer.SaveToString();
				CHECK( result.find("on offer") != std::string::npos );
			}
		}
	}
}

SCENARIO( "NPCAction trigger behavior", "[NPCAction][Trigger]" ) {
	GIVEN( "an NPCAction created without triggered flag" ) {
		DataNode node = CreateNPCActionNode("complete");
		NPCAction action(node, nullptr, nullptr, nullptr);
		WHEN( "saving the action" ) {
			DataWriter writer;
			action.Save(writer);
			THEN( "triggered is not present in output" ) {
				std::string result = writer.SaveToString();
				CHECK( result.find("triggered") == std::string::npos );
			}
		}
	}
	GIVEN( "an NPCAction created with triggered flag" ) {
		DataNode node = CreateNPCActionNode("complete", true);
		NPCAction action(node, nullptr, nullptr, nullptr);
		WHEN( "saving the action" ) {
			DataWriter writer;
			action.Save(writer);
			THEN( "triggered is present in output" ) {
				std::string result = writer.SaveToString();
				CHECK( result.find("triggered") != std::string::npos );
			}
		}
	}
}

SCENARIO( "Copying and moving NPCAction objects", "[NPCAction][Copy][Move]" ) {
	GIVEN( "an NPCAction with data" ) {
		DataNode node = CreateNPCActionNode("complete", true);
		NPCAction original(node, nullptr, nullptr, nullptr);
		WHEN( "copy constructing" ) {
			NPCAction copied(original);
			THEN( "the copy has the same data" ) {
				DataWriter writer;
				copied.Save(writer);
				std::string result = writer.SaveToString();
				CHECK( result.find("on complete") != std::string::npos );
				CHECK( result.find("triggered") != std::string::npos );
			}
		}
		WHEN( "copy assigning" ) {
			NPCAction assigned;
			assigned = original;
			THEN( "the assigned has the same data" ) {
				DataWriter writer;
				assigned.Save(writer);
				std::string result = writer.SaveToString();
				CHECK( result.find("on complete") != std::string::npos );
				CHECK( result.find("triggered") != std::string::npos );
			}
		}
		WHEN( "move constructing" ) {
			NPCAction moved(std::move(original));
			THEN( "the moved object has the data" ) {
				DataWriter writer;
				moved.Save(writer);
				std::string result = writer.SaveToString();
				CHECK( result.find("on complete") != std::string::npos );
				CHECK( result.find("triggered") != std::string::npos );
			}
		}
		WHEN( "move assigning" ) {
			NPCAction moveAssigned;
			moveAssigned = std::move(original);
			THEN( "the move assigned has the data" ) {
				DataWriter writer;
				moveAssigned.Save(writer);
				std::string result = writer.SaveToString();
				CHECK( result.find("on complete") != std::string::npos );
				CHECK( result.find("triggered") != std::string::npos );
			}
		}
	}
}

SCENARIO( "NPCAction with different trigger types", "[NPCAction][Triggers]" ) {
	GIVEN( "various trigger names" ) {
		std::vector<std::string> triggers = {"offer", "accept", "decline", "complete", "fail", "abort", "visit", "stopover"};
		WHEN( "creating and saving NPCActions" ) {
			THEN( "each trigger is preserved correctly" ) {
				for(const auto &trigger : triggers)
				{
					DataNode node = CreateNPCActionNode(trigger);
					NPCAction action(node, nullptr, nullptr, nullptr);
					DataWriter writer;
					action.Save(writer);
					std::string result = writer.SaveToString();
					CHECK( result.find("on " + trigger) != std::string::npos );
				}
			}
		}
	}
}

// #endregion unit tests



} // test namespace