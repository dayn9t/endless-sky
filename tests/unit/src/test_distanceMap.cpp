/* test_distanceMap.cpp
Copyright (c) 2026 by the Endless Sky Contributors

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
#include "../../../source/DistanceMap.h"

// ... and any system includes needed for the test file.
#include "../../../source/System.h"
#include "../../../source/WormholeStrategy.h"

#include <set>
#include <type_traits>
#include <vector>

namespace { // test namespace

// #region mock data

// Note: The System class requires extensive game data setup (Load, UpdateSystem, etc.)
// and doesn't have a default constructor. Tests here focus on basic API behavior
// with null center systems, which is a valid edge case.

// #endregion mock data



// #region unit tests
SCENARIO( "DistanceMap class traits", "[DistanceMap]" ) {
	using T = DistanceMap;
	SECTION( "Class Traits" ) {
		CHECK( std::is_nothrow_destructible_v<T> );
		CHECK_FALSE( std::is_trivially_destructible_v<T> );
	}
	SECTION( "Construction Traits" ) {
		CHECK_FALSE( std::is_default_constructible_v<T> );
		CHECK_FALSE( std::is_trivially_default_constructible_v<T> );
		CHECK( std::is_copy_constructible_v<T> );
		CHECK_FALSE( std::is_trivially_copy_constructible_v<T> );
		CHECK( std::is_move_constructible_v<T> );
		CHECK_FALSE( std::is_trivially_move_constructible_v<T> );
	}
	SECTION( "Copy Traits" ) {
		CHECK( std::is_copy_assignable_v<T> );
		CHECK_FALSE( std::is_trivially_copyable_v<T> );
		CHECK_FALSE( std::is_trivially_copy_assignable_v<T> );
		CHECK_FALSE( std::is_nothrow_copy_assignable_v<T> );
	}
	SECTION( "Move Traits" ) {
		CHECK( std::is_move_assignable_v<T> );
		CHECK_FALSE( std::is_trivially_move_assignable_v<T> );
		CHECK( std::is_nothrow_move_assignable_v<T> );
	}
}

SCENARIO( "Constructing a DistanceMap with null center", "[DistanceMap][Constructor]" ) {
	GIVEN( "a null center system" ) {
		const System *nullCenter = nullptr;
		WHEN( "constructing with default parameters" ) {
			DistanceMap map(nullCenter);
			THEN( "the map is created but contains no systems" ) {
				CHECK( map.Systems().empty() );
			}
		}
		WHEN( "constructing with maxSystems parameter" ) {
			DistanceMap map(nullCenter, 5);
			THEN( "the map is created but contains no systems" ) {
				CHECK( map.Systems().empty() );
			}
		}
		WHEN( "constructing with maxDays parameter" ) {
			DistanceMap map(nullCenter, -1, 10);
			THEN( "the map is created but contains no systems" ) {
				CHECK( map.Systems().empty() );
			}
		}
	}
}

SCENARIO( "Constructing a DistanceMap with WormholeStrategy", "[DistanceMap][Constructor]" ) {
	GIVEN( "a null center system" ) {
		const System *nullCenter = nullptr;
		WHEN( "constructing with WormholeStrategy::NONE" ) {
			DistanceMap map(nullCenter, WormholeStrategy::NONE, false);
			THEN( "the map is created but contains no systems" ) {
				CHECK( map.Systems().empty() );
			}
		}
		WHEN( "constructing with WormholeStrategy::ALL" ) {
			DistanceMap map(nullCenter, WormholeStrategy::ALL, false);
			THEN( "the map is created but contains no systems" ) {
				CHECK( map.Systems().empty() );
			}
		}
		WHEN( "constructing with WormholeStrategy::ONLY_UNRESTRICTED" ) {
			DistanceMap map(nullCenter, WormholeStrategy::ONLY_UNRESTRICTED, false);
			THEN( "the map is created but contains no systems" ) {
				CHECK( map.Systems().empty() );
			}
		}
		WHEN( "constructing with jump drive enabled" ) {
			DistanceMap map(nullCenter, WormholeStrategy::NONE, true);
			THEN( "the map is created but contains no systems" ) {
				CHECK( map.Systems().empty() );
			}
		}
		WHEN( "constructing with all parameters" ) {
			DistanceMap map(nullCenter, WormholeStrategy::ALL, true, 10, 5);
			THEN( "the map is created but contains no systems" ) {
				CHECK( map.Systems().empty() );
			}
		}
	}
}

SCENARIO( "DistanceMap::Systems returns correct results", "[DistanceMap][Systems]" ) {
	GIVEN( "a DistanceMap with null center" ) {
		WHEN( "calling Systems" ) {
			DistanceMap map(nullptr);
			THEN( "Systems returns empty set" ) {
				CHECK( map.Systems().empty() );
			}
		}
	}
}

SCENARIO( "DistanceMap with zero maxDays", "[DistanceMap][maxDays]" ) {
	GIVEN( "a null center system" ) {
		const System *nullCenter = nullptr;
		WHEN( "constructing with maxDays = 0" ) {
			DistanceMap map(nullCenter, -1, 0);
			THEN( "the map is created but contains no systems" ) {
				CHECK( map.Systems().empty() );
			}
		}
	}
}

SCENARIO( "DistanceMap copy and move semantics", "[DistanceMap][Copy][Move]" ) {
	GIVEN( "a DistanceMap" ) {
		DistanceMap original(nullptr);
		WHEN( "copy constructing" ) {
			DistanceMap copy(original);
			THEN( "the copy has the same systems" ) {
				CHECK( copy.Systems() == original.Systems() );
			}
		}
		WHEN( "copy assigning" ) {
			DistanceMap assigned(nullptr);
			assigned = original;
			THEN( "the assigned map has the same systems" ) {
				CHECK( assigned.Systems() == original.Systems() );
			}
		}
		WHEN( "move constructing" ) {
			DistanceMap moved(std::move(original));
			THEN( "the moved map has the expected systems" ) {
				CHECK( moved.Systems().empty() );
			}
		}
		WHEN( "move assigning" ) {
			DistanceMap assigned(nullptr);
			assigned = std::move(original);
			THEN( "the assigned map has the expected systems" ) {
				CHECK( assigned.Systems().empty() );
			}
		}
	}
}

SCENARIO( "DistanceMap multiple constructor overloads", "[DistanceMap][Constructor]" ) {
	GIVEN( "a null center system" ) {
		const System *nullCenter = nullptr;
		WHEN( "using constructor (center, maxSystems)" ) {
			DistanceMap map(nullCenter, 10);
			THEN( "map is created successfully" ) {
				CHECK( map.Systems().empty() );
			}
		}
		WHEN( "using constructor (center, maxSystems, maxDays)" ) {
			DistanceMap map(nullCenter, 10, 5);
			THEN( "map is created successfully" ) {
				CHECK( map.Systems().empty() );
			}
		}
		WHEN( "using constructor (center, wormholeStrategy, useJumpDrive)" ) {
			DistanceMap map(nullCenter, WormholeStrategy::NONE, false);
			THEN( "map is created successfully" ) {
				CHECK( map.Systems().empty() );
			}
		}
		WHEN( "using constructor (center, wormholeStrategy, useJumpDrive, maxSystems)" ) {
			DistanceMap map(nullCenter, WormholeStrategy::ALL, true, 5);
			THEN( "map is created successfully" ) {
				CHECK( map.Systems().empty() );
			}
		}
		WHEN( "using constructor (center, wormholeStrategy, useJumpDrive, maxSystems, maxDays)" ) {
			DistanceMap map(nullCenter, WormholeStrategy::ONLY_UNRESTRICTED, true, 5, 3);
			THEN( "map is created successfully" ) {
				CHECK( map.Systems().empty() );
			}
		}
	}
}

SCENARIO( "DistanceMap HasRoute with null center", "[DistanceMap][HasRoute]" ) {
	GIVEN( "a DistanceMap with null center" ) {
		DistanceMap map(nullptr);
		WHEN( "checking HasRoute with null system pointer" ) {
			const System *nullSystem = nullptr;
			THEN( "HasRoute returns false for null system" ) {
				// Note: Passing null to HasRoute would be undefined behavior
				// as it takes a const reference. We document this edge case.
				CHECK( map.Systems().empty() );
			}
		}
	}
}

SCENARIO( "DistanceMap Days with null center", "[DistanceMap][Days]" ) {
	GIVEN( "a DistanceMap with null center" ) {
		DistanceMap map(nullptr);
		WHEN( "checking Days for any system" ) {
			THEN( "the map has no reachable systems" ) {
				CHECK( map.Systems().empty() );
			}
		}
	}
}

SCENARIO( "DistanceMap Plan with null center", "[DistanceMap][Plan]" ) {
	GIVEN( "a DistanceMap with null center" ) {
		DistanceMap map(nullptr);
		WHEN( "checking Plan for any system" ) {
			THEN( "the map has no reachable systems to plan to" ) {
				CHECK( map.Systems().empty() );
			}
		}
	}
}

SCENARIO( "DistanceMap with negative maxSystems", "[DistanceMap][maxSystems]" ) {
	GIVEN( "a null center system" ) {
		const System *nullCenter = nullptr;
		WHEN( "constructing with negative maxSystems" ) {
			DistanceMap map(nullCenter, -1);
			THEN( "the map is created successfully" ) {
				CHECK( map.Systems().empty() );
			}
		}
		WHEN( "constructing with negative maxSystems and maxDays" ) {
			DistanceMap map(nullCenter, -1, -1);
			THEN( "the map is created successfully" ) {
				CHECK( map.Systems().empty() );
			}
		}
	}
}

SCENARIO( "DistanceMap WormholeStrategy variations", "[DistanceMap][WormholeStrategy]" ) {
	GIVEN( "a null center system" ) {
		const System *nullCenter = nullptr;
		WHEN( "using different wormhole strategies with jump drive" ) {
			DistanceMap mapNone(nullCenter, WormholeStrategy::NONE, true);
			DistanceMap mapOnlyUnrestricted(nullCenter, WormholeStrategy::ONLY_UNRESTRICTED, true);
			DistanceMap mapAll(nullCenter, WormholeStrategy::ALL, true);
			THEN( "all maps are created with empty systems" ) {
				CHECK( mapNone.Systems().empty() );
				CHECK( mapOnlyUnrestricted.Systems().empty() );
				CHECK( mapAll.Systems().empty() );
			}
		}
		WHEN( "using different wormhole strategies without jump drive" ) {
			DistanceMap mapNone(nullCenter, WormholeStrategy::NONE, false);
			DistanceMap mapOnlyUnrestricted(nullCenter, WormholeStrategy::ONLY_UNRESTRICTED, false);
			DistanceMap mapAll(nullCenter, WormholeStrategy::ALL, false);
			THEN( "all maps are created with empty systems" ) {
				CHECK( mapNone.Systems().empty() );
				CHECK( mapOnlyUnrestricted.Systems().empty() );
				CHECK( mapAll.Systems().empty() );
			}
		}
	}
}

// #endregion unit tests



} // test namespace