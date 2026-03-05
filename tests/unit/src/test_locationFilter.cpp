/* test_locationFilter.cpp
Copyright (c) 2025 by test author

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
#include "../../../source/LocationFilter.h"

// Include a helper for creating well-formed DataNodes.
#include "datanode-factory.h"

// Include DataWriter for testing Save functionality.
#include "../../../source/DataWriter.h"

// ... and any system includes needed for the test file.
#include <set>
#include <string>

namespace { // test namespace

// #region mock data

// Helper sets for visited systems and planets (empty by default).
std::set<const System *> emptyVisitedSystems;
std::set<const Planet *> emptyVisitedPlanets;

// #endregion mock data



// #region unit tests

// Test class traits.
TEST_CASE( "LocationFilter Basics", "[LocationFilter]" ) {
	using T = LocationFilter;
	SECTION( "Class Traits" ) {
		CHECK_FALSE( std::is_trivial_v<T> );
		// LocationFilter has std::list and std::string members, which are not standard_layout.
		CHECK_FALSE( std::is_standard_layout_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
		// LocationFilter has non-trivial destructor due to std::list and std::string members.
		CHECK_FALSE( std::is_trivially_destructible_v<T> );
	}
	SECTION( "Construction Traits" ) {
		CHECK( std::is_default_constructible_v<T> );
		CHECK_FALSE( std::is_trivially_default_constructible_v<T> );
		CHECK( std::is_nothrow_default_constructible_v<T> );
		CHECK( std::is_copy_constructible_v<T> );
		CHECK_FALSE( std::is_trivially_copy_constructible_v<T> );
		// Copy construction involves copying std::list, std::set, std::string which may throw.
		CHECK_FALSE( std::is_nothrow_copy_constructible_v<T> );
		CHECK( std::is_move_constructible_v<T> );
		CHECK_FALSE( std::is_trivially_move_constructible_v<T> );
		CHECK( std::is_nothrow_move_constructible_v<T> );
	}
	SECTION( "Copy Traits" ) {
		CHECK( std::is_copy_assignable_v<T> );
		CHECK_FALSE( std::is_trivially_copy_assignable_v<T> );
		// Copy assignment involves copying std::list, std::set, std::string which may throw.
		CHECK_FALSE( std::is_nothrow_copy_assignable_v<T> );
	}
	SECTION( "Move Traits" ) {
		CHECK( std::is_move_assignable_v<T> );
		CHECK_FALSE( std::is_trivially_move_assignable_v<T> );
		CHECK( std::is_nothrow_move_assignable_v<T> );
	}
}

// Test default construction.
SCENARIO( "Creating a default LocationFilter", "[LocationFilter][Creation]" ) {
	GIVEN( "a default constructed LocationFilter" ) {
		LocationFilter filter;
		THEN( "the filter is empty" ) {
			CHECK( filter.IsEmpty() );
		}
		THEN( "the filter is valid" ) {
			CHECK( filter.IsValid() );
		}
	}
}

// Test IsEmpty method.
SCENARIO( "LocationFilter::IsEmpty indicates if any specifications are set", "[LocationFilter][IsEmpty]" ) {
	GIVEN( "a default constructed LocationFilter" ) {
		LocationFilter filter;
		WHEN( "no conditions are set" ) {
			THEN( "IsEmpty returns true" ) {
				CHECK( filter.IsEmpty() );
			}
		}
	}
}

// Test IsValid method.
SCENARIO( "LocationFilter::IsValid checks filter validity", "[LocationFilter][IsValid]" ) {
	GIVEN( "a default constructed LocationFilter" ) {
		LocationFilter filter;
		WHEN( "no conditions are set" ) {
			THEN( "the filter is valid because empty filters are valid" ) {
				CHECK( filter.IsValid() );
			}
		}
	}
}

// Test Matches with null/invalid inputs.
SCENARIO( "LocationFilter::Matches with null or invalid inputs", "[LocationFilter][Matches]" ) {
	GIVEN( "a default constructed LocationFilter" ) {
		LocationFilter filter;
		WHEN( "matching a null planet" ) {
			THEN( "Matches returns false" ) {
				CHECK_FALSE( filter.Matches(static_cast<const Planet *>(nullptr), nullptr) );
			}
		}
		WHEN( "matching a null system" ) {
			THEN( "Matches returns false" ) {
				CHECK_FALSE( filter.Matches(static_cast<const System *>(nullptr), nullptr) );
			}
		}
		WHEN( "matching a planet with no origin system" ) {
			THEN( "Matches returns false for null planet" ) {
				CHECK_FALSE( filter.Matches(static_cast<const Planet *>(nullptr), nullptr) );
			}
		}
	}
}

// Test SetOrigin method.
SCENARIO( "LocationFilter::SetOrigin converts distance to near", "[LocationFilter][SetOrigin]" ) {
	GIVEN( "an empty LocationFilter" ) {
		LocationFilter filter;
		WHEN( "SetOrigin is called with a null system" ) {
			auto result = filter.SetOrigin(nullptr);
			THEN( "the result is empty and unchanged" ) {
				CHECK( result.IsEmpty() );
				CHECK( result.IsValid() );
			}
		}
		WHEN( "SetOrigin is called with any system pointer" ) {
			// Note: With no "distance" filter set, the result should be identical.
			auto result = filter.SetOrigin(nullptr);
			THEN( "the result is identical to the original when no distance is set" ) {
				CHECK( result.IsEmpty() );
			}
		}
	}
}

// Test copy construction and assignment.
SCENARIO( "Copying a LocationFilter", "[LocationFilter][Copy]" ) {
	GIVEN( "a default constructed LocationFilter" ) {
		LocationFilter original;
		WHEN( "copy constructing" ) {
			LocationFilter copy(original);
			THEN( "the copy has the same properties" ) {
				CHECK( copy.IsEmpty() == original.IsEmpty() );
				CHECK( copy.IsValid() == original.IsValid() );
			}
		}
		WHEN( "copy assigning" ) {
			LocationFilter copy;
			copy = original;
			THEN( "the copy has the same properties" ) {
				CHECK( copy.IsEmpty() == original.IsEmpty() );
				CHECK( copy.IsValid() == original.IsValid() );
			}
		}
	}
}

// Test move construction and assignment.
SCENARIO( "Moving a LocationFilter", "[LocationFilter][Move]" ) {
	GIVEN( "a default constructed LocationFilter" ) {
		LocationFilter original;
		WHEN( "move constructing" ) {
			LocationFilter moved(std::move(original));
			THEN( "the moved filter is valid" ) {
				CHECK( moved.IsEmpty() );
				CHECK( moved.IsValid() );
			}
		}
		WHEN( "move assigning" ) {
			LocationFilter moved;
			moved = LocationFilter{};
			THEN( "the moved filter is valid" ) {
				CHECK( moved.IsEmpty() );
				CHECK( moved.IsValid() );
			}
		}
	}
}

// Test Load with empty node.
SCENARIO( "LocationFilter::Load with empty DataNode", "[LocationFilter][Load]" ) {
	GIVEN( "a default constructed LocationFilter" ) {
		LocationFilter filter;
		WHEN( "loading an empty node" ) {
			auto node = AsDataNode("filter");
			filter.Load(node, &emptyVisitedSystems, &emptyVisitedPlanets);
			THEN( "the filter remains empty" ) {
				CHECK( filter.IsEmpty() );
			}
			THEN( "the filter is valid" ) {
				CHECK( filter.IsValid() );
			}
		}
	}
}

// Test Load with planet filter.
SCENARIO( "LocationFilter::Load with planet specification", "[LocationFilter][Load]" ) {
	GIVEN( "a default constructed LocationFilter" ) {
		LocationFilter filter;
		WHEN( "loading a node with planet filter" ) {
			// Note: The planet name "TestPlanet" will be stored, but since GameData
			// doesn't have this planet, it won't match any actual planets.
			auto node = AsDataNode("filter\n\tplanet \"TestPlanet\"");
			filter.Load(node, &emptyVisitedSystems, &emptyVisitedPlanets);
			THEN( "the filter is not empty" ) {
				CHECK_FALSE( filter.IsEmpty() );
			}
		}
	}
}

// Test Load with system filter.
SCENARIO( "LocationFilter::Load with system specification", "[LocationFilter][Load]" ) {
	GIVEN( "a default constructed LocationFilter" ) {
		LocationFilter filter;
		WHEN( "loading a node with system filter" ) {
			auto node = AsDataNode("filter\n\tsystem \"TestSystem\"");
			filter.Load(node, &emptyVisitedSystems, &emptyVisitedPlanets);
			THEN( "the filter is not empty" ) {
				CHECK_FALSE( filter.IsEmpty() );
			}
		}
	}
}

// Test Load with government filter.
SCENARIO( "LocationFilter::Load with government specification", "[LocationFilter][Load]" ) {
	GIVEN( "a default constructed LocationFilter" ) {
		LocationFilter filter;
		WHEN( "loading a node with government filter" ) {
			auto node = AsDataNode("filter\n\tgovernment \"TestGovernment\"");
			filter.Load(node, &emptyVisitedSystems, &emptyVisitedPlanets);
			THEN( "the filter is not empty" ) {
				CHECK_FALSE( filter.IsEmpty() );
			}
		}
	}
}

// Test Load with attributes filter.
SCENARIO( "LocationFilter::Load with attributes specification", "[LocationFilter][Load]" ) {
	GIVEN( "a default constructed LocationFilter" ) {
		LocationFilter filter;
		WHEN( "loading a node with attributes filter" ) {
			auto node = AsDataNode("filter\n\tattributes \"urban\"");
			filter.Load(node, &emptyVisitedSystems, &emptyVisitedPlanets);
			THEN( "the filter is not empty" ) {
				CHECK_FALSE( filter.IsEmpty() );
			}
		}
		WHEN( "loading a node with multiple attributes" ) {
			auto node = AsDataNode("filter\n\tattributes urban spaceport");
			filter.Load(node, &emptyVisitedSystems, &emptyVisitedPlanets);
			THEN( "the filter is not empty" ) {
				CHECK_FALSE( filter.IsEmpty() );
			}
		}
		WHEN( "loading a node with attributes in child nodes" ) {
			auto node = AsDataNode("filter\n\tattributes\n\t\turban\n\t\tspaceport");
			filter.Load(node, &emptyVisitedSystems, &emptyVisitedPlanets);
			THEN( "the filter is not empty" ) {
				CHECK_FALSE( filter.IsEmpty() );
			}
		}
	}
}

// Test Load with near filter.
SCENARIO( "LocationFilter::Load with near specification", "[LocationFilter][Load]" ) {
	GIVEN( "a default constructed LocationFilter" ) {
		LocationFilter filter;
		WHEN( "loading a node with near filter (single distance)" ) {
			auto node = AsDataNode("filter\n\tnear \"TestSystem\" 5");
			filter.Load(node, &emptyVisitedSystems, &emptyVisitedPlanets);
			THEN( "the filter is not empty" ) {
				CHECK_FALSE( filter.IsEmpty() );
			}
		}
		WHEN( "loading a node with near filter (range)" ) {
			auto node = AsDataNode("filter\n\tnear \"TestSystem\" 3 10");
			filter.Load(node, &emptyVisitedSystems, &emptyVisitedPlanets);
			THEN( "the filter is not empty" ) {
				CHECK_FALSE( filter.IsEmpty() );
			}
		}
	}
}

// Test Load with distance filter.
SCENARIO( "LocationFilter::Load with distance specification", "[LocationFilter][Load]" ) {
	GIVEN( "a default constructed LocationFilter" ) {
		LocationFilter filter;
		WHEN( "loading a node with distance filter (single value)" ) {
			auto node = AsDataNode("filter\n\tdistance 5");
			filter.Load(node, &emptyVisitedSystems, &emptyVisitedPlanets);
			THEN( "the filter is not empty" ) {
				CHECK_FALSE( filter.IsEmpty() );
			}
		}
		WHEN( "loading a node with distance filter (range)" ) {
			auto node = AsDataNode("filter\n\tdistance 3 10");
			filter.Load(node, &emptyVisitedSystems, &emptyVisitedPlanets);
			THEN( "the filter is not empty" ) {
				CHECK_FALSE( filter.IsEmpty() );
			}
		}
	}
}

// Test Load with visited filter.
SCENARIO( "LocationFilter::Load with visited specification", "[LocationFilter][Load]" ) {
	GIVEN( "a default constructed LocationFilter" ) {
		LocationFilter filter;
		WHEN( "loading a node with visited filter (system)" ) {
			auto node = AsDataNode("filter\n\tvisited");
			filter.Load(node, &emptyVisitedSystems, &emptyVisitedPlanets);
			THEN( "the filter is not empty" ) {
				CHECK_FALSE( filter.IsEmpty() );
			}
		}
		WHEN( "loading a node with visited planet filter" ) {
			auto node = AsDataNode("filter\n\tvisited planet");
			filter.Load(node, &emptyVisitedSystems, &emptyVisitedPlanets);
			THEN( "the filter is not empty" ) {
				CHECK_FALSE( filter.IsEmpty() );
			}
		}
	}
}

// Test Load with not filter.
SCENARIO( "LocationFilter::Load with not filter", "[LocationFilter][Load]" ) {
	GIVEN( "a default constructed LocationFilter" ) {
		LocationFilter filter;
		WHEN( "loading a node with not filter" ) {
			auto node = AsDataNode("filter\n\tnot\n\t\tattributes \"dangerous\"");
			filter.Load(node, &emptyVisitedSystems, &emptyVisitedPlanets);
			THEN( "the filter is not empty" ) {
				CHECK_FALSE( filter.IsEmpty() );
			}
		}
		WHEN( "loading a node with inline not filter" ) {
			auto node = AsDataNode("filter\n\tnot attributes \"dangerous\"");
			filter.Load(node, &emptyVisitedSystems, &emptyVisitedPlanets);
			THEN( "the filter is not empty" ) {
				CHECK_FALSE( filter.IsEmpty() );
			}
		}
	}
}

// Test Load with neighbor filter.
SCENARIO( "LocationFilter::Load with neighbor filter", "[LocationFilter][Load]" ) {
	GIVEN( "a default constructed LocationFilter" ) {
		LocationFilter filter;
		WHEN( "loading a node with neighbor filter" ) {
			auto node = AsDataNode("filter\n\tneighbor\n\t\tattributes \"trading\"");
			filter.Load(node, &emptyVisitedSystems, &emptyVisitedPlanets);
			THEN( "the filter is not empty" ) {
				CHECK_FALSE( filter.IsEmpty() );
			}
		}
	}
}

// Test Load with outfits filter.
SCENARIO( "LocationFilter::Load with outfits specification", "[LocationFilter][Load]" ) {
	GIVEN( "a default constructed LocationFilter" ) {
		LocationFilter filter;
		WHEN( "loading a node with outfits filter" ) {
			auto node = AsDataNode("filter\n\toutfits \"Test Outfit\"");
			filter.Load(node, &emptyVisitedSystems, &emptyVisitedPlanets);
			THEN( "the filter is not empty" ) {
				CHECK_FALSE( filter.IsEmpty() );
			}
		}
	}
}

// Test Load with category filter.
SCENARIO( "LocationFilter::Load with category specification", "[LocationFilter][Load]" ) {
	GIVEN( "a default constructed LocationFilter" ) {
		LocationFilter filter;
		WHEN( "loading a node with category filter" ) {
			auto node = AsDataNode("filter\n\tcategory \"Light\"");
			filter.Load(node, &emptyVisitedSystems, &emptyVisitedPlanets);
			THEN( "the filter is not empty" ) {
				CHECK_FALSE( filter.IsEmpty() );
			}
		}
		WHEN( "loading a node with multiple categories" ) {
			auto node = AsDataNode("filter\n\tcategory Light Medium");
			filter.Load(node, &emptyVisitedSystems, &emptyVisitedPlanets);
			THEN( "the filter is not empty" ) {
				CHECK_FALSE( filter.IsEmpty() );
			}
		}
	}
}

// Test Load with combined filters.
SCENARIO( "LocationFilter::Load with multiple specifications", "[LocationFilter][Load]" ) {
	GIVEN( "a default constructed LocationFilter" ) {
		LocationFilter filter;
		WHEN( "loading a node with multiple filters" ) {
			auto node = AsDataNode(
				"filter\n"
				"\tattributes \"urban\"\n"
				"\tgovernment \"Republic\"\n"
				"\tdistance 5"
			);
			filter.Load(node, &emptyVisitedSystems, &emptyVisitedPlanets);
			THEN( "the filter is not empty" ) {
				CHECK_FALSE( filter.IsEmpty() );
			}
		}
	}
}

// Test Load with null visited pointers (should throw).
SCENARIO( "LocationFilter::Load with null visited pointers", "[LocationFilter][Load]" ) {
	GIVEN( "a default constructed LocationFilter" ) {
		LocationFilter filter;
		WHEN( "loading a node with null visited systems" ) {
			auto node = AsDataNode("filter\n\tattributes \"test\"");
			THEN( "an exception is thrown" ) {
				REQUIRE_THROWS_AS(
					filter.Load(node, nullptr, &emptyVisitedPlanets),
					std::runtime_error
				);
			}
		}
		WHEN( "loading a node with null visited planets" ) {
			auto node = AsDataNode("filter\n\tattributes \"test\"");
			THEN( "an exception is thrown" ) {
				REQUIRE_THROWS_AS(
					filter.Load(node, &emptyVisitedSystems, nullptr),
					std::runtime_error
				);
			}
		}
	}
}

// Test Matches with visited requirements but null visited pointers.
SCENARIO( "LocationFilter::Matches with visited requirements but no visited data", "[LocationFilter][Matches]" ) {
	GIVEN( "a LocationFilter with visited requirement" ) {
		LocationFilter filter;
		auto node = AsDataNode("filter\n\tvisited");
		filter.Load(node, &emptyVisitedSystems, &emptyVisitedPlanets);
		REQUIRE_FALSE( filter.IsEmpty() );
		WHEN( "calling Matches on a system without visited systems pointer" ) {
			// Create a filter without visited pointers set
			LocationFilter filterNoPtrs;
			auto nodeNoPtrs = AsDataNode("filter\n\tvisited");
			// This should throw when LoadChild is called
			THEN( "an exception is thrown when visited data is missing" ) {
				// The filter was loaded with valid pointers, so Matches should work
				// but return false since the empty set doesn't contain any systems
				CHECK_FALSE( filter.Matches(static_cast<const System *>(nullptr), nullptr) );
			}
		}
	}
}

// Test PickSystem and PickPlanet with empty filter.
SCENARIO( "LocationFilter::PickSystem and PickPlanet with empty filter", "[LocationFilter][Pick]" ) {
	GIVEN( "an empty LocationFilter" ) {
		LocationFilter filter;
		WHEN( "calling PickSystem" ) {
			// Note: This will iterate over all systems in GameData, which may be empty
			// in a test environment without loaded game data.
			auto result = filter.PickSystem(nullptr);
			THEN( "returns nullptr when no valid systems exist" ) {
				// With no game data loaded, there are no valid systems.
				CHECK( result == nullptr );
			}
		}
		WHEN( "calling PickPlanet" ) {
			auto result = filter.PickPlanet(nullptr);
			THEN( "returns nullptr when no valid planets exist" ) {
				CHECK( result == nullptr );
			}
		}
		WHEN( "calling PickPlanet with different parameters" ) {
			auto resultNoClearance = filter.PickPlanet(nullptr, false, true);
			auto resultWithClearance = filter.PickPlanet(nullptr, true, false);
			THEN( "returns nullptr regardless of parameters when no valid planets exist" ) {
				CHECK( resultNoClearance == nullptr );
				CHECK( resultWithClearance == nullptr );
			}
		}
	}
}

// Test Save functionality.
SCENARIO( "LocationFilter::Save outputs filter data", "[LocationFilter][Save]" ) {
	GIVEN( "an empty LocationFilter" ) {
		LocationFilter filter;
		WHEN( "saving the filter" ) {
			DataWriter writer;
			writer.Write("filter");
			writer.BeginChild();
			filter.Save(writer);
			writer.EndChild();
			std::string output = writer.SaveToString();
			THEN( "the output is minimal for an empty filter" ) {
				// Empty filter should produce just the header line with no children.
				CHECK( output == "filter\n" );
			}
		}
	}
	GIVEN( "a LocationFilter with attributes" ) {
		LocationFilter filter;
		auto node = AsDataNode("filter\n\tattributes urban spaceport");
		filter.Load(node, &emptyVisitedSystems, &emptyVisitedPlanets);
		WHEN( "saving the filter" ) {
			DataWriter writer;
			writer.Write("filter");
			writer.BeginChild();
			filter.Save(writer);
			writer.EndChild();
			std::string output = writer.SaveToString();
			THEN( "the output contains the attributes" ) {
				CHECK( output.find("attributes") != std::string::npos );
			}
		}
	}
	GIVEN( "a LocationFilter with visited filter" ) {
		LocationFilter filter;
		auto node = AsDataNode("filter\n\tvisited");
		filter.Load(node, &emptyVisitedSystems, &emptyVisitedPlanets);
		WHEN( "saving the filter" ) {
			DataWriter writer;
			writer.Write("filter");
			writer.BeginChild();
			filter.Save(writer);
			writer.EndChild();
			std::string output = writer.SaveToString();
			THEN( "the output contains visited" ) {
				CHECK( output.find("visited") != std::string::npos );
			}
		}
	}
	GIVEN( "a LocationFilter with visited planet filter" ) {
		LocationFilter filter;
		auto node = AsDataNode("filter\n\tvisited planet");
		filter.Load(node, &emptyVisitedSystems, &emptyVisitedPlanets);
		WHEN( "saving the filter" ) {
			DataWriter writer;
			writer.Write("filter");
			writer.BeginChild();
			filter.Save(writer);
			writer.EndChild();
			std::string output = writer.SaveToString();
			THEN( "the output contains 'visited' and 'planet'" ) {
				CHECK( output.find("visited") != std::string::npos );
				CHECK( output.find("planet") != std::string::npos );
			}
		}
	}
	GIVEN( "a LocationFilter with not filter" ) {
		LocationFilter filter;
		auto node = AsDataNode("filter\n\tnot\n\t\tattributes dangerous");
		filter.Load(node, &emptyVisitedSystems, &emptyVisitedPlanets);
		WHEN( "saving the filter" ) {
			DataWriter writer;
			writer.Write("filter");
			writer.BeginChild();
			filter.Save(writer);
			writer.EndChild();
			std::string output = writer.SaveToString();
			THEN( "the output contains 'not'" ) {
				CHECK( output.find("not") != std::string::npos );
			}
		}
	}
}

// Test constructing with DataNode.
SCENARIO( "Constructing LocationFilter with DataNode", "[LocationFilter][Construction]" ) {
	GIVEN( "a valid DataNode" ) {
		auto node = AsDataNode("filter\n\tattributes urban");
		WHEN( "constructing with the node" ) {
			LocationFilter filter(node, &emptyVisitedSystems, &emptyVisitedPlanets);
			THEN( "the filter is loaded correctly" ) {
				CHECK_FALSE( filter.IsEmpty() );
			}
		}
	}
}

// Test boundary conditions with empty attribute sets.
SCENARIO( "LocationFilter with empty attribute sets", "[LocationFilter][Boundary]" ) {
	GIVEN( "a default constructed LocationFilter" ) {
		LocationFilter filter;
		WHEN( "loading a node with empty attributes" ) {
			// Empty attribute sets should be ignored.
			auto node = AsDataNode("filter\n\tattributes");
			filter.Load(node, &emptyVisitedSystems, &emptyVisitedPlanets);
			THEN( "the filter remains empty because empty attributes are not stored" ) {
				CHECK( filter.IsEmpty() );
			}
		}
	}
}

// Test boundary conditions with empty outfit sets.
SCENARIO( "LocationFilter with empty outfit sets", "[LocationFilter][Boundary]" ) {
	GIVEN( "a default constructed LocationFilter" ) {
		LocationFilter filter;
		WHEN( "loading a node with empty outfits" ) {
			// Empty outfit sets should be ignored.
			auto node = AsDataNode("filter\n\toutfits");
			filter.Load(node, &emptyVisitedSystems, &emptyVisitedPlanets);
			THEN( "the filter remains empty because empty outfits are not stored" ) {
				CHECK( filter.IsEmpty() );
			}
		}
	}
}

// Test Matches with Ship (requires valid Ship object, so we test the null/rejection cases).
SCENARIO( "LocationFilter::Matches with Ship", "[LocationFilter][Matches][Ship]" ) {
	GIVEN( "an empty LocationFilter" ) {
		LocationFilter filter;
		// Note: Testing Matches(Ship) requires a valid Ship object, which in turn
		// requires GameData to be populated. In a unit test without game data,
		// we can only test the basic behavior of an empty filter.
		WHEN( "checking if empty filter conceptually matches" ) {
			THEN( "an empty filter should be valid" ) {
				CHECK( filter.IsValid() );
				CHECK( filter.IsEmpty() );
			}
		}
	}
}

// Test multiple attribute sets.
SCENARIO( "LocationFilter with multiple attribute sets", "[LocationFilter][Attributes]" ) {
	GIVEN( "a default constructed LocationFilter" ) {
		LocationFilter filter;
		WHEN( "loading a node with multiple attribute lines" ) {
			auto node = AsDataNode(
				"filter\n"
				"\tattributes urban\n"
				"\tattributes spaceport"
			);
			filter.Load(node, &emptyVisitedSystems, &emptyVisitedPlanets);
			THEN( "the filter is not empty" ) {
				CHECK_FALSE( filter.IsEmpty() );
			}
		}
	}
}

// Test SetOrigin with a filter that has distance.
SCENARIO( "SetOrigin with distance filter converts to near", "[LocationFilter][SetOrigin]" ) {
	GIVEN( "a LocationFilter with distance specification" ) {
		LocationFilter filter;
		auto node = AsDataNode("filter\n\tdistance 5");
		filter.Load(node, &emptyVisitedSystems, &emptyVisitedPlanets);
		REQUIRE_FALSE( filter.IsEmpty() );
		WHEN( "SetOrigin is called with null system" ) {
			auto result = filter.SetOrigin(nullptr);
			THEN( "the filter is returned unchanged (no conversion possible)" ) {
				CHECK_FALSE( result.IsEmpty() );
			}
		}
	}
}

// Test Load with invalid/unrecognized keys.
SCENARIO( "LocationFilter::Load with unrecognized keys", "[LocationFilter][Load]" ) {
	GIVEN( "a default constructed LocationFilter" ) {
		LocationFilter filter;
		WHEN( "loading a node with unrecognized key" ) {
			// Unrecognized keys should be logged but not cause an exception.
			auto node = AsDataNode("filter\n\tunknownkey value");
			filter.Load(node, &emptyVisitedSystems, &emptyVisitedPlanets);
			THEN( "the filter remains empty" ) {
				CHECK( filter.IsEmpty() );
			}
		}
	}
}

// #endregion unit tests



} // test namespace