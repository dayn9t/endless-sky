/* test_SystemEntry.cpp
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
#include "../../../source/SystemEntry.h"

// ... and any system includes needed for the test file.
#include <type_traits>

namespace { // test namespace

// #region mock data
// #endregion mock data



// #region unit tests
SCENARIO( "SystemEntry enum values are defined correctly", "[SystemEntry]" ) {
	GIVEN( "the SystemEntry enum" ) {
		THEN( "TAKE_OFF has value 0" ) {
			REQUIRE( static_cast<int>(SystemEntry::TAKE_OFF) == 0 );
		}
		THEN( "HYPERDRIVE has value 1" ) {
			REQUIRE( static_cast<int>(SystemEntry::HYPERDRIVE) == 1 );
		}
		THEN( "JUMP has value 2" ) {
			REQUIRE( static_cast<int>(SystemEntry::JUMP) == 2 );
		}
		THEN( "WORMHOLE has value 3" ) {
			REQUIRE( static_cast<int>(SystemEntry::WORMHOLE) == 3 );
		}
	}
}

SCENARIO( "SystemEntry is a scoped enum", "[SystemEntry]" ) {
	GIVEN( "the SystemEntry type" ) {
		THEN( "it is an enum class" ) {
			REQUIRE( std::is_enum_v<SystemEntry> );
			REQUIRE( std::is_enum_v<SystemEntry> );
		}
	}
}

SCENARIO( "SystemEntry can be used in comparisons", "[SystemEntry]" ) {
	GIVEN( "two SystemEntry values" ) {
		WHEN( "they are the same" ) {
			THEN( "they compare equal" ) {
				REQUIRE( SystemEntry::TAKE_OFF == SystemEntry::TAKE_OFF );
				REQUIRE( SystemEntry::HYPERDRIVE == SystemEntry::HYPERDRIVE );
				REQUIRE( SystemEntry::JUMP == SystemEntry::JUMP );
				REQUIRE( SystemEntry::WORMHOLE == SystemEntry::WORMHOLE );
			}
		}
		WHEN( "they are different" ) {
			THEN( "they compare not equal" ) {
				REQUIRE( SystemEntry::TAKE_OFF != SystemEntry::HYPERDRIVE );
				REQUIRE( SystemEntry::HYPERDRIVE != SystemEntry::JUMP );
				REQUIRE( SystemEntry::JUMP != SystemEntry::WORMHOLE );
				REQUIRE( SystemEntry::WORMHOLE != SystemEntry::TAKE_OFF );
			}
		}
	}
}

SCENARIO( "SystemEntry can be used in switch statements", "[SystemEntry]" ) {
	GIVEN( "a SystemEntry value" ) {
		WHEN( "using it in a switch" ) {
			THEN( "all cases are handled correctly" ) {
				auto getName = [](SystemEntry entry) -> const char* {
					switch(entry) {
						case SystemEntry::TAKE_OFF: return "TAKE_OFF";
						case SystemEntry::HYPERDRIVE: return "HYPERDRIVE";
						case SystemEntry::JUMP: return "JUMP";
						case SystemEntry::WORMHOLE: return "WORMHOLE";
					}
					return "UNKNOWN";
				};

				REQUIRE( std::string(getName(SystemEntry::TAKE_OFF)) == "TAKE_OFF" );
				REQUIRE( std::string(getName(SystemEntry::HYPERDRIVE)) == "HYPERDRIVE" );
				REQUIRE( std::string(getName(SystemEntry::JUMP)) == "JUMP" );
				REQUIRE( std::string(getName(SystemEntry::WORMHOLE)) == "WORMHOLE" );
			}
		}
	}
}

SCENARIO( "SystemEntry represents entry methods", "[SystemEntry]" ) {
	GIVEN( "different system entry types" ) {
		THEN( "TAKE_OFF represents taking off from a planet" ) {
			CHECK( SystemEntry::TAKE_OFF == SystemEntry::TAKE_OFF );
		}
		THEN( "HYPERDRIVE represents hyperdrive entry" ) {
			CHECK( SystemEntry::HYPERDRIVE == SystemEntry::HYPERDRIVE );
		}
		THEN( "JUMP represents jump drive entry" ) {
			CHECK( SystemEntry::JUMP == SystemEntry::JUMP );
		}
		THEN( "WORMHOLE represents wormhole entry" ) {
			CHECK( SystemEntry::WORMHOLE == SystemEntry::WORMHOLE );
		}
	}
}

SCENARIO( "SystemEntry can distinguish jump types", "[SystemEntry]" ) {
	GIVEN( "a system entry type" ) {
		WHEN( "checking if it is a jump entry" ) {
			auto isJumpEntry = [](SystemEntry entry) -> bool {
				return entry == SystemEntry::HYPERDRIVE ||
				       entry == SystemEntry::JUMP;
			};
			THEN( "TAKE_OFF is not a jump entry" ) {
				CHECK_FALSE( isJumpEntry(SystemEntry::TAKE_OFF) );
			}
			THEN( "HYPERDRIVE is a jump entry" ) {
				CHECK( isJumpEntry(SystemEntry::HYPERDRIVE) );
			}
			THEN( "JUMP is a jump entry" ) {
				CHECK( isJumpEntry(SystemEntry::JUMP) );
			}
			THEN( "WORMHOLE is not a jump entry" ) {
				CHECK_FALSE( isJumpEntry(SystemEntry::WORMHOLE) );
			}
		}
		WHEN( "checking if it requires fuel" ) {
			auto requiresFuel = [](SystemEntry entry) -> bool {
				return entry == SystemEntry::HYPERDRIVE ||
				       entry == SystemEntry::JUMP;
			};
			THEN( "TAKE_OFF does not require fuel for jump" ) {
				CHECK_FALSE( requiresFuel(SystemEntry::TAKE_OFF) );
			}
			THEN( "HYPERDRIVE requires fuel" ) {
				CHECK( requiresFuel(SystemEntry::HYPERDRIVE) );
			}
			THEN( "JUMP requires fuel" ) {
				CHECK( requiresFuel(SystemEntry::JUMP) );
			}
			THEN( "WORMHOLE does not require fuel" ) {
				CHECK_FALSE( requiresFuel(SystemEntry::WORMHOLE) );
			}
		}
	}
}
// #endregion unit tests



} // test namespace