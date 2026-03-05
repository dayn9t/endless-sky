/* test_mortgage.cpp
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
#include "../../../source/Mortgage.h"

// Include helpers for creating DataNodes.
#include "datanode-factory.h"

// ... and any system includes needed for the test file.
#include <cmath>
#include <cstdint>
#include <string>

namespace { // test namespace

// #region mock data

// Helper function to calculate expected payment using the same formula as Mortgage.
double CalculatePayment(int64_t principal, double interest, int term)
{
	if(!term)
		return static_cast<double>(principal);
	if(!interest)
		return static_cast<double>(principal) / term;

	const double power = pow(1. + interest, term);
	return principal * interest * power / (power - 1.);
}

// #endregion mock data



// #region unit tests

TEST_CASE( "Mortgage Basics", "[Mortgage]" ) {
	using T = Mortgage;
	SECTION( "Class Traits" ) {
		CHECK_FALSE( std::is_trivial_v<T> );
		CHECK( std::is_standard_layout_v<T> );
		CHECK( std::is_nothrow_destructible_v<T> );
		// Has std::string member, not trivially destructible
		CHECK_FALSE( std::is_trivially_destructible_v<T> );
	}
	SECTION( "Construction Traits" ) {
		CHECK( std::is_default_constructible_v<T> );
		// Has std::string member with non-trivial constructor
		CHECK_FALSE( std::is_trivially_default_constructible_v<T> );
		// std::string has noexcept default constructor
		CHECK( std::is_nothrow_default_constructible_v<T> );
		CHECK( std::is_copy_constructible_v<T> );
		// std::string copy may throw
		CHECK_FALSE( std::is_trivially_copy_constructible_v<T> );
		CHECK_FALSE( std::is_nothrow_copy_constructible_v<T> );
		CHECK( std::is_move_constructible_v<T> );
		CHECK_FALSE( std::is_trivially_move_constructible_v<T> );
		// std::string has noexcept move constructor
		CHECK( std::is_nothrow_move_constructible_v<T> );
	}
	SECTION( "Copy Traits" ) {
		CHECK( std::is_copy_assignable_v<T> );
		// Has std::string member, not trivially copyable
		CHECK_FALSE( std::is_trivially_copyable_v<T> );
		CHECK_FALSE( std::is_trivially_copy_assignable_v<T> );
		CHECK_FALSE( std::is_nothrow_copy_assignable_v<T> );
	}
	SECTION( "Move Traits" ) {
		CHECK( std::is_move_assignable_v<T> );
		CHECK_FALSE( std::is_trivially_move_assignable_v<T> );
		// std::string has noexcept move assignment
		CHECK( std::is_nothrow_move_assignable_v<T> );
	}
}

SCENARIO( "Creating a default Mortgage", "[Mortgage][Creation]" ) {
	GIVEN( "a default-constructed mortgage" ) {
		Mortgage mortgage;
		THEN( "it has default property values" ) {
			CHECK( mortgage.Type() == "" );
			CHECK( mortgage.Principal() == 0 );
			// Default mortgage has empty interest string (no interest set)
			CHECK( mortgage.Interest() == "" );
			CHECK( mortgage.Term() == 365 );
			CHECK( mortgage.Payment() == 0 );
		}
	}
}

SCENARIO( "Creating a Mortgage with credit score", "[Mortgage][Creation]" ) {
	GIVEN( "a mortgage created with credit score" ) {
		WHEN( "created with type, principal, and credit score" ) {
			Mortgage mortgage("Mortgage", 100000, 400);
			THEN( "properties are set correctly" ) {
				CHECK( mortgage.Type() == "Mortgage" );
				CHECK( mortgage.Principal() == 100000 );
				CHECK( mortgage.Term() == 365 );
				// Interest rate = (600 - 400/2) * 0.00001 = (600 - 200) * 0.00001 = 0.004
				CHECK( mortgage.Interest() == "0.400%" );
			}
			THEN( "payment is calculated correctly" ) {
				double expectedPayment = CalculatePayment(100000, 0.004, 365);
				CHECK_THAT( mortgage.PrecisePayment(), Catch::Matchers::WithinAbs(expectedPayment, 0.01) );
			}
		}
		WHEN( "created with a fine type (credit score 0)" ) {
			Mortgage fine("Fine", 50000, 0);
			THEN( "fine has higher interest rate" ) {
				CHECK( fine.Type() == "Fine" );
				CHECK( fine.Principal() == 50000 );
				// Interest rate = (600 - 0/2) * 0.00001 = 0.006
				CHECK( fine.Interest() == "0.600%" );
			}
		}
		WHEN( "created with a high credit score" ) {
			Mortgage mortgage("Mortgage", 100000, 800);
			THEN( "interest rate is lower" ) {
				// Interest rate = (600 - 800/2) * 0.00001 = (600 - 400) * 0.00001 = 0.002
				CHECK( mortgage.Interest() == "0.200%" );
			}
		}
		WHEN( "created with a custom term" ) {
			Mortgage mortgage("Mortgage", 100000, 500, 730);
			THEN( "term is set correctly" ) {
				CHECK( mortgage.Term() == 730 );
			}
		}
	}
}

SCENARIO( "Creating a Mortgage with explicit interest rate", "[Mortgage][Creation]" ) {
	GIVEN( "a mortgage created with explicit interest" ) {
		WHEN( "created with type, principal, and interest rate" ) {
			Mortgage mortgage("Debt", 100000, 2.5, 365);
			THEN( "properties are set correctly" ) {
				CHECK( mortgage.Type() == "Debt" );
				CHECK( mortgage.Principal() == 100000 );
				CHECK( mortgage.Term() == 365 );
				// Interest = 2.5 * 0.01 = 0.025
				// Interest string = "0." + to_string(1000 * 2.5) + "%" = "0.2500%"
				CHECK( mortgage.Interest() == "0.2500%" );
			}
		}
		WHEN( "created with zero interest" ) {
			Mortgage mortgage("Mortgage", 100000, 0.0, 365);
			THEN( "payment is simple division" ) {
				// Integer division: 100000 / 365 = 273 (rounded down)
				CHECK( mortgage.Payment() == 273 );
			}
		}
	}
}

SCENARIO( "Creating a Mortgage from DataNode", "[Mortgage][Creation][Serialization]" ) {
	GIVEN( "a DataNode with mortgage data" ) {
		WHEN( "DataNode has all fields" ) {
			auto node = AsDataNode("mortgage Mortgage\n"
			                       "\tprincipal 50000\n"
			                       "\tinterest 0.003\n"
			                       "\tterm 200");
			Mortgage mortgage(node);
			THEN( "all properties are loaded correctly" ) {
				CHECK( mortgage.Type() == "Mortgage" );
				CHECK( mortgage.Principal() == 50000 );
				CHECK( mortgage.Interest() == "0.300%" );
				CHECK( mortgage.Term() == 200 );
			}
		}
		WHEN( "DataNode has no type" ) {
			auto node = AsDataNode("mortgage\n"
			                       "\tprincipal 50000\n"
			                       "\tinterest 0.003\n"
			                       "\tterm 200");
			Mortgage mortgage(node);
			THEN( "type defaults to Mortgage" ) {
				CHECK( mortgage.Type() == "Mortgage" );
			}
		}
		WHEN( "DataNode specifies Fine type" ) {
			auto node = AsDataNode("mortgage Fine\n"
			                       "\tprincipal 10000\n"
			                       "\tinterest 0.006\n"
			                       "\tterm 365");
			Mortgage mortgage(node);
			THEN( "type is Fine" ) {
				CHECK( mortgage.Type() == "Fine" );
			}
		}
		WHEN( "DataNode specifies Debt type" ) {
			auto node = AsDataNode("mortgage Debt\n"
			                       "\tprincipal 25000\n"
			                       "\tinterest 0.001\n"
			                       "\tterm 365");
			Mortgage mortgage(node);
			THEN( "type is Debt" ) {
				CHECK( mortgage.Type() == "Debt" );
			}
		}
	}
}

SCENARIO( "Making mortgage payments", "[Mortgage][Payment]" ) {
	GIVEN( "a mortgage with principal and interest" ) {
		Mortgage mortgage("Mortgage", 100000, 500, 365);
		int64_t initialPrincipal = mortgage.Principal();
		int initialTerm = mortgage.Term();
		double initialPayment = mortgage.PrecisePayment();

		WHEN( "a payment is made" ) {
			int64_t payment = mortgage.MakePayment();
			THEN( "payment amount is returned" ) {
				CHECK_THAT( static_cast<double>(payment), Catch::Matchers::WithinAbs(initialPayment, 1.0) );
			}
			THEN( "principal is reduced by payment amount (after interest added)" ) {
				// MakePayment adds interest via MissPayment(), then subtracts payment
				// Interest = (600 - 500/2) * 0.00001 = 0.0035
				// After interest: principal = initialPrincipal * (1 + 0.0035)
				// Then payment is subtracted
				CHECK( mortgage.Principal() == lround(initialPrincipal * 1.0035) - payment );
			}
			THEN( "term is reduced by 1" ) {
				CHECK( mortgage.Term() == initialTerm - 1 );
			}
		}
	}
	GIVEN( "a mortgage with zero interest" ) {
		Mortgage mortgage("Mortgage", 100000, 0.0, 10);
		WHEN( "payments are made" ) {
			THEN( "each payment is principal/term" ) {
				int64_t expectedPayment = 100000 / 10;
				CHECK( mortgage.Payment() == expectedPayment );
				int64_t payment = mortgage.MakePayment();
				CHECK( payment == expectedPayment );
				CHECK( mortgage.Principal() == 100000 - expectedPayment );
				CHECK( mortgage.Term() == 9 );
			}
		}
	}
	GIVEN( "a mortgage near the end of term" ) {
		Mortgage mortgage("Mortgage", 100, 500, 1);
		WHEN( "the last payment is made" ) {
			int64_t payment = mortgage.MakePayment();
			THEN( "term becomes 0" ) {
				CHECK( mortgage.Term() == 0 );
			}
		}
	}
}

SCENARIO( "Missing mortgage payments", "[Mortgage][Payment]" ) {
	GIVEN( "a mortgage with principal and interest" ) {
		Mortgage mortgage("Mortgage", 100000, 500, 365);
		int64_t initialPrincipal = mortgage.Principal();
		int initialTerm = mortgage.Term();
		// Interest = (600 - 500/2) * 0.00001 = 0.0035
		double interestRate = 0.0035;

		WHEN( "a payment is missed" ) {
			mortgage.MissPayment();
			THEN( "interest is added to principal" ) {
				int64_t expectedPrincipal = initialPrincipal + lround(initialPrincipal * interestRate);
				CHECK( mortgage.Principal() == expectedPrincipal );
			}
			THEN( "term is unchanged" ) {
				CHECK( mortgage.Term() == initialTerm );
			}
		}
	}
	GIVEN( "a mortgage with zero interest" ) {
		Mortgage mortgage("Mortgage", 100000, 0.0, 365);
		int64_t initialPrincipal = mortgage.Principal();

		WHEN( "a payment is missed" ) {
			mortgage.MissPayment();
			THEN( "principal is unchanged" ) {
				CHECK( mortgage.Principal() == initialPrincipal );
			}
		}
	}
}

SCENARIO( "Paying extra principal", "[Mortgage][Payment]" ) {
	GIVEN( "a mortgage with principal" ) {
		Mortgage mortgage("Mortgage", 100000, 500, 365);
		int initialTerm = mortgage.Term();

		WHEN( "extra principal is paid" ) {
			int64_t paid = mortgage.PayExtra(10000);
			THEN( "the amount paid is returned" ) {
				CHECK( paid == 10000 );
			}
			THEN( "principal is reduced" ) {
				CHECK( mortgage.Principal() == 90000 );
			}
			THEN( "term is unchanged" ) {
				CHECK( mortgage.Term() == initialTerm );
			}
			THEN( "payment amount is reduced" ) {
				double originalPayment = CalculatePayment(100000, 0.0035, 365);
				double newPayment = CalculatePayment(90000, 0.0035, 365);
				CHECK_THAT( mortgage.PrecisePayment(), Catch::Matchers::WithinAbs(newPayment, 0.01) );
			}
		}
		WHEN( "trying to pay more than remaining principal" ) {
			int64_t paid = mortgage.PayExtra(200000);
			THEN( "only the remaining principal is paid" ) {
				CHECK( paid == 100000 );
				CHECK( mortgage.Principal() == 0 );
			}
		}
		WHEN( "paying extra on a zero principal mortgage" ) {
			mortgage.PayExtra(100000);
			REQUIRE( mortgage.Principal() == 0 );
			int64_t paid = mortgage.PayExtra(1000);
			THEN( "nothing is paid" ) {
				CHECK( paid == 0 );
				CHECK( mortgage.Principal() == 0 );
			}
		}
	}
}

SCENARIO( "Payment calculation edge cases", "[Mortgage][Payment]" ) {
	GIVEN( "a mortgage with zero term" ) {
		Mortgage mortgage("Mortgage", 1000, 500, 0);
		WHEN( "payment is calculated" ) {
			THEN( "entire principal is due" ) {
				CHECK( mortgage.Payment() == 1000 );
				CHECK( mortgage.PrecisePayment() == 1000.0 );
			}
		}
	}
	GIVEN( "a mortgage with very small principal" ) {
		Mortgage mortgage("Mortgage", 10, 500, 365);
		WHEN( "payment is calculated" ) {
			THEN( "payment is at least 1" ) {
				CHECK( mortgage.Payment() >= 1 );
			}
		}
	}
	GIVEN( "a mortgage with very low principal and long term" ) {
		Mortgage mortgage("Mortgage", 1, 500, 365);
		WHEN( "payment is calculated" ) {
			THEN( "payment is at least 1" ) {
				CHECK( mortgage.Payment() == 1 );
			}
		}
	}
}

SCENARIO( "Mortgage::Maximum calculation", "[Mortgage][Maximum]" ) {
	GIVEN( "annual revenue and credit score" ) {
		WHEN( "calculating maximum mortgage with good credit" ) {
			int64_t maxMortgage = Mortgage::Maximum(1000000, 700, 0);
			THEN( "a positive amount is returned" ) {
				CHECK( maxMortgage > 0 );
			}
		}
		WHEN( "calculating with existing payments" ) {
			int64_t maxMortgage1 = Mortgage::Maximum(1000000, 500, 0);
			int64_t maxMortgage2 = Mortgage::Maximum(1000000, 500, 100);
			THEN( "existing payments reduce maximum" ) {
				CHECK( maxMortgage2 < maxMortgage1 );
			}
		}
		WHEN( "annual revenue equals current payments" ) {
			int64_t maxMortgage = Mortgage::Maximum(36500, 500, 100);
			THEN( "maximum is zero" ) {
				// Revenue = 36500 - 365 * 100 = 0
				CHECK( maxMortgage == 0 );
			}
		}
		WHEN( "annual revenue is less than current payments" ) {
			int64_t maxMortgage = Mortgage::Maximum(10000, 500, 100);
			THEN( "maximum is zero" ) {
				// Revenue = 10000 - 365 * 100 < 0
				CHECK( maxMortgage == 0 );
			}
		}
		WHEN( "comparing different credit scores" ) {
			int64_t maxGood = Mortgage::Maximum(1000000, 700, 0);
			int64_t maxBad = Mortgage::Maximum(1000000, 300, 0);
			THEN( "better credit allows larger mortgage" ) {
				CHECK( maxGood > maxBad );
			}
		}
	}
}

SCENARIO( "Mortgage type variants", "[Mortgage][Types]" ) {
	GIVEN( "different mortgage types" ) {
		WHEN( "type is Mortgage" ) {
			Mortgage mortgage("Mortgage", 100000, 500, 365);
			THEN( "type is Mortgage" ) {
				CHECK( mortgage.Type() == "Mortgage" );
			}
		}
		WHEN( "type is Fine" ) {
			Mortgage fine("Fine", 10000, 0, 365);
			THEN( "type is Fine" ) {
				CHECK( fine.Type() == "Fine" );
			}
		}
		WHEN( "type is Debt" ) {
			Mortgage debt("Debt", 50000, 800, 365);
			THEN( "type is Debt" ) {
				CHECK( debt.Type() == "Debt" );
			}
		}
	}
}

SCENARIO( "Payment and PrecisePayment comparison", "[Mortgage][Payment]" ) {
	GIVEN( "a mortgage with principal and interest" ) {
		Mortgage mortgage("Mortgage", 123456, 450, 365);
		WHEN( "comparing Payment and PrecisePayment" ) {
			THEN( "Payment is rounded PrecisePayment" ) {
				int64_t payment = mortgage.Payment();
				double precisePayment = mortgage.PrecisePayment();
				// Payment should be the rounded value of PrecisePayment
				CHECK_THAT( static_cast<double>(payment), Catch::Matchers::WithinAbs(precisePayment, 0.5) );
			}
		}
	}
}

// #endregion unit tests

} // test namespace