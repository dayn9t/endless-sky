/* test_logger.cpp
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
#include "../../../source/Logger.h"

// Include test helpers.
#include "output-capture.hpp"
#include "logger-output.h"

// ... and any system includes needed for the test file.
#include <functional>
#include <string>

namespace { // test namespace

// #region mock data

// Insert file-local data here, e.g. classes, structs, or fixtures that will be useful
// to help test this class/method.

// Helper struct to capture callback data
struct CallbackCapture {
	std::string lastMessage;
	Logger::Level lastLevel = Logger::Level::INFO;
	int callCount = 0;

	void reset() {
		lastMessage.clear();
		lastLevel = Logger::Level::INFO;
		callCount = 0;
	}

	void operator()(const std::string &message, Logger::Level level) {
		lastMessage = message;
		lastLevel = level;
		++callCount;
	}
};

// #endregion mock data



// #region unit tests

// Test Logger::Level enum values
SCENARIO( "Logger::Level enum has correct values", "[Logger][Level]" ) {
	GIVEN( "The Logger::Level enum" ) {
		WHEN( "checking enum values" ) {
			THEN( "INFO has character value 'I'" ) {
				CHECK( static_cast<char>(Logger::Level::INFO) == 'I' );
			}
			THEN( "WARNING has character value 'W'" ) {
				CHECK( static_cast<char>(Logger::Level::WARNING) == 'W' );
			}
			THEN( "ERROR has character value 'E'" ) {
				CHECK( static_cast<char>(Logger::Level::ERROR) == 'E' );
			}
		}
	}
}

// Test Logger::Session class
SCENARIO( "Logger::Session construction and destruction", "[Logger][Session]" ) {
	GIVEN( "Output capture for stdout" ) {
		OutputSink out(std::cout);

		WHEN( "creating a non-quiet session" ) {
			{
				Logger::Session session(false);
			}
			THEN( "session begin and end messages are logged" ) {
				std::string output = out.Flush();
				CHECK( output.find("Logger session beginning") != std::string::npos );
				CHECK( output.find("Logger session end") != std::string::npos );
			}
		}
	}
}

SCENARIO( "Logger::Session with quiet flag", "[Logger][Session]" ) {
	GIVEN( "Output capture for stdout" ) {
		OutputSink out(std::cout);

		WHEN( "creating a quiet session" ) {
			{
				Logger::Session session(true);
			}
			THEN( "no session messages are logged" ) {
				std::string output = out.Flush();
				CHECK( output.find("Logger session") == std::string::npos );
			}
		}
	}
}

SCENARIO( "Logger::Session contains game version info", "[Logger][Session]" ) {
	GIVEN( "Output capture for stdout" ) {
		OutputSink out(std::cout);

		WHEN( "creating a non-quiet session" ) {
			{
				Logger::Session session(false);
			}
			THEN( "the session message contains game version" ) {
				std::string output = out.Flush();
				CHECK( output.find("Game version:") != std::string::npos );
			}
		}
	}
}

// Test Logger::Log method
SCENARIO( "Logger::Log with INFO level", "[Logger][Log]" ) {
	GIVEN( "Output capture for stdout and no callback" ) {
		OutputSink out(std::cout);
		Logger::SetLogCallback(nullptr);

		WHEN( "logging an INFO message" ) {
			Logger::Log("Test info message", Logger::Level::INFO);
			THEN( "message is written to stdout with INFO level marker" ) {
				std::string output = out.Flush();
				CHECK( output.find("Test info message") != std::string::npos );
				CHECK( output.find("| I |") != std::string::npos );
			}
		}
	}
}

SCENARIO( "Logger::Log with WARNING level", "[Logger][Log]" ) {
	GIVEN( "Output capture for stderr and no callback" ) {
		OutputSink err(std::cerr);
		Logger::SetLogCallback(nullptr);

		WHEN( "logging a WARNING message" ) {
			Logger::Log("Test warning message", Logger::Level::WARNING);
			THEN( "message is written to stderr with WARNING level marker" ) {
				std::string output = err.Flush();
				CHECK( output.find("Test warning message") != std::string::npos );
				CHECK( output.find("| W |") != std::string::npos );
			}
		}
	}
}

SCENARIO( "Logger::Log with ERROR level", "[Logger][Log]" ) {
	GIVEN( "Output capture for stderr and no callback" ) {
		OutputSink err(std::cerr);
		Logger::SetLogCallback(nullptr);

		WHEN( "logging an ERROR message" ) {
			Logger::Log("Test error message", Logger::Level::ERROR);
			THEN( "message is written to stderr with ERROR level marker" ) {
				std::string output = err.Flush();
				CHECK( output.find("Test error message") != std::string::npos );
				CHECK( output.find("| E |") != std::string::npos );
			}
		}
	}
}

SCENARIO( "Logger::Log includes timestamp", "[Logger][Log]" ) {
	GIVEN( "Output capture for stdout and no callback" ) {
		OutputSink out(std::cout);
		Logger::SetLogCallback(nullptr);

		WHEN( "logging a message" ) {
			Logger::Log("Timestamp test", Logger::Level::INFO);
			THEN( "output contains a timestamp in ISO format" ) {
				std::string output = out.Flush();
				// Timestamp format: YYYY-MM-DD HH:MM:SS.mmm
				CHECK( output.find("-") != std::string::npos );
				CHECK( output.find(":") != std::string::npos );
			}
		}
	}
}

// Test Logger::SetLogCallback
SCENARIO( "Logger::SetLogCallback receives log messages", "[Logger][Callback]" ) {
	GIVEN( "A callback capture and output sinks" ) {
		OutputSink out(std::cout);
		CallbackCapture capture;
		Logger::SetLogCallback(std::ref(capture));

		WHEN( "logging an INFO message" ) {
			Logger::Log("Callback test info", Logger::Level::INFO);
			THEN( "callback receives the formatted message and level" ) {
				CHECK( capture.callCount == 1 );
				CHECK( capture.lastMessage.find("Callback test info") != std::string::npos );
				CHECK( capture.lastLevel == Logger::Level::INFO );
			}
			AND_THEN( "callback message matches console output" ) {
				std::string consoleOutput = IgnoreLogHeaders(out.Flush());
				CHECK( capture.lastMessage.find("Callback test info") != std::string::npos );
			}
		}

		// Reset callback to nullptr after test
		Logger::SetLogCallback(nullptr);
	}
}

SCENARIO( "Logger callback receives different levels", "[Logger][Callback]" ) {
	GIVEN( "A callback capture and output sink for stderr" ) {
		OutputSink err(std::cerr);
		CallbackCapture capture;
		Logger::SetLogCallback(std::ref(capture));

		WHEN( "logging a WARNING message" ) {
			Logger::Log("Callback test warning", Logger::Level::WARNING);
			THEN( "callback receives WARNING level" ) {
				CHECK( capture.callCount == 1 );
				CHECK( capture.lastLevel == Logger::Level::WARNING );
			}
		}

		WHEN( "logging an ERROR message" ) {
			capture.reset();
			Logger::Log("Callback test error", Logger::Level::ERROR);
			THEN( "callback receives ERROR level" ) {
				CHECK( capture.callCount == 1 );
				CHECK( capture.lastLevel == Logger::Level::ERROR );
			}
		}

		// Reset callback to nullptr after test
		Logger::SetLogCallback(nullptr);
	}
}

SCENARIO( "Logger::SetLogCallback can be cleared", "[Logger][Callback]" ) {
	GIVEN( "A callback set and then cleared" ) {
		OutputSink out(std::cout);
		CallbackCapture capture;

		Logger::SetLogCallback(std::ref(capture));
		Logger::Log("First message", Logger::Level::INFO);
		REQUIRE( capture.callCount == 1 );

		WHEN( "callback is set to nullptr" ) {
			Logger::SetLogCallback(nullptr);
			Logger::Log("Second message", Logger::Level::INFO);
			THEN( "callback is no longer invoked" ) {
				CHECK( capture.callCount == 1 ); // still 1, not 2
			}
		}
	}
}

SCENARIO( "Logger callback can be replaced", "[Logger][Callback]" ) {
	GIVEN( "Two different callbacks" ) {
		OutputSink out(std::cout);
		CallbackCapture firstCallback;
		CallbackCapture secondCallback;

		Logger::SetLogCallback(std::ref(firstCallback));
		Logger::Log("First callback message", Logger::Level::INFO);

		WHEN( "replacing the callback" ) {
			Logger::SetLogCallback(std::ref(secondCallback));
			Logger::Log("Second callback message", Logger::Level::INFO);

			THEN( "only the second callback receives new messages" ) {
				CHECK( firstCallback.callCount == 1 );
				CHECK( secondCallback.callCount == 1 );
				CHECK( firstCallback.lastMessage.find("First callback message") != std::string::npos );
				CHECK( secondCallback.lastMessage.find("Second callback message") != std::string::npos );
			}
		}

		// Reset callback to nullptr after test
		Logger::SetLogCallback(nullptr);
	}
}

SCENARIO( "Logger handles multiple sequential messages", "[Logger][Log]" ) {
	GIVEN( "Output capture and no callback" ) {
		OutputSink out(std::cout);
		OutputSink err(std::cerr);
		Logger::SetLogCallback(nullptr);

		WHEN( "logging multiple messages at different levels" ) {
			Logger::Log("Info 1", Logger::Level::INFO);
			Logger::Log("Warning 1", Logger::Level::WARNING);
			Logger::Log("Error 1", Logger::Level::ERROR);
			Logger::Log("Info 2", Logger::Level::INFO);

			THEN( "all messages are logged correctly" ) {
				std::string stdoutOutput = out.Flush();
				std::string stderrOutput = err.Flush();

				CHECK( stdoutOutput.find("Info 1") != std::string::npos );
				CHECK( stdoutOutput.find("Info 2") != std::string::npos );
				CHECK( stderrOutput.find("Warning 1") != std::string::npos );
				CHECK( stderrOutput.find("Error 1") != std::string::npos );
			}
		}
	}
}

SCENARIO( "Logger handles empty messages", "[Logger][Log]" ) {
	GIVEN( "Output capture and no callback" ) {
		OutputSink out(std::cout);
		Logger::SetLogCallback(nullptr);

		WHEN( "logging an empty message" ) {
			Logger::Log("", Logger::Level::INFO);
			THEN( "an empty message is still logged with formatting" ) {
				std::string output = out.Flush();
				CHECK( output.find("| I |") != std::string::npos );
			}
		}
	}
}

// #endregion unit tests



} // test namespace