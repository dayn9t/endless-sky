/* test_government.cpp
Copyright (c) 2025 by the Endless Sky community

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
#include "../../../source/Government.h"

// ... and any system includes needed for the test file.
#include <cmath>
#include <limits>
#include <type_traits>

namespace { // test namespace

// #region mock data

// Helper function to compare doubles with tolerance
bool doubleEquals(double a, double b, double epsilon = 0.0001)
{
	return std::fabs(a - b) < epsilon;
}

// #endregion mock data



// #region unit tests

// Test class traits
TEST_CASE( "Government Class Traits", "[Government]" ) {
	using T = Government;
	SECTION( "Class Layout" ) {
		CHECK( std::is_default_constructible_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
		CHECK( std::is_copy_constructible_v<T> );
		CHECK( std::is_copy_assignable_v<T> );
		CHECK( std::is_move_constructible_v<T> );
		CHECK( std::is_move_assignable_v<T> );
	}
}

SCENARIO( "Creating a Government with default constructor", "[Government][Creation]" ) {
	GIVEN( "a default-constructed Government" ) {
		Government gov;

		THEN( "DisplayName returns empty string" ) {
			CHECK( gov.DisplayName().empty() );
		}
		THEN( "TrueName returns empty string" ) {
			CHECK( gov.TrueName().empty() );
		}
		THEN( "GetSwizzle returns a valid swizzle" ) {
			CHECK( gov.GetSwizzle() != nullptr );
		}
		THEN( "InitialPlayerReputation is 0" ) {
			CHECK( gov.InitialPlayerReputation() == 0. );
		}
		THEN( "GetBribeFraction is 0 (cannot be bribed)" ) {
			CHECK( gov.GetBribeFraction() == 0. );
		}
		THEN( "GetBribeThreshold is 0" ) {
			CHECK( gov.GetBribeThreshold() == 0. );
		}
		THEN( "GetFineFraction is 1" ) {
			CHECK( gov.GetFineFraction() == 1. );
		}
		THEN( "DeathSentence returns nullptr" ) {
			CHECK( gov.DeathSentence() == nullptr );
		}
		THEN( "Language returns empty string" ) {
			CHECK( gov.Language().empty() );
		}
		THEN( "SendUntranslatedHails returns false" ) {
			CHECK_FALSE( gov.SendUntranslatedHails() );
		}
		THEN( "RaidFleets returns empty vector" ) {
			CHECK( gov.RaidFleets().empty() );
		}
		THEN( "CrewAttack returns default value 1.0" ) {
			CHECK( gov.CrewAttack() == 1. );
		}
		THEN( "CrewDefense returns default value 2.0" ) {
			CHECK( gov.CrewDefense() == 2. );
		}
		THEN( "IsProvokedOnScan returns false" ) {
			CHECK_FALSE( gov.IsProvokedOnScan() );
		}
		THEN( "ReputationMax returns max double" ) {
			CHECK( gov.ReputationMax() == std::numeric_limits<double>::max() );
		}
		THEN( "ReputationMin returns lowest double" ) {
			CHECK( gov.ReputationMin() == std::numeric_limits<double>::lowest() );
		}
		THEN( "Tribute phrases are all nullptr" ) {
			CHECK( gov.TributeAlreadyPaying() == nullptr );
			CHECK( gov.TributeUndefined() == nullptr );
			CHECK( gov.TributeUnworthy() == nullptr );
			CHECK( gov.TributeFleetLaunching() == nullptr );
			CHECK( gov.TributeFleetUndefeated() == nullptr );
			CHECK( gov.TributeSurrendered() == nullptr );
		}
		THEN( "IgnoresUniversalAtrocities returns false" ) {
			CHECK_FALSE( gov.IgnoresUniversalAtrocities() );
		}
		THEN( "IgnoresUniversalIllegals returns false" ) {
			CHECK_FALSE( gov.IgnoresUniversalIllegals() );
		}
	}
}

SCENARIO( "Setting and getting TrueName", "[Government][TrueName]" ) {
	GIVEN( "a default-constructed Government" ) {
		Government gov;
		REQUIRE( gov.TrueName().empty() );

		WHEN( "SetTrueName is called with a name" ) {
			gov.SetTrueName("Republic");

			THEN( "TrueName returns the set name" ) {
				CHECK( gov.TrueName() == "Republic" );
			}
		}
	}

	GIVEN( "a Government with a TrueName set" ) {
		Government gov;
		gov.SetTrueName("Pirate");

		WHEN( "SetTrueName is called with a different name" ) {
			gov.SetTrueName("Merchant");

			THEN( "TrueName returns the new name" ) {
				CHECK( gov.TrueName() == "Merchant" );
			}
		}
	}
}

SCENARIO( "Testing Government::AttitudeToward", "[Government][AttitudeToward]" ) {
	GIVEN( "a default-constructed Government" ) {
		Government gov;

		WHEN( "checking attitude toward nullptr" ) {
			THEN( "returns 0" ) {
				CHECK( gov.AttitudeToward(nullptr) == 0. );
			}
		}

		WHEN( "checking attitude toward itself" ) {
			THEN( "returns 1" ) {
				CHECK( gov.AttitudeToward(&gov) == 1. );
			}
		}

		WHEN( "checking attitude toward another government" ) {
			Government other;

			THEN( "returns default attitude (0)" ) {
				CHECK( gov.AttitudeToward(&other) == 0. );
			}
		}
	}
}

SCENARIO( "Testing Government::PenaltyFor with null government", "[Government][PenaltyFor]" ) {
	GIVEN( "a default-constructed Government" ) {
		Government gov;

		WHEN( "checking penalty for null government" ) {
			auto penalty = gov.PenaltyFor(0, nullptr);

			THEN( "returns zero penalty" ) {
				CHECK( penalty.reputationChange == 0. );
				CHECK( penalty.specialPenalty == Government::SpecialPenalty::NONE );
			}
		}
	}
}

SCENARIO( "Testing Government::Trusts", "[Government][Trusts]" ) {
	GIVEN( "a default-constructed Government" ) {
		Government gov;

		WHEN( "checking if it trusts itself" ) {
			THEN( "returns true" ) {
				CHECK( gov.Trusts(&gov) );
			}
		}

		WHEN( "checking if it trusts another government" ) {
			Government other;

			THEN( "returns false (not in trusted set)" ) {
				CHECK_FALSE( gov.Trusts(&other) );
			}
		}

		WHEN( "checking if it trusts nullptr" ) {
			THEN( "returns false" ) {
				CHECK_FALSE( gov.Trusts(nullptr) );
			}
		}
	}
}

SCENARIO( "Testing Government::GetHail", "[Government][GetHail]" ) {
	GIVEN( "a default-constructed Government" ) {
		Government gov;

		WHEN( "getting hail when not disabled" ) {
			std::string hail = gov.GetHail(false);

			THEN( "returns empty string (no phrase set)" ) {
				CHECK( hail.empty() );
			}
		}

		WHEN( "getting hail when disabled" ) {
			std::string hail = gov.GetHail(true);

			THEN( "returns empty string (no phrase set)" ) {
				CHECK( hail.empty() );
			}
		}
	}
}

SCENARIO( "Testing Government bribe hail messages", "[Government][BribeHails]" ) {
	GIVEN( "a default-constructed Government" ) {
		Government gov;

		WHEN( "getting ship bribe acceptance hail" ) {
			std::string hail = gov.GetShipBribeAcceptanceHail();

			THEN( "returns default message" ) {
				CHECK( hail == "It's a pleasure doing business with you." );
			}
		}

		WHEN( "getting ship bribe rejection hail" ) {
			std::string hail = gov.GetShipBribeRejectionHail();

			THEN( "returns default message" ) {
				CHECK( hail == "I do not want your money." );
			}
		}

		WHEN( "getting planet bribe acceptance hail" ) {
			std::string hail = gov.GetPlanetBribeAcceptanceHail();

			THEN( "returns default message" ) {
				CHECK( hail == "It's a pleasure doing business with you." );
			}
		}

		WHEN( "getting planet bribe rejection hail" ) {
			std::string hail = gov.GetPlanetBribeRejectionHail();

			THEN( "returns default message" ) {
				CHECK( hail == "I do not want your money." );
			}
		}
	}
}

SCENARIO( "Testing Government::PenaltyEffect", "[Government][PenaltyEffect]" ) {
	GIVEN( "default PenaltyEffect" ) {
		Government::PenaltyEffect effect;

		THEN( "default values are zero and NONE" ) {
			CHECK( effect.reputationChange == 0. );
			CHECK( effect.specialPenalty == Government::SpecialPenalty::NONE );
		}
	}

	GIVEN( "PenaltyEffect with custom values" ) {
		Government::PenaltyEffect effect(5.0, Government::SpecialPenalty::ATROCITY);

		THEN( "values are set correctly" ) {
			CHECK( effect.reputationChange == 5.0 );
			CHECK( effect.specialPenalty == Government::SpecialPenalty::ATROCITY );
		}
	}
}

SCENARIO( "Testing Government::SpecialPenalty enum values", "[Government][SpecialPenalty]" ) {
	GIVEN( "the SpecialPenalty enum" ) {
		THEN( "NONE is 0" ) {
			CHECK( static_cast<int>(Government::SpecialPenalty::NONE) == 0 );
		}
		THEN( "PROVOKE is 1" ) {
			CHECK( static_cast<int>(Government::SpecialPenalty::PROVOKE) == 1 );
		}
		THEN( "ATROCITY is 2" ) {
			CHECK( static_cast<int>(Government::SpecialPenalty::ATROCITY) == 2 );
		}
		THEN( "values are ordered NONE < PROVOKE < ATROCITY" ) {
			CHECK( Government::SpecialPenalty::NONE < Government::SpecialPenalty::PROVOKE );
			CHECK( Government::SpecialPenalty::PROVOKE < Government::SpecialPenalty::ATROCITY );
		}
	}
}

SCENARIO( "Testing Government::Atrocity struct", "[Government][Atrocity]" ) {
	GIVEN( "default Atrocity" ) {
		Government::Atrocity atrocity;

		THEN( "isAtrocity is true by default" ) {
			CHECK( atrocity.isAtrocity );
		}
		THEN( "customDeathSentence is nullptr by default" ) {
			CHECK( atrocity.customDeathSentence == nullptr );
		}
	}
}

SCENARIO( "Testing Government default penalties", "[Government][DefaultPenalties]" ) {
	GIVEN( "a default-constructed Government" ) {
		Government gov;

		WHEN( "checking penalty for self for ASSIST event" ) {
			// ASSIST = 1 << 0 = 1
			auto penalty = gov.PenaltyFor(1, &gov);

			THEN( "reputation change is -0.1 (helping improves reputation)" ) {
				CHECK( doubleEquals(penalty.reputationChange, -0.1) );
				CHECK( penalty.specialPenalty == Government::SpecialPenalty::NONE );
			}
		}

		WHEN( "checking penalty for self for DISABLE event" ) {
			// DISABLE = 1 << 4 = 16
			auto penalty = gov.PenaltyFor(16, &gov);

			THEN( "reputation change is 0.5" ) {
				CHECK( doubleEquals(penalty.reputationChange, 0.5) );
				CHECK( penalty.specialPenalty == Government::SpecialPenalty::NONE );
			}
		}

		WHEN( "checking penalty for self for BOARD event" ) {
			// BOARD = 1 << 5 = 32
			auto penalty = gov.PenaltyFor(32, &gov);

			THEN( "reputation change is 0.3" ) {
				CHECK( doubleEquals(penalty.reputationChange, 0.3) );
				CHECK( penalty.specialPenalty == Government::SpecialPenalty::NONE );
			}
		}

		WHEN( "checking penalty for self for CAPTURE event" ) {
			// CAPTURE = 1 << 6 = 64
			auto penalty = gov.PenaltyFor(64, &gov);

			THEN( "reputation change is 1.0" ) {
				CHECK( doubleEquals(penalty.reputationChange, 1.0) );
				CHECK( penalty.specialPenalty == Government::SpecialPenalty::NONE );
			}
		}

		WHEN( "checking penalty for self for DESTROY event" ) {
			// DESTROY = 1 << 7 = 128
			auto penalty = gov.PenaltyFor(128, &gov);

			THEN( "reputation change is 1.0" ) {
				CHECK( doubleEquals(penalty.reputationChange, 1.0) );
				CHECK( penalty.specialPenalty == Government::SpecialPenalty::NONE );
			}
		}
	}
}

SCENARIO( "Testing Government combined event penalties", "[Government][CombinedEvents]" ) {
	GIVEN( "a default-constructed Government" ) {
		Government gov;

		WHEN( "checking combined penalty for DISABLE and BOARD" ) {
			// DISABLE | BOARD = 16 | 32 = 48
			auto penalty = gov.PenaltyFor(48, &gov);

			THEN( "reputation changes are combined (0.5 + 0.3 = 0.8)" ) {
				CHECK( doubleEquals(penalty.reputationChange, 0.8) );
			}
		}

		WHEN( "checking combined penalty for CAPTURE and DESTROY" ) {
			// CAPTURE | DESTROY = 64 | 128 = 192
			auto penalty = gov.PenaltyFor(192, &gov);

			THEN( "reputation changes are combined (1.0 + 1.0 = 2.0)" ) {
				CHECK( doubleEquals(penalty.reputationChange, 2.0) );
			}
		}
	}
}

SCENARIO( "Testing Government multiple instances have unique IDs", "[Government][UniqueID]" ) {
	GIVEN( "multiple Government instances" ) {
		Government gov1;
		Government gov2;
		Government gov3;

		WHEN( "checking attitude toward each other" ) {
			// Each government should have a unique ID, so they can track attitudes
			THEN( "attitude toward other governments returns default (0)" ) {
				CHECK( gov1.AttitudeToward(&gov2) == 0. );
				CHECK( gov2.AttitudeToward(&gov1) == 0. );
				CHECK( gov1.AttitudeToward(&gov3) == 0. );
			}
			THEN( "each government trusts itself" ) {
				CHECK( gov1.Trusts(&gov1) );
				CHECK( gov2.Trusts(&gov2) );
				CHECK( gov3.Trusts(&gov3) );
			}
			THEN( "governments don't trust each other by default" ) {
				CHECK_FALSE( gov1.Trusts(&gov2) );
				CHECK_FALSE( gov2.Trusts(&gov1) );
				CHECK_FALSE( gov1.Trusts(&gov3) );
			}
		}
	}
}

SCENARIO( "Testing Government copy semantics", "[Government][Copy]" ) {
	GIVEN( "a Government" ) {
		Government original;
		original.SetTrueName("Original");

		WHEN( "copy-constructing a new Government" ) {
			Government copy(original);

			THEN( "the copy has the same TrueName" ) {
				CHECK( copy.TrueName() == "Original" );
			}
		}
	}

	GIVEN( "two Governments" ) {
		Government gov1;
		gov1.SetTrueName("First");
		Government gov2;
		gov2.SetTrueName("Second");

		WHEN( "copy-assigning one to another" ) {
			gov2 = gov1;

			THEN( "the assigned-to government has the source's TrueName" ) {
				CHECK( gov2.TrueName() == "First" );
			}
		}
	}
}

SCENARIO( "Testing Government move semantics", "[Government][Move]" ) {
	GIVEN( "a Government" ) {
		Government original;
		original.SetTrueName("Original");

		WHEN( "move-constructing a new Government" ) {
			Government moved(std::move(original));

			THEN( "the moved-to object has the correct TrueName" ) {
				CHECK( moved.TrueName() == "Original" );
			}
		}
	}

	GIVEN( "two Governments" ) {
		Government gov1;
		gov1.SetTrueName("First");
		Government gov2;
		gov2.SetTrueName("Second");

		WHEN( "move-assigning one to another" ) {
			gov2 = std::move(gov1);

			THEN( "the assigned-to government has the source's TrueName" ) {
				CHECK( gov2.TrueName() == "First" );
			}
		}
	}
}

SCENARIO( "Testing Government GetColor", "[Government][GetColor]" ) {
	GIVEN( "a default-constructed Government" ) {
		Government gov;

		WHEN( "getting the color" ) {
			const Color& color = gov.GetColor();

			THEN( "a valid color reference is returned" ) {
				// Default color should be valid (may be default initialized)
				const float* values = color.Get();
				CHECK( values != nullptr );
			}
		}
	}
}

SCENARIO( "Testing Government GetSwizzle", "[Government][GetSwizzle]" ) {
	GIVEN( "a default-constructed Government" ) {
		Government gov;

		WHEN( "getting the swizzle" ) {
			const Swizzle* swizzle = gov.GetSwizzle();

			THEN( "returns a valid swizzle pointer" ) {
				CHECK( swizzle != nullptr );
			}
			THEN( "the default swizzle is the None swizzle" ) {
				CHECK( swizzle == Swizzle::None() );
			}
		}
	}
}

SCENARIO( "Testing Government CanEnforce with null system/planet", "[Government][CanEnforce]" ) {
	GIVEN( "a default-constructed Government with no enforcement zones" ) {
		Government gov;

		WHEN( "checking if it can enforce in null system" ) {
			THEN( "returns true (no restrictions)" ) {
				CHECK( gov.CanEnforce(static_cast<const System*>(nullptr)) );
			}
		}

		WHEN( "checking if it can enforce in null planet" ) {
			THEN( "returns true (no restrictions)" ) {
				CHECK( gov.CanEnforce(static_cast<const Planet*>(nullptr)) );
			}
		}
	}
}

// Note: Tests involving Load, GameData, Politics, and related operations
// require GameData and other runtime dependencies, which are not available
// in this unit test environment. Those scenarios would be better suited
// for integration tests or would require mocking infrastructure.
//
// Methods that require GameData/Politics:
// - Load() - requires DataNode, GameData
// - IsEnemy() - requires GameData::GetPolitics()
// - IsPlayer() - requires GameData::PlayerGovernment()
// - Offend() - requires GameData::GetPolitics()
// - Bribe() - requires GameData::GetPolitics()
// - Fine() - requires PlayerInfo, GameData::GetPolitics()
// - Condemns() - requires Outfit/Ship with GameData
// - Fines() - requires Outfit/Ship with GameData
// - FinesContents() - requires Ship with GameData
// - Reputation() - requires GameData::GetPolitics()
// - AddReputation() - requires GameData::GetPolitics()
// - SetReputation() - requires GameData::GetPolitics()
// - IsRestrictedFrom() - requires System/Planet with GameData

// #endregion unit tests



} // test namespace