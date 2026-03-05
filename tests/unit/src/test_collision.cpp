/* test_collision.cpp
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
#include "../../../source/Collision.h"
#include "../../../source/CollisionType.h"

// ... and any system includes needed for the test file.
#include "../../../source/Body.h"

namespace { // test namespace

// #region mock data

// TestBody class to create concrete Body instances for testing
class TestBody : public Body {
public:
	using Body::Body;
};

// #endregion mock data



// #region unit tests

SCENARIO( "Collision construction with nullptr body", "[Collision]" ) {
	GIVEN( "a nullptr body and NONE collision type" ) {
		WHEN( "a Collision is constructed" ) {
			Collision collision(nullptr, CollisionType::NONE, 0.5);
			THEN( "HitBody returns nullptr" ) {
				CHECK( collision.HitBody() == nullptr );
			}
			AND_THEN( "GetCollisionType returns NONE" ) {
				CHECK( collision.GetCollisionType() == CollisionType::NONE );
			}
			AND_THEN( "IntersectionRange returns the correct value" ) {
				CHECK_THAT( collision.IntersectionRange(), Catch::Matchers::WithinAbs(0.5, 0.0001) );
			}
		}
	}
}

SCENARIO( "Collision construction with a valid body", "[Collision]" ) {
	GIVEN( "a valid Body pointer and SHIP collision type" ) {
		TestBody body(nullptr, Point(100., 200.));
		double range = 0.75;
		WHEN( "a Collision is constructed" ) {
			Collision collision(&body, CollisionType::SHIP, range);
			THEN( "HitBody returns the correct body pointer" ) {
				CHECK( collision.HitBody() == &body );
			}
			AND_THEN( "GetCollisionType returns SHIP" ) {
				CHECK( collision.GetCollisionType() == CollisionType::SHIP );
			}
			AND_THEN( "IntersectionRange returns the correct value" ) {
				CHECK_THAT( collision.IntersectionRange(), Catch::Matchers::WithinAbs(0.75, 0.0001) );
			}
		}
	}
}

SCENARIO( "Collision construction with different collision types", "[Collision]" ) {
	GIVEN( "a valid Body pointer" ) {
		TestBody body;
		WHEN( "a Collision is constructed with SHIP type" ) {
			Collision collision(&body, CollisionType::SHIP, 0.1);
			THEN( "GetCollisionType returns SHIP" ) {
				CHECK( collision.GetCollisionType() == CollisionType::SHIP );
			}
		}
		WHEN( "a Collision is constructed with MINABLE type" ) {
			Collision collision(&body, CollisionType::MINABLE, 0.2);
			THEN( "GetCollisionType returns MINABLE" ) {
				CHECK( collision.GetCollisionType() == CollisionType::MINABLE );
			}
		}
		WHEN( "a Collision is constructed with ASTEROID type" ) {
			Collision collision(&body, CollisionType::ASTEROID, 0.3);
			THEN( "GetCollisionType returns ASTEROID" ) {
				CHECK( collision.GetCollisionType() == CollisionType::ASTEROID );
			}
		}
		WHEN( "a Collision is constructed with NONE type" ) {
			Collision collision(&body, CollisionType::NONE, 0.4);
			THEN( "GetCollisionType returns NONE" ) {
				CHECK( collision.GetCollisionType() == CollisionType::NONE );
			}
		}
	}
}

SCENARIO( "Collision construction with various range values", "[Collision]" ) {
	GIVEN( "a valid Body pointer" ) {
		TestBody body;
		WHEN( "a Collision is constructed with range 0.0" ) {
			Collision collision(&body, CollisionType::SHIP, 0.0);
			THEN( "IntersectionRange returns 0.0" ) {
				CHECK_THAT( collision.IntersectionRange(), Catch::Matchers::WithinAbs(0.0, 0.0001) );
			}
		}
		WHEN( "a Collision is constructed with range 1.0" ) {
			Collision collision(&body, CollisionType::SHIP, 1.0);
			THEN( "IntersectionRange returns 1.0" ) {
				CHECK_THAT( collision.IntersectionRange(), Catch::Matchers::WithinAbs(1.0, 0.0001) );
			}
		}
		WHEN( "a Collision is constructed with a large range value" ) {
			Collision collision(&body, CollisionType::SHIP, 1000.5);
			THEN( "IntersectionRange returns the correct value" ) {
				CHECK_THAT( collision.IntersectionRange(), Catch::Matchers::WithinAbs(1000.5, 0.0001) );
			}
		}
		WHEN( "a Collision is constructed with a negative range value" ) {
			Collision collision(&body, CollisionType::SHIP, -0.5);
			THEN( "IntersectionRange returns the negative value" ) {
				CHECK_THAT( collision.IntersectionRange(), Catch::Matchers::WithinAbs(-0.5, 0.0001) );
			}
		}
	}
}

SCENARIO( "Collision comparison operator", "[Collision]" ) {
	GIVEN( "two Collisions with different range values" ) {
		TestBody body1;
		TestBody body2;
		Collision collision1(&body1, CollisionType::SHIP, 0.3);
		Collision collision2(&body2, CollisionType::ASTEROID, 0.7);
		WHEN( "comparing collisions with different ranges" ) {
			THEN( "collision with smaller range is less than collision with larger range" ) {
				CHECK( collision1 < collision2 );
				CHECK_FALSE( collision2 < collision1 );
			}
		}
	}
	GIVEN( "two Collisions with the same range" ) {
		TestBody body1;
		TestBody body2;
		Collision collision1(&body1, CollisionType::SHIP, 0.5);
		Collision collision2(&body2, CollisionType::ASTEROID, 0.5);
		WHEN( "comparing collisions with same range" ) {
			THEN( "neither is less than the other" ) {
				CHECK_FALSE( collision1 < collision2 );
				CHECK_FALSE( collision2 < collision1 );
			}
		}
	}
	GIVEN( "multiple Collisions with different range values" ) {
		TestBody body;
		Collision collision1(&body, CollisionType::SHIP, 0.1);
		Collision collision2(&body, CollisionType::SHIP, 0.5);
		Collision collision3(&body, CollisionType::SHIP, 1.0);
		WHEN( "comparing multiple collisions" ) {
			THEN( "the comparison forms a total order by range" ) {
				CHECK( collision1 < collision2 );
				CHECK( collision2 < collision3 );
				CHECK( collision1 < collision3 );
			}
		}
	}
	GIVEN( "Collisions with nullptr and valid body but different ranges" ) {
		TestBody body;
		Collision collisionNullptr(nullptr, CollisionType::NONE, 0.2);
		Collision collisionValid(&body, CollisionType::SHIP, 0.8);
		WHEN( "comparing collisions with nullptr and valid body" ) {
			THEN( "comparison is still based on range only" ) {
				CHECK( collisionNullptr < collisionValid );
				CHECK_FALSE( collisionValid < collisionNullptr );
			}
		}
	}
}

SCENARIO( "Collision HitBody returns the same pointer after construction", "[Collision][HitBody]" ) {
	GIVEN( "a Collision with a valid body" ) {
		TestBody body;
		Collision collision(&body, CollisionType::SHIP, 0.5);
		WHEN( "HitBody is called multiple times" ) {
			Body* first = collision.HitBody();
			Body* second = collision.HitBody();
			THEN( "it returns the same pointer each time" ) {
				CHECK( first == second );
				CHECK( first == &body );
			}
		}
	}
	GIVEN( "a Collision with nullptr" ) {
		Collision collision(nullptr, CollisionType::NONE, 0.0);
		WHEN( "HitBody is called multiple times" ) {
			Body* first = collision.HitBody();
			Body* second = collision.HitBody();
			THEN( "it returns nullptr each time" ) {
				CHECK( first == nullptr );
				CHECK( second == nullptr );
			}
		}
	}
}

SCENARIO( "Collision GetCollisionType is const correct", "[Collision][GetCollisionType]" ) {
	GIVEN( "a const Collision" ) {
		TestBody body;
		const Collision collision(&body, CollisionType::MINABLE, 0.3);
		WHEN( "GetCollisionType is called on const object" ) {
			CollisionType type = collision.GetCollisionType();
			THEN( "it returns the correct type" ) {
				CHECK( type == CollisionType::MINABLE );
			}
		}
	}
}

SCENARIO( "Collision IntersectionRange is const correct", "[Collision][IntersectionRange]" ) {
	GIVEN( "a const Collision" ) {
		TestBody body;
		const Collision collision(&body, CollisionType::SHIP, 0.42);
		WHEN( "IntersectionRange is called on const object" ) {
			double range = collision.IntersectionRange();
			THEN( "it returns the correct range" ) {
				CHECK_THAT( range, Catch::Matchers::WithinAbs(0.42, 0.0001) );
			}
		}
	}
}

// #endregion unit tests



// #region benchmarks
#ifdef CATCH_CONFIG_ENABLE_BENCHMARKING
TEST_CASE( "Benchmark Collision construction", "[!benchmark][Collision]" ) {
	TestBody body;
	BENCHMARK( "Collision::Collision(nullptr, NONE, 0.5)" ) {
		return Collision(nullptr, CollisionType::NONE, 0.5);
	};
	BENCHMARK( "Collision::Collision(&body, SHIP, 0.5)" ) {
		return Collision(&body, CollisionType::SHIP, 0.5);
	};
}

TEST_CASE( "Benchmark Collision getters", "[!benchmark][Collision]" ) {
	TestBody body;
	Collision collision(&body, CollisionType::SHIP, 0.75);
	BENCHMARK( "Collision::HitBody()" ) {
		return collision.HitBody();
	};
	BENCHMARK( "Collision::GetCollisionType()" ) {
		return collision.GetCollisionType();
	};
	BENCHMARK( "Collision::IntersectionRange()" ) {
		return collision.IntersectionRange();
	};
}

TEST_CASE( "Benchmark Collision comparison", "[!benchmark][Collision]" ) {
	TestBody body;
	Collision collision1(&body, CollisionType::SHIP, 0.3);
	Collision collision2(&body, CollisionType::SHIP, 0.7);
	BENCHMARK( "Collision::operator<" ) {
		return collision1 < collision2;
	};
}
#endif
// #endregion benchmarks

} // test namespace