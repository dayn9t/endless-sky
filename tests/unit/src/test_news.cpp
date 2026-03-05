/* test_news.cpp
Copyright (c) 2025 by test-author

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
#include "../../../source/News.h"
#include "../../../source/ConditionsStore.h"

// Include a helper for creating well-formed DataNodes.
#include "datanode-factory.h"

// Include a helper for handling logger output.
#include "logger-output.h"
// Include a helper for capturing & asserting on logged output.
#include "output-capture.hpp"

// ... and any system includes needed for the test file.
#include <set>
#include <string>
#include <type_traits>

namespace { // test namespace

// #region mock data

// Insert file-local data here, e.g. classes, structs, or fixtures that will be useful
// to help test this class/method.

// #endregion mock data



// #region unit tests

// Test code goes here. Preferably, use scenario-driven language making use of the SCENARIO, GIVEN,
// WHEN, and THEN macros. (There will be cases where the more traditional TEST_CASE and SECTION macros
// are better suited to declaration of the public API.)

// When writing assertions, prefer the CHECK and CHECK_FALSE macros when probing the scenario, and prefer
// the REQUIRE / REQUIRE_FALSE macros for fundamental / "validity" assertions. If a CHECK fails, the rest
// of the block's statements will still be evaluated, but a REQUIRE failure will exit the current block.

TEST_CASE( "News Class Traits", "[News]" ) {
	using T = News;
	SECTION( "Class Layout" ) {
		CHECK( std::is_default_constructible_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
		CHECK( std::is_copy_constructible_v<T> );
		CHECK( std::is_copy_assignable_v<T> );
		CHECK( std::is_move_constructible_v<T> );
		CHECK( std::is_move_assignable_v<T> );
	}
}

SCENARIO( "Creating a News with default constructor", "[News][Creation]" ) {
	GIVEN( "a default-constructed News" ) {
		News news;

		THEN( "it is considered empty" ) {
			CHECK( news.IsEmpty() );
		}
		THEN( "it does not match any planet" ) {
			CHECK_FALSE( news.Matches(nullptr) );
		}
		THEN( "it returns an empty speaker name" ) {
			CHECK( news.SpeakerName().empty() );
		}
		THEN( "it returns a null portrait" ) {
			CHECK( news.Portrait() == nullptr );
		}
		THEN( "it returns an empty message" ) {
			CHECK( news.Message().empty() );
		}
	}
}

SCENARIO( "Checking if News is empty", "[News][IsEmpty]" ) {
	GIVEN( "a default-constructed News" ) {
		News news;

		WHEN( "checking if empty" ) {
			THEN( "it is empty because it has no messages or speaker names" ) {
				CHECK( news.IsEmpty() );
			}
		}
	}
}

SCENARIO( "Matching News with planets", "[News][Matches]" ) {
	GIVEN( "a default-constructed News" ) {
		News news;

		WHEN( "matching against a null planet" ) {
			THEN( "it does not match because location is empty" ) {
				CHECK_FALSE( news.Matches(nullptr) );
			}
		}
	}

	GIVEN( "a default-constructed News with no location filter" ) {
		News news;

		WHEN( "matching against any planet" ) {
			THEN( "it does not match because empty location filter means never show" ) {
				// According to the implementation, if location.IsEmpty() is true,
				// Matches() returns false regardless of the planet.
				CHECK_FALSE( news.Matches(nullptr) );
			}
		}
	}
}

SCENARIO( "Getting speaker name from News", "[News][SpeakerName]" ) {
	GIVEN( "a default-constructed News" ) {
		News news;

		WHEN( "getting the speaker name" ) {
			THEN( "an empty string is returned" ) {
				CHECK( news.SpeakerName().empty() );
			}
		}
	}
}

SCENARIO( "Getting portrait from News", "[News][Portrait]" ) {
	GIVEN( "a default-constructed News with no portraits" ) {
		News news;

		WHEN( "getting a portrait" ) {
			THEN( "nullptr is returned" ) {
				CHECK( news.Portrait() == nullptr );
			}
		}
	}
}

SCENARIO( "Getting message from News", "[News][Message]" ) {
	GIVEN( "a default-constructed News" ) {
		News news;

		WHEN( "getting the message" ) {
			THEN( "an empty string is returned" ) {
				CHECK( news.Message().empty() );
			}
		}
	}
}

SCENARIO( "Loading News with empty data node", "[News][Load]" ) {
	OutputSink warnings(std::cerr);

	GIVEN( "a News item and an empty data node" ) {
		News news;
		auto node = AsDataNode("news");

		WHEN( "loading with no children" ) {
			news.Load(node, nullptr, nullptr, nullptr);

			THEN( "the news remains empty" ) {
				CHECK( news.IsEmpty() );
			}
			AND_THEN( "speaker name is empty" ) {
				CHECK( news.SpeakerName().empty() );
			}
			AND_THEN( "message is empty" ) {
				CHECK( news.Message().empty() );
			}
			AND_THEN( "portrait is null" ) {
				CHECK( news.Portrait() == nullptr );
			}
		}
	}
}

SCENARIO( "Loading News with name only", "[News][Load]" ) {
	GIVEN( "a News item loaded with only a name" ) {
		News news;
		auto node = AsDataNode("news\n\tname\n\t\tCaptain");
		news.Load(node, nullptr, nullptr, nullptr);

		WHEN( "checking if empty" ) {
			THEN( "it is empty because message is missing" ) {
				CHECK( news.IsEmpty() );
			}
		}
		WHEN( "getting speaker name" ) {
			THEN( "the speaker name is available" ) {
				CHECK( news.SpeakerName() == "Captain" );
			}
		}
	}
}

SCENARIO( "Loading News with message only", "[News][Load]" ) {
	GIVEN( "a News item loaded with only a message" ) {
		News news;
		auto node = AsDataNode("news\n\tmessage\n\t\tHello there!");
		news.Load(node, nullptr, nullptr, nullptr);

		WHEN( "checking if empty" ) {
			THEN( "it is empty because speaker name is missing" ) {
				CHECK( news.IsEmpty() );
			}
		}
		WHEN( "getting message" ) {
			THEN( "the message is available" ) {
				CHECK( news.Message() == "Hello there!" );
			}
		}
	}
}

SCENARIO( "Loading News with both name and message", "[News][Load]" ) {
	GIVEN( "a News item loaded with name and message" ) {
		News news;
		auto node = AsDataNode("news\n\tname\n\t\tCaptain\n\tmessage\n\t\tHello there!");
		news.Load(node, nullptr, nullptr, nullptr);

		WHEN( "checking if empty" ) {
			THEN( "it is not empty" ) {
				CHECK_FALSE( news.IsEmpty() );
			}
		}
		WHEN( "getting speaker name" ) {
			THEN( "the speaker name is correct" ) {
				CHECK( news.SpeakerName() == "Captain" );
			}
		}
		WHEN( "getting message" ) {
			THEN( "the message is correct" ) {
				CHECK( news.Message() == "Hello there!" );
			}
		}
	}
}

SCENARIO( "Loading News with multiple names using word", "[News][Load]" ) {
	GIVEN( "a News item loaded with multiple name choices" ) {
		News news;
		auto node = AsDataNode("news\n\tname\n\t\tword\n\t\t\tCaptain\n\t\t\tCommander\n\tmessage\n\t\tHello!");
		news.Load(node, nullptr, nullptr, nullptr);

		WHEN( "checking if empty" ) {
			THEN( "it is not empty" ) {
				CHECK_FALSE( news.IsEmpty() );
			}
		}
		WHEN( "getting speaker name multiple times" ) {
			THEN( "one of the names is returned each time" ) {
				// Since Phrase::Get() returns one of the choices, we can't predict which one.
				// But we can check that a valid name is returned.
				std::string name = news.SpeakerName();
				CHECK( (name == "Captain" || name == "Commander") );
			}
		}
	}
}

SCENARIO( "Loading News with multiple messages using word", "[News][Load]" ) {
	GIVEN( "a News item loaded with multiple message choices" ) {
		News news;
		auto node = AsDataNode("news\n\tname\n\t\tCaptain\n\tmessage\n\t\tword\n\t\t\tHello!\n\t\t\tGreetings!");
		news.Load(node, nullptr, nullptr, nullptr);

		WHEN( "checking if empty" ) {
			THEN( "it is not empty" ) {
				CHECK_FALSE( news.IsEmpty() );
			}
		}
		WHEN( "getting message multiple times" ) {
			THEN( "one of the messages is returned each time" ) {
				std::string msg = news.Message();
				CHECK( (msg == "Hello!" || msg == "Greetings!") );
			}
		}
	}
}

SCENARIO( "Loading News with location filter", "[News][Load][Matches]" ) {
	GIVEN( "a News item loaded with a location filter" ) {
		News news;
		// Loading a location filter requires valid location data, but even with
		// an empty location spec, the location filter becomes non-empty.
		auto node = AsDataNode("news\n\tlocation\n\t\tplanet \"Test Planet\"\n\tname\n\t\tCaptain\n\tmessage\n\t\tHello!");
		news.Load(node, nullptr, nullptr, nullptr);

		WHEN( "matching against a null planet" ) {
			THEN( "it does not match because the planet is not the filtered one" ) {
				CHECK_FALSE( news.Matches(nullptr) );
			}
		}
	}
}

SCENARIO( "Loading News with remove name", "[News][Load]" ) {
	GIVEN( "a News item loaded with name then removed" ) {
		News news;
		auto node = AsDataNode("news\n\tname\n\t\tCaptain\n\tremove name\n\tmessage\n\t\tHello!");
		news.Load(node, nullptr, nullptr, nullptr);

		WHEN( "checking if empty" ) {
			THEN( "it is empty because name was removed" ) {
				CHECK( news.IsEmpty() );
			}
		}
		WHEN( "getting speaker name" ) {
			THEN( "the speaker name is empty" ) {
				CHECK( news.SpeakerName().empty() );
			}
		}
	}
}

SCENARIO( "Loading News with remove message", "[News][Load]" ) {
	GIVEN( "a News item loaded with message then removed" ) {
		News news;
		auto node = AsDataNode("news\n\tname\n\t\tCaptain\n\tmessage\n\t\tHello!\n\tremove message");
		news.Load(node, nullptr, nullptr, nullptr);

		WHEN( "checking if empty" ) {
			THEN( "it is empty because message was removed" ) {
				CHECK( news.IsEmpty() );
			}
		}
		WHEN( "getting message" ) {
			THEN( "the message is empty" ) {
				CHECK( news.Message().empty() );
			}
		}
	}
}

SCENARIO( "Loading News with remove location", "[News][Load]" ) {
	GIVEN( "a News item loaded with location then removed" ) {
		News news;
		auto node = AsDataNode("news\n\tlocation\n\t\tplanet \"Test\"\n\tremove location\n\tname\n\t\tCaptain\n\tmessage\n\t\tHello!");
		news.Load(node, nullptr, nullptr, nullptr);

		WHEN( "matching against any planet" ) {
			THEN( "it does not match because location was removed (empty location = never show)" ) {
				CHECK_FALSE( news.Matches(nullptr) );
			}
		}
	}
}

SCENARIO( "Loading News with unrecognized tags", "[News][Load]" ) {
	OutputSink warnings(std::cerr);

	GIVEN( "a News item with unrecognized tags" ) {
		News news;
		auto node = AsDataNode("news\n\tunknownTag\n\t\tvalue\n\tname\n\t\tCaptain\n\tmessage\n\t\tHello!");
		news.Load(node, nullptr, nullptr, nullptr);

		WHEN( "loading completes" ) {
			THEN( "the news still has valid name and message" ) {
				CHECK_FALSE( news.IsEmpty() );
				CHECK( news.SpeakerName() == "Captain" );
				CHECK( news.Message() == "Hello!" );
			}
			AND_THEN( "a warning is logged for the unrecognized tag" ) {
				std::string warning = IgnoreLogHeaders(warnings.Flush());
				CHECK( warning.find("Skipping unrecognized attribute:") != std::string::npos );
			}
		}
	}
}

SCENARIO( "Loading News with add keyword errors", "[News][Load]" ) {
	OutputSink warnings(std::cerr);

	GIVEN( "a News item with add location when location already exists" ) {
		News news;
		auto node = AsDataNode("news\n\tlocation\n\t\tplanet \"A\"\n\tadd location\n\t\tplanet \"B\"\n\tname\n\t\tCaptain\n\tmessage\n\t\tHello!");
		news.Load(node, nullptr, nullptr, nullptr);

		WHEN( "loading completes" ) {
			THEN( "the first location is preserved" ) {
				CHECK_FALSE( news.IsEmpty() );
			}
			AND_THEN( "a warning is logged" ) {
				std::string warning = IgnoreLogHeaders(warnings.Flush());
				CHECK( warning.find("Cannot \"add\" to an existing location filter:") != std::string::npos );
			}
		}
	}
}

SCENARIO( "Loading News with to show condition", "[News][Load]" ) {
	ConditionsStore store;

	GIVEN( "a News item with a 'to show' condition that is never" ) {
		News news;
		auto node = AsDataNode("news\n\tlocation\n\t\tplanet \"Test\"\n\tto show\n\t\tnever\n\tname\n\t\tCaptain\n\tmessage\n\t\tHello!");
		news.Load(node, &store, nullptr, nullptr);

		WHEN( "matching with a location and condition that never passes" ) {
			THEN( "it does not match because condition is never" ) {
				// The location filter is non-empty, but condition 'never' fails
				CHECK_FALSE( news.Matches(nullptr) );
			}
		}
	}
}

// #endregion unit tests



} // test namespace