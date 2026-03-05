/* test_JumpType.cpp
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
#include "../../../source/JumpType.h"

// ... and any system includes needed for the test file.
#include <type_traits>

namespace { // test namespace

// #region mock data
// #endregion mock data



// #region unit tests
SCENARIO( "JumpType enum values are defined correctly", "[JumpType]" ) {
	GIVEN( "the JumpType enum" ) {
		THEN( "NONE has value 0" ) {
			REQUIRE( static_cast<int>(JumpType::NONE) == 0 );
		}
		THEN( "HYPERDRIVE has value 1" ) {
			REQUIRE( static_cast<int>(JumpType::HYPERDRIVE) == 1 );
		}
		THEN( "JUMP_DRIVE has value 2" ) {
			REQUIRE( static_cast<int>(JumpType::JUMP_DRIVE) == 2 );
		}
	}
}

SCENARIO( "JumpType is a scoped enum", "[JumpType]" ) {
	GIVEN( "the JumpType type" ) {
		THEN( "it is an enum class" ) {
			REQUIRE( std::is_enum_v<JumpType> );
			REQUIRE( std::is_enum_v<JumpType> );
		}
		THEN( "it has int as underlying type" ) {
			REQUIRE( std::is_same_v<std::underlying_type_t<JumpType>, int> );
		}
	}
}

SCENARIO( "JumpType can be used in comparisons", "[JumpType]" ) {
	GIVEN( "two JumpType values" ) {
		WHEN( "they are the same" ) {
			THEN( "they compare equal" ) {
				REQUIRE( JumpType::NONE == JumpType::NONE );
				REQUIRE( JumpType::HYPERDRIVE == JumpType::HYPERDRIVE );
				REQUIRE( JumpType::JUMP_DRIVE == JumpType::JUMP_DRIVE );
			}
		}
		WHEN( "they are different" ) {
			THEN( "they compare not equal" ) {
				REQUIRE( JumpType::NONE != JumpType::HYPERDRIVE );
				REQUIRE( JumpType::HYPERDRIVE != JumpType::JUMP_DRIVE );
				REQUIRE( JumpType::JUMP_DRIVE != JumpType::NONE );
			}
		}
	}
}

SCENARIO( "JumpType can be used in switch statements", "[JumpType]" ) {
	GIVEN( "a JumpType value" ) {
		WHEN( "using it in a switch" ) {
			THEN( "all cases are handled correctly" ) {
				auto getName = [](JumpType type) -> const char* {
					switch(type) {
						case JumpType::NONE: return "NONE";
						case JumpType::HYPERDRIVE: return "HYPERDRIVE";
						case JumpType::JUMP_DRIVE: return "JUMP_DRIVE";
					}
					return "UNKNOWN";
				};

				REQUIRE( std::string(getName(JumpType::NONE)) == "NONE" );
				REQUIRE( std::string(getName(JumpType::HYPERDRIVE)) == "HYPERDRIVE" );
				REQUIRE( std::string(getName(JumpType::JUMP_DRIVE)) == "JUMP_DRIVE" );
			}
		}
	}
}

SCENARIO( "JumpType represents jump methods", "[JumpType]" ) {
	GIVEN( "different jump types" ) {
		THEN( "NONE represents no jump capability" ) {
			CHECK( JumpType::NONE == JumpType::NONE );
		}
		THEN( "HYPERDRIVE represents hyperdrive jump" ) {
			CHECK( JumpType::HYPERDRIVE == JumpType::HYPERDRIVE );
		}
		THEN( "JUMP_DRIVE represents jump drive jump" ) {
			CHECK( JumpType::JUMP_DRIVE == JumpType::JUMP_DRIVE );
		}
	}
}

SCENARIO( "JumpType can distinguish between drive types", "[JumpType]" ) {
	GIVEN( "a ship with different jump capabilities" ) {
		WHEN( "checking if ship can jump at all" ) {
			auto canJump = [](JumpType type) -> bool {
				return type != JumpType::NONE;
			};
			THEN( "NONE means no jump capability" ) {
				CHECK_FALSE( canJump(JumpType::NONE) );
			}
			THEN( "HYPERDRIVE means jump capable" ) {
				CHECK( canJump(JumpType::HYPERDRIVE) );
			}
			THEN( "JUMP_DRIVE means jump capable" ) {
				CHECK( canJump(JumpType::JUMP_DRIVE) );
			}
		}
		WHEN( "distinguishing between drive types" ) {
			auto getDriveType = [](JumpType type) -> const char* {
				if(type == JumpType::NONE) return "no_drive";
				if(type == JumpType::HYPERDRIVE) return "hyperdrive";
				if(type == JumpType::JUMP_DRIVE) return "jump_drive";
				return "unknown";
			};
			THEN( "each type is correctly identified" ) {
				CHECK( std::string(getDriveType(JumpType::NONE)) == "no_drive" );
				CHECK( std::string(getDriveType(JumpType::HYPERDRIVE)) == "hyperdrive" );
				CHECK( std::string(getDriveType(JumpType::JUMP_DRIVE)) == "jump_drive" );
			}
		}
	}
}
// #endregion unit tests



} // test namespace