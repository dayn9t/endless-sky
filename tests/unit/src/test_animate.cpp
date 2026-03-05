/* test_animate.cpp
Copyright (c) 2025 by thewierdnut

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
#include "../../../source/Animate.h"

// ... and any system includes needed for the test file.

namespace { // test namespace

// #region mock data
// #endregion mock data



// #region unit tests
SCENARIO( "Creating an Animate object", "[Animate][Creation]" ) {
	GIVEN( "a default-constructed Animate<double>" ) {
		Animate<double> anim;
		THEN( "it has default-initialized values and no animation pending" ) {
			CHECK( anim.AnimatedValue() == 0.0 );
			CHECK( anim.Value() == 0.0 );
			CHECK( anim.IsAnimationDone() );
		}
		WHEN( "converted implicitly to its value type" ) {
			double value = anim;
			THEN( "it returns the target value" ) {
				CHECK( value == 0.0 );
			}
		}
	}

	GIVEN( "a default-constructed Animate<int>" ) {
		Animate<int> anim;
		THEN( "it has default-initialized values and no animation pending" ) {
			CHECK( anim.AnimatedValue() == 0 );
			CHECK( anim.Value() == 0 );
			CHECK( anim.IsAnimationDone() );
		}
	}
}

SCENARIO( "Setting animation target values", "[Animate][Set]" ) {
	GIVEN( "an Animate<double> with no animation" ) {
		Animate<double> anim;
		WHEN( "Set is called with a target value" ) {
			anim.Set(100.0);
			THEN( "the target value is set with default 5 steps" ) {
				CHECK( anim.Value() == 100.0 );
				CHECK( anim.AnimatedValue() == 0.0 );
				CHECK_FALSE( anim.IsAnimationDone() );
			}
		}
		WHEN( "Set is called with a target and custom step count" ) {
			anim.Set(50.0, 10);
			THEN( "the target value is set with the specified steps" ) {
				CHECK( anim.Value() == 50.0 );
				CHECK( anim.AnimatedValue() == 0.0 );
				CHECK_FALSE( anim.IsAnimationDone() );
			}
		}
		WHEN( "Set is called with zero steps" ) {
			anim.Set(200.0, 0);
			THEN( "the animation is already done" ) {
				CHECK( anim.Value() == 200.0 );
				CHECK( anim.IsAnimationDone() );
			}
		}
	}

	GIVEN( "an Animate<int> with no animation" ) {
		Animate<int> anim;
		WHEN( "Set is called with a negative target" ) {
			anim.Set(-50, 3);
			THEN( "the target value is set correctly" ) {
				CHECK( anim.Value() == -50 );
				CHECK_FALSE( anim.IsAnimationDone() );
			}
		}
	}
}

SCENARIO( "Stepping through animation", "[Animate][Step]" ) {
	GIVEN( "an Animate<double> set to animate to 100 in 5 steps" ) {
		Animate<double> anim;
		anim.Set(100.0, 5);
		WHEN( "Step is called once" ) {
			anim.Step();
			THEN( "the animated value advances by one step increment" ) {
				// delta = (100 - 0) / 5 = 20
				CHECK( anim.AnimatedValue() == 20.0 );
				CHECK( anim.Value() == 100.0 );
				CHECK_FALSE( anim.IsAnimationDone() );
			}
		}
		WHEN( "Step is called 5 times" ) {
			for(int i = 0; i < 5; ++i)
				anim.Step();
			THEN( "the animation completes and reaches the target" ) {
				CHECK( anim.AnimatedValue() == 100.0 );
				CHECK( anim.Value() == 100.0 );
				CHECK( anim.IsAnimationDone() );
			}
		}
		WHEN( "Step is called after animation is done" ) {
			for(int i = 0; i < 5; ++i)
				anim.Step();
			CHECK( anim.IsAnimationDone() );
			anim.Step();
			THEN( "Step has no further effect" ) {
				CHECK( anim.AnimatedValue() == 100.0 );
				CHECK( anim.IsAnimationDone() );
			}
		}
	}

	GIVEN( "an Animate<double> with a single step animation" ) {
		Animate<double> anim;
		anim.Set(50.0, 1);
		WHEN( "Step is called once" ) {
			anim.Step();
			THEN( "the animation completes immediately" ) {
				CHECK( anim.AnimatedValue() == 50.0 );
				CHECK( anim.IsAnimationDone() );
			}
		}
	}

	GIVEN( "an Animate<double> animating from non-zero starting point" ) {
		Animate<double> anim;
		anim.Set(100.0, 5);
		// Step 4 times to get close
		for(int i = 0; i < 4; ++i)
			anim.Step();
		WHEN( "Step is called the final time" ) {
			anim.Step();
			THEN( "the animated value matches the target exactly" ) {
				CHECK( anim.AnimatedValue() == 100.0 );
				CHECK( anim.Value() == 100.0 );
				CHECK( anim.IsAnimationDone() );
			}
		}
	}
}

SCENARIO( "Ending animation early", "[Animate][EndAnimation]" ) {
	GIVEN( "an Animate<double> in the middle of animation" ) {
		Animate<double> anim;
		anim.Set(100.0, 10);
		anim.Step(); // Animated value = 10.0
		anim.Step(); // Animated value = 20.0
		WHEN( "EndAnimation is called" ) {
			anim.EndAnimation();
			THEN( "the animation is marked as done but animated value unchanged" ) {
				CHECK( anim.IsAnimationDone() );
				CHECK( anim.AnimatedValue() == 20.0 ); // Still at previous position
				CHECK( anim.Value() == 100.0 );
			}
			AND_WHEN( "Step is called after EndAnimation" ) {
				anim.Step();
				THEN( "the animated value jumps to target" ) {
					CHECK( anim.AnimatedValue() == 100.0 );
					CHECK( anim.IsAnimationDone() );
				}
			}
		}
	}
}

SCENARIO( "Using assignment operators", "[Animate][Operators]" ) {
	GIVEN( "an Animate<double> with no animation" ) {
		Animate<double> anim;
		WHEN( "using assignment operator" ) {
			anim = 42.0;
			THEN( "the target is set with default steps" ) {
				CHECK( anim.Value() == 42.0 );
				CHECK_FALSE( anim.IsAnimationDone() );
			}
		}
		WHEN( "using += operator" ) {
			anim.Set(10.0, 5);
			anim.Step(); // Animated at 2.0
			anim += 5.0;
			THEN( "the target is increased by the amount" ) {
				CHECK( anim.Value() == 15.0 );
			}
		}
		WHEN( "using -= operator" ) {
			anim.Set(20.0, 5);
			anim.Step(); // Animated at 4.0
			anim -= 8.0;
			THEN( "the target is decreased by the amount" ) {
				CHECK( anim.Value() == 12.0 );
			}
		}
	}

	GIVEN( "an Animate<int> with no animation" ) {
		Animate<int> anim;
		WHEN( "using assignment operator" ) {
			anim = 100;
			THEN( "the target is set with default steps" ) {
				CHECK( anim.Value() == 100 );
				CHECK_FALSE( anim.IsAnimationDone() );
			}
		}
		WHEN( "chaining assignment operators" ) {
			anim = 10;
			anim += 5;
			anim -= 3;
			THEN( "all operations are applied to target" ) {
				CHECK( anim.Value() == 12 );
			}
		}
	}
}

SCENARIO( "Implicit conversion to value type", "[Animate][Conversion]" ) {
	GIVEN( "an Animate<double> with a target value" ) {
		Animate<double> anim;
		anim.Set(75.5, 5);
		WHEN( "implicitly converted to double" ) {
			double value = anim;
			THEN( "it returns the target value, not animated value" ) {
				CHECK( value == 75.5 );
				CHECK( value != anim.AnimatedValue() );
			}
		}
		WHEN( "used in a comparison" ) {
			THEN( "it compares against the target value" ) {
				CHECK( anim == 75.5 );
				CHECK_FALSE( anim == 0.0 );
			}
		}
	}
}

SCENARIO( "Animation with different step counts", "[Animate][Steps]" ) {
	GIVEN( "an Animate<double> with a large step count" ) {
		Animate<double> anim;
		anim.Set(1000.0, 100);
		WHEN( "stepping through partial animation" ) {
			for(int i = 0; i < 50; ++i)
				anim.Step();
			THEN( "animation is half complete" ) {
				CHECK_FALSE( anim.IsAnimationDone() );
				// Each step: delta = 1000/100 = 10
				// After 50 steps: 50 * 10 = 500
				CHECK( anim.AnimatedValue() == Catch::Approx(500.0) );
			}
		}
	}

	GIVEN( "an Animate<double> with step count of 1" ) {
		Animate<double> anim;
		anim.Set(123.0, 1);
		WHEN( "stepping once" ) {
			anim.Step();
			THEN( "animation completes immediately" ) {
				CHECK( anim.IsAnimationDone() );
				CHECK( anim.AnimatedValue() == 123.0 );
			}
		}
	}
}

SCENARIO( "Changing target during animation", "[Animate][Interrupt]" ) {
	GIVEN( "an Animate<double> animating to a target" ) {
		Animate<double> anim;
		anim.Set(100.0, 10);
		anim.Step(); // Animated at 10.0
		WHEN( "a new target is set before animation completes" ) {
			anim.Set(200.0, 5);
			THEN( "the animation restarts from current position" ) {
				CHECK( anim.Value() == 200.0 );
				CHECK( anim.AnimatedValue() == 10.0 ); // Still at previous position
				CHECK_FALSE( anim.IsAnimationDone() );
			}
			AND_WHEN( "stepping through the new animation" ) {
				for(int i = 0; i < 5; ++i)
					anim.Step();
				THEN( "animation reaches the new target" ) {
					CHECK( anim.AnimatedValue() == Catch::Approx(200.0) );
					CHECK( anim.IsAnimationDone() );
				}
			}
		}
	}
}

SCENARIO( "Negative values and direction", "[Animate][Negative]" ) {
	GIVEN( "an Animate<double> animating to a negative value" ) {
		Animate<double> anim;
		anim.Set(-100.0, 5);
		WHEN( "stepping through the animation" ) {
			for(int i = 0; i < 5; ++i)
				anim.Step();
			THEN( "the animation reaches the negative target" ) {
				CHECK( anim.AnimatedValue() == -100.0 );
				CHECK( anim.Value() == -100.0 );
			}
		}
	}

	GIVEN( "an Animate<double> starting positive going negative" ) {
		Animate<double> anim;
		anim = 50.0;
		anim.Step(); // Animated at 10.0
		anim.Step(); // Animated at 20.0
		anim.Set(-50.0, 5);
		WHEN( "stepping through the animation" ) {
			for(int i = 0; i < 5; ++i)
				anim.Step();
			THEN( "the animation transitions through zero to negative" ) {
				CHECK( anim.AnimatedValue() == Catch::Approx(-50.0) );
			}
		}
	}
}

SCENARIO( "Integer division in animation", "[Animate][Integer]" ) {
	GIVEN( "an Animate<int> with non-divisible step increment" ) {
		Animate<int> anim;
		anim.Set(10, 3);
		WHEN( "stepping through the animation" ) {
			anim.Step(); // delta = 10/3 = 3 (integer division)
			THEN( "first step uses integer division" ) {
				CHECK( anim.AnimatedValue() == 3 );
			}
			AND_WHEN( "stepping again" ) {
				anim.Step(); // delta = (10-3)/2 = 3 (remaining: 7, steps: 2)
				THEN( "second step continues" ) {
					CHECK( anim.AnimatedValue() == 6 );
				}
				AND_WHEN( "final step" ) {
					anim.Step(); // delta = (10-6)/1 = 4
					THEN( "final step reaches target" ) {
						CHECK( anim.AnimatedValue() == 10 );
						CHECK( anim.IsAnimationDone() );
					}
				}
			}
		}
	}
}

SCENARIO( "Value method returns target", "[Animate][Value]" ) {
	GIVEN( "an Animate<double> with target set" ) {
		Animate<double> anim;
		anim.Set(42.5, 10);
		WHEN( "querying Value()" ) {
			THEN( "it always returns the target regardless of animation state" ) {
				CHECK( anim.Value() == 42.5 );
				anim.Step();
				CHECK( anim.Value() == 42.5 );
				anim.Step();
				CHECK( anim.Value() == 42.5 );
			}
		}
	}
}

SCENARIO( "AnimatedValue returns interpolated value", "[Animate][AnimatedValue]" ) {
	GIVEN( "an Animate<double> animating from 0 to 50 in 5 steps" ) {
		Animate<double> anim;
		anim.Set(50.0, 5);
		WHEN( "checking animated value progression" ) {
			THEN( "animated value increments by 10 each step" ) {
				CHECK( anim.AnimatedValue() == 0.0 );
				anim.Step();
				CHECK( anim.AnimatedValue() == 10.0 );
				anim.Step();
				CHECK( anim.AnimatedValue() == 20.0 );
				anim.Step();
				CHECK( anim.AnimatedValue() == 30.0 );
				anim.Step();
				CHECK( anim.AnimatedValue() == 40.0 );
				anim.Step();
				CHECK( anim.AnimatedValue() == 50.0 );
			}
		}
	}
}

// #endregion unit tests



} // test namespace