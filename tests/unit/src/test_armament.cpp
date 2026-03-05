/* test_armament.cpp
Copyright (c) 2021 by Benjamin Hauch

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
#include "../../../source/Armament.h"

// ... and any system includes needed for the test file.
#include "../../../source/Hardpoint.h"
#include "../../../source/Point.h"

#include <memory>
#include <type_traits>

namespace { // test namespace

// #region mock data



// #endregion mock data



// #region unit tests
SCENARIO( "Creating an Armament instance", "[armament]" ) {
	GIVEN( "a default-constructed Armament" ) {
		Armament armament;
		THEN( "it has no hardpoints" ) {
			CHECK( armament.Get().empty() );
			CHECK( armament.GunCount() == 0 );
			CHECK( armament.TurretCount() == 0 );
		}
		THEN( "there is no restockable ammo" ) {
			CHECK( armament.RestockableAmmo().empty() );
		}
	}
}

SCENARIO( "Adding gun ports to an Armament", "[armament]" ) {
	GIVEN( "an empty Armament" ) {
		Armament armament;
		WHEN( "a gun port is added" ) {
			Point point(10., 20.);
			Hardpoint::BaseAttributes attrs;
			armament.AddGunPort(point, attrs);
			THEN( "the gun count increases" ) {
				CHECK( armament.GunCount() == 1 );
				CHECK( armament.TurretCount() == 0 );
			}
			THEN( "the hardpoint is accessible" ) {
				CHECK( armament.Get().size() == 1 );
				CHECK_FALSE( armament.Get()[0].IsTurret() );
			}
		}
		WHEN( "multiple gun ports are added" ) {
			Hardpoint::BaseAttributes attrs;
			armament.AddGunPort(Point(10., 0.), attrs);
			armament.AddGunPort(Point(-10., 0.), attrs);
			armament.AddGunPort(Point(0., 20.), attrs);
			THEN( "the gun count reflects all additions" ) {
				CHECK( armament.GunCount() == 3 );
				CHECK( armament.TurretCount() == 0 );
			}
		}
	}
}

SCENARIO( "Adding turrets to an Armament", "[armament]" ) {
	GIVEN( "an empty Armament" ) {
		Armament armament;
		WHEN( "a turret is added" ) {
			Point point(15., 25.);
			Hardpoint::BaseAttributes attrs;
			armament.AddTurret(point, attrs);
			THEN( "the turret count increases" ) {
				CHECK( armament.GunCount() == 0 );
				CHECK( armament.TurretCount() == 1 );
			}
			THEN( "the hardpoint is a turret" ) {
				CHECK( armament.Get().size() == 1 );
				CHECK( armament.Get()[0].IsTurret() );
			}
		}
		WHEN( "multiple turrets are added" ) {
			Hardpoint::BaseAttributes attrs;
			armament.AddTurret(Point(10., 0.), attrs);
			armament.AddTurret(Point(-10., 0.), attrs);
			THEN( "the turret count reflects all additions" ) {
				CHECK( armament.GunCount() == 0 );
				CHECK( armament.TurretCount() == 2 );
			}
		}
	}
}

SCENARIO( "Adding mixed hardpoints to an Armament", "[armament]" ) {
	GIVEN( "an empty Armament" ) {
		Armament armament;
		WHEN( "both guns and turrets are added" ) {
			Hardpoint::BaseAttributes attrs;
			armament.AddGunPort(Point(10., 0.), attrs);
			armament.AddTurret(Point(0., 10.), attrs);
			armament.AddGunPort(Point(-10., 0.), attrs);
			armament.AddTurret(Point(0., -10.), attrs);
			THEN( "counts are correct" ) {
				CHECK( armament.GunCount() == 2 );
				CHECK( armament.TurretCount() == 2 );
				CHECK( armament.Get().size() == 4 );
			}
		}
	}
}

SCENARIO( "Swapping weapons in hardpoints", "[armament]" ) {
	GIVEN( "an Armament with two gun ports" ) {
		Armament armament;
		Hardpoint::BaseAttributes attrs;
		armament.AddGunPort(Point(10., 0.), attrs);
		armament.AddGunPort(Point(-10., 0.), attrs);
		REQUIRE( armament.GunCount() == 2 );
		REQUIRE( armament.Get().size() == 2 );

		WHEN( "swapping two valid gun ports" ) {
			armament.Swap(0, 1);
			THEN( "the hardpoints remain unchanged (both empty)" ) {
				CHECK_FALSE( armament.Get()[0].GetOutfit() );
				CHECK_FALSE( armament.Get()[1].GetOutfit() );
			}
		}
		WHEN( "attempting to swap with an invalid index" ) {
			armament.Swap(0, 5);
			THEN( "nothing happens" ) {
				CHECK( armament.Get().size() == 2 );
			}
		}
		WHEN( "attempting to swap the same index" ) {
			armament.Swap(0, 0);
			THEN( "nothing happens" ) {
				CHECK( armament.Get().size() == 2 );
			}
		}
	}
	GIVEN( "an Armament with one gun and one turret" ) {
		Armament armament;
		Hardpoint::BaseAttributes attrs;
		armament.AddGunPort(Point(10., 0.), attrs);
		armament.AddTurret(Point(0., 10.), attrs);
		REQUIRE( armament.GunCount() == 1 );
		REQUIRE( armament.TurretCount() == 1 );

		WHEN( "attempting to swap gun and turret" ) {
			armament.Swap(0, 1);
			THEN( "the swap does not occur (different types)" ) {
				CHECK_FALSE( armament.Get()[0].IsTurret() );
				CHECK( armament.Get()[1].IsTurret() );
			}
		}
	}
}

SCENARIO( "Uninstalling all weapons", "[armament]" ) {
	GIVEN( "an Armament with hardpoints" ) {
		Armament armament;
		Hardpoint::BaseAttributes attrs;
		armament.AddGunPort(Point(10., 0.), attrs);
		armament.AddTurret(Point(0., 10.), attrs);
		REQUIRE( armament.Get().size() == 2 );

		WHEN( "UninstallAll is called" ) {
			armament.UninstallAll();
			THEN( "all hardpoints are empty but still present" ) {
				CHECK( armament.Get().size() == 2 );
				CHECK_FALSE( armament.Get()[0].GetOutfit() );
				CHECK_FALSE( armament.Get()[1].GetOutfit() );
			}
		}
	}
}

SCENARIO( "Reloading all weapons", "[armament]" ) {
	GIVEN( "an Armament with hardpoints" ) {
		Armament armament;
		Hardpoint::BaseAttributes attrs;
		armament.AddGunPort(Point(10., 0.), attrs);
		armament.AddTurret(Point(0., 10.), attrs);

		WHEN( "ReloadAll is called" ) {
			// This should not throw, even with empty hardpoints
			REQUIRE_NOTHROW( armament.ReloadAll() );
			THEN( "the hardpoints remain" ) {
				CHECK( armament.Get().size() == 2 );
			}
		}
	}
}

SCENARIO( "Finishing loading an Armament", "[armament]" ) {
	GIVEN( "an Armament with hardpoints" ) {
		Armament armament;
		Hardpoint::BaseAttributes attrs;
		armament.AddGunPort(Point(10., 0.), attrs);
		armament.AddTurret(Point(0., 10.), attrs);

		WHEN( "FinishLoading is called" ) {
			// This should not throw, even with empty hardpoints
			REQUIRE_NOTHROW( armament.FinishLoading() );
			THEN( "the hardpoints remain" ) {
				CHECK( armament.Get().size() == 2 );
			}
		}
	}
}

SCENARIO( "Getting restockable ammo from an empty Armament", "[armament]" ) {
	GIVEN( "an Armament with empty hardpoints" ) {
		Armament armament;
		Hardpoint::BaseAttributes attrs;
		armament.AddGunPort(Point(10., 0.), attrs);
		armament.AddTurret(Point(0., 10.), attrs);

		THEN( "no restockable ammo is returned" ) {
			CHECK( armament.RestockableAmmo().empty() );
		}
	}
}

SCENARIO( "An Armament instance is being copied", "[armament]" ) {
	GIVEN( "an Armament with hardpoints" ) {
		Armament armament;
		Hardpoint::BaseAttributes attrs;
		armament.AddGunPort(Point(10., 0.), attrs);
		armament.AddTurret(Point(0., 10.), attrs);

		WHEN( "the copy is made" ) {
			Armament copy = armament;
			THEN( "the copy has the correct hardpoints" ) {
				CHECK( copy.Get().size() == armament.Get().size() );
				CHECK( copy.GunCount() == armament.GunCount() );
				CHECK( copy.TurretCount() == armament.TurretCount() );
			}
			THEN( "the two armaments are independent" ) {
				copy.AddGunPort(Point(5., 5.), attrs);
				CHECK( copy.Get().size() == 3 );
				CHECK( armament.Get().size() == 2 );
			}
		}
	}
}

SCENARIO( "Adding outfit with nullptr", "[armament]" ) {
	GIVEN( "an Armament with a gun port" ) {
		Armament armament;
		Hardpoint::BaseAttributes attrs;
		armament.AddGunPort(Point(10., 0.), attrs);

		WHEN( "adding nullptr outfit" ) {
			int result = armament.Add(nullptr, 1);
			THEN( "nothing is added" ) {
				CHECK( result == 0 );
				CHECK_FALSE( armament.Get()[0].GetOutfit() );
			}
		}
	}
}

SCENARIO( "Adding outfit with zero count", "[armament]" ) {
	GIVEN( "an Armament with a gun port" ) {
		Armament armament;
		Hardpoint::BaseAttributes attrs;
		armament.AddGunPort(Point(10., 0.), attrs);

		WHEN( "adding outfit with count 0" ) {
			int result = armament.Add(nullptr, 0);
			THEN( "nothing is added" ) {
				CHECK( result == 0 );
			}
		}
	}
}

SCENARIO( "Hardpoint ordering after multiple additions", "[armament]" ) {
	GIVEN( "an empty Armament" ) {
		Armament armament;
		Hardpoint::BaseAttributes attrs;

		WHEN( "adding hardpoints in specific order" ) {
			armament.AddGunPort(Point(1., 0.), attrs);
			armament.AddTurret(Point(2., 0.), attrs);
			armament.AddGunPort(Point(3., 0.), attrs);
			armament.AddTurret(Point(4., 0.), attrs);

			THEN( "hardpoints maintain order of addition" ) {
				REQUIRE( armament.Get().size() == 4 );
				CHECK_FALSE( armament.Get()[0].IsTurret() );
				CHECK( armament.Get()[1].IsTurret() );
				CHECK_FALSE( armament.Get()[2].IsTurret() );
				CHECK( armament.Get()[3].IsTurret() );
			}
		}
	}
}

// #endregion unit tests



} // test namespace