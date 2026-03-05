/* test_message.cpp
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
#include "../../../source/Message.h"

// Include a helper for creating well-formed DataNodes.
#include "datanode-factory.h"

// ... and any system includes needed for the test file.
#include <string>

namespace { // test namespace

// #region mock data

// Insert file-local data here, e.g. classes, structs, or fixtures that will be useful
// to help test this class/method.

// #endregion mock data



// #region unit tests
// Test Category class functionality
SCENARIO( "Message::Category class tests", "[Message::Category]" ) {
	GIVEN( "A Message::Category instance" ) {
		Message::Category category;

		WHEN( "it is default constructed" ) {
			THEN( "it is not loaded" ) {
				CHECK_FALSE( category.IsLoaded() );
			}
			THEN( "TrueName returns empty string" ) {
				CHECK( category.TrueName().empty() );
			}
			THEN( "default MainDuplicatesStrategy is KEEP_NEW" ) {
				CHECK( category.MainDuplicatesStrategy() == Message::Category::DuplicatesStrategy::KEEP_NEW );
			}
			THEN( "default AllowsLogDuplicates is false" ) {
				CHECK_FALSE( category.AllowsLogDuplicates() );
			}
			THEN( "default IsImportant is false" ) {
				CHECK_FALSE( category.IsImportant() );
			}
			THEN( "default LogOnly is false" ) {
				CHECK_FALSE( category.LogOnly() );
			}
		}

		WHEN( "Load is called with a name" ) {
			auto node = AsDataNode("message category \"test category\"");
			category.Load(node);

			THEN( "it becomes loaded" ) {
				CHECK( category.IsLoaded() );
			}
			THEN( "TrueName returns the provided name" ) {
				CHECK( category.TrueName() == "test category" );
			}
		}

		WHEN( "Load is called without enough tokens" ) {
			auto node = AsDataNode("message");
			category.Load(node);

			THEN( "it remains not loaded" ) {
				CHECK_FALSE( category.IsLoaded() );
			}
		}
	}
}

SCENARIO( "Message::Category color loading", "[Message::Category][Color]" ) {
	GIVEN( "A Message::Category instance" ) {
		Message::Category category;

		WHEN( "main color is specified with RGB values" ) {
			auto node = AsDataNode("message category test\n\tmain color 1.0 0.5 0.25");
			category.Load(node);

			THEN( "MainColor returns the specified color" ) {
				const Color &color = category.MainColor();
				CHECK( color.Get()[0] == Catch::Approx(1.0f) );
				CHECK( color.Get()[1] == Catch::Approx(0.5f) );
				CHECK( color.Get()[2] == Catch::Approx(0.25f) );
			}
		}

		WHEN( "log color is specified with RGB values" ) {
			auto node = AsDataNode("message category test\n\tlog color 0.1 0.2 0.3");
			category.Load(node);

			THEN( "LogColor returns the specified color" ) {
				const Color &color = category.LogColor();
				CHECK( color.Get()[0] == Catch::Approx(0.1f) );
				CHECK( color.Get()[1] == Catch::Approx(0.2f) );
				CHECK( color.Get()[2] == Catch::Approx(0.3f) );
			}
		}
	}
}

SCENARIO( "Message::Category duplicates strategy", "[Message::Category][Duplicates]" ) {
	GIVEN( "A Message::Category instance" ) {
		Message::Category category;

		WHEN( "main duplicates is set to 'keep new'" ) {
			auto node = AsDataNode("message category test\n\tmain duplicates keep new");
			category.Load(node);

			THEN( "MainDuplicatesStrategy returns KEEP_NEW" ) {
				CHECK( category.MainDuplicatesStrategy() == Message::Category::DuplicatesStrategy::KEEP_NEW );
			}
		}

		WHEN( "main duplicates is set to 'keep old'" ) {
			auto node = AsDataNode("message category test\n\tmain duplicates keep old");
			category.Load(node);

			THEN( "MainDuplicatesStrategy returns KEEP_OLD" ) {
				CHECK( category.MainDuplicatesStrategy() == Message::Category::DuplicatesStrategy::KEEP_OLD );
			}
		}

		WHEN( "main duplicates is set to 'keep both'" ) {
			auto node = AsDataNode("message category test\n\tmain duplicates keep both");
			category.Load(node);

			THEN( "MainDuplicatesStrategy returns KEEP_BOTH" ) {
				CHECK( category.MainDuplicatesStrategy() == Message::Category::DuplicatesStrategy::KEEP_BOTH );
			}
		}

		WHEN( "log duplicates is set to 'keep old'" ) {
			auto node = AsDataNode("message category test\n\tlog duplicates keep old");
			category.Load(node);

			THEN( "AllowsLogDuplicates returns false" ) {
				CHECK_FALSE( category.AllowsLogDuplicates() );
			}
		}

		WHEN( "log duplicates is set to 'keep both'" ) {
			auto node = AsDataNode("message category test\n\tlog duplicates keep both");
			category.Load(node);

			THEN( "AllowsLogDuplicates returns true" ) {
				CHECK( category.AllowsLogDuplicates() );
			}
		}
	}
}

SCENARIO( "Message::Category important and log only flags", "[Message::Category][Flags]" ) {
	GIVEN( "A Message::Category instance" ) {
		Message::Category category;

		WHEN( "important is set without value" ) {
			auto node = AsDataNode("message category test\n\timportant");
			category.Load(node);

			THEN( "IsImportant returns true" ) {
				CHECK( category.IsImportant() );
			}
		}

		WHEN( "important is set to true explicitly" ) {
			auto node = AsDataNode("message category test\n\timportant true");
			category.Load(node);

			THEN( "IsImportant returns true" ) {
				CHECK( category.IsImportant() );
			}
		}

		WHEN( "important is set to false explicitly" ) {
			auto node = AsDataNode("message category test\n\timportant false");
			category.Load(node);

			THEN( "IsImportant returns false" ) {
				CHECK_FALSE( category.IsImportant() );
			}
		}

		WHEN( "log only is set without value" ) {
			auto node = AsDataNode("message category test\n\tlog only");
			category.Load(node);

			THEN( "LogOnly returns true" ) {
				CHECK( category.LogOnly() );
			}
		}

		WHEN( "log only is set to true explicitly" ) {
			auto node = AsDataNode("message category test\n\tlog only true");
			category.Load(node);

			THEN( "LogOnly returns true" ) {
				CHECK( category.LogOnly() );
			}
		}

		WHEN( "log only is set to false explicitly" ) {
			auto node = AsDataNode("message category test\n\tlog only false");
			category.Load(node);

			THEN( "LogOnly returns false" ) {
				CHECK_FALSE( category.LogOnly() );
			}
		}
	}
}

// Test Message class functionality
SCENARIO( "Message construction", "[Message][Construction]" ) {
	GIVEN( "A Message instance" ) {
		WHEN( "constructed with text and category" ) {
			Message::Category category;
			category.Load(AsDataNode("message category test"));
			Message message("Hello, world!", &category);

			THEN( "it is loaded" ) {
				CHECK( message.IsLoaded() );
			}
			THEN( "it returns the correct text" ) {
				CHECK_FALSE( message.IsPhrase() );
			}
			THEN( "GetCategory returns the correct category" ) {
				CHECK( message.GetCategory() == &category );
			}
			THEN( "TrueName is empty" ) {
				CHECK( message.TrueName().empty() );
			}
		}

		WHEN( "constructed with text and null category" ) {
			Message message("Test message", nullptr);

			THEN( "it is loaded" ) {
				CHECK( message.IsLoaded() );
			}
			THEN( "GetCategory returns null" ) {
				CHECK( message.GetCategory() == nullptr );
			}
		}
	}
}

SCENARIO( "Message loading from DataNode", "[Message][Loading]" ) {
	GIVEN( "A Message instance" ) {
		Message message;

		WHEN( "Load is called with a name only" ) {
			auto node = AsDataNode("message \"test message\"");
			message.Load(node);

			THEN( "it becomes loaded" ) {
				CHECK( message.IsLoaded() );
			}
			THEN( "TrueName returns the provided name" ) {
				CHECK( message.TrueName() == "test message" );
			}
		}

		WHEN( "Load is called without enough tokens" ) {
			auto node = AsDataNode("message");
			message.Load(node);

			THEN( "it becomes loaded (isLoaded set regardless of name)" ) {
				CHECK( message.IsLoaded() );
			}
			THEN( "TrueName is empty" ) {
				CHECK( message.TrueName().empty() );
			}
		}

		WHEN( "Load is called with text" ) {
			auto node = AsDataNode("message test\n\ttext \"Hello world\"");
			message.Load(node);

			THEN( "IsPhrase returns false" ) {
				CHECK_FALSE( message.IsPhrase() );
			}
		}

		WHEN( "Load is called with phrase" ) {
			auto node = AsDataNode("message test\n\tphrase \"greeting\"");
			message.Load(node);

			THEN( "IsPhrase returns true" ) {
				CHECK( message.IsPhrase() );
			}
		}
	}
}

SCENARIO( "Message TrueName management", "[Message][TrueName]" ) {
	GIVEN( "A Message instance" ) {
		Message message;

		WHEN( "SetTrueName is called" ) {
			message.SetTrueName("custom name");

			THEN( "TrueName returns the set name" ) {
				CHECK( message.TrueName() == "custom name" );
			}
		}

		WHEN( "TrueName is set via Load" ) {
			auto node = AsDataNode("message \"loaded name\"");
			message.Load(node);

			THEN( "TrueName returns the loaded name" ) {
				CHECK( message.TrueName() == "loaded name" );
			}

			AND_WHEN( "SetTrueName is called to override" ) {
				message.SetTrueName("overridden name");

				THEN( "TrueName returns the new name" ) {
					CHECK( message.TrueName() == "overridden name" );
				}
			}
		}
	}
}

SCENARIO( "Message with multiple attributes", "[Message][Attributes]" ) {
	GIVEN( "A Message loaded with multiple attributes" ) {
		Message message;
		auto node = AsDataNode("message \"full test\"\n\ttext \"Sample text\"\n\tcategory special");
		message.Load(node);

		THEN( "all attributes are correctly loaded" ) {
			CHECK( message.IsLoaded() );
			CHECK( message.TrueName() == "full test" );
			CHECK_FALSE( message.IsPhrase() );
		}
	}
}
// #endregion unit tests



} // test namespace