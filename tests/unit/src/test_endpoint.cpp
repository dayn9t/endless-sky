/* test_endpoint.cpp
Copyright (c) 2025 by Amazinite

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
#include "../../../source/Endpoint.h"

// ... and any system includes needed for the test file.
#include "../../../source/DataWriter.h"
#include <string>

namespace { // test namespace

// #region mock data
// #endregion mock data



// #region unit tests

SCENARIO( "Endpoint constants are defined correctly", "[endpoint]" ) {
	GIVEN( "the Endpoint class constants" ) {
		THEN( "all endpoint values are negative and unique" ) {
			CHECK( Endpoint::ACCEPT == -1 );
			CHECK( Endpoint::DECLINE == -2 );
			CHECK( Endpoint::DEFER == -3 );
			CHECK( Endpoint::LAUNCH == -4 );
			CHECK( Endpoint::FLEE == -5 );
			CHECK( Endpoint::DEPART == -6 );
			CHECK( Endpoint::DIE == -7 );
			CHECK( Endpoint::EXPLODE == -8 );
		}
	}
}

SCENARIO( "Endpoint::RequiresLaunch identifies launch-triggering outcomes", "[endpoint]" ) {
	GIVEN( "outcomes that require launch" ) {
		THEN( "LAUNCH requires launch" ) {
			CHECK( Endpoint::RequiresLaunch(Endpoint::LAUNCH) );
		}
		THEN( "FLEE requires launch" ) {
			CHECK( Endpoint::RequiresLaunch(Endpoint::FLEE) );
		}
		THEN( "DEPART requires launch" ) {
			CHECK( Endpoint::RequiresLaunch(Endpoint::DEPART) );
		}
	}
	GIVEN( "outcomes that do not require launch" ) {
		THEN( "ACCEPT does not require launch" ) {
			CHECK_FALSE( Endpoint::RequiresLaunch(Endpoint::ACCEPT) );
		}
		THEN( "DECLINE does not require launch" ) {
			CHECK_FALSE( Endpoint::RequiresLaunch(Endpoint::DECLINE) );
		}
		THEN( "DEFER does not require launch" ) {
			CHECK_FALSE( Endpoint::RequiresLaunch(Endpoint::DEFER) );
		}
		THEN( "DIE does not require launch" ) {
			CHECK_FALSE( Endpoint::RequiresLaunch(Endpoint::DIE) );
		}
		THEN( "EXPLODE does not require launch" ) {
			CHECK_FALSE( Endpoint::RequiresLaunch(Endpoint::EXPLODE) );
		}
	}
	GIVEN( "arbitrary integer values" ) {
		THEN( "positive values do not require launch" ) {
			CHECK_FALSE( Endpoint::RequiresLaunch(0) );
			CHECK_FALSE( Endpoint::RequiresLaunch(1) );
			CHECK_FALSE( Endpoint::RequiresLaunch(42) );
			CHECK_FALSE( Endpoint::RequiresLaunch(100) );
		}
		THEN( "other negative values do not require launch" ) {
			CHECK_FALSE( Endpoint::RequiresLaunch(-9) );
			CHECK_FALSE( Endpoint::RequiresLaunch(-100) );
		}
	}
}

SCENARIO( "Endpoint::TokenIndex converts token strings to indices", "[endpoint]" ) {
	GIVEN( "valid endpoint token strings" ) {
		THEN( "\"accept\" maps to ACCEPT" ) {
			CHECK( Endpoint::TokenIndex("accept") == Endpoint::ACCEPT );
		}
		THEN( "\"decline\" maps to DECLINE" ) {
			CHECK( Endpoint::TokenIndex("decline") == Endpoint::DECLINE );
		}
		THEN( "\"defer\" maps to DEFER" ) {
			CHECK( Endpoint::TokenIndex("defer") == Endpoint::DEFER );
		}
		THEN( "\"launch\" maps to LAUNCH" ) {
			CHECK( Endpoint::TokenIndex("launch") == Endpoint::LAUNCH );
		}
		THEN( "\"flee\" maps to FLEE" ) {
			CHECK( Endpoint::TokenIndex("flee") == Endpoint::FLEE );
		}
		THEN( "\"depart\" maps to DEPART" ) {
			CHECK( Endpoint::TokenIndex("depart") == Endpoint::DEPART );
		}
		THEN( "\"die\" maps to DIE" ) {
			CHECK( Endpoint::TokenIndex("die") == Endpoint::DIE );
		}
		THEN( "\"explode\" maps to EXPLODE" ) {
			CHECK( Endpoint::TokenIndex("explode") == Endpoint::EXPLODE );
		}
	}
	GIVEN( "invalid token strings" ) {
		THEN( "unknown strings return 0" ) {
			CHECK( Endpoint::TokenIndex("unknown") == 0 );
			CHECK( Endpoint::TokenIndex("random") == 0 );
			CHECK( Endpoint::TokenIndex("") == 0 );
		}
		THEN( "case-sensitive matching is required" ) {
			CHECK( Endpoint::TokenIndex("Accept") == 0 );
			CHECK( Endpoint::TokenIndex("ACCEPT") == 0 );
			CHECK( Endpoint::TokenIndex("Accept") == 0 );
		}
		THEN( "strings with whitespace return 0" ) {
			CHECK( Endpoint::TokenIndex(" accept") == 0 );
			CHECK( Endpoint::TokenIndex("accept ") == 0 );
			CHECK( Endpoint::TokenIndex(" accept ") == 0 );
		}
	}
}

SCENARIO( "Endpoint::TokenName converts indices back to token strings", "[endpoint]" ) {
	GIVEN( "valid endpoint indices" ) {
		THEN( "ACCEPT maps to \"accept\"" ) {
			CHECK( Endpoint::TokenName(Endpoint::ACCEPT) == "accept" );
		}
		THEN( "DECLINE maps to \"decline\"" ) {
			CHECK( Endpoint::TokenName(Endpoint::DECLINE) == "decline" );
		}
		THEN( "DEFER maps to \"defer\"" ) {
			CHECK( Endpoint::TokenName(Endpoint::DEFER) == "defer" );
		}
		THEN( "LAUNCH maps to \"launch\"" ) {
			CHECK( Endpoint::TokenName(Endpoint::LAUNCH) == "launch" );
		}
		THEN( "FLEE maps to \"flee\"" ) {
			CHECK( Endpoint::TokenName(Endpoint::FLEE) == "flee" );
		}
		THEN( "DEPART maps to \"depart\"" ) {
			CHECK( Endpoint::TokenName(Endpoint::DEPART) == "depart" );
		}
		THEN( "DIE maps to \"die\"" ) {
			CHECK( Endpoint::TokenName(Endpoint::DIE) == "die" );
		}
		THEN( "EXPLODE maps to \"explode\"" ) {
			CHECK( Endpoint::TokenName(Endpoint::EXPLODE) == "explode" );
		}
	}
	GIVEN( "arbitrary integer values" ) {
		THEN( "positive values return their string representation" ) {
			CHECK( Endpoint::TokenName(0) == "0" );
			CHECK( Endpoint::TokenName(1) == "1" );
			CHECK( Endpoint::TokenName(42) == "42" );
			CHECK( Endpoint::TokenName(100) == "100" );
		}
		THEN( "unknown negative values return their string representation" ) {
			CHECK( Endpoint::TokenName(-9) == "-9" );
			CHECK( Endpoint::TokenName(-100) == "-100" );
		}
	}
}

SCENARIO( "Endpoint token roundtrip conversion", "[endpoint]" ) {
	GIVEN( "all valid endpoint tokens" ) {
		THEN( "TokenIndex and TokenName are inverse operations" ) {
			CHECK( Endpoint::TokenName(Endpoint::TokenIndex("accept")) == "accept" );
			CHECK( Endpoint::TokenName(Endpoint::TokenIndex("decline")) == "decline" );
			CHECK( Endpoint::TokenName(Endpoint::TokenIndex("defer")) == "defer" );
			CHECK( Endpoint::TokenName(Endpoint::TokenIndex("launch")) == "launch" );
			CHECK( Endpoint::TokenName(Endpoint::TokenIndex("flee")) == "flee" );
			CHECK( Endpoint::TokenName(Endpoint::TokenIndex("depart")) == "depart" );
			CHECK( Endpoint::TokenName(Endpoint::TokenIndex("die")) == "die" );
			CHECK( Endpoint::TokenName(Endpoint::TokenIndex("explode")) == "explode" );
		}
	}
	GIVEN( "all valid endpoint indices" ) {
		THEN( "TokenIndex and TokenName are inverse operations" ) {
			CHECK( Endpoint::TokenIndex(Endpoint::TokenName(Endpoint::ACCEPT)) == Endpoint::ACCEPT );
			CHECK( Endpoint::TokenIndex(Endpoint::TokenName(Endpoint::DECLINE)) == Endpoint::DECLINE );
			CHECK( Endpoint::TokenIndex(Endpoint::TokenName(Endpoint::DEFER)) == Endpoint::DEFER );
			CHECK( Endpoint::TokenIndex(Endpoint::TokenName(Endpoint::LAUNCH)) == Endpoint::LAUNCH );
			CHECK( Endpoint::TokenIndex(Endpoint::TokenName(Endpoint::FLEE)) == Endpoint::FLEE );
			CHECK( Endpoint::TokenIndex(Endpoint::TokenName(Endpoint::DEPART)) == Endpoint::DEPART );
			CHECK( Endpoint::TokenIndex(Endpoint::TokenName(Endpoint::DIE)) == Endpoint::DIE );
			CHECK( Endpoint::TokenIndex(Endpoint::TokenName(Endpoint::EXPLODE)) == Endpoint::EXPLODE );
		}
	}
}

SCENARIO( "Endpoint::WriteToken writes tokens correctly", "[endpoint]" ) {
	GIVEN( "a DataWriter" ) {
		DataWriter writer;
		WHEN( "writing a goto token with positive index" ) {
			Endpoint::WriteToken(5, writer);
			THEN( "it writes \"goto 5\" with proper indentation" ) {
				CHECK( writer.SaveToString() == "\tgoto 5\n" );
			}
		}
	}
	GIVEN( "a DataWriter" ) {
		DataWriter writer;
		WHEN( "writing a goto token with zero index" ) {
			Endpoint::WriteToken(0, writer);
			THEN( "it writes \"goto 0\" with proper indentation" ) {
				CHECK( writer.SaveToString() == "\tgoto 0\n" );
			}
		}
	}
	GIVEN( "a DataWriter" ) {
		DataWriter writer;
		WHEN( "writing endpoint tokens" ) {
			Endpoint::WriteToken(Endpoint::ACCEPT, writer);
			THEN( "it writes the token name with proper indentation" ) {
				CHECK( writer.SaveToString() == "\taccept\n" );
			}
		}
	}
	GIVEN( "a DataWriter" ) {
		DataWriter writer;
		WHEN( "writing decline token" ) {
			Endpoint::WriteToken(Endpoint::DECLINE, writer);
			THEN( "it writes the token name with proper indentation" ) {
				CHECK( writer.SaveToString() == "\tdecline\n" );
			}
		}
	}
	GIVEN( "a DataWriter" ) {
		DataWriter writer;
		WHEN( "writing defer token" ) {
			Endpoint::WriteToken(Endpoint::DEFER, writer);
			THEN( "it writes the token name with proper indentation" ) {
				CHECK( writer.SaveToString() == "\tdefer\n" );
			}
		}
	}
	GIVEN( "a DataWriter" ) {
		DataWriter writer;
		WHEN( "writing launch token" ) {
			Endpoint::WriteToken(Endpoint::LAUNCH, writer);
			THEN( "it writes the token name with proper indentation" ) {
				CHECK( writer.SaveToString() == "\tlaunch\n" );
			}
		}
	}
	GIVEN( "a DataWriter" ) {
		DataWriter writer;
		WHEN( "writing flee token" ) {
			Endpoint::WriteToken(Endpoint::FLEE, writer);
			THEN( "it writes the token name with proper indentation" ) {
				CHECK( writer.SaveToString() == "\tflee\n" );
			}
		}
	}
	GIVEN( "a DataWriter" ) {
		DataWriter writer;
		WHEN( "writing depart token" ) {
			Endpoint::WriteToken(Endpoint::DEPART, writer);
			THEN( "it writes the token name with proper indentation" ) {
				CHECK( writer.SaveToString() == "\tdepart\n" );
			}
		}
	}
	GIVEN( "a DataWriter" ) {
		DataWriter writer;
		WHEN( "writing die token" ) {
			Endpoint::WriteToken(Endpoint::DIE, writer);
			THEN( "it writes the token name with proper indentation" ) {
				CHECK( writer.SaveToString() == "\tdie\n" );
			}
		}
	}
	GIVEN( "a DataWriter" ) {
		DataWriter writer;
		WHEN( "writing explode token" ) {
			Endpoint::WriteToken(Endpoint::EXPLODE, writer);
			THEN( "it writes the token name with proper indentation" ) {
				CHECK( writer.SaveToString() == "\texplode\n" );
			}
		}
	}
	GIVEN( "a DataWriter with existing content" ) {
		DataWriter writer;
		writer.Write("conversation");
		WHEN( "writing a goto token inside a child block" ) {
			writer.BeginChild();
			Endpoint::WriteToken(10, writer);
			writer.EndChild();
			THEN( "it maintains proper indentation hierarchy" ) {
				// Note: BeginChild increases indentation by one tab
				CHECK( writer.SaveToString() == "conversation\n\t\tgoto 10\n" );
			}
		}
	}
}

// #endregion unit tests



} // test namespace