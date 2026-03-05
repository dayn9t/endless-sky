/* test_savedGame.cpp
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
#include "../../../source/SavedGame.h"

// ... and any system includes needed for the test file.
#include <type_traits>

namespace { // test namespace

// #region mock data

// #endregion mock data



// #region unit tests

SCENARIO( "Creating a SavedGame with default constructor", "[SavedGame][Creation]" ) {
	GIVEN( "a default-constructed SavedGame" ) {
		SavedGame savedGame;

		THEN( "it is not loaded" ) {
			REQUIRE_FALSE( savedGame.IsLoaded() );
		}
		THEN( "the path is empty" ) {
			REQUIRE( savedGame.Path().empty() );
		}
		THEN( "all string getters return empty strings" ) {
			REQUIRE( savedGame.Name().empty() );
			REQUIRE( savedGame.Credits().empty() );
			REQUIRE( savedGame.GetDate().empty() );
			REQUIRE( savedGame.GetSystem().empty() );
			REQUIRE( savedGame.GetPlanet().empty() );
			REQUIRE( savedGame.ShipName().empty() );
		}
		THEN( "play time has default value of 0s" ) {
			REQUIRE( savedGame.GetPlayTime() == "0s" );
		}
		THEN( "ship sprite is null" ) {
			REQUIRE( savedGame.ShipSprite() == nullptr );
		}
	}
}

SCENARIO( "Clearing a SavedGame", "[SavedGame][Clear]" ) {
	GIVEN( "a default-constructed SavedGame" ) {
		SavedGame savedGame;

		WHEN( "Clear() is called" ) {
			savedGame.Clear();

			THEN( "it remains unloaded" ) {
				REQUIRE_FALSE( savedGame.IsLoaded() );
			}
			THEN( "the path is empty" ) {
				REQUIRE( savedGame.Path().empty() );
			}
			THEN( "all string getters return empty strings" ) {
				REQUIRE( savedGame.Name().empty() );
				REQUIRE( savedGame.Credits().empty() );
				REQUIRE( savedGame.GetDate().empty() );
				REQUIRE( savedGame.GetSystem().empty() );
				REQUIRE( savedGame.GetPlanet().empty() );
				REQUIRE( savedGame.ShipName().empty() );
			}
			THEN( "play time has default value of 0s" ) {
				REQUIRE( savedGame.GetPlayTime() == "0s" );
			}
			THEN( "ship sprite is null" ) {
				REQUIRE( savedGame.ShipSprite() == nullptr );
			}
		}
	}
}

SCENARIO( "Checking SavedGame loaded state", "[SavedGame][IsLoaded]" ) {
	GIVEN( "a default-constructed SavedGame" ) {
		SavedGame savedGame;

		WHEN( "checking IsLoaded" ) {
			THEN( "it returns false" ) {
				REQUIRE_FALSE( savedGame.IsLoaded() );
			}
		}
	}
}

SCENARIO( "Getting the path from SavedGame", "[SavedGame][Path]" ) {
	GIVEN( "a default-constructed SavedGame" ) {
		SavedGame savedGame;

		WHEN( "getting the path" ) {
			THEN( "it returns an empty path" ) {
				REQUIRE( savedGame.Path().empty() );
			}
		}
	}
}

SCENARIO( "Getting player name from SavedGame", "[SavedGame][Name]" ) {
	GIVEN( "a default-constructed SavedGame" ) {
		SavedGame savedGame;

		WHEN( "getting the name" ) {
			THEN( "it returns an empty string" ) {
				REQUIRE( savedGame.Name().empty() );
			}
		}
	}
}

SCENARIO( "Getting credits from SavedGame", "[SavedGame][Credits]" ) {
	GIVEN( "a default-constructed SavedGame" ) {
		SavedGame savedGame;

		WHEN( "getting credits" ) {
			THEN( "it returns an empty string" ) {
				REQUIRE( savedGame.Credits().empty() );
			}
		}
	}
}

SCENARIO( "Getting date from SavedGame", "[SavedGame][GetDate]" ) {
	GIVEN( "a default-constructed SavedGame" ) {
		SavedGame savedGame;

		WHEN( "getting the date" ) {
			THEN( "it returns an empty string" ) {
				REQUIRE( savedGame.GetDate().empty() );
			}
		}
	}
}

SCENARIO( "Getting system from SavedGame", "[SavedGame][GetSystem]" ) {
	GIVEN( "a default-constructed SavedGame" ) {
		SavedGame savedGame;

		WHEN( "getting the system" ) {
			THEN( "it returns an empty string" ) {
				REQUIRE( savedGame.GetSystem().empty() );
			}
		}
	}
}

SCENARIO( "Getting planet from SavedGame", "[SavedGame][GetPlanet]" ) {
	GIVEN( "a default-constructed SavedGame" ) {
		SavedGame savedGame;

		WHEN( "getting the planet" ) {
			THEN( "it returns an empty string" ) {
				REQUIRE( savedGame.GetPlanet().empty() );
			}
		}
	}
}

SCENARIO( "Getting play time from SavedGame", "[SavedGame][GetPlayTime]" ) {
	GIVEN( "a default-constructed SavedGame" ) {
		SavedGame savedGame;

		WHEN( "getting play time" ) {
			THEN( "it returns '0s'" ) {
				REQUIRE( savedGame.GetPlayTime() == "0s" );
			}
		}
	}
}

SCENARIO( "Getting ship sprite from SavedGame", "[SavedGame][ShipSprite]" ) {
	GIVEN( "a default-constructed SavedGame" ) {
		SavedGame savedGame;

		WHEN( "getting the ship sprite" ) {
			THEN( "it returns nullptr" ) {
				REQUIRE( savedGame.ShipSprite() == nullptr );
			}
		}
	}
}

SCENARIO( "Getting ship name from SavedGame", "[SavedGame][ShipName]" ) {
	GIVEN( "a default-constructed SavedGame" ) {
		SavedGame savedGame;

		WHEN( "getting the ship name" ) {
			THEN( "it returns an empty string" ) {
				REQUIRE( savedGame.ShipName().empty() );
			}
		}
	}
}

SCENARIO( "SavedGame class traits", "[SavedGame][Traits]" ) {
	using T = SavedGame;

	THEN( "the class has the expected traits" ) {
		CHECK( std::is_default_constructible_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
		CHECK( std::is_copy_constructible_v<T> );
		CHECK( std::is_copy_assignable_v<T> );
		CHECK( std::is_move_constructible_v<T> );
		CHECK( std::is_move_assignable_v<T> );
	}
}

SCENARIO( "Clearing and re-clearing a SavedGame", "[SavedGame][Clear]" ) {
	GIVEN( "a default-constructed SavedGame" ) {
		SavedGame savedGame;

		WHEN( "Clear() is called multiple times" ) {
			savedGame.Clear();
			savedGame.Clear();
			savedGame.Clear();

			THEN( "the state remains consistent" ) {
				REQUIRE_FALSE( savedGame.IsLoaded() );
				REQUIRE( savedGame.Path().empty() );
				REQUIRE( savedGame.Name().empty() );
				REQUIRE( savedGame.Credits().empty() );
				REQUIRE( savedGame.GetDate().empty() );
				REQUIRE( savedGame.GetSystem().empty() );
				REQUIRE( savedGame.GetPlanet().empty() );
				REQUIRE( savedGame.GetPlayTime() == "0s" );
				REQUIRE( savedGame.ShipSprite() == nullptr );
				REQUIRE( savedGame.ShipName().empty() );
			}
		}
	}
}

// Note: Tests for Load() require file I/O and GameData/SpriteSet dependencies.
// According to the unit test README, methods using GameData cannot be tested
// in unit tests due to the lack of dependency injection. The Load() method
// would need integration tests to verify proper file parsing behavior.
//
// The following scenarios would require integration tests:
// - Loading a valid saved game file
// - Loading a non-existent file
// - Loading an empty file
// - Loading a file with various data fields (pilot, date, system, planet, etc.)
// - Parsing flagship information

// #endregion unit tests



} // test namespace