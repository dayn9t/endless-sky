/* test_politics.cpp
Copyright (c) 2025 by the Endless Sky community

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
#include "../../../source/Politics.h"

// ... and any system includes needed for the test file.
#include <map>
#include <set>
#include <string>
#include <type_traits>
#include <utility>

namespace { // test namespace

// #region mock data

// Simple mock Government for testing purposes.
// This provides a minimal implementation for tests that need Government pointers.
class MockGovernment {
public:
	MockGovernment() = default;
	// Non-copyable, non-movable to ensure pointer stability.
	MockGovernment(const MockGovernment&) = delete;
	MockGovernment& operator=(const MockGovernment&) = delete;
};

// Simple mock Planet for testing purposes.
class MockPlanet {
public:
	MockPlanet() = default;
	MockPlanet(const MockPlanet&) = delete;
	MockPlanet& operator=(const MockPlanet&) = delete;
};

// #endregion mock data



// #region unit tests
TEST_CASE( "Politics Class Traits", "[Politics]" ) {
	using T = Politics;
	SECTION( "Class Traits" ) {
		CHECK_FALSE( std::is_trivial_v<T> );
		CHECK_FALSE( std::is_standard_layout_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
	}
	SECTION( "Construction Traits" ) {
		CHECK( std::is_default_constructible_v<T> );
		CHECK_FALSE( std::is_trivially_default_constructible_v<T> );
		CHECK( std::is_copy_constructible_v<T> );
		CHECK( std::is_move_constructible_v<T> );
	}
	SECTION( "Copy Traits" ) {
		CHECK( std::is_copy_assignable_v<T> );
		CHECK( std::is_move_assignable_v<T> );
	}
}

SCENARIO( "Creating a Politics with default constructor", "[Politics]" ) {
	GIVEN( "A default-constructed Politics" ) {
		Politics politics;
		THEN( "the object is in a valid default state" ) {
			// A default-constructed Politics has empty internal containers.
			// Reputation for unknown governments returns 0.
			CHECK( politics.Reputation(nullptr) == 0. );
		}
	}
}

SCENARIO( "Politics IsEnemy with null pointers", "[Politics][IsEnemy]" ) {
	GIVEN( "A Politics instance" ) {
		Politics politics;
		WHEN( "checking if nullptr governments are enemies" ) {
			THEN( "two null governments are not enemies" ) {
				CHECK_FALSE( politics.IsEnemy(nullptr, nullptr) );
			}
		}
		// Note: Testing with one null and one non-null would require a valid Government
		// object, which we cannot create without full GameData initialization.
		// The IsEnemy implementation returns false if either pointer is null.
	}
}

SCENARIO( "Politics IsEnemy with same null government", "[Politics][IsEnemy]" ) {
	GIVEN( "A Politics instance" ) {
		Politics politics;
		WHEN( "checking if null government is its own enemy" ) {
			THEN( "null is not considered an enemy of itself" ) {
				// IsEnemy returns false if either pointer is null
				CHECK_FALSE( politics.IsEnemy(nullptr, nullptr) );
			}
		}
	}
}

SCENARIO( "Politics Reputation with null pointer", "[Politics][Reputation]" ) {
	GIVEN( "A Politics instance" ) {
		Politics politics;
		WHEN( "getting reputation for null government" ) {
			THEN( "reputation is 0" ) {
				CHECK( politics.Reputation(nullptr) == 0. );
			}
		}
	}
}

SCENARIO( "Politics HasDominated with null pointer", "[Politics][HasDominated]" ) {
	GIVEN( "A Politics instance" ) {
		Politics politics;
		WHEN( "checking if null planet is dominated" ) {
			THEN( "it is not dominated" ) {
				CHECK_FALSE( politics.HasDominated(nullptr) );
			}
		}
	}
}

SCENARIO( "Politics DominatePlanet operations", "[Politics][DominatePlanet]" ) {
	GIVEN( "A Politics instance and mock planets" ) {
		Politics politics;
		MockPlanet mockPlanet1;
		MockPlanet mockPlanet2;
		const Planet* planet1 = reinterpret_cast<const Planet*>(&mockPlanet1);
		const Planet* planet2 = reinterpret_cast<const Planet*>(&mockPlanet2);

		WHEN( "dominating a planet" ) {
			politics.DominatePlanet(planet1, true);
			THEN( "the planet is dominated" ) {
				CHECK( politics.HasDominated(planet1) );
			}
			THEN( "other planets are not dominated" ) {
				CHECK_FALSE( politics.HasDominated(planet2) );
			}
		}

		WHEN( "dominating and then releasing a planet" ) {
			politics.DominatePlanet(planet1, true);
			politics.DominatePlanet(planet1, false);
			THEN( "the planet is no longer dominated" ) {
				CHECK_FALSE( politics.HasDominated(planet1) );
			}
		}

		WHEN( "dominating multiple planets" ) {
			politics.DominatePlanet(planet1, true);
			politics.DominatePlanet(planet2, true);
			THEN( "both planets are dominated" ) {
				CHECK( politics.HasDominated(planet1) );
				CHECK( politics.HasDominated(planet2) );
			}
		}
	}
}

SCENARIO( "Politics HasClearance with null pointer", "[Politics][HasClearance]" ) {
	GIVEN( "A Politics instance" ) {
		Politics politics;
		WHEN( "checking clearance for null planet" ) {
			THEN( "no clearance is granted" ) {
				CHECK_FALSE( politics.HasClearance(nullptr) );
			}
		}
	}
}

SCENARIO( "Politics BribePlanet operations", "[Politics][BribePlanet]" ) {
	GIVEN( "A Politics instance and mock planets" ) {
		Politics politics;
		MockPlanet mockPlanet;
		const Planet* planet = reinterpret_cast<const Planet*>(&mockPlanet);

		WHEN( "bribing a planet with full access" ) {
			politics.BribePlanet(planet, true);
			THEN( "the planet has clearance" ) {
				CHECK( politics.HasClearance(planet) );
			}
		}

		WHEN( "bribing a planet without full access" ) {
			politics.BribePlanet(planet, false);
			THEN( "the planet still has clearance" ) {
				CHECK( politics.HasClearance(planet) );
			}
		}
	}
}

SCENARIO( "Politics Bribe government operations", "[Politics][Bribe]" ) {
	GIVEN( "A Politics instance" ) {
		Politics politics;

		WHEN( "bribing a null government" ) {
			// This should not crash
			politics.Bribe(nullptr);
			THEN( "the operation completes without error" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "Politics ResetDaily clears temporary effects", "[Politics][ResetDaily]" ) {
	GIVEN( "A Politics instance with a bribed planet" ) {
		Politics politics;
		MockPlanet mockPlanet;
		const Planet* planet = reinterpret_cast<const Planet*>(&mockPlanet);

		// Set up some temporary states (only planet-based, as Government needs valid object)
		politics.BribePlanet(planet, true);
		REQUIRE( politics.HasClearance(planet) );

		WHEN( "ResetDaily is called" ) {
			politics.ResetDaily();
			THEN( "bribed planets are cleared" ) {
				CHECK_FALSE( politics.HasClearance(planet) );
			}
		}
	}
}

SCENARIO( "Politics CanLand with null planet", "[Politics][CanLand]" ) {
	GIVEN( "A Politics instance" ) {
		Politics politics;
		WHEN( "checking if player can land on null planet" ) {
			THEN( "cannot land" ) {
				CHECK_FALSE( politics.CanLand(nullptr) );
			}
		}
	}
}

SCENARIO( "Politics CanUseServices with null planet", "[Politics][CanUseServices]" ) {
	GIVEN( "A Politics instance" ) {
		Politics politics;
		WHEN( "checking if player can use services on null planet" ) {
			THEN( "cannot use services" ) {
				CHECK_FALSE( politics.CanUseServices(nullptr) );
			}
		}
	}
}

SCENARIO( "Politics DominatePlanet default parameter", "[Politics][DominatePlanet]" ) {
	GIVEN( "A Politics instance and a mock planet" ) {
		Politics politics;
		MockPlanet mockPlanet;
		const Planet* planet = reinterpret_cast<const Planet*>(&mockPlanet);

		WHEN( "calling DominatePlanet without second parameter" ) {
			politics.DominatePlanet(planet);
			THEN( "the planet is dominated (default is true)" ) {
				CHECK( politics.HasDominated(planet) );
			}
		}
	}
}

SCENARIO( "Politics dominated planet grants clearance", "[Politics][HasClearance]" ) {
	GIVEN( "A Politics instance and a mock planet" ) {
		Politics politics;
		MockPlanet mockPlanet;
		const Planet* planet = reinterpret_cast<const Planet*>(&mockPlanet);

		WHEN( "a planet is dominated" ) {
			politics.DominatePlanet(planet, true);
			THEN( "the planet has clearance" ) {
				CHECK( politics.HasClearance(planet) );
			}
		}

		WHEN( "a planet is dominated and then released" ) {
			politics.DominatePlanet(planet, true);
			politics.DominatePlanet(planet, false);
			THEN( "the planet no longer has clearance" ) {
				CHECK_FALSE( politics.HasClearance(planet) );
			}
		}
	}
}

SCENARIO( "Politics Offend with null government", "[Politics][Offend]" ) {
	GIVEN( "A Politics instance" ) {
		Politics politics;
		WHEN( "offending with null government" ) {
			// This should not crash and should have no effect
			politics.Offend(nullptr, 0, 1);
			THEN( "no exception is thrown and state is unchanged" ) {
				// If we get here, the null check worked
				CHECK( true );
			}
		}
	}
}

SCENARIO( "Politics copy semantics", "[Politics][Copy]" ) {
	GIVEN( "A default-constructed Politics" ) {
		Politics original;

		WHEN( "copy-constructing a new Politics" ) {
			Politics copy(original);
			THEN( "the copy is in a valid state" ) {
				CHECK( copy.Reputation(nullptr) == 0. );
			}
		}

		WHEN( "copy-assigning to a new Politics" ) {
			Politics assigned;
			assigned = original;
			THEN( "the assigned copy is in a valid state" ) {
				CHECK( assigned.Reputation(nullptr) == 0. );
			}
		}
	}
}

SCENARIO( "Politics move semantics", "[Politics][Move]" ) {
	GIVEN( "A default-constructed Politics" ) {
		Politics original;

		WHEN( "move-constructing a new Politics" ) {
			Politics moved(std::move(original));
			THEN( "the moved-to object is in a valid state" ) {
				CHECK( moved.Reputation(nullptr) == 0. );
			}
		}

		WHEN( "move-assigning to a new Politics" ) {
			Politics assigned;
			assigned = std::move(original);
			THEN( "the assigned object is in a valid state" ) {
				CHECK( assigned.Reputation(nullptr) == 0. );
			}
		}
	}
}

// #endregion unit tests



} // test namespace