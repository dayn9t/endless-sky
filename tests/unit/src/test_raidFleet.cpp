/* test_raidFleet.cpp
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
#include "../../../source/RaidFleet.h"

// ... and any system includes needed for the test file.
#include <type_traits>
#include <vector>

namespace { // test namespace

// #region mock data

// Note: The RaidFleet class depends on the Fleet class, which itself depends on
// GameData for full functionality. However, we can still test RaidFleet's
// constructor and getter methods without GameData by using nullptr for Fleet*
// or by using a default-constructed Fleet.
//
// The static Load() method depends on GameData::Fleets().Get() and cannot be
// easily unit tested. It should be tested via integration tests.

// #endregion mock data



// #region unit tests
TEST_CASE( "RaidFleet Class Traits", "[RaidFleet]" ) {
	using T = RaidFleet;
	SECTION( "Class Layout" ) {
		CHECK_FALSE( std::is_trivial_v<T> );
		CHECK( std::is_standard_layout_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
	}
	SECTION( "Construction Traits" ) {
		// RaidFleet has no default constructor, only a 3-parameter constructor
		CHECK_FALSE( std::is_default_constructible_v<T> );
		CHECK( std::is_copy_constructible_v<T> );
		CHECK( std::is_nothrow_move_constructible_v<T> );
	}
	SECTION( "Copy and Move Traits" ) {
		CHECK( std::is_copy_assignable_v<T> );
		CHECK( std::is_move_assignable_v<T> );
		CHECK( std::is_nothrow_move_assignable_v<T> );
	}
}

SCENARIO( "Creating a RaidFleet with constructor", "[RaidFleet][Creation]" ) {
	GIVEN( "a null Fleet pointer and attraction values" ) {
		const Fleet *nullFleet = nullptr;
		double minAttraction = 5.0;
		double maxAttraction = 10.0;

		WHEN( "constructing a RaidFleet" ) {
			RaidFleet raidFleet(nullFleet, minAttraction, maxAttraction);

			THEN( "it stores the fleet pointer" ) {
				CHECK( raidFleet.GetFleet() == nullptr );
			}

			THEN( "it stores the min attraction" ) {
				CHECK( raidFleet.MinAttraction() == minAttraction );
			}

			THEN( "it stores the max attraction" ) {
				CHECK( raidFleet.MaxAttraction() == maxAttraction );
			}
		}
	}

	GIVEN( "attraction values with zero" ) {
		const Fleet *nullFleet = nullptr;

		WHEN( "constructing a RaidFleet with zero min attraction" ) {
			RaidFleet raidFleet(nullFleet, 0.0, 10.0);

			THEN( "min attraction is zero" ) {
				CHECK( raidFleet.MinAttraction() == 0.0 );
			}
		}

		WHEN( "constructing a RaidFleet with zero max attraction" ) {
			RaidFleet raidFleet(nullFleet, 5.0, 0.0);

			THEN( "max attraction is zero" ) {
				CHECK( raidFleet.MaxAttraction() == 0.0 );
			}
		}

		WHEN( "constructing a RaidFleet with both attractions zero" ) {
			RaidFleet raidFleet(nullFleet, 0.0, 0.0);

			THEN( "both attractions are zero" ) {
				CHECK( raidFleet.MinAttraction() == 0.0 );
				CHECK( raidFleet.MaxAttraction() == 0.0 );
			}
		}
	}

	GIVEN( "negative attraction values" ) {
		const Fleet *nullFleet = nullptr;

		WHEN( "constructing a RaidFleet with negative attractions" ) {
			RaidFleet raidFleet(nullFleet, -5.0, -10.0);

			THEN( "negative values are stored as-is" ) {
				CHECK( raidFleet.MinAttraction() == -5.0 );
				CHECK( raidFleet.MaxAttraction() == -10.0 );
			}
		}
	}

	GIVEN( "very large attraction values" ) {
		const Fleet *nullFleet = nullptr;
		double largeMin = 1e100;
		double largeMax = 1e200;

		WHEN( "constructing a RaidFleet with large values" ) {
			RaidFleet raidFleet(nullFleet, largeMin, largeMax);

			THEN( "large values are stored correctly" ) {
				CHECK( raidFleet.MinAttraction() == largeMin );
				CHECK( raidFleet.MaxAttraction() == largeMax );
			}
		}
	}
}

SCENARIO( "RaidFleet getter methods", "[RaidFleet][Getters]" ) {
	GIVEN( "a RaidFleet with specific values" ) {
		const Fleet *nullFleet = nullptr;
		double expectedMin = 2.5;
		double expectedMax = 7.5;
		RaidFleet raidFleet(nullFleet, expectedMin, expectedMax);

		WHEN( "calling GetFleet" ) {
			const Fleet *result = raidFleet.GetFleet();

			THEN( "it returns the fleet pointer" ) {
				CHECK( result == nullptr );
			}
		}

		WHEN( "calling MinAttraction" ) {
			double result = raidFleet.MinAttraction();

			THEN( "it returns the min attraction" ) {
				CHECK( result == expectedMin );
			}
		}

		WHEN( "calling MaxAttraction" ) {
			double result = raidFleet.MaxAttraction();

			THEN( "it returns the max attraction" ) {
				CHECK( result == expectedMax );
			}
		}
	}
}

SCENARIO( "RaidFleet copy semantics", "[RaidFleet][Copy]" ) {
	GIVEN( "a RaidFleet with specific values" ) {
		const Fleet *nullFleet = nullptr;
		RaidFleet original(nullFleet, 3.0, 8.0);

		WHEN( "copy-constructing another RaidFleet" ) {
			RaidFleet copy(original);

			THEN( "the copy has the same values" ) {
				CHECK( copy.GetFleet() == original.GetFleet() );
				CHECK( copy.MinAttraction() == original.MinAttraction() );
				CHECK( copy.MaxAttraction() == original.MaxAttraction() );
			}
		}

		WHEN( "copy-assigning to another RaidFleet" ) {
			RaidFleet assigned(nullFleet, 0.0, 0.0);
			assigned = original;

			THEN( "the assigned RaidFleet has the same values" ) {
				CHECK( assigned.GetFleet() == original.GetFleet() );
				CHECK( assigned.MinAttraction() == original.MinAttraction() );
				CHECK( assigned.MaxAttraction() == original.MaxAttraction() );
			}
		}
	}
}

SCENARIO( "RaidFleet move semantics", "[RaidFleet][Move]" ) {
	GIVEN( "a RaidFleet with specific values" ) {
		const Fleet *nullFleet = nullptr;
		double originalMin = 3.0;
		double originalMax = 8.0;
		RaidFleet original(nullFleet, originalMin, originalMax);

		WHEN( "move-constructing another RaidFleet" ) {
			RaidFleet moved(std::move(original));

			THEN( "the moved RaidFleet has the expected values" ) {
				CHECK( moved.GetFleet() == nullptr );
				CHECK( moved.MinAttraction() == originalMin );
				CHECK( moved.MaxAttraction() == originalMax );
			}
		}

		WHEN( "move-assigning to another RaidFleet" ) {
			RaidFleet assigned(nullFleet, 0.0, 0.0);
			assigned = std::move(original);

			THEN( "the assigned RaidFleet has the expected values" ) {
				CHECK( assigned.GetFleet() == nullptr );
				CHECK( assigned.MinAttraction() == originalMin );
				CHECK( assigned.MaxAttraction() == originalMax );
			}
		}
	}
}

SCENARIO( "RaidFleet in a vector", "[RaidFleet][Container]" ) {
	GIVEN( "an empty vector of RaidFleets" ) {
		std::vector<RaidFleet> raidFleets;

		WHEN( "adding RaidFleets via emplace_back" ) {
			raidFleets.emplace_back(nullptr, 1.0, 2.0);
			raidFleets.emplace_back(nullptr, 3.0, 4.0);
			raidFleets.emplace_back(nullptr, 5.0, 6.0);

			THEN( "the vector has the correct size" ) {
				CHECK( raidFleets.size() == 3 );
			}

			THEN( "each RaidFleet has the correct values" ) {
				CHECK( raidFleets[0].MinAttraction() == 1.0 );
				CHECK( raidFleets[0].MaxAttraction() == 2.0 );
				CHECK( raidFleets[1].MinAttraction() == 3.0 );
				CHECK( raidFleets[1].MaxAttraction() == 4.0 );
				CHECK( raidFleets[2].MinAttraction() == 5.0 );
				CHECK( raidFleets[2].MaxAttraction() == 6.0 );
			}
		}
	}
}

SCENARIO( "RaidFleet with default min/max attraction values", "[RaidFleet][Defaults]" ) {
	GIVEN( "the default values from Load() implementation" ) {
		// In Load(), default minAttraction is 2.0 and default maxAttraction is 0.0
		// when not specified in the DataNode

		WHEN( "creating a RaidFleet with Load defaults" ) {
			RaidFleet raidFleet(nullptr, 2.0, 0.0);

			THEN( "the default values are stored correctly" ) {
				CHECK( raidFleet.MinAttraction() == 2.0 );
				CHECK( raidFleet.MaxAttraction() == 0.0 );
			}
		}
	}
}

SCENARIO( "RaidFleet return types", "[RaidFleet][Types]" ) {
	GIVEN( "a RaidFleet" ) {
		RaidFleet raidFleet(nullptr, 1.0, 2.0);

		WHEN( "getting the fleet pointer" ) {
			auto fleet = raidFleet.GetFleet();

			THEN( "the return type is const Fleet*" ) {
				CHECK( (std::is_same_v<decltype(fleet), const Fleet*>) );
			}
		}

		WHEN( "getting min attraction" ) {
			auto minAttr = raidFleet.MinAttraction();

			THEN( "the return type is double" ) {
				CHECK( (std::is_same_v<decltype(minAttr), double>) );
			}
		}

		WHEN( "getting max attraction" ) {
			auto maxAttr = raidFleet.MaxAttraction();

			THEN( "the return type is double" ) {
				CHECK( (std::is_same_v<decltype(maxAttr), double>) );
			}
		}
	}
}

SCENARIO( "RaidFleet with equal min and max attraction", "[RaidFleet][EqualAttractions]" ) {
	GIVEN( "equal min and max attraction values" ) {
		double sameValue = 5.0;
		RaidFleet raidFleet(nullptr, sameValue, sameValue);

		WHEN( "querying the attraction values" ) {
			THEN( "both values are equal" ) {
				CHECK( raidFleet.MinAttraction() == raidFleet.MaxAttraction() );
				CHECK( raidFleet.MinAttraction() == sameValue );
			}
		}
	}
}

SCENARIO( "RaidFleet with inverted attraction values", "[RaidFleet][Inverted]" ) {
	GIVEN( "min attraction greater than max attraction" ) {
		// The class doesn't enforce any ordering constraint
		RaidFleet raidFleet(nullptr, 10.0, 5.0);

		WHEN( "querying the attraction values" ) {
			THEN( "both values are stored as-is" ) {
				CHECK( raidFleet.MinAttraction() == 10.0 );
				CHECK( raidFleet.MaxAttraction() == 5.0 );
			}
		}
	}
}

SCENARIO( "RaidFleet self-assignment", "[RaidFleet][SelfAssignment]" ) {
	GIVEN( "a RaidFleet with specific values" ) {
		RaidFleet raidFleet(nullptr, 3.0, 7.0);

		WHEN( "self-copy-assigning" ) {
			raidFleet = raidFleet;

			THEN( "the RaidFleet remains in valid state" ) {
				CHECK( raidFleet.GetFleet() == nullptr );
				CHECK( raidFleet.MinAttraction() == 3.0 );
				CHECK( raidFleet.MaxAttraction() == 7.0 );
			}
		}

		WHEN( "self-move-assigning" ) {
			raidFleet = std::move(raidFleet);

			THEN( "the RaidFleet remains in valid state" ) {
				CHECK( raidFleet.GetFleet() == nullptr );
				CHECK( raidFleet.MinAttraction() == 3.0 );
				CHECK( raidFleet.MaxAttraction() == 7.0 );
			}
		}
	}
}

SCENARIO( "RaidFleet multiple copies consistency", "[RaidFleet][CopyConsistency]" ) {
	GIVEN( "a RaidFleet with specific values" ) {
		RaidFleet original(nullptr, 4.5, 9.5);

		WHEN( "performing multiple copies" ) {
			RaidFleet firstCopy(original);
			RaidFleet secondCopy(firstCopy);

			THEN( "all copies have consistent values" ) {
				CHECK( original.MinAttraction() == 4.5 );
				CHECK( original.MaxAttraction() == 9.5 );
				CHECK( firstCopy.MinAttraction() == 4.5 );
				CHECK( firstCopy.MaxAttraction() == 9.5 );
				CHECK( secondCopy.MinAttraction() == 4.5 );
				CHECK( secondCopy.MaxAttraction() == 9.5 );
			}
		}

		WHEN( "performing copy then move" ) {
			RaidFleet copied(original);
			RaidFleet moved(std::move(copied));

			THEN( "the final RaidFleet has expected values" ) {
				CHECK( moved.MinAttraction() == 4.5 );
				CHECK( moved.MaxAttraction() == 9.5 );
			}
		}
	}
}

// Note: The static Load() method cannot be fully tested with unit tests
// because it depends on GameData::Fleets().Get() to retrieve a Fleet by name.
// The Load() method:
// - Gets a Fleet pointer from GameData using the token at valueIndex
// - If remove is true: removes all RaidFleets with matching fleet pointer
// - If remove is false: adds a new RaidFleet with default min=2.0, max=0.0
//   or specified values from the DataNode
//
// Testing Load() would require:
// 1. A properly initialized GameData instance
// 2. Registered Fleet objects in GameData
// 3. A valid DataNode with appropriate tokens
//
// These conditions are better suited for integration tests.

// #endregion unit tests



} // test namespace