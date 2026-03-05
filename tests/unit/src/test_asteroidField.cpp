/* test_asteroidField.cpp
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
#include "../../../source/AsteroidField.h"

// ... and any system includes needed for the test file.
#include <list>
#include <memory>

namespace { // test namespace

// #region mock data

// Insert file-local data here, e.g. classes, structs, or fixtures that will be useful
// to help test this class/method.

// #endregion mock data



// #region unit tests

SCENARIO( "AsteroidField construction", "[AsteroidField]" ) {
	GIVEN( "no parameters" ) {
		WHEN( "an AsteroidField is default constructed" ) {
			AsteroidField field;
			THEN( "it has no minables" ) {
				CHECK( field.Minables().empty() );
			}
		}
	}
}

SCENARIO( "AsteroidField Clear method", "[AsteroidField]" ) {
	GIVEN( "an AsteroidField" ) {
		AsteroidField field;
		WHEN( "Clear is called" ) {
			field.Clear();
			THEN( "the minables list is empty" ) {
				CHECK( field.Minables().empty() );
			}
		}
	}
}

SCENARIO( "AsteroidField Minables getter", "[AsteroidField]" ) {
	GIVEN( "a default constructed AsteroidField" ) {
		AsteroidField field;
		WHEN( "Minables is called" ) {
			const std::list<std::shared_ptr<Minable>> &minables = field.Minables();
			THEN( "it returns an empty list" ) {
				CHECK( minables.empty() );
			}
			AND_THEN( "the returned reference is valid" ) {
				CHECK( &minables == &field.Minables() );
			}
		}
	}
	GIVEN( "an AsteroidField after Clear" ) {
		AsteroidField field;
		field.Clear();
		WHEN( "Minables is called" ) {
			const std::list<std::shared_ptr<Minable>> &minables = field.Minables();
			THEN( "it returns an empty list" ) {
				CHECK( minables.empty() );
			}
		}
	}
}

SCENARIO( "AsteroidField Add with null Minable", "[AsteroidField]" ) {
	GIVEN( "an AsteroidField" ) {
		AsteroidField field;
		WHEN( "Add is called with a null Minable pointer" ) {
			WeightedList<double> belts;
			field.Add(nullptr, 5, 1.0, belts);
			THEN( "no minables are added" ) {
				CHECK( field.Minables().empty() );
			}
		}
	}
}

SCENARIO( "AsteroidField multiple Clear calls", "[AsteroidField]" ) {
	GIVEN( "an AsteroidField" ) {
		AsteroidField field;
		WHEN( "Clear is called multiple times" ) {
			field.Clear();
			field.Clear();
			field.Clear();
			THEN( "the field remains empty and valid" ) {
				CHECK( field.Minables().empty() );
			}
		}
	}
}

SCENARIO( "AsteroidField is copyable", "[AsteroidField]" ) {
	GIVEN( "a default constructed AsteroidField" ) {
		AsteroidField original;
		WHEN( "a copy is made" ) {
			AsteroidField copy(original);
			THEN( "the copy has no minables" ) {
				CHECK( copy.Minables().empty() );
			}
		}
	}
}

SCENARIO( "AsteroidField is assignable", "[AsteroidField]" ) {
	GIVEN( "two AsteroidField instances" ) {
		AsteroidField first;
		AsteroidField second;
		WHEN( "one is assigned to another" ) {
			second = first;
			THEN( "the assigned field has no minables" ) {
				CHECK( second.Minables().empty() );
			}
		}
	}
}

// #endregion unit tests



// #region benchmarks
#ifdef CATCH_CONFIG_ENABLE_BENCHMARKING
TEST_CASE( "Benchmark AsteroidField construction", "[!benchmark][AsteroidField]" ) {
	BENCHMARK( "AsteroidField::AsteroidField()" ) {
		return AsteroidField();
	};
}

TEST_CASE( "Benchmark AsteroidField Clear", "[!benchmark][AsteroidField]" ) {
	AsteroidField field;
	BENCHMARK( "AsteroidField::Clear()" ) {
		field.Clear();
		return;
	};
}

TEST_CASE( "Benchmark AsteroidField Minables getter", "[!benchmark][AsteroidField]" ) {
	AsteroidField field;
	BENCHMARK( "AsteroidField::Minables()" ) {
		return field.Minables();
	};
}
#endif
// #endregion benchmarks

} // test namespace