/* test_customEvents.cpp
Copyright (c) 2025 by TomGoodIdea

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
#include "../../../source/CustomEvents.h"

// ... and any system includes needed for the test file.
#include <SDL2/SDL.h>

namespace { // test namespace

// #region mock data

// Fixture for SDL initialization in tests that require it.
struct SDLFixture {
	SDLFixture()
	{
		// Initialize SDL events subsystem only.
		SDL_InitSubSystem(SDL_INIT_EVENTS);
	}
	~SDLFixture()
	{
		// Clean up any events in the queue.
		SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
		SDL_QuitSubSystem(SDL_INIT_EVENTS);
	}
};

// Reset CustomEvents internal state by reinitializing.
// Note: CustomEvents uses an anonymous namespace variable initialized to -1,
// so calling Init() again will re-register the event (SDL_RegisterEvents returns
// the same value for already-registered events if called with the same count).
void ResetCustomEvents()
{
	// Flush any pending events before reset.
	SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
	CustomEvents::Init();
}

// #endregion mock data



// #region unit tests

SCENARIO("CustomEvents initialization", "[CustomEvents][init]") {
	GIVEN("SDL is initialized") {
		SDLFixture sdl;

		WHEN("Init() is called") {
			ResetCustomEvents();

			THEN("GetResize() returns a valid event type") {
				Uint32 eventType = CustomEvents::GetResize();
				// SDL_RegisterEvents returns a value in the range [SDL_USEREVENT, SDL_LASTEVENT]
				// or 0 on failure (which is less than SDL_USEREVENT).
				CHECK(eventType >= SDL_USEREVENT);
				CHECK(eventType <= SDL_LASTEVENT);
			}
		}
	}
}

SCENARIO("CustomEvents::GetResize returns consistent values", "[CustomEvents][GetResize]") {
	GIVEN("SDL is initialized and CustomEvents is initialized") {
		SDLFixture sdl;
		ResetCustomEvents();

		WHEN("GetResize() is called multiple times") {
			Uint32 first = CustomEvents::GetResize();
			Uint32 second = CustomEvents::GetResize();
			Uint32 third = CustomEvents::GetResize();

			THEN("all calls return the same value") {
				CHECK(first == second);
				CHECK(second == third);
				CHECK(first == third);
			}
		}
	}
}

SCENARIO("CustomEvents::SendResize pushes event to queue", "[CustomEvents][SendResize]") {
	GIVEN("SDL is initialized and CustomEvents is initialized") {
		SDLFixture sdl;
		ResetCustomEvents();

		// Clear any pending events before the test.
		SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);

		WHEN("SendResize() is called") {
			Uint32 expectedType = CustomEvents::GetResize();
			CustomEvents::SendResize();

			THEN("a resize event is pushed to the SDL event queue") {
				SDL_Event event;
				int found = 0;
				while(SDL_PollEvent(&event))
				{
					if(event.type == expectedType)
						++found;
				}
				CHECK(found == 1);
			}
		}

		AND_GIVEN("multiple SendResize calls") {
			WHEN("SendResize() is called multiple times") {
				Uint32 expectedType = CustomEvents::GetResize();
				CustomEvents::SendResize();
				CustomEvents::SendResize();
				CustomEvents::SendResize();

				THEN("multiple events are pushed to the queue") {
					SDL_Event event;
					int found = 0;
					while(SDL_PollEvent(&event))
					{
						if(event.type == expectedType)
							++found;
					}
					CHECK(found == 3);
				}
			}
		}
	}
}

SCENARIO("CustomEvents event type is unique", "[CustomEvents][uniqueness]") {
	GIVEN("SDL is initialized and CustomEvents is initialized") {
		SDLFixture sdl;
		ResetCustomEvents();

		WHEN("checking the registered event type") {
			Uint32 eventType = CustomEvents::GetResize();

			THEN("the event type is not a standard SDL event type") {
				// Custom events should be in the SDL_USEREVENT range.
				CHECK(eventType >= SDL_USEREVENT);
			}
		}
	}
}

SCENARIO("CustomEvents can be reinitialized", "[CustomEvents][reinit]") {
	GIVEN("SDL is initialized and CustomEvents was initialized once") {
		SDLFixture sdl;
		ResetCustomEvents();
		Uint32 firstType = CustomEvents::GetResize();
		SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);

		WHEN("Init() is called again") {
			CustomEvents::Init();
			Uint32 secondType = CustomEvents::GetResize();

			THEN("the event type remains valid") {
				CHECK(secondType >= SDL_USEREVENT);
				CHECK(secondType <= SDL_LASTEVENT);
			}

			THEN("the event type may change (SDL_RegisterEvents behavior is implementation-defined)") {
				// Note: SDL_RegisterEvents may return different values each time
				// depending on the implementation. We just check that both are valid.
				CHECK(firstType >= SDL_USEREVENT);
				CHECK(secondType >= SDL_USEREVENT);
			}
		}
	}
}

// Test that the event can be processed like a normal SDL event.
SCENARIO("CustomEvents resize event can be processed", "[CustomEvents][integration]") {
	GIVEN("SDL is initialized and CustomEvents is initialized") {
		SDLFixture sdl;
		ResetCustomEvents();
		SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);

		WHEN("a resize event is sent and processed") {
			Uint32 expectedType = CustomEvents::GetResize();
			CustomEvents::SendResize();

			THEN("the event can be polled and identified") {
				SDL_Event event;
				bool found = false;
				while(SDL_PollEvent(&event))
				{
					if(event.type == expectedType)
					{
						found = true;
						break;
					}
				}
				CHECK(found);
			}
		}
	}
}

// Test edge case: event queue can be flushed.
SCENARIO("CustomEvents event can be flushed from queue", "[CustomEvents][flush]") {
	GIVEN("SDL is initialized, CustomEvents is initialized, and an event was sent") {
		SDLFixture sdl;
		ResetCustomEvents();
		CustomEvents::SendResize();

		WHEN("SDL_FlushEvents is called") {
			SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);

			THEN("the event queue is empty") {
				SDL_Event event;
				int count = 0;
				while(SDL_PollEvent(&event))
				{
					++count;
				}
				CHECK(count == 0);
			}
		}
	}
}

// #endregion unit tests



} // test namespace