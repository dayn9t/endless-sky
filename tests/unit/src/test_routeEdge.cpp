/* test_routeEdge.cpp
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
#include "../../../source/RouteEdge.h"

// ... and any system includes needed for the test file.
#include <type_traits>

namespace { // test namespace

// #region mock data

// Note: The System class requires extensive game data setup.
// For RouteEdge tests, we only need the System pointer for identity comparison,
// not actual System functionality. RouteEdge does not dereference the pointer,
// so we can use any valid memory address for testing.

// A simple struct to provide a valid memory address for System pointer tests.
// The content doesn't matter since RouteEdge only stores and compares pointers.
struct MockSystemPlaceholder { int unused; };

// #endregion mock data



// #region unit tests
SCENARIO( "RouteEdge class traits", "[RouteEdge]" ) {
	using T = RouteEdge;
	SECTION( "Class Traits" ) {
		CHECK( std::is_nothrow_destructible_v<T> );
		CHECK( std::is_trivially_destructible_v<T> );
	}
	SECTION( "Construction Traits" ) {
		CHECK( std::is_default_constructible_v<T> );
		CHECK( std::is_trivially_default_constructible_v<T> );
		CHECK( std::is_copy_constructible_v<T> );
		CHECK( std::is_trivially_copy_constructible_v<T> );
		CHECK( std::is_move_constructible_v<T> );
		CHECK( std::is_trivially_move_constructible_v<T> );
	}
	SECTION( "Copy Traits" ) {
		CHECK( std::is_copy_assignable_v<T> );
		CHECK( std::is_trivially_copyable_v<T> );
		CHECK( std::is_trivially_copy_assignable_v<T> );
		CHECK( std::is_nothrow_copy_assignable_v<T> );
	}
	SECTION( "Move Traits" ) {
		CHECK( std::is_move_assignable_v<T> );
		CHECK( std::is_trivially_move_assignable_v<T> );
		CHECK( std::is_nothrow_move_assignable_v<T> );
	}
}

SCENARIO( "Constructing a RouteEdge with default constructor", "[RouteEdge][Constructor]" ) {
	GIVEN( "no arguments" ) {
		WHEN( "constructing a RouteEdge" ) {
			RouteEdge edge;
			THEN( "all members have default values" ) {
				CHECK( edge.prev == nullptr );
				CHECK( edge.fuel == 0 );
				CHECK( edge.days == 0 );
				CHECK( edge.danger == 0. );
			}
		}
	}
}

SCENARIO( "Constructing a RouteEdge with null system", "[RouteEdge][Constructor]" ) {
	GIVEN( "a null system pointer" ) {
		const System *nullSystem = nullptr;
		WHEN( "constructing a RouteEdge" ) {
			RouteEdge edge(nullSystem);
			THEN( "prev is nullptr and other members have default values" ) {
				CHECK( edge.prev == nullptr );
				CHECK( edge.fuel == 0 );
				CHECK( edge.days == 0 );
				CHECK( edge.danger == 0. );
			}
		}
	}
}

SCENARIO( "Constructing a RouteEdge with a system pointer", "[RouteEdge][Constructor]" ) {
	GIVEN( "a valid system pointer" ) {
		// Use a mock placeholder to get a valid memory address.
		// RouteEdge only stores the pointer, never dereferences it.
		MockSystemPlaceholder placeholder;
		const System *systemPtr = reinterpret_cast<const System*>(&placeholder);
		WHEN( "constructing a RouteEdge with the system pointer" ) {
			RouteEdge edge(systemPtr);
			THEN( "prev points to the given system" ) {
				CHECK( edge.prev == systemPtr );
				CHECK( edge.fuel == 0 );
				CHECK( edge.days == 0 );
				CHECK( edge.danger == 0. );
			}
		}
	}
}

SCENARIO( "RouteEdge member modification", "[RouteEdge][Members]" ) {
	GIVEN( "a default constructed RouteEdge" ) {
		RouteEdge edge;
		WHEN( "modifying fuel" ) {
			edge.fuel = 100;
			THEN( "fuel is updated" ) {
				CHECK( edge.fuel == 100 );
				CHECK( edge.days == 0 );
				CHECK( edge.danger == 0. );
			}
		}
		WHEN( "modifying days" ) {
			edge.days = 5;
			THEN( "days is updated" ) {
				CHECK( edge.fuel == 0 );
				CHECK( edge.days == 5 );
				CHECK( edge.danger == 0. );
			}
		}
		WHEN( "modifying danger" ) {
			edge.danger = 2.5;
			THEN( "danger is updated" ) {
				CHECK( edge.fuel == 0 );
				CHECK( edge.days == 0 );
				CHECK( edge.danger == 2.5 );
			}
		}
		WHEN( "modifying all members" ) {
			MockSystemPlaceholder placeholder;
			edge.prev = reinterpret_cast<const System*>(&placeholder);
			edge.fuel = 50;
			edge.days = 3;
			edge.danger = 1.5;
			THEN( "all members are updated" ) {
				CHECK( edge.prev == reinterpret_cast<const System*>(&placeholder) );
				CHECK( edge.fuel == 50 );
				CHECK( edge.days == 3 );
				CHECK( edge.danger == 1.5 );
			}
		}
	}
}

SCENARIO( "RouteEdge copy semantics", "[RouteEdge][Copy]" ) {
	GIVEN( "a RouteEdge with modified values" ) {
		MockSystemPlaceholder placeholder;
		RouteEdge original(reinterpret_cast<const System*>(&placeholder));
		original.fuel = 100;
		original.days = 10;
		original.danger = 5.5;
		WHEN( "copy constructing" ) {
			RouteEdge copy(original);
			THEN( "the copy has the same values" ) {
				CHECK( copy.prev == original.prev );
				CHECK( copy.fuel == original.fuel );
				CHECK( copy.days == original.days );
				CHECK( copy.danger == original.danger );
			}
		}
		WHEN( "copy assigning" ) {
			RouteEdge assigned;
			assigned = original;
			THEN( "the assigned edge has the same values" ) {
				CHECK( assigned.prev == original.prev );
				CHECK( assigned.fuel == original.fuel );
				CHECK( assigned.days == original.days );
				CHECK( assigned.danger == original.danger );
			}
		}
	}
}

SCENARIO( "RouteEdge move semantics", "[RouteEdge][Move]" ) {
	GIVEN( "a RouteEdge with modified values" ) {
		MockSystemPlaceholder placeholder;
		RouteEdge original(reinterpret_cast<const System*>(&placeholder));
		original.fuel = 100;
		original.days = 10;
		original.danger = 5.5;
		WHEN( "move constructing" ) {
			RouteEdge moved(std::move(original));
			THEN( "the moved edge has the same values" ) {
				CHECK( moved.prev == reinterpret_cast<const System*>(&placeholder) );
				CHECK( moved.fuel == 100 );
				CHECK( moved.days == 10 );
				CHECK( moved.danger == 5.5 );
			}
		}
		WHEN( "move assigning" ) {
			RouteEdge assigned;
			assigned = std::move(original);
			THEN( "the assigned edge has the same values" ) {
				CHECK( assigned.prev == reinterpret_cast<const System*>(&placeholder) );
				CHECK( assigned.fuel == 100 );
				CHECK( assigned.days == 10 );
				CHECK( assigned.danger == 5.5 );
			}
		}
	}
}

SCENARIO( "RouteEdge comparison operator - fuel priority", "[RouteEdge][operator<]" ) {
	GIVEN( "two RouteEdges with different fuel values" ) {
		RouteEdge lowerFuel;
		lowerFuel.fuel = 50;
		lowerFuel.days = 10;
		lowerFuel.danger = 0.;

		RouteEdge higherFuel;
		higherFuel.fuel = 100;
		higherFuel.days = 10;
		higherFuel.danger = 0.;

		WHEN( "comparing edges with different fuel (same days and danger)" ) {
			THEN( "the edge with lower fuel has higher priority (operator< returns false)" ) {
				// lowerFuel < higherFuel means: is lowerFuel lower priority?
				// Lower fuel = higher priority, so lowerFuel is NOT lower priority
				CHECK_FALSE( lowerFuel < higherFuel );
				// Higher fuel = lower priority, so higherFuel IS lower priority
				CHECK( higherFuel < lowerFuel );
			}
		}
	}
}

SCENARIO( "RouteEdge comparison operator - days priority", "[RouteEdge][operator<]" ) {
	GIVEN( "two RouteEdges with same fuel but different days" ) {
		RouteEdge lowerDays;
		lowerDays.fuel = 100;
		lowerDays.days = 5;
		lowerDays.danger = 0.;

		RouteEdge higherDays;
		higherDays.fuel = 100;
		higherDays.days = 10;
		higherDays.danger = 0.;

		WHEN( "comparing edges with same fuel but different days" ) {
			THEN( "the edge with lower days has higher priority" ) {
				CHECK_FALSE( lowerDays < higherDays );
				CHECK( higherDays < lowerDays );
			}
		}
	}
}

SCENARIO( "RouteEdge comparison operator - danger priority", "[RouteEdge][operator<]" ) {
	GIVEN( "two RouteEdges with same fuel and days but different danger" ) {
		RouteEdge lowerDanger;
		lowerDanger.fuel = 100;
		lowerDanger.days = 10;
		lowerDanger.danger = 1.;

		RouteEdge higherDanger;
		higherDanger.fuel = 100;
		higherDanger.days = 10;
		higherDanger.danger = 5.;

		WHEN( "comparing edges with same fuel and days but different danger" ) {
			THEN( "the edge with lower danger has higher priority" ) {
				CHECK_FALSE( lowerDanger < higherDanger );
				CHECK( higherDanger < lowerDanger );
			}
		}
	}
}

SCENARIO( "RouteEdge comparison operator - equal values", "[RouteEdge][operator<]" ) {
	GIVEN( "two RouteEdges with identical values" ) {
		RouteEdge edge1;
		edge1.fuel = 100;
		edge1.days = 10;
		edge1.danger = 5.;

		RouteEdge edge2;
		edge2.fuel = 100;
		edge2.days = 10;
		edge2.danger = 5.;

		WHEN( "comparing edges with identical values" ) {
			THEN( "neither is less than the other" ) {
				CHECK_FALSE( edge1 < edge2 );
				CHECK_FALSE( edge2 < edge1 );
			}
		}
	}
}

SCENARIO( "RouteEdge comparison operator - priority order", "[RouteEdge][operator<]" ) {
	GIVEN( "multiple RouteEdges with different priority attributes" ) {
		RouteEdge best;
		best.fuel = 50;
		best.days = 5;
		best.danger = 1.;

		RouteEdge medium;
		medium.fuel = 50;
		medium.days = 5;
		medium.danger = 5.;

		RouteEdge worst;
		worst.fuel = 100;
		worst.days = 10;
		worst.danger = 10.;

		WHEN( "comparing best vs medium (same fuel and days, different danger)" ) {
			THEN( "best has higher priority (lower danger)" ) {
				CHECK_FALSE( best < medium );
				CHECK( medium < best );
			}
		}
		WHEN( "comparing best vs worst" ) {
			THEN( "best has higher priority" ) {
				CHECK_FALSE( best < worst );
				CHECK( worst < best );
			}
		}
		WHEN( "comparing medium vs worst (different fuel)" ) {
			THEN( "medium has higher priority (lower fuel)" ) {
				CHECK_FALSE( medium < worst );
				CHECK( worst < medium );
			}
		}
	}
}

SCENARIO( "RouteEdge comparison operator - fuel takes precedence", "[RouteEdge][operator<]" ) {
	GIVEN( "edges where fuel difference should override days and danger" ) {
		RouteEdge lowFuelHighDays;
		lowFuelHighDays.fuel = 50;
		lowFuelHighDays.days = 100;
		lowFuelHighDays.danger = 100.;

		RouteEdge highFuelLowDays;
		highFuelLowDays.fuel = 100;
		highFuelLowDays.days = 1;
		highFuelLowDays.danger = 0.;

		WHEN( "comparing edges with different fuel" ) {
			THEN( "fuel takes precedence over days and danger" ) {
				// Lower fuel = higher priority
				CHECK_FALSE( lowFuelHighDays < highFuelLowDays );
				CHECK( highFuelLowDays < lowFuelHighDays );
			}
		}
	}
}

SCENARIO( "RouteEdge comparison operator - days takes precedence over danger", "[RouteEdge][operator<]" ) {
	GIVEN( "edges where days difference should override danger" ) {
		RouteEdge lowDaysHighDanger;
		lowDaysHighDanger.fuel = 100;
		lowDaysHighDanger.days = 5;
		lowDaysHighDanger.danger = 100.;

		RouteEdge highDaysLowDanger;
		highDaysLowDanger.fuel = 100;
		highDaysLowDanger.days = 50;
		highDaysLowDanger.danger = 0.;

		WHEN( "comparing edges with same fuel but different days" ) {
			THEN( "days takes precedence over danger" ) {
				// Lower days = higher priority
				CHECK_FALSE( lowDaysHighDanger < highDaysLowDanger );
				CHECK( highDaysLowDanger < lowDaysHighDanger );
			}
		}
	}
}

SCENARIO( "RouteEdge with zero values", "[RouteEdge][edge]" ) {
	GIVEN( "a RouteEdge with all zero values" ) {
		RouteEdge zero;
		WHEN( "checking default values" ) {
			THEN( "all values are zero" ) {
				CHECK( zero.prev == nullptr );
				CHECK( zero.fuel == 0 );
				CHECK( zero.days == 0 );
				CHECK( zero.danger == 0. );
			}
		}
		WHEN( "comparing with another zero edge" ) {
			RouteEdge otherZero;
			THEN( "they are equal in priority" ) {
				CHECK_FALSE( zero < otherZero );
				CHECK_FALSE( otherZero < zero );
			}
		}
	}
}

SCENARIO( "RouteEdge with negative fuel", "[RouteEdge][edge]" ) {
	GIVEN( "RouteEdges with negative fuel values" ) {
		RouteEdge negativeFuel;
		negativeFuel.fuel = -10;

		RouteEdge zeroFuel;
		zeroFuel.fuel = 0;

		WHEN( "comparing negative fuel with zero fuel" ) {
			THEN( "negative fuel has higher priority (lower value)" ) {
				CHECK_FALSE( negativeFuel < zeroFuel );
				CHECK( zeroFuel < negativeFuel );
			}
		}
	}
}

SCENARIO( "RouteEdge with negative days", "[RouteEdge][edge]" ) {
	GIVEN( "RouteEdges with negative days values" ) {
		RouteEdge negativeDays;
		negativeDays.fuel = 100;
		negativeDays.days = -5;

		RouteEdge zeroDays;
		zeroDays.fuel = 100;
		zeroDays.days = 0;

		WHEN( "comparing negative days with zero days" ) {
			THEN( "negative days has higher priority (lower value)" ) {
				CHECK_FALSE( negativeDays < zeroDays );
				CHECK( zeroDays < negativeDays );
			}
		}
	}
}

SCENARIO( "RouteEdge with negative danger", "[RouteEdge][edge]" ) {
	GIVEN( "RouteEdges with negative danger values" ) {
		RouteEdge negativeDanger;
		negativeDanger.fuel = 100;
		negativeDanger.days = 10;
		negativeDanger.danger = -5.;

		RouteEdge zeroDanger;
		zeroDanger.fuel = 100;
		zeroDanger.days = 10;
		zeroDanger.danger = 0.;

		WHEN( "comparing negative danger with zero danger" ) {
			THEN( "negative danger has higher priority (lower value)" ) {
				CHECK_FALSE( negativeDanger < zeroDanger );
				CHECK( zeroDanger < negativeDanger );
			}
		}
	}
}

SCENARIO( "RouteEdge with large values", "[RouteEdge][edge]" ) {
	GIVEN( "RouteEdges with large integer values" ) {
		RouteEdge largeFuel;
		largeFuel.fuel = 1000000;
		largeFuel.days = 365;
		largeFuel.danger = 1000.;

		RouteEdge smallFuel;
		smallFuel.fuel = 1;
		smallFuel.days = 1;
		smallFuel.danger = 0.;

		WHEN( "comparing edges with large vs small values" ) {
			THEN( "smaller values have higher priority" ) {
				CHECK_FALSE( smallFuel < largeFuel );
				CHECK( largeFuel < smallFuel );
			}
		}
	}
}

SCENARIO( "RouteEdge danger precision", "[RouteEdge][edge]" ) {
	GIVEN( "RouteEdges with small danger differences" ) {
		RouteEdge smallDanger;
		smallDanger.fuel = 100;
		smallDanger.days = 10;
		smallDanger.danger = 0.001;

		RouteEdge largerDanger;
		largerDanger.fuel = 100;
		largerDanger.days = 10;
		largerDanger.danger = 0.002;

		WHEN( "comparing edges with small danger differences" ) {
			THEN( "smaller danger has higher priority" ) {
				CHECK_FALSE( smallDanger < largerDanger );
				CHECK( largerDanger < smallDanger );
			}
		}
	}
	GIVEN( "RouteEdges with very small danger values" ) {
		RouteEdge tiny;
		tiny.fuel = 100;
		tiny.days = 10;
		tiny.danger = 1e-10;

		RouteEdge slightlyLarger;
		slightlyLarger.fuel = 100;
		slightlyLarger.days = 10;
		slightlyLarger.danger = 2e-10;

		WHEN( "comparing edges with tiny danger differences" ) {
			THEN( "smaller danger has higher priority" ) {
				CHECK_FALSE( tiny < slightlyLarger );
				CHECK( slightlyLarger < tiny );
			}
		}
	}
}

SCENARIO( "RouteEdge prev pointer comparison independence", "[RouteEdge][edge]" ) {
	GIVEN( "RouteEdges with different prev pointers but same metrics" ) {
		MockSystemPlaceholder placeholder1;
		MockSystemPlaceholder placeholder2;
		RouteEdge edge1(reinterpret_cast<const System*>(&placeholder1));
		edge1.fuel = 100;
		edge1.days = 10;
		edge1.danger = 5.;

		RouteEdge edge2(reinterpret_cast<const System*>(&placeholder2));
		edge2.fuel = 100;
		edge2.days = 10;
		edge2.danger = 5.;

		WHEN( "comparing edges with different prev but same metrics" ) {
			THEN( "the prev pointer does not affect comparison" ) {
				CHECK_FALSE( edge1 < edge2 );
				CHECK_FALSE( edge2 < edge1 );
			}
		}
	}
}

SCENARIO( "RouteEdge self comparison", "[RouteEdge][edge]" ) {
	GIVEN( "a RouteEdge" ) {
		RouteEdge edge;
		edge.fuel = 100;
		edge.days = 10;
		edge.danger = 5.;

		WHEN( "comparing an edge with itself" ) {
			THEN( "it is not less than itself" ) {
				CHECK_FALSE( edge < edge );
			}
		}
	}
}

SCENARIO( "RouteEdge transitive comparison", "[RouteEdge][edge]" ) {
	GIVEN( "three RouteEdges with different priorities" ) {
		RouteEdge best;
		best.fuel = 10;

		RouteEdge medium;
		medium.fuel = 50;

		RouteEdge worst;
		worst.fuel = 100;

		WHEN( "checking transitive property" ) {
			THEN( "if worst < medium and medium < best, then worst < best" ) {
				CHECK( worst < medium );
				CHECK( medium < best );
				CHECK( worst < best );
			}
		}
	}
}
// #endregion unit tests



} // test namespace