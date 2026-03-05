/* test_personality.cpp
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
#include "../../../source/Personality.h"

// Include helper for creating DataNodes.
#include "datanode-factory.h"

// ... and any system includes needed for the test file.
#include <cmath>
#include <string>

namespace { // test namespace

// #region mock data

// #endregion mock data



// #region unit tests
TEST_CASE( "Personality Basics", "[Personality]" ) {
	using T = Personality;
	SECTION( "Class Traits" ) {
		CHECK_FALSE( std::is_trivial_v<T> );
		CHECK( std::is_standard_layout_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
		CHECK( std::is_trivially_destructible_v<T> );
	}
	SECTION( "Construction Traits" ) {
		CHECK( std::is_default_constructible_v<T> );
		CHECK_FALSE( std::is_trivially_default_constructible_v<T> );
		CHECK( std::is_nothrow_default_constructible_v<T> );
		CHECK( std::is_copy_constructible_v<T> );
		CHECK( std::is_trivially_copy_constructible_v<T> );
		CHECK( std::is_nothrow_copy_constructible_v<T> );
		CHECK( std::is_move_constructible_v<T> );
		CHECK( std::is_trivially_move_constructible_v<T> );
		CHECK( std::is_nothrow_move_constructible_v<T> );
	}
	SECTION( "Copy Traits" ) {
		CHECK( std::is_copy_assignable_v<T> );
		CHECK( std::is_trivially_copyable_v<T> );
		CHECK( std::is_trivially_copy_assignable_v<T> );
		CHECK( std::is_nothrow_copy_assignable_v<T> );
	}
	SECTION( "Move Traits" ) {
		CHECK( std::is_move_assignable_v<T> );
		CHECK( std::is_trivially_move_assignable_v<T> );
		CHECK( std::is_nothrow_move_assignable_v<T> );
	}
}

SCENARIO( "Creating a Personality with default constructor", "[Personality]" ) {
	GIVEN( "A default-constructed Personality" ) {
		Personality personality;
		THEN( "IsDefined returns false" ) {
			CHECK_FALSE( personality.IsDefined() );
		}
		THEN( "Disables flag is set by default (for player ships)" ) {
			CHECK( personality.Disables() );
		}
		THEN( "Other personality flags are not set" ) {
			CHECK_FALSE( personality.IsPacifist() );
			CHECK_FALSE( personality.IsForbearing() );
			CHECK_FALSE( personality.IsTimid() );
			CHECK_FALSE( personality.IsHunting() );
			CHECK_FALSE( personality.IsNemesis() );
			CHECK_FALSE( personality.IsDaring() );
			CHECK_FALSE( personality.IsFrugal() );
			CHECK_FALSE( personality.Plunders() );
			CHECK_FALSE( personality.IsVindictive() );
			CHECK_FALSE( personality.IsUnconstrained() );
			CHECK_FALSE( personality.IsUnrestricted() );
			CHECK_FALSE( personality.IsRestricted() );
			CHECK_FALSE( personality.IsCoward() );
			CHECK_FALSE( personality.IsAppeasing() );
			CHECK_FALSE( personality.IsOpportunistic() );
			CHECK_FALSE( personality.IsMerciful() );
			CHECK_FALSE( personality.IsRamming() );
			CHECK_FALSE( personality.IsGetaway() );
		}
		THEN( "Mission NPC flags are not set" ) {
			CHECK_FALSE( personality.IsStaying() );
			CHECK_FALSE( personality.IsEntering() );
			CHECK_FALSE( personality.IsWaiting() );
			CHECK_FALSE( personality.IsLaunching() );
			CHECK_FALSE( personality.IsFleeing() );
			CHECK_FALSE( personality.IsDerelict() );
			CHECK_FALSE( personality.IsUninterested() );
		}
		THEN( "Non-combat goal flags are not set" ) {
			CHECK_FALSE( personality.IsSurveillance() );
			CHECK_FALSE( personality.IsMining() );
			CHECK_FALSE( personality.Harvests() );
			CHECK_FALSE( personality.IsSwarming() );
			CHECK_FALSE( personality.IsLingering() );
			CHECK_FALSE( personality.IsSecretive() );
		}
		THEN( "Special flags are not set" ) {
			CHECK_FALSE( personality.IsEscort() );
			CHECK_FALSE( personality.IsTarget() );
			CHECK_FALSE( personality.IsMarked() );
			CHECK_FALSE( personality.IsTracked() );
			CHECK_FALSE( personality.IsMute() );
			CHECK_FALSE( personality.IsDecloaked() );
			CHECK_FALSE( personality.IsQuiet() );
		}
		THEN( "Confusion starts at origin" ) {
			CHECK( personality.Confusion().X() == 0. );
			CHECK( personality.Confusion().Y() == 0. );
		}
	}
}

SCENARIO( "Loading Personality from DataNode", "[Personality][Load]" ) {
	GIVEN( "A Personality and a simple DataNode" ) {
		Personality personality;
		WHEN( "loading a single personality trait" ) {
			DataNode node = AsDataNode("personality\n\tpacifist");
			personality.Load(node);
			THEN( "IsDefined returns true" ) {
				CHECK( personality.IsDefined() );
			}
			THEN( "the specified flag is set" ) {
				CHECK( personality.IsPacifist() );
			}
			THEN( "other flags remain unset" ) {
				CHECK_FALSE( personality.IsHunting() );
				CHECK_FALSE( personality.IsDaring() );
			}
		}
	}

	GIVEN( "A Personality and a DataNode with multiple traits" ) {
		Personality personality;
		WHEN( "loading multiple personality traits" ) {
			DataNode node = AsDataNode("personality\n\tpacifist\n\ttimid\n\tforbearing");
			personality.Load(node);
			THEN( "all specified flags are set" ) {
				CHECK( personality.IsPacifist() );
				CHECK( personality.IsTimid() );
				CHECK( personality.IsForbearing() );
			}
			THEN( "unspecified flags remain unset" ) {
				CHECK_FALSE( personality.IsHunting() );
				CHECK_FALSE( personality.IsDaring() );
			}
		}
	}

	GIVEN( "A Personality and a DataNode with inline traits" ) {
		Personality personality;
		WHEN( "loading personality traits on the same line" ) {
			DataNode node = AsDataNode("personality pacifist timid forbearing");
			personality.Load(node);
			THEN( "all specified flags are set" ) {
				CHECK( personality.IsPacifist() );
				CHECK( personality.IsTimid() );
				CHECK( personality.IsForbearing() );
			}
		}
	}

	GIVEN( "A Personality and a DataNode with the heroic composite token" ) {
		Personality personality;
		WHEN( "loading the heroic composite trait" ) {
			DataNode node = AsDataNode("personality\n\theroic");
			personality.Load(node);
			THEN( "heroic sets both daring and hunting flags" ) {
				CHECK( personality.IsDaring() );
				CHECK( personality.IsHunting() );
			}
		}
	}

	GIVEN( "A Personality with existing traits" ) {
		Personality personality;
		DataNode initialNode = AsDataNode("personality\n\tpacifist\n\ttimid");
		personality.Load(initialNode);
		REQUIRE( personality.IsPacifist() );
		REQUIRE( personality.IsTimid() );

		WHEN( "loading with 'add' prefix" ) {
			DataNode addNode = AsDataNode("add personality\n\thunting\n\tdaring");
			personality.Load(addNode);
			THEN( "new traits are added while existing traits remain" ) {
				CHECK( personality.IsPacifist() );
				CHECK( personality.IsTimid() );
				CHECK( personality.IsHunting() );
				CHECK( personality.IsDaring() );
			}
		}

		WHEN( "loading with 'remove' prefix" ) {
			DataNode removeNode = AsDataNode("remove personality\n\tpacifist");
			personality.Load(removeNode);
			THEN( "specified traits are removed while others remain" ) {
				CHECK_FALSE( personality.IsPacifist() );
				CHECK( personality.IsTimid() );
			}
		}
	}

	GIVEN( "A Personality and a DataNode with confusion value" ) {
		Personality personality;
		WHEN( "loading with a custom confusion value" ) {
			DataNode node = AsDataNode("personality\n\tconfusion 5.0");
			personality.Load(node);
			THEN( "the personality is defined" ) {
				CHECK( personality.IsDefined() );
			}
		}
	}
}

SCENARIO( "Personality attack behavior flags", "[Personality]" ) {
	GIVEN( "A Personality" ) {
		Personality personality;
		WHEN( "pacifist is set" ) {
			DataNode node = AsDataNode("personality\n\tpacifist");
			personality.Load(node);
			THEN( "IsPacifist returns true" ) {
				CHECK( personality.IsPacifist() );
			}
		}

		WHEN( "forbearing is set" ) {
			DataNode node = AsDataNode("personality\n\tforbearing");
			personality.Load(node);
			THEN( "IsForbearing returns true" ) {
				CHECK( personality.IsForbearing() );
			}
		}

		WHEN( "timid is set" ) {
			DataNode node = AsDataNode("personality\n\ttimid");
			personality.Load(node);
			THEN( "IsTimid returns true" ) {
				CHECK( personality.IsTimid() );
			}
		}

		WHEN( "hunting is set" ) {
			DataNode node = AsDataNode("personality\n\thunting");
			personality.Load(node);
			THEN( "IsHunting returns true" ) {
				CHECK( personality.IsHunting() );
			}
		}

		WHEN( "nemesis is set" ) {
			DataNode node = AsDataNode("personality\n\tnemesis");
			personality.Load(node);
			THEN( "IsNemesis returns true" ) {
				CHECK( personality.IsNemesis() );
			}
		}

		WHEN( "daring is set" ) {
			DataNode node = AsDataNode("personality\n\tdaring");
			personality.Load(node);
			THEN( "IsDaring returns true" ) {
				CHECK( personality.IsDaring() );
			}
		}
	}
}

SCENARIO( "Personality combat style flags", "[Personality]" ) {
	GIVEN( "A Personality" ) {
		Personality personality;

		WHEN( "frugal is set" ) {
			DataNode node = AsDataNode("personality\n\tfrugal");
			personality.Load(node);
			THEN( "IsFrugal returns true" ) {
				CHECK( personality.IsFrugal() );
			}
		}

		WHEN( "disables is set" ) {
			DataNode node = AsDataNode("personality\n\tdisables");
			personality.Load(node);
			THEN( "Disables returns true" ) {
				CHECK( personality.Disables() );
			}
		}

		WHEN( "plunders is set" ) {
			DataNode node = AsDataNode("personality\n\tplunders");
			personality.Load(node);
			THEN( "Plunders returns true" ) {
				CHECK( personality.Plunders() );
			}
		}

		WHEN( "vindictive is set" ) {
			DataNode node = AsDataNode("personality\n\tvindictive");
			personality.Load(node);
			THEN( "IsVindictive returns true" ) {
				CHECK( personality.IsVindictive() );
			}
		}

		WHEN( "unconstrained is set" ) {
			DataNode node = AsDataNode("personality\n\tunconstrained");
			personality.Load(node);
			THEN( "IsUnconstrained returns true" ) {
				CHECK( personality.IsUnconstrained() );
			}
		}

		WHEN( "unrestricted is set" ) {
			DataNode node = AsDataNode("personality\n\tunrestricted");
			personality.Load(node);
			THEN( "IsUnrestricted returns true" ) {
				CHECK( personality.IsUnrestricted() );
			}
		}

		WHEN( "restricted is set" ) {
			DataNode node = AsDataNode("personality\n\trestricted");
			personality.Load(node);
			THEN( "IsRestricted returns true" ) {
				CHECK( personality.IsRestricted() );
			}
		}

		WHEN( "coward is set" ) {
			DataNode node = AsDataNode("personality\n\tcoward");
			personality.Load(node);
			THEN( "IsCoward returns true" ) {
				CHECK( personality.IsCoward() );
			}
		}

		WHEN( "appeasing is set" ) {
			DataNode node = AsDataNode("personality\n\tappeasing");
			personality.Load(node);
			THEN( "IsAppeasing returns true" ) {
				CHECK( personality.IsAppeasing() );
			}
		}

		WHEN( "opportunistic is set" ) {
			DataNode node = AsDataNode("personality\n\topportunistic");
			personality.Load(node);
			THEN( "IsOpportunistic returns true" ) {
				CHECK( personality.IsOpportunistic() );
			}
		}

		WHEN( "merciful is set" ) {
			DataNode node = AsDataNode("personality\n\tmerciful");
			personality.Load(node);
			THEN( "IsMerciful returns true" ) {
				CHECK( personality.IsMerciful() );
			}
		}

		WHEN( "ramming is set" ) {
			DataNode node = AsDataNode("personality\n\tramming");
			personality.Load(node);
			THEN( "IsRamming returns true" ) {
				CHECK( personality.IsRamming() );
			}
		}

		WHEN( "getaway is set" ) {
			DataNode node = AsDataNode("personality\n\tgetaway");
			personality.Load(node);
			THEN( "IsGetaway returns true" ) {
				CHECK( personality.IsGetaway() );
			}
		}
	}
}

SCENARIO( "Personality mission NPC flags", "[Personality]" ) {
	GIVEN( "A Personality" ) {
		Personality personality;

		WHEN( "staying is set" ) {
			DataNode node = AsDataNode("personality\n\tstaying");
			personality.Load(node);
			THEN( "IsStaying returns true" ) {
				CHECK( personality.IsStaying() );
			}
		}

		WHEN( "entering is set" ) {
			DataNode node = AsDataNode("personality\n\tentering");
			personality.Load(node);
			THEN( "IsEntering returns true" ) {
				CHECK( personality.IsEntering() );
			}
		}

		WHEN( "waiting is set" ) {
			DataNode node = AsDataNode("personality\n\twaiting");
			personality.Load(node);
			THEN( "IsWaiting returns true" ) {
				CHECK( personality.IsWaiting() );
			}
		}

		WHEN( "launching is set" ) {
			DataNode node = AsDataNode("personality\n\tlaunching");
			personality.Load(node);
			THEN( "IsLaunching returns true" ) {
				CHECK( personality.IsLaunching() );
			}
		}

		WHEN( "fleeing is set" ) {
			DataNode node = AsDataNode("personality\n\tfleeing");
			personality.Load(node);
			THEN( "IsFleeing returns true" ) {
				CHECK( personality.IsFleeing() );
			}
		}

		WHEN( "derelict is set" ) {
			DataNode node = AsDataNode("personality\n\tderelict");
			personality.Load(node);
			THEN( "IsDerelict returns true" ) {
				CHECK( personality.IsDerelict() );
			}
		}

		WHEN( "uninterested is set" ) {
			DataNode node = AsDataNode("personality\n\tuninterested");
			personality.Load(node);
			THEN( "IsUninterested returns true" ) {
				CHECK( personality.IsUninterested() );
			}
		}
	}
}

SCENARIO( "Personality non-combat goal flags", "[Personality]" ) {
	GIVEN( "A Personality" ) {
		Personality personality;

		WHEN( "surveillance is set" ) {
			DataNode node = AsDataNode("personality\n\tsurveillance");
			personality.Load(node);
			THEN( "IsSurveillance returns true" ) {
				CHECK( personality.IsSurveillance() );
			}
		}

		WHEN( "mining is set" ) {
			DataNode node = AsDataNode("personality\n\tmining");
			personality.Load(node);
			THEN( "IsMining returns true" ) {
				CHECK( personality.IsMining() );
			}
		}

		WHEN( "harvests is set" ) {
			DataNode node = AsDataNode("personality\n\tharvests");
			personality.Load(node);
			THEN( "Harvests returns true" ) {
				CHECK( personality.Harvests() );
			}
		}

		WHEN( "swarming is set" ) {
			DataNode node = AsDataNode("personality\n\tswarming");
			personality.Load(node);
			THEN( "IsSwarming returns true" ) {
				CHECK( personality.IsSwarming() );
			}
		}

		WHEN( "lingering is set" ) {
			DataNode node = AsDataNode("personality\n\tlingering");
			personality.Load(node);
			THEN( "IsLingering returns true" ) {
				CHECK( personality.IsLingering() );
			}
		}

		WHEN( "secretive is set" ) {
			DataNode node = AsDataNode("personality\n\tsecretive");
			personality.Load(node);
			THEN( "IsSecretive returns true" ) {
				CHECK( personality.IsSecretive() );
			}
		}
	}
}

SCENARIO( "Personality special flags", "[Personality]" ) {
	GIVEN( "A Personality" ) {
		Personality personality;

		WHEN( "escort is set" ) {
			DataNode node = AsDataNode("personality\n\tescort");
			personality.Load(node);
			THEN( "IsEscort returns true" ) {
				CHECK( personality.IsEscort() );
			}
		}

		WHEN( "target is set" ) {
			DataNode node = AsDataNode("personality\n\ttarget");
			personality.Load(node);
			THEN( "IsTarget returns true" ) {
				CHECK( personality.IsTarget() );
			}
		}

		WHEN( "marked is set" ) {
			DataNode node = AsDataNode("personality\n\tmarked");
			personality.Load(node);
			THEN( "IsMarked returns true" ) {
				CHECK( personality.IsMarked() );
			}
		}

		WHEN( "tracked is set" ) {
			DataNode node = AsDataNode("personality\n\ttracked");
			personality.Load(node);
			THEN( "IsTracked returns true" ) {
				CHECK( personality.IsTracked() );
			}
		}

		WHEN( "mute is set" ) {
			DataNode node = AsDataNode("personality\n\tmute");
			personality.Load(node);
			THEN( "IsMute returns true" ) {
				CHECK( personality.IsMute() );
			}
		}

		WHEN( "decloaked is set" ) {
			DataNode node = AsDataNode("personality\n\tdecloaked");
			personality.Load(node);
			THEN( "IsDecloaked returns true" ) {
				CHECK( personality.IsDecloaked() );
			}
		}

		WHEN( "quiet is set" ) {
			DataNode node = AsDataNode("personality\n\tquiet");
			personality.Load(node);
			THEN( "IsQuiet returns true" ) {
				CHECK( personality.IsQuiet() );
			}
		}
	}
}

SCENARIO( "Personality static factory methods", "[Personality]" ) {
	GIVEN( "The Defender factory method" ) {
		WHEN( "creating a Defender personality" ) {
			Personality defender = Personality::Defender();
			THEN( "it has the expected flags set" ) {
				CHECK( defender.IsStaying() );
				CHECK( defender.IsMarked() );
				CHECK( defender.IsHunting() );
				CHECK( defender.IsDaring() );
				CHECK( defender.IsUnconstrained() );
				CHECK( defender.IsTarget() );
			}
			THEN( "other flags are not set" ) {
				CHECK_FALSE( defender.IsPacifist() );
				CHECK_FALSE( defender.IsTimid() );
				CHECK_FALSE( defender.IsNemesis() );
				CHECK_FALSE( defender.IsEscort() );
			}
		}
	}

	GIVEN( "The DefenderFighter factory method" ) {
		WHEN( "creating a DefenderFighter personality" ) {
			Personality defenderFighter = Personality::DefenderFighter();
			THEN( "it has the expected flags set" ) {
				CHECK( defenderFighter.IsStaying() );
				CHECK( defenderFighter.IsHunting() );
				CHECK( defenderFighter.IsDaring() );
				CHECK( defenderFighter.IsUnconstrained() );
			}
			THEN( "target and marked are not set (unlike Defender)" ) {
				CHECK_FALSE( defenderFighter.IsTarget() );
				CHECK_FALSE( defenderFighter.IsMarked() );
			}
			THEN( "other flags are not set" ) {
				CHECK_FALSE( defenderFighter.IsPacifist() );
				CHECK_FALSE( defenderFighter.IsTimid() );
				CHECK_FALSE( defenderFighter.IsEscort() );
			}
		}
	}
}

SCENARIO( "Personality confusion mechanics", "[Personality][Confusion]" ) {
	GIVEN( "A default-constructed Personality" ) {
		Personality personality;
		THEN( "confusion starts at origin" ) {
			CHECK( personality.Confusion().X() == 0. );
			CHECK( personality.Confusion().Y() == 0. );
		}

		WHEN( "UpdateConfusion is called with isFiring=false" ) {
			personality.UpdateConfusion(false);
			THEN( "confusion changes from origin" ) {
				// Confusion should have moved away from origin after update
				// The exact values depend on random angle, but it shouldn't be zero
				// Note: This test may occasionally pass even if confusion stays near origin
				// due to random angle cancellation, but that's extremely unlikely
				bool confusionChanged = (personality.Confusion().X() != 0. ||
				                         personality.Confusion().Y() != 0.);
				CHECK( confusionChanged );
			}
		}

		WHEN( "UpdateConfusion is called with isFiring=true" ) {
			personality.UpdateConfusion(true);
			THEN( "confusion changes from origin" ) {
				bool confusionChanged = (personality.Confusion().X() != 0. ||
				                         personality.Confusion().Y() != 0.);
				CHECK( confusionChanged );
			}
		}

		WHEN( "UpdateConfusion is called multiple times" ) {
			for(int i = 0; i < 10; ++i)
				personality.UpdateConfusion(false);
			THEN( "confusion remains bounded (damping effect)" ) {
				// With default confusionMultiplier of 10 and damping, values should remain reasonable
				double maxConfusion = 1000.; // A reasonable upper bound
				CHECK( std::abs(personality.Confusion().X()) < maxConfusion );
				CHECK( std::abs(personality.Confusion().Y()) < maxConfusion );
			}
		}
	}
}

SCENARIO( "Personality copying", "[Personality]" ) {
	GIVEN( "A Personality with various flags set" ) {
		Personality original;
		DataNode node = AsDataNode("personality\n\tpacifist\n\ttimid\n\thunting");
		original.Load(node);

		WHEN( "copy-constructing a new Personality" ) {
			Personality copy(original);
			THEN( "the copy has the same flags" ) {
				CHECK( copy.IsPacifist() );
				CHECK( copy.IsTimid() );
				CHECK( copy.IsHunting() );
				CHECK( copy.IsDefined() );
			}
		}

		WHEN( "copy-assigning to a new Personality" ) {
			Personality assigned;
			assigned = original;
			THEN( "the assigned copy has the same flags" ) {
				CHECK( assigned.IsPacifist() );
				CHECK( assigned.IsTimid() );
				CHECK( assigned.IsHunting() );
				CHECK( assigned.IsDefined() );
			}
		}
	}
}

SCENARIO( "Personality move semantics", "[Personality]" ) {
	GIVEN( "A Personality with various flags set" ) {
		Personality original;
		DataNode node = AsDataNode("personality\n\tpacifist\n\ttimid\n\thunting");
		original.Load(node);

		WHEN( "move-constructing a new Personality" ) {
			Personality moved(std::move(original));
			THEN( "the moved-to object has the same flags" ) {
				CHECK( moved.IsPacifist() );
				CHECK( moved.IsTimid() );
				CHECK( moved.IsHunting() );
				CHECK( moved.IsDefined() );
			}
		}

		WHEN( "move-assigning to a new Personality" ) {
			Personality assigned;
			assigned = std::move(original);
			THEN( "the assigned object has the same flags" ) {
				CHECK( assigned.IsPacifist() );
				CHECK( assigned.IsTimid() );
				CHECK( assigned.IsHunting() );
				CHECK( assigned.IsDefined() );
			}
		}
	}
}

SCENARIO( "Personality with all flags set", "[Personality]" ) {
	GIVEN( "A Personality loaded with all possible traits" ) {
		Personality personality;
		DataNode node = AsDataNode(
			"personality\n"
			"\tpacifist\n"
			"\tforbearing\n"
			"\ttimid\n"
			"\tdisables\n"
			"\tplunders\n"
			"\thunting\n"
			"\tstaying\n"
			"\tentering\n"
			"\tnemesis\n"
			"\tsurveillance\n"
			"\tuninterested\n"
			"\twaiting\n"
			"\tderelict\n"
			"\tfleeing\n"
			"\tescort\n"
			"\tfrugal\n"
			"\tcoward\n"
			"\tvindictive\n"
			"\tswarming\n"
			"\tunconstrained\n"
			"\tmining\n"
			"\tharvests\n"
			"\tappeasing\n"
			"\tmute\n"
			"\topportunistic\n"
			"\tmerciful\n"
			"\ttarget\n"
			"\tmarked\n"
			"\ttracked\n"
			"\tlaunching\n"
			"\tlingering\n"
			"\tdaring\n"
			"\tsecretive\n"
			"\tramming\n"
			"\tunrestricted\n"
			"\trestricted\n"
			"\tdecloaked\n"
			"\tquiet\n"
			"\tgetaway"
		);
		personality.Load(node);

		THEN( "all attack behavior flags are set" ) {
			CHECK( personality.IsPacifist() );
			CHECK( personality.IsForbearing() );
			CHECK( personality.IsTimid() );
			CHECK( personality.IsHunting() );
			CHECK( personality.IsNemesis() );
			CHECK( personality.IsDaring() );
		}

		THEN( "all combat style flags are set" ) {
			CHECK( personality.IsFrugal() );
			CHECK( personality.Disables() );
			CHECK( personality.Plunders() );
			CHECK( personality.IsVindictive() );
			CHECK( personality.IsUnconstrained() );
			CHECK( personality.IsUnrestricted() );
			CHECK( personality.IsRestricted() );
			CHECK( personality.IsCoward() );
			CHECK( personality.IsAppeasing() );
			CHECK( personality.IsOpportunistic() );
			CHECK( personality.IsMerciful() );
			CHECK( personality.IsRamming() );
			CHECK( personality.IsGetaway() );
		}

		THEN( "all mission NPC flags are set" ) {
			CHECK( personality.IsStaying() );
			CHECK( personality.IsEntering() );
			CHECK( personality.IsWaiting() );
			CHECK( personality.IsLaunching() );
			CHECK( personality.IsFleeing() );
			CHECK( personality.IsDerelict() );
			CHECK( personality.IsUninterested() );
		}

		THEN( "all non-combat goal flags are set" ) {
			CHECK( personality.IsSurveillance() );
			CHECK( personality.IsMining() );
			CHECK( personality.Harvests() );
			CHECK( personality.IsSwarming() );
			CHECK( personality.IsLingering() );
			CHECK( personality.IsSecretive() );
		}

		THEN( "all special flags are set" ) {
			CHECK( personality.IsEscort() );
			CHECK( personality.IsTarget() );
			CHECK( personality.IsMarked() );
			CHECK( personality.IsTracked() );
			CHECK( personality.IsMute() );
			CHECK( personality.IsDecloaked() );
			CHECK( personality.IsQuiet() );
		}
	}
}

// #endregion unit tests



} // test namespace