/* test_minable.cpp
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
#include "../../../source/Minable.h"

// ... and any system includes needed for the test file.
#include "../../../source/MinableDamageDealt.h"
#include "../../../source/Point.h"

#include <cmath>
#include <type_traits>
#include <vector>

namespace { // test namespace

// #region mock data

// Insert file-local data here, e.g. classes, structs, or fixtures that will be useful
// to help test this class/method.

// #endregion mock data



// #region unit tests

SCENARIO( "Minable class traits", "[Minable]" ) {
	GIVEN( "the Minable class" ) {
		THEN( "it has the expected construction traits" ) {
			CHECK( std::is_default_constructible_v<Minable> );
			CHECK_FALSE( std::is_trivially_default_constructible_v<Minable> );
			CHECK( std::is_nothrow_destructible_v<Minable> );
			CHECK_FALSE( std::is_trivially_destructible_v<Minable> );
		}
		THEN( "it has the expected copy traits (via Entity base)" ) {
			CHECK( std::is_copy_constructible_v<Minable> );
			CHECK( std::is_copy_assignable_v<Minable> );
		}
		THEN( "it has the expected move traits (via Entity base)" ) {
			CHECK( std::is_move_constructible_v<Minable> );
			CHECK( std::is_move_assignable_v<Minable> );
		}
	}
}

SCENARIO( "Minable default construction", "[Minable]" ) {
	GIVEN( "no parameters" ) {
		WHEN( "a Minable is default constructed" ) {
			Minable minable;
			THEN( "hull is at maximum (full health)" ) {
				CHECK_THAT( minable.Hull(), Catch::Matchers::WithinAbs(1., 0.0001) );
			}
			AND_THEN( "max hull is 1000 (default value)" ) {
				CHECK_THAT( minable.MaxHull(), Catch::Matchers::WithinAbs(1000., 0.0001) );
			}
			AND_THEN( "mass is 0 (default Outfit has no mass)" ) {
				CHECK_THAT( minable.Mass(), Catch::Matchers::WithinAbs(0., 0.0001) );
			}
			AND_THEN( "value is 0" ) {
				CHECK( minable.GetValue() == 0 );
			}
			AND_THEN( "payload is empty" ) {
				CHECK( minable.GetPayload().empty() );
			}
			AND_THEN( "true name is empty" ) {
				CHECK( minable.TrueName().empty() );
			}
			AND_THEN( "display name is empty" ) {
				CHECK( minable.DisplayName().empty() );
			}
			AND_THEN( "noun is empty" ) {
				CHECK( minable.Noun().empty() );
			}
		}
	}
}

SCENARIO( "Minable Hull getter", "[Minable][Hull]" ) {
	GIVEN( "a default constructed Minable" ) {
		Minable minable;
		WHEN( "Hull() is called" ) {
			THEN( "it returns 1.0 (full hull as a fraction)" ) {
				CHECK_THAT( minable.Hull(), Catch::Matchers::WithinAbs(1., 0.0001) );
			}
		}
	}
}

SCENARIO( "Minable MaxHull getter", "[Minable][MaxHull]" ) {
	GIVEN( "a default constructed Minable" ) {
		Minable minable;
		WHEN( "MaxHull() is called" ) {
			THEN( "it returns 1000. (default value)" ) {
				CHECK_THAT( minable.MaxHull(), Catch::Matchers::WithinAbs(1000., 0.0001) );
			}
		}
	}
}

SCENARIO( "Minable Mass getter", "[Minable][Mass]" ) {
	GIVEN( "a default constructed Minable" ) {
		Minable minable;
		WHEN( "Mass() is called" ) {
			THEN( "it returns 0. (default Outfit has no mass)" ) {
				CHECK_THAT( minable.Mass(), Catch::Matchers::WithinAbs(0., 0.0001) );
			}
		}
	}
}

SCENARIO( "Minable MaximumHeat getter", "[Minable][MaximumHeat]" ) {
	GIVEN( "a default constructed Minable" ) {
		Minable minable;
		WHEN( "MaximumHeat() is called" ) {
			THEN( "it returns MAXIMUM_TEMPERATURE * mass (100. * 0. = 0.)" ) {
				// MAXIMUM_TEMPERATURE = 100., mass = 0, heat capacity = 0
				CHECK_THAT( minable.MaximumHeat(), Catch::Matchers::WithinAbs(0., 0.0001) );
			}
		}
	}
}

SCENARIO( "Minable TrueName getter", "[Minable][TrueName]" ) {
	GIVEN( "a default constructed Minable" ) {
		Minable minable;
		WHEN( "TrueName() is called" ) {
			const std::string& name = minable.TrueName();
			THEN( "it returns an empty string" ) {
				CHECK( name.empty() );
			}
		}
	}
}

SCENARIO( "Minable DisplayName getter", "[Minable][DisplayName]" ) {
	GIVEN( "a default constructed Minable" ) {
		Minable minable;
		WHEN( "DisplayName() is called" ) {
			const std::string& displayName = minable.DisplayName();
			THEN( "it returns an empty string" ) {
				CHECK( displayName.empty() );
			}
		}
	}
}

SCENARIO( "Minable Noun getter", "[Minable][Noun]" ) {
	GIVEN( "a default constructed Minable" ) {
		Minable minable;
		WHEN( "Noun() is called" ) {
			const std::string& noun = minable.Noun();
			THEN( "it returns an empty string" ) {
				CHECK( noun.empty() );
			}
		}
	}
}

SCENARIO( "Minable GetPayload getter", "[Minable][GetPayload]" ) {
	GIVEN( "a default constructed Minable" ) {
		Minable minable;
		WHEN( "GetPayload() is called" ) {
			const std::vector<Minable::Payload>& payload = minable.GetPayload();
			THEN( "it returns an empty vector" ) {
				CHECK( payload.empty() );
			}
		}
	}
}

SCENARIO( "Minable GetValue getter", "[Minable][GetValue]" ) {
	GIVEN( "a default constructed Minable" ) {
		Minable minable;
		WHEN( "GetValue() is called" ) {
			const int64_t& value = minable.GetValue();
			THEN( "it returns 0" ) {
				CHECK( value == 0 );
			}
		}
	}
}

SCENARIO( "Minable TakeDamage method", "[Minable][TakeDamage]" ) {
	GIVEN( "a default constructed Minable" ) {
		Minable minable;
		REQUIRE_THAT( minable.Hull(), Catch::Matchers::WithinAbs(1., 0.0001) );
		REQUIRE_THAT( minable.MaxHull(), Catch::Matchers::WithinAbs(1000., 0.0001) );

		WHEN( "TakeDamage is called with hull damage" ) {
			MinableDamageDealt damage;
			damage.hullDamage = 250.;
			damage.prospecting = 0.;
			minable.TakeDamage(damage);

			THEN( "hull is reduced" ) {
				// Hull() returns hull / maxHull as fraction
				// hull = 1000 - 250 = 750, so Hull() = 750/1000 = 0.75
				CHECK_THAT( minable.Hull(), Catch::Matchers::WithinAbs(0.75, 0.0001) );
			}
			AND_THEN( "MaxHull remains unchanged" ) {
				CHECK_THAT( minable.MaxHull(), Catch::Matchers::WithinAbs(1000., 0.0001) );
			}
		}

		WHEN( "TakeDamage is called with enough damage to destroy the minable" ) {
			MinableDamageDealt damage;
			damage.hullDamage = 1001.;
			damage.prospecting = 0.;
			minable.TakeDamage(damage);

			THEN( "hull becomes negative" ) {
				// hull = 1000 - 1001 = -1
				// Hull() returns min(1., hull / maxHull) = min(1., -1/1000) = min(1., -0.001)
				// This will be -0.001
				CHECK_THAT( minable.Hull(), Catch::Matchers::WithinAbs(-0.001, 0.0001) );
			}
		}

		WHEN( "TakeDamage is called multiple times" ) {
			MinableDamageDealt damage1;
			damage1.hullDamage = 200.;
			damage1.prospecting = 0.;
			minable.TakeDamage(damage1);

			MinableDamageDealt damage2;
			damage2.hullDamage = 300.;
			damage2.prospecting = 0.;
			minable.TakeDamage(damage2);

			THEN( "damage accumulates" ) {
				// hull = 1000 - 200 - 300 = 500
				// Hull() = 500/1000 = 0.5
				CHECK_THAT( minable.Hull(), Catch::Matchers::WithinAbs(0.5, 0.0001) );
			}
		}

		WHEN( "TakeDamage is called with zero damage" ) {
			MinableDamageDealt damage;
			damage.hullDamage = 0.;
			damage.prospecting = 0.;
			minable.TakeDamage(damage);

			THEN( "hull remains unchanged" ) {
				CHECK_THAT( minable.Hull(), Catch::Matchers::WithinAbs(1., 0.0001) );
			}
		}

		WHEN( "TakeDamage is called with negative damage (healing)" ) {
			MinableDamageDealt damage;
			damage.hullDamage = -100.;
			damage.prospecting = 0.;
			minable.TakeDamage(damage);

			THEN( "hull increases beyond max" ) {
				// hull = 1000 - (-100) = 1100
				// Hull() = min(1., 1100/1000) = min(1., 1.1) = 1.0
				CHECK_THAT( minable.Hull(), Catch::Matchers::WithinAbs(1., 0.0001) );
			}
		}
	}
}

SCENARIO( "Minable TakeDamage with prospecting", "[Minable][TakeDamage][Prospecting]" ) {
	GIVEN( "a default constructed Minable" ) {
		Minable minable;
		REQUIRE_THAT( minable.Hull(), Catch::Matchers::WithinAbs(1., 0.0001) );

		WHEN( "TakeDamage is called with prospecting value" ) {
			MinableDamageDealt damage;
			damage.hullDamage = 100.;
			damage.prospecting = 50.;
			minable.TakeDamage(damage);

			THEN( "hull is reduced correctly" ) {
				// hull = 1000 - 100 = 900
				// Hull() = 900/1000 = 0.9
				CHECK_THAT( minable.Hull(), Catch::Matchers::WithinAbs(0.9, 0.0001) );
			}
			// Note: prospecting affects the drop rate in Move() when destroyed,
			// but we can't test that directly without GameData.
		}

		WHEN( "TakeDamage is called multiple times with prospecting" ) {
			MinableDamageDealt damage1;
			damage1.hullDamage = 100.;
			damage1.prospecting = 25.;
			minable.TakeDamage(damage1);

			MinableDamageDealt damage2;
			damage2.hullDamage = 100.;
			damage2.prospecting = 25.;
			minable.TakeDamage(damage2);

			THEN( "damage and prospecting accumulate" ) {
				// hull = 1000 - 100 - 100 = 800
				// Hull() = 800/1000 = 0.8
				CHECK_THAT( minable.Hull(), Catch::Matchers::WithinAbs(0.8, 0.0001) );
			}
		}
	}
}

SCENARIO( "Minable Payload class traits", "[Minable][Payload]" ) {
	GIVEN( "the Minable::Payload class" ) {
		THEN( "it has the expected construction traits" ) {
			// Payload has no default constructor (requires DataNode)
			CHECK_FALSE( std::is_default_constructible_v<Minable::Payload> );
			CHECK( std::is_copy_constructible_v<Minable::Payload> );
			CHECK( std::is_copy_assignable_v<Minable::Payload> );
			CHECK( std::is_move_constructible_v<Minable::Payload> );
			CHECK( std::is_move_assignable_v<Minable::Payload> );
		}
	}
}

// Note: The following methods cannot be tested without GameData dependencies:
// - Load(): Requires DataNode with GameData::Outfits(), GameData::Effects(), SpriteSet
// - FinishLoading(): Requires payload populated by Load()
// - Place(): Uses Random module and requires sprite data
// - Move(): Complex dependencies on Visual, Flotsam, Effect, and Random

// #endregion unit tests



} // test namespace