/* test_gameVersion.cpp
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
#include "../../../source/GameVersion.h"

// ... and any system includes needed for the test file.
#include <string>

namespace { // test namespace

// #region mock data

// #endregion mock data



// #region unit tests

SCENARIO( "Creating a GameVersion", "[GameVersion][Creation]" ) {
	GIVEN( "a version created with all parameters" ) {
		WHEN( "creating version 1.2.3.4 (full release)" ) {
			GameVersion version(1, 2, 3, 4, true);
			THEN( "ToString returns the correct format" ) {
				REQUIRE( version.ToString() == "1.2.3.4" );
			}
		}
		WHEN( "creating version 1.2.3.4 (alpha release)" ) {
			GameVersion version(1, 2, 3, 4, false);
			THEN( "ToString includes -alpha suffix" ) {
				REQUIRE( version.ToString() == "1.2.3.4-alpha" );
			}
		}
	}
	GIVEN( "a version created with default patch" ) {
		WHEN( "creating version 2.0.0 (patch defaults to 0)" ) {
			GameVersion version(2, 0, 0);
			THEN( "ToString shows patch as 0" ) {
				REQUIRE( version.ToString() == "2.0.0.0" );
			}
		}
		WHEN( "creating version 1.5.2 as full release (default)" ) {
			GameVersion version(1, 5, 2);
			THEN( "ToString has no -alpha suffix" ) {
				REQUIRE( version.ToString() == "1.5.2.0" );
			}
		}
	}
	GIVEN( "versions with zero values" ) {
		WHEN( "all components are zero (full release)" ) {
			GameVersion version(0, 0, 0, 0, true);
			THEN( "ToString returns 0.0.0.0" ) {
				REQUIRE( version.ToString() == "0.0.0.0" );
			}
		}
		WHEN( "all components are zero (alpha release)" ) {
			GameVersion version(0, 0, 0, 0, false);
			THEN( "ToString returns 0.0.0.0-alpha" ) {
				REQUIRE( version.ToString() == "0.0.0.0-alpha" );
			}
		}
		WHEN( "only major is non-zero" ) {
			GameVersion version(1, 0, 0, 0);
			THEN( "ToString returns 1.0.0.0" ) {
				REQUIRE( version.ToString() == "1.0.0.0" );
			}
		}
		WHEN( "only minor is non-zero" ) {
			GameVersion version(0, 1, 0, 0);
			THEN( "ToString returns 0.1.0.0" ) {
				REQUIRE( version.ToString() == "0.1.0.0" );
			}
		}
		WHEN( "only release is non-zero" ) {
			GameVersion version(0, 0, 1, 0);
			THEN( "ToString returns 0.0.1.0" ) {
				REQUIRE( version.ToString() == "0.0.1.0" );
			}
		}
		WHEN( "only patch is non-zero" ) {
			GameVersion version(0, 0, 0, 1);
			THEN( "ToString returns 0.0.0.1" ) {
				REQUIRE( version.ToString() == "0.0.0.1" );
			}
		}
	}
}

SCENARIO( "Getting the running version", "[GameVersion][Running]" ) {
	GIVEN( "the Running() static method" ) {
		WHEN( "calling GameVersion::Running()" ) {
			GameVersion running = GameVersion::Running();
			THEN( "ToString returns the expected format" ) {
				REQUIRE( running.ToString() == "0.11.1.0-alpha" );
			}
		}
	}
}

SCENARIO( "Converting GameVersion to string", "[GameVersion][ToString]" ) {
	GIVEN( "various version numbers" ) {
		WHEN( "version has single-digit components" ) {
			GameVersion version(1, 2, 3, 4);
			THEN( "components are separated by dots" ) {
				REQUIRE( version.ToString() == "1.2.3.4" );
			}
		}
		WHEN( "version has multi-digit components" ) {
			GameVersion version(10, 20, 30, 40);
			THEN( "all digits are preserved" ) {
				REQUIRE( version.ToString() == "10.20.30.40" );
			}
		}
		WHEN( "version has large values" ) {
			GameVersion version(999, 999, 999, 999);
			THEN( "all digits are preserved" ) {
				REQUIRE( version.ToString() == "999.999.999.999" );
			}
		}
	}
	GIVEN( "full release versions" ) {
		WHEN( "fullRelease is true" ) {
			GameVersion version(1, 0, 0, 0, true);
			THEN( "ToString has no suffix" ) {
				REQUIRE( version.ToString() == "1.0.0.0" );
			}
		}
	}
	GIVEN( "alpha release versions" ) {
		WHEN( "fullRelease is false" ) {
			GameVersion version(0, 11, 1, 0, false);
			THEN( "ToString has -alpha suffix" ) {
				REQUIRE( version.ToString() == "0.11.1.0-alpha" );
			}
		}
		WHEN( "alpha with all zeros" ) {
			GameVersion version(0, 0, 0, 0, false);
			THEN( "ToString is 0.0.0.0-alpha" ) {
				REQUIRE( version.ToString() == "0.0.0.0-alpha" );
			}
		}
	}
}

SCENARIO( "GameVersion with typical version patterns", "[GameVersion][Patterns]" ) {
	GIVEN( "common version formats" ) {
		WHEN( "major release (1.0.0.0)" ) {
			GameVersion version(1, 0, 0, 0);
			THEN( "ToString is correct" ) {
				REQUIRE( version.ToString() == "1.0.0.0" );
			}
		}
		WHEN( "minor release (1.1.0.0)" ) {
			GameVersion version(1, 1, 0, 0);
			THEN( "ToString is correct" ) {
				REQUIRE( version.ToString() == "1.1.0.0" );
			}
		}
		WHEN( "patch release (1.0.1.0)" ) {
			GameVersion version(1, 0, 1, 0);
			THEN( "ToString is correct" ) {
				REQUIRE( version.ToString() == "1.0.1.0" );
			}
		}
		WHEN( "build number (1.0.0.1)" ) {
			GameVersion version(1, 0, 0, 1);
			THEN( "ToString is correct" ) {
				REQUIRE( version.ToString() == "1.0.0.1" );
			}
		}
		WHEN( "full version with all parts (2.3.4.5)" ) {
			GameVersion version(2, 3, 4, 5);
			THEN( "ToString is correct" ) {
				REQUIRE( version.ToString() == "2.3.4.5" );
			}
		}
	}
}

SCENARIO( "GameVersion constexpr construction", "[GameVersion][Constexpr]" ) {
	GIVEN( "constexpr construction capabilities" ) {
		WHEN( "creating a constexpr version" ) {
			constexpr GameVersion version(1, 2, 3, 4, true);
			THEN( "the version can be used at compile time" ) {
				REQUIRE( version.ToString() == "1.2.3.4" );
			}
		}
		WHEN( "creating a constexpr version with defaults" ) {
			constexpr GameVersion version(2, 0, 0);
			THEN( "defaults are applied correctly" ) {
				REQUIRE( version.ToString() == "2.0.0.0" );
			}
		}
	}
}

SCENARIO( "GameVersion alpha vs full release distinction", "[GameVersion][ReleaseType]" ) {
	GIVEN( "two versions with same numbers but different release types" ) {
		GameVersion fullRelease(1, 0, 0, 0, true);
		GameVersion alphaRelease(1, 0, 0, 0, false);
		WHEN( "comparing ToString outputs" ) {
			THEN( "full release has no suffix" ) {
				REQUIRE( fullRelease.ToString() == "1.0.0.0" );
			}
			THEN( "alpha release has -alpha suffix" ) {
				REQUIRE( alphaRelease.ToString() == "1.0.0.0-alpha" );
			}
			THEN( "outputs differ only by the suffix" ) {
				REQUIRE( fullRelease.ToString() + "-alpha" == alphaRelease.ToString() );
			}
		}
	}
}

SCENARIO( "GameVersion boundary conditions", "[GameVersion][Boundary]" ) {
	GIVEN( "minimum values" ) {
		WHEN( "all components are zero" ) {
			GameVersion version(0, 0, 0, 0);
			THEN( "ToString handles zeros correctly" ) {
				REQUIRE( version.ToString() == "0.0.0.0" );
			}
		}
	}
	GIVEN( "large values" ) {
		WHEN( "using large unsigned values" ) {
			GameVersion version(4294967295U, 4294967295U, 4294967295U, 4294967295U);
			THEN( "ToString handles large values" ) {
				REQUIRE( version.ToString() == "4294967295.4294967295.4294967295.4294967295" );
			}
		}
	}
	GIVEN( "mixed boundary values" ) {
		WHEN( "major is max, others are zero" ) {
			GameVersion version(4294967295U, 0, 0, 0);
			THEN( "ToString is correct" ) {
				REQUIRE( version.ToString() == "4294967295.0.0.0" );
			}
		}
		WHEN( "patch is max, others are zero" ) {
			GameVersion version(0, 0, 0, 4294967295U);
			THEN( "ToString is correct" ) {
				REQUIRE( version.ToString() == "0.0.0.4294967295" );
			}
		}
	}
}

SCENARIO( "GameVersion common real-world versions", "[GameVersion][RealWorld]" ) {
	GIVEN( "realistic version scenarios" ) {
		WHEN( "semantic versioning style (1.0.0)" ) {
			GameVersion version(1, 0, 0);
			THEN( "ToString matches semantic versioning" ) {
				REQUIRE( version.ToString() == "1.0.0.0" );
			}
		}
		WHEN( "pre-release alpha version" ) {
			GameVersion version(0, 1, 0, 0, false);
			THEN( "indicates alpha status" ) {
				REQUIRE( version.ToString() == "0.1.0.0-alpha" );
			}
		}
		WHEN( "stable release version" ) {
			GameVersion version(1, 0, 0, 0, true);
			THEN( "no alpha indicator" ) {
				REQUIRE( version.ToString() == "1.0.0.0" );
			}
		}
		WHEN( "beta-like version (0.x.x)" ) {
			GameVersion version(0, 9, 0, 1, false);
			THEN( "shows development status" ) {
				REQUIRE( version.ToString() == "0.9.0.1-alpha" );
			}
		}
		WHEN( "Endless Sky current version format" ) {
			GameVersion version(0, 11, 1, 0, false);
			THEN( "matches expected format" ) {
				REQUIRE( version.ToString() == "0.11.1.0-alpha" );
			}
		}
	}
}

// #endregion unit tests



} // test namespace