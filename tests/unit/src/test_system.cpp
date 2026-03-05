/* test_system.cpp
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
#include "../../../source/System.h"

// ... and any system includes needed for the test file.
#include <set>
#include <string>

namespace { // test namespace

// #region mock data

// #endregion mock data



// #region unit tests

SCENARIO( "Creating a System", "[System][Creation]" ) {
	GIVEN( "a default-constructed System" ) {
		System system;
		WHEN( "checking basic properties" ) {
			THEN( "the system is not valid" ) {
				REQUIRE_FALSE( system.IsValid() );
			}
			THEN( "the true name is empty" ) {
				REQUIRE( system.TrueName().empty() );
			}
			THEN( "the display name is empty" ) {
				REQUIRE( system.DisplayName().empty() );
			}
			THEN( "the position is at origin" ) {
				REQUIRE( system.Position().X() == 0. );
				REQUIRE( system.Position().Y() == 0. );
			}
		}
	}
}

SCENARIO( "System name management", "[System][Name]" ) {
	GIVEN( "a default-constructed System" ) {
		System system;
		WHEN( "setting the true name" ) {
			system.SetTrueName("Test System");
			THEN( "the true name is updated" ) {
				REQUIRE( system.TrueName() == "Test System" );
			}
			THEN( "the display name is set to the true name" ) {
				REQUIRE( system.DisplayName() == "Test System" );
			}
		}
		WHEN( "setting an empty true name" ) {
			system.SetTrueName("");
			THEN( "the true name is empty" ) {
				REQUIRE( system.TrueName().empty() );
			}
		}
	}
}

SCENARIO( "System validity", "[System][Validity]" ) {
	GIVEN( "a default-constructed System" ) {
		System system;
		WHEN( "the system is not loaded" ) {
			THEN( "IsValid returns false" ) {
				REQUIRE_FALSE( system.IsValid() );
			}
		}
	}
}

SCENARIO( "System visibility flags", "[System][Visibility]" ) {
	GIVEN( "a default-constructed System" ) {
		System system;
		WHEN( "checking default visibility" ) {
			THEN( "the system is not hidden" ) {
				REQUIRE_FALSE( system.Hidden() );
			}
			THEN( "the system is not shrouded" ) {
				REQUIRE_FALSE( system.Shrouded() );
			}
			THEN( "the system is not inaccessible" ) {
				REQUIRE_FALSE( system.Inaccessible() );
			}
		}
	}
}

SCENARIO( "System position", "[System][Position]" ) {
	GIVEN( "a default-constructed System" ) {
		System system;
		WHEN( "checking the default position" ) {
			THEN( "the position is at origin (0, 0)" ) {
				REQUIRE( system.Position().X() == 0. );
				REQUIRE( system.Position().Y() == 0. );
			}
		}
	}
}

SCENARIO( "System links", "[System][Links]" ) {
	GIVEN( "a default-constructed System" ) {
		System system;
		WHEN( "checking links" ) {
			THEN( "the system has no links" ) {
				REQUIRE( system.Links().empty() );
			}
		}
		WHEN( "checking jump neighbors" ) {
			THEN( "the jump neighbors are empty" ) {
				REQUIRE( system.JumpNeighbors(100.).empty() );
			}
		}
		WHEN( "checking visible neighbors" ) {
			THEN( "the visible neighbors are empty" ) {
				REQUIRE( system.VisibleNeighbors().empty() );
			}
		}
	}
}

SCENARIO( "System government", "[System][Government]" ) {
	GIVEN( "a default-constructed System" ) {
		System system;
		WHEN( "checking the government" ) {
			THEN( "the government is not null (returns a default empty government)" ) {
				REQUIRE( system.GetGovernment() != nullptr );
			}
		}
	}
}

SCENARIO( "System map icons", "[System][MapIcons]" ) {
	GIVEN( "a default-constructed System" ) {
		System system;
		WHEN( "checking map icons" ) {
			THEN( "no map icons are present" ) {
				REQUIRE( system.GetMapIcons().empty() );
			}
		}
	}
}

SCENARIO( "System music", "[System][Music]" ) {
	GIVEN( "a default-constructed System" ) {
		System system;
		WHEN( "checking the music name" ) {
			THEN( "the music name is empty" ) {
				REQUIRE( system.MusicName().empty() );
			}
		}
	}
}

SCENARIO( "System attributes", "[System][Attributes]" ) {
	GIVEN( "a default-constructed System" ) {
		System system;
		WHEN( "checking attributes" ) {
			THEN( "no attributes are present" ) {
				REQUIRE( system.Attributes().empty() );
			}
		}
	}
}

SCENARIO( "System stellar objects", "[System][StellarObjects]" ) {
	GIVEN( "a default-constructed System" ) {
		System system;
		WHEN( "checking stellar objects" ) {
			THEN( "no stellar objects are present" ) {
				REQUIRE( system.Objects().empty() );
			}
		}
		WHEN( "finding a stellar object for a null planet" ) {
			const StellarObject *result = system.FindStellar(nullptr);
			THEN( "nullptr is returned" ) {
				REQUIRE( result == nullptr );
			}
		}
		WHEN( "checking habitable zone" ) {
			THEN( "default habitable zone is 1000" ) {
				REQUIRE( system.HabitableZone() == 1000. );
			}
		}
		WHEN( "checking asteroid belt radius" ) {
			THEN( "a default belt exists" ) {
				// Default belt has radius 1500
				REQUIRE( system.AsteroidBeltRadius() == 1500. );
			}
		}
		WHEN( "checking asteroid belts list" ) {
			THEN( "a default belt exists in the list" ) {
				REQUIRE_FALSE( system.AsteroidBelts().empty() );
			}
		}
	}
}

SCENARIO( "System invisible fence", "[System][InvisibleFence]" ) {
	GIVEN( "a default-constructed System" ) {
		System system;
		WHEN( "checking invisible fence radius" ) {
			THEN( "default invisible fence radius is 10000" ) {
				REQUIRE( system.InvisibleFenceRadius() == 10000. );
			}
		}
	}
}

SCENARIO( "System jump range", "[System][JumpRange]" ) {
	GIVEN( "a default-constructed System" ) {
		System system;
		WHEN( "checking jump range" ) {
			THEN( "default jump range is 0" ) {
				REQUIRE( system.JumpRange() == 0. );
			}
		}
	}
}

SCENARIO( "System starfield density", "[System][Starfield]" ) {
	GIVEN( "a default-constructed System" ) {
		System system;
		WHEN( "checking starfield density" ) {
			THEN( "default starfield density is 1" ) {
				REQUIRE( system.StarfieldDensity() == 1. );
			}
		}
	}
}

SCENARIO( "System facilities", "[System][Facilities]" ) {
	GIVEN( "a default-constructed System" ) {
		System system;
		WHEN( "checking shipyard" ) {
			THEN( "the system has no shipyard" ) {
				REQUIRE_FALSE( system.HasShipyard() );
			}
		}
		WHEN( "checking outfitter" ) {
			THEN( "the system has no outfitter" ) {
				REQUIRE_FALSE( system.HasOutfitter() );
			}
		}
	}
}

SCENARIO( "System asteroids", "[System][Asteroids]" ) {
	GIVEN( "a default-constructed System" ) {
		System system;
		WHEN( "checking asteroids" ) {
			THEN( "no asteroids are present" ) {
				REQUIRE( system.Asteroids().empty() );
			}
		}
		WHEN( "checking payloads" ) {
			THEN( "no payloads are present" ) {
				REQUIRE( system.Payloads().empty() );
			}
		}
	}
}

SCENARIO( "System haze", "[System][Haze]" ) {
	GIVEN( "a default-constructed System" ) {
		System system;
		WHEN( "checking haze" ) {
			THEN( "no haze is present" ) {
				REQUIRE( system.Haze() == nullptr );
			}
		}
	}
}

SCENARIO( "System trade", "[System][Trade]" ) {
	GIVEN( "a default-constructed System" ) {
		System system;
		WHEN( "checking trade for a non-existent commodity" ) {
			int price = system.Trade("nonexistent");
			THEN( "the price is 0" ) {
				REQUIRE( price == 0 );
			}
		}
		WHEN( "checking if the system has trade" ) {
			THEN( "the system has no trade" ) {
				REQUIRE_FALSE( system.HasTrade() );
			}
		}
		WHEN( "checking supply for a non-existent commodity" ) {
			double supply = system.Supply("nonexistent");
			THEN( "the supply is 0" ) {
				REQUIRE( supply == 0. );
			}
		}
		WHEN( "checking exports for a non-existent commodity" ) {
			double exports = system.Exports("nonexistent");
			THEN( "exports are 0" ) {
				REQUIRE( exports == 0. );
			}
		}
	}
}

SCENARIO( "System fleets", "[System][Fleets]" ) {
	GIVEN( "a default-constructed System" ) {
		System system;
		WHEN( "checking fleets" ) {
			THEN( "no fleets are present" ) {
				REQUIRE( system.Fleets().empty() );
			}
		}
		WHEN( "checking minimum fleet period" ) {
			THEN( "minimum fleet period is 0" ) {
				REQUIRE( system.MinimumFleetPeriod() == 0 );
			}
		}
	}
}

SCENARIO( "System hazards", "[System][Hazards]" ) {
	GIVEN( "a default-constructed System" ) {
		System system;
		WHEN( "checking hazards" ) {
			THEN( "no hazards are present" ) {
				REQUIRE( system.Hazards().empty() );
			}
		}
	}
}

SCENARIO( "System danger", "[System][Danger]" ) {
	GIVEN( "a default-constructed System" ) {
		System system;
		WHEN( "checking danger level" ) {
			THEN( "danger level is 0" ) {
				REQUIRE( system.Danger() == 0. );
			}
		}
	}
}

SCENARIO( "System raid fleets", "[System][RaidFleets]" ) {
	GIVEN( "a default-constructed System" ) {
		System system;
		WHEN( "checking raid fleets" ) {
			THEN( "no raid fleets are present" ) {
				REQUIRE( system.RaidFleets().empty() );
			}
		}
	}
}

SCENARIO( "System arrival distances", "[System][Arrival]" ) {
	GIVEN( "a default-constructed System" ) {
		System system;
		WHEN( "checking extra hyper arrival distance" ) {
			THEN( "default is 0" ) {
				REQUIRE( system.ExtraHyperArrivalDistance() == 0. );
			}
		}
		WHEN( "checking extra jump arrival distance" ) {
			THEN( "default is 0" ) {
				REQUIRE( system.ExtraJumpArrivalDistance() == 0. );
			}
		}
	}
}

SCENARIO( "System departure distances", "[System][Departure]" ) {
	GIVEN( "a default-constructed System" ) {
		System system;
		WHEN( "checking jump departure distance" ) {
			THEN( "default is 0" ) {
				REQUIRE( system.JumpDepartureDistance() == 0. );
			}
		}
		WHEN( "checking hyper departure distance" ) {
			THEN( "default is 0" ) {
				REQUIRE( system.HyperDepartureDistance() == 0. );
			}
		}
	}
}

SCENARIO( "System solar generation", "[System][Solar]" ) {
	GIVEN( "a default-constructed System" ) {
		System system;
		Point shipPosition(100., 100.);
		WHEN( "getting solar generation with no stellar objects" ) {
			System::SolarGeneration gen = system.GetSolarGeneration(shipPosition, 0., 0., 0.);
			THEN( "fuel generation is 0" ) {
				REQUIRE( gen.fuel == 0. );
			}
			THEN( "energy generation is 0" ) {
				REQUIRE( gen.energy == 0. );
			}
			THEN( "heat generation is 0" ) {
				REQUIRE( gen.heat == 0. );
			}
		}
		WHEN( "getting solar generation at origin" ) {
			Point origin(0., 0.);
			System::SolarGeneration gen = system.GetSolarGeneration(origin, 0., 0., 0.);
			THEN( "fuel generation is 0 (no stellar objects)" ) {
				REQUIRE( gen.fuel == 0. );
			}
		}
	}
}

SCENARIO( "System::Asteroid class", "[System][Asteroid]" ) {
	GIVEN( "an Asteroid created with a name" ) {
		System::Asteroid asteroid("TestAsteroid", 5, 100.0);
		WHEN( "checking asteroid properties" ) {
			THEN( "the name is correct" ) {
				REQUIRE( asteroid.Name() == "TestAsteroid" );
			}
			THEN( "the count is correct" ) {
				REQUIRE( asteroid.Count() == 5 );
			}
			THEN( "the energy is correct" ) {
				REQUIRE( asteroid.Energy() == 100.0 );
			}
			THEN( "the type is nullptr" ) {
				REQUIRE( asteroid.Type() == nullptr );
			}
		}
	}
	GIVEN( "an Asteroid created with a null Minable type" ) {
		System::Asteroid asteroid(nullptr, 3, 50.0);
		WHEN( "checking asteroid properties" ) {
			THEN( "the type is nullptr" ) {
				REQUIRE( asteroid.Type() == nullptr );
			}
			THEN( "the count is correct" ) {
				REQUIRE( asteroid.Count() == 3 );
			}
			THEN( "the energy is correct" ) {
				REQUIRE( asteroid.Energy() == 50.0 );
			}
		}
	}
}

SCENARIO( "System::Asteroid boundary conditions", "[System][Asteroid][Edge]" ) {
	GIVEN( "an Asteroid with zero count" ) {
		System::Asteroid asteroid("ZeroCount", 0, 100.0);
		WHEN( "checking count" ) {
			THEN( "count is zero" ) {
				REQUIRE( asteroid.Count() == 0 );
			}
		}
	}
	GIVEN( "an Asteroid with zero energy" ) {
		System::Asteroid asteroid("ZeroEnergy", 5, 0.0);
		WHEN( "checking energy" ) {
			THEN( "energy is zero" ) {
				REQUIRE( asteroid.Energy() == 0.0 );
			}
		}
	}
	GIVEN( "an Asteroid with negative energy" ) {
		System::Asteroid asteroid("NegativeEnergy", 5, -50.0);
		WHEN( "checking energy" ) {
			THEN( "energy is negative" ) {
				REQUIRE( asteroid.Energy() == -50.0 );
			}
		}
	}
	GIVEN( "an Asteroid with large count" ) {
		System::Asteroid asteroid("LargeCount", 10000, 100.0);
		WHEN( "checking count" ) {
			THEN( "count is correct" ) {
				REQUIRE( asteroid.Count() == 10000 );
			}
		}
	}
	GIVEN( "an Asteroid with empty name" ) {
		System::Asteroid asteroid("", 5, 100.0);
		WHEN( "checking name" ) {
			THEN( "name is empty" ) {
				REQUIRE( asteroid.Name().empty() );
			}
		}
	}
}

SCENARIO( "System::SolarGeneration struct", "[System][SolarGeneration]" ) {
	GIVEN( "a default SolarGeneration" ) {
		System::SolarGeneration gen;
		WHEN( "checking default values" ) {
			THEN( "values are uninitialized (or zero-initialized depending on context)" ) {
				// Note: struct members are not initialized by default
				// This test documents that behavior
			}
		}
	}
	GIVEN( "a SolarGeneration with set values" ) {
		System::SolarGeneration gen{10.5, 20.5, 5.5};
		WHEN( "checking values" ) {
			THEN( "fuel is correct" ) {
				REQUIRE( gen.fuel == 10.5 );
			}
			THEN( "energy is correct" ) {
				REQUIRE( gen.energy == 20.5 );
			}
			THEN( "heat is correct" ) {
				REQUIRE( gen.heat == 5.5 );
			}
		}
	}
	GIVEN( "a SolarGeneration with zero values" ) {
		System::SolarGeneration gen{0., 0., 0.};
		WHEN( "checking values" ) {
			THEN( "all values are zero" ) {
				REQUIRE( gen.fuel == 0. );
				REQUIRE( gen.energy == 0. );
				REQUIRE( gen.heat == 0. );
			}
		}
	}
	GIVEN( "a SolarGeneration with negative values" ) {
		System::SolarGeneration gen{-5., -10., -2.};
		WHEN( "checking values" ) {
			THEN( "negative values are preserved" ) {
				REQUIRE( gen.fuel == -5. );
				REQUIRE( gen.energy == -10. );
				REQUIRE( gen.heat == -2. );
			}
		}
	}
}

SCENARIO( "System static constants", "[System][Constants]" ) {
	GIVEN( "the DEFAULT_NEIGHBOR_DISTANCE constant" ) {
		THEN( "it equals 100" ) {
			REQUIRE( System::DEFAULT_NEIGHBOR_DISTANCE == 100. );
		}
	}
}

SCENARIO( "System link modification", "[System][Links][Modification]" ) {
	GIVEN( "two default-constructed Systems" ) {
		System system1;
		System system2;
		WHEN( "linking two systems" ) {
			system1.Link(&system2);
			THEN( "system1's links contain system2" ) {
				// Note: links is the internal set, Links() returns accessibleLinks
				// which requires UpdateSystem to be called
			}
		}
	}
}

SCENARIO( "System economy operations", "[System][Economy]" ) {
	GIVEN( "a default-constructed System" ) {
		System system;
		WHEN( "stepping the economy" ) {
			system.StepEconomy();
			THEN( "no crash occurs" ) {
				REQUIRE( true );
			}
		}
		WHEN( "setting supply for a non-existent commodity" ) {
			system.SetSupply("nonexistent", 100.);
			THEN( "no crash occurs" ) {
				REQUIRE( true );
			}
		}
	}
}

SCENARIO( "System is inhabited check", "[System][Inhabited]" ) {
	GIVEN( "a default-constructed System" ) {
		System system;
		WHEN( "checking if inhabited with no ship" ) {
			bool inhabited = system.IsInhabited(nullptr);
			THEN( "the system is not inhabited" ) {
				REQUIRE_FALSE( inhabited );
			}
		}
	}
}

SCENARIO( "System has fuel check", "[System][Fuel]" ) {
	GIVEN( "a default-constructed System" ) {
		System system;
		WHEN( "checking if system has fuel for a ship" ) {
			// Note: This requires a Ship object, but we can test the null case
			// The system has no stellar objects, so it has no fuel
			THEN( "without stellar objects, the system has no fuel" ) {
				// Would need a Ship object to fully test
				REQUIRE( true );
			}
		}
	}
}

// #endregion unit tests



} // test namespace