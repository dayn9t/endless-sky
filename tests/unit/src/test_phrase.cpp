/* test_phrase.cpp
Copyright (c) 2025 by Amazinite

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
#include "../../../source/Phrase.h"

// Include a helper for creating well-formed DataNodes.
#include "datanode-factory.h"

// ... and any system includes needed for the test file.
#include "../../../source/Random.h"
#include <set>
#include <string>
#include <type_traits>

namespace { // test namespace

// #region mock data

// #endregion mock data



// #region unit tests
TEST_CASE( "Phrase Basics", "[Phrase]" ) {
	using T = Phrase;
	SECTION( "Class Traits" ) {
		CHECK( std::is_default_constructible_v<T> );
		CHECK_FALSE( std::is_trivially_default_constructible_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
		CHECK_FALSE( std::is_trivially_destructible_v<T> );
	}
	SECTION( "Construction Traits" ) {
		CHECK( std::is_copy_constructible_v<T> );
		CHECK( std::is_move_constructible_v<T> );
		CHECK( std::is_copy_assignable_v<T> );
		CHECK( std::is_move_assignable_v<T> );
	}
}

SCENARIO( "Creating a Phrase", "[Phrase][Creation]" ) {
	GIVEN( "a default constructed Phrase" ) {
		Phrase phrase;

		WHEN( "checking if it is empty" ) {
			THEN( "it returns true" ) {
				CHECK( phrase.IsEmpty() );
			}
		}

		WHEN( "getting the name" ) {
			THEN( "it returns an empty string" ) {
				CHECK( phrase.Name().empty() );
			}
		}

		WHEN( "calling Get()" ) {
			THEN( "it returns an empty string" ) {
				CHECK( phrase.Get().empty() );
			}
		}
	}
}

SCENARIO( "Loading a Phrase from a DataNode", "[Phrase][Loading]" ) {
	Random::Seed(42);

	GIVEN( "a simple phrase with a single word" ) {
		auto node = AsDataNode(R"(test_phrase
	word
		"hello"
)");

		WHEN( "the Phrase is constructed from the node" ) {
			Phrase phrase(node);

			THEN( "the phrase is not empty" ) {
				CHECK_FALSE( phrase.IsEmpty() );
			}

			AND_THEN( "Get() returns the word" ) {
				CHECK( phrase.Get() == "hello" );
			}
		}
	}

	GIVEN( "a phrase with multiple words" ) {
		auto node = AsDataNode(R"(multi_word
	word
		"hello"
		"world"
)");

		WHEN( "the Phrase is constructed from the node" ) {
			Phrase phrase(node);

			THEN( "the phrase is not empty" ) {
				CHECK_FALSE( phrase.IsEmpty() );
			}

			AND_THEN( "Get() returns one of the words" ) {
				std::string result = phrase.Get();
				CHECK( (result == "hello" || result == "world") );
			}
		}
	}

	GIVEN( "a phrase with weighted words" ) {
		auto node = AsDataNode(R"(weighted_phrase
	word
		"common" 5
		"rare" 1
)");

		WHEN( "the Phrase is constructed from the node" ) {
			Phrase phrase(node);

			THEN( "the phrase is not empty" ) {
				CHECK_FALSE( phrase.IsEmpty() );
			}

			AND_THEN( "Get() respects weights (common appears more often)" ) {
				int commonCount = 0;
				int rareCount = 0;
				for(int i = 0; i < 100; ++i)
				{
					std::string result = phrase.Get();
					if(result == "common")
						++commonCount;
					else if(result == "rare")
						++rareCount;
				}
				// With weights 5:1, "common" should appear much more often
				CHECK( commonCount > rareCount );
			}
		}
	}

	GIVEN( "a phrase with multiple sentences" ) {
		auto node = AsDataNode(R"(multi_sentence
	word
		"first"
)");

		WHEN( "Load() is called multiple times" ) {
			Phrase phrase(node);

			auto node2 = AsDataNode(R"(multi_sentence
	word
		"second"
)");
			phrase.Load(node2);

			THEN( "the phrase is not empty" ) {
				CHECK_FALSE( phrase.IsEmpty() );
			}

			AND_THEN( "Get() can return from either sentence" ) {
				bool gotFirst = false;
				bool gotSecond = false;
				for(int i = 0; i < 100; ++i)
				{
					std::string result = phrase.Get();
					if(result == "first")
						gotFirst = true;
					else if(result == "second")
						gotSecond = true;
				}
				CHECK( gotFirst );
				CHECK( gotSecond );
			}
		}
	}

	GIVEN( "a phrase with replace rules" ) {
		auto node = AsDataNode(R"(replace_phrase
	word
		"hello hello"
	replace
		"hello" "hey"
)");

		WHEN( "the Phrase is constructed from the node" ) {
			Phrase phrase(node);

			THEN( "the phrase is not empty" ) {
				CHECK_FALSE( phrase.IsEmpty() );
			}

			AND_THEN( "Get() applies replacements" ) {
				CHECK( phrase.Get() == "hey hey" );
			}
		}
	}

	GIVEN( "a phrase with multiple parts (word + replace)" ) {
		auto node = AsDataNode(R"(complex_phrase
	word
		"Hello World"
	replace
		"Hello" "Goodbye"
		"World" "Universe"
)");

		WHEN( "the Phrase is constructed from the node" ) {
			Phrase phrase(node);

			THEN( "Get() applies all replacements" ) {
				CHECK( phrase.Get() == "Goodbye Universe" );
			}
		}
	}

	GIVEN( "a phrase with an empty name" ) {
		auto node = AsDataNode(R"(
	word
		"test"
)");

		WHEN( "the Phrase is constructed from the node" ) {
			Phrase phrase(node);

			THEN( "the name is 'Unnamed Phrase'" ) {
				CHECK( phrase.Name() == "Unnamed Phrase" );
			}
		}
	}

	GIVEN( "a phrase with invalid characters in name" ) {
		auto node = AsDataNode(R"(bad${name
	word
		"test"
)");

		WHEN( "the Phrase is constructed from the node" ) {
			Phrase phrase(node);

			THEN( "the phrase is not empty (Phrase still loads content despite invalid name)" ) {
				// Phrase only validates the name but still processes children.
				CHECK_FALSE( phrase.IsEmpty() );
			}
		}
	}

	GIVEN( "a phrase name with closing brace" ) {
		auto node = AsDataNode(R"(bad}name
	word
		"test"
)");

		WHEN( "the Phrase is constructed from the node" ) {
			Phrase phrase(node);

			THEN( "the phrase is not empty (Phrase still loads content despite invalid name)" ) {
				// Phrase only validates the name but still processes children.
				CHECK_FALSE( phrase.IsEmpty() );
			}
		}
	}

	GIVEN( "a node with no children" ) {
		auto node = AsDataNode("empty_phrase");

		WHEN( "the Phrase is constructed from the node" ) {
			Phrase phrase(node);

			THEN( "the phrase is empty" ) {
				CHECK( phrase.IsEmpty() );
			}
		}
	}

	GIVEN( "a node with unrecognized child key" ) {
		auto node = AsDataNode(R"(bad_phrase
	unknown
		"value"
)");

		WHEN( "the Phrase is constructed from the node" ) {
			Phrase phrase(node);

			THEN( "the phrase is empty" ) {
				CHECK( phrase.IsEmpty() );
			}
		}
	}

	GIVEN( "a word node with no children" ) {
		auto node = AsDataNode(R"(incomplete_phrase
	word
)");

		WHEN( "the Phrase is constructed from the node" ) {
			Phrase phrase(node);

			THEN( "the phrase is empty" ) {
				CHECK( phrase.IsEmpty() );
			}
		}
	}

	GIVEN( "a blank word choice" ) {
		auto node = AsDataNode(R"(blank_choice
	word
		""
)");

		WHEN( "the Phrase is constructed from the node" ) {
			Phrase phrase(node);

			THEN( "the phrase is not empty" ) {
				CHECK_FALSE( phrase.IsEmpty() );
			}

			AND_THEN( "Get() returns an empty string" ) {
				CHECK( phrase.Get().empty() );
			}
		}
	}
}

SCENARIO( "Phrase Get() randomness", "[Phrase][Random]" ) {
	Random::Seed(42);

	GIVEN( "a phrase with many equally weighted choices" ) {
		auto node = AsDataNode(R"(random_test
	word
		"a"
		"b"
		"c"
		"d"
		"e"
)");
		Phrase phrase(node);

		WHEN( "Get() is called multiple times" ) {
			THEN( "different values are returned over time" ) {
				std::set<std::string> results;
				for(int i = 0; i < 100; ++i)
					results.insert(phrase.Get());

				// With 5 choices and 100 iterations, we should see at least 4 different values
				CHECK( results.size() >= 4 );
			}
		}
	}
}

SCENARIO( "Phrase copy and move semantics", "[Phrase][Copy][Move]" ) {
	GIVEN( "a phrase with content" ) {
		auto node = AsDataNode(R"(original
	word
		"test"
)");
		Phrase original(node);

		WHEN( "the phrase is copy constructed" ) {
			Phrase copy(original);

			THEN( "both have the same name" ) {
				CHECK( copy.Name() == original.Name() );
			}

			AND_THEN( "both return the same text" ) {
				CHECK( copy.Get() == original.Get() );
			}
		}

		WHEN( "the phrase is move constructed" ) {
			std::string originalName = original.Name();
			Phrase moved(std::move(original));

			THEN( "the moved phrase has the name" ) {
				CHECK( moved.Name() == originalName );
			}
		}

		WHEN( "the phrase is copy assigned" ) {
			Phrase copy;
			copy = original;

			THEN( "both have the same name" ) {
				CHECK( copy.Name() == original.Name() );
			}
		}

		WHEN( "the phrase is move assigned" ) {
			std::string originalName = original.Name();
			Phrase moved;
			moved = std::move(original);

			THEN( "the moved phrase has the name" ) {
				CHECK( moved.Name() == originalName );
			}
		}
	}
}

SCENARIO( "Phrase Load() method", "[Phrase][Load]" ) {
	GIVEN( "an empty phrase" ) {
		Phrase phrase;

		WHEN( "Load() is called with valid data" ) {
			auto node = AsDataNode(R"(loaded_phrase
	word
		"loaded"
)");
			phrase.Load(node);

			THEN( "the phrase is no longer empty" ) {
				CHECK_FALSE( phrase.IsEmpty() );
			}
		}
	}

	GIVEN( "a phrase that is loaded twice" ) {
		auto node1 = AsDataNode(R"(double_loaded
	word
		"first"
)");
		Phrase phrase(node1);

		auto node2 = AsDataNode(R"(double_loaded
	word
		"second"
)");
		phrase.Load(node2);

		WHEN( "checking the content" ) {
			THEN( "both sentences are available" ) {
				std::set<std::string> results;
				for(int i = 0; i < 100; ++i)
					results.insert(phrase.Get());
				CHECK( results.count("first") > 0 );
				CHECK( results.count("second") > 0 );
			}
		}
	}
}
// #endregion unit tests



} // test namespace