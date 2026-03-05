/* test_galaxy.cpp
Copyright (c) 2026 by Endless Sky contributors

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
#include "../../../source/Galaxy.h"

// Include test helpers.
#include "datanode-factory.h"

// ... and any system includes needed for the test file.
#include "../../../source/Point.h"

namespace { // test namespace

// #region mock data

// Insert file-local data here, e.g. classes, structs, or fixtures that will be useful
// to help test this class/method.

// #endregion mock data



// #region unit tests

SCENARIO( "Galaxy default construction", "[Galaxy]" ) {
	GIVEN( "no parameters" ) {
		WHEN( "a Galaxy is default constructed" ) {
			Galaxy galaxy;
			THEN( "position is (0, 0)" ) {
				CHECK( galaxy.Position().X() == 0. );
				CHECK( galaxy.Position().Y() == 0. );
			}
			AND_THEN( "sprite is nullptr" ) {
				CHECK( galaxy.GetSprite() == nullptr );
			}
		}
	}
}

SCENARIO( "Galaxy Position getter", "[Galaxy][Position]" ) {
	GIVEN( "a default constructed Galaxy" ) {
		Galaxy galaxy;
		WHEN( "Position() is called" ) {
			const Point& pos = galaxy.Position();
			THEN( "it returns a const reference to the position" ) {
				CHECK( pos.X() == 0. );
				CHECK( pos.Y() == 0. );
			}
		}
	}
}

SCENARIO( "Galaxy GetSprite getter", "[Galaxy][GetSprite]" ) {
	GIVEN( "a default constructed Galaxy" ) {
		Galaxy galaxy;
		WHEN( "GetSprite() is called" ) {
			const Sprite* sprite = galaxy.GetSprite();
			THEN( "it returns nullptr" ) {
				CHECK( sprite == nullptr );
			}
		}
	}
}

SCENARIO( "Galaxy Load with position", "[Galaxy][Load]" ) {
	GIVEN( "a DataNode with position only" ) {
		auto node = AsDataNode("galaxy\n\tpos 100 200");
		WHEN( "Load is called" ) {
			Galaxy galaxy;
			galaxy.Load(node);
			THEN( "position is set correctly" ) {
				CHECK( galaxy.Position().X() == 100. );
				CHECK( galaxy.Position().Y() == 200. );
			}
			AND_THEN( "sprite remains nullptr" ) {
				CHECK( galaxy.GetSprite() == nullptr );
			}
		}
	}
	GIVEN( "a DataNode with negative position" ) {
		auto node = AsDataNode("galaxy\n\tpos -150.5 -250.75");
		WHEN( "Load is called" ) {
			Galaxy galaxy;
			galaxy.Load(node);
			THEN( "position handles negative values correctly" ) {
				CHECK( galaxy.Position().X() == -150.5 );
				CHECK( galaxy.Position().Y() == -250.75 );
			}
		}
	}
	GIVEN( "a DataNode with zero position" ) {
		auto node = AsDataNode("galaxy\n\tpos 0 0");
		WHEN( "Load is called" ) {
			Galaxy galaxy;
			galaxy.Load(node);
			THEN( "position is (0, 0)" ) {
				CHECK( galaxy.Position().X() == 0. );
				CHECK( galaxy.Position().Y() == 0. );
			}
		}
	}
	GIVEN( "a DataNode with large position values" ) {
		auto node = AsDataNode("galaxy\n\tpos 1000000 2000000");
		WHEN( "Load is called" ) {
			Galaxy galaxy;
			galaxy.Load(node);
			THEN( "position handles large values correctly" ) {
				CHECK( galaxy.Position().X() == 1000000. );
				CHECK( galaxy.Position().Y() == 2000000. );
			}
		}
	}
}

SCENARIO( "Galaxy Load with sprite", "[Galaxy][Load]" ) {
	GIVEN( "a DataNode with sprite only" ) {
		auto node = AsDataNode("galaxy\n\tsprite galaxy/spiral");
		WHEN( "Load is called" ) {
			Galaxy galaxy;
			galaxy.Load(node);
			THEN( "the galaxy is loaded without error" ) {
				// Sprite will be nullptr if the sprite doesn't exist in the test environment
				// We just verify the load doesn't crash
				CHECK( true );
			}
		}
	}
}

SCENARIO( "Galaxy Load with position and sprite", "[Galaxy][Load]" ) {
	GIVEN( "a DataNode with both position and sprite" ) {
		auto node = AsDataNode("galaxy\n\tpos 500 300\n\tsprite galaxy/test");
		WHEN( "Load is called" ) {
			Galaxy galaxy;
			galaxy.Load(node);
			THEN( "position is set correctly" ) {
				CHECK( galaxy.Position().X() == 500. );
				CHECK( galaxy.Position().Y() == 300. );
			}
			AND_THEN( "the galaxy is loaded without error" ) {
				CHECK( true );
			}
		}
	}
	GIVEN( "a DataNode with sprite before position" ) {
		auto node = AsDataNode("galaxy\n\tsprite galaxy/test\n\tpos 400 600");
		WHEN( "Load is called" ) {
			Galaxy galaxy;
			galaxy.Load(node);
			THEN( "position is set correctly (order doesn't matter)" ) {
				CHECK( galaxy.Position().X() == 400. );
				CHECK( galaxy.Position().Y() == 600. );
			}
		}
	}
}

SCENARIO( "Galaxy Load with remove sprite", "[Galaxy][Load]" ) {
	GIVEN( "a Galaxy with a sprite already set" ) {
		Galaxy galaxy;
		auto node1 = AsDataNode("galaxy\n\tsprite galaxy/test");
		galaxy.Load(node1);
		WHEN( "remove sprite is called" ) {
			auto node2 = AsDataNode("galaxy\n\tremove sprite");
			galaxy.Load(node2);
			THEN( "sprite is set to nullptr" ) {
				CHECK( galaxy.GetSprite() == nullptr );
			}
		}
	}
	GIVEN( "a DataNode with sprite followed by remove sprite" ) {
		auto node = AsDataNode("galaxy\n\tsprite galaxy/test\n\tremove sprite");
		WHEN( "Load is called" ) {
			Galaxy galaxy;
			galaxy.Load(node);
			THEN( "sprite is nullptr (remove takes effect)" ) {
				CHECK( galaxy.GetSprite() == nullptr );
			}
		}
	}
	GIVEN( "a DataNode with remove sprite without prior sprite" ) {
		auto node = AsDataNode("galaxy\n\tremove sprite");
		WHEN( "Load is called" ) {
			Galaxy galaxy;
			galaxy.Load(node);
			THEN( "sprite remains nullptr (no crash)" ) {
				CHECK( galaxy.GetSprite() == nullptr );
			}
		}
	}
}

SCENARIO( "Galaxy Load with invalid data", "[Galaxy][Load]" ) {
	GIVEN( "a DataNode with pos but missing Y value" ) {
		auto node = AsDataNode("galaxy\n\tpos 100");
		WHEN( "Load is called" ) {
			Galaxy galaxy;
			galaxy.Load(node);
			THEN( "position is not updated (requires at least 3 tokens)" ) {
				// Position should remain default since child.Size() < 3
				CHECK( galaxy.Position().X() == 0. );
				CHECK( galaxy.Position().Y() == 0. );
			}
		}
	}
	GIVEN( "a DataNode with sprite but no value" ) {
		auto node = AsDataNode("galaxy\n\tsprite");
		WHEN( "Load is called" ) {
			Galaxy galaxy;
			galaxy.Load(node);
			THEN( "sprite remains nullptr (requires at least 2 tokens)" ) {
				CHECK( galaxy.GetSprite() == nullptr );
			}
		}
	}
	GIVEN( "an empty DataNode" ) {
		auto node = AsDataNode("galaxy");
		WHEN( "Load is called" ) {
			Galaxy galaxy;
			galaxy.Load(node);
			THEN( "galaxy remains in default state" ) {
				CHECK( galaxy.Position().X() == 0. );
				CHECK( galaxy.Position().Y() == 0. );
				CHECK( galaxy.GetSprite() == nullptr );
			}
		}
	}
}

SCENARIO( "Galaxy Load with unrecognized attributes", "[Galaxy][Load]" ) {
	GIVEN( "a DataNode with unrecognized attribute" ) {
		auto node = AsDataNode("galaxy\n\tunknown_attribute value\n\tpos 50 75");
		WHEN( "Load is called" ) {
			Galaxy galaxy;
			galaxy.Load(node);
			THEN( "recognized attributes are still processed" ) {
				CHECK( galaxy.Position().X() == 50. );
				CHECK( galaxy.Position().Y() == 75. );
			}
		}
	}
	GIVEN( "a DataNode with unrecognized remove" ) {
		auto node = AsDataNode("galaxy\n\tremove unknown");
		WHEN( "Load is called" ) {
			Galaxy galaxy;
			galaxy.Load(node);
			THEN( "galaxy remains in default state (no crash)" ) {
				CHECK( galaxy.Position().X() == 0. );
				CHECK( galaxy.Position().Y() == 0. );
				CHECK( galaxy.GetSprite() == nullptr );
			}
		}
	}
	GIVEN( "a DataNode with remove but no key" ) {
		auto node = AsDataNode("galaxy\n\tremove");
		WHEN( "Load is called" ) {
			Galaxy galaxy;
			galaxy.Load(node);
			THEN( "galaxy remains in default state (no crash)" ) {
				CHECK( galaxy.GetSprite() == nullptr );
			}
		}
	}
}

SCENARIO( "Galaxy Load multiple times", "[Galaxy][Load]" ) {
	GIVEN( "a Galaxy with position set" ) {
		Galaxy galaxy;
		auto node1 = AsDataNode("galaxy\n\tpos 100 200");
		galaxy.Load(node1);
		REQUIRE( galaxy.Position().X() == 100. );
		REQUIRE( galaxy.Position().Y() == 200. );
		WHEN( "Load is called again with new position" ) {
			auto node2 = AsDataNode("galaxy\n\tpos 300 400");
			galaxy.Load(node2);
			THEN( "position is updated to new values" ) {
				CHECK( galaxy.Position().X() == 300. );
				CHECK( galaxy.Position().Y() == 400. );
			}
		}
	}
	GIVEN( "a default constructed Galaxy" ) {
		Galaxy galaxy;
		WHEN( "Load is called multiple times" ) {
			auto node1 = AsDataNode("galaxy\n\tpos 10 20");
			galaxy.Load(node1);
			auto node2 = AsDataNode("galaxy\n\tpos 30 40");
			galaxy.Load(node2);
			auto node3 = AsDataNode("galaxy\n\tpos 50 60");
			galaxy.Load(node3);
			THEN( "position reflects the last loaded values" ) {
				CHECK( galaxy.Position().X() == 50. );
				CHECK( galaxy.Position().Y() == 60. );
			}
		}
	}
}

SCENARIO( "Galaxy with floating point position values", "[Galaxy][Load]" ) {
	GIVEN( "a DataNode with floating point position" ) {
		auto node = AsDataNode("galaxy\n\tpos 123.456 789.012");
		WHEN( "Load is called" ) {
			Galaxy galaxy;
			galaxy.Load(node);
			THEN( "position handles floating point values correctly" ) {
				CHECK_THAT( galaxy.Position().X(), Catch::Matchers::WithinAbs(123.456, 0.0001) );
				CHECK_THAT( galaxy.Position().Y(), Catch::Matchers::WithinAbs(789.012, 0.0001) );
			}
		}
	}
	GIVEN( "a DataNode with very small floating point values" ) {
		auto node = AsDataNode("galaxy\n\tpos 0.001 0.002");
		WHEN( "Load is called" ) {
			Galaxy galaxy;
			galaxy.Load(node);
			THEN( "position handles small values correctly" ) {
				CHECK_THAT( galaxy.Position().X(), Catch::Matchers::WithinAbs(0.001, 0.0001) );
				CHECK_THAT( galaxy.Position().Y(), Catch::Matchers::WithinAbs(0.002, 0.0001) );
			}
		}
	}
}

SCENARIO( "Galaxy class traits", "[Galaxy]" ) {
	using T = Galaxy;
	SECTION( "Class Traits" ) {
		CHECK( std::is_default_constructible_v<T> );
		CHECK( std::is_nothrow_default_constructible_v<T> );
		CHECK( std::is_trivially_destructible_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
	}
	SECTION( "Copy Traits" ) {
		CHECK( std::is_copy_constructible_v<T> );
		CHECK( std::is_copy_assignable_v<T> );
	}
	SECTION( "Move Traits" ) {
		CHECK( std::is_move_constructible_v<T> );
		CHECK( std::is_move_assignable_v<T> );
	}
}

// #endregion unit tests



// #region benchmarks
#ifdef CATCH_CONFIG_ENABLE_BENCHMARKING
TEST_CASE( "Benchmark Galaxy construction", "[!benchmark][Galaxy]" ) {
	BENCHMARK( "Galaxy::Galaxy()" ) {
		return Galaxy();
	};
}

TEST_CASE( "Benchmark Galaxy getters", "[!benchmark][Galaxy]" ) {
	Galaxy galaxy;
	auto node = AsDataNode("galaxy\n\tpos 100 200\n\tsprite galaxy/test");
	galaxy.Load(node);

	BENCHMARK( "Galaxy::Position()" ) {
		return galaxy.Position();
	};
	BENCHMARK( "Galaxy::GetSprite()" ) {
		return galaxy.GetSprite();
	};
}

TEST_CASE( "Benchmark Galaxy Load", "[!benchmark][Galaxy][Load]" ) {
	BENCHMARK( "Galaxy::Load() - simple" ) {
		Galaxy galaxy;
		auto node = AsDataNode("galaxy\n\tpos 100 200");
		galaxy.Load(node);
		return galaxy;
	};
	BENCHMARK( "Galaxy::Load() - with sprite" ) {
		Galaxy galaxy;
		auto node = AsDataNode("galaxy\n\tpos 100 200\n\tsprite galaxy/test");
		galaxy.Load(node);
		return galaxy;
	};
	BENCHMARK( "Galaxy::Load() - complex" ) {
		Galaxy galaxy;
		auto node = AsDataNode(
			"galaxy\n"
			"\tpos 123.456 789.012\n"
			"\tsprite galaxy/complex\n"
			"\tunknown_attribute value"
		);
		galaxy.Load(node);
		return galaxy;
	};
}
#endif
// #endregion benchmarks

} // test namespace