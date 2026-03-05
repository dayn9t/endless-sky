/* test_command.cpp
Copyright (c) 2026 by quyykk

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
#include "../../../source/Command.h"

// ... and any system includes needed for the test file.
#include <string>

namespace { // test namespace

// #region mock data
// #endregion mock data



// #region unit tests
SCENARIO( "Static command constants exist and are distinct", "[command]" ) {
	GIVEN( "the static command constants" ) {
		THEN( "each command constant is unique" ) {
			CHECK_FALSE( Command::NONE.Has(Command::MENU) );
			CHECK_FALSE( Command::NONE.Has(Command::FORWARD) );
			CHECK_FALSE( Command::MENU.Has(Command::FORWARD) );
			CHECK_FALSE( Command::FORWARD.Has(Command::LEFT) );
			CHECK_FALSE( Command::LEFT.Has(Command::RIGHT) );
			CHECK_FALSE( Command::RIGHT.Has(Command::BACK) );
			CHECK_FALSE( Command::BACK.Has(Command::PRIMARY) );
			CHECK_FALSE( Command::PRIMARY.Has(Command::SECONDARY) );
		}
		AND_THEN( "NONE is empty" ) {
			CHECK_FALSE( Command::NONE );
		}
		AND_THEN( "non-NONE commands are non-empty" ) {
			CHECK( Command::MENU );
			CHECK( Command::FORWARD );
			CHECK( Command::LEFT );
			CHECK( Command::RIGHT );
			CHECK( Command::BACK );
		}
	}
}

SCENARIO( "Creating a Command instance", "[command]" ) {
	GIVEN( "a default constructed command" ) {
		Command command;
		THEN( "it has the correct default properties" ) {
			CHECK_FALSE( command );
			CHECK_FALSE( command.Has(Command::FORWARD) );
			CHECK_FALSE( command.Has(Command::LEFT) );
			CHECK_FALSE( command.Has(Command::RIGHT) );
			CHECK( command.Turn() == 0. );
		}
	}
	GIVEN( "a command constructed from an unmapped keycode" ) {
		Command command(0);
		THEN( "it is empty" ) {
			CHECK_FALSE( command );
		}
	}
}

SCENARIO( "Using Clear method", "[command]" ) {
	GIVEN( "a command with multiple commands set" ) {
		Command command;
		command.Set(Command::FORWARD);
		command.Set(Command::LEFT);
		command.Set(Command::PRIMARY);

		REQUIRE( command.Has(Command::FORWARD) );
		REQUIRE( command.Has(Command::LEFT) );
		REQUIRE( command.Has(Command::PRIMARY) );

		WHEN( "Clear() is called without arguments" ) {
			command.Clear();
			THEN( "all commands are cleared" ) {
				CHECK_FALSE( command.Has(Command::FORWARD) );
				CHECK_FALSE( command.Has(Command::LEFT) );
				CHECK_FALSE( command.Has(Command::PRIMARY) );
				CHECK_FALSE( command );
			}
		}
	}

	GIVEN( "a command with multiple commands set" ) {
		Command command;
		command.Set(Command::FORWARD);
		command.Set(Command::LEFT);
		command.Set(Command::PRIMARY);

		WHEN( "Clear(Command) is called for a specific command" ) {
			command.Clear(Command::LEFT);
			THEN( "only that command is cleared" ) {
				CHECK( command.Has(Command::FORWARD) );
				CHECK_FALSE( command.Has(Command::LEFT) );
				CHECK( command.Has(Command::PRIMARY) );
				CHECK( command );
			}
		}
		AND_WHEN( "Clear(Command) is called for multiple commands" ) {
			command.Clear(Command::FORWARD);
			command.Clear(Command::PRIMARY);
			THEN( "those commands are cleared" ) {
				CHECK_FALSE( command.Has(Command::FORWARD) );
				CHECK( command.Has(Command::LEFT) );
				CHECK_FALSE( command.Has(Command::PRIMARY) );
			}
		}
	}

	GIVEN( "a command with turn set" ) {
		Command command;
		command.Set(Command::FORWARD);
		command.SetTurn(0.5);

		WHEN( "Clear() is called" ) {
			command.Clear();
			THEN( "turn is also reset" ) {
				CHECK( command.Turn() == 0. );
			}
		}
	}
}

SCENARIO( "Using Set method", "[command]" ) {
	GIVEN( "an empty command" ) {
		Command command;
		REQUIRE_FALSE( command );

		WHEN( "Set() is called with a single command" ) {
			command.Set(Command::FORWARD);
			THEN( "that command is set" ) {
				CHECK( command.Has(Command::FORWARD) );
				CHECK( command );
			}
		}
		AND_WHEN( "Set() is called multiple times" ) {
			command.Set(Command::FORWARD);
			command.Set(Command::LEFT);
			command.Set(Command::PRIMARY);
			THEN( "all commands are set" ) {
				CHECK( command.Has(Command::FORWARD) );
				CHECK( command.Has(Command::LEFT) );
				CHECK( command.Has(Command::PRIMARY) );
			}
		}
	}
}

SCENARIO( "Using Has method", "[command]" ) {
	GIVEN( "a command with specific commands set" ) {
		Command command;
		command.Set(Command::FORWARD);
		command.Set(Command::LEFT);

		THEN( "Has returns correct values" ) {
			CHECK( command.Has(Command::FORWARD) );
			CHECK( command.Has(Command::LEFT) );
			CHECK_FALSE( command.Has(Command::RIGHT) );
			CHECK_FALSE( command.Has(Command::BACK) );
		}
		AND_THEN( "Has works with combined commands" ) {
			Command combined = Command::FORWARD | Command::LEFT;
			CHECK( command.Has(combined) );
		}
	}
}

SCENARIO( "Using And method", "[command]" ) {
	GIVEN( "two commands with overlapping bits" ) {
		Command command1;
		command1.Set(Command::FORWARD);
		command1.Set(Command::LEFT);
		command1.Set(Command::PRIMARY);

		Command command2;
		command2.Set(Command::LEFT);
		command2.Set(Command::PRIMARY);
		command2.Set(Command::SECONDARY);

		WHEN( "And is called" ) {
			Command result = command1.And(command2);
			THEN( "only overlapping commands are in the result" ) {
				CHECK( result.Has(Command::LEFT) );
				CHECK( result.Has(Command::PRIMARY) );
				CHECK_FALSE( result.Has(Command::FORWARD) );
				CHECK_FALSE( result.Has(Command::SECONDARY) );
			}
		}
	}

	GIVEN( "two commands with no overlapping bits" ) {
		Command command1;
		command1.Set(Command::FORWARD);

		Command command2;
		command2.Set(Command::BACK);

		WHEN( "And is called" ) {
			Command result = command1.And(command2);
			THEN( "result is empty" ) {
				CHECK_FALSE( result );
			}
		}
	}
}

SCENARIO( "Using AndNot method", "[command]" ) {
	GIVEN( "two commands with overlapping bits" ) {
		Command command1;
		command1.Set(Command::FORWARD);
		command1.Set(Command::LEFT);
		command1.Set(Command::PRIMARY);

		Command command2;
		command2.Set(Command::LEFT);
		command2.Set(Command::PRIMARY);

		WHEN( "AndNot is called" ) {
			Command result = command1.AndNot(command2);
			THEN( "only non-overlapping commands remain" ) {
				CHECK( result.Has(Command::FORWARD) );
				CHECK_FALSE( result.Has(Command::LEFT) );
				CHECK_FALSE( result.Has(Command::PRIMARY) );
			}
		}
	}
}

SCENARIO( "Using SetTurn and Turn methods", "[command]" ) {
	GIVEN( "an empty command" ) {
		Command command;
		REQUIRE( command.Turn() == 0. );

		WHEN( "SetTurn is called with positive value" ) {
			command.SetTurn(0.5);
			THEN( "turn is set correctly" ) {
				CHECK( command.Turn() == 0.5 );
			}
		}
		AND_WHEN( "SetTurn is called with negative value" ) {
			command.SetTurn(-0.75);
			THEN( "turn is set correctly" ) {
				CHECK( command.Turn() == -0.75 );
			}
		}
		AND_WHEN( "SetTurn is called with full turn values" ) {
			command.SetTurn(1.0);
			THEN( "turn is set to 1.0" ) {
				CHECK( command.Turn() == 1.0 );
			}
			command.SetTurn(-1.0);
			THEN( "turn is set to -1.0" ) {
				CHECK( command.Turn() == -1.0 );
			}
		}
		AND_WHEN( "SetTurn is called with values outside range" ) {
			command.SetTurn(1.5);
			THEN( "turn is clamped to 1.0" ) {
				CHECK( command.Turn() == 1.0 );
			}
			command.SetTurn(-2.0);
			THEN( "turn is clamped to -1.0" ) {
				CHECK( command.Turn() == -1.0 );
			}
		}
	}
}

SCENARIO( "Using operator|", "[command]" ) {
	GIVEN( "two commands with different commands set" ) {
		Command command1;
		command1.Set(Command::FORWARD);
		command1.Set(Command::LEFT);

		Command command2;
		command2.Set(Command::RIGHT);
		command2.Set(Command::PRIMARY);

		WHEN( "operator| is used" ) {
			Command result = command1 | command2;
			THEN( "result contains all commands" ) {
				CHECK( result.Has(Command::FORWARD) );
				CHECK( result.Has(Command::LEFT) );
				CHECK( result.Has(Command::RIGHT) );
				CHECK( result.Has(Command::PRIMARY) );
			}
		}
	}

	GIVEN( "two commands with overlapping commands" ) {
		Command command1;
		command1.Set(Command::FORWARD);

		Command command2;
		command2.Set(Command::FORWARD);
		command2.Set(Command::LEFT);

		WHEN( "operator| is used" ) {
			Command result = command1 | command2;
			THEN( "result contains all unique commands" ) {
				CHECK( result.Has(Command::FORWARD) );
				CHECK( result.Has(Command::LEFT) );
			}
		}
	}

	GIVEN( "commands with turn values" ) {
		Command command1;
		command1.Set(Command::FORWARD);
		command1.SetTurn(0.5);

		Command command2;
		command2.Set(Command::LEFT);
		command2.SetTurn(0.75);

		WHEN( "operator| is used" ) {
			Command result = command1 | command2;
			THEN( "right operand's turn overrides" ) {
				CHECK( result.Has(Command::FORWARD) );
				CHECK( result.Has(Command::LEFT) );
				CHECK( result.Turn() == 0.75 );
			}
		}
		AND_WHEN( "command with no turn is combined" ) {
			Command command3;
			command3.Set(Command::RIGHT);

			Command result = command1 | command3;
			THEN( "original turn is preserved" ) {
				CHECK( result.Turn() == 0.5 );
			}
		}
	}
}

SCENARIO( "Using operator|=", "[command]" ) {
	GIVEN( "an empty command" ) {
		Command command;

		WHEN( "operator|= is used with FORWARD" ) {
			command |= Command::FORWARD;
			THEN( "FORWARD is set" ) {
				CHECK( command.Has(Command::FORWARD) );
			}
		}
	}

	GIVEN( "a command with FORWARD set" ) {
		Command command;
		command.Set(Command::FORWARD);

		WHEN( "operator|= is used with LEFT" ) {
			command |= Command::LEFT;
			THEN( "both commands are set" ) {
				CHECK( command.Has(Command::FORWARD) );
				CHECK( command.Has(Command::LEFT) );
			}
		}
	}

	GIVEN( "a command with turn value" ) {
		Command command;
		command.Set(Command::FORWARD);
		command.SetTurn(0.5);

		WHEN( "operator|= is used with command having different turn" ) {
			Command other;
			other.Set(Command::LEFT);
			other.SetTurn(0.75);
			command |= other;
			THEN( "turn is overridden" ) {
				CHECK( command.Turn() == 0.75 );
			}
		}
		AND_WHEN( "operator|= is used with command having no turn" ) {
			Command other;
			other.Set(Command::LEFT);
			command |= other;
			THEN( "original turn is preserved" ) {
				CHECK( command.Turn() == 0.5 );
			}
		}
	}
}

SCENARIO( "Using operator bool and operator!", "[command]" ) {
	GIVEN( "an empty command" ) {
		Command command;
		THEN( "operator bool returns false" ) {
			CHECK_FALSE( static_cast<bool>(command) );
		}
		AND_THEN( "operator! returns true" ) {
			CHECK( !command );
		}
	}

	GIVEN( "a command with a command set" ) {
		Command command;
		command.Set(Command::FORWARD);
		THEN( "operator bool returns true" ) {
			CHECK( static_cast<bool>(command) );
		}
		AND_THEN( "operator! returns false" ) {
			CHECK_FALSE( !command );
		}
	}

	GIVEN( "a command with only turn set" ) {
		Command command;
		command.SetTurn(0.5);
		THEN( "operator bool returns true" ) {
			CHECK( static_cast<bool>(command) );
		}
		AND_THEN( "operator! returns false" ) {
			CHECK_FALSE( !command );
		}
	}

	GIVEN( "a command with both command and turn set" ) {
		Command command;
		command.Set(Command::FORWARD);
		command.SetTurn(0.5);
		THEN( "both operators return correct values" ) {
			CHECK( static_cast<bool>(command) );
			CHECK_FALSE( !command );
		}
	}
}

SCENARIO( "Using operator< for sorting", "[command]" ) {
	GIVEN( "two different commands" ) {
		THEN( "they can be compared" ) {
			// Just verify the operator works, not making assumptions about ordering
			bool less = Command::FORWARD < Command::LEFT;
			bool greater = Command::LEFT < Command::FORWARD;
			CHECK( (less != greater) );
		}
	}

	GIVEN( "identical commands" ) {
		Command command1;
		command1.Set(Command::FORWARD);
		Command command2;
		command2.Set(Command::FORWARD);
		THEN( "neither is less than the other" ) {
			CHECK_FALSE( command1 < command2 );
			CHECK_FALSE( command2 < command1 );
		}
	}
}

SCENARIO( "Using Description method", "[command]" ) {
	GIVEN( "a single command constant with description" ) {
		THEN( "Description returns the correct string" ) {
			CHECK( Command::FORWARD.Description() == "Forward thrust" );
			CHECK( Command::LEFT.Description() == "Turn left" );
			CHECK( Command::RIGHT.Description() == "Turn right" );
			CHECK( Command::BACK.Description() == "Reverse" );
		}
	}

	GIVEN( "a command with no description" ) {
		THEN( "Description returns empty string" ) {
			CHECK( Command::NONE.Description().empty() );
			CHECK( Command::WAIT.Description().empty() );
			CHECK( Command::SHIFT.Description().empty() );
		}
	}

	GIVEN( "a combined command" ) {
		Command combined = Command::FORWARD | Command::LEFT;
		THEN( "Description returns empty string" ) {
			CHECK( combined.Description().empty() );
		}
	}
}

SCENARIO( "Using KeyName and HasBinding methods", "[command]" ) {
	// Note: KeyName requires LoadSettings to have been called to have a binding.
	// Without loading settings, most commands will not have bindings.
	GIVEN( "a command that has not been assigned a key" ) {
		Command command;
		command.Set(Command::FORWARD);
		THEN( "HasBinding returns false" ) {
			CHECK_FALSE( command.HasBinding() );
		}
		AND_THEN( "KeyName returns (none)" ) {
			CHECK( command.KeyName() == "(none)" );
		}
	}

	GIVEN( "the NONE command" ) {
		THEN( "HasBinding returns false" ) {
			CHECK_FALSE( Command::NONE.HasBinding() );
		}
		AND_THEN( "KeyName returns (none)" ) {
			CHECK( Command::NONE.KeyName() == "(none)" );
		}
	}

	GIVEN( "a combined command" ) {
		Command combined = Command::FORWARD | Command::LEFT;
		THEN( "HasBinding returns false" ) {
			CHECK_FALSE( combined.HasBinding() );
		}
		AND_THEN( "KeyName returns (none)" ) {
			CHECK( combined.KeyName() == "(none)" );
		}
	}
}

SCENARIO( "Combining static constants with operators", "[command]" ) {
	GIVEN( "static command constants" ) {
		WHEN( "combining with operator|" ) {
			Command combined = Command::FORWARD | Command::LEFT | Command::PRIMARY;
			THEN( "all commands are set" ) {
				CHECK( combined.Has(Command::FORWARD) );
				CHECK( combined.Has(Command::LEFT) );
				CHECK( combined.Has(Command::PRIMARY) );
				CHECK_FALSE( combined.Has(Command::RIGHT) );
			}
		}
	}

	GIVEN( "a command combined from constants" ) {
		Command command = Command::FORWARD | Command::LEFT;

		WHEN( "using And to check overlap" ) {
			Command overlap = command.And(Command::LEFT);
			THEN( "overlap contains LEFT" ) {
				CHECK( overlap.Has(Command::LEFT) );
				CHECK_FALSE( overlap.Has(Command::FORWARD) );
			}
		}

		WHEN( "using AndNot to remove commands" ) {
			Command remaining = command.AndNot(Command::LEFT);
			THEN( "only FORWARD remains" ) {
				CHECK( remaining.Has(Command::FORWARD) );
				CHECK_FALSE( remaining.Has(Command::LEFT) );
			}
		}
	}
}

SCENARIO( "Clear with combined command", "[command]" ) {
	GIVEN( "a command with multiple bits set" ) {
		Command command;
		command.Set(Command::FORWARD);
		command.Set(Command::LEFT);
		command.Set(Command::RIGHT);
		command.Set(Command::PRIMARY);

		WHEN( "Clear is called with a combined command" ) {
			Command toClear = Command::FORWARD | Command::RIGHT;
			command.Clear(toClear);
			THEN( "specified bits are cleared" ) {
				CHECK_FALSE( command.Has(Command::FORWARD) );
				CHECK( command.Has(Command::LEFT) );
				CHECK_FALSE( command.Has(Command::RIGHT) );
				CHECK( command.Has(Command::PRIMARY) );
			}
		}
	}
}

SCENARIO( "Set with combined command", "[command]" ) {
	GIVEN( "an empty command" ) {
		Command command;

		WHEN( "Set is called with a combined command" ) {
			Command toSet = Command::FORWARD | Command::LEFT | Command::RIGHT;
			command.Set(toSet);
			THEN( "all bits in combined command are set" ) {
				CHECK( command.Has(Command::FORWARD) );
				CHECK( command.Has(Command::LEFT) );
				CHECK( command.Has(Command::RIGHT) );
				CHECK_FALSE( command.Has(Command::PRIMARY) );
			}
		}
	}
}

SCENARIO( "Turn value is preserved with command operations", "[command]" ) {
	GIVEN( "a command with turn and commands" ) {
		Command command;
		command.Set(Command::FORWARD);
		command.SetTurn(0.75);

		WHEN( "Clear(Command) is called" ) {
			command.Clear(Command::LEFT);
			THEN( "turn is preserved" ) {
				CHECK( command.Turn() == 0.75 );
				CHECK( command.Has(Command::FORWARD) );
			}
		}

		AND_WHEN( "Set(Command) is called" ) {
			command.Set(Command::LEFT);
			THEN( "turn is preserved" ) {
				CHECK( command.Turn() == 0.75 );
			}
		}

		AND_WHEN( "Has(Command) is called" ) {
			THEN( "turn value does not affect Has" ) {
				CHECK( command.Has(Command::FORWARD) );
				CHECK_FALSE( command.Has(Command::LEFT) );
			}
		}
	}
}

SCENARIO( "All static constants are non-NONE", "[command]" ) {
	// Verify that all static command constants are distinct from NONE and each other
	GIVEN( "all static command constants" ) {
		THEN( "they are all non-empty" ) {
			CHECK( Command::MENU );
			CHECK( Command::FORWARD );
			CHECK( Command::LEFT );
			CHECK( Command::RIGHT );
			CHECK( Command::BACK );
			CHECK( Command::MOUSE_TURNING_HOLD );
			CHECK( Command::AIM_TURRET_HOLD );
			CHECK( Command::PRIMARY );
			CHECK( Command::TURRET_TRACKING );
			CHECK( Command::SECONDARY );
			CHECK( Command::SELECT );
			CHECK( Command::LAND );
			CHECK( Command::BOARD );
			CHECK( Command::HAIL );
			CHECK( Command::SCAN );
			CHECK( Command::JUMP );
			CHECK( Command::FLEET_JUMP );
			CHECK( Command::TARGET );
			CHECK( Command::NEAREST );
			CHECK( Command::NEAREST_ASTEROID );
			CHECK( Command::DEPLOY );
			CHECK( Command::AFTERBURNER );
			CHECK( Command::CLOAK );
			CHECK( Command::MAP );
			CHECK( Command::INFO );
			CHECK( Command::MESSAGE_LOG );
			CHECK( Command::FULLSCREEN );
			CHECK( Command::FASTFORWARD );
			CHECK( Command::HELP );
			CHECK( Command::PAUSE );
			CHECK( Command::PERFORMANCE_DISPLAY );
			CHECK( Command::FIGHT );
			CHECK( Command::HOLD_FIRE );
			CHECK( Command::GATHER );
			CHECK( Command::HOLD_POSITION );
			CHECK( Command::AMMO );
			CHECK( Command::HARVEST );
			CHECK( Command::AUTOSTEER );
			CHECK( Command::WAIT );
			CHECK( Command::STOP );
			CHECK( Command::SHIFT );
		}
	}
}

// #endregion unit tests



} // test namespace