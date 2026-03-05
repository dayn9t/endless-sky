/* test_date.cpp
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
#include "../../../source/Date.h"

// ... and any system includes needed for the test file.
#include <string>

namespace { // test namespace

// #region mock data

// #endregion mock data



// #region unit tests

SCENARIO( "Creating a Date", "[Date][Creation]" ) {
	GIVEN( "a default-constructed date" ) {
		Date date;
		WHEN( "no parameters are provided" ) {
			THEN( "the date is uninitialized" ) {
				REQUIRE( !date );
				REQUIRE( false == static_cast<bool>(date) );
			}
		}
	}

	GIVEN( "a date constructed with specific values" ) {
		WHEN( "creating date 15 March 2024" ) {
			Date date(15, 3, 2024);
			THEN( "the day, month, and year are correctly stored" ) {
				REQUIRE( date.Day() == 15 );
				REQUIRE( date.Month() == 3 );
				REQUIRE( date.Year() == 2024 );
			}
			AND_THEN( "the date is considered initialized" ) {
				REQUIRE( date );
				REQUIRE( !(!date) );
			}
		}
		WHEN( "creating date 1 January 1" ) {
			Date date(1, 1, 1);
			THEN( "the values are correctly stored" ) {
				REQUIRE( date.Day() == 1 );
				REQUIRE( date.Month() == 1 );
				REQUIRE( date.Year() == 1 );
			}
		}
		WHEN( "creating date 31 December 9999" ) {
			Date date(31, 12, 9999);
			THEN( "the values are correctly stored" ) {
				REQUIRE( date.Day() == 31 );
				REQUIRE( date.Month() == 12 );
				REQUIRE( date.Year() == 9999 );
			}
		}
	}
}

SCENARIO( "Getting date components", "[Date][Accessors]" ) {
	GIVEN( "a specific date" ) {
		Date date(25, 12, 2023);
		WHEN( "querying individual components" ) {
			THEN( "Day() returns the day of month" ) {
				REQUIRE( date.Day() == 25 );
			}
			THEN( "Month() returns the month (1-indexed)" ) {
				REQUIRE( date.Month() == 12 );
			}
			THEN( "Year() returns the year" ) {
				REQUIRE( date.Year() == 2023 );
			}
		}
	}
	GIVEN( "dates with boundary day values" ) {
		WHEN( "day is 1" ) {
			Date date(1, 6, 2024);
			REQUIRE( date.Day() == 1 );
		}
		WHEN( "day is 31" ) {
			Date date(31, 1, 2024);
			REQUIRE( date.Day() == 31 );
		}
		WHEN( "day is 28 (February non-leap)" ) {
			Date date(28, 2, 2023);
			REQUIRE( date.Day() == 28 );
		}
		WHEN( "day is 29 (February leap year)" ) {
			Date date(29, 2, 2024);
			REQUIRE( date.Day() == 29 );
		}
	}
	GIVEN( "dates with all months" ) {
		THEN( "all months 1-12 can be retrieved" ) {
			for( int m = 1; m <= 12; ++m ) {
				Date date(1, m, 2024);
				REQUIRE( date.Month() == m );
			}
		}
	}
}

SCENARIO( "Converting Date to string", "[Date][String]" ) {
	GIVEN( "an uninitialized date" ) {
		Date date;
		WHEN( "calling ToString()" ) {
			THEN( "returns an empty string" ) {
				REQUIRE( date.ToString().empty() );
			}
		}
		WHEN( "calling LongString()" ) {
			THEN( "returns an empty string" ) {
				REQUIRE( date.LongString().empty() );
			}
		}
	}
	GIVEN( "a valid date 1 January 2024" ) {
		Date date(1, 1, 2024);
		WHEN( "calling ToString()" ) {
			THEN( "returns a non-empty string" ) {
				REQUIRE_FALSE( date.ToString().empty() );
			}
		}
		WHEN( "calling LongString()" ) {
			std::string result = date.LongString();
			THEN( "returns a non-empty string" ) {
				REQUIRE_FALSE( result.empty() );
			}
			THEN( "contains the day with ordinal suffix" ) {
				REQUIRE( result.find("1st") != std::string::npos );
			}
			THEN( "contains the full month name" ) {
				REQUIRE( result.find("January") != std::string::npos );
			}
		}
	}
	GIVEN( "dates with different day endings" ) {
		WHEN( "day ends in 1 (except 11)" ) {
			Date date(1, 3, 2024);
			std::string result = date.LongString();
			REQUIRE( result.find("1st") != std::string::npos );
		}
		WHEN( "day ends in 2 (except 12)" ) {
			Date date(2, 3, 2024);
			std::string result = date.LongString();
			REQUIRE( result.find("2nd") != std::string::npos );
		}
		WHEN( "day ends in 3 (except 13)" ) {
			Date date(3, 3, 2024);
			std::string result = date.LongString();
			REQUIRE( result.find("3rd") != std::string::npos );
		}
		WHEN( "day is 11 (uses 'th')" ) {
			Date date(11, 3, 2024);
			std::string result = date.LongString();
			REQUIRE( result.find("11th") != std::string::npos );
		}
		WHEN( "day is 12 (uses 'th')" ) {
			Date date(12, 3, 2024);
			std::string result = date.LongString();
			REQUIRE( result.find("12th") != std::string::npos );
		}
		WHEN( "day is 13 (uses 'th')" ) {
			Date date(13, 3, 2024);
			std::string result = date.LongString();
			REQUIRE( result.find("13th") != std::string::npos );
		}
		WHEN( "day ends in 4" ) {
			Date date(4, 3, 2024);
			std::string result = date.LongString();
			REQUIRE( result.find("4th") != std::string::npos );
		}
		WHEN( "day ends in 0" ) {
			Date date(20, 3, 2024);
			std::string result = date.LongString();
			REQUIRE( result.find("20th") != std::string::npos );
		}
	}
	GIVEN( "dates in different months" ) {
		WHEN( "checking January" ) {
			Date date(15, 1, 2024);
			REQUIRE( date.LongString().find("January") != std::string::npos );
		}
		WHEN( "checking February" ) {
			Date date(15, 2, 2024);
			REQUIRE( date.LongString().find("February") != std::string::npos );
		}
		WHEN( "checking December" ) {
			Date date(15, 12, 2024);
			REQUIRE( date.LongString().find("December") != std::string::npos );
		}
	}
}

SCENARIO( "Incrementing dates", "[Date][Arithmetic]" ) {
	GIVEN( "an uninitialized date" ) {
		Date date;
		WHEN( "using prefix increment" ) {
			Date result = ++date;
			THEN( "the date remains uninitialized" ) {
				REQUIRE( !result );
				REQUIRE( !date );
			}
		}
		WHEN( "using postfix increment" ) {
			Date result = date++;
			THEN( "the date remains uninitialized" ) {
				REQUIRE( !result );
				REQUIRE( !date );
			}
		}
	}
	GIVEN( "a date within a month" ) {
		Date date(15, 6, 2024);
		WHEN( "using prefix increment" ) {
			Date result = ++date;
			THEN( "the date advances by one day" ) {
				REQUIRE( date.Day() == 16 );
				REQUIRE( result.Day() == 16 );
			}
		}
		WHEN( "using postfix increment" ) {
			Date original = date;
			Date result = date++;
			THEN( "returns the original value" ) {
				REQUIRE( result.Day() == 15 );
			}
			THEN( "the date advances by one day" ) {
				REQUIRE( date.Day() == 16 );
			}
		}
	}
	GIVEN( "a date at month boundary" ) {
		WHEN( "incrementing from January 31" ) {
			Date date(31, 1, 2024);
			++date;
			THEN( "rolls over to February 1" ) {
				REQUIRE( date.Day() == 1 );
				REQUIRE( date.Month() == 2 );
				REQUIRE( date.Year() == 2024 );
			}
		}
		WHEN( "incrementing from February 28 in non-leap year" ) {
			Date date(28, 2, 2023);
			++date;
			THEN( "rolls over to March 1" ) {
				REQUIRE( date.Day() == 1 );
				REQUIRE( date.Month() == 3 );
				REQUIRE( date.Year() == 2023 );
			}
		}
		WHEN( "incrementing from February 28 in leap year" ) {
			Date date(28, 2, 2024);
			++date;
			THEN( "advances to February 29" ) {
				REQUIRE( date.Day() == 29 );
				REQUIRE( date.Month() == 2 );
				REQUIRE( date.Year() == 2024 );
			}
		}
		WHEN( "incrementing from February 29 in leap year" ) {
			Date date(29, 2, 2024);
			++date;
			THEN( "rolls over to March 1" ) {
				REQUIRE( date.Day() == 1 );
				REQUIRE( date.Month() == 3 );
				REQUIRE( date.Year() == 2024 );
			}
		}
		WHEN( "incrementing from December 31" ) {
			Date date(31, 12, 2024);
			++date;
			THEN( "rolls over to January 1 of next year" ) {
				REQUIRE( date.Day() == 1 );
				REQUIRE( date.Month() == 1 );
				REQUIRE( date.Year() == 2025 );
			}
		}
	}
	GIVEN( "dates with different month lengths" ) {
		WHEN( "incrementing from April 30" ) {
			Date date(30, 4, 2024);
			++date;
			THEN( "rolls over to May 1" ) {
				REQUIRE( date.Day() == 1 );
				REQUIRE( date.Month() == 5 );
			}
		}
		WHEN( "incrementing from May 31" ) {
			Date date(31, 5, 2024);
			++date;
			THEN( "rolls over to June 1" ) {
				REQUIRE( date.Day() == 1 );
				REQUIRE( date.Month() == 6 );
			}
		}
	}
}

SCENARIO( "Adding days to dates", "[Date][Arithmetic]" ) {
	GIVEN( "an uninitialized date" ) {
		Date date;
		WHEN( "adding days" ) {
			Date result = date + 10;
			THEN( "the date remains uninitialized" ) {
				REQUIRE( !result );
			}
		}
	}
	GIVEN( "a date in the middle of a month" ) {
		Date date(15, 6, 2024);
		WHEN( "adding zero days" ) {
			Date result = date + 0;
			THEN( "returns the same date" ) {
				REQUIRE( result.Day() == 15 );
				REQUIRE( result.Month() == 6 );
				REQUIRE( result.Year() == 2024 );
			}
		}
		WHEN( "adding positive days within the same month" ) {
			Date result = date + 10;
			THEN( "advances the correct number of days" ) {
				REQUIRE( result.Day() == 25 );
				REQUIRE( result.Month() == 6 );
				REQUIRE( result.Year() == 2024 );
			}
		}
		WHEN( "adding days that cross month boundary" ) {
			Date result = date + 20;
			THEN( "correctly rolls over to next month" ) {
				REQUIRE( result.Day() == 5 );
				REQUIRE( result.Month() == 7 );
				REQUIRE( result.Year() == 2024 );
			}
		}
		WHEN( "adding days that cross year boundary" ) {
			Date date(25, 12, 2024);
			Date result = date + 10;
			THEN( "correctly rolls over to next year" ) {
				REQUIRE( result.Day() == 4 );
				REQUIRE( result.Month() == 1 );
				REQUIRE( result.Year() == 2025 );
			}
		}
	}
	GIVEN( "a date where adding days crosses leap year" ) {
		WHEN( "adding days across February in leap year" ) {
			Date date(28, 2, 2024);
			Date result = date + 2;
			THEN( "accounts for leap day" ) {
				REQUIRE( result.Day() == 1 );
				REQUIRE( result.Month() == 3 );
				REQUIRE( result.Year() == 2024 );
			}
		}
		WHEN( "adding days across February in non-leap year" ) {
			Date date(28, 2, 2023);
			Date result = date + 1;
			THEN( "no leap day" ) {
				REQUIRE( result.Day() == 1 );
				REQUIRE( result.Month() == 3 );
				REQUIRE( result.Year() == 2023 );
			}
		}
	}
	GIVEN( "a date where subtracting days (negative addition)" ) {
		WHEN( "subtracting days within the same month" ) {
			Date date(15, 6, 2024);
			Date result = date + (-5);
			THEN( "moves backward in time" ) {
				REQUIRE( result.Day() == 10 );
				REQUIRE( result.Month() == 6 );
				REQUIRE( result.Year() == 2024 );
			}
		}
		WHEN( "subtracting days that cross month boundary" ) {
			Date date(5, 6, 2024);
			Date result = date + (-10);
			THEN( "correctly rolls back to previous month" ) {
				REQUIRE( result.Day() == 26 );
				REQUIRE( result.Month() == 5 );
				REQUIRE( result.Year() == 2024 );
			}
		}
		WHEN( "subtracting days that cross year boundary" ) {
			Date date(5, 1, 2024);
			Date result = date + (-10);
			THEN( "correctly rolls back to previous year" ) {
				REQUIRE( result.Day() == 26 );
				REQUIRE( result.Month() == 12 );
				REQUIRE( result.Year() == 2023 );
			}
		}
		WHEN( "subtracting days across February in leap year" ) {
			Date date(5, 3, 2024);
			Date result = date + (-10);
			THEN( "accounts for leap day" ) {
				REQUIRE( result.Day() == 24 );
				REQUIRE( result.Month() == 2 );
				REQUIRE( result.Year() == 2024 );
			}
		}
		WHEN( "subtracting days across February in non-leap year" ) {
			Date date(5, 3, 2023);
			Date result = date + (-10);
			THEN( "no leap day" ) {
				REQUIRE( result.Day() == 23 );
				REQUIRE( result.Month() == 2 );
				REQUIRE( result.Year() == 2023 );
			}
		}
	}
}

SCENARIO( "Calculating days between dates", "[Date][Arithmetic]" ) {
	GIVEN( "two dates in the same year" ) {
		WHEN( "dates are consecutive" ) {
			Date date1(1, 1, 2024);
			Date date2(2, 1, 2024);
			THEN( "difference is 1" ) {
				REQUIRE( date2 - date1 == 1 );
				REQUIRE( date1 - date2 == -1 );
			}
		}
		WHEN( "same date" ) {
			Date date1(15, 6, 2024);
			Date date2(15, 6, 2024);
			THEN( "difference is 0" ) {
				REQUIRE( date1 - date2 == 0 );
			}
		}
		WHEN( "dates span multiple months" ) {
			Date date1(1, 1, 2024);
			Date date2(1, 2, 2024);
			THEN( "correctly calculates difference" ) {
				REQUIRE( date2 - date1 == 31 );
			}
		}
	}
	GIVEN( "dates in different years" ) {
		WHEN( "across a year boundary" ) {
			Date date1(31, 12, 2023);
			Date date2(1, 1, 2024);
			THEN( "difference is 1" ) {
				REQUIRE( date2 - date1 == 1 );
			}
		}
		WHEN( "across multiple years" ) {
			Date date1(1, 1, 2020);
			Date date2(1, 1, 2024);
			THEN( "correctly accounts for leap years" ) {
				// 2020-2023: 366 + 365 + 365 + 365 = 1461 days
				REQUIRE( date2 - date1 == 1461 );
			}
		}
	}
	GIVEN( "dates involving leap years" ) {
		WHEN( "across February 29 in leap year" ) {
			Date date1(28, 2, 2024);
			Date date2(1, 3, 2024);
			THEN( "accounts for leap day" ) {
				REQUIRE( date2 - date1 == 2 );
			}
		}
		WHEN( "across February in non-leap year" ) {
			Date date1(28, 2, 2023);
			Date date2(1, 3, 2023);
			THEN( "no leap day" ) {
				REQUIRE( date2 - date1 == 1 );
			}
		}
	}
}

SCENARIO( "Comparing dates", "[Date][Comparison]" ) {
	GIVEN( "identical dates" ) {
		Date date1(15, 6, 2024);
		Date date2(15, 6, 2024);
		WHEN( "comparing equality" ) {
			THEN( "dates are equal" ) {
				REQUIRE( date1 == date2 );
				REQUIRE_FALSE( date1 != date2 );
			}
		}
		WHEN( "comparing order" ) {
			THEN( "date is not less than itself" ) {
				REQUIRE( date1 <= date2 );
				REQUIRE( date1 >= date2 );
				REQUIRE_FALSE( date1 < date2 );
				REQUIRE_FALSE( date1 > date2 );
			}
		}
	}
	GIVEN( "different dates" ) {
		Date earlier(1, 1, 2024);
		Date later(2, 1, 2024);
		WHEN( "comparing different dates" ) {
			THEN( "earlier is less than later" ) {
				REQUIRE( earlier < later );
				REQUIRE( earlier <= later );
				REQUIRE_FALSE( earlier > later );
				REQUIRE_FALSE( earlier >= later );
				REQUIRE( earlier != later );
				REQUIRE_FALSE( earlier == later );
			}
		}
	}
	GIVEN( "dates in different months" ) {
		Date jan(31, 1, 2024);
		Date feb(1, 2, 2024);
		WHEN( "comparing" ) {
			THEN( "January is earlier than February" ) {
				REQUIRE( jan < feb );
			}
		}
	}
	GIVEN( "dates in different years" ) {
		Date year2023(31, 12, 2023);
		Date year2024(1, 1, 2024);
		WHEN( "comparing" ) {
			THEN( "2023 is earlier than 2024" ) {
				REQUIRE( year2023 < year2024 );
			}
		}
	}
	GIVEN( "uninitialized dates" ) {
		Date uninit1;
		Date uninit2;
		Date init(1, 1, 2024);
		WHEN( "comparing uninitialized dates" ) {
			THEN( "uninitialized dates are equal" ) {
				REQUIRE( uninit1 == uninit2 );
			}
		}
		WHEN( "comparing uninitialized with initialized" ) {
			THEN( "uninitialized is less than any initialized date" ) {
				REQUIRE( uninit1 < init );
				REQUIRE_FALSE( init < uninit1 );
			}
		}
	}
}

SCENARIO( "Calculating days since epoch", "[Date][Epoch]" ) {
	GIVEN( "an uninitialized date" ) {
		Date date;
		WHEN( "calling DaysSinceEpoch()" ) {
			THEN( "returns 0" ) {
				REQUIRE( date.DaysSinceEpoch() == 0 );
			}
		}
	}
	GIVEN( "date January 1, year 1" ) {
		Date date(1, 1, 1);
		WHEN( "calling DaysSinceEpoch()" ) {
			THEN( "returns 1 (first day)" ) {
				REQUIRE( date.DaysSinceEpoch() == 1 );
			}
		}
	}
	GIVEN( "dates in the same year" ) {
		WHEN( "comparing DaysSinceEpoch" ) {
			Date jan1(1, 1, 2024);
			Date jan2(2, 1, 2024);
			THEN( "difference matches DaysSinceEpoch difference" ) {
				REQUIRE( jan2.DaysSinceEpoch() - jan1.DaysSinceEpoch() == 1 );
			}
		}
	}
	GIVEN( "a leap year date" ) {
		WHEN( "date is after February 29, 2024" ) {
			Date mar1(1, 3, 2024);
			Date feb28(28, 2, 2024);
			THEN( "leap day is accounted for" ) {
				REQUIRE( mar1.DaysSinceEpoch() - feb28.DaysSinceEpoch() == 2 );
			}
		}
		WHEN( "date is February 29, 2024" ) {
			Date feb29(29, 2, 2024);
			Date feb28(28, 2, 2024);
			THEN( "February 29 exists in leap year" ) {
				REQUIRE( feb29.DaysSinceEpoch() - feb28.DaysSinceEpoch() == 1 );
			}
		}
	}
	GIVEN( "a non-leap year date" ) {
		WHEN( "date is after February 28, 2023" ) {
			Date mar1(1, 3, 2023);
			Date feb28(28, 2, 2023);
			THEN( "no leap day" ) {
				REQUIRE( mar1.DaysSinceEpoch() - feb28.DaysSinceEpoch() == 1 );
			}
		}
	}
}

SCENARIO( "Calculating days since year start", "[Date][YearCalculations]" ) {
	GIVEN( "January 1 of any year" ) {
		Date date(1, 1, 2024);
		WHEN( "calling DaysSinceYearStart()" ) {
			THEN( "returns 1 (first day of year)" ) {
				REQUIRE( date.DaysSinceYearStart() == 1 );
			}
		}
	}
	GIVEN( "December 31 in non-leap year" ) {
		Date date(31, 12, 2023);
		WHEN( "calling DaysSinceYearStart()" ) {
			THEN( "returns 365" ) {
				REQUIRE( date.DaysSinceYearStart() == 365 );
			}
		}
	}
	GIVEN( "December 31 in leap year" ) {
		Date date(31, 12, 2024);
		WHEN( "calling DaysSinceYearStart()" ) {
			THEN( "returns 366" ) {
				REQUIRE( date.DaysSinceYearStart() == 366 );
			}
		}
	}
	GIVEN( "dates after February in leap year" ) {
		WHEN( "date is March 1, 2024" ) {
			Date date(1, 3, 2024);
			THEN( "includes the leap day" ) {
				// Jan: 31 + Feb: 29 + Mar 1 = 61
				REQUIRE( date.DaysSinceYearStart() == 61 );
			}
		}
		WHEN( "date is March 1, 2023 (non-leap)" ) {
			Date date(1, 3, 2023);
			THEN( "does not include a leap day" ) {
				// Jan: 31 + Feb: 28 + Mar 1 = 60
				REQUIRE( date.DaysSinceYearStart() == 60 );
			}
		}
	}
	GIVEN( "date before March (no leap day effect yet)" ) {
		WHEN( "date is February 28, 2024" ) {
			Date date(28, 2, 2024);
			THEN( "leap day not yet counted" ) {
				// Jan: 31 + Feb 28 = 59
				REQUIRE( date.DaysSinceYearStart() == 59 );
			}
		}
	}
}

SCENARIO( "Calculating days until year end", "[Date][YearCalculations]" ) {
	GIVEN( "January 1 in non-leap year" ) {
		Date date(1, 1, 2023);
		WHEN( "calling DaysUntilYearEnd()" ) {
			THEN( "returns 364 (365 - 1)" ) {
				REQUIRE( date.DaysUntilYearEnd() == 364 );
			}
		}
	}
	GIVEN( "January 1 in leap year" ) {
		Date date(1, 1, 2024);
		WHEN( "calling DaysUntilYearEnd()" ) {
			THEN( "returns 365 (366 - 1)" ) {
				REQUIRE( date.DaysUntilYearEnd() == 365 );
			}
		}
	}
	GIVEN( "December 31 in non-leap year" ) {
		Date date(31, 12, 2023);
		WHEN( "calling DaysUntilYearEnd()" ) {
			THEN( "returns 0 (last day of year)" ) {
				REQUIRE( date.DaysUntilYearEnd() == 0 );
			}
		}
	}
	GIVEN( "December 31 in leap year" ) {
		Date date(31, 12, 2024);
		WHEN( "calling DaysUntilYearEnd()" ) {
			THEN( "returns 0 (last day of year)" ) {
				REQUIRE( date.DaysUntilYearEnd() == 0 );
			}
		}
	}
	GIVEN( "a mid-year date" ) {
		WHEN( "date is July 1, 2024 (leap year)" ) {
			Date date(1, 7, 2024);
			THEN( "correctly calculates remaining days" ) {
				// 366 - (31 + 29 + 31 + 30 + 31 + 30 + 1) = 366 - 183 = 183
				REQUIRE( date.DaysUntilYearEnd() == 183 );
			}
		}
		WHEN( "date is July 1, 2023 (non-leap year)" ) {
			Date date(1, 7, 2023);
			THEN( "correctly calculates remaining days" ) {
				// 365 - (31 + 28 + 31 + 30 + 31 + 30 + 1) = 365 - 182 = 183
				REQUIRE( date.DaysUntilYearEnd() == 183 );
			}
		}
	}
}

SCENARIO( "Boolean conversion and operator!", "[Date][Boolean]" ) {
	GIVEN( "an uninitialized date" ) {
		Date date;
		WHEN( "checking boolean conversion" ) {
			THEN( "evaluates to false" ) {
				REQUIRE( !date );
				REQUIRE_FALSE( static_cast<bool>(date) );
			}
		}
	}
	GIVEN( "an initialized date" ) {
		Date date(1, 1, 2024);
		WHEN( "checking boolean conversion" ) {
			THEN( "evaluates to true" ) {
				REQUIRE( date );
				REQUIRE_FALSE( !date );
			}
		}
	}
	GIVEN( "various valid dates" ) {
		WHEN( "date is minimum valid (1/1/1)" ) {
			Date date(1, 1, 1);
			REQUIRE( date );
		}
		WHEN( "date is large year" ) {
			Date date(1, 1, 9999);
			REQUIRE( date );
		}
		WHEN( "date is any valid day/month" ) {
			Date date(15, 6, 2024);
			REQUIRE( date );
		}
	}
}

SCENARIO( "Date arithmetic consistency", "[Date][Arithmetic][Integration]" ) {
	GIVEN( "a sequence of date operations" ) {
		WHEN( "incrementing and then comparing difference" ) {
			Date date1(1, 1, 2024);
			Date date2 = date1;
			for( int i = 0; i < 100; ++i )
				++date2;
			THEN( "difference equals number of increments" ) {
				REQUIRE( date2 - date1 == 100 );
			}
		}
		WHEN( "adding days and then subtracting same amount" ) {
			Date original(15, 6, 2024);
			Date modified = original + 50;
			Date restored = modified + (-50);
			THEN( "returns to original date" ) {
				REQUIRE( original == restored );
			}
		}
		WHEN( "comparing DaysSinceEpoch with subtraction" ) {
			Date date1(1, 1, 2024);
			Date date2(31, 12, 2024);
			THEN( "difference matches DaysSinceEpoch difference" ) {
				REQUIRE( date2 - date1 == date2.DaysSinceEpoch() - date1.DaysSinceEpoch() );
			}
		}
	}
	GIVEN( "year transitions" ) {
		WHEN( "crossing century boundary (non-leap century)" ) {
			Date date1(1, 1, 1900);
			Date date2(1, 1, 1901);
			THEN( "year 1900 is not a leap year (365 days)" ) {
				REQUIRE( date2 - date1 == 365 );
			}
		}
		WHEN( "crossing century boundary (leap century)" ) {
			Date date1(1, 1, 2000);
			Date date2(1, 1, 2001);
			THEN( "year 2000 is a leap year (366 days)" ) {
				REQUIRE( date2 - date1 == 366 );
			}
		}
	}
}

// #endregion unit tests



} // test namespace