/* test_npc.cpp
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
#include "../../../source/NPC.h"

// Include helper for creating DataNodes.
#include "datanode-factory.h"

// ... and any system includes needed for the test file.
#include <list>
#include <memory>
#include <string>

namespace { // test namespace

// #region mock data

// #endregion mock data



// #region unit tests
TEST_CASE( "NPC Class Traits", "[NPC]" ) {
	using T = NPC;
	SECTION( "Class Traits" ) {
		CHECK_FALSE( std::is_trivial_v<T> );
		CHECK( std::is_standard_layout_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
	}
	SECTION( "Construction Traits" ) {
		CHECK( std::is_default_constructible_v<T> );
		CHECK_FALSE( std::is_trivially_default_constructible_v<T> );
		CHECK( std::is_nothrow_default_constructible_v<T> );
		// Copy construction is deleted.
		CHECK_FALSE( std::is_copy_constructible_v<T> );
		CHECK( std::is_move_constructible_v<T> );
		CHECK( std::is_nothrow_move_constructible_v<T> );
	}
	SECTION( "Copy Traits" ) {
		// Copy assignment is deleted.
		CHECK_FALSE( std::is_copy_assignable_v<T> );
	}
	SECTION( "Move Traits" ) {
		CHECK( std::is_move_assignable_v<T> );
		CHECK( std::is_nothrow_move_assignable_v<T> );
	}
}

SCENARIO( "Creating an NPC with default constructor", "[NPC][Construction]" ) {
	GIVEN( "A default-constructed NPC" ) {
		NPC npc;
		THEN( "UUID is valid and non-empty" ) {
			CHECK_FALSE( npc.UUID().ToString().empty() );
		}
		THEN( "Ships list is empty" ) {
			CHECK( npc.Ships().empty() );
		}
		THEN( "ShouldSpawn returns true (empty spawn conditions)" ) {
			CHECK( npc.ShouldSpawn() );
		}
		THEN( "HasFailed returns false" ) {
			CHECK_FALSE( npc.HasFailed() );
		}
		THEN( "HasSucceeded returns true (no ships and despawnable)" ) {
			CHECK( npc.HasSucceeded(nullptr) );
		}
		THEN( "IsLeftBehind returns false (no mustAccompany)" ) {
			CHECK_FALSE( npc.IsLeftBehind(nullptr) );
		}
		THEN( "GetPersonality returns a default personality" ) {
			CHECK_FALSE( npc.GetPersonality().IsDefined() );
		}
		THEN( "Validate returns empty string for instantiated NPC" ) {
			CHECK( npc.Validate().empty() );
		}
		THEN( "Validate returns empty string for template NPC" ) {
			CHECK( npc.Validate(true).empty() );
		}
	}
}

SCENARIO( "NPC UUID behavior", "[NPC][UUID]" ) {
	GIVEN( "Two default-constructed NPCs" ) {
		NPC npc1;
		NPC npc2;
		THEN( "Their UUIDs are different" ) {
			CHECK( npc1.UUID().ToString() != npc2.UUID().ToString() );
		}
		THEN( "Each UUID is valid and not empty" ) {
			CHECK_FALSE( npc1.UUID().ToString().empty() );
			CHECK_FALSE( npc2.UUID().ToString().empty() );
		}
	}
}

SCENARIO( "NPC move semantics", "[NPC][Move]" ) {
	GIVEN( "An NPC with default construction" ) {
		NPC original;
		std::string originalUuid = original.UUID().ToString();
		WHEN( "Move-constructing a new NPC" ) {
			NPC moved(std::move(original));
			THEN( "The moved-to NPC has the same UUID" ) {
				CHECK( moved.UUID().ToString() == originalUuid );
			}
			THEN( "The moved-to NPC is in a valid state" ) {
				CHECK_FALSE( moved.HasFailed() );
				CHECK( moved.ShouldSpawn() );
			}
		}
	}
	GIVEN( "Two NPCs" ) {
		NPC npc1;
		NPC npc2;
		std::string uuid1 = npc1.UUID().ToString();
		std::string uuid2 = npc2.UUID().ToString();
		REQUIRE( uuid1 != uuid2 );
		WHEN( "Move-assigning one to another" ) {
			npc2 = std::move(npc1);
			THEN( "The assigned NPC has the source's UUID" ) {
				CHECK( npc2.UUID().ToString() == uuid1 );
			}
		}
	}
}

SCENARIO( "NPC spawning conditions", "[NPC][Spawning]" ) {
	GIVEN( "A default-constructed NPC" ) {
		NPC npc;
		THEN( "ShouldSpawn returns true by default" ) {
			CHECK( npc.ShouldSpawn() );
		}
		THEN( "HasSucceeded returns true when ignoreIfDespawnable" ) {
			CHECK( npc.HasSucceeded(nullptr, true) );
		}
		THEN( "HasSucceeded returns true even when not ignoring despawnable" ) {
			CHECK( npc.HasSucceeded(nullptr, false) );
		}
	}
}

SCENARIO( "NPC failure state", "[NPC][Failure]" ) {
	GIVEN( "A default-constructed NPC" ) {
		NPC npc;
		THEN( "HasFailed returns false by default" ) {
			CHECK_FALSE( npc.HasFailed() );
		}
		THEN( "IsLeftBehind returns false by default" ) {
			CHECK_FALSE( npc.IsLeftBehind(nullptr) );
		}
	}
}

SCENARIO( "NPC Ships list", "[NPC][Ships]" ) {
	GIVEN( "A default-constructed NPC" ) {
		NPC npc;
		THEN( "Ships returns an empty list" ) {
			auto ships = npc.Ships();
			CHECK( ships.empty() );
		}
	}
}

SCENARIO( "NPC Personality", "[NPC][Personality]" ) {
	GIVEN( "A default-constructed NPC" ) {
		NPC npc;
		THEN( "GetPersonality returns a default personality" ) {
			const Personality &personality = npc.GetPersonality();
			CHECK_FALSE( personality.IsDefined() );
		}
		THEN( "Default personality has disables flag set" ) {
			const Personality &personality = npc.GetPersonality();
			CHECK( personality.Disables() );
		}
	}
}

SCENARIO( "NPC Validate method", "[NPC][Validate]" ) {
	GIVEN( "A default-constructed NPC" ) {
		NPC npc;
		WHEN( "Validating as instantiated NPC" ) {
			std::string result = npc.Validate(false);
			THEN( "No validation errors" ) {
				CHECK( result.empty() );
			}
		}
		WHEN( "Validating as template NPC" ) {
			std::string result = npc.Validate(true);
			THEN( "No validation errors" ) {
				CHECK( result.empty() );
			}
		}
	}
}

SCENARIO( "NPC Trigger enum values", "[NPC][Trigger]" ) {
	GIVEN( "The NPC::Trigger enum" ) {
		THEN( "All expected trigger values exist" ) {
			CHECK( static_cast<int>(NPC::Trigger::ASSIST) >= 0 );
			CHECK( static_cast<int>(NPC::Trigger::SCAN_CARGO) >= 0 );
			CHECK( static_cast<int>(NPC::Trigger::SCAN_OUTFITS) >= 0 );
			CHECK( static_cast<int>(NPC::Trigger::PROVOKE) >= 0 );
			CHECK( static_cast<int>(NPC::Trigger::DISABLE) >= 0 );
			CHECK( static_cast<int>(NPC::Trigger::BOARD) >= 0 );
			CHECK( static_cast<int>(NPC::Trigger::CAPTURE) >= 0 );
			CHECK( static_cast<int>(NPC::Trigger::DESTROY) >= 0 );
			CHECK( static_cast<int>(NPC::Trigger::ENCOUNTER) >= 0 );
			CHECK( static_cast<int>(NPC::Trigger::KILL) >= 0 );
		}
		THEN( "Trigger values are distinct" ) {
			CHECK( NPC::Trigger::ASSIST != NPC::Trigger::SCAN_CARGO );
			CHECK( NPC::Trigger::SCAN_CARGO != NPC::Trigger::SCAN_OUTFITS );
			CHECK( NPC::Trigger::BOARD != NPC::Trigger::CAPTURE );
			CHECK( NPC::Trigger::DESTROY != NPC::Trigger::KILL );
		}
	}
}

SCENARIO( "NPC HasSucceeded with no ships", "[NPC][HasSucceeded]" ) {
	GIVEN( "A default-constructed NPC with no ships" ) {
		NPC npc;
		WHEN( "Checking HasSucceeded with null playerSystem" ) {
			THEN( "Returns true (no ships to check)" ) {
				CHECK( npc.HasSucceeded(nullptr) );
				CHECK( npc.HasSucceeded(nullptr, true) );
				CHECK( npc.HasSucceeded(nullptr, false) );
			}
		}
	}
}

SCENARIO( "NPC IsLeftBehind with no ships", "[NPC][IsLeftBehind]" ) {
	GIVEN( "A default-constructed NPC with no ships" ) {
		NPC npc;
		WHEN( "Checking IsLeftBehind" ) {
			THEN( "Returns false (mustAccompany is false)" ) {
				CHECK_FALSE( npc.IsLeftBehind(nullptr) );
			}
		}
	}
}

SCENARIO( "NPC loading from minimal DataNode", "[NPC][Load]" ) {
	GIVEN( "A minimal npc DataNode" ) {
		DataNode node = AsDataNode("npc");
		WHEN( "Loading the NPC" ) {
			NPC npc(node, nullptr, nullptr, nullptr);
			THEN( "NPC is created successfully" ) {
				CHECK_FALSE( npc.UUID().ToString().empty() );
				CHECK( npc.Ships().empty() );
				CHECK( npc.ShouldSpawn() );
			}
		}
	}
}

SCENARIO( "NPC loading with objective tokens", "[NPC][Load][Objectives]" ) {
	GIVEN( "An NPC DataNode with 'save' objective" ) {
		DataNode node = AsDataNode("npc save");
		WHEN( "Loading the NPC" ) {
			NPC npc(node, nullptr, nullptr, nullptr);
			THEN( "NPC is created successfully" ) {
				CHECK_FALSE( npc.HasFailed() );
			}
		}
	}
	GIVEN( "An NPC DataNode with 'kill' objective" ) {
		DataNode node = AsDataNode("npc kill");
		WHEN( "Loading the NPC" ) {
			NPC npc(node, nullptr, nullptr, nullptr);
			THEN( "NPC is created successfully" ) {
				CHECK_FALSE( npc.HasFailed() );
			}
		}
	}
	GIVEN( "An NPC DataNode with 'board' objective" ) {
		DataNode node = AsDataNode("npc board");
		WHEN( "Loading the NPC" ) {
			NPC npc(node, nullptr, nullptr, nullptr);
			THEN( "NPC is created successfully" ) {
				CHECK_FALSE( npc.HasFailed() );
			}
		}
	}
	GIVEN( "An NPC DataNode with 'assist' objective" ) {
		DataNode node = AsDataNode("npc assist");
		WHEN( "Loading the NPC" ) {
			NPC npc(node, nullptr, nullptr, nullptr);
			THEN( "NPC is created successfully" ) {
				CHECK_FALSE( npc.HasFailed() );
			}
		}
	}
	GIVEN( "An NPC DataNode with 'evade' objective" ) {
		DataNode node = AsDataNode("npc evade");
		WHEN( "Loading the NPC" ) {
			NPC npc(node, nullptr, nullptr, nullptr);
			THEN( "NPC is created successfully" ) {
				CHECK_FALSE( npc.HasFailed() );
			}
		}
	}
	GIVEN( "An NPC DataNode with 'accompany' objective" ) {
		DataNode node = AsDataNode("npc accompany");
		WHEN( "Loading the NPC" ) {
			NPC npc(node, nullptr, nullptr, nullptr);
			THEN( "NPC is created successfully" ) {
				CHECK_FALSE( npc.HasFailed() );
			}
		}
	}
}

SCENARIO( "NPC loading with multiple objectives", "[NPC][Load][Objectives]" ) {
	GIVEN( "An NPC DataNode with multiple objective tokens" ) {
		DataNode node = AsDataNode("npc kill board");
		WHEN( "Loading the NPC" ) {
			NPC npc(node, nullptr, nullptr, nullptr);
			THEN( "NPC is created successfully" ) {
				CHECK_FALSE( npc.HasFailed() );
			}
		}
	}
	GIVEN( "An NPC DataNode with conflicting objectives (save and kill)" ) {
		// Note: This combination generates a warning trace but still creates the NPC.
		DataNode node = AsDataNode("npc save kill");
		WHEN( "Loading the NPC" ) {
			NPC npc(node, nullptr, nullptr, nullptr);
			THEN( "NPC is created (with warning)" ) {
				CHECK_FALSE( npc.HasFailed() );
			}
		}
	}
}

SCENARIO( "NPC loading with child nodes", "[NPC][Load]" ) {
	GIVEN( "An NPC DataNode with personality child node" ) {
		DataNode node = AsDataNode("npc\n\tpersonality\n\t\tpacifist");
		WHEN( "Loading the NPC" ) {
			NPC npc(node, nullptr, nullptr, nullptr);
			THEN( "NPC personality is set" ) {
				CHECK( npc.GetPersonality().IsDefined() );
				CHECK( npc.GetPersonality().IsPacifist() );
			}
		}
	}
	GIVEN( "An NPC DataNode with uuid child node" ) {
		DataNode node = AsDataNode("npc\n\tuuid \"5be91256-f6ba-47cd-96df-1ce1cb4fee86\"");
		WHEN( "Loading the NPC" ) {
			NPC npc(node, nullptr, nullptr, nullptr);
			THEN( "NPC uuid is set from the data" ) {
				CHECK( npc.UUID().ToString() == "5be91256-f6ba-47cd-96df-1ce1cb4fee86" );
			}
		}
	}
	GIVEN( "An NPC DataNode with evade child node" ) {
		DataNode node = AsDataNode("npc\n\tevade");
		WHEN( "Loading the NPC" ) {
			NPC npc(node, nullptr, nullptr, nullptr);
			THEN( "NPC is created successfully" ) {
				CHECK_FALSE( npc.HasFailed() );
			}
		}
	}
	GIVEN( "An NPC DataNode with accompany child node" ) {
		DataNode node = AsDataNode("npc\n\taccompany");
		WHEN( "Loading the NPC" ) {
			NPC npc(node, nullptr, nullptr, nullptr);
			THEN( "NPC is created successfully" ) {
				CHECK_FALSE( npc.HasFailed() );
			}
		}
	}
}

SCENARIO( "NPC loading with spawn/despawn conditions", "[NPC][Load][Conditions]" ) {
	GIVEN( "An NPC DataNode with to spawn conditions" ) {
		DataNode node = AsDataNode("npc\n\tto spawn\n\t\tnever");
		WHEN( "Loading the NPC" ) {
			NPC npc(node, nullptr, nullptr, nullptr);
			THEN( "NPC should not spawn (conditions not met)" ) {
				CHECK_FALSE( npc.ShouldSpawn() );
			}
		}
	}
}

SCENARIO( "NPC with on triggers", "[NPC][Load][Triggers]" ) {
	GIVEN( "An NPC DataNode with 'on destroy' trigger" ) {
		DataNode node = AsDataNode("npc\n\ton destroy\n\t\ttriggered");
		WHEN( "Loading the NPC" ) {
			NPC npc(node, nullptr, nullptr, nullptr);
			THEN( "NPC is created successfully" ) {
				CHECK_FALSE( npc.HasFailed() );
			}
		}
	}
	GIVEN( "An NPC DataNode with 'on board' trigger" ) {
		DataNode node = AsDataNode("npc\n\ton board\n\t\ttriggered");
		WHEN( "Loading the NPC" ) {
			NPC npc(node, nullptr, nullptr, nullptr);
			THEN( "NPC is created successfully" ) {
				CHECK_FALSE( npc.HasFailed() );
			}
		}
	}
	GIVEN( "An NPC DataNode with 'on encounter' trigger" ) {
		DataNode node = AsDataNode("npc\n\ton encounter\n\t\ttriggered");
		WHEN( "Loading the NPC" ) {
			NPC npc(node, nullptr, nullptr, nullptr);
			THEN( "NPC is created successfully" ) {
				CHECK_FALSE( npc.HasFailed() );
			}
		}
	}
	GIVEN( "An NPC DataNode with 'on kill' trigger" ) {
		DataNode node = AsDataNode("npc\n\ton kill\n\t\ttriggered");
		WHEN( "Loading the NPC" ) {
			NPC npc(node, nullptr, nullptr, nullptr);
			THEN( "NPC is created successfully" ) {
				CHECK_FALSE( npc.HasFailed() );
			}
		}
	}
}

SCENARIO( "NPC boundary conditions", "[NPC][Boundary]" ) {
	GIVEN( "An NPC with no ships and no objectives" ) {
		NPC npc;
		WHEN( "Checking various states" ) {
			THEN( "All state checks return expected defaults" ) {
				CHECK( npc.ShouldSpawn() );
				CHECK_FALSE( npc.HasFailed() );
				CHECK( npc.HasSucceeded(nullptr) );
				CHECK_FALSE( npc.IsLeftBehind(nullptr) );
			}
		}
	}
	GIVEN( "An NPC loaded from an empty DataNode" ) {
		DataNode node = AsDataNode("npc");
		NPC npc(node, nullptr, nullptr, nullptr);
		WHEN( "Checking states with empty configuration" ) {
			THEN( "NPC is in default state" ) {
				CHECK( npc.ShouldSpawn() );
				CHECK_FALSE( npc.HasFailed() );
				CHECK( npc.Ships().empty() );
			}
		}
	}
}

SCENARIO( "NPC with system destination", "[NPC][Load][System]" ) {
	GIVEN( "An NPC DataNode with system destination" ) {
		DataNode node = AsDataNode("npc\n\tsystem destination");
		WHEN( "Loading the NPC" ) {
			NPC npc(node, nullptr, nullptr, nullptr);
			THEN( "NPC is created successfully" ) {
				CHECK( npc.ShouldSpawn() );
			}
		}
	}
}

SCENARIO( "NPC with dialog", "[NPC][Load][Dialog]" ) {
	GIVEN( "An NPC DataNode with dialog text" ) {
		DataNode node = AsDataNode("npc\n\tdialog\n\t\t\"Test dialog message\"");
		WHEN( "Loading the NPC" ) {
			NPC npc(node, nullptr, nullptr, nullptr);
			THEN( "NPC is created successfully" ) {
				CHECK( npc.ShouldSpawn() );
				CHECK_FALSE( npc.HasFailed() );
			}
		}
	}
}

SCENARIO( "NPC copying is disabled", "[NPC][Copy]" ) {
	GIVEN( "An NPC instance" ) {
		NPC npc;
		THEN( "Copy construction is not available" ) {
			CHECK_FALSE( std::is_copy_constructible_v<NPC> );
		}
		THEN( "Copy assignment is not available" ) {
			CHECK_FALSE( std::is_copy_assignable_v<NPC> );
		}
	}
}

// #endregion unit tests



} // test namespace