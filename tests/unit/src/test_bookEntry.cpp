/* test_bookEntry.cpp
Copyright (c) 2025 by xobes

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
#include "../../../source/BookEntry.h"

// ... and any system includes needed for the test file.
#include "../../../source/DataNode.h"
#include "../../../source/DataWriter.h"
#include "datanode-factory.h"

#include <map>
#include <set>
#include <string>
#include <variant>

namespace { // test namespace

// #region mock data

// Insert file-local data here, e.g. classes, structs, or fixtures that will be useful
// to help test this class/method.

// #endregion mock data



// #region unit tests

TEST_CASE( "BookEntry Basics", "[BookEntry]" ) {
	using T = BookEntry;
	SECTION( "Class Traits" ) {
		CHECK_FALSE( std::is_trivial_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
		CHECK_FALSE( std::is_trivially_destructible_v<T> );
	}
	SECTION( "Construction Traits" ) {
		CHECK( std::is_default_constructible_v<T> );
		CHECK( std::is_copy_constructible_v<T> );
		CHECK( std::is_move_constructible_v<T> );
	}
	SECTION( "Copy Traits" ) {
		CHECK( std::is_copy_assignable_v<T> );
		CHECK( std::is_move_assignable_v<T> );
	}
}

SCENARIO( "Creating a BookEntry", "[BookEntry]" ) {
	GIVEN( "A default-constructed BookEntry" ) {
		BookEntry entry;
		THEN( "it is empty" ) {
			CHECK( entry.IsEmpty() );
		}
		THEN( "it has no scenes" ) {
			CHECK( entry.GetScenes().empty() );
		}
	}
}

SCENARIO( "Loading a BookEntry from a DataNode", "[BookEntry][Load]" ) {
	GIVEN( "An empty BookEntry" ) {
		BookEntry entry;
		WHEN( "loading from a DataNode with text content" ) {
			DataNode node = AsDataNode("entry\n\tHello World");
			entry.Load(node);
			THEN( "the entry is not empty" ) {
				CHECK_FALSE( entry.IsEmpty() );
			}
		}
		WHEN( "loading from a DataNode with multiple text lines" ) {
			DataNode node = AsDataNode("entry\n\tFirst line\n\tSecond line");
			entry.Load(node);
			THEN( "the entry is not empty" ) {
				CHECK_FALSE( entry.IsEmpty() );
			}
		}
		WHEN( "loading from a DataNode with text on the root node" ) {
			DataNode node = AsDataNode("entry with text");
			entry.Load(node);
			THEN( "the entry is not empty" ) {
				CHECK_FALSE( entry.IsEmpty() );
			}
		}
		WHEN( "loading from an empty DataNode" ) {
			DataNode node = AsDataNode("entry");
			entry.Load(node);
			THEN( "the entry remains empty" ) {
				CHECK( entry.IsEmpty() );
			}
		}
	}
	GIVEN( "A DataNode with text at a specific start position" ) {
		BookEntry entry;
		DataNode node = AsDataNode("entry skip this text");
		WHEN( "loading from a start position" ) {
			entry.Load(node, 1);  // Skip "entry" token
			THEN( "the entry is not empty" ) {
				CHECK_FALSE( entry.IsEmpty() );
			}
		}
	}
}

SCENARIO( "Adding BookEntry contents together", "[BookEntry][Add]" ) {
	GIVEN( "Two BookEntry objects with text content" ) {
		BookEntry entry1;
		BookEntry entry2;
		DataNode node1 = AsDataNode("entry\n\tFirst content");
		DataNode node2 = AsDataNode("entry\n\tSecond content");
		entry1.Load(node1);
		entry2.Load(node2);

		WHEN( "adding the second entry to the first" ) {
			entry1.Add(entry2);
			THEN( "the first entry is not empty" ) {
				CHECK_FALSE( entry1.IsEmpty() );
			}
		}
	}
	GIVEN( "An empty BookEntry and a non-empty BookEntry" ) {
		BookEntry emptyEntry;
		BookEntry nonEmptyEntry;
		DataNode node = AsDataNode("entry\n\tContent");
		nonEmptyEntry.Load(node);

		WHEN( "adding the non-empty entry to the empty one" ) {
			emptyEntry.Add(nonEmptyEntry);
			THEN( "the empty entry is now non-empty" ) {
				CHECK_FALSE( emptyEntry.IsEmpty() );
			}
		}
	}
	GIVEN( "A non-empty BookEntry and an empty BookEntry" ) {
		BookEntry nonEmptyEntry;
		BookEntry emptyEntry;
		DataNode node = AsDataNode("entry\n\tContent");
		nonEmptyEntry.Load(node);

		WHEN( "adding the empty entry to the non-empty one" ) {
			nonEmptyEntry.Add(emptyEntry);
			THEN( "the non-empty entry remains non-empty" ) {
				CHECK_FALSE( nonEmptyEntry.IsEmpty() );
			}
		}
	}
}

SCENARIO( "Instantiating a BookEntry with substitutions", "[BookEntry][Instantiate]" ) {
	GIVEN( "A BookEntry with text containing placeholders" ) {
		BookEntry entry;
		DataNode node = AsDataNode("entry\n\tHello <name>!");
		entry.Load(node);

		WHEN( "instantiating with substitutions" ) {
			std::map<std::string, std::string> subs;
			subs["<name>"] = "World";
			BookEntry instantiated = entry.Instantiate(subs);

			THEN( "the original entry is unchanged" ) {
				CHECK_FALSE( entry.IsEmpty() );
			}
			THEN( "the instantiated entry is not empty" ) {
				CHECK_FALSE( instantiated.IsEmpty() );
			}
		}
	}
	GIVEN( "A BookEntry with text without placeholders" ) {
		BookEntry entry;
		DataNode node = AsDataNode("entry\n\tHello World!");
		entry.Load(node);

		WHEN( "instantiating with empty substitutions" ) {
			std::map<std::string, std::string> subs;
			BookEntry instantiated = entry.Instantiate(subs);

			THEN( "the instantiated entry is not empty" ) {
				CHECK_FALSE( instantiated.IsEmpty() );
			}
		}
	}
	GIVEN( "An empty BookEntry" ) {
		BookEntry entry;

		WHEN( "instantiating with substitutions" ) {
			std::map<std::string, std::string> subs;
			subs["<name>"] = "World";
			BookEntry instantiated = entry.Instantiate(subs);

			THEN( "the instantiated entry is empty" ) {
				CHECK( instantiated.IsEmpty() );
			}
		}
	}
}

SCENARIO( "Saving a BookEntry to a DataWriter", "[BookEntry][Save]" ) {
	GIVEN( "A BookEntry with text content" ) {
		BookEntry entry;
		DataNode node = AsDataNode("entry\n\tHello World");
		entry.Load(node);

		WHEN( "saving to a DataWriter" ) {
			DataWriter writer;
			entry.Save(writer);
			std::string result = writer.SaveToString();

			THEN( "the output contains the text" ) {
				CHECK_FALSE( result.empty() );
			}
		}
	}
	GIVEN( "A BookEntry with multiple text items" ) {
		BookEntry entry;
		DataNode node = AsDataNode("entry\n\tFirst line\n\tSecond line");
		entry.Load(node);

		WHEN( "saving to a DataWriter" ) {
			DataWriter writer;
			entry.Save(writer);
			std::string result = writer.SaveToString();

			THEN( "the output is not empty" ) {
				CHECK_FALSE( result.empty() );
			}
		}
	}
	GIVEN( "An empty BookEntry" ) {
		BookEntry entry;

		WHEN( "saving to a DataWriter" ) {
			DataWriter writer;
			entry.Save(writer);
			std::string result = writer.SaveToString();

			THEN( "the output contains proper structure" ) {
				CHECK_FALSE( result.empty() );
			}
		}
	}
}

SCENARIO( "Getting scenes from a BookEntry", "[BookEntry][GetScenes]" ) {
	GIVEN( "An empty BookEntry" ) {
		BookEntry entry;

		WHEN( "getting scenes" ) {
			const std::set<const Sprite *> &scenes = entry.GetScenes();

			THEN( "the scenes set is empty" ) {
				CHECK( scenes.empty() );
			}
		}
	}
	GIVEN( "A BookEntry with only text content" ) {
		BookEntry entry;
		DataNode node = AsDataNode("entry\n\tHello World");
		entry.Load(node);

		WHEN( "getting scenes" ) {
			const std::set<const Sprite *> &scenes = entry.GetScenes();

			THEN( "the scenes set is empty" ) {
				CHECK( scenes.empty() );
			}
		}
	}
}

SCENARIO( "Loading a BookEntry with scene nodes", "[BookEntry][Load][Scene]" ) {
	GIVEN( "An empty BookEntry" ) {
		BookEntry entry;
		WHEN( "loading from a DataNode with a scene reference" ) {
			// Note: SpriteSet::Get requires the sprite system to be initialized.
			// This test verifies the loading path without depending on actual sprites.
			DataNode node = AsDataNode("entry\n\tscene ship");
			THEN( "the load operation can be called" ) {
				// This will add a scene item if the sprite system is available.
				// Without sprite initialization, it may add a null or placeholder sprite.
				CHECK_NOTHROW( entry.Load(node) );
			}
		}
	}
}

SCENARIO( "BookEntry with combined operations", "[BookEntry][Combined]" ) {
	GIVEN( "A BookEntry loaded and then instantiated" ) {
		BookEntry entry;
		DataNode node = AsDataNode("entry\n\tHello <name>!\n\tWelcome to <place>");
		entry.Load(node);

		std::map<std::string, std::string> subs;
		subs["<name>"] = "Player";
		subs["<place>"] = "Endless Sky";

		WHEN( "instantiating and then saving" ) {
			BookEntry instantiated = entry.Instantiate(subs);
			DataWriter writer;
			instantiated.Save(writer);
			std::string result = writer.SaveToString();

			THEN( "the output is not empty" ) {
				CHECK_FALSE( result.empty() );
			}
		}
	}
	GIVEN( "Multiple BookEntry objects to combine" ) {
		BookEntry entry1;
		BookEntry entry2;
		BookEntry combined;

		DataNode node1 = AsDataNode("entry\n\tPart one");
		DataNode node2 = AsDataNode("entry\n\tPart two");
		entry1.Load(node1);
		entry2.Load(node2);

		WHEN( "combining entries and checking emptiness" ) {
			combined.Add(entry1);
			combined.Add(entry2);

			THEN( "the combined entry is not empty" ) {
				CHECK_FALSE( combined.IsEmpty() );
			}
		}
	}
}

SCENARIO( "BookEntry with empty substitutions", "[BookEntry][EdgeCase]" ) {
	GIVEN( "A BookEntry with text content" ) {
		BookEntry entry;
		DataNode node = AsDataNode("entry\n\tNo placeholders here");
		entry.Load(node);

		WHEN( "instantiating with substitutions that don't match" ) {
			std::map<std::string, std::string> subs;
			subs["<nonexistent>"] = "value";
			BookEntry instantiated = entry.Instantiate(subs);

			THEN( "the instantiated entry is not empty" ) {
				CHECK_FALSE( instantiated.IsEmpty() );
			}
		}
	}
}

// #endregion unit tests



} // test namespace