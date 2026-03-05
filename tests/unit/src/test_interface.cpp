/* test_interface.cpp
Copyright (c) 2026 by test contributor

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
#include "../../../source/Interface.h"

// ... and any system includes needed for the test file.
#include "../../../source/Information.h"
#include "../../../source/Point.h"
#include "../../../source/Rectangle.h"

#include "datanode-factory.h"

#include <cmath>
#include <string>
#include <vector>

namespace { // test namespace

// #region mock data

// #endregion mock data



// #region unit tests
TEST_CASE( "Interface Basics", "[Interface]" ) {
	using T = Interface;
	SECTION( "Class Traits" ) {
		CHECK( std::is_default_constructible_v<T> );
		CHECK( std::is_nothrow_default_constructible_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
		CHECK( std::is_trivially_destructible_v<T> == false );
	}
	SECTION( "Copy Traits" ) {
		// Interface has unique_ptr members, but type traits report copy constructible as true
		// because the compiler doesn't validate implicit constructor validity until instantiation.
		CHECK( std::is_copy_constructible_v<T> );
		CHECK_FALSE( std::is_copy_assignable_v<T> );
	}
	SECTION( "Move Traits" ) {
		CHECK( std::is_move_constructible_v<T> );
		CHECK( std::is_nothrow_move_constructible_v<T> );
		CHECK( std::is_move_assignable_v<T> );
		CHECK( std::is_nothrow_move_assignable_v<T> );
	}
}

SCENARIO( "An Interface must be constructed", "[Interface][Construction]" ) {
	GIVEN( "No initial values" ) {
		WHEN( "a default-constructed Interface is created" ) {
			Interface iface;
			THEN( "it has no points, values, or lists" ) {
				CHECK_FALSE( iface.HasPoint("any") );
				CHECK( iface.GetPoint("any") == Point() );
				// Rectangle doesn't have operator==, check dimensions instead
				Rectangle r = iface.GetBox("any");
				CHECK_THAT( r.Dimensions().X(), Catch::Matchers::WithinAbs(0., 0.001) );
				CHECK_THAT( r.Dimensions().Y(), Catch::Matchers::WithinAbs(0., 0.001) );
				CHECK( iface.GetValue("any") == 0. );
				CHECK( iface.GetList("any").empty() );
			}
		}
	}
}

SCENARIO( "Interface can be loaded from DataNode", "[Interface][Load]" ) {
	GIVEN( "an empty interface" ) {
		Interface iface;
		WHEN( "loading an unnamed interface" ) {
			auto node = AsDataNode("interface");
			iface.Load(node);
			THEN( "the interface remains empty" ) {
				CHECK_FALSE( iface.HasPoint("any") );
			}
		}
		WHEN( "loading a named interface with no elements" ) {
			auto node = AsDataNode("interface test");
			iface.Load(node);
			THEN( "the interface has no elements" ) {
				CHECK_FALSE( iface.HasPoint("any") );
				CHECK( iface.GetValue("any") == 0. );
			}
		}
	}

	GIVEN( "a named interface with a value" ) {
		Interface iface;
		WHEN( "loading the interface with a value element" ) {
			auto node = AsDataNode(R"(interface test
	value speed 100.5
)");
			iface.Load(node);
			THEN( "the value is stored correctly" ) {
				CHECK( iface.GetValue("speed") == 100.5 );
			}
			THEN( "other values return default" ) {
				CHECK( iface.GetValue("other") == 0. );
			}
		}
	}

	GIVEN( "a named interface with a point" ) {
		Interface iface;
		WHEN( "loading the interface with a point element" ) {
			auto node = AsDataNode(R"(interface test
	point myPoint
		from 10 20
)");
			iface.Load(node);
			THEN( "the point exists" ) {
				CHECK( iface.HasPoint("myPoint") );
			}
		}
		WHEN( "loading the interface with a box element" ) {
			auto node = AsDataNode(R"(interface test
	box myBox
		from 0 0 to 100 50
)");
			iface.Load(node);
			THEN( "the box exists as a point" ) {
				CHECK( iface.HasPoint("myBox") );
			}
		}
	}

	GIVEN( "a named interface with a list" ) {
		Interface iface;
		WHEN( "loading the interface with a list element" ) {
			auto node = AsDataNode(R"(interface test
	list numbers
		1.5
		2.5
		3.5
)");
			iface.Load(node);
			THEN( "the list is stored correctly" ) {
				const auto& list = iface.GetList("numbers");
				REQUIRE( list.size() == 3 );
				CHECK( list[0] == 1.5 );
				CHECK( list[1] == 2.5 );
				CHECK( list[2] == 3.5 );
			}
			THEN( "non-existent lists return empty" ) {
				CHECK( iface.GetList("nonexistent").empty() );
			}
		}
	}
}

SCENARIO( "Interface re-loading clears previous data", "[Interface][Load]" ) {
	GIVEN( "an interface with existing data" ) {
		Interface iface;
		auto node1 = AsDataNode(R"(interface test
	value existing 100
	point myPoint
		from 10 20
)");
		iface.Load(node1);
		REQUIRE( iface.GetValue("existing") == 100. );
		REQUIRE( iface.HasPoint("myPoint") );

		WHEN( "re-loading with new data" ) {
			auto node2 = AsDataNode(R"(interface test
	value newvalue 200
)");
			iface.Load(node2);
			THEN( "previous data is cleared" ) {
				CHECK( iface.GetValue("existing") == 0. );
				CHECK_FALSE( iface.HasPoint("myPoint") );
			}
			THEN( "new data is present" ) {
				CHECK( iface.GetValue("newvalue") == 200. );
			}
		}
	}
}

SCENARIO( "Interface HasPoint method", "[Interface][HasPoint]" ) {
	GIVEN( "an empty interface" ) {
		Interface iface;
		WHEN( "checking for any point" ) {
			THEN( "no points exist" ) {
				CHECK_FALSE( iface.HasPoint("any") );
				CHECK_FALSE( iface.HasPoint("") );
				CHECK_FALSE( iface.HasPoint("nonexistent") );
			}
		}
	}

	GIVEN( "an interface with points" ) {
		Interface iface;
		auto node = AsDataNode(R"(interface test
	point alpha
		from 0 0
	point beta
		from 10 10
	box gamma
		from 0 0 to 20 20
)");
		iface.Load(node);

		WHEN( "checking for existing points" ) {
			THEN( "the points exist" ) {
				CHECK( iface.HasPoint("alpha") );
				CHECK( iface.HasPoint("beta") );
				CHECK( iface.HasPoint("gamma") );
			}
		}
		WHEN( "checking for non-existing points" ) {
			THEN( "the points do not exist" ) {
				CHECK_FALSE( iface.HasPoint("delta") );
				CHECK_FALSE( iface.HasPoint("Alpha") ); // case sensitive
				CHECK_FALSE( iface.HasPoint("") );
			}
		}
	}
}

SCENARIO( "Interface GetPoint method", "[Interface][GetPoint]" ) {
	GIVEN( "an empty interface" ) {
		Interface iface;
		WHEN( "getting any point" ) {
			THEN( "returns zero point" ) {
				CHECK( iface.GetPoint("any") == Point() );
				CHECK( iface.GetPoint("") == Point() );
			}
		}
	}

	GIVEN( "an interface with a point defined by center" ) {
		Interface iface;
		auto node = AsDataNode(R"(interface test
	point myPoint
		center 50 100
)");
		iface.Load(node);

		WHEN( "getting the point" ) {
			Point p = iface.GetPoint("myPoint");
			THEN( "the center is returned" ) {
				CHECK_THAT( p.X(), Catch::Matchers::WithinAbs(50., 0.001) );
				CHECK_THAT( p.Y(), Catch::Matchers::WithinAbs(100., 0.001) );
			}
		}
	}

	GIVEN( "an interface with a box defined by from-to" ) {
		Interface iface;
		auto node = AsDataNode(R"(interface test
	point myBox
		from 0 0 to 100 50
)");
		iface.Load(node);

		WHEN( "getting the point" ) {
			Point p = iface.GetPoint("myBox");
			THEN( "the center of the box is returned" ) {
				CHECK_THAT( p.X(), Catch::Matchers::WithinAbs(50., 0.001) );
				CHECK_THAT( p.Y(), Catch::Matchers::WithinAbs(25., 0.001) );
			}
		}
	}
}

SCENARIO( "Interface GetBox method", "[Interface][GetBox]" ) {
	GIVEN( "an empty interface" ) {
		Interface iface;
		WHEN( "getting any box" ) {
			THEN( "returns empty rectangle" ) {
				Rectangle r = iface.GetBox("any");
				// Rectangle doesn't have operator==, check dimensions
				CHECK_THAT( r.Dimensions().X(), Catch::Matchers::WithinAbs(0., 0.001) );
				CHECK_THAT( r.Dimensions().Y(), Catch::Matchers::WithinAbs(0., 0.001) );
			}
		}
	}

	GIVEN( "an interface with a box defined by from-to" ) {
		Interface iface;
		auto node = AsDataNode(R"(interface test
	box myBox
		from 10 20 to 110 70
)");
		iface.Load(node);

		WHEN( "getting the box" ) {
			Rectangle r = iface.GetBox("myBox");
			THEN( "the correct rectangle is returned" ) {
				// Center should be (60, 45), dimensions should be (100, 50)
				CHECK_THAT( r.Center().X(), Catch::Matchers::WithinAbs(60., 0.001) );
				CHECK_THAT( r.Center().Y(), Catch::Matchers::WithinAbs(45., 0.001) );
				CHECK_THAT( r.Width(), Catch::Matchers::WithinAbs(100., 0.001) );
				CHECK_THAT( r.Height(), Catch::Matchers::WithinAbs(50., 0.001) );
			}
		}
	}

	GIVEN( "an interface with a box defined by center and dimensions" ) {
		Interface iface;
		auto node = AsDataNode(R"(interface test
	box myBox
		center 50 50
		dimensions 80 40
)");
		iface.Load(node);

		WHEN( "getting the box" ) {
			Rectangle r = iface.GetBox("myBox");
			THEN( "the correct rectangle is returned" ) {
				CHECK_THAT( r.Center().X(), Catch::Matchers::WithinAbs(50., 0.001) );
				CHECK_THAT( r.Center().Y(), Catch::Matchers::WithinAbs(50., 0.001) );
				CHECK_THAT( r.Width(), Catch::Matchers::WithinAbs(80., 0.001) );
				CHECK_THAT( r.Height(), Catch::Matchers::WithinAbs(40., 0.001) );
			}
		}
	}
}

SCENARIO( "Interface GetValue method", "[Interface][GetValue]" ) {
	GIVEN( "an empty interface" ) {
		Interface iface;
		WHEN( "getting any value" ) {
			THEN( "returns zero" ) {
				CHECK( iface.GetValue("any") == 0. );
				CHECK( iface.GetValue("") == 0. );
			}
		}
	}

	GIVEN( "an interface with multiple values" ) {
		Interface iface;
		auto node = AsDataNode(R"(interface test
	value alpha 10.5
	value beta -25.3
	value gamma 0
)");
		iface.Load(node);

		WHEN( "getting the values" ) {
			THEN( "correct values are returned" ) {
				CHECK( iface.GetValue("alpha") == 10.5 );
				CHECK( iface.GetValue("beta") == -25.3 );
				CHECK( iface.GetValue("gamma") == 0. );
			}
		}
		WHEN( "getting a non-existent value" ) {
			THEN( "returns zero" ) {
				CHECK( iface.GetValue("delta") == 0. );
			}
		}
	}
}

SCENARIO( "Interface GetList method", "[Interface][GetList]" ) {
	GIVEN( "an empty interface" ) {
		Interface iface;
		WHEN( "getting any list" ) {
			THEN( "returns empty list" ) {
				CHECK( iface.GetList("any").empty() );
				CHECK( iface.GetList("").empty() );
			}
		}
	}

	GIVEN( "an interface with lists" ) {
		Interface iface;
		auto node = AsDataNode(R"(interface test
	list empty
	list single
		42
	list multiple
		1
		2
		3
		4
		5
)");
		iface.Load(node);

		WHEN( "getting the lists" ) {
			THEN( "empty list is empty" ) {
				CHECK( iface.GetList("empty").empty() );
			}
			THEN( "single element list is correct" ) {
				const auto& list = iface.GetList("single");
				REQUIRE( list.size() == 1 );
				CHECK( list[0] == 42. );
			}
			THEN( "multiple element list is correct" ) {
				const auto& list = iface.GetList("multiple");
				REQUIRE( list.size() == 5 );
				CHECK( list[0] == 1. );
				CHECK( list[1] == 2. );
				CHECK( list[2] == 3. );
				CHECK( list[3] == 4. );
				CHECK( list[4] == 5. );
			}
		}
		WHEN( "getting a non-existent list" ) {
			THEN( "returns empty list" ) {
				CHECK( iface.GetList("nonexistent").empty() );
			}
		}
	}
}

SCENARIO( "Interface Draw method with Information", "[Interface][Draw]" ) {
	GIVEN( "an empty interface" ) {
		Interface iface;
		Information info;
		WHEN( "drawing with no panel" ) {
			THEN( "no crash occurs" ) {
				CHECK_NOTHROW( iface.Draw(info, nullptr) );
			}
		}
	}

	GIVEN( "an interface with elements" ) {
		Interface iface;
		auto node = AsDataNode(R"(interface test
	value health 100
	point statusPos
		center 100 100
)");
		iface.Load(node);
		Information info;

		WHEN( "drawing the interface" ) {
			THEN( "no crash occurs" ) {
				CHECK_NOTHROW( iface.Draw(info, nullptr) );
			}
		}
	}
}

SCENARIO( "Interface with anchor settings", "[Interface][Anchor]" ) {
	GIVEN( "an interface with different anchor settings" ) {
		Interface iface;
		WHEN( "loading with left top anchor" ) {
			auto node = AsDataNode(R"(interface test left top
	point myPoint
		center 50 50
)");
			iface.Load(node);
			THEN( "the point exists" ) {
				CHECK( iface.HasPoint("myPoint") );
			}
		}
		WHEN( "loading with center anchor" ) {
			auto node = AsDataNode(R"(interface test center
	point myPoint
		center 50 50
)");
			iface.Load(node);
			THEN( "the point exists" ) {
				CHECK( iface.HasPoint("myPoint") );
			}
		}
		WHEN( "loading with right bottom anchor" ) {
			auto node = AsDataNode(R"(interface test right bottom
	point myPoint
		center 50 50
)");
			iface.Load(node);
			THEN( "the point exists" ) {
				CHECK( iface.HasPoint("myPoint") );
			}
		}
	}
}

SCENARIO( "Interface with visible and active conditions", "[Interface][Conditions]" ) {
	GIVEN( "an interface with visibility conditions" ) {
		Interface iface;
		auto node = AsDataNode(R"(interface test
	visible if hasPermission
	point protected
		center 100 100
	active if canEdit
	point editable
		center 200 200
)");
		iface.Load(node);

		WHEN( "checking points exist" ) {
			THEN( "points are loaded regardless of conditions" ) {
				CHECK( iface.HasPoint("protected") );
				CHECK( iface.HasPoint("editable") );
			}
		}
	}
}

SCENARIO( "Interface value with negative numbers", "[Interface][Edge]" ) {
	GIVEN( "an interface with negative values" ) {
		Interface iface;
		auto node = AsDataNode(R"(interface test
	value negative -100.5
	value zero 0
	value verySmall -0.00001
)");
		iface.Load(node);

		WHEN( "getting the values" ) {
			THEN( "negative values are stored correctly" ) {
				CHECK( iface.GetValue("negative") == -100.5 );
				CHECK( iface.GetValue("zero") == 0. );
				CHECK_THAT( iface.GetValue("verySmall"), Catch::Matchers::WithinAbs(-0.00001, 0.000001) );
			}
		}
	}
}

SCENARIO( "Interface with large number of elements", "[Interface][Edge]" ) {
	GIVEN( "an interface with many values" ) {
		Interface iface;
		std::string data = "interface test\n";
		for(int i = 0; i < 100; ++i)
			data += "\tvalue key" + std::to_string(i) + " " + std::to_string(i * 10) + "\n";

		auto node = AsDataNode(data);
		iface.Load(node);

		WHEN( "accessing values" ) {
			THEN( "all values are stored correctly" ) {
				for(int i = 0; i < 100; ++i)
					CHECK( iface.GetValue("key" + std::to_string(i)) == i * 10. );
			}
		}
	}
}

SCENARIO( "Interface list with various number types", "[Interface][List][Edge]" ) {
	GIVEN( "an interface with list containing various number types" ) {
		Interface iface;
		auto node = AsDataNode(R"(interface test
	list mixed
		0
		-1
		1.5
		-2.5
		1000000
		0.000001
)");
		iface.Load(node);

		WHEN( "getting the list" ) {
			const auto& list = iface.GetList("mixed");
			THEN( "all values are stored correctly" ) {
				REQUIRE( list.size() == 6 );
				CHECK( list[0] == 0. );
				CHECK( list[1] == -1. );
				CHECK( list[2] == 1.5 );
				CHECK( list[3] == -2.5 );
				CHECK( list[4] == 1000000. );
				CHECK_THAT( list[5], Catch::Matchers::WithinAbs(0.000001, 0.0000001) );
			}
		}
	}
}

SCENARIO( "Interface point with various dimension specifications", "[Interface][Point][Dimensions]" ) {
	GIVEN( "an interface with points using width and height" ) {
		Interface iface;
		auto node = AsDataNode(R"(interface test
	point withDimensions
		center 50 50
		width 100
		height 60
	point withFrom
		from 0 0
		dimensions 80 40
)");
		iface.Load(node);

		WHEN( "getting the boxes" ) {
			THEN( "dimensions are correctly applied" ) {
				Rectangle r1 = iface.GetBox("withDimensions");
				CHECK_THAT( r1.Width(), Catch::Matchers::WithinAbs(100., 0.001) );
				CHECK_THAT( r1.Height(), Catch::Matchers::WithinAbs(60., 0.001) );

				Rectangle r2 = iface.GetBox("withFrom");
				CHECK_THAT( r2.Width(), Catch::Matchers::WithinAbs(80., 0.001) );
				CHECK_THAT( r2.Height(), Catch::Matchers::WithinAbs(40., 0.001) );
			}
		}
	}
}

SCENARIO( "Interface with empty names", "[Interface][Edge]" ) {
	GIVEN( "an interface with elements" ) {
		Interface iface;
		WHEN( "querying with empty name" ) {
			THEN( "returns default values without crash" ) {
				CHECK_FALSE( iface.HasPoint("") );
				CHECK( iface.GetPoint("") == Point() );
				// GetBox returns a Rectangle, check it doesn't crash
				(void)iface.GetBox("");
				CHECK( iface.GetValue("") == 0. );
				CHECK( iface.GetList("").empty() );
			}
		}
	}
}

SCENARIO( "Interface move semantics", "[Interface][Move]" ) {
	GIVEN( "an interface with data" ) {
		Interface iface;
		auto node = AsDataNode(R"(interface test
	value myValue 42
	point myPoint
		center 10 20
)");
		iface.Load(node);

		WHEN( "move constructing" ) {
			Interface moved(std::move(iface));
			THEN( "data is transferred" ) {
				CHECK( moved.GetValue("myValue") == 42. );
				CHECK( moved.HasPoint("myPoint") );
			}
		}

		WHEN( "move assigning" ) {
			Interface other;
			other = std::move(iface);
			THEN( "data is transferred" ) {
				CHECK( other.GetValue("myValue") == 42. );
				CHECK( other.HasPoint("myPoint") );
			}
		}
	}
}
// #endregion unit tests



} // test namespace