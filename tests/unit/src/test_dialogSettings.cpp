/* test_dialogSettings.cpp
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
#include "../../../source/DialogSettings.h"

// Include a helper for creating well-formed DataNodes.
#include "datanode-factory.h"
// Include DataWriter for save tests.
#include "../../../source/DataWriter.h"
// Include ConditionsStore for condition-related tests.
#include "../../../source/ConditionsStore.h"

// ... and any system includes needed for the test file.
#include <map>
#include <string>

namespace { // test namespace

// #region mock data

// #endregion mock data



// #region unit tests
TEST_CASE( "DialogSettings Basics", "[DialogSettings]" ) {
	using T = DialogSettings;
	SECTION( "Class Traits" ) {
		CHECK_FALSE( std::is_trivial_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
	}
	SECTION( "Construction Traits" ) {
		CHECK( std::is_default_constructible_v<T> );
		CHECK( std::is_nothrow_default_constructible_v<T> );
		CHECK( std::is_copy_constructible_v<T> );
		CHECK( std::is_move_constructible_v<T> );
	}
	SECTION( "Copy Traits" ) {
		CHECK( std::is_copy_assignable_v<T> );
		CHECK( std::is_move_assignable_v<T> );
	}
}

SCENARIO( "Creating a DialogSettings", "[DialogSettings][Creation]" ) {
	GIVEN( "no arguments" ) {
		const auto settings = DialogSettings{};
		THEN( "it is empty and returns empty text" ) {
			CHECK( settings.IsEmpty() );
			CHECK( settings.Text().empty() );
		}
		THEN( "it validates as true (empty dialog is valid)" ) {
			CHECK( settings.Validate() );
		}
	}
}

SCENARIO( "Loading a DialogSettings from a DataNode", "[DialogSettings][Loading]" ) {
	ConditionsStore store;

	GIVEN( "a node with a single line of text" ) {
		const auto node = AsDataNode("dialog \"Hello, pilot!\"");
		const auto settings = DialogSettings(node, &store);

		THEN( "the text is stored correctly" ) {
			CHECK_FALSE( settings.IsEmpty() );
			CHECK( settings.Text() == "Hello, pilot!" );
		}
		THEN( "it validates as true" ) {
			CHECK( settings.Validate() );
		}
	}

	GIVEN( "a node with multiple lines of text" ) {
		const auto node = AsDataNode("dialog\n\t\"First line\"\n\t\"Second line\"");
		const auto settings = DialogSettings(node, &store);

		THEN( "the lines are concatenated with proper formatting" ) {
			CHECK_FALSE( settings.IsEmpty() );
			CHECK( settings.Text() == "First line\n\tSecond line" );
		}
	}

	GIVEN( "a node with a single child text" ) {
		const auto node = AsDataNode("dialog\n\t\"This is dialog text\"");
		const auto settings = DialogSettings(node, &store);

		THEN( "the text is stored correctly" ) {
			CHECK_FALSE( settings.IsEmpty() );
			CHECK( settings.Text() == "This is dialog text" );
		}
	}

	GIVEN( "a node with multiple child lines" ) {
		const auto node = AsDataNode("dialog\n\tLine one\n\tLine two\n\tLine three");
		const auto settings = DialogSettings(node, &store);

		THEN( "the lines are concatenated correctly" ) {
			CHECK_FALSE( settings.IsEmpty() );
			CHECK( settings.Text() == "Line one\n\tLine two\n\tLine three" );
		}
	}

	GIVEN( "an empty dialog node" ) {
		const auto node = AsDataNode("dialog");
		const auto settings = DialogSettings(node, &store);

		THEN( "it is empty" ) {
			CHECK( settings.IsEmpty() );
			CHECK( settings.Text().empty() );
		}
	}
}

SCENARIO( "Loading DialogSettings with conditions", "[DialogSettings][Conditions]" ) {
	ConditionsStore store{{"test condition", 1}};

	GIVEN( "a node with 'to display' conditions that are met" ) {
		const auto node = AsDataNode("dialog\n\t\"Visible text\"\n\t\tto display\n\t\t\ttest condition");
		const auto settings = DialogSettings(node, &store);

		THEN( "the dialog is loaded with condition text" ) {
			// Text is empty because condition prevents early collapsing
			CHECK( settings.Text().empty() );
		}
		THEN( "validation succeeds" ) {
			CHECK( settings.Validate() );
		}
	}

	GIVEN( "a node with 'to display' conditions that are not met" ) {
		ConditionsStore emptyStore;
		const auto node = AsDataNode("dialog\n\t\"Hidden text\"\n\t\tto display\n\t\t\tunavailable condition");
		const auto settings = DialogSettings(node, &emptyStore);

		THEN( "the dialog is still loaded (conditions evaluated at instantiate time)" ) {
			CHECK( settings.Text().empty() );
		}
		THEN( "validation succeeds" ) {
			CHECK( settings.Validate() );
		}
	}
}

SCENARIO( "Saving a DialogSettings", "[DialogSettings][Saving]" ) {
	ConditionsStore store;

	GIVEN( "a simple dialog with text" ) {
		const auto node = AsDataNode("dialog \"Save me\"");
		const auto settings = DialogSettings(node, &store);

		WHEN( "saved to a DataWriter" ) {
			DataWriter writer;
			settings.Save(writer);

			THEN( "the output is correctly formatted" ) {
				const std::string output = writer.SaveToString();
				CHECK( output == "dialog\n\t\"Save me\"\n" );
			}
		}
	}

	GIVEN( "a multi-line dialog" ) {
		const auto node = AsDataNode("dialog\n\tFirst\n\tSecond");
		const auto settings = DialogSettings(node, &store);

		WHEN( "saved to a DataWriter" ) {
			DataWriter writer;
			settings.Save(writer);

			THEN( "the output preserves line structure" ) {
				const std::string output = writer.SaveToString();
				CHECK( output == "dialog\n\tFirst\n\tSecond\n" );
			}
		}
	}

	GIVEN( "an instantiated dialog" ) {
		const auto node = AsDataNode("dialog \"Template text\"");
		const auto original = DialogSettings(node, &store);
		const auto instantiated = original.Instantiate({});

		WHEN( "saved to a DataWriter" ) {
			DataWriter writer;
			instantiated.Save(writer);

			THEN( "the output contains the instantiated text" ) {
				const std::string output = writer.SaveToString();
				CHECK( output == "dialog\n\t\"Template text\"\n" );
			}
		}
	}
}

SCENARIO( "Validating a DialogSettings", "[DialogSettings][Validation]" ) {
	ConditionsStore store;

	GIVEN( "a simple text dialog" ) {
		const auto node = AsDataNode("dialog \"Valid text\"");
		const auto settings = DialogSettings(node, &store);

		THEN( "validation returns true" ) {
			CHECK( settings.Validate() );
		}
	}

	GIVEN( "an empty dialog" ) {
		const auto settings = DialogSettings{};

		THEN( "validation returns true (empty is valid)" ) {
			CHECK( settings.Validate() );
		}
	}

	GIVEN( "a dialog with an anonymous phrase" ) {
		const auto node = AsDataNode("dialog\n\tphrase\n\t\tword\n\t\t\t\"test word\"");
		const auto settings = DialogSettings(node, &store);

		THEN( "validation returns true if phrase is not empty" ) {
			CHECK( settings.Validate() );
		}
	}
}

SCENARIO( "Instantiating a DialogSettings", "[DialogSettings][Instantiation]" ) {
	ConditionsStore store;

	GIVEN( "a simple text dialog" ) {
		const auto node = AsDataNode("dialog \"Simple text\"");
		const auto original = DialogSettings(node, &store);

		WHEN( "instantiated with empty substitutions" ) {
			const auto instance = original.Instantiate({});

			THEN( "the text is unchanged" ) {
				CHECK( instance.Text() == "Simple text" );
			}
		}
	}

	GIVEN( "a dialog with substitution placeholders" ) {
		const auto node = AsDataNode("dialog \"Hello <name>!\"");
		const auto original = DialogSettings(node, &store);

		WHEN( "instantiated with matching substitutions" ) {
			const auto instance = original.Instantiate({{"name", "Captain"}});

			THEN( "the placeholders are replaced" ) {
				CHECK( instance.Text() == "Hello Captain!" );
			}
		}

		WHEN( "instantiated without matching substitutions" ) {
			const auto instance = original.Instantiate({});

			THEN( "the placeholders remain unchanged" ) {
				CHECK( instance.Text() == "Hello <name>!" );
			}
		}
	}

	GIVEN( "a dialog with multiple substitution placeholders" ) {
		const auto node = AsDataNode("dialog \"<greeting> <name>, welcome to <place>!\"");
		const auto original = DialogSettings(node, &store);

		WHEN( "instantiated with all substitutions" ) {
			const auto instance = original.Instantiate({
				{"greeting", "Hello"},
				{"name", "Pilot"},
				{"place", "the station"}
			});

			THEN( "all placeholders are replaced" ) {
				CHECK( instance.Text() == "Hello Pilot, welcome to the station!" );
			}
		}
	}

	GIVEN( "a dialog with an anonymous phrase" ) {
		const auto node = AsDataNode("dialog\n\tphrase\n\t\tword\n\t\t\t\"generated\"");
		const auto original = DialogSettings(node, &store);

		WHEN( "instantiated" ) {
			const auto instance = original.Instantiate({});

			THEN( "the phrase is evaluated" ) {
				CHECK( instance.Text() == "generated" );
			}
		}
	}
}

SCENARIO( "Getting text from DialogSettings", "[DialogSettings][Text]" ) {
	ConditionsStore store;

	GIVEN( "a dialog with text" ) {
		const auto node = AsDataNode("dialog \"Test text\"");
		const auto settings = DialogSettings(node, &store);

		THEN( "Text() returns the content" ) {
			CHECK( settings.Text() == "Test text" );
		}
	}

	GIVEN( "an empty dialog" ) {
		const auto settings = DialogSettings{};

		THEN( "Text() returns an empty string" ) {
			CHECK( settings.Text().empty() );
		}
	}
}

SCENARIO( "Checking if DialogSettings is empty", "[DialogSettings][IsEmpty]" ) {
	ConditionsStore store;

	GIVEN( "a default-constructed DialogSettings" ) {
		const auto settings = DialogSettings{};

		THEN( "IsEmpty() returns true" ) {
			CHECK( settings.IsEmpty() );
		}
	}

	GIVEN( "a dialog with text" ) {
		const auto node = AsDataNode("dialog \"Content\"");
		const auto settings = DialogSettings(node, &store);

		THEN( "IsEmpty() returns false" ) {
			CHECK_FALSE( settings.IsEmpty() );
		}
	}

	GIVEN( "a dialog loaded from an empty node" ) {
		const auto node = AsDataNode("dialog");
		const auto settings = DialogSettings(node, &store);

		THEN( "IsEmpty() returns true" ) {
			CHECK( settings.IsEmpty() );
		}
	}
}

SCENARIO( "DialogSettings Load method", "[DialogSettings][Load]" ) {
	ConditionsStore store;

	GIVEN( "an existing DialogSettings" ) {
		auto settings = DialogSettings(AsDataNode("dialog \"Initial\""), &store);
		REQUIRE( settings.Text() == "Initial" );

		WHEN( "Load is called with new content" ) {
			const auto newNode = AsDataNode("dialog \"Replaced\"");
			settings.Load(newNode, &store);

			THEN( "the content is replaced" ) {
				CHECK( settings.Text() == "Replaced" );
			}
		}
	}
}

SCENARIO( "DialogSettings with phrases", "[DialogSettings][Phrase]" ) {
	ConditionsStore store;

	GIVEN( "a dialog with an anonymous inline phrase" ) {
		const auto node = AsDataNode("dialog\n\tphrase\n\t\tword\n\t\t\t\"Hello\"");
		const auto settings = DialogSettings(node, &store);

		THEN( "it can be validated" ) {
			CHECK( settings.Validate() );
		}

		WHEN( "instantiated" ) {
			const auto instance = settings.Instantiate({});

			THEN( "the phrase is expanded" ) {
				CHECK( instance.Text() == "Hello" );
			}
		}
	}

	GIVEN( "a dialog with multiple phrase choices" ) {
		const auto node = AsDataNode("dialog\n\tphrase\n\t\tword\n\t\t\t\"Hello\"\n\t\t\t\"Hi\"\n\t\t\t\"Hey\"");
		const auto settings = DialogSettings(node, &store);

		WHEN( "instantiated multiple times" ) {
			const auto instance1 = settings.Instantiate({});
			const auto instance2 = settings.Instantiate({});

			THEN( "each instance has valid text" ) {
				CHECK_FALSE( instance1.Text().empty() );
				CHECK_FALSE( instance2.Text().empty() );
			}
		}
	}
}
// #endregion unit tests



} // test namespace