/* test_printData.cpp
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
#include "../../../source/PrintData.h"

// ... and any system includes needed for the test file.
#include <string>
#include <vector>

#include "../include/output-capture.hpp"

namespace { // test namespace

// #region mock data

// Helper function to create a null-terminated argv array from a vector of strings.
// The returned vector contains the pointers and a null terminator.
std::vector<const char *> MakeArgv(const std::vector<std::string> &args)
{
	std::vector<const char *> argv;
	argv.reserve(args.size() + 1);
	for(const auto &arg : args)
		argv.push_back(arg.c_str());
	argv.push_back(nullptr);
	return argv;
}

// #endregion mock data



// #region unit tests

// Test PrintData::IsPrintDataArgument with various argument combinations
SCENARIO( "PrintData::IsPrintDataArgument validates command line arguments", "[PrintData][IsPrintDataArgument]" ) {
	GIVEN( "Various command line argument combinations" ) {
		WHEN( "checking outfit-related arguments" ) {
			THEN( "returns true for valid outfit arguments" ) {
				CHECK( PrintData::IsPrintDataArgument(MakeArgv({"program", "-w"}).data()) );
				CHECK( PrintData::IsPrintDataArgument(MakeArgv({"program", "--weapons"}).data()) );
				CHECK( PrintData::IsPrintDataArgument(MakeArgv({"program", "-e"}).data()) );
				CHECK( PrintData::IsPrintDataArgument(MakeArgv({"program", "--engines"}).data()) );
				CHECK( PrintData::IsPrintDataArgument(MakeArgv({"program", "--power"}).data()) );
				CHECK( PrintData::IsPrintDataArgument(MakeArgv({"program", "-o"}).data()) );
				CHECK( PrintData::IsPrintDataArgument(MakeArgv({"program", "--outfits"}).data()) );
			}
		}

		WHEN( "checking ship-related arguments" ) {
			THEN( "returns true for valid ship arguments" ) {
				CHECK( PrintData::IsPrintDataArgument(MakeArgv({"program", "-s"}).data()) );
				CHECK( PrintData::IsPrintDataArgument(MakeArgv({"program", "--ships"}).data()) );
			}
		}

		WHEN( "checking other valid arguments" ) {
			THEN( "returns true for sales, planets, systems, matches, and changes" ) {
				CHECK( PrintData::IsPrintDataArgument(MakeArgv({"program", "--sales"}).data()) );
				CHECK( PrintData::IsPrintDataArgument(MakeArgv({"program", "--planets"}).data()) );
				CHECK( PrintData::IsPrintDataArgument(MakeArgv({"program", "--systems"}).data()) );
				CHECK( PrintData::IsPrintDataArgument(MakeArgv({"program", "--matches"}).data()) );
				CHECK( PrintData::IsPrintDataArgument(MakeArgv({"program", "--changes"}).data()) );
			}
		}

		WHEN( "checking invalid arguments" ) {
			THEN( "returns false for unrecognized arguments" ) {
				CHECK_FALSE( PrintData::IsPrintDataArgument(MakeArgv({"program", "--invalid"}).data()) );
				CHECK_FALSE( PrintData::IsPrintDataArgument(MakeArgv({"program", "-x"}).data()) );
				CHECK_FALSE( PrintData::IsPrintDataArgument(MakeArgv({"program", "unknown"}).data()) );
			}
		}

		WHEN( "checking with no arguments after program name" ) {
			THEN( "returns false" ) {
				CHECK_FALSE( PrintData::IsPrintDataArgument(MakeArgv({"program"}).data()) );
			}
		}

		WHEN( "checking with empty argv" ) {
			THEN( "returns false" ) {
				CHECK_FALSE( PrintData::IsPrintDataArgument(MakeArgv({}).data()) );
			}
		}

		WHEN( "checking valid argument among multiple arguments" ) {
			THEN( "returns true if any argument is valid" ) {
				CHECK( PrintData::IsPrintDataArgument(MakeArgv({"program", "--unknown", "--ships"}).data()) );
				CHECK( PrintData::IsPrintDataArgument(MakeArgv({"program", "--weapons", "--invalid"}).data()) );
				CHECK( PrintData::IsPrintDataArgument(MakeArgv({"program", "arg1", "arg2", "--planets"}).data()) );
			}
		}

		WHEN( "checking multiple invalid arguments" ) {
			THEN( "returns false" ) {
				CHECK_FALSE( PrintData::IsPrintDataArgument(MakeArgv({"program", "--unknown", "--other"}).data()) );
				CHECK_FALSE( PrintData::IsPrintDataArgument(MakeArgv({"program", "arg1", "arg2", "arg3"}).data()) );
			}
		}
	}
}

// Test PrintData::Help output
SCENARIO( "PrintData::Help prints help text to stderr", "[PrintData][Help]" ) {
	GIVEN( "A captured stderr stream" ) {
		OutputSink sink(std::cerr);

		WHEN( "calling Help" ) {
			PrintData::Help();
			std::string output = sink.Flush();

			THEN( "output contains descriptions for all main options" ) {
				CHECK( output.find("--ships") != std::string::npos );
				CHECK( output.find("--weapons") != std::string::npos );
				CHECK( output.find("--engines") != std::string::npos );
				CHECK( output.find("--power") != std::string::npos );
				CHECK( output.find("--outfits") != std::string::npos );
				CHECK( output.find("--sales") != std::string::npos );
				CHECK( output.find("--planets") != std::string::npos );
				CHECK( output.find("--systems") != std::string::npos );
				CHECK( output.find("--matches") != std::string::npos );
				CHECK( output.find("--changes") != std::string::npos );
			}

			THEN( "output contains descriptions for modifier options" ) {
				CHECK( output.find("--loaded") != std::string::npos );
				CHECK( output.find("--list") != std::string::npos );
				CHECK( output.find("--variants") != std::string::npos );
				CHECK( output.find("--descriptions") != std::string::npos );
				CHECK( output.find("--attributes") != std::string::npos );
				CHECK( output.find("--reverse") != std::string::npos );
			}

			THEN( "output contains short option forms" ) {
				CHECK( output.find("-s") != std::string::npos );
				CHECK( output.find("-w") != std::string::npos );
				CHECK( output.find("-e") != std::string::npos );
				CHECK( output.find("-o") != std::string::npos );
			}

			THEN( "output is non-empty" ) {
				CHECK_FALSE( output.empty() );
			}
		}
	}
}

// Test boundary conditions
SCENARIO( "PrintData::IsPrintDataArgument handles boundary conditions", "[PrintData][Boundary]" ) {
	GIVEN( "Various edge cases" ) {
		WHEN( "checking arguments that are substrings of valid arguments" ) {
			THEN( "returns false for partial matches" ) {
				CHECK_FALSE( PrintData::IsPrintDataArgument(MakeArgv({"program", "--ship"}).data()) );
				CHECK_FALSE( PrintData::IsPrintDataArgument(MakeArgv({"program", "--weapon"}).data()) );
				CHECK_FALSE( PrintData::IsPrintDataArgument(MakeArgv({"program", "--engine"}).data()) );
				CHECK_FALSE( PrintData::IsPrintDataArgument(MakeArgv({"program", "--planet"}).data()) );
				CHECK_FALSE( PrintData::IsPrintDataArgument(MakeArgv({"program", "--system"}).data()) );
				CHECK_FALSE( PrintData::IsPrintDataArgument(MakeArgv({"program", "--match"}).data()) );
				CHECK_FALSE( PrintData::IsPrintDataArgument(MakeArgv({"program", "--change"}).data()) );
			}
		}

		WHEN( "checking arguments with extra characters" ) {
			THEN( "returns false for arguments with extra suffixes" ) {
				CHECK_FALSE( PrintData::IsPrintDataArgument(MakeArgv({"program", "--shipss"}).data()) );
				CHECK_FALSE( PrintData::IsPrintDataArgument(MakeArgv({"program", "--weaponss"}).data()) );
				CHECK_FALSE( PrintData::IsPrintDataArgument(MakeArgv({"program", "--outfitss"}).data()) );
			}
		}

		WHEN( "checking case sensitivity" ) {
			THEN( "returns false for uppercase variants" ) {
				CHECK_FALSE( PrintData::IsPrintDataArgument(MakeArgv({"program", "--SHIPS"}).data()) );
				CHECK_FALSE( PrintData::IsPrintDataArgument(MakeArgv({"program", "--Ships"}).data()) );
				CHECK_FALSE( PrintData::IsPrintDataArgument(MakeArgv({"program", "-S"}).data()) );
				CHECK_FALSE( PrintData::IsPrintDataArgument(MakeArgv({"program", "-W"}).data()) );
			}
		}

		WHEN( "checking arguments with leading/trailing spaces simulation" ) {
			THEN( "returns false for arguments with spaces" ) {
				CHECK_FALSE( PrintData::IsPrintDataArgument(MakeArgv({"program", " --ships"}).data()) );
				CHECK_FALSE( PrintData::IsPrintDataArgument(MakeArgv({"program", "--ships "}).data()) );
			}
		}

		WHEN( "checking mixed valid and invalid arguments" ) {
			THEN( "returns true when at least one valid argument exists" ) {
				CHECK( PrintData::IsPrintDataArgument(MakeArgv({"program", "", "--ships"}).data()) );
				CHECK( PrintData::IsPrintDataArgument(MakeArgv({"program", "--ships", ""}).data()) );
			}
		}
	}
}

// Test that Help can be called multiple times without issues
SCENARIO( "PrintData::Help is idempotent", "[PrintData][Help]" ) {
	GIVEN( "A captured stderr stream" ) {
		OutputSink sink(std::cerr);

		WHEN( "calling Help multiple times" ) {
			PrintData::Help();
			std::string firstOutput = sink.Flush();

			PrintData::Help();
			std::string secondOutput = sink.Flush();

			THEN( "both calls produce the same output" ) {
				CHECK( firstOutput == secondOutput );
				CHECK_FALSE( firstOutput.empty() );
			}
		}
	}
}

// Test Help output format
SCENARIO( "PrintData::Help output format", "[PrintData][Help][Format]" ) {
	GIVEN( "A captured stderr stream" ) {
		OutputSink sink(std::cerr);

		WHEN( "calling Help" ) {
			PrintData::Help();
			std::string output = sink.Flush();

			THEN( "output contains proper formatting with spaces for indentation" ) {
				CHECK( output.find("    ") != std::string::npos );
			}

			THEN( "output describes ship options with their modifiers" ) {
				// Ships has modifiers --sales, --loaded, --list, --variants
				CHECK( output.find("ship stats") != std::string::npos );
				CHECK( output.find("loaded") != std::string::npos );
				CHECK( output.find("variant") != std::string::npos );
			}

			THEN( "output describes outfit options with categories" ) {
				CHECK( output.find("weapon stats") != std::string::npos );
				CHECK( output.find("engine stats") != std::string::npos );
				CHECK( output.find("power outfit stats") != std::string::npos );
			}

			THEN( "output describes planet and system options" ) {
				CHECK( output.find("planet") != std::string::npos );
				CHECK( output.find("system") != std::string::npos );
			}
		}
	}
}

// #endregion unit tests



} // test namespace