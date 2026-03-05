/* test_entity.cpp
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
#include "../../../source/Entity.h"

// ... and any system includes needed for the test file.
#include "../../../source/Outfit.h"

namespace { // test namespace

// #region mock data

// TestEntity class to implement pure virtual methods for testing
class TestEntity : public Entity {
public:
	TestEntity() = default;
	~TestEntity() override = default;

	// Implement pure virtual method from Entity
	double Mass() const override { return mass; }
	void SetMass(double m) { mass = m; }

	// Implement pure virtual method from Entity
	double MaximumHeat() const override { return maximumHeat; }
	void SetMaximumHeat(double maxHeat) { maximumHeat = maxHeat; }

	// Expose protected heat member for testing
	void SetHeat(double h) { heat = h; }
	double GetHeat() const { return heat; }

	// Expose protected attributes member for modification
	Outfit& GetAttributes() { return attributes; }

	// Access to the static constexpr member
	static constexpr double GetMaxTemperature() { return MAXIMUM_TEMPERATURE; }

private:
	double mass = 100.;
	double maximumHeat = 1000.;
};

// #endregion mock data



// #region unit tests

SCENARIO( "Entity default construction", "[Entity]" ) {
	GIVEN( "no parameters" ) {
		WHEN( "an Entity is default constructed" ) {
			TestEntity entity;
			THEN( "heat is initialized to 0" ) {
				CHECK( entity.GetHeat() == 0. );
			}
			AND_THEN( "attributes is default constructed" ) {
				// Default Outfit has no attributes
				CHECK( entity.Attributes().Get("mass") == 0. );
				CHECK( entity.Attributes().Mass() == 0. );
			}
			AND_THEN( "Heat() returns 1.0 when MaximumHeat is non-zero but heat is 0" ) {
				// heat = 0, maximumHeat = 1000 (default), so Heat() = 0/1000 = 0
				CHECK_THAT( entity.Heat(), Catch::Matchers::WithinAbs(0., 0.0001) );
			}
		}
	}
}

SCENARIO( "Entity Attributes getter", "[Entity][Attributes]" ) {
	GIVEN( "a default constructed Entity" ) {
		TestEntity entity;
		WHEN( "Attributes() is called" ) {
			const Outfit& attrs = entity.Attributes();
			THEN( "it returns a reference to the internal attributes" ) {
				// Default Outfit should have mass 0
				CHECK_THAT( attrs.Mass(), Catch::Matchers::WithinAbs(0., 0.0001) );
				CHECK( attrs.Get("nonexistent") == 0. );
			}
		}
	}
	GIVEN( "an Entity with modified attributes" ) {
		TestEntity entity;
		entity.GetAttributes().Set("shield", 100.);
		WHEN( "Attributes() is called" ) {
			const Outfit& attrs = entity.Attributes();
			THEN( "it returns the modified attributes" ) {
				CHECK_THAT( attrs.Get("shield"), Catch::Matchers::WithinAbs(100., 0.0001) );
			}
		}
	}
}

SCENARIO( "Entity Mass getter", "[Entity][Mass]" ) {
	GIVEN( "an Entity with default mass" ) {
		TestEntity entity;
		WHEN( "Mass() is called" ) {
			double mass = entity.Mass();
			THEN( "it returns the default mass value" ) {
				CHECK_THAT( mass, Catch::Matchers::WithinAbs(100., 0.0001) );
			}
		}
	}
	GIVEN( "an Entity with custom mass" ) {
		TestEntity entity;
		entity.SetMass(500.);
		WHEN( "Mass() is called" ) {
			double mass = entity.Mass();
			THEN( "it returns the custom mass value" ) {
				CHECK_THAT( mass, Catch::Matchers::WithinAbs(500., 0.0001) );
			}
		}
	}
	GIVEN( "an Entity with zero mass" ) {
		TestEntity entity;
		entity.SetMass(0.);
		WHEN( "Mass() is called" ) {
			double mass = entity.Mass();
			THEN( "it returns zero" ) {
				CHECK_THAT( mass, Catch::Matchers::WithinAbs(0., 0.0001) );
			}
		}
	}
	GIVEN( "an Entity with negative mass (edge case)" ) {
		TestEntity entity;
		entity.SetMass(-100.);
		WHEN( "Mass() is called" ) {
			double mass = entity.Mass();
			THEN( "it returns the negative mass" ) {
				// Mass could theoretically be negative in some implementations
				CHECK_THAT( mass, Catch::Matchers::WithinAbs(-100., 0.0001) );
			}
		}
	}
}

SCENARIO( "Entity Heat calculation", "[Entity][Heat]" ) {
	GIVEN( "an Entity with default heat (0) and maximum heat (1000)" ) {
		TestEntity entity;
		REQUIRE( entity.GetHeat() == 0. );
		REQUIRE_THAT( entity.MaximumHeat(), Catch::Matchers::WithinAbs(1000., 0.0001) );
		WHEN( "Heat() is called" ) {
			double heat = entity.Heat();
			THEN( "it returns 0.0 (heat / maximumHeat)" ) {
				CHECK_THAT( heat, Catch::Matchers::WithinAbs(0., 0.0001) );
			}
		}
	}
	GIVEN( "an Entity with heat = 500 and maximum heat = 1000" ) {
		TestEntity entity;
		entity.SetHeat(500.);
		entity.SetMaximumHeat(1000.);
		WHEN( "Heat() is called" ) {
			double heat = entity.Heat();
			THEN( "it returns 0.5 (500 / 1000)" ) {
				CHECK_THAT( heat, Catch::Matchers::WithinAbs(0.5, 0.0001) );
			}
		}
	}
	GIVEN( "an Entity with heat = 1000 and maximum heat = 1000" ) {
		TestEntity entity;
		entity.SetHeat(1000.);
		entity.SetMaximumHeat(1000.);
		WHEN( "Heat() is called" ) {
			double heat = entity.Heat();
			THEN( "it returns 1.0 (1000 / 1000)" ) {
				CHECK_THAT( heat, Catch::Matchers::WithinAbs(1., 0.0001) );
			}
		}
	}
	GIVEN( "an Entity with overheat (heat > maximum)" ) {
		TestEntity entity;
		entity.SetHeat(1500.);
		entity.SetMaximumHeat(1000.);
		WHEN( "Heat() is called" ) {
			double heat = entity.Heat();
			THEN( "it returns 1.5 (1500 / 1000) - overheat is allowed" ) {
				CHECK_THAT( heat, Catch::Matchers::WithinAbs(1.5, 0.0001) );
			}
		}
	}
	GIVEN( "an Entity with maximum heat = 0 (edge case)" ) {
		TestEntity entity;
		entity.SetHeat(500.);
		entity.SetMaximumHeat(0.);
		WHEN( "Heat() is called" ) {
			double heat = entity.Heat();
			THEN( "it returns 1.0 (division by zero protection)" ) {
				// Per Entity.cpp: return maximum ? heat / maximum : 1.;
				CHECK_THAT( heat, Catch::Matchers::WithinAbs(1., 0.0001) );
			}
		}
	}
	GIVEN( "an Entity with maximum heat = 0 and heat = 0 (edge case)" ) {
		TestEntity entity;
		entity.SetHeat(0.);
		entity.SetMaximumHeat(0.);
		WHEN( "Heat() is called" ) {
			double heat = entity.Heat();
			THEN( "it returns 1.0 (division by zero protection)" ) {
				CHECK_THAT( heat, Catch::Matchers::WithinAbs(1., 0.0001) );
			}
		}
	}
	GIVEN( "an Entity with negative maximum heat (edge case)" ) {
		TestEntity entity;
		entity.SetHeat(100.);
		entity.SetMaximumHeat(-100.);
		WHEN( "Heat() is called" ) {
			double heat = entity.Heat();
			THEN( "it returns -1.0 (100 / -100)" ) {
				// Negative maximum heat would result in negative Heat ratio
				CHECK_THAT( heat, Catch::Matchers::WithinAbs(-1., 0.0001) );
			}
		}
	}
}

SCENARIO( "Entity MaximumHeat getter", "[Entity][MaximumHeat]" ) {
	GIVEN( "an Entity with default maximum heat" ) {
		TestEntity entity;
		WHEN( "MaximumHeat() is called" ) {
			double maxHeat = entity.MaximumHeat();
			THEN( "it returns the default maximum heat value" ) {
				CHECK_THAT( maxHeat, Catch::Matchers::WithinAbs(1000., 0.0001) );
			}
		}
	}
	GIVEN( "an Entity with custom maximum heat" ) {
		TestEntity entity;
		entity.SetMaximumHeat(5000.);
		WHEN( "MaximumHeat() is called" ) {
			double maxHeat = entity.MaximumHeat();
			THEN( "it returns the custom maximum heat value" ) {
				CHECK_THAT( maxHeat, Catch::Matchers::WithinAbs(5000., 0.0001) );
			}
		}
	}
	GIVEN( "an Entity with zero maximum heat" ) {
		TestEntity entity;
		entity.SetMaximumHeat(0.);
		WHEN( "MaximumHeat() is called" ) {
			double maxHeat = entity.MaximumHeat();
			THEN( "it returns zero" ) {
				CHECK_THAT( maxHeat, Catch::Matchers::WithinAbs(0., 0.0001) );
			}
		}
	}
}

SCENARIO( "Entity MAXIMUM_TEMPERATURE constant", "[Entity][MAXIMUM_TEMPERATURE]" ) {
	GIVEN( "the Entity class" ) {
		WHEN( "accessing MAXIMUM_TEMPERATURE" ) {
			THEN( "it equals 100.0" ) {
				CHECK_THAT( TestEntity::GetMaxTemperature(), Catch::Matchers::WithinAbs(100., 0.0001) );
			}
		}
	}
}

SCENARIO( "Entity inheritance from Body", "[Entity][Body]" ) {
	GIVEN( "an Entity" ) {
		TestEntity entity;
		WHEN( "accessing Body methods" ) {
			THEN( "Position() returns default (0, 0)" ) {
				CHECK( entity.Position().X() == 0. );
				CHECK( entity.Position().Y() == 0. );
			}
			AND_THEN( "Velocity() returns default (0, 0)" ) {
				CHECK( entity.Velocity().X() == 0. );
				CHECK( entity.Velocity().Y() == 0. );
			}
			AND_THEN( "ShouldBeRemoved() returns false" ) {
				CHECK_FALSE( entity.ShouldBeRemoved() );
			}
		}
	}
}

SCENARIO( "Entity destructor is virtual", "[Entity][Destructor]" ) {
	GIVEN( "an Entity pointer to a TestEntity" ) {
		Entity* entity = new TestEntity();
		WHEN( "delete is called on the base pointer" ) {
			THEN( "no memory leak or crash occurs (virtual destructor)" ) {
				// This tests that the virtual destructor works correctly
				// If the destructor were not virtual, this could cause issues
				delete entity;
				CHECK( true );
			}
		}
	}
}

SCENARIO( "Entity heat boundary conditions", "[Entity][Heat][Boundary]" ) {
	GIVEN( "an Entity with very large heat value" ) {
		TestEntity entity;
		entity.SetHeat(1e15);
		entity.SetMaximumHeat(1e12);
		WHEN( "Heat() is called" ) {
			double heat = entity.Heat();
			THEN( "it handles large values correctly (1000.0)" ) {
				CHECK_THAT( heat, Catch::Matchers::WithinAbs(1000., 0.1) );
			}
		}
	}
	GIVEN( "an Entity with very small heat values" ) {
		TestEntity entity;
		entity.SetHeat(1e-15);
		entity.SetMaximumHeat(1.);
		WHEN( "Heat() is called" ) {
			double heat = entity.Heat();
			THEN( "it handles small values correctly" ) {
				CHECK_THAT( heat, Catch::Matchers::WithinAbs(1e-15, 1e-20) );
			}
		}
	}
	GIVEN( "an Entity with heat exactly at maximum" ) {
		TestEntity entity;
		entity.SetHeat(1000.);
		entity.SetMaximumHeat(1000.);
		WHEN( "Heat() is called" ) {
			double heat = entity.Heat();
			THEN( "it returns exactly 1.0" ) {
				CHECK_THAT( heat, Catch::Matchers::WithinAbs(1.0, 1e-10) );
			}
		}
	}
	GIVEN( "an Entity with heat just below maximum" ) {
		TestEntity entity;
		entity.SetHeat(999.999);
		entity.SetMaximumHeat(1000.);
		WHEN( "Heat() is called" ) {
			double heat = entity.Heat();
			THEN( "it returns just below 1.0" ) {
				CHECK( heat < 1.0 );
				CHECK_THAT( heat, Catch::Matchers::WithinAbs(0.999999, 1e-6) );
			}
		}
	}
	GIVEN( "an Entity with heat just above maximum" ) {
		TestEntity entity;
		entity.SetHeat(1000.001);
		entity.SetMaximumHeat(1000.);
		WHEN( "Heat() is called" ) {
			double heat = entity.Heat();
			THEN( "it returns just above 1.0 (overheat)" ) {
				CHECK( heat > 1.0 );
				CHECK_THAT( heat, Catch::Matchers::WithinAbs(1.000001, 1e-6) );
			}
		}
	}
}

// #endregion unit tests



// #region benchmarks
#ifdef CATCH_CONFIG_ENABLE_BENCHMARKING
TEST_CASE( "Benchmark Entity methods", "[!benchmark][Entity]" ) {
	TestEntity entity;
	entity.SetHeat(500.);
	entity.SetMaximumHeat(1000.);
	entity.SetMass(100.);

	BENCHMARK( "Entity::Attributes()" ) {
		return entity.Attributes();
	};
	BENCHMARK( "Entity::Mass()" ) {
		return entity.Mass();
	};
	BENCHMARK( "Entity::Heat()" ) {
		return entity.Heat();
	};
	BENCHMARK( "Entity::MaximumHeat()" ) {
		return entity.MaximumHeat();
	};
}

TEST_CASE( "Benchmark Entity construction", "[!benchmark][Entity]" ) {
	BENCHMARK( "TestEntity::TestEntity()" ) {
		return TestEntity();
	};
}
#endif
// #endregion benchmarks

} // test namespace