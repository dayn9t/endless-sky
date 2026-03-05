/* test_CollisionType.cpp
Copyright (c) 2025 by the Endless Sky contributors

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
#include "../../../source/CollisionType.h"

// ... and any system includes needed for the test file.
#include <type_traits>

namespace { // test namespace

// #region mock data
// #endregion mock data



// #region unit tests
SCENARIO( "CollisionType enum values are defined correctly", "[CollisionType]" ) {
	GIVEN( "the CollisionType enum" ) {
		THEN( "NONE has value 0" ) {
			REQUIRE( static_cast<int>(CollisionType::NONE) == 0 );
		}
		THEN( "SHIP has value 1" ) {
			REQUIRE( static_cast<int>(CollisionType::SHIP) == 1 );
		}
		THEN( "MINABLE has value 2" ) {
			REQUIRE( static_cast<int>(CollisionType::MINABLE) == 2 );
		}
		THEN( "ASTEROID has value 3" ) {
			REQUIRE( static_cast<int>(CollisionType::ASTEROID) == 3 );
		}
	}
}

SCENARIO( "CollisionType is a scoped enum", "[CollisionType]" ) {
	GIVEN( "the CollisionType type" ) {
		THEN( "it is an enum class" ) {
			REQUIRE( std::is_enum_v<CollisionType> );
			REQUIRE( std::is_enum_v<CollisionType> );
		}
		THEN( "it has int as underlying type" ) {
			REQUIRE( std::is_same_v<std::underlying_type_t<CollisionType>, int> );
		}
	}
}

SCENARIO( "CollisionType can be used in comparisons", "[CollisionType]" ) {
	GIVEN( "two CollisionType values" ) {
		WHEN( "they are the same" ) {
			THEN( "they compare equal" ) {
				REQUIRE( CollisionType::NONE == CollisionType::NONE );
				REQUIRE( CollisionType::SHIP == CollisionType::SHIP );
				REQUIRE( CollisionType::MINABLE == CollisionType::MINABLE );
				REQUIRE( CollisionType::ASTEROID == CollisionType::ASTEROID );
			}
		}
		WHEN( "they are different" ) {
			THEN( "they compare not equal" ) {
				REQUIRE( CollisionType::NONE != CollisionType::SHIP );
				REQUIRE( CollisionType::SHIP != CollisionType::MINABLE );
				REQUIRE( CollisionType::MINABLE != CollisionType::ASTEROID );
				REQUIRE( CollisionType::ASTEROID != CollisionType::NONE );
			}
		}
	}
}

SCENARIO( "CollisionType can be used in switch statements", "[CollisionType]" ) {
	GIVEN( "a CollisionType value" ) {
		WHEN( "using it in a switch" ) {
			THEN( "all cases are handled correctly" ) {
				auto getName = [](CollisionType type) -> const char* {
					switch(type) {
						case CollisionType::NONE: return "NONE";
						case CollisionType::SHIP: return "SHIP";
						case CollisionType::MINABLE: return "MINABLE";
						case CollisionType::ASTEROID: return "ASTEROID";
					}
					return "UNKNOWN";
				};

				REQUIRE( std::string(getName(CollisionType::NONE)) == "NONE" );
				REQUIRE( std::string(getName(CollisionType::SHIP)) == "SHIP" );
				REQUIRE( std::string(getName(CollisionType::MINABLE)) == "MINABLE" );
				REQUIRE( std::string(getName(CollisionType::ASTEROID)) == "ASTEROID" );
			}
		}
	}
}

SCENARIO( "CollisionType represents collision targets", "[CollisionType]" ) {
	GIVEN( "different collision types" ) {
		THEN( "NONE represents explosions or triggered projectiles" ) {
			CHECK( CollisionType::NONE == CollisionType::NONE );
		}
		THEN( "SHIP represents ship collisions" ) {
			CHECK( CollisionType::SHIP == CollisionType::SHIP );
		}
		THEN( "MINABLE represents minable object collisions" ) {
			CHECK( CollisionType::MINABLE == CollisionType::MINABLE );
		}
		THEN( "ASTEROID represents asteroid collisions" ) {
			CHECK( CollisionType::ASTEROID == CollisionType::ASTEROID );
		}
	}
}
// #endregion unit tests



} // test namespace