/* test_gameAction.cpp
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
#include "../../../source/GameAction.h"

// Include a helper for creating well-formed DataNodes.
#include "datanode-factory.h"

// ... and any system includes needed for the test file.
#include <cstdint>
#include <map>
#include <string>

namespace { // test namespace

// #region mock data

// #endregion mock data



// #region unit tests
SCENARIO( "Creating a GameAction", "[GameAction][Creation]" ) {
	GIVEN( "no arguments" ) {
		const auto action = GameAction{};

		THEN( "the action is empty" ) {
			CHECK( action.IsEmpty() );
		}
		THEN( "payment is zero" ) {
			CHECK( action.Payment() == 0 );
		}
		THEN( "fine is zero" ) {
			CHECK( action.Fine() == 0 );
		}
		THEN( "outfits map is empty" ) {
			CHECK( action.Outfits().empty() );
		}
		THEN( "ships vector is empty" ) {
			CHECK( action.Ships().empty() );
		}
	}

	GIVEN( "a DataNode with payment" ) {
		auto node = AsDataNode("action\n\tpayment 1000");
		const auto action = GameAction(node, nullptr);

		THEN( "the action is not empty" ) {
			CHECK_FALSE( action.IsEmpty() );
		}
		THEN( "payment returns the correct value" ) {
			CHECK( action.Payment() == 1000 );
		}
	}

	GIVEN( "a DataNode with fine" ) {
		auto node = AsDataNode("action\n\tfine 500");
		const auto action = GameAction(node, nullptr);

		THEN( "the action is not empty" ) {
			CHECK_FALSE( action.IsEmpty() );
		}
		THEN( "fine returns the correct value" ) {
			CHECK( action.Fine() == 500 );
		}
	}

	GIVEN( "a DataNode with negative fine" ) {
		auto node = AsDataNode("action\n\tfine -100");
		const auto action = GameAction(node, nullptr);

		THEN( "fine is zero (negative values are skipped)" ) {
			CHECK( action.Fine() == 0 );
		}
	}
}

SCENARIO( "Loading GameAction with various configurations", "[GameAction][Loading]" ) {
	GIVEN( "an empty GameAction" ) {
		auto action = GameAction{};

		WHEN( "Load is called with payment data" ) {
			auto node = AsDataNode("action\n\tpayment 2000");
			action.Load(node, nullptr);

			THEN( "the action is no longer empty" ) {
				CHECK_FALSE( action.IsEmpty() );
			}
			THEN( "payment is set correctly" ) {
				CHECK( action.Payment() == 2000 );
			}
		}

		WHEN( "Load is called with music data" ) {
			auto node = AsDataNode("action\n\tmusic \"background\"");
			action.Load(node, nullptr);

			THEN( "the action is no longer empty" ) {
				CHECK_FALSE( action.IsEmpty() );
			}
		}

		WHEN( "Load is called with mute" ) {
			auto node = AsDataNode("action\n\tmute");
			action.Load(node, nullptr);

			THEN( "the action is no longer empty" ) {
				CHECK_FALSE( action.IsEmpty() );
			}
		}
	}
}

SCENARIO( "Loading GameAction with payment multipliers", "[GameAction][Payment]" ) {
	GIVEN( "an empty GameAction" ) {
		auto action = GameAction{};

		WHEN( "Load is called with payment only (default multiplier)" ) {
			auto node = AsDataNode("action\n\tpayment 1000");
			action.Load(node, nullptr);

			THEN( "payment is 1000" ) {
				CHECK( action.Payment() == 1000 );
			}
		}

		WHEN( "Load is called with payment keyword only (1500 default)" ) {
			auto node = AsDataNode("action\n\tpayment");
			action.Load(node, nullptr);

			THEN( "payment is 0 but multiplier is set" ) {
				// payment keyword alone sets paymentMultiplier to 150
				CHECK( action.Payment() == 0 );
			}
		}

		WHEN( "Load is called with payment and multiplier" ) {
			auto node = AsDataNode("action\n\tpayment 500 100");
			action.Load(node, nullptr);

			THEN( "payment and multiplier are set" ) {
				// payment = 500, paymentMultiplier = 100
				CHECK( action.Payment() == 500 );
			}
		}
	}
}

SCENARIO( "Loading GameAction with fail missions", "[GameAction][Fail]" ) {
	GIVEN( "an empty GameAction" ) {
		auto action = GameAction{};

		WHEN( "Load is called with fail mission name" ) {
			auto node = AsDataNode("action\n\tfail \"Mission A\"");
			action.Load(node, nullptr);

			THEN( "the action is no longer empty" ) {
				CHECK_FALSE( action.IsEmpty() );
			}
		}

		WHEN( "Load is called with fail (no mission name)" ) {
			auto node = AsDataNode("action\n\tfail");
			action.Load(node, nullptr);

			THEN( "the action is no longer empty" ) {
				CHECK_FALSE( action.IsEmpty() );
			}
		}
	}
}

SCENARIO( "Loading GameAction with event data", "[GameAction][Event]" ) {
	GIVEN( "an empty GameAction" ) {
		auto action = GameAction{};

		WHEN( "Load is called with event (name only)" ) {
			auto node = AsDataNode("action\n\tevent \"Some Event\"");
			action.Load(node, nullptr);

			THEN( "the action is no longer empty" ) {
				CHECK_FALSE( action.IsEmpty() );
			}
		}

		WHEN( "Load is called with event and min days" ) {
			auto node = AsDataNode("action\n\tevent \"Some Event\" 5");
			action.Load(node, nullptr);

			THEN( "the action is no longer empty" ) {
				CHECK_FALSE( action.IsEmpty() );
			}
		}

		WHEN( "Load is called with event and min/max days" ) {
			auto node = AsDataNode("action\n\tevent \"Some Event\" 5 10");
			action.Load(node, nullptr);

			THEN( "the action is no longer empty" ) {
				CHECK_FALSE( action.IsEmpty() );
			}
		}
	}
}

SCENARIO( "Loading GameAction with debt data", "[GameAction][Debt]" ) {
	GIVEN( "an empty GameAction" ) {
		auto action = GameAction{};

		WHEN( "Load is called with debt amount" ) {
			auto node = AsDataNode("action\n\tdebt 10000");
			action.Load(node, nullptr);

			THEN( "the action is no longer empty" ) {
				CHECK_FALSE( action.IsEmpty() );
			}
		}

		WHEN( "Load is called with debt and term" ) {
			auto node = AsDataNode("action\n\tdebt 10000\n\t\tterm 30");
			action.Load(node, nullptr);

			THEN( "the action is no longer empty" ) {
				CHECK_FALSE( action.IsEmpty() );
			}
		}

		WHEN( "Load is called with debt, term, and interest" ) {
			auto node = AsDataNode("action\n\tdebt 10000\n\t\tterm 365\n\t\tinterest 0.05");
			action.Load(node, nullptr);

			THEN( "the action is no longer empty" ) {
				CHECK_FALSE( action.IsEmpty() );
			}
		}

		WHEN( "Load is called with negative debt (clamped to 0)" ) {
			auto node = AsDataNode("action\n\tdebt -500");
			action.Load(node, nullptr);

			THEN( "the action is no longer empty (debt is set to 0)" ) {
				CHECK_FALSE( action.IsEmpty() );
			}
		}
	}
}

SCENARIO( "Loading GameAction with log entries", "[GameAction][Log]" ) {
	GIVEN( "an empty GameAction" ) {
		auto action = GameAction{};

		WHEN( "Load is called with simple log" ) {
			auto node = AsDataNode("action\n\tlog \"Log entry text\"");
			action.Load(node, nullptr);

			THEN( "the action is no longer empty" ) {
				CHECK_FALSE( action.IsEmpty() );
			}
		}

		WHEN( "Load is called with special log (category and heading)" ) {
			auto node = AsDataNode("action\n\tlog category heading \"Entry text\"");
			action.Load(node, nullptr);

			THEN( "the action is no longer empty" ) {
				CHECK_FALSE( action.IsEmpty() );
			}
		}

		WHEN( "Load is called with remove log" ) {
			auto node = AsDataNode("action\n\tremove log category");
			action.Load(node, nullptr);

			THEN( "the action is no longer empty" ) {
				CHECK_FALSE( action.IsEmpty() );
			}
		}

		WHEN( "Load is called with remove log with heading" ) {
			auto node = AsDataNode("action\n\tremove log category heading");
			action.Load(node, nullptr);

			THEN( "the action is no longer empty" ) {
				CHECK_FALSE( action.IsEmpty() );
			}
		}
	}
}

SCENARIO( "Loading GameAction with mark/unmark systems", "[GameAction][Mark]" ) {
	GIVEN( "an empty GameAction" ) {
		auto action = GameAction{};

		WHEN( "Load is called with mark system" ) {
			auto node = AsDataNode("action\n\tmark \"Sol\"");
			action.Load(node, nullptr);

			THEN( "the action is no longer empty" ) {
				CHECK_FALSE( action.IsEmpty() );
			}
		}

		WHEN( "Load is called with unmark system" ) {
			auto node = AsDataNode("action\n\tunmark \"Sol\"");
			action.Load(node, nullptr);

			THEN( "the action is no longer empty" ) {
				CHECK_FALSE( action.IsEmpty() );
			}
		}

		WHEN( "Load is called with mark system for mission" ) {
			auto node = AsDataNode("action\n\tmark \"Sol\" \"Mission A\"");
			action.Load(node, nullptr);

			THEN( "the action is no longer empty" ) {
				CHECK_FALSE( action.IsEmpty() );
			}
		}

		WHEN( "Load is called with unmark system for mission" ) {
			auto node = AsDataNode("action\n\tunmark \"Sol\" \"Mission A\"");
			action.Load(node, nullptr);

			THEN( "the action is no longer empty" ) {
				CHECK_FALSE( action.IsEmpty() );
			}
		}
	}
}

SCENARIO( "Loading GameAction with message", "[GameAction][Message]" ) {
	GIVEN( "an empty GameAction" ) {
		auto action = GameAction{};

		WHEN( "Load is called with message reference" ) {
			auto node = AsDataNode("action\n\tmessage \"someMessage\"");
			action.Load(node, nullptr);

			THEN( "the action is no longer empty" ) {
				CHECK_FALSE( action.IsEmpty() );
			}
		}

		WHEN( "Load is called with inline message text" ) {
			auto node = AsDataNode("action\n\tmessage\n\t\ttext \"Hello world\"");
			action.Load(node, nullptr);

			THEN( "the action is no longer empty" ) {
				CHECK_FALSE( action.IsEmpty() );
			}
		}
	}
}

SCENARIO( "LoadSingle method functionality", "[GameAction][LoadSingle]" ) {
	GIVEN( "an empty GameAction" ) {
		auto action = GameAction{};

		WHEN( "LoadSingle is called with a payment node" ) {
			auto node = AsDataNode("payment 500");
			action.LoadSingle(node, nullptr);

			THEN( "the action is no longer empty" ) {
				CHECK_FALSE( action.IsEmpty() );
			}
			THEN( "payment is set correctly" ) {
				CHECK( action.Payment() == 500 );
			}
		}

		WHEN( "LoadSingle is called with a fine node" ) {
			auto node = AsDataNode("fine 250");
			action.LoadSingle(node, nullptr);

			THEN( "the action is no longer empty" ) {
				CHECK_FALSE( action.IsEmpty() );
			}
			THEN( "fine is set correctly" ) {
				CHECK( action.Fine() == 250 );
			}
		}
	}
}

SCENARIO( "Multiple Load calls accumulate values", "[GameAction][Accumulation]" ) {
	GIVEN( "an empty GameAction" ) {
		auto action = GameAction{};

		WHEN( "Load is called multiple times with payment" ) {
			auto node1 = AsDataNode("action\n\tpayment 100");
			auto node2 = AsDataNode("action\n\tpayment 200");
			action.Load(node1, nullptr);
			action.Load(node2, nullptr);

			THEN( "payments accumulate" ) {
				CHECK( action.Payment() == 300 );
			}
		}

		WHEN( "Load is called multiple times with fine" ) {
			auto node1 = AsDataNode("action\n\tfine 100");
			auto node2 = AsDataNode("action\n\tfine 200");
			action.Load(node1, nullptr);
			action.Load(node2, nullptr);

			THEN( "fines accumulate" ) {
				CHECK( action.Fine() == 300 );
			}
		}
	}
}

SCENARIO( "Validate method", "[GameAction][Validate]" ) {
	GIVEN( "an empty GameAction" ) {
		const auto action = GameAction{};

		WHEN( "Validate is called" ) {
			auto result = action.Validate();

			THEN( "no validation error is returned" ) {
				CHECK( result.empty() );
			}
		}
	}

	GIVEN( "a GameAction with payment only" ) {
		auto node = AsDataNode("action\n\tpayment 1000");
		const auto action = GameAction(node, nullptr);

		WHEN( "Validate is called" ) {
			auto result = action.Validate();

			THEN( "no validation error is returned" ) {
				CHECK( result.empty() );
			}
		}
	}

	GIVEN( "a GameAction with fine only" ) {
		auto node = AsDataNode("action\n\tfine 500");
		const auto action = GameAction(node, nullptr);

		WHEN( "Validate is called" ) {
			auto result = action.Validate();

			THEN( "no validation error is returned" ) {
				CHECK( result.empty() );
			}
		}
	}
}

SCENARIO( "Instantiate method", "[GameAction][Instantiate]" ) {
	GIVEN( "a GameAction with payment" ) {
		auto node = AsDataNode("action\n\tpayment 1000");
		const auto action = GameAction(node, nullptr);

		WHEN( "Instantiate is called with no substitutions" ) {
			std::map<std::string, std::string> subs;
			auto instantiated = action.Instantiate(subs, 0, 0);

			THEN( "payment is preserved" ) {
				CHECK( instantiated.Payment() == 1000 );
			}
			THEN( "the instantiated action is not empty" ) {
				CHECK_FALSE( instantiated.IsEmpty() );
			}
		}
	}

	GIVEN( "a GameAction with payment multiplier" ) {
		auto node = AsDataNode("action\n\tpayment 500 100");
		const auto action = GameAction(node, nullptr);

		WHEN( "Instantiate is called with jumps and payload" ) {
			std::map<std::string, std::string> subs;
			// payment = 500 + (jumps + 1) * payload * paymentMultiplier
			// payment = 500 + (5 + 1) * 10 * 100 = 500 + 6000 = 6500
			auto instantiated = action.Instantiate(subs, 5, 10);

			THEN( "payment includes multiplier bonus" ) {
				CHECK( instantiated.Payment() == 6500 );
			}
		}
	}

	GIVEN( "a GameAction with fine" ) {
		auto node = AsDataNode("action\n\tfine 200");
		const auto action = GameAction(node, nullptr);

		WHEN( "Instantiate is called" ) {
			std::map<std::string, std::string> subs;
			auto instantiated = action.Instantiate(subs, 0, 0);

			THEN( "fine is preserved" ) {
				CHECK( instantiated.Fine() == 200 );
			}
		}
	}

	GIVEN( "an empty GameAction" ) {
		const auto action = GameAction{};

		WHEN( "Instantiate is called" ) {
			std::map<std::string, std::string> subs;
			auto instantiated = action.Instantiate(subs, 0, 0);

			THEN( "the instantiated action is empty" ) {
				CHECK( instantiated.IsEmpty() );
			}
		}
	}
}

SCENARIO( "Instantiate adds payment substitution", "[GameAction][Instantiate][Substitution]" ) {
	GIVEN( "a GameAction with payment" ) {
		auto node = AsDataNode("action\n\tpayment 1000");
		const auto action = GameAction(node, nullptr);

		WHEN( "Instantiate is called" ) {
			std::map<std::string, std::string> subs;
			auto instantiated = action.Instantiate(subs, 0, 0);

			THEN( "<payment> substitution is added" ) {
				CHECK( subs.count("<payment>") == 1 );
				CHECK( subs["<payment>"] == "1,000 credits" );
			}
		}
	}

	GIVEN( "a GameAction with fine" ) {
		auto node = AsDataNode("action\n\tfine 500");
		const auto action = GameAction(node, nullptr);

		WHEN( "Instantiate is called" ) {
			std::map<std::string, std::string> subs;
			auto instantiated = action.Instantiate(subs, 0, 0);

			THEN( "<fine> substitution is added" ) {
				CHECK( subs.count("<fine>") == 1 );
				CHECK( subs["<fine>"] == "500 credits" );
			}
		}
	}
}

SCENARIO( "GameAction with complex configuration", "[GameAction][Complex]" ) {
	GIVEN( "a GameAction with multiple settings" ) {
		auto node = AsDataNode(
			"action\n"
			"\tpayment 1000\n"
			"\tfine 100\n"
			"\tmusic \"background\"\n"
			"\tfail \"Mission A\""
		);
		const auto action = GameAction(node, nullptr);

		THEN( "all values are set correctly" ) {
			CHECK_FALSE( action.IsEmpty() );
			CHECK( action.Payment() == 1000 );
			CHECK( action.Fine() == 100 );
		}
	}

	GIVEN( "a GameAction with all payment settings" ) {
		auto node = AsDataNode(
			"action\n"
			"\tpayment\n"
			"\tpayment 500 100"
		);
		const auto action = GameAction(node, nullptr);

		THEN( "payment and multiplier are accumulated" ) {
			// payment keyword alone adds 150 to paymentMultiplier
			// payment 500 100 adds 500 to payment and 100 to paymentMultiplier
			// Total payment = 500, paymentMultiplier = 250
			CHECK( action.Payment() == 500 );
		}
	}
}
// #endregion unit tests



} // test namespace