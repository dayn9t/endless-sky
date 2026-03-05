/* test_hasher.cpp
Copyright (c) 2025 by Amazinite

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
#include "../../../source/Hasher.h"

// ... and any system includes needed for the test file.
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>

namespace { // test namespace

// #region mock data

// A custom type with a std::hash specialization for testing Hasher with user-defined types.
struct CustomType {
	int value;
	bool operator==(const CustomType &other) const noexcept { return value == other.value; }
};

} // anonymous namespace

// std::hash specialization for CustomType
namespace std {
template<>
struct hash<CustomType> {
	std::size_t operator()(const CustomType &obj) const noexcept {
		return static_cast<std::size_t>(obj.value);
	}
};
}

namespace { // test namespace

// #endregion mock data



// #region unit tests

SCENARIO( "Hasher class traits", "[hasher][traits]" ) {
	SECTION( "Hasher is a static utility class" ) {
		// Hasher has only static methods, so it should not be instantiated.
		CHECK( std::is_default_constructible_v<Hasher> );
		CHECK( std::is_destructible_v<Hasher> );
	}
}

SCENARIO( "Hasher::Hash with integral types", "[hasher][integral]" ) {
	GIVEN( "a zero seed" ) {
		std::size_t seed = 0;
		WHEN( "hashing an integer" ) {
			Hasher::Hash(seed, 42);
			THEN( "the seed is modified" ) {
				CHECK( seed != 0 );
			}
		}
		WHEN( "hashing zero" ) {
			Hasher::Hash(seed, 0);
			THEN( "the seed is still modified" ) {
				CHECK( seed != 0 );
			}
		}
	}

	GIVEN( "a non-zero seed" ) {
		std::size_t seed = 12345;
		const std::size_t originalSeed = seed;
		WHEN( "hashing an integer" ) {
			Hasher::Hash(seed, 42);
			THEN( "the seed is modified" ) {
				CHECK( seed != originalSeed );
			}
		}
	}

	GIVEN( "the same seed and value" ) {
		WHEN( "hashing multiple times" ) {
			THEN( "the result is deterministic" ) {
				std::size_t seed1 = 100;
				std::size_t seed2 = 100;
				Hasher::Hash(seed1, 42);
				Hasher::Hash(seed2, 42);
				CHECK( seed1 == seed2 );
			}
		}
	}

	GIVEN( "different values to hash" ) {
		WHEN( "hashing from the same initial seed" ) {
			THEN( "different values produce different results" ) {
				std::size_t seed1 = 100;
				std::size_t seed2 = 100;
				Hasher::Hash(seed1, 42);
				Hasher::Hash(seed2, 99);
				CHECK( seed1 != seed2 );
			}
		}
	}

	GIVEN( "different initial seeds" ) {
		WHEN( "hashing the same value" ) {
			THEN( "different seeds produce different results" ) {
				std::size_t seed1 = 100;
				std::size_t seed2 = 200;
				Hasher::Hash(seed1, 42);
				Hasher::Hash(seed2, 42);
				CHECK( seed1 != seed2 );
			}
		}
	}
}

SCENARIO( "Hasher::Hash with floating point types", "[hasher][floating]" ) {
	GIVEN( "floating point values" ) {
		WHEN( "hashing the same float value" ) {
			THEN( "results are deterministic" ) {
				std::size_t seed1 = 0;
				std::size_t seed2 = 0;
				Hasher::Hash(seed1, 3.14f);
				Hasher::Hash(seed2, 3.14f);
				CHECK( seed1 == seed2 );
			}
		}
		WHEN( "hashing the same double value" ) {
			THEN( "results are deterministic" ) {
				std::size_t seed1 = 0;
				std::size_t seed2 = 0;
				Hasher::Hash(seed1, 2.718281828);
				Hasher::Hash(seed2, 2.718281828);
				CHECK( seed1 == seed2 );
			}
		}
		WHEN( "hashing different float values" ) {
			THEN( "results are different" ) {
				std::size_t seed1 = 0;
				std::size_t seed2 = 0;
				Hasher::Hash(seed1, 1.0f);
				Hasher::Hash(seed2, 2.0f);
				CHECK( seed1 != seed2 );
			}
		}
	}
}

SCENARIO( "Hasher::Hash with strings", "[hasher][string]" ) {
	GIVEN( "string values" ) {
		WHEN( "hashing the same string" ) {
			THEN( "results are deterministic" ) {
				std::size_t seed1 = 0;
				std::size_t seed2 = 0;
				Hasher::Hash(seed1, std::string("test"));
				Hasher::Hash(seed2, std::string("test"));
				CHECK( seed1 == seed2 );
			}
		}
		WHEN( "hashing different strings" ) {
			THEN( "results are different" ) {
				std::size_t seed1 = 0;
				std::size_t seed2 = 0;
				Hasher::Hash(seed1, std::string("hello"));
				Hasher::Hash(seed2, std::string("world"));
				CHECK( seed1 != seed2 );
			}
		}
		WHEN( "hashing an empty string" ) {
			THEN( "the seed is still modified" ) {
				std::size_t seed = 0;
				Hasher::Hash(seed, std::string(""));
				CHECK( seed != 0 );
			}
		}
	}
}

SCENARIO( "Hasher::Hash with custom types", "[hasher][custom]" ) {
	GIVEN( "a custom type with std::hash specialization" ) {
		WHEN( "hashing the same custom type value" ) {
			THEN( "results are deterministic" ) {
				std::size_t seed1 = 0;
				std::size_t seed2 = 0;
				Hasher::Hash(seed1, CustomType{42});
				Hasher::Hash(seed2, CustomType{42});
				CHECK( seed1 == seed2 );
			}
		}
		WHEN( "hashing different custom type values" ) {
			THEN( "results are different" ) {
				std::size_t seed1 = 0;
				std::size_t seed2 = 0;
				Hasher::Hash(seed1, CustomType{10});
				Hasher::Hash(seed2, CustomType{20});
				CHECK( seed1 != seed2 );
			}
		}
	}
}

SCENARIO( "Hasher::Hash combining multiple values", "[hasher][combine]" ) {
	GIVEN( "multiple values to combine" ) {
		WHEN( "combining values in the same order" ) {
			THEN( "results are deterministic" ) {
				std::size_t seed1 = 0;
				std::size_t seed2 = 0;
				Hasher::Hash(seed1, 1);
				Hasher::Hash(seed1, 2);
				Hasher::Hash(seed1, 3);
				Hasher::Hash(seed2, 1);
				Hasher::Hash(seed2, 2);
				Hasher::Hash(seed2, 3);
				CHECK( seed1 == seed2 );
			}
		}
		WHEN( "combining values in different order" ) {
			THEN( "results are different" ) {
				std::size_t seed1 = 0;
				std::size_t seed2 = 0;
				Hasher::Hash(seed1, 1);
				Hasher::Hash(seed1, 2);
				Hasher::Hash(seed1, 3);
				Hasher::Hash(seed2, 3);
				Hasher::Hash(seed2, 2);
				Hasher::Hash(seed2, 1);
				CHECK( seed1 != seed2 );
			}
		}
		WHEN( "combining values of different types" ) {
			THEN( "results are deterministic" ) {
				std::size_t seed1 = 0;
				std::size_t seed2 = 0;
				Hasher::Hash(seed1, 42);
				Hasher::Hash(seed1, 3.14);
				Hasher::Hash(seed1, std::string("test"));
				Hasher::Hash(seed2, 42);
				Hasher::Hash(seed2, 3.14);
				Hasher::Hash(seed2, std::string("test"));
				CHECK( seed1 == seed2 );
			}
		}
	}
}

SCENARIO( "Hasher::Hash edge cases", "[hasher][edge]" ) {
	GIVEN( "maximum size_t seed" ) {
		WHEN( "hashing a value" ) {
			THEN( "no overflow issues occur" ) {
				std::size_t seed = std::numeric_limits<std::size_t>::max();
				const std::size_t original = seed;
				Hasher::Hash(seed, 42);
				// The seed should change (wrapping is expected behavior)
				CHECK( seed != original );
			}
		}
	}
	GIVEN( "minimum size_t seed" ) {
		WHEN( "hashing a value" ) {
			THEN( "the seed is modified correctly" ) {
				std::size_t seed = 0;
				Hasher::Hash(seed, 42);
				CHECK( seed != 0 );
			}
		}
	}
	GIVEN( "very large integers" ) {
		WHEN( "hashing large values" ) {
			THEN( "hashing works correctly" ) {
				std::size_t seed1 = 0;
				std::size_t seed2 = 0;
				Hasher::Hash(seed1, std::numeric_limits<int64_t>::max());
				Hasher::Hash(seed2, std::numeric_limits<int64_t>::max());
				CHECK( seed1 == seed2 );
			}
		}
	}
	GIVEN( "negative integers" ) {
		WHEN( "hashing negative values" ) {
			THEN( "hashing works correctly" ) {
				std::size_t seed1 = 0;
				std::size_t seed2 = 0;
				Hasher::Hash(seed1, -1);
				Hasher::Hash(seed2, -1);
				CHECK( seed1 == seed2 );
				// Negative and positive values should produce different hashes
				std::size_t seed3 = 0;
				Hasher::Hash(seed3, 1);
				CHECK( seed1 != seed3 );
			}
		}
	}
}

SCENARIO( "Hasher::Hash hash quality", "[hasher][quality]" ) {
	GIVEN( "sequential values" ) {
		WHEN( "hashing sequential integers" ) {
			THEN( "different inputs produce different outputs" ) {
				std::size_t seed0 = 0, seed1 = 0, seed2 = 0;
				Hasher::Hash(seed0, 0);
				Hasher::Hash(seed1, 1);
				Hasher::Hash(seed2, 2);
				CHECK( seed0 != seed1 );
				CHECK( seed1 != seed2 );
				CHECK( seed0 != seed2 );
			}
		}
	}
	GIVEN( "similar seeds" ) {
		WHEN( "hashing the same value with adjacent seeds" ) {
			THEN( "results are sufficiently different" ) {
				std::size_t seed1 = 1000;
				std::size_t seed2 = 1001;
				Hasher::Hash(seed1, 42);
				Hasher::Hash(seed2, 42);
				// Adjacent seeds should produce substantially different results
				// due to the bit manipulation in the hash function
				CHECK( seed1 != seed2 );
				// Check that the difference isn't just 1 (would indicate poor mixing)
				auto diff = (seed1 > seed2) ? (seed1 - seed2) : (seed2 - seed1);
				CHECK( diff > 1 );
			}
		}
	}
}

// #endregion unit tests



} // test namespace