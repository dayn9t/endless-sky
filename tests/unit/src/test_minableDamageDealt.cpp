/* test_minableDamageDealt.cpp
Copyright (c) 2026 by jiang

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
#include "../../../source/MinableDamageDealt.h"

// ... and any system includes needed for the test file.
#include <type_traits>

namespace { // test namespace

// #region mock data

// Insert file-local data here, e.g. classes, structs, or fixtures that will be useful
// to help test this class/method.

// #endregion mock data



// #region unit tests
SCENARIO( "MinableDamageDealt class traits", "[MinableDamageDealt]" ) {
	GIVEN( "the MinableDamageDealt class" ) {
		THEN( "it has the expected construction traits" ) {
			// MinableDamageDealt is an aggregate type with default member initializers.
			CHECK( std::is_default_constructible_v<MinableDamageDealt> );
			CHECK( std::is_trivially_default_constructible_v<MinableDamageDealt> );
			CHECK( std::is_nothrow_default_constructible_v<MinableDamageDealt> );
		}
		THEN( "it has the expected destructor traits" ) {
			CHECK( std::is_destructible_v<MinableDamageDealt> );
			CHECK( std::is_trivially_destructible_v<MinableDamageDealt> );
			CHECK( std::is_nothrow_destructible_v<MinableDamageDealt> );
		}
		THEN( "it has the expected copy traits" ) {
			CHECK( std::is_copy_constructible_v<MinableDamageDealt> );
			CHECK( std::is_trivially_copy_constructible_v<MinableDamageDealt> );
			CHECK( std::is_copy_assignable_v<MinableDamageDealt> );
			CHECK( std::is_trivially_copy_assignable_v<MinableDamageDealt> );
		}
		THEN( "it has the expected move traits" ) {
			CHECK( std::is_move_constructible_v<MinableDamageDealt> );
			CHECK( std::is_trivially_move_constructible_v<MinableDamageDealt> );
			CHECK( std::is_move_assignable_v<MinableDamageDealt> );
			CHECK( std::is_trivially_move_assignable_v<MinableDamageDealt> );
		}
		THEN( "it is a trivial type" ) {
			CHECK( std::is_trivial_v<MinableDamageDealt> );
		}
	}
}

SCENARIO( "MinableDamageDealt default construction", "[MinableDamageDealt]" ) {
	GIVEN( "no parameters" ) {
		WHEN( "a MinableDamageDealt is default constructed" ) {
			MinableDamageDealt damage;

			THEN( "hullDamage is initialized to 0.0" ) {
				CHECK_THAT( damage.hullDamage, Catch::Matchers::WithinAbs(0.0, 0.0001) );
			}
			AND_THEN( "prospecting is initialized to 0.0" ) {
				CHECK_THAT( damage.prospecting, Catch::Matchers::WithinAbs(0.0, 0.0001) );
			}
		}
	}
}

SCENARIO( "MinableDamageDealt aggregate initialization", "[MinableDamageDealt]" ) {
	GIVEN( "values for hullDamage and prospecting" ) {
		WHEN( "MinableDamageDealt is aggregate initialized with both values" ) {
			MinableDamageDealt damage{100.0, 50.0};

			THEN( "hullDamage is set to the provided value" ) {
				CHECK_THAT( damage.hullDamage, Catch::Matchers::WithinAbs(100.0, 0.0001) );
			}
			AND_THEN( "prospecting is set to the provided value" ) {
				CHECK_THAT( damage.prospecting, Catch::Matchers::WithinAbs(50.0, 0.0001) );
			}
		}

		WHEN( "MinableDamageDealt is partially initialized (hullDamage only)" ) {
			MinableDamageDealt damage{250.0};

			THEN( "hullDamage is set to the provided value" ) {
				CHECK_THAT( damage.hullDamage, Catch::Matchers::WithinAbs(250.0, 0.0001) );
			}
			AND_THEN( "prospecting remains at default 0.0" ) {
				CHECK_THAT( damage.prospecting, Catch::Matchers::WithinAbs(0.0, 0.0001) );
			}
		}
	}
}

SCENARIO( "MinableDamageDealt member modification", "[MinableDamageDealt]" ) {
	GIVEN( "a default constructed MinableDamageDealt" ) {
		MinableDamageDealt damage;

		WHEN( "hullDamage is modified" ) {
			damage.hullDamage = 500.0;

			THEN( "hullDamage has the new value" ) {
				CHECK_THAT( damage.hullDamage, Catch::Matchers::WithinAbs(500.0, 0.0001) );
			}
			AND_THEN( "prospecting is unchanged" ) {
				CHECK_THAT( damage.prospecting, Catch::Matchers::WithinAbs(0.0, 0.0001) );
			}
		}

		WHEN( "prospecting is modified" ) {
			damage.prospecting = 75.5;

			THEN( "prospecting has the new value" ) {
				CHECK_THAT( damage.prospecting, Catch::Matchers::WithinAbs(75.5, 0.0001) );
			}
			AND_THEN( "hullDamage is unchanged" ) {
				CHECK_THAT( damage.hullDamage, Catch::Matchers::WithinAbs(0.0, 0.0001) );
			}
		}

		WHEN( "both members are modified" ) {
			damage.hullDamage = 1000.0;
			damage.prospecting = 100.0;

			THEN( "both values are updated" ) {
				CHECK_THAT( damage.hullDamage, Catch::Matchers::WithinAbs(1000.0, 0.0001) );
				CHECK_THAT( damage.prospecting, Catch::Matchers::WithinAbs(100.0, 0.0001) );
			}
		}
	}
}

SCENARIO( "MinableDamageDealt copy and assignment", "[MinableDamageDealt]" ) {
	GIVEN( "a MinableDamageDealt with set values" ) {
		MinableDamageDealt original{300.0, 150.0};

		WHEN( "copied via copy constructor" ) {
			MinableDamageDealt copy(original);

			THEN( "copy has the same values" ) {
				CHECK_THAT( copy.hullDamage, Catch::Matchers::WithinAbs(300.0, 0.0001) );
				CHECK_THAT( copy.prospecting, Catch::Matchers::WithinAbs(150.0, 0.0001) );
			}
		}

		WHEN( "assigned via copy assignment" ) {
			MinableDamageDealt assigned;
			assigned = original;

			THEN( "assigned has the same values" ) {
				CHECK_THAT( assigned.hullDamage, Catch::Matchers::WithinAbs(300.0, 0.0001) );
				CHECK_THAT( assigned.prospecting, Catch::Matchers::WithinAbs(150.0, 0.0001) );
			}
		}
	}
}

SCENARIO( "MinableDamageDealt use cases", "[MinableDamageDealt]" ) {
	GIVEN( "a MinableDamageDealt representing damage from a mining weapon" ) {
		MinableDamageDealt damage{250.0, 100.0};

		THEN( "hullDamage represents direct hull damage" ) {
			CHECK_THAT( damage.hullDamage, Catch::Matchers::WithinAbs(250.0, 0.0001) );
		}
		AND_THEN( "prospecting represents bonus to drop rates" ) {
			CHECK_THAT( damage.prospecting, Catch::Matchers::WithinAbs(100.0, 0.0001) );
		}
	}

	GIVEN( "a MinableDamageDealt with only hull damage" ) {
		MinableDamageDealt damage{500.0, 0.0};

		THEN( "it represents damage without prospecting bonus" ) {
			CHECK_THAT( damage.hullDamage, Catch::Matchers::WithinAbs(500.0, 0.0001) );
			CHECK_THAT( damage.prospecting, Catch::Matchers::WithinAbs(0.0, 0.0001) );
		}
	}

	GIVEN( "a MinableDamageDealt with only prospecting" ) {
		MinableDamageDealt damage{0.0, 50.0};

		THEN( "it represents prospecting without hull damage" ) {
			CHECK_THAT( damage.hullDamage, Catch::Matchers::WithinAbs(0.0, 0.0001) );
			CHECK_THAT( damage.prospecting, Catch::Matchers::WithinAbs(50.0, 0.0001) );
		}
	}

	GIVEN( "accumulated damage values" ) {
		MinableDamageDealt total{0.0, 0.0};
		MinableDamageDealt hit1{100.0, 25.0};
		MinableDamageDealt hit2{150.0, 30.0};

		WHEN( "damage values are accumulated" ) {
			total.hullDamage = hit1.hullDamage + hit2.hullDamage;
			total.prospecting = hit1.prospecting + hit2.prospecting;

			THEN( "total reflects the sum" ) {
				CHECK_THAT( total.hullDamage, Catch::Matchers::WithinAbs(250.0, 0.0001) );
				CHECK_THAT( total.prospecting, Catch::Matchers::WithinAbs(55.0, 0.0001) );
			}
		}
	}
}

// #endregion unit tests



} // test namespace