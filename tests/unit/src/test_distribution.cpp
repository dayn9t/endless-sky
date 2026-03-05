/* test_distribution.cpp
Copyright (c) 2025 by jiang

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
#include "../../../source/Distribution.h"

// ... and any system includes needed for the test file.
#include "../../../source/Angle.h"
#include "../../../source/Random.h"

#include <cmath>
#include <utility>

namespace { // test namespace

// #region mock data

// Helper function to check if an angle is within expected bounds
// Returns true if the angle's absolute degrees is within the expected range
bool IsAngleWithinBounds(const Angle &angle, double minDegrees, double maxDegrees)
{
	double absDeg = std::abs(angle.Degrees());
	return absDeg >= minDegrees && absDeg <= maxDegrees;
}

// #endregion mock data



// #region unit tests

SCENARIO( "Distribution::Type enum values", "[Distribution][Type]" )
{
	GIVEN( "The Distribution::Type enum" )
	{
		WHEN( "checking enum values" )
		{
			THEN( "the enum values are defined correctly" )
			{
				CHECK( static_cast<int>(Distribution::Type::Narrow) == 0 );
				CHECK( static_cast<int>(Distribution::Type::Medium) == 1 );
				CHECK( static_cast<int>(Distribution::Type::Wide) == 2 );
				CHECK( static_cast<int>(Distribution::Type::Uniform) == 3 );
				CHECK( static_cast<int>(Distribution::Type::Triangular) == 4 );
			}
		}
	}
}

SCENARIO( "Distribution::GenerateInaccuracy with zero value", "[Distribution][GenerateInaccuracy]" )
{
	GIVEN( "A zero inaccuracy value" )
	{
		double value = 0.0;

		WHEN( "generating inaccuracy with any distribution type" )
		{
			THEN( "the result is a zero angle for Uniform type" )
			{
				auto result = Distribution::GenerateInaccuracy(value, {Distribution::Type::Uniform, false});
				CHECK_THAT( result.Degrees(), Catch::Matchers::WithinAbs(0.0, 0.0001) );
			}
			THEN( "the result is a zero angle for Narrow type" )
			{
				auto result = Distribution::GenerateInaccuracy(value, {Distribution::Type::Narrow, false});
				CHECK_THAT( result.Degrees(), Catch::Matchers::WithinAbs(0.0, 0.0001) );
			}
			THEN( "the result is a zero angle for Medium type" )
			{
				auto result = Distribution::GenerateInaccuracy(value, {Distribution::Type::Medium, false});
				CHECK_THAT( result.Degrees(), Catch::Matchers::WithinAbs(0.0, 0.0001) );
			}
			THEN( "the result is a zero angle for Wide type" )
			{
				auto result = Distribution::GenerateInaccuracy(value, {Distribution::Type::Wide, false});
				CHECK_THAT( result.Degrees(), Catch::Matchers::WithinAbs(0.0, 0.0001) );
			}
			THEN( "the result is a zero angle for Triangular type" )
			{
				auto result = Distribution::GenerateInaccuracy(value, {Distribution::Type::Triangular, false});
				CHECK_THAT( result.Degrees(), Catch::Matchers::WithinAbs(0.0, 0.0001) );
			}
		}
	}
}

SCENARIO( "Distribution::GenerateInaccuracy with Uniform distribution", "[Distribution][GenerateInaccuracy][Uniform]" )
{
	GIVEN( "A non-zero inaccuracy value and Uniform distribution" )
	{
		double value = 10.0;
		auto dist = std::make_pair(Distribution::Type::Uniform, false);

		WHEN( "generating inaccuracy multiple times" )
		{
			THEN( "all results are within [-value, value] degrees" )
			{
				for(int i = 0; i < 100; ++i)
				{
					auto result = Distribution::GenerateInaccuracy(value, dist);
					CHECK_THAT( result.Degrees(), Catch::Matchers::WithinAbs(0.0, value) );
				}
			}
		}

		WHEN( "using inverted flag" )
		{
			auto distInverted = std::make_pair(Distribution::Type::Uniform, true);
			THEN( "all results are still within [-value, value] degrees" )
			{
				for(int i = 0; i < 100; ++i)
				{
					auto result = Distribution::GenerateInaccuracy(value, distInverted);
					CHECK_THAT( result.Degrees(), Catch::Matchers::WithinAbs(0.0, value) );
				}
			}
		}
	}
}

SCENARIO( "Distribution::GenerateInaccuracy with Triangular distribution", "[Distribution][GenerateInaccuracy][Triangular]" )
{
	GIVEN( "A non-zero inaccuracy value and Triangular distribution" )
	{
		double value = 15.0;
		auto dist = std::make_pair(Distribution::Type::Triangular, false);

		WHEN( "generating inaccuracy multiple times" )
		{
			THEN( "all results are within [-value, value] degrees" )
			{
				for(int i = 0; i < 100; ++i)
				{
					auto result = Distribution::GenerateInaccuracy(value, dist);
					CHECK_THAT( result.Degrees(), Catch::Matchers::WithinAbs(0.0, value) );
				}
			}
		}

		WHEN( "using inverted flag" )
		{
			auto distInverted = std::make_pair(Distribution::Type::Triangular, true);
			THEN( "all results are still within [-value, value] degrees" )
			{
				for(int i = 0; i < 100; ++i)
				{
					auto result = Distribution::GenerateInaccuracy(value, distInverted);
					CHECK_THAT( result.Degrees(), Catch::Matchers::WithinAbs(0.0, value) );
				}
			}
		}
	}
}

SCENARIO( "Distribution::GenerateInaccuracy with Narrow distribution", "[Distribution][GenerateInaccuracy][Narrow]" )
{
	GIVEN( "A non-zero inaccuracy value and Narrow distribution" )
	{
		double value = 5.0;
		auto dist = std::make_pair(Distribution::Type::Narrow, false);

		WHEN( "generating inaccuracy multiple times" )
		{
			THEN( "all results are within reasonable bounds" )
			{
				for(int i = 0; i < 100; ++i)
				{
					auto result = Distribution::GenerateInaccuracy(value, dist);
					// The result should be within [-value, value] with some tolerance
					CHECK_THAT( result.Degrees(), Catch::Matchers::WithinAbs(0.0, value * 2.0) );
				}
			}
		}

		WHEN( "using inverted flag" )
		{
			auto distInverted = std::make_pair(Distribution::Type::Narrow, true);
			THEN( "all results are within reasonable bounds" )
			{
				for(int i = 0; i < 100; ++i)
				{
					auto result = Distribution::GenerateInaccuracy(value, distInverted);
					CHECK_THAT( result.Degrees(), Catch::Matchers::WithinAbs(0.0, value * 2.0) );
				}
			}
		}
	}
}

SCENARIO( "Distribution::GenerateInaccuracy with Medium distribution", "[Distribution][GenerateInaccuracy][Medium]" )
{
	GIVEN( "A non-zero inaccuracy value and Medium distribution" )
	{
		double value = 8.0;
		auto dist = std::make_pair(Distribution::Type::Medium, false);

		WHEN( "generating inaccuracy multiple times" )
		{
			THEN( "all results are within reasonable bounds" )
			{
				for(int i = 0; i < 100; ++i)
				{
					auto result = Distribution::GenerateInaccuracy(value, dist);
					CHECK_THAT( result.Degrees(), Catch::Matchers::WithinAbs(0.0, value * 2.0) );
				}
			}
		}

		WHEN( "using inverted flag" )
		{
			auto distInverted = std::make_pair(Distribution::Type::Medium, true);
			THEN( "all results are within reasonable bounds" )
			{
				for(int i = 0; i < 100; ++i)
				{
					auto result = Distribution::GenerateInaccuracy(value, distInverted);
					CHECK_THAT( result.Degrees(), Catch::Matchers::WithinAbs(0.0, value * 2.0) );
				}
			}
		}
	}
}

SCENARIO( "Distribution::GenerateInaccuracy with Wide distribution", "[Distribution][GenerateInaccuracy][Wide]" )
{
	GIVEN( "A non-zero inaccuracy value and Wide distribution" )
	{
		double value = 12.0;
		auto dist = std::make_pair(Distribution::Type::Wide, false);

		WHEN( "generating inaccuracy multiple times" )
		{
			THEN( "all results are within reasonable bounds" )
			{
				for(int i = 0; i < 100; ++i)
				{
					auto result = Distribution::GenerateInaccuracy(value, dist);
					CHECK_THAT( result.Degrees(), Catch::Matchers::WithinAbs(0.0, value * 2.0) );
				}
			}
		}

		WHEN( "using inverted flag" )
		{
			auto distInverted = std::make_pair(Distribution::Type::Wide, true);
			THEN( "all results are within reasonable bounds" )
			{
				for(int i = 0; i < 100; ++i)
				{
					auto result = Distribution::GenerateInaccuracy(value, distInverted);
					CHECK_THAT( result.Degrees(), Catch::Matchers::WithinAbs(0.0, value * 2.0) );
				}
			}
		}
	}
}

SCENARIO( "Distribution::GenerateInaccuracy with various inaccuracy values", "[Distribution][GenerateInaccuracy][Values]" )
{
	GIVEN( "Different inaccuracy values" )
	{
		auto dist = std::make_pair(Distribution::Type::Uniform, false);

		WHEN( "using a small inaccuracy value" )
		{
			double value = 1.0;
			THEN( "results are within the small range" )
			{
				for(int i = 0; i < 50; ++i)
				{
					auto result = Distribution::GenerateInaccuracy(value, dist);
					CHECK_THAT( result.Degrees(), Catch::Matchers::WithinAbs(0.0, value) );
				}
			}
		}

		WHEN( "using a large inaccuracy value" )
		{
			double value = 100.0;
			THEN( "results are within the large range" )
			{
				for(int i = 0; i < 50; ++i)
				{
					auto result = Distribution::GenerateInaccuracy(value, dist);
					CHECK_THAT( result.Degrees(), Catch::Matchers::WithinAbs(0.0, value) );
				}
			}
		}

		WHEN( "using a very large inaccuracy value" )
		{
			double value = 360.0;
			THEN( "results are still within bounds" )
			{
				for(int i = 0; i < 50; ++i)
				{
					auto result = Distribution::GenerateInaccuracy(value, dist);
					CHECK_THAT( result.Degrees(), Catch::Matchers::WithinAbs(0.0, value) );
				}
			}
		}
	}
}

SCENARIO( "Distribution::GenerateInaccuracy returns Angle type", "[Distribution][GenerateInaccuracy][ReturnType]" )
{
	GIVEN( "Any distribution parameters" )
	{
		double value = 10.0;
		auto dist = std::make_pair(Distribution::Type::Uniform, false);

		WHEN( "calling GenerateInaccuracy" )
		{
			auto result = Distribution::GenerateInaccuracy(value, dist);
			THEN( "the result is an Angle object with valid methods" )
			{
				// Verify the returned Angle has valid Degrees() method
				double degrees = result.Degrees();
				CHECK( std::isfinite(degrees) );

				// Verify the returned Angle has valid Unit() method
				Point unit = result.Unit();
				CHECK_THAT( unit.X(), Catch::Matchers::WithinAbs(0.0, 1.0) );
				CHECK_THAT( unit.Y(), Catch::Matchers::WithinAbs(0.0, 1.0) );

				// Verify the unit vector has length approximately 1
				double length = std::sqrt(unit.X() * unit.X() + unit.Y() * unit.Y());
				CHECK_THAT( length, Catch::Matchers::WithinAbs(1.0, 0.0001) );
			}
		}
	}
}

SCENARIO( "Distribution::GenerateInaccuracy distribution variability", "[Distribution][GenerateInaccuracy][Variability]" )
{
	GIVEN( "A distribution with non-zero value" )
	{
		double value = 45.0;
		auto dist = std::make_pair(Distribution::Type::Uniform, false);

		WHEN( "generating many inaccuracy values" )
		{
			THEN( "the results show variability (not all identical)" )
			{
				double first = Distribution::GenerateInaccuracy(value, dist).Degrees();
				bool hasVariation = false;

				for(int i = 0; i < 50; ++i)
				{
					double current = Distribution::GenerateInaccuracy(value, dist).Degrees();
					if(std::abs(current - first) > 0.001)
					{
						hasVariation = true;
						break;
					}
				}

				CHECK( hasVariation );
			}
		}
	}
}

SCENARIO( "Distribution::GenerateInaccuracy with negative inaccuracy value", "[Distribution][GenerateInaccuracy][Negative]" )
{
	GIVEN( "A negative inaccuracy value" )
	{
		double value = -10.0;

		WHEN( "generating inaccuracy with Uniform distribution" )
		{
			auto dist = std::make_pair(Distribution::Type::Uniform, false);
			THEN( "the function handles negative values (produces valid Angle)" )
			{
				for(int i = 0; i < 20; ++i)
				{
					auto result = Distribution::GenerateInaccuracy(value, dist);
					// Result should still be a valid Angle
					CHECK( std::isfinite(result.Degrees()) );
				}
			}
		}
	}
}

// #endregion unit tests



} // test namespace