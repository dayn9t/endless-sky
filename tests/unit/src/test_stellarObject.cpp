/* test_stellarObject.cpp
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
#include "../../../source/StellarObject.h"

// Additional headers for types used in tests.
#include "../../../source/Radar.h"

// ... and any system includes needed for the test file.
#include <type_traits>
#include <string>

namespace { // test namespace

// #region mock data

// Insert file-local data here, e.g. classes, structs, or fixtures that will be useful
// to help test this class/method.

// #endregion mock data



// #region unit tests

// Test Class Traits
TEST_CASE( "StellarObject Class Traits", "[StellarObject]" ) {
	using T = StellarObject;
	SECTION( "Class Properties" ) {
		// StellarObject inherits from Body which has non-trivial members.
		CHECK_FALSE( std::is_trivial_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
		CHECK_FALSE( std::is_trivially_destructible_v<T> );
	}
	SECTION( "Construction Traits" ) {
		CHECK( std::is_default_constructible_v<T> );
		CHECK_FALSE( std::is_trivially_default_constructible_v<T> );
		CHECK( std::is_copy_constructible_v<T> );
		CHECK( std::is_move_constructible_v<T> );
	}
	SECTION( "Copy/Move Traits" ) {
		CHECK( std::is_copy_assignable_v<T> );
		CHECK( std::is_move_assignable_v<T> );
		CHECK_FALSE( std::is_trivially_copyable_v<T> );
	}
}

// Test default construction.
SCENARIO( "Creating a StellarObject with default constructor", "[StellarObject][Creation]" ) {
	GIVEN( "a default-constructed StellarObject" ) {
		StellarObject obj;

		WHEN( "checking initial state" ) {
			THEN( "it has no valid planet" ) {
				CHECK_FALSE( obj.HasValidPlanet() );
			}
			THEN( "planet pointer is null" ) {
				CHECK( obj.GetPlanet() == nullptr );
			}
			THEN( "display name returns unknown placeholder" ) {
				CHECK( obj.DisplayName() == "???" );
			}
			THEN( "it is not a star" ) {
				CHECK_FALSE( obj.IsStar() );
			}
			THEN( "it is not a station" ) {
				CHECK_FALSE( obj.IsStation() );
			}
			THEN( "it is not a moon" ) {
				CHECK_FALSE( obj.IsMoon() );
			}
			THEN( "parent index is -1" ) {
				CHECK( obj.Parent() == -1 );
			}
			THEN( "distance is zero" ) {
				CHECK( obj.Distance() == 0. );
			}
			THEN( "hazards vector is empty" ) {
				CHECK( obj.Hazards().empty() );
			}
		}
	}
}

// Test HasValidPlanet method.
SCENARIO( "StellarObject::HasValidPlanet", "[StellarObject][HasValidPlanet]" ) {
	GIVEN( "a default-constructed StellarObject" ) {
		StellarObject obj;

		WHEN( "checking HasValidPlanet" ) {
			THEN( "returns false because planet pointer is null" ) {
				CHECK_FALSE( obj.HasValidPlanet() );
			}
		}
	}
	// Note: Testing with a valid Planet pointer requires GameData initialization
	// and a properly loaded Planet instance.
}

// Test GetPlanet method.
SCENARIO( "StellarObject::GetPlanet", "[StellarObject][GetPlanet]" ) {
	GIVEN( "a default-constructed StellarObject" ) {
		StellarObject obj;

		WHEN( "getting the planet pointer" ) {
			THEN( "returns nullptr" ) {
				CHECK( obj.GetPlanet() == nullptr );
			}
		}
	}
}

// Test DisplayName method.
SCENARIO( "StellarObject::DisplayName", "[StellarObject][DisplayName]" ) {
	GIVEN( "a default-constructed StellarObject" ) {
		StellarObject obj;

		WHEN( "querying display name with no planet" ) {
			THEN( "returns '???' placeholder" ) {
				CHECK( obj.DisplayName() == "???" );
			}
		}
	}
	// Note: When a valid planet is set, DisplayName returns the planet's display name.
	// This requires GameData initialization and a properly loaded Planet.
}

// Test LandingMessage method.
SCENARIO( "StellarObject::LandingMessage", "[StellarObject][LandingMessage]" ) {
	GIVEN( "a default-constructed StellarObject" ) {
		StellarObject obj;

		WHEN( "querying landing message with no planet and no sprite" ) {
			THEN( "returns empty string" ) {
				CHECK( obj.LandingMessage().empty() );
			}
		}
	}
	// Note: LandingMessage depends on:
	// 1. GameData::HasLandingMessage() for custom sprite messages
	// 2. The 'message' member pointer set by System class
	// Full testing requires GameData initialization.
}

// Test IsStar method.
SCENARIO( "StellarObject::IsStar", "[StellarObject][IsStar]" ) {
	GIVEN( "a default-constructed StellarObject" ) {
		StellarObject obj;

		WHEN( "checking if it is a star" ) {
			THEN( "returns false by default" ) {
				CHECK_FALSE( obj.IsStar() );
			}
		}
	}
	// Note: isStar is set by the System class during stellar object configuration.
}

// Test IsStation method.
SCENARIO( "StellarObject::IsStation", "[StellarObject][IsStation]" ) {
	GIVEN( "a default-constructed StellarObject" ) {
		StellarObject obj;

		WHEN( "checking if it is a station" ) {
			THEN( "returns false by default" ) {
				CHECK_FALSE( obj.IsStation() );
			}
		}
	}
	// Note: isStation is set by the System class during stellar object configuration.
}

// Test IsMoon method.
SCENARIO( "StellarObject::IsMoon", "[StellarObject][IsMoon]" ) {
	GIVEN( "a default-constructed StellarObject" ) {
		StellarObject obj;

		WHEN( "checking if it is a moon" ) {
			THEN( "returns false by default" ) {
				CHECK_FALSE( obj.IsMoon() );
			}
		}
	}
	// Note: isMoon is set by the System class during stellar object configuration.
}

// Test Parent method.
SCENARIO( "StellarObject::Parent", "[StellarObject][Parent]" ) {
	GIVEN( "a default-constructed StellarObject" ) {
		StellarObject obj;

		WHEN( "getting parent index" ) {
			THEN( "returns -1 (no parent)" ) {
				CHECK( obj.Parent() == -1 );
			}
		}
	}
	// Note: parent index is set by System class to reference another object in the system.
}

// Test Distance method.
SCENARIO( "StellarObject::Distance", "[StellarObject][Distance]" ) {
	GIVEN( "a default-constructed StellarObject" ) {
		StellarObject obj;

		WHEN( "getting distance from parent" ) {
			THEN( "returns 0 by default" ) {
				CHECK( obj.Distance() == 0. );
			}
		}
	}
	// Note: distance is set by System class to indicate orbital distance from parent.
}

// Test Hazards method.
SCENARIO( "StellarObject::Hazards", "[StellarObject][Hazards]" ) {
	GIVEN( "a default-constructed StellarObject" ) {
		StellarObject obj;

		WHEN( "getting hazards" ) {
			THEN( "returns empty vector by default" ) {
				CHECK( obj.Hazards().empty() );
			}
		}
	}
	// Note: hazards are populated by System class when loading system data.
}

// Test Radius method.
SCENARIO( "StellarObject::Radius", "[StellarObject][Radius]" ) {
	GIVEN( "a default-constructed StellarObject" ) {
		StellarObject obj;

		WHEN( "getting radius without a sprite" ) {
			THEN( "returns -1 (invalid)" ) {
				CHECK( obj.Radius() == -1. );
			}
		}
	}
	// Note: Radius depends on HasSprite() and Width()/Height() from Body.
	// When sprite is present, radius = 0.5 * min(Width, Height).
	// For stars, radius is capped at 80.
}

// Test HasSprite method.
SCENARIO( "StellarObject::HasSprite", "[StellarObject][HasSprite]" ) {
	GIVEN( "a default-constructed StellarObject" ) {
		StellarObject obj;

		WHEN( "checking HasSprite without UsingMatchesCommand" ) {
			THEN( "returns false (no sprite set)" ) {
				CHECK_FALSE( obj.HasSprite() );
			}
		}
	}
	// Note: HasSprite returns Body::HasSprite() || usingMatches.
	// The usingMatches flag is set by UsingMatchesCommand() static method.
}

// Test UsingMatchesCommand static method.
SCENARIO( "StellarObject::UsingMatchesCommand", "[StellarObject][UsingMatchesCommand]" ) {
	GIVEN( "the static UsingMatchesCommand flag" ) {
		// Reset state: we can't easily reset the flag, so we just test that
		// calling it affects subsequent HasSprite calls.

		WHEN( "UsingMatchesCommand is called" ) {
			StellarObject::UsingMatchesCommand();
			StellarObject obj;

			THEN( "HasSprite returns true even without a sprite" ) {
				CHECK( obj.HasSprite() );
			}
		}
	}
	// Note: This test has side effects on the global 'usingMatches' flag.
	// In the actual codebase, this flag is used for the "matches" console command.
}

// Test RadarType method.
SCENARIO( "StellarObject::RadarType", "[StellarObject][RadarType]" ) {
	GIVEN( "a default-constructed StellarObject" ) {
		StellarObject obj;

		WHEN( "checking radar type for a non-star without planet" ) {
			THEN( "returns Radar::INACTIVE" ) {
				// When planet is null and IsStar() is false, returns INACTIVE
				CHECK( obj.RadarType(nullptr) == Radar::INACTIVE );
			}
		}

		WHEN( "checking radar type with a ship pointer" ) {
			THEN( "still returns INACTIVE because planet is null" ) {
				// Even with a ship pointer, if planet is null, returns INACTIVE
				CHECK( obj.RadarType(reinterpret_cast<const Ship*>(0x1)) == Radar::INACTIVE );
			}
		}
	}
	// Note: Full RadarType testing requires:
	// 1. A valid Planet with accessibility info
	// 2. GameData::GetPolitics() for domination status
	// 3. Planet with government for hostility checks
}

// Test copy construction.
SCENARIO( "Copying StellarObject instances", "[StellarObject][Copy]" ) {
	GIVEN( "a default-constructed StellarObject" ) {
		StellarObject original;

		WHEN( "copy-constructing" ) {
			StellarObject copy(original);

			THEN( "the copy has the same state" ) {
				CHECK( copy.GetPlanet() == original.GetPlanet() );
				CHECK( copy.HasValidPlanet() == original.HasValidPlanet() );
				CHECK( copy.IsStar() == original.IsStar() );
				CHECK( copy.IsStation() == original.IsStation() );
				CHECK( copy.IsMoon() == original.IsMoon() );
				CHECK( copy.Parent() == original.Parent() );
				CHECK( copy.Distance() == original.Distance() );
				CHECK( copy.Hazards().size() == original.Hazards().size() );
			}
		}
	}
}

// Test copy assignment.
SCENARIO( "Copy assigning StellarObject instances", "[StellarObject][CopyAssign]" ) {
	GIVEN( "two StellarObject instances" ) {
		StellarObject original;
		StellarObject assigned;

		WHEN( "copy-assigning one to another" ) {
			assigned = original;

			THEN( "both have the same state" ) {
				CHECK( assigned.GetPlanet() == original.GetPlanet() );
				CHECK( assigned.HasValidPlanet() == original.HasValidPlanet() );
				CHECK( assigned.IsStar() == original.IsStar() );
				CHECK( assigned.IsStation() == original.IsStation() );
				CHECK( assigned.IsMoon() == original.IsMoon() );
				CHECK( assigned.Parent() == original.Parent() );
				CHECK( assigned.Distance() == original.Distance() );
			}
		}
	}
}

// Test move construction.
SCENARIO( "Move constructing StellarObject instances", "[StellarObject][Move]" ) {
	GIVEN( "a StellarObject to move" ) {
		StellarObject original;

		WHEN( "move-constructing" ) {
			StellarObject moved(std::move(original));

			THEN( "the moved-to object has valid state" ) {
				CHECK_FALSE( moved.HasValidPlanet() );
				CHECK( moved.GetPlanet() == nullptr );
				CHECK_FALSE( moved.IsStar() );
				CHECK_FALSE( moved.IsStation() );
				CHECK_FALSE( moved.IsMoon() );
				CHECK( moved.Parent() == -1 );
				CHECK( moved.Distance() == 0. );
			}
		}
	}
}

// Test move assignment.
SCENARIO( "Move assigning StellarObject instances", "[StellarObject][MoveAssign]" ) {
	GIVEN( "two StellarObject instances" ) {
		StellarObject original;
		StellarObject assigned;

		WHEN( "move-assigning one to another" ) {
			assigned = std::move(original);

			THEN( "the assigned object has valid state" ) {
				CHECK_FALSE( assigned.HasValidPlanet() );
				CHECK( assigned.GetPlanet() == nullptr );
				CHECK_FALSE( assigned.IsStar() );
				CHECK_FALSE( assigned.IsStation() );
				CHECK_FALSE( assigned.IsMoon() );
				CHECK( assigned.Parent() == -1 );
				CHECK( assigned.Distance() == 0. );
			}
		}
	}
}

// Test inherited Body methods still work.
SCENARIO( "StellarObject inherits from Body correctly", "[StellarObject][Inheritance]" ) {
	GIVEN( "a default-constructed StellarObject" ) {
		StellarObject obj;

		WHEN( "checking inherited Body properties" ) {
			THEN( "position is (0, 0)" ) {
				CHECK( obj.Position().X() == 0. );
				CHECK( obj.Position().Y() == 0. );
			}
			AND_THEN( "velocity is (0, 0)" ) {
				CHECK( obj.Velocity().X() == 0. );
				CHECK( obj.Velocity().Y() == 0. );
			}
			AND_THEN( "facing angle is 0 degrees" ) {
				CHECK_THAT( obj.Facing().Degrees(), Catch::Matchers::WithinAbs(0., 0.0001) );
			}
			AND_THEN( "zoom is 2.0 (StellarObject default, not Body's 1.0)" ) {
				CHECK_THAT( obj.Zoom(), Catch::Matchers::WithinAbs(2., 0.0001) );
			}
			AND_THEN( "government is null" ) {
				CHECK( obj.GetGovernment() == nullptr );
			}
		}
	}
}

// Test zoom default value (StellarObject sets zoom to 2.0 unlike Body's 1.0).
SCENARIO( "StellarObject zoom default", "[StellarObject][Zoom]" ) {
	GIVEN( "a default-constructed StellarObject" ) {
		StellarObject obj;

		WHEN( "checking zoom value" ) {
			THEN( "zoom is 2.0 (not half size like ships)" ) {
				CHECK_THAT( obj.Zoom(), Catch::Matchers::WithinAbs(2., 0.0001) );
			}
		}
	}
}

// ============================================================================
// Integration Test Notes
// ============================================================================
// The following StellarObject functionality requires integration tests with full
// GameData and System initialization:
//
// 1. Setting object properties via System (friend class):
//    - Setting planet pointer
//    - Setting distance, speed, offset
//    - Setting parent index
//    - Setting isStar, isStation, isMoon flags
//    - Setting message pointer
//    - Adding hazards
//
// 2. Radius() with valid sprite:
//    - Requires a loaded Sprite
//    - Tests the sprite-based radius calculation
//    - Tests star radius capping at 80
//
// 3. HasValidPlanet() with valid Planet:
//    - Requires a Planet instance with IsValid() returning true
//
// 4. DisplayName() with valid Planet:
//    - Requires a Planet with a non-empty DisplayName()
//
// 5. LandingMessage() with custom messages:
//    - Requires GameData with landing messages
//    - Requires message pointer to be set
//
// 6. RadarType() with valid Planet and Ship:
//    - Requires a valid Ship instance
//    - Requires a Planet with IsAccessible() behavior
//    - Requires GameData::GetPolitics() for domination checks
//    - Tests all Radar types: STAR, INACTIVE, ANOMALOUS, PLAYER, FRIENDLY, UNFRIENDLY, HOSTILE
//
// 7. Hazards with actual Hazard objects:
//    - Requires System to populate hazards
//    - Requires Hazard instances
// ============================================================================

// #endregion unit tests



// #region benchmarks
#ifdef CATCH_CONFIG_ENABLE_BENCHMARKING
TEST_CASE( "Benchmark StellarObject construction", "[!benchmark][StellarObject]" ) {
	BENCHMARK( "StellarObject::StellarObject()" ) {
		return StellarObject();
	};
}

TEST_CASE( "Benchmark StellarObject getters", "[!benchmark][StellarObject]" ) {
	StellarObject obj;
	BENCHMARK( "StellarObject::HasValidPlanet()" ) {
		return obj.HasValidPlanet();
	};
	BENCHMARK( "StellarObject::GetPlanet()" ) {
		return obj.GetPlanet();
	};
	BENCHMARK( "StellarObject::DisplayName()" ) {
		return obj.DisplayName();
	};
	BENCHMARK( "StellarObject::LandingMessage()" ) {
		return obj.LandingMessage();
	};
	BENCHMARK( "StellarObject::IsStar()" ) {
		return obj.IsStar();
	};
	BENCHMARK( "StellarObject::IsStation()" ) {
		return obj.IsStation();
	};
	BENCHMARK( "StellarObject::IsMoon()" ) {
		return obj.IsMoon();
	};
	BENCHMARK( "StellarObject::Parent()" ) {
		return obj.Parent();
	};
	BENCHMARK( "StellarObject::Distance()" ) {
		return obj.Distance();
	};
	BENCHMARK( "StellarObject::Radius()" ) {
		return obj.Radius();
	};
	BENCHMARK( "StellarObject::Hazards()" ) {
		return obj.Hazards();
	};
	BENCHMARK( "StellarObject::HasSprite()" ) {
		return obj.HasSprite();
	};
	BENCHMARK( "StellarObject::RadarType(nullptr)" ) {
		return obj.RadarType(nullptr);
	};
}

TEST_CASE( "Benchmark StellarObject copy/move", "[!benchmark][StellarObject][CopyMove]" ) {
	StellarObject original;
	BENCHMARK( "StellarObject copy construction" ) {
		return StellarObject(original);
	};
	BENCHMARK( "StellarObject move construction" ) {
		StellarObject toMove;
		return StellarObject(std::move(toMove));
	};
	BENCHMARK( "StellarObject copy assignment" ) {
		StellarObject copy;
		return copy = original;
	};
	BENCHMARK( "StellarObject move assignment" ) {
		StellarObject toMove;
		StellarObject moved;
		return moved = std::move(toMove);
	};
}
#endif
// #endregion benchmarks

} // test namespace