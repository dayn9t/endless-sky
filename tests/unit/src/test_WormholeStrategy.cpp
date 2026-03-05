/* test_WormholeStrategy.cpp
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
#include "../../../source/WormholeStrategy.h"

// ... and any system includes needed for the test file.
#include <type_traits>
#include <cstdint>

namespace { // test namespace

// #region mock data
// #endregion mock data



// #region unit tests
SCENARIO( "WormholeStrategy enum values are defined correctly", "[WormholeStrategy]" ) {
	GIVEN( "the WormholeStrategy enum" ) {
		THEN( "NONE has value 0" ) {
			REQUIRE( static_cast<int>(WormholeStrategy::NONE) == 0 );
		}
		THEN( "ONLY_UNRESTRICTED has value 1" ) {
			REQUIRE( static_cast<int>(WormholeStrategy::ONLY_UNRESTRICTED) == 1 );
		}
		THEN( "ALL has value 2" ) {
			REQUIRE( static_cast<int>(WormholeStrategy::ALL) == 2 );
		}
	}
}

SCENARIO( "WormholeStrategy is a scoped enum", "[WormholeStrategy]" ) {
	GIVEN( "the WormholeStrategy type" ) {
		THEN( "it is an enum class" ) {
			REQUIRE( std::is_enum_v<WormholeStrategy> );
			REQUIRE( std::is_enum_v<WormholeStrategy> );
		}
		THEN( "it has int_fast8_t as underlying type" ) {
			REQUIRE( std::is_same_v<std::underlying_type_t<WormholeStrategy>, int_fast8_t> );
		}
	}
}

SCENARIO( "WormholeStrategy can be used in comparisons", "[WormholeStrategy]" ) {
	GIVEN( "two WormholeStrategy values" ) {
		WHEN( "they are the same" ) {
			THEN( "they compare equal" ) {
				REQUIRE( WormholeStrategy::NONE == WormholeStrategy::NONE );
				REQUIRE( WormholeStrategy::ONLY_UNRESTRICTED == WormholeStrategy::ONLY_UNRESTRICTED );
				REQUIRE( WormholeStrategy::ALL == WormholeStrategy::ALL );
			}
		}
		WHEN( "they are different" ) {
			THEN( "they compare not equal" ) {
				REQUIRE( WormholeStrategy::NONE != WormholeStrategy::ONLY_UNRESTRICTED );
				REQUIRE( WormholeStrategy::ONLY_UNRESTRICTED != WormholeStrategy::ALL );
				REQUIRE( WormholeStrategy::ALL != WormholeStrategy::NONE );
			}
		}
	}
}

SCENARIO( "WormholeStrategy can be used in switch statements", "[WormholeStrategy]" ) {
	GIVEN( "a WormholeStrategy value" ) {
		WHEN( "using it in a switch" ) {
			THEN( "all cases are handled correctly" ) {
				auto getName = [](WormholeStrategy strategy) -> const char* {
					switch(strategy) {
						case WormholeStrategy::NONE: return "NONE";
						case WormholeStrategy::ONLY_UNRESTRICTED: return "ONLY_UNRESTRICTED";
						case WormholeStrategy::ALL: return "ALL";
					}
					return "UNKNOWN";
				};

				REQUIRE( std::string(getName(WormholeStrategy::NONE)) == "NONE" );
				REQUIRE( std::string(getName(WormholeStrategy::ONLY_UNRESTRICTED)) == "ONLY_UNRESTRICTED" );
				REQUIRE( std::string(getName(WormholeStrategy::ALL)) == "ALL" );
			}
		}
	}
}

SCENARIO( "WormholeStrategy represents wormhole usage options", "[WormholeStrategy]" ) {
	GIVEN( "different wormhole strategies" ) {
		THEN( "NONE disallows all wormholes" ) {
			CHECK( WormholeStrategy::NONE == WormholeStrategy::NONE );
		}
		THEN( "ONLY_UNRESTRICTED allows only unrestricted wormholes" ) {
			CHECK( WormholeStrategy::ONLY_UNRESTRICTED == WormholeStrategy::ONLY_UNRESTRICTED );
		}
		THEN( "ALL allows all wormholes" ) {
			CHECK( WormholeStrategy::ALL == WormholeStrategy::ALL );
		}
	}
}

SCENARIO( "WormholeStrategy can determine if wormholes are allowed", "[WormholeStrategy]" ) {
	GIVEN( "a wormhole strategy" ) {
		WHEN( "checking if wormholes are allowed" ) {
			auto allowsWormholes = [](WormholeStrategy strategy) -> bool {
				return strategy != WormholeStrategy::NONE;
			};
			THEN( "NONE does not allow wormholes" ) {
				CHECK_FALSE( allowsWormholes(WormholeStrategy::NONE) );
			}
			THEN( "ONLY_UNRESTRICTED allows wormholes" ) {
				CHECK( allowsWormholes(WormholeStrategy::ONLY_UNRESTRICTED) );
			}
			THEN( "ALL allows wormholes" ) {
				CHECK( allowsWormholes(WormholeStrategy::ALL) );
			}
		}
		WHEN( "checking if restricted wormholes are allowed" ) {
			auto allowsRestricted = [](WormholeStrategy strategy) -> bool {
				return strategy == WormholeStrategy::ALL;
			};
			THEN( "NONE does not allow restricted wormholes" ) {
				CHECK_FALSE( allowsRestricted(WormholeStrategy::NONE) );
			}
			THEN( "ONLY_UNRESTRICTED does not allow restricted wormholes" ) {
				CHECK_FALSE( allowsRestricted(WormholeStrategy::ONLY_UNRESTRICTED) );
			}
			THEN( "ALL allows restricted wormholes" ) {
				CHECK( allowsRestricted(WormholeStrategy::ALL) );
			}
		}
	}
}
// #endregion unit tests



} // test namespace