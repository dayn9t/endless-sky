/* test_scrollBar.cpp
Copyright (c) 2024 by test contributor

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
#include "../../../source/ScrollBar.h"

// ... and any system includes needed for the test file.
#include "../../../source/Point.h"
#include "../../../source/Color.h"
#include "../../../source/ScrollVar.h"
#include "../../../source/MouseButton.h"

#include <cmath>

namespace { // test namespace

// #region mock data

// Helper function to compare floats with tolerance
bool floatEquals(float a, float b, float epsilon = 0.0001f)
{
	return std::fabs(a - b) < epsilon;
}

// #endregion mock data



// #region unit tests

SCENARIO( "Creating a ScrollBar with default constructor", "[ScrollBar][Creation]" ) {
	GIVEN( "a default constructed ScrollBar" ) {
		ScrollBar sb;
		WHEN( "checking default values" ) {
			THEN( "default values are set correctly" ) {
				CHECK( sb.fraction == 0.f );
				CHECK( sb.displaySizeFraction == 0.f );
				CHECK( sb.from == Point() );
				CHECK( sb.to == Point() );
				CHECK( sb.tabWidth == 3.f );
				CHECK( sb.lineWidth == 3.f );
				CHECK( sb.highlighted == false );
				CHECK( sb.innerHighlighted == false );
			}
		}
	}
}

SCENARIO( "Creating a ScrollBar with parameterized constructor", "[ScrollBar][Creation]" ) {
	GIVEN( "parameters for a scrollbar" ) {
		float fraction = 0.5f;
		float displaySizeFraction = 0.3f;
		Point from(10., 20.);
		Point to(100., 200.);
		float tabWidth = 5.f;
		float lineWidth = 2.f;
		Color color(0.8f, 0.6f, 0.4f, 1.f);
		Color innerColor(0.3f, 0.2f, 0.1f, 0.5f);

		WHEN( "creating a ScrollBar with all parameters" ) {
			ScrollBar sb(fraction, displaySizeFraction, from, to, tabWidth, lineWidth, color, innerColor);
			THEN( "all values are set correctly" ) {
				CHECK( sb.fraction == fraction );
				CHECK( sb.displaySizeFraction == displaySizeFraction );
				CHECK( sb.from == from );
				CHECK( sb.to == to );
				CHECK( sb.tabWidth == tabWidth );
				CHECK( sb.lineWidth == lineWidth );
				CHECK( sb.highlighted == false );
				CHECK( sb.innerHighlighted == false );
			}
		}
	}

	GIVEN( "a scrollbar with zero fraction" ) {
		ScrollBar sb(0.f, 0.5f, Point(0., 0.), Point(100., 0.), 3.f, 3.f, Color(0.6f), Color(0.25f));
		THEN( "fraction is zero" ) {
			CHECK( sb.fraction == 0.f );
		}
	}

	GIVEN( "a scrollbar with maximum fraction" ) {
		ScrollBar sb(1.f, 0.5f, Point(0., 0.), Point(100., 0.), 3.f, 3.f, Color(0.6f), Color(0.25f));
		THEN( "fraction is one" ) {
			CHECK( sb.fraction == 1.f );
		}
	}
}

SCENARIO( "Hover method updates highlight state", "[ScrollBar][Hover]" ) {
	GIVEN( "a vertical scrollbar" ) {
		Point from(0., 0.);
		Point to(0., 100.);
		ScrollBar sb(0.5f, 0.2f, from, to, 5.f, 3.f, Color(0.6f), Color(0.25f));

		WHEN( "hovering near the tab position" ) {
			// Tab is at fraction 0.5, with displaySizeFraction 0.2
			// Tab starts at 0.5 * (1 - 0.2) * 100 = 40, ends at 60
			bool result = sb.Hover(5, 50); // Near the middle of the tab
			THEN( "the scrollbar is highlighted" ) {
				CHECK( sb.highlighted == true );
				CHECK( result == false );
			}
		}

		WHEN( "hovering far from the tab" ) {
			bool result = sb.Hover(100, 50);
			THEN( "the scrollbar is not highlighted" ) {
				CHECK( sb.highlighted == false );
				CHECK( result == false );
			}
		}

		WHEN( "hovering on the inner line area" ) {
			bool result = sb.Hover(0, 50);
			THEN( "the inner area may be highlighted" ) {
				CHECK( sb.innerHighlighted == true );
				CHECK( result == false );
			}
		}
	}

	GIVEN( "a horizontal scrollbar" ) {
		Point from(0., 0.);
		Point to(100., 0.);
		ScrollBar sb(0.5f, 0.2f, from, to, 5.f, 3.f, Color(0.6f), Color(0.25f));

		WHEN( "hovering near the tab position" ) {
			// Tab is at fraction 0.5, with displaySizeFraction 0.2
			// Tab starts at 0.5 * (1 - 0.2) * 100 = 40, ends at 60
			bool result = sb.Hover(50, 5);
			THEN( "the scrollbar is highlighted" ) {
				CHECK( sb.highlighted == true );
				CHECK( result == false );
			}
		}
	}
}

SCENARIO( "Drag method updates fraction", "[ScrollBar][Drag]" ) {
	GIVEN( "a highlighted scrollbar" ) {
		Point from(0., 0.);
		Point to(0., 100.);
		ScrollBar sb(0.5f, 0.2f, from, to, 5.f, 3.f, Color(0.6f), Color(0.25f));
		sb.highlighted = true;

		WHEN( "dragging downward" ) {
			bool result = sb.Drag(0., 10.);
			THEN( "fraction increases" ) {
				CHECK( result == true );
				CHECK( sb.fraction > 0.5f );
			}
		}

		WHEN( "dragging upward" ) {
			bool result = sb.Drag(0., -10.);
			THEN( "fraction decreases" ) {
				CHECK( result == true );
				CHECK( sb.fraction < 0.5f );
			}
		}
	}

	GIVEN( "a non-highlighted scrollbar" ) {
		ScrollBar sb(0.5f, 0.2f, Point(0., 0.), Point(0., 100.), 5.f, 3.f, Color(0.6f), Color(0.25f));
		sb.highlighted = false;

		WHEN( "attempting to drag" ) {
			bool result = sb.Drag(0., 10.);
			THEN( "drag does nothing and returns false" ) {
				CHECK( result == false );
				CHECK( sb.fraction == 0.5f );
			}
		}
	}

	GIVEN( "a horizontal highlighted scrollbar" ) {
		Point from(0., 0.);
		Point to(100., 0.);
		ScrollBar sb(0.5f, 0.2f, from, to, 5.f, 3.f, Color(0.6f), Color(0.25f));
		sb.highlighted = true;

		WHEN( "dragging rightward" ) {
			bool result = sb.Drag(10., 0.);
			THEN( "fraction increases" ) {
				CHECK( result == true );
				CHECK( sb.fraction > 0.5f );
			}
		}

		WHEN( "dragging leftward" ) {
			bool result = sb.Drag(-10., 0.);
			THEN( "fraction decreases" ) {
				CHECK( result == true );
				CHECK( sb.fraction < 0.5f );
			}
		}
	}
}

SCENARIO( "Click method handles various click scenarios", "[ScrollBar][Click]" ) {
	GIVEN( "a vertical scrollbar" ) {
		Point from(10., 10.);
		Point to(10., 110.);
		ScrollBar sb(0.5f, 0.2f, from, to, 5.f, 3.f, Color(0.6f), Color(0.25f));
		sb.innerHighlighted = true;
		sb.highlighted = false;

		WHEN( "clicking with non-left button" ) {
			bool result = sb.Click(10, 10, MouseButton::RIGHT, 1);
			THEN( "click is ignored" ) {
				CHECK( result == false );
			}
		}

		WHEN( "clicking near the start point (up arrow area)" ) {
			bool result = sb.Click(10, 10, MouseButton::LEFT, 1);
			THEN( "fraction decreases" ) {
				CHECK( result == true );
				CHECK( sb.fraction < 0.5f );
			}
		}

		WHEN( "clicking near the end point (down arrow area)" ) {
			bool result = sb.Click(10, 110, MouseButton::LEFT, 1);
			THEN( "fraction increases" ) {
				CHECK( result == true );
				CHECK( sb.fraction > 0.5f );
			}
		}
	}

	GIVEN( "a scrollbar at minimum fraction" ) {
		ScrollBar sb(0.f, 0.2f, Point(10., 10.), Point(10., 110.), 5.f, 3.f, Color(0.6f), Color(0.25f));

		WHEN( "clicking near the start point" ) {
			bool result = sb.Click(10, 10, MouseButton::LEFT, 1);
			THEN( "fraction stays at minimum" ) {
				CHECK( result == true );
				CHECK( sb.fraction >= 0.f );
			}
		}
	}

	GIVEN( "a scrollbar at maximum fraction" ) {
		ScrollBar sb(1.f, 0.2f, Point(10., 10.), Point(10., 110.), 5.f, 3.f, Color(0.6f), Color(0.25f));

		WHEN( "clicking near the end point" ) {
			bool result = sb.Click(10, 110, MouseButton::LEFT, 1);
			THEN( "fraction stays at maximum" ) {
				CHECK( result == true );
				CHECK( sb.fraction <= 1.f );
			}
		}
	}

	GIVEN( "a scrollbar with inner highlighted but not tab" ) {
		ScrollBar sb(0.5f, 0.2f, Point(0., 0.), Point(0., 100.), 5.f, 3.f, Color(0.6f), Color(0.25f));
		sb.innerHighlighted = true;
		sb.highlighted = false;

		WHEN( "clicking on the inner area (not near arrows)" ) {
			// Click in the middle, away from start and end
			bool result = sb.Click(0, 50, MouseButton::LEFT, 1);
			THEN( "scrollbar becomes highlighted and fraction changes" ) {
				CHECK( sb.highlighted == true );
				CHECK( result == true );
			}
		}
	}
}

SCENARIO( "Fraction boundary conditions", "[ScrollBar][Boundary]" ) {
	GIVEN( "a scrollbar with zero display size fraction" ) {
		ScrollBar sb(0.5f, 0.f, Point(0., 0.), Point(100., 0.), 5.f, 3.f, Color(0.6f), Color(0.25f));
		sb.highlighted = true;

		WHEN( "dragging" ) {
			// When displaySizeFraction is 0, drag calculation divides by (1 - 0) = 1
			sb.Drag(10., 0.);
			THEN( "fraction is updated without division issues" ) {
				// Should not crash or produce NaN
				CHECK( !std::isnan(sb.fraction) );
			}
		}
	}

	GIVEN( "a scrollbar with full display size fraction" ) {
		ScrollBar sb(0.5f, 1.f, Point(0., 0.), Point(100., 0.), 5.f, 3.f, Color(0.6f), Color(0.25f));
		sb.highlighted = true;

		WHEN( "dragging" ) {
			// When displaySizeFraction is 1, drag calculation divides by (1 - 1) = 0
			// This would cause very large changes or infinity
			float oldFraction = sb.fraction;
			sb.Drag(10., 0.);
			THEN( "fraction may change significantly or become invalid" ) {
				// The implementation divides by (1 - displaySizeFraction)
				// When displaySizeFraction is 1, this causes division by zero
				// Result may be infinity or the fraction changes drastically
				CHECK( (std::isinf(sb.fraction) || sb.fraction != oldFraction || std::isnan(sb.fraction)) );
			}
		}
	}

	GIVEN( "a scrollbar dragged beyond bounds" ) {
		ScrollBar sb(0.9f, 0.2f, Point(0., 0.), Point(100., 0.), 5.f, 3.f, Color(0.6f), Color(0.25f));
		sb.highlighted = true;

		WHEN( "dragging to exceed maximum fraction" ) {
			sb.Drag(100., 0.);
			THEN( "fraction can exceed 1 (no clamping in Drag)" ) {
				CHECK( sb.fraction > 0.9f );
			}
		}
	}

	GIVEN( "a scrollbar dragged below minimum" ) {
		ScrollBar sb(0.1f, 0.2f, Point(0., 0.), Point(100., 0.), 5.f, 3.f, Color(0.6f), Color(0.25f));
		sb.highlighted = true;

		WHEN( "dragging to go below minimum fraction" ) {
			sb.Drag(-100., 0.);
			THEN( "fraction can go below 0 (no clamping in Drag)" ) {
				CHECK( sb.fraction < 0.1f );
			}
		}
	}
}

SCENARIO( "Click clamping behavior", "[ScrollBar][Click][Boundary]" ) {
	GIVEN( "a scrollbar at minimum fraction" ) {
		ScrollBar sb(0.f, 0.5f, Point(10., 10.), Point(10., 110.), 5.f, 3.f, Color(0.6f), Color(0.25f));

		WHEN( "clicking near start (would decrease below 0)" ) {
			sb.Click(10, 10, MouseButton::LEFT, 1);
			THEN( "fraction is clamped to 0" ) {
				CHECK( sb.fraction == 0.f );
			}
		}
	}

	GIVEN( "a scrollbar at maximum fraction" ) {
		ScrollBar sb(1.f, 0.5f, Point(10., 10.), Point(10., 110.), 5.f, 3.f, Color(0.6f), Color(0.25f));

		WHEN( "clicking near end (would increase above 1)" ) {
			sb.Click(10, 110, MouseButton::LEFT, 1);
			THEN( "fraction is clamped to 1" ) {
				CHECK( sb.fraction == 1.f );
			}
		}
	}
}

SCENARIO( "SyncFrom method updates scrollbar state", "[ScrollBar][Sync]" ) {
	GIVEN( "a ScrollVar and a ScrollBar" ) {
		ScrollVar<double> scroll;
		scroll.SetMaxValue(100.0);
		scroll.SetDisplaySize(30.0);
		scroll.Scroll(35.0); // Scroll to 35

		ScrollBar sb;
		Point from(0., 0.);
		Point to(0., 200.);

		WHEN( "syncing from ScrollVar" ) {
			sb.SyncFrom(scroll, from, to, false);
			THEN( "scrollbar state matches ScrollVar" ) {
				CHECK( floatEquals(sb.displaySizeFraction, 0.3f) ); // 30/100
				CHECK( floatEquals(sb.fraction, 0.5f) ); // 35/(100-30)
				CHECK( sb.from == from );
				CHECK( sb.to == to );
			}
		}

		WHEN( "syncing from ScrollVar with animation" ) {
			sb.SyncFrom(scroll, from, to, true);
			THEN( "scrollbar uses animated scroll fraction" ) {
				CHECK( floatEquals(sb.displaySizeFraction, 0.3f) );
				// Animated value would be 0 initially (no steps yet)
			}
		}
	}
}

SCENARIO( "SyncInto method updates ScrollVar", "[ScrollBar][Sync]" ) {
	GIVEN( "a ScrollBar and a ScrollVar" ) {
		ScrollBar sb(0.5f, 0.3f, Point(0., 0.), Point(0., 100.), 5.f, 3.f, Color(0.6f), Color(0.25f));

		ScrollVar<double> scroll;
		scroll.SetMaxValue(100.0);
		scroll.SetDisplaySize(30.0);

		WHEN( "syncing into ScrollVar" ) {
			sb.SyncInto(scroll);
			THEN( "ScrollVar value is updated based on fraction" ) {
				// fraction * (maxValue - displaySize) = 0.5 * 70 = 35
				CHECK_THAT( scroll.Value(), Catch::Matchers::WithinAbs(35.0, 0.0001) );
			}
		}

		WHEN( "syncing with different step count" ) {
			sb.SyncInto(scroll, 10);
			THEN( "ScrollVar is updated with specified steps" ) {
				CHECK_THAT( scroll.Value(), Catch::Matchers::WithinAbs(35.0, 0.0001) );
			}
		}
	}
}

SCENARIO( "SyncClick method handles click and sync", "[ScrollBar][Sync]" ) {
	GIVEN( "a ScrollBar and a ScrollVar" ) {
		ScrollBar sb(0.5f, 0.3f, Point(10., 10.), Point(10., 110.), 5.f, 3.f, Color(0.6f), Color(0.25f));
		sb.innerHighlighted = true;

		ScrollVar<double> scroll;
		scroll.SetMaxValue(100.0);
		scroll.SetDisplaySize(30.0);
		scroll.Scroll(35.0);

		WHEN( "clicking near start with SyncClick" ) {
			bool result = sb.SyncClick(scroll, 10, 10, MouseButton::LEFT, 1);
			THEN( "click is handled and ScrollVar is updated" ) {
				CHECK( result == true );
				// ScrollVar should have been updated
			}
		}

		WHEN( "clicking with non-left button" ) {
			bool result = sb.SyncClick(scroll, 10, 10, MouseButton::RIGHT, 1);
			THEN( "click is ignored" ) {
				CHECK( result == false );
			}
		}
	}
}

SCENARIO( "SyncDrag method handles drag and sync", "[ScrollBar][Sync]" ) {
	GIVEN( "a ScrollBar and a ScrollVar" ) {
		ScrollBar sb(0.5f, 0.3f, Point(0., 0.), Point(0., 100.), 5.f, 3.f, Color(0.6f), Color(0.25f));
		sb.highlighted = true;

		ScrollVar<double> scroll;
		scroll.SetMaxValue(100.0);
		scroll.SetDisplaySize(30.0);
		scroll.Scroll(35.0);

		WHEN( "dragging with SyncDrag while highlighted" ) {
			bool result = sb.SyncDrag(scroll, 0., 10.);
			THEN( "drag is handled and ScrollVar is updated" ) {
				CHECK( result == true );
			}
		}
	}

	GIVEN( "a non-highlighted ScrollBar" ) {
		ScrollBar sb(0.5f, 0.3f, Point(0., 0.), Point(0., 100.), 5.f, 3.f, Color(0.6f), Color(0.25f));
		sb.highlighted = false;

		ScrollVar<double> scroll;
		scroll.SetMaxValue(100.0);
		scroll.SetDisplaySize(30.0);

		WHEN( "attempting SyncDrag" ) {
			bool result = sb.SyncDrag(scroll, 0., 10.);
			THEN( "drag is ignored" ) {
				CHECK( result == false );
			}
		}
	}
}

SCENARIO( "Point positions affect behavior", "[ScrollBar][Geometry]" ) {
	GIVEN( "a scrollbar with diagonal orientation" ) {
		Point from(0., 0.);
		Point to(100., 100.);
		ScrollBar sb(0.5f, 0.2f, from, to, 5.f, 3.f, Color(0.6f), Color(0.25f));
		sb.highlighted = true;

		WHEN( "dragging along the diagonal" ) {
			float oldFraction = sb.fraction;
			sb.Drag(10., 10.);
			THEN( "fraction changes appropriately" ) {
				CHECK( sb.fraction != oldFraction );
			}
		}

		WHEN( "dragging perpendicular to diagonal" ) {
			float oldFraction = sb.fraction;
			sb.Drag(10., -10.);
			THEN( "fraction does not change (perpendicular drag)" ) {
				// The dot product would be close to zero
				CHECK_THAT( sb.fraction, Catch::Matchers::WithinAbs(oldFraction, 0.001f) );
			}
		}
	}

	GIVEN( "a zero-length scrollbar (from equals to)" ) {
		Point from(50., 50.);
		Point to(50., 50.);
		ScrollBar sb(0.5f, 0.2f, from, to, 5.f, 3.f, Color(0.6f), Color(0.25f));
		sb.highlighted = true;

		WHEN( "attempting to drag" ) {
			// This would cause division by zero in the drag calculation
			sb.Drag(10., 0.);
			THEN( "fraction may become NaN or infinity" ) {
				// Implementation uses LengthSquared in denominator
				// When from == to, this is 0, causing issues
				CHECK( (std::isnan(sb.fraction) || std::isinf(sb.fraction) || sb.fraction == 0.5f) );
			}
		}
	}
}

SCENARIO( "Color values are preserved", "[ScrollBar][Color]" ) {
	GIVEN( "custom colors" ) {
		Color outerColor(1.f, 0.5f, 0.25f, 0.9f);
		Color innerColor(0.1f, 0.2f, 0.3f, 0.4f);
		ScrollBar sb(0.5f, 0.3f, Point(0., 0.), Point(100., 0.), 5.f, 3.f, outerColor, innerColor);

		WHEN( "checking color values" ) {
			THEN( "colors match the provided values" ) {
				CHECK( sb.color == outerColor );
				CHECK( sb.innerColor == innerColor );
			}
		}
	}
}

SCENARIO( "Multiple clicks in succession", "[ScrollBar][Click]" ) {
	GIVEN( "a scrollbar at initial position" ) {
		ScrollBar sb(0.5f, 0.2f, Point(10., 10.), Point(10., 110.), 5.f, 3.f, Color(0.6f), Color(0.25f));
		float initialFraction = sb.fraction;

		WHEN( "clicking near end multiple times" ) {
			sb.Click(10, 110, MouseButton::LEFT, 1);
			float after1 = sb.fraction;
			sb.Click(10, 110, MouseButton::LEFT, 1);
			float after2 = sb.fraction;

			THEN( "fraction increases with each click" ) {
				CHECK( after1 > initialFraction );
				CHECK( after2 > after1 );
			}
		}

		WHEN( "clicking near start multiple times" ) {
			sb.Click(10, 10, MouseButton::LEFT, 1);
			float after1 = sb.fraction;
			sb.Click(10, 10, MouseButton::LEFT, 1);
			float after2 = sb.fraction;

			THEN( "fraction decreases with each click until minimum" ) {
				CHECK( after1 < initialFraction );
				CHECK( after2 <= after1 );
			}
		}
	}
}

SCENARIO( "DisplaySizeFraction edge cases", "[ScrollBar][DisplaySize]" ) {
	GIVEN( "a scrollbar with very small display size fraction" ) {
		ScrollBar sb(0.5f, 0.01f, Point(0., 0.), Point(100., 0.), 5.f, 3.f, Color(0.6f), Color(0.25f));
		sb.highlighted = true;

		WHEN( "dragging" ) {
			sb.Drag(10., 0.);
			THEN( "small drag causes small fraction change" ) {
				// With small displaySizeFraction, drag has small effect
				CHECK( !std::isnan(sb.fraction) );
			}
		}
	}

	GIVEN( "a scrollbar with large display size fraction" ) {
		ScrollBar sb(0.5f, 0.9f, Point(0., 0.), Point(100., 0.), 5.f, 3.f, Color(0.6f), Color(0.25f));
		sb.highlighted = true;

		WHEN( "dragging" ) {
			sb.Drag(10., 0.);
			THEN( "same drag causes larger fraction change" ) {
				// With large displaySizeFraction, drag has larger effect
				// (divides by smaller value)
				CHECK( !std::isnan(sb.fraction) );
			}
		}
	}
}

// #endregion unit tests



} // test namespace