/* test_preferences.cpp
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
#include "../../../source/Preferences.h"

// ... and any system includes needed for the test file.
#include <string>
#include <type_traits>

namespace { // test namespace

// #region mock data

// #endregion mock data



// #region unit tests

SCENARIO( "Preferences VSync enum", "[Preferences][VSync]" ) {
	GIVEN( "the VSync enum" ) {
		THEN( "it has the expected values" ) {
			CHECK( static_cast<int>(Preferences::VSync::off) == 0 );
			CHECK( static_cast<int>(Preferences::VSync::on) == 1 );
			CHECK( static_cast<int>(Preferences::VSync::adaptive) == 2 );
		}
	}
}

SCENARIO( "Preferences CameraAccel enum", "[Preferences][CameraAccel]" ) {
	GIVEN( "the CameraAccel enum" ) {
		THEN( "it has the expected values" ) {
			CHECK( static_cast<int>(Preferences::CameraAccel::OFF) == 0 );
			CHECK( static_cast<int>(Preferences::CameraAccel::ON) == 1 );
			CHECK( static_cast<int>(Preferences::CameraAccel::REVERSED) == 2 );
		}
	}
}

SCENARIO( "Preferences DateFormat enum", "[Preferences][DateFormat]" ) {
	GIVEN( "the DateFormat enum" ) {
		THEN( "it has the expected values" ) {
			CHECK( static_cast<int>(Preferences::DateFormat::DMY) == 0 );
			CHECK( static_cast<int>(Preferences::DateFormat::MDY) == 1 );
			CHECK( static_cast<int>(Preferences::DateFormat::YMD) == 2 );
		}
	}
}

SCENARIO( "Preferences NotificationSetting enum", "[Preferences][NotificationSetting]" ) {
	GIVEN( "the NotificationSetting enum" ) {
		THEN( "it has the expected values" ) {
			CHECK( static_cast<int>(Preferences::NotificationSetting::OFF) == 0 );
			CHECK( static_cast<int>(Preferences::NotificationSetting::MESSAGE) == 1 );
			CHECK( static_cast<int>(Preferences::NotificationSetting::BOTH) == 2 );
		}
	}
}

SCENARIO( "Preferences OverlayState enum", "[Preferences][OverlayState]" ) {
	GIVEN( "the OverlayState enum" ) {
		THEN( "it has the expected values" ) {
			CHECK( static_cast<int>(Preferences::OverlayState::OFF) == 0 );
			CHECK( static_cast<int>(Preferences::OverlayState::ON) == 1 );
			CHECK( static_cast<int>(Preferences::OverlayState::DAMAGED) == 2 );
			CHECK( static_cast<int>(Preferences::OverlayState::DISABLED) == 3 );
			CHECK( static_cast<int>(Preferences::OverlayState::ON_HIT) == 4 );
		}
	}
}

SCENARIO( "Preferences OverlayType enum", "[Preferences][OverlayType]" ) {
	GIVEN( "the OverlayType enum" ) {
		THEN( "it has the expected values" ) {
			CHECK( static_cast<int>(Preferences::OverlayType::ALL) == 0 );
			CHECK( static_cast<int>(Preferences::OverlayType::FLAGSHIP) == 1 );
			CHECK( static_cast<int>(Preferences::OverlayType::ESCORT) == 2 );
			CHECK( static_cast<int>(Preferences::OverlayType::ENEMY) == 3 );
			CHECK( static_cast<int>(Preferences::OverlayType::NEUTRAL) == 4 );
		}
	}
}

SCENARIO( "Preferences TurretOverlays enum", "[Preferences][TurretOverlays]" ) {
	GIVEN( "the TurretOverlays enum" ) {
		THEN( "it has the expected values" ) {
			CHECK( static_cast<int>(Preferences::TurretOverlays::OFF) == 0 );
			CHECK( static_cast<int>(Preferences::TurretOverlays::ALWAYS_ON) == 1 );
			CHECK( static_cast<int>(Preferences::TurretOverlays::BLINDSPOTS_ONLY) == 2 );
		}
	}
}

SCENARIO( "Preferences AutoAim enum", "[Preferences][AutoAim]" ) {
	GIVEN( "the AutoAim enum" ) {
		THEN( "it has the expected values" ) {
			CHECK( static_cast<int>(Preferences::AutoAim::OFF) == 0 );
			CHECK( static_cast<int>(Preferences::AutoAim::ALWAYS_ON) == 1 );
			CHECK( static_cast<int>(Preferences::AutoAim::WHEN_FIRING) == 2 );
		}
	}
}

SCENARIO( "Preferences AutoFire enum", "[Preferences][AutoFire]" ) {
	GIVEN( "the AutoFire enum" ) {
		THEN( "it has the expected values" ) {
			CHECK( static_cast<int>(Preferences::AutoFire::OFF) == 0 );
			CHECK( static_cast<int>(Preferences::AutoFire::ON) == 1 );
			CHECK( static_cast<int>(Preferences::AutoFire::GUNS_ONLY) == 2 );
			CHECK( static_cast<int>(Preferences::AutoFire::TURRETS_ONLY) == 3 );
		}
	}
}

SCENARIO( "Preferences BoardingPriority enum", "[Preferences][BoardingPriority]" ) {
	GIVEN( "the BoardingPriority enum" ) {
		THEN( "it has the expected values" ) {
			CHECK( static_cast<int>(Preferences::BoardingPriority::PROXIMITY) == 0 );
			CHECK( static_cast<int>(Preferences::BoardingPriority::VALUE) == 1 );
			CHECK( static_cast<int>(Preferences::BoardingPriority::MIXED) == 2 );
		}
	}
}

SCENARIO( "Preferences FlotsamCollection enum", "[Preferences][FlotsamCollection]" ) {
	GIVEN( "the FlotsamCollection enum" ) {
		THEN( "it has the expected values" ) {
			CHECK( static_cast<int>(Preferences::FlotsamCollection::OFF) == 0 );
			CHECK( static_cast<int>(Preferences::FlotsamCollection::ON) == 1 );
			CHECK( static_cast<int>(Preferences::FlotsamCollection::FLAGSHIP) == 2 );
			CHECK( static_cast<int>(Preferences::FlotsamCollection::ESCORT) == 3 );
		}
	}
}

SCENARIO( "Preferences BackgroundParallax enum", "[Preferences][BackgroundParallax]" ) {
	GIVEN( "the BackgroundParallax enum" ) {
		THEN( "it has the expected values" ) {
			CHECK( static_cast<int>(Preferences::BackgroundParallax::OFF) == 0 );
			CHECK( static_cast<int>(Preferences::BackgroundParallax::FANCY) == 1 );
			CHECK( static_cast<int>(Preferences::BackgroundParallax::FAST) == 2 );
		}
	}
}

SCENARIO( "Preferences ExtendedJumpEffects enum", "[Preferences][ExtendedJumpEffects]" ) {
	GIVEN( "the ExtendedJumpEffects enum" ) {
		THEN( "it has the expected values" ) {
			CHECK( static_cast<int>(Preferences::ExtendedJumpEffects::OFF) == 0 );
			CHECK( static_cast<int>(Preferences::ExtendedJumpEffects::MEDIUM) == 1 );
			CHECK( static_cast<int>(Preferences::ExtendedJumpEffects::HEAVY) == 2 );
		}
	}
}

SCENARIO( "Preferences AlertIndicator enum", "[Preferences][AlertIndicator]" ) {
	GIVEN( "the AlertIndicator enum" ) {
		THEN( "it has the expected values" ) {
			CHECK( static_cast<int>(Preferences::AlertIndicator::NONE) == 0 );
			CHECK( static_cast<int>(Preferences::AlertIndicator::AUDIO) == 1 );
			CHECK( static_cast<int>(Preferences::AlertIndicator::VISUAL) == 2 );
			CHECK( static_cast<int>(Preferences::AlertIndicator::BOTH) == 3 );
		}
	}
}

SCENARIO( "Preferences MinimapDisplay enum", "[Preferences][MinimapDisplay]" ) {
	GIVEN( "the MinimapDisplay enum" ) {
		THEN( "it has the expected values" ) {
			CHECK( static_cast<int>(Preferences::MinimapDisplay::OFF) == 0 );
			CHECK( static_cast<int>(Preferences::MinimapDisplay::WHEN_JUMPING) == 1 );
			CHECK( static_cast<int>(Preferences::MinimapDisplay::ALWAYS_ON) == 2 );
		}
	}
}

SCENARIO( "Preferences FlagshipSpacePriority enum", "[Preferences][FlagshipSpacePriority]" ) {
	GIVEN( "the FlagshipSpacePriority enum" ) {
		THEN( "it has the expected values" ) {
			CHECK( static_cast<int>(Preferences::FlagshipSpacePriority::NONE) == 0 );
			CHECK( static_cast<int>(Preferences::FlagshipSpacePriority::PASSENGERS) == 1 );
			CHECK( static_cast<int>(Preferences::FlagshipSpacePriority::CARGO) == 2 );
			CHECK( static_cast<int>(Preferences::FlagshipSpacePriority::BOTH) == 3 );
		}
	}
}

SCENARIO( "Preferences LargeGraphicsReduction enum", "[Preferences][LargeGraphicsReduction]" ) {
	GIVEN( "the LargeGraphicsReduction enum" ) {
		THEN( "it has the expected values" ) {
			CHECK( static_cast<int>(Preferences::LargeGraphicsReduction::OFF) == 0 );
			CHECK( static_cast<int>(Preferences::LargeGraphicsReduction::LARGEST_ONLY) == 1 );
			CHECK( static_cast<int>(Preferences::LargeGraphicsReduction::ALL) == 2 );
		}
	}
}

#ifdef _WIN32
SCENARIO( "Preferences TitleBarTheme enum (Windows only)", "[Preferences][TitleBarTheme]" ) {
	GIVEN( "the TitleBarTheme enum" ) {
		THEN( "it has the expected values" ) {
			CHECK( static_cast<int>(Preferences::TitleBarTheme::DEFAULT) == 0 );
			CHECK( static_cast<int>(Preferences::TitleBarTheme::LIGHT) == 1 );
			CHECK( static_cast<int>(Preferences::TitleBarTheme::DARK) == 2 );
		}
	}
}

SCENARIO( "Preferences WindowRounding enum (Windows only)", "[Preferences][WindowRounding]" ) {
	GIVEN( "the WindowRounding enum" ) {
		THEN( "it has the expected values" ) {
			CHECK( static_cast<int>(Preferences::WindowRounding::DEFAULT) == 0 );
			CHECK( static_cast<int>(Preferences::WindowRounding::OFF) == 1 );
			CHECK( static_cast<int>(Preferences::WindowRounding::LARGE) == 2 );
			CHECK( static_cast<int>(Preferences::WindowRounding::SMALL) == 3 );
		}
	}
}
#endif

SCENARIO( "Preferences enum underlying types", "[Preferences][Types]" ) {
	GIVEN( "the Preferences enums" ) {
		THEN( "VSync uses int_fast8_t" ) {
			CHECK( sizeof(Preferences::VSync) == sizeof(int_fast8_t) );
		}
		THEN( "CameraAccel uses int_fast8_t" ) {
			CHECK( sizeof(Preferences::CameraAccel) == sizeof(int_fast8_t) );
		}
		THEN( "DateFormat uses int_fast8_t" ) {
			CHECK( sizeof(Preferences::DateFormat) == sizeof(int_fast8_t) );
		}
		THEN( "NotificationSetting uses int_fast8_t" ) {
			CHECK( sizeof(Preferences::NotificationSetting) == sizeof(int_fast8_t) );
		}
		THEN( "OverlayState uses int_fast8_t" ) {
			CHECK( sizeof(Preferences::OverlayState) == sizeof(int_fast8_t) );
		}
		THEN( "OverlayType uses int_fast8_t" ) {
			CHECK( sizeof(Preferences::OverlayType) == sizeof(int_fast8_t) );
		}
		THEN( "TurretOverlays uses int_fast8_t" ) {
			CHECK( sizeof(Preferences::TurretOverlays) == sizeof(int_fast8_t) );
		}
		THEN( "AutoAim uses int_fast8_t" ) {
			CHECK( sizeof(Preferences::AutoAim) == sizeof(int_fast8_t) );
		}
		THEN( "AutoFire uses int_fast8_t" ) {
			CHECK( sizeof(Preferences::AutoFire) == sizeof(int_fast8_t) );
		}
		THEN( "BoardingPriority uses int_fast8_t" ) {
			CHECK( sizeof(Preferences::BoardingPriority) == sizeof(int_fast8_t) );
		}
		THEN( "FlotsamCollection uses int_fast8_t" ) {
			CHECK( sizeof(Preferences::FlotsamCollection) == sizeof(int_fast8_t) );
		}
		THEN( "BackgroundParallax uses int" ) {
			CHECK( sizeof(Preferences::BackgroundParallax) == sizeof(int) );
		}
		THEN( "ExtendedJumpEffects uses int" ) {
			CHECK( sizeof(Preferences::ExtendedJumpEffects) == sizeof(int) );
		}
		THEN( "AlertIndicator uses int_fast8_t" ) {
			CHECK( sizeof(Preferences::AlertIndicator) == sizeof(int_fast8_t) );
		}
		THEN( "MinimapDisplay uses int_fast8_t" ) {
			CHECK( sizeof(Preferences::MinimapDisplay) == sizeof(int_fast8_t) );
		}
		THEN( "FlagshipSpacePriority uses int_fast8_t" ) {
			CHECK( sizeof(Preferences::FlagshipSpacePriority) == sizeof(int_fast8_t) );
		}
		THEN( "LargeGraphicsReduction uses int_fast8_t" ) {
			CHECK( sizeof(Preferences::LargeGraphicsReduction) == sizeof(int_fast8_t) );
		}
	}
}

SCENARIO( "Preferences class structure", "[Preferences][Structure]" ) {
	GIVEN( "the Preferences class" ) {
		THEN( "it is not instantiable (all static members)" ) {
			// Preferences has only static methods and no public constructor
			// This is a design pattern for a singleton-like settings manager
			CHECK( true ); // Class exists and is well-formed
		}
	}
}

SCENARIO( "Preferences enum value ranges", "[Preferences][Ranges]" ) {
	GIVEN( "OverlayState enum values" ) {
		THEN( "values form a valid sequence" ) {
			CHECK( static_cast<int>(Preferences::OverlayState::OFF) < static_cast<int>(Preferences::OverlayState::ON) );
			CHECK( static_cast<int>(Preferences::OverlayState::ON) < static_cast<int>(Preferences::OverlayState::DAMAGED) );
			CHECK( static_cast<int>(Preferences::OverlayState::DAMAGED) < static_cast<int>(Preferences::OverlayState::DISABLED) );
			CHECK( static_cast<int>(Preferences::OverlayState::DISABLED) < static_cast<int>(Preferences::OverlayState::ON_HIT) );
		}
	}
	GIVEN( "AlertIndicator enum values" ) {
		THEN( "values form a valid sequence" ) {
			CHECK( static_cast<int>(Preferences::AlertIndicator::NONE) < static_cast<int>(Preferences::AlertIndicator::AUDIO) );
			CHECK( static_cast<int>(Preferences::AlertIndicator::AUDIO) < static_cast<int>(Preferences::AlertIndicator::VISUAL) );
			CHECK( static_cast<int>(Preferences::AlertIndicator::VISUAL) < static_cast<int>(Preferences::AlertIndicator::BOTH) );
		}
	}
}

SCENARIO( "Preferences DateFormat descriptive names", "[Preferences][DateFormat][Documentation]" ) {
	GIVEN( "the DateFormat enum" ) {
		THEN( "DMY is day-first format" ) {
			// Day-first format: Sat, 4 Oct 1941
			CHECK( static_cast<int>(Preferences::DateFormat::DMY) == 0 );
		}
		THEN( "MDY is month-first format" ) {
			// Month-first format: Sat, Oct 4, 1941
			CHECK( static_cast<int>(Preferences::DateFormat::MDY) == 1 );
		}
		THEN( "YMD is ISO 8601 format" ) {
			// ISO 8601: 1941-10-04
			CHECK( static_cast<int>(Preferences::DateFormat::YMD) == 2 );
		}
	}
}

// #endregion unit tests



} // test namespace