/* test_person.cpp
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
#include "../../../source/Person.h"

// ... and any system includes needed for the test file.
#include <list>
#include <memory>
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

TEST_CASE( "Person Class Traits", "[Person]" ) {
	using T = Person;
	SECTION( "Class Layout" ) {
		CHECK( std::is_default_constructible_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
		CHECK( std::is_copy_constructible_v<T> );
		CHECK( std::is_copy_assignable_v<T> );
		CHECK( std::is_move_constructible_v<T> );
		CHECK( std::is_move_assignable_v<T> );
	}
}

SCENARIO( "Creating a Person with default constructor", "[Person][Creation]" ) {
	GIVEN( "a default-constructed Person" ) {
		Person person;

		THEN( "it is not loaded" ) {
			CHECK_FALSE( person.IsLoaded() );
		}
		THEN( "it has no ships" ) {
			CHECK( person.Ships().empty() );
		}
		THEN( "it has no government" ) {
			CHECK( person.GetGovernment() == nullptr );
		}
		THEN( "it is considered destroyed (no flagship)" ) {
			CHECK( person.IsDestroyed() );
		}
		THEN( "it is not placed anywhere" ) {
			CHECK_FALSE( person.IsPlaced() );
		}
	}
}

SCENARIO( "Preventing a Person from spawning", "[Person][NeverSpawn]" ) {
	GIVEN( "a Person" ) {
		Person person;

		WHEN( "NeverSpawn is called" ) {
			person.NeverSpawn();

			THEN( "Frequency returns 0 for any system" ) {
				CHECK( person.Frequency(nullptr) == 0 );
			}
		}
	}
}

SCENARIO( "Checking Person destruction state", "[Person][IsDestroyed]" ) {
	GIVEN( "a Person with no ships" ) {
		Person person;

		WHEN( "checking if destroyed" ) {
			THEN( "it is considered destroyed" ) {
				CHECK( person.IsDestroyed() );
			}
		}
	}
}

SCENARIO( "Checking Person placement state", "[Person][IsPlaced]" ) {
	GIVEN( "a Person with no ships" ) {
		Person person;

		WHEN( "checking if placed" ) {
			THEN( "it is not placed" ) {
				CHECK_FALSE( person.IsPlaced() );
			}
		}
	}
}

SCENARIO( "Managing Person destruction", "[Person][Destroy][Restore]" ) {
	GIVEN( "a Person with no ships" ) {
		Person person;

		WHEN( "Destroy is called" ) {
			person.Destroy();

			THEN( "the person is still considered destroyed" ) {
				CHECK( person.IsDestroyed() );
			}
		}

		WHEN( "Restore is called" ) {
			person.Restore();

			THEN( "the person is still considered destroyed (no ships)" ) {
				CHECK( person.IsDestroyed() );
			}
		}
	}
}

SCENARIO( "Clearing Person placement", "[Person][ClearPlacement]" ) {
	GIVEN( "a Person with no ships" ) {
		Person person;

		WHEN( "ClearPlacement is called" ) {
			person.ClearPlacement();

			THEN( "the person is still not placed" ) {
				CHECK_FALSE( person.IsPlaced() );
			}
		}
	}
}

SCENARIO( "Getting Person characteristics", "[Person][Accessors]" ) {
	GIVEN( "a default-constructed Person" ) {
		Person person;

		THEN( "Ships returns an empty list" ) {
			const auto &ships = person.Ships();
			CHECK( ships.empty() );
		}

		THEN( "GetGovernment returns nullptr" ) {
			CHECK( person.GetGovernment() == nullptr );
		}

		THEN( "GetPersonality returns a default Personality" ) {
			// A default Personality should not be defined
			CHECK_FALSE( person.GetPersonality().IsDefined() );
		}

		THEN( "GetHail returns an empty Phrase" ) {
			CHECK( person.GetHail().IsEmpty() );
		}
	}
}

SCENARIO( "Frequency calculation with different conditions", "[Person][Frequency]" ) {
	GIVEN( "a Person with default frequency" ) {
		Person person;

		WHEN( "checking frequency with null system" ) {
			THEN( "frequency is 0" ) {
				CHECK( person.Frequency(nullptr) == 0 );
			}
		}
	}

	GIVEN( "a Person that should never spawn" ) {
		Person person;
		person.NeverSpawn();

		WHEN( "checking frequency" ) {
			THEN( "frequency is always 0" ) {
				CHECK( person.Frequency(nullptr) == 0 );
			}
		}
	}
}

// Note: Tests involving Load, FinishLoading, and Ship-related operations
// require GameData and other runtime dependencies, which are not available
// in this unit test environment. Those scenarios would be better suited
// for integration tests or would require mocking infrastructure.

// #endregion unit tests



} // test namespace