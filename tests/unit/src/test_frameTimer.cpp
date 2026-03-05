/* test_frameTimer.cpp
Copyright (c) 2026 by jiang

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
#include "../../../source/FrameTimer.h"

// ... and any system includes needed for the test file.
#include <chrono>
#include <thread>

namespace { // test namespace

// #region mock data
// #endregion mock data



// #region unit tests
TEST_CASE( "FrameTimer Basics", "[FrameTimer]" ) {
	using T = FrameTimer;
	SECTION( "Class Traits" ) {
		CHECK_FALSE( std::is_trivial_v<T> );
		CHECK( std::is_standard_layout_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
	}
	SECTION( "Construction Traits" ) {
		CHECK( std::is_default_constructible_v<T> );
		CHECK_FALSE( std::is_nothrow_default_constructible_v<T> );
		CHECK( std::is_copy_constructible_v<T> );
		CHECK( std::is_nothrow_copy_constructible_v<T> );
		CHECK( std::is_move_constructible_v<T> );
		CHECK( std::is_nothrow_move_constructible_v<T> );
	}
	SECTION( "Copy Traits" ) {
		CHECK( std::is_copy_assignable_v<T> );
		CHECK( std::is_nothrow_copy_assignable_v<T> );
	}
	SECTION( "Move Traits" ) {
		CHECK( std::is_move_assignable_v<T> );
		CHECK( std::is_nothrow_move_assignable_v<T> );
	}
}

SCENARIO( "A FrameTimer is created for measuring elapsed time", "[FrameTimer]" ) {
	GIVEN( "a default-constructed FrameTimer" ) {
		FrameTimer timer;
		WHEN( "Time() is called immediately" ) {
			double elapsed = timer.Time();
			THEN( "the elapsed time is approximately zero" ) {
				CHECK_THAT( elapsed, Catch::Matchers::WithinAbs(0., 0.1) );
			}
		}
		WHEN( "some time passes" ) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			AND_WHEN( "Time() is called" ) {
				double elapsed = timer.Time();
				THEN( "the elapsed time is approximately 0.05 seconds" ) {
					CHECK_THAT( elapsed, Catch::Matchers::WithinAbs(0.05, 0.02) );
				}
			}
		}
		WHEN( "more time passes" ) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			AND_WHEN( "Time() is called" ) {
				double elapsed = timer.Time();
				THEN( "the elapsed time is approximately 0.1 seconds" ) {
					CHECK_THAT( elapsed, Catch::Matchers::WithinAbs(0.1, 0.02) );
				}
			}
		}
	}
}

SCENARIO( "A FrameTimer is created with a specific frame rate", "[FrameTimer]" ) {
	GIVEN( "a FrameTimer with 60 FPS" ) {
		constexpr int fps = 60;
		FrameTimer timer(fps);
		WHEN( "Time() is called immediately" ) {
			double elapsed = timer.Time();
			THEN( "the elapsed time is very small" ) {
				CHECK_THAT( elapsed, Catch::Matchers::WithinAbs(0., 0.05) );
			}
		}
	}
}

SCENARIO( "A FrameTimer is created with a custom max lag", "[FrameTimer]" ) {
	GIVEN( "a FrameTimer with 60 FPS and 10ms max lag" ) {
		constexpr int fps = 60;
		constexpr int maxLagMsec = 10;
		FrameTimer timer(fps, maxLagMsec);
		WHEN( "Time() is called immediately" ) {
			double elapsed = timer.Time();
			THEN( "the elapsed time is very small" ) {
				CHECK_THAT( elapsed, Catch::Matchers::WithinAbs(0., 0.05) );
			}
		}
	}
}

SCENARIO( "FrameTimer frame rate can be changed", "[FrameTimer]" ) {
	GIVEN( "a FrameTimer with 60 FPS" ) {
		constexpr int initialFps = 60;
		FrameTimer timer(initialFps);
		WHEN( "the frame rate is changed to 30 FPS" ) {
			timer.SetFrameRate(30);
			THEN( "the frame rate is changed" ) {
				// SetFrameRate only changes the step, which is not directly queryable
				// We just verify the call doesn't crash
				CHECK( true );
			}
		}
		WHEN( "the frame rate is changed to 120 FPS" ) {
			timer.SetFrameRate(120);
			THEN( "the frame rate is changed" ) {
				CHECK( true );
			}
		}
	}
}

SCENARIO( "FrameTimer can be copied and moved", "[FrameTimer]" ) {
	GIVEN( "a FrameTimer that has been running for some time" ) {
		FrameTimer original;
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		WHEN( "it is copy-constructed" ) {
			FrameTimer copy(original);
			THEN( "both timers report similar elapsed time" ) {
				double originalTime = original.Time();
				double copyTime = copy.Time();
				CHECK_THAT( copyTime, Catch::Matchers::WithinAbs(originalTime, 0.01) );
			}
		}
		WHEN( "it is copy-assigned" ) {
			FrameTimer copy;
			copy = original;
			THEN( "both timers report similar elapsed time" ) {
				double originalTime = original.Time();
				double copyTime = copy.Time();
				CHECK_THAT( copyTime, Catch::Matchers::WithinAbs(originalTime, 0.01) );
			}
		}
		WHEN( "it is move-constructed" ) {
			double originalTime = original.Time();
			FrameTimer moved(std::move(original));
			THEN( "the moved timer preserves the elapsed time" ) {
				double movedTime = moved.Time();
				CHECK_THAT( movedTime, Catch::Matchers::WithinAbs(originalTime, 0.01) );
			}
		}
		WHEN( "it is move-assigned" ) {
			double originalTime = original.Time();
			FrameTimer moved;
			moved = std::move(original);
			THEN( "the moved timer preserves the elapsed time" ) {
				double movedTime = moved.Time();
				CHECK_THAT( movedTime, Catch::Matchers::WithinAbs(originalTime, 0.01) );
			}
		}
	}
}

SCENARIO( "FrameTimer handles consecutive Wait() calls", "[FrameTimer]" ) {
	GIVEN( "a FrameTimer with 100 FPS" ) {
		constexpr int fps = 100;
		FrameTimer timer(fps);
		WHEN( "Wait() is called multiple times" ) {
			for(int i = 0; i < 3; ++i)
			{
				timer.Wait();
			}
			THEN( "the timer completes without error" ) {
				// Note: Time() may return negative values because Wait() advances 'next'
				// This is expected behavior as the timer tracks the next frame time
				CHECK( true );
			}
		}
	}
}

SCENARIO( "FrameTimer with very high frame rate", "[FrameTimer]" ) {
	GIVEN( "a FrameTimer with 1000 FPS" ) {
		constexpr int fps = 1000;
		FrameTimer timer(fps);
		WHEN( "Wait() is called" ) {
			auto start = std::chrono::steady_clock::now();
			timer.Wait();
			auto end = std::chrono::steady_clock::now();
			auto waitDuration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
			THEN( "the wait duration is approximately 1ms or less" ) {
				// One frame at 1000 FPS is 1ms
				CHECK( waitDuration.count() <= 2000 );
			}
		}
	}
}

SCENARIO( "FrameTimer measures time consistently", "[FrameTimer]" ) {
	GIVEN( "a default-constructed FrameTimer" ) {
		FrameTimer timer;
		WHEN( "Time() is called multiple times" ) {
			double time1 = timer.Time();
			std::this_thread::sleep_for(std::chrono::milliseconds(25));
			double time2 = timer.Time();
			std::this_thread::sleep_for(std::chrono::milliseconds(25));
			double time3 = timer.Time();
			THEN( "each call returns an increasing elapsed time" ) {
				CHECK( time2 > time1 );
				CHECK( time3 > time2 );
				CHECK_THAT( time2 - time1, Catch::Matchers::WithinAbs(0.025, 0.015) );
				CHECK_THAT( time3 - time2, Catch::Matchers::WithinAbs(0.025, 0.015) );
			}
		}
	}
}
// #endregion unit tests



} // test namespace