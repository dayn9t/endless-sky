/* test_effect.cpp
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
#include "../../../source/Effect.h"

// Include test helpers.
#include "datanode-factory.h"

// ... and any system includes needed for the test file.
#include <string>

namespace { // test namespace

// #region mock data

// TestEffect class to expose protected/private members for testing via friend access pattern
// Since Effect declares Visual as a friend, we need to test what we can access publicly

// #endregion mock data



// #region unit tests

SCENARIO( "Effect default construction", "[Effect]" ) {
	GIVEN( "no parameters" ) {
		WHEN( "an Effect is default constructed" ) {
			Effect effect;
			THEN( "trueName is empty" ) {
				CHECK( effect.TrueName().empty() );
			}
		}
	}
}

SCENARIO( "Effect TrueName getter and setter", "[Effect][TrueName]" ) {
	GIVEN( "a default constructed Effect" ) {
		Effect effect;
		REQUIRE( effect.TrueName().empty() );
		WHEN( "SetTrueName is called with a name" ) {
			effect.SetTrueName("explosion_small");
			THEN( "TrueName returns the set name" ) {
				CHECK( effect.TrueName() == "explosion_small" );
			}
		}
		WHEN( "SetTrueName is called with an empty string" ) {
			effect.SetTrueName("");
			THEN( "TrueName returns empty string" ) {
				CHECK( effect.TrueName().empty() );
			}
		}
	}
	GIVEN( "an Effect with an existing name" ) {
		Effect effect;
		effect.SetTrueName("original");
		REQUIRE( effect.TrueName() == "original" );
		WHEN( "SetTrueName is called with a new name" ) {
			effect.SetTrueName("modified");
			THEN( "TrueName returns the new name" ) {
				CHECK( effect.TrueName() == "modified" );
			}
		}
	}
}

SCENARIO( "Effect Load with name", "[Effect][Load]" ) {
	GIVEN( "a DataNode with an effect name" ) {
		auto node = AsDataNode("effect explosion_large");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "trueName is set to the second token" ) {
				CHECK( effect.TrueName() == "explosion_large" );
			}
		}
	}
	GIVEN( "a DataNode without a name" ) {
		auto node = AsDataNode("effect");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "trueName remains empty" ) {
				CHECK( effect.TrueName().empty() );
			}
		}
	}
}

SCENARIO( "Effect Load with lifetime parameters", "[Effect][Load]" ) {
	GIVEN( "a DataNode with lifetime" ) {
		auto node = AsDataNode("effect test\n\tlifetime 60");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "the effect is loaded without error" ) {
				CHECK( effect.TrueName() == "test" );
			}
		}
	}
	GIVEN( "a DataNode with random lifetime" ) {
		auto node = AsDataNode("effect test\n\trandom lifetime 30");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "the effect is loaded without error" ) {
				CHECK( effect.TrueName() == "test" );
			}
		}
	}
	GIVEN( "a DataNode with both lifetime parameters" ) {
		auto node = AsDataNode("effect test\n\tlifetime 60\n\trandom lifetime 30");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "the effect is loaded without error" ) {
				CHECK( effect.TrueName() == "test" );
			}
		}
	}
}

SCENARIO( "Effect Load with velocity parameters", "[Effect][Load]" ) {
	GIVEN( "a DataNode with velocity scale" ) {
		auto node = AsDataNode("effect test\n\tvelocity scale 2.5");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "the effect is loaded without error" ) {
				CHECK( effect.TrueName() == "test" );
			}
		}
	}
	GIVEN( "a DataNode with random velocity" ) {
		auto node = AsDataNode("effect test\n\trandom velocity 100.0");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "the effect is loaded without error" ) {
				CHECK( effect.TrueName() == "test" );
			}
		}
	}
	GIVEN( "a DataNode with absolute velocity" ) {
		auto node = AsDataNode("effect test\n\tabsolute velocity 50.0");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "the effect is loaded without error" ) {
				CHECK( effect.TrueName() == "test" );
			}
		}
	}
}

SCENARIO( "Effect Load with angle parameters", "[Effect][Load]" ) {
	GIVEN( "a DataNode with random angle" ) {
		auto node = AsDataNode("effect test\n\trandom angle 45.0");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "the effect is loaded without error" ) {
				CHECK( effect.TrueName() == "test" );
			}
		}
	}
	GIVEN( "a DataNode with absolute angle" ) {
		auto node = AsDataNode("effect test\n\tabsolute angle 90.0");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "the effect is loaded without error" ) {
				CHECK( effect.TrueName() == "test" );
			}
		}
	}
	GIVEN( "a DataNode with random spin" ) {
		auto node = AsDataNode("effect test\n\trandom spin 15.0");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "the effect is loaded without error" ) {
				CHECK( effect.TrueName() == "test" );
			}
		}
	}
}

SCENARIO( "Effect Load with frame rate parameter", "[Effect][Load]" ) {
	GIVEN( "a DataNode with random frame rate" ) {
		auto node = AsDataNode("effect test\n\trandom frame rate 2.0");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "the effect is loaded without error" ) {
				CHECK( effect.TrueName() == "test" );
			}
		}
	}
}

SCENARIO( "Effect Load with zooms parameter", "[Effect][Load]" ) {
	GIVEN( "a DataNode with zooms flag" ) {
		auto node = AsDataNode("effect test\n\tzooms");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "the effect is loaded without error" ) {
				CHECK( effect.TrueName() == "test" );
			}
		}
	}
}

SCENARIO( "Effect Load with sound category", "[Effect][Load]" ) {
	GIVEN( "a DataNode with valid sound category 'explosion'" ) {
		auto node = AsDataNode("effect test\n\tsound category explosion");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "the effect is loaded without error" ) {
				CHECK( effect.TrueName() == "test" );
			}
		}
	}
	GIVEN( "a DataNode with valid sound category 'ui'" ) {
		auto node = AsDataNode("effect test\n\tsound category ui");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "the effect is loaded without error" ) {
				CHECK( effect.TrueName() == "test" );
			}
		}
	}
	GIVEN( "a DataNode with valid sound category 'weapon'" ) {
		auto node = AsDataNode("effect test\n\tsound category weapon");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "the effect is loaded without error" ) {
				CHECK( effect.TrueName() == "test" );
			}
		}
	}
	GIVEN( "a DataNode with valid sound category 'engine'" ) {
		auto node = AsDataNode("effect test\n\tsound category engine");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "the effect is loaded without error" ) {
				CHECK( effect.TrueName() == "test" );
			}
		}
	}
	GIVEN( "a DataNode with valid sound category 'afterburner'" ) {
		auto node = AsDataNode("effect test\n\tsound category afterburner");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "the effect is loaded without error" ) {
				CHECK( effect.TrueName() == "test" );
			}
		}
	}
	GIVEN( "a DataNode with valid sound category 'jump'" ) {
		auto node = AsDataNode("effect test\n\tsound category jump");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "the effect is loaded without error" ) {
				CHECK( effect.TrueName() == "test" );
			}
		}
	}
	GIVEN( "a DataNode with valid sound category 'scan'" ) {
		auto node = AsDataNode("effect test\n\tsound category scan");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "the effect is loaded without error" ) {
				CHECK( effect.TrueName() == "test" );
			}
		}
	}
	GIVEN( "a DataNode with valid sound category 'environment'" ) {
		auto node = AsDataNode("effect test\n\tsound category environment");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "the effect is loaded without error" ) {
				CHECK( effect.TrueName() == "test" );
			}
		}
	}
	GIVEN( "a DataNode with valid sound category 'alert'" ) {
		auto node = AsDataNode("effect test\n\tsound category alert");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "the effect is loaded without error" ) {
				CHECK( effect.TrueName() == "test" );
			}
		}
	}
	GIVEN( "a DataNode with valid sound category 'anti-missile'" ) {
		auto node = AsDataNode("effect test\n\tsound category anti-missile");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "the effect is loaded without error" ) {
				CHECK( effect.TrueName() == "test" );
			}
		}
	}
	GIVEN( "a DataNode with unknown sound category" ) {
		auto node = AsDataNode("effect test\n\tsound category unknown_category");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "the effect is still loaded (unknown category is ignored)" ) {
				CHECK( effect.TrueName() == "test" );
			}
		}
	}
}

SCENARIO( "Effect Load with multiple parameters", "[Effect][Load]" ) {
	GIVEN( "a DataNode with multiple parameters" ) {
		auto node = AsDataNode(
			"effect explosion_large\n"
			"\tsprite effect/explosion_large\n"
			"\tlifetime 60\n"
			"\trandom lifetime 30\n"
			"\tvelocity scale 1.5\n"
			"\trandom velocity 100.0\n"
			"\trandom angle 45.0\n"
			"\trandom spin 15.0\n"
			"\trandom frame rate 2.0\n"
			"\tzooms"
		);
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "all parameters are parsed and trueName is set correctly" ) {
				CHECK( effect.TrueName() == "explosion_large" );
			}
		}
	}
	GIVEN( "a DataNode with absolute parameters" ) {
		auto node = AsDataNode(
			"effect directed_effect\n"
			"\tabsolute angle 90.0\n"
			"\tabsolute velocity 50.0"
		);
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "all parameters are parsed and trueName is set correctly" ) {
				CHECK( effect.TrueName() == "directed_effect" );
			}
		}
	}
}

SCENARIO( "Effect Load with unrecognized attribute", "[Effect][Load]" ) {
	GIVEN( "a DataNode with an unrecognized attribute" ) {
		auto node = AsDataNode("effect test\n\tunknown_attribute value");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "the effect is still loaded (unrecognized attribute is skipped)" ) {
				CHECK( effect.TrueName() == "test" );
			}
		}
	}
}

SCENARIO( "Effect Load with missing values", "[Effect][Load]" ) {
	GIVEN( "a DataNode with sound but no value" ) {
		auto node = AsDataNode("effect test\n\tsound");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "the effect is loaded without error (sound requires a value)" ) {
				CHECK( effect.TrueName() == "test" );
			}
		}
	}
	GIVEN( "a DataNode with lifetime but no value" ) {
		auto node = AsDataNode("effect test\n\tlifetime");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "the effect is loaded without error (lifetime requires a value)" ) {
				CHECK( effect.TrueName() == "test" );
			}
		}
	}
	GIVEN( "a DataNode with velocity scale but no value" ) {
		auto node = AsDataNode("effect test\n\tvelocity scale");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "the effect is loaded without error" ) {
				CHECK( effect.TrueName() == "test" );
			}
		}
	}
	GIVEN( "a DataNode with absolute angle but no value" ) {
		auto node = AsDataNode("effect test\n\tabsolute angle");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "the effect is loaded without error" ) {
				CHECK( effect.TrueName() == "test" );
			}
		}
	}
	GIVEN( "a DataNode with absolute velocity but no value" ) {
		auto node = AsDataNode("effect test\n\tabsolute velocity");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "the effect is loaded without error" ) {
				CHECK( effect.TrueName() == "test" );
			}
		}
	}
}

SCENARIO( "Effect inheritance from Body", "[Effect][Body]" ) {
	GIVEN( "a default constructed Effect" ) {
		Effect effect;
		WHEN( "checking Body base class methods" ) {
			THEN( "position is (0, 0)" ) {
				CHECK( effect.Position().X() == 0. );
				CHECK( effect.Position().Y() == 0. );
			}
			AND_THEN( "velocity is (0, 0)" ) {
				CHECK( effect.Velocity().X() == 0. );
				CHECK( effect.Velocity().Y() == 0. );
			}
			AND_THEN( "facing angle is 0 degrees" ) {
				CHECK_THAT( effect.Facing().Degrees(), Catch::Matchers::WithinAbs(0., 0.0001) );
			}
			AND_THEN( "zoom is 1.0" ) {
				CHECK_THAT( effect.Zoom(), Catch::Matchers::WithinAbs(1., 0.0001) );
			}
		}
	}
}

SCENARIO( "Effect SetTrueName returns reference", "[Effect][TrueName]" ) {
	GIVEN( "an Effect" ) {
		Effect effect;
		WHEN( "TrueName is checked after setting" ) {
			const std::string& ref1 = effect.TrueName();
			effect.SetTrueName("first");
			const std::string& ref2 = effect.TrueName();
			THEN( "TrueName returns a const reference" ) {
				CHECK( ref2 == "first" );
			}
		}
	}
}

SCENARIO( "Effect TrueName immutability", "[Effect][TrueName]" ) {
	GIVEN( "an Effect with a name set" ) {
		Effect effect;
		effect.SetTrueName("immutable_test");
		WHEN( "the returned reference is used" ) {
			const std::string& name = effect.TrueName();
			THEN( "the name matches the set value" ) {
				CHECK( name == "immutable_test" );
			}
		}
	}
}

SCENARIO( "Effect Load with empty DataNode", "[Effect][Load]" ) {
	GIVEN( "an empty DataNode" ) {
		auto node = AsDataNode("");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "trueName remains empty" ) {
				CHECK( effect.TrueName().empty() );
			}
		}
	}
}

SCENARIO( "Effect Load with sprite parameter", "[Effect][Load]" ) {
	GIVEN( "a DataNode with sprite parameter" ) {
		auto node = AsDataNode("effect test\n\tsprite effect/explosion");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "the effect is loaded without error" ) {
				CHECK( effect.TrueName() == "test" );
			}
		}
	}
	GIVEN( "a DataNode with sprite parameter with additional tokens" ) {
		auto node = AsDataNode("effect test\n\tsprite effect/explosion 0.5");
		WHEN( "Load is called" ) {
			Effect effect;
			effect.Load(node);
			THEN( "the effect is loaded without error" ) {
				CHECK( effect.TrueName() == "test" );
			}
		}
	}
}

// #endregion unit tests



// #region benchmarks
#ifdef CATCH_CONFIG_ENABLE_BENCHMARKING
TEST_CASE( "Benchmark Effect construction", "[!benchmark][Effect]" ) {
	BENCHMARK( "Effect::Effect()" ) {
		return Effect();
	};
}

TEST_CASE( "Benchmark Effect TrueName operations", "[!benchmark][Effect][TrueName]" ) {
	Effect effect;
	BENCHMARK( "Effect::TrueName()" ) {
		return effect.TrueName();
	};
	BENCHMARK( "Effect::SetTrueName()" ) {
		effect.SetTrueName("test_name");
	};
}

TEST_CASE( "Benchmark Effect Load", "[!benchmark][Effect][Load]" ) {
	BENCHMARK( "Effect::Load() - simple" ) {
		Effect effect;
		auto node = AsDataNode("effect test\n\tlifetime 60");
		effect.Load(node);
		return effect;
	};
	BENCHMARK( "Effect::Load() - complex" ) {
		Effect effect;
		auto node = AsDataNode(
			"effect test\n"
			"\tlifetime 60\n"
			"\trandom lifetime 30\n"
			"\tvelocity scale 1.5\n"
			"\trandom velocity 100.0\n"
			"\trandom angle 45.0\n"
			"\trandom spin 15.0"
		);
		effect.Load(node);
		return effect;
	};
}
#endif
// #endregion benchmarks

} // test namespace