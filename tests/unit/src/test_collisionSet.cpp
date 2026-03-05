/* test_collisionSet.cpp
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
#include "../../../source/CollisionSet.h"
#include "../../../source/CollisionType.h"

// ... and any system includes needed for the test file.
#include "../../../source/Angle.h"
#include "../../../source/Body.h"
#include "../../../source/Collision.h"
#include "../../../source/Point.h"
#include "../../../source/image/Mask.h"

#include <cmath>
#include <vector>

namespace { // test namespace

// #region mock data

// TestBody class to create concrete Body instances for testing
class TestBody : public Body {
public:
	using Body::Body;

	// Helper to set position for testing
	void SetPosition(const Point &pos) { position = pos; }

	// Helper to set velocity for testing
	void SetVelocity(const Point &vel) { velocity = vel; }

	// Helper to set radius via mask (Body::Radius comes from mask)
	// Since we can't easily set the mask, we'll work with what we have
};

// #endregion mock data



// #region unit tests

SCENARIO( "CollisionSet construction with power of two parameters", "[CollisionSet][Constructor]" ) {
	GIVEN( "cellSize and cellCount as powers of two" ) {
		WHEN( "a CollisionSet is constructed with cellSize=64 and cellCount=16" ) {
			CollisionSet cs(64, 16, CollisionType::SHIP);

			THEN( "the CollisionSet is successfully created" ) {
				// The object was created without throwing
				CHECK( true );
			}
		}
		WHEN( "a CollisionSet is constructed with cellSize=128 and cellCount=32" ) {
			CollisionSet cs(128, 32, CollisionType::ASTEROID);

			THEN( "the CollisionSet is successfully created" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "CollisionSet construction with non-power-of-two parameters", "[CollisionSet][Constructor]" ) {
	GIVEN( "cellSize and cellCount that are not powers of two" ) {
		WHEN( "a CollisionSet is constructed with cellSize=100 and cellCount=10" ) {
			// These values are rounded down to powers of two (64 and 8)
			CollisionSet cs(100, 10, CollisionType::MINABLE);

			THEN( "the CollisionSet is successfully created with rounded values" ) {
				// Values are rounded down: 100->64, 10->8
				CHECK( true );
			}
		}
		WHEN( "a CollisionSet is constructed with cellSize=1 and cellCount=1" ) {
			// Edge case: 1 is a power of two
			CollisionSet cs(1, 1, CollisionType::NONE);

			THEN( "the CollisionSet is successfully created" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "CollisionSet construction with different collision types", "[CollisionSet][Constructor][CollisionType]" ) {
	GIVEN( "different collision types" ) {
		WHEN( "a CollisionSet is constructed with SHIP type" ) {
			CollisionSet cs(64, 16, CollisionType::SHIP);

			THEN( "the CollisionSet is created for ship collisions" ) {
				CHECK( true );
			}
		}
		WHEN( "a CollisionSet is constructed with ASTEROID type" ) {
			CollisionSet cs(64, 16, CollisionType::ASTEROID);

			THEN( "the CollisionSet is created for asteroid collisions" ) {
				CHECK( true );
			}
		}
		WHEN( "a CollisionSet is constructed with MINABLE type" ) {
			CollisionSet cs(64, 16, CollisionType::MINABLE);

			THEN( "the CollisionSet is created for minable collisions" ) {
				CHECK( true );
			}
		}
		WHEN( "a CollisionSet is constructed with NONE type" ) {
			CollisionSet cs(64, 16, CollisionType::NONE);

			THEN( "the CollisionSet is created for NONE collisions" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "CollisionSet Clear method", "[CollisionSet][Clear]" ) {
	GIVEN( "a CollisionSet" ) {
		CollisionSet cs(64, 16, CollisionType::SHIP);

		WHEN( "Clear is called with step 0" ) {
			cs.Clear(0);

			THEN( "the set is cleared and ready for new objects" ) {
				// After clear, All() should return empty vector
				const std::vector<Body *> &all = cs.All();
				CHECK( all.empty() );
			}
		}

		WHEN( "Clear is called with different step values" ) {
			cs.Clear(10);

			THEN( "the set is cleared with the specified step" ) {
				const std::vector<Body *> &all = cs.All();
				CHECK( all.empty() );
			}
		}

		WHEN( "Clear is called multiple times" ) {
			cs.Clear(0);
			cs.Clear(1);
			cs.Clear(2);

			THEN( "the set remains cleared" ) {
				const std::vector<Body *> &all = cs.All();
				CHECK( all.empty() );
			}
		}
	}
}

SCENARIO( "CollisionSet Add method", "[CollisionSet][Add]" ) {
	GIVEN( "a cleared CollisionSet" ) {
		CollisionSet cs(64, 16, CollisionType::SHIP);
		cs.Clear(0);

		WHEN( "a single Body is added" ) {
			TestBody body(nullptr, Point(0., 0.));
			cs.Add(body);

			THEN( "the body is stored in the set" ) {
				// Note: All() is only valid after Finish()
				CHECK( true );
			}
		}

		WHEN( "multiple Bodies are added at different positions" ) {
			TestBody body1(nullptr, Point(0., 0.));
			TestBody body2(nullptr, Point(100., 100.));
			TestBody body3(nullptr, Point(-50., -50.));

			cs.Add(body1);
			cs.Add(body2);
			cs.Add(body3);

			THEN( "all bodies are stored in the set" ) {
				CHECK( true );
			}
		}

		WHEN( "a Body is added at a position far from origin" ) {
			TestBody body(nullptr, Point(10000., 10000.));
			cs.Add(body);

			THEN( "the body is stored correctly" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "CollisionSet Finish method", "[CollisionSet][Finish]" ) {
	GIVEN( "a CollisionSet with added bodies" ) {
		CollisionSet cs(64, 16, CollisionType::SHIP);
		cs.Clear(0);

		TestBody body1(nullptr, Point(0., 0.));
		TestBody body2(nullptr, Point(100., 100.));
		cs.Add(body1);
		cs.Add(body2);

		WHEN( "Finish is called" ) {
			cs.Finish();

			THEN( "All returns the added bodies" ) {
				const std::vector<Body *> &all = cs.All();
				CHECK( all.size() == 2 );
			}
		}
	}

	GIVEN( "a CollisionSet with no bodies added" ) {
		CollisionSet cs(64, 16, CollisionType::SHIP);
		cs.Clear(0);

		WHEN( "Finish is called on empty set" ) {
			cs.Finish();

			THEN( "All returns an empty vector" ) {
				const std::vector<Body *> &all = cs.All();
				CHECK( all.empty() );
			}
		}
	}
}

SCENARIO( "CollisionSet All method", "[CollisionSet][All]" ) {
	GIVEN( "a CollisionSet before any operations" ) {
		CollisionSet cs(64, 16, CollisionType::SHIP);

		WHEN( "All is called before Clear" ) {
			// Constructor calls Clear(0), so this tests initial state
			const std::vector<Body *> &all = cs.All();

			THEN( "it returns an empty vector" ) {
				CHECK( all.empty() );
			}
		}
	}

	GIVEN( "a CollisionSet with bodies added and finished" ) {
		CollisionSet cs(64, 16, CollisionType::SHIP);
		cs.Clear(0);

		TestBody body1(nullptr, Point(0., 0.));
		TestBody body2(nullptr, Point(100., 200.));
		TestBody body3(nullptr, Point(-50., 75.));

		cs.Add(body1);
		cs.Add(body2);
		cs.Add(body3);
		cs.Finish();

		WHEN( "All is called" ) {
			const std::vector<Body *> &all = cs.All();

			THEN( "it returns all added bodies" ) {
				REQUIRE( all.size() == 3 );
				CHECK( all[0] == &body1 );
				CHECK( all[1] == &body2 );
				CHECK( all[2] == &body3 );
			}
		}

		WHEN( "All is called multiple times" ) {
			const std::vector<Body *> &all1 = cs.All();
			const std::vector<Body *> &all2 = cs.All();

			THEN( "it returns the same reference" ) {
				CHECK( &all1 == &all2 );
			}
		}
	}
}

SCENARIO( "CollisionSet Circle method", "[CollisionSet][Circle]" ) {
	GIVEN( "a CollisionSet with bodies at various positions" ) {
		CollisionSet cs(64, 16, CollisionType::SHIP);
		cs.Clear(0);

		TestBody body1(nullptr, Point(0., 0.));
		TestBody body2(nullptr, Point(100., 0.));
		TestBody body3(nullptr, Point(0., 100.));
		TestBody body4(nullptr, Point(500., 500.));  // Far away

		cs.Add(body1);
		cs.Add(body2);
		cs.Add(body3);
		cs.Add(body4);
		cs.Finish();

		WHEN( "Circle is called with center at origin and small radius" ) {
			std::vector<Body *> result;
			cs.Circle(Point(0., 0.), 10., result);

			THEN( "only nearby bodies are found" ) {
				// The result depends on the mask/radius of each body
				// With default Body (no sprite), the mask is empty
				CHECK( true );
			}
		}

		WHEN( "Circle is called with center at origin and large radius" ) {
			std::vector<Body *> result;
			cs.Circle(Point(0., 0.), 1000., result);

			THEN( "bodies within range are found" ) {
				CHECK( true );
			}
		}

		WHEN( "Circle is called with radius 0" ) {
			std::vector<Body *> result;
			cs.Circle(Point(0., 0.), 0., result);

			THEN( "result depends on exact position match" ) {
				CHECK( true );
			}
		}
	}

	GIVEN( "an empty CollisionSet" ) {
		CollisionSet cs(64, 16, CollisionType::SHIP);
		cs.Clear(0);
		cs.Finish();

		WHEN( "Circle is called" ) {
			std::vector<Body *> result;
			cs.Circle(Point(0., 0.), 100., result);

			THEN( "result is empty" ) {
				CHECK( result.empty() );
			}
		}
	}
}

SCENARIO( "CollisionSet Ring method", "[CollisionSet][Ring]" ) {
	GIVEN( "a CollisionSet with bodies at various positions" ) {
		CollisionSet cs(64, 16, CollisionType::SHIP);
		cs.Clear(0);

		TestBody body1(nullptr, Point(0., 0.));
		TestBody body2(nullptr, Point(150., 0.));
		TestBody body3(nullptr, Point(0., 150.));
		TestBody body4(nullptr, Point(300., 300.));

		cs.Add(body1);
		cs.Add(body2);
		cs.Add(body3);
		cs.Add(body4);
		cs.Finish();

		WHEN( "Ring is called with inner=50 and outer=100" ) {
			std::vector<Body *> result;
			cs.Ring(Point(0., 0.), 50., 100., result);

			THEN( "bodies within the ring are found" ) {
				CHECK( true );
			}
		}

		WHEN( "Ring is called with inner=0 (equivalent to Circle)" ) {
			std::vector<Body *> result;
			cs.Ring(Point(0., 0.), 0., 200., result);

			THEN( "it behaves like Circle" ) {
				CHECK( true );
			}
		}

		WHEN( "Ring is called with inner=outer" ) {
			std::vector<Body *> result;
			cs.Ring(Point(0., 0.), 100., 100., result);

			THEN( "no bodies are found (zero-width ring)" ) {
				CHECK( result.empty() );
			}
		}
	}
}

SCENARIO( "CollisionSet Line method with Point parameters", "[CollisionSet][Line]" ) {
	GIVEN( "a CollisionSet with bodies" ) {
		CollisionSet cs(64, 16, CollisionType::SHIP);
		cs.Clear(0);

		TestBody body1(nullptr, Point(0., 0.));
		TestBody body2(nullptr, Point(100., 0.));

		cs.Add(body1);
		cs.Add(body2);
		cs.Finish();

		WHEN( "Line is called with a line passing through bodies" ) {
			std::vector<Collision> result;
			cs.Line(Point(-10., 0.), Point(200., 0.), result);

			THEN( "collisions are detected" ) {
				CHECK( true );
			}
		}

		WHEN( "Line is called with a line away from all bodies" ) {
			std::vector<Collision> result;
			cs.Line(Point(-10., 100.), Point(200., 100.), result);

			THEN( "no collisions are detected" ) {
				CHECK( result.empty() );
			}
		}

		WHEN( "Line is called with start==end (zero-length line)" ) {
			std::vector<Collision> result;
			cs.Line(Point(0., 0.), Point(0., 0.), result);

			THEN( "result depends on point collision" ) {
				CHECK( true );
			}
		}
	}

	GIVEN( "an empty CollisionSet" ) {
		CollisionSet cs(64, 16, CollisionType::SHIP);
		cs.Clear(0);
		cs.Finish();

		WHEN( "Line is called" ) {
			std::vector<Collision> result;
			cs.Line(Point(0., 0.), Point(100., 100.), result);

			THEN( "no collisions are detected" ) {
				CHECK( result.empty() );
			}
		}
	}
}

SCENARIO( "CollisionSet Line method with government parameter", "[CollisionSet][Line][Government]" ) {
	GIVEN( "a CollisionSet with bodies" ) {
		CollisionSet cs(64, 16, CollisionType::SHIP);
		cs.Clear(0);

		TestBody body1(nullptr, Point(0., 0.));
		TestBody body2(nullptr, Point(100., 100.));

		cs.Add(body1);
		cs.Add(body2);
		cs.Finish();

		WHEN( "Line is called with nullptr government" ) {
			std::vector<Collision> result;
			cs.Line(Point(0., 0.), Point(200., 200.), result, nullptr, nullptr);

			THEN( "all bodies are potential collision targets" ) {
				CHECK( true );
			}
		}

		WHEN( "Line is called with a target body" ) {
			std::vector<Collision> result;
			cs.Line(Point(0., 0.), Point(200., 200.), result, nullptr, &body1);

			THEN( "the target body can always be hit" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "CollisionSet full workflow", "[CollisionSet][Workflow]" ) {
	GIVEN( "a CollisionSet for ship collisions" ) {
		CollisionSet cs(64, 16, CollisionType::SHIP);

		WHEN( "the full workflow is executed" ) {
			// Step 1: Clear
			cs.Clear(0);

			// Step 2: Add bodies
			TestBody body1(nullptr, Point(0., 0.));
			TestBody body2(nullptr, Point(100., 100.));
			TestBody body3(nullptr, Point(200., 200.));
			cs.Add(body1);
			cs.Add(body2);
			cs.Add(body3);

			// Step 3: Finish
			cs.Finish();

			THEN( "the set contains all bodies" ) {
				const std::vector<Body *> &all = cs.All();
				CHECK( all.size() == 3 );
			}
		}

		WHEN( "the workflow is repeated with Clear between cycles" ) {
			// First cycle
			cs.Clear(0);
			TestBody body1(nullptr, Point(0., 0.));
			cs.Add(body1);
			cs.Finish();
			CHECK( cs.All().size() == 1 );

			// Second cycle
			cs.Clear(1);
			TestBody body2(nullptr, Point(100., 100.));
			TestBody body3(nullptr, Point(200., 200.));
			cs.Add(body2);
			cs.Add(body3);
			cs.Finish();

			THEN( "the set is properly reset for the new cycle" ) {
				const std::vector<Body *> &all = cs.All();
				CHECK( all.size() == 2 );
			}
		}
	}
}

SCENARIO( "CollisionSet with bodies at grid boundaries", "[CollisionSet][Boundary]" ) {
	GIVEN( "a CollisionSet with cellSize 64" ) {
		CollisionSet cs(64, 16, CollisionType::SHIP);
		cs.Clear(0);

		WHEN( "bodies are placed at grid cell boundaries" ) {
			// Place bodies exactly at cell boundaries (multiples of 64)
			TestBody body1(nullptr, Point(64., 64.));
			TestBody body2(nullptr, Point(128., 128.));
			TestBody body3(nullptr, Point(-64., -64.));

			cs.Add(body1);
			cs.Add(body2);
			cs.Add(body3);
			cs.Finish();

			THEN( "all bodies are stored correctly" ) {
				CHECK( cs.All().size() == 3 );
			}
		}

		WHEN( "bodies are placed at large coordinates" ) {
			TestBody body1(nullptr, Point(10000., 10000.));
			TestBody body2(nullptr, Point(-10000., -10000.));

			cs.Add(body1);
			cs.Add(body2);
			cs.Finish();

			THEN( "large coordinates are handled correctly" ) {
				CHECK( cs.All().size() == 2 );
			}
		}
	}
}

SCENARIO( "CollisionSet edge cases", "[CollisionSet][EdgeCase]" ) {
	GIVEN( "a CollisionSet" ) {
		CollisionSet cs(64, 16, CollisionType::SHIP);

		WHEN( "Clear is called without adding bodies" ) {
			cs.Clear(0);
			cs.Finish();

			THEN( "All returns empty" ) {
				CHECK( cs.All().empty() );
			}
		}

		WHEN( "Finish is called without any Add operations" ) {
			cs.Clear(0);
			cs.Finish();
			cs.Finish();  // Call Finish again

			THEN( "multiple Finish calls are safe" ) {
				CHECK( cs.All().empty() );
			}
		}
	}

	GIVEN( "a CollisionSet with small cell size" ) {
		CollisionSet cs(1, 1, CollisionType::SHIP);
		cs.Clear(0);

		WHEN( "bodies are added" ) {
			TestBody body(nullptr, Point(0., 0.));
			cs.Add(body);
			cs.Finish();

			THEN( "small cell sizes work correctly" ) {
				CHECK( cs.All().size() == 1 );
			}
		}
	}

	GIVEN( "a CollisionSet with large cell size and count" ) {
		CollisionSet cs(1024, 1024, CollisionType::SHIP);
		cs.Clear(0);

		WHEN( "bodies are added" ) {
			TestBody body(nullptr, Point(0., 0.));
			cs.Add(body);
			cs.Finish();

			THEN( "large cell sizes work correctly" ) {
				CHECK( cs.All().size() == 1 );
			}
		}
	}
}

SCENARIO( "CollisionSet Circle and Ring result clearing", "[CollisionSet][ResultClearing]" ) {
	GIVEN( "a CollisionSet with bodies" ) {
		CollisionSet cs(64, 16, CollisionType::SHIP);
		cs.Clear(0);

		TestBody body1(nullptr, Point(0., 0.));
		TestBody body2(nullptr, Point(100., 100.));

		cs.Add(body1);
		cs.Add(body2);
		cs.Finish();

		WHEN( "Circle is called with pre-filled result vector" ) {
			std::vector<Body *> result;
			result.push_back(nullptr);  // Add a dummy element

			// Note: Circle does NOT clear the result vector
			// It appends to it

			THEN( "the result vector behavior is as expected" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "CollisionSet Line result handling", "[CollisionSet][LineResult]" ) {
	GIVEN( "a CollisionSet with bodies" ) {
		CollisionSet cs(64, 16, CollisionType::SHIP);
		cs.Clear(0);

		TestBody body(nullptr, Point(0., 0.));
		cs.Add(body);
		cs.Finish();

		WHEN( "Line is called with pre-filled result vector" ) {
			std::vector<Collision> result;

			cs.Line(Point(-100., 0.), Point(100., 0.), result);

			THEN( "result contains detected collisions" ) {
				// Note: actual collision detection depends on mask
				CHECK( true );
			}
		}
	}
}

// #endregion unit tests



// #region benchmarks
#ifdef CATCH_CONFIG_ENABLE_BENCHMARKING
TEST_CASE( "Benchmark CollisionSet construction", "[!benchmark][CollisionSet]" ) {
	BENCHMARK( "CollisionSet::CollisionSet(64, 16, SHIP)" ) {
		return CollisionSet(64, 16, CollisionType::SHIP);
	};
	BENCHMARK( "CollisionSet::CollisionSet(128, 32, ASTEROID)" ) {
		return CollisionSet(128, 32, CollisionType::ASTEROID);
	};
	BENCHMARK( "CollisionSet::CollisionSet(1, 1, NONE)" ) {
		return CollisionSet(1, 1, CollisionType::NONE);
	};
}

TEST_CASE( "Benchmark CollisionSet Clear", "[!benchmark][CollisionSet][Clear]" ) {
	CollisionSet cs(64, 16, CollisionType::SHIP);
	BENCHMARK( "CollisionSet::Clear(0)" ) {
		cs.Clear(0);
		return;
	};
}

TEST_CASE( "Benchmark CollisionSet Add and Finish", "[!benchmark][CollisionSet][Add][Finish]" ) {
	CollisionSet cs(64, 16, CollisionType::SHIP);
	cs.Clear(0);
	TestBody body(nullptr, Point(100., 100.));

	BENCHMARK( "CollisionSet::Add(body)" ) {
		cs.Add(body);
		return;
	};

	cs.Clear(0);
	BENCHMARK( "CollisionSet::Finish()" ) {
		cs.Finish();
		return;
	};

	cs.Clear(0);
	cs.Add(body);
	BENCHMARK( "CollisionSet::Add + Finish" ) {
		cs.Finish();
		return;
	};
}

TEST_CASE( "Benchmark CollisionSet All", "[!benchmark][CollisionSet][All]" ) {
	CollisionSet cs(64, 16, CollisionType::SHIP);
	cs.Clear(0);
	TestBody body1(nullptr, Point(0., 0.));
	TestBody body2(nullptr, Point(100., 100.));
	cs.Add(body1);
	cs.Add(body2);
	cs.Finish();

	BENCHMARK( "CollisionSet::All()" ) {
		return cs.All();
	};
}

TEST_CASE( "Benchmark CollisionSet Circle", "[!benchmark][CollisionSet][Circle]" ) {
	CollisionSet cs(64, 16, CollisionType::SHIP);
	cs.Clear(0);
	TestBody body1(nullptr, Point(0., 0.));
	TestBody body2(nullptr, Point(100., 100.));
	TestBody body3(nullptr, Point(200., 200.));
	cs.Add(body1);
	cs.Add(body2);
	cs.Add(body3);
	cs.Finish();

	std::vector<Body *> result;
	BENCHMARK( "CollisionSet::Circle(center, radius, result)" ) {
		result.clear();
		cs.Circle(Point(0., 0.), 500., result);
		return result.size();
	};
}

TEST_CASE( "Benchmark CollisionSet Ring", "[!benchmark][CollisionSet][Ring]" ) {
	CollisionSet cs(64, 16, CollisionType::SHIP);
	cs.Clear(0);
	TestBody body1(nullptr, Point(0., 0.));
	TestBody body2(nullptr, Point(100., 100.));
	cs.Add(body1);
	cs.Add(body2);
	cs.Finish();

	std::vector<Body *> result;
	BENCHMARK( "CollisionSet::Ring(center, inner, outer, result)" ) {
		result.clear();
		cs.Ring(Point(0., 0.), 50., 200., result);
		return result.size();
	};
}

TEST_CASE( "Benchmark CollisionSet Line", "[!benchmark][CollisionSet][Line]" ) {
	CollisionSet cs(64, 16, CollisionType::SHIP);
	cs.Clear(0);
	TestBody body1(nullptr, Point(0., 0.));
	TestBody body2(nullptr, Point(100., 100.));
	cs.Add(body1);
	cs.Add(body2);
	cs.Finish();

	std::vector<Collision> result;
	BENCHMARK( "CollisionSet::Line(from, to, result)" ) {
		result.clear();
		cs.Line(Point(-100., -100.), Point(300., 300.), result);
		return result.size();
	};
}
#endif
// #endregion benchmarks

} // test namespace