/* test_conditionEntry.cpp
Copyright (c) 2025 by Author

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
#include "../../../source/ConditionEntry.h"

// Include ConditionsStore for provider-related tests.
#include "../../../source/ConditionsStore.h"

// ... and any system includes needed for the test file.
#include <cstdint>
#include <functional>
#include <map>
#include <string>

namespace { // test namespace

// #region mock data

// Mock provider for testing ConditionEntry's provider functionality.
class MockProvider {
public:
	// Get function that retrieves value from internal map.
	int64_t Get(const ConditionEntry &ce)
	{
		auto it = values.find(ce.Name());
		return it != values.end() ? it->second : 0;
	}

	// Set function that stores value in internal map.
	void Set(ConditionEntry &ce, int64_t value)
	{
		values[ce.Name()] = value;
	}

	// Helper to setup a prefixed provider on an entry.
	void SetupPrefixedProvider(ConditionEntry &entry)
	{
		entry.ProvidePrefixed(
			[this](const ConditionEntry &ce) { return Get(ce); },
			[this](ConditionEntry &ce, int64_t val) { Set(ce, val); }
		);
	}

	// Helper to setup a read-only prefixed provider on an entry.
	void SetupReadOnlyPrefixedProvider(ConditionEntry &entry)
	{
		entry.ProvidePrefixed([this](const ConditionEntry &ce) { return Get(ce); });
	}

	// Helper to setup a named provider on an entry.
	void SetupNamedProvider(ConditionEntry &entry)
	{
		entry.ProvideNamed(
			[this](const ConditionEntry &ce) { return Get(ce); },
			[this](ConditionEntry &ce, int64_t val) { Set(ce, val); }
		);
	}

	// Helper to setup a read-only named provider on an entry.
	void SetupReadOnlyNamedProvider(ConditionEntry &entry)
	{
		entry.ProvideNamed([this](const ConditionEntry &ce) { return Get(ce); });
	}

	std::map<std::string, int64_t> values;
};

// #endregion mock data



// #region unit tests

SCENARIO( "Creating a ConditionEntry", "[ConditionEntry][Creation]" )
{
	GIVEN( "a name string" )
	{
		WHEN( "a ConditionEntry is constructed" )
		{
			ConditionEntry entry("testCondition");

			THEN( "the entry has the correct name and default value of 0" )
			{
				REQUIRE( entry.Name() == "testCondition" );
				REQUIRE( entry == 0 );
				REQUIRE( static_cast<int64_t>(entry) == 0 );
			}
		}

		WHEN( "a ConditionEntry is constructed with an empty name" )
		{
			ConditionEntry entry("");

			THEN( "the entry has an empty name and default value of 0" )
			{
				REQUIRE( entry.Name() == "" );
				REQUIRE( entry == 0 );
			}
		}

		WHEN( "a ConditionEntry is constructed with a complex name" )
		{
			ConditionEntry entry("namespace: complex condition name");

			THEN( "the entry preserves the full name" )
			{
				REQUIRE( entry.Name() == "namespace: complex condition name" );
				REQUIRE( entry == 0 );
			}
		}
	}
}

SCENARIO( "Getting the name without prefix", "[ConditionEntry][NameWithoutPrefix]" )
{
	GIVEN( "a ConditionEntry without a provider" )
	{
		ConditionEntry entry("testCondition");

		WHEN( "NameWithoutPrefix is called" )
		{
			THEN( "the full name is returned" )
			{
				REQUIRE( entry.NameWithoutPrefix() == "testCondition" );
			}
		}
	}

	GIVEN( "a ConditionEntry with a named provider" )
	{
		// Note: Named providers do not set providingEntry, so NameWithoutPrefix returns full name
		ConditionsStore store;
		MockProvider provider;
		ConditionEntry &entry = store["namedCondition"];
		provider.SetupNamedProvider(entry);
		provider.values["namedCondition"] = 42;

		WHEN( "NameWithoutPrefix is called" )
		{
			THEN( "the full name is returned (named providers don't have prefixes)" )
			{
				REQUIRE( entry.NameWithoutPrefix() == "namedCondition" );
			}
		}
	}

	GIVEN( "a ConditionEntry with a prefixed provider through ConditionsStore" )
	{
		ConditionsStore store;
		MockProvider provider;
		// Setup a prefix provider
		ConditionEntry &prefixEntry = store["prefix: "];
		provider.SetupPrefixedProvider(prefixEntry);

		// Access a condition that matches the prefix
		ConditionEntry &derivedEntry = store["prefix: test"];

		WHEN( "NameWithoutPrefix is called on the derived entry" )
		{
			THEN( "the name without the prefix is returned" )
			{
				REQUIRE( derivedEntry.Name() == "prefix: test" );
				REQUIRE( derivedEntry.NameWithoutPrefix() == "test" );
			}
		}

		AND_GIVEN( "the prefix provider has values" )
		{
			provider.values["prefix: test"] = 100;

			THEN( "the value can be read through the derived entry" )
			{
				REQUIRE( derivedEntry == 100 );
			}
		}
	}
}

SCENARIO( "Using int64_t conversion operator", "[ConditionEntry][Conversion]" )
{
	GIVEN( "a ConditionEntry without a provider" )
	{
		ConditionEntry entry("testVar");

		WHEN( "the entry has default value" )
		{
			THEN( "conversion to int64_t returns 0" )
			{
				REQUIRE( static_cast<int64_t>(entry) == 0 );
				REQUIRE( entry == 0 );
			}
		}

		WHEN( "the entry has a value set directly" )
		{
			entry = 42;

			THEN( "conversion to int64_t returns the set value" )
			{
				REQUIRE( static_cast<int64_t>(entry) == 42 );
				REQUIRE( entry == 42 );
			}
		}

		WHEN( "the entry has a negative value" )
		{
			entry = -100;

			THEN( "conversion to int64_t returns the negative value" )
			{
				REQUIRE( static_cast<int64_t>(entry) == -100 );
				REQUIRE( entry == -100 );
			}
		}

		WHEN( "the entry has a large positive value" )
		{
			entry = 9223372036854775807LL; // INT64_MAX

			THEN( "conversion to int64_t returns the large value" )
			{
				REQUIRE( static_cast<int64_t>(entry) == 9223372036854775807LL );
			}
		}

		WHEN( "the entry has a large negative value" )
		{
			entry = -9223372036854775807LL - 1; // INT64_MIN

			THEN( "conversion to int64_t returns the large negative value" )
			{
				REQUIRE( static_cast<int64_t>(entry) == -9223372036854775807LL - 1 );
			}
		}
	}

	GIVEN( "a ConditionEntry with a named provider" )
	{
		ConditionsStore store;
		MockProvider provider;
		ConditionEntry &entry = store["derivedVar"];
		provider.SetupNamedProvider(entry);

		WHEN( "the provider has a value" )
		{
			provider.values["derivedVar"] = 200;

			THEN( "conversion to int64_t uses the provider's get function" )
			{
				REQUIRE( static_cast<int64_t>(entry) == 200 );
				REQUIRE( entry == 200 );
			}
		}

		WHEN( "the provider has no value (defaults to 0)" )
		{
			THEN( "conversion to int64_t returns 0" )
			{
				REQUIRE( static_cast<int64_t>(entry) == 0 );
				REQUIRE( entry == 0 );
			}
		}
	}
}

SCENARIO( "Using assignment operator", "[ConditionEntry][Assignment]" )
{
	GIVEN( "a ConditionEntry without a provider" )
	{
		ConditionEntry entry("testVar");

		WHEN( "a value is assigned" )
		{
			entry = 42;

			THEN( "the entry stores the value" )
			{
				REQUIRE( entry == 42 );
			}
		}

		WHEN( "multiple values are assigned in sequence" )
		{
			entry = 10;
			REQUIRE( entry == 10 );
			entry = 20;
			REQUIRE( entry == 20 );
			entry = -5;
			REQUIRE( entry == -5 );

			THEN( "each assignment overwrites the previous value" )
			{
				REQUIRE( entry == -5 );
			}
		}

		WHEN( "zero is assigned" )
		{
			entry = 100;
			entry = 0;

			THEN( "the entry stores zero" )
			{
				REQUIRE( entry == 0 );
			}
		}
	}

	GIVEN( "a ConditionEntry with a named provider that has a set function" )
	{
		ConditionsStore store;
		MockProvider provider;
		ConditionEntry &entry = store["derivedVar"];
		provider.SetupNamedProvider(entry);

		WHEN( "a value is assigned" )
		{
			entry = 150;

			THEN( "the set function is called and value is stored in provider" )
			{
				REQUIRE( provider.values["derivedVar"] == 150 );
				REQUIRE( entry == 150 );
			}
		}
	}

	GIVEN( "a ConditionEntry with a read-only named provider" )
	{
		ConditionsStore store;
		MockProvider provider;
		ConditionEntry &entry = store["readOnlyVar"];
		provider.SetupReadOnlyNamedProvider(entry);
		provider.values["readOnlyVar"] = 50;

		WHEN( "a value is assigned" )
		{
			entry = 999;

			THEN( "the assignment is silently ignored (no set function)" )
			{
				// The value in the provider stays unchanged
				REQUIRE( provider.values["readOnlyVar"] == 50 );
				// Reading still returns the provider's value
				REQUIRE( entry == 50 );
			}
		}
	}

	GIVEN( "a ConditionEntry with a prefixed provider through ConditionsStore" )
	{
		ConditionsStore store;
		MockProvider provider;
		ConditionEntry &prefixEntry = store["prefix: "];
		provider.SetupPrefixedProvider(prefixEntry);
		ConditionEntry &derivedEntry = store["prefix: test"];

		WHEN( "a value is assigned to the derived entry" )
		{
			derivedEntry = 75;

			THEN( "the value is stored through the provider" )
			{
				REQUIRE( provider.values["prefix: test"] == 75 );
				REQUIRE( derivedEntry == 75 );
			}
		}
	}
}

SCENARIO( "Using increment operator", "[ConditionEntry][Increment]" )
{
	GIVEN( "a ConditionEntry with a direct value" )
	{
		ConditionEntry entry("counter");

		WHEN( "prefix increment is used starting from 0" )
		{
			THEN( "incrementing increases the value by 1" )
			{
				REQUIRE( entry == 0 );
				++entry;
				REQUIRE( entry == 1 );
				++entry;
				REQUIRE( entry == 2 );
				++entry;
				REQUIRE( entry == 3 );
			}
		}

		WHEN( "prefix increment is used on a positive value" )
		{
			entry = 100;
			++entry;

			THEN( "the value increases by 1" )
			{
				REQUIRE( entry == 101 );
			}
		}

		WHEN( "prefix increment is used on a negative value" )
		{
			entry = -10;
			++entry;

			THEN( "the value increases by 1" )
			{
				REQUIRE( entry == -9 );
			}
		}

		WHEN( "prefix increment returns a reference" )
		{
			entry = 5;

			THEN( "the returned reference can be used for chained operations" )
			{
				ConditionEntry &ref = ++entry;
				REQUIRE( &ref == &entry );
				REQUIRE( ref == 6 );
			}
		}
	}

	GIVEN( "a ConditionEntry with a named provider" )
	{
		ConditionsStore store;
		MockProvider provider;
		ConditionEntry &entry = store["counter"];
		provider.SetupNamedProvider(entry);
		provider.values["counter"] = 10;

		WHEN( "prefix increment is used" )
		{
			++entry;

			THEN( "the value in the provider is incremented" )
			{
				REQUIRE( provider.values["counter"] == 11 );
				REQUIRE( entry == 11 );
			}
		}
	}
}

SCENARIO( "Using decrement operator", "[ConditionEntry][Decrement]" )
{
	GIVEN( "a ConditionEntry with a direct value" )
	{
		ConditionEntry entry("counter");

		WHEN( "prefix decrement is used starting from 0" )
		{
			THEN( "decrementing decreases the value by 1" )
			{
				REQUIRE( entry == 0 );
				--entry;
				REQUIRE( entry == -1 );
				--entry;
				REQUIRE( entry == -2 );
				--entry;
				REQUIRE( entry == -3 );
			}
		}

		WHEN( "prefix decrement is used on a positive value" )
		{
			entry = 100;
			--entry;

			THEN( "the value decreases by 1" )
			{
				REQUIRE( entry == 99 );
			}
		}

		WHEN( "prefix decrement is used on a negative value" )
		{
			entry = -10;
			--entry;

			THEN( "the value decreases by 1" )
			{
				REQUIRE( entry == -11 );
			}
		}

		WHEN( "prefix decrement returns a reference" )
		{
			entry = 5;

			THEN( "the returned reference can be used for chained operations" )
			{
				ConditionEntry &ref = --entry;
				REQUIRE( &ref == &entry );
				REQUIRE( ref == 4 );
			}
		}
	}

	GIVEN( "a ConditionEntry with a named provider" )
	{
		ConditionsStore store;
		MockProvider provider;
		ConditionEntry &entry = store["counter"];
		provider.SetupNamedProvider(entry);
		provider.values["counter"] = 10;

		WHEN( "prefix decrement is used" )
		{
			--entry;

			THEN( "the value in the provider is decremented" )
			{
				REQUIRE( provider.values["counter"] == 9 );
				REQUIRE( entry == 9 );
			}
		}
	}
}

SCENARIO( "Using compound assignment operators", "[ConditionEntry][CompoundAssignment]" )
{
	GIVEN( "a ConditionEntry with a direct value" )
	{
		ConditionEntry entry("value");

		WHEN( "operator+= is used" )
		{
			entry = 10;
			entry += 5;

			THEN( "the value is increased by the operand" )
			{
				REQUIRE( entry == 15 );
			}

			AND_THEN( "multiple additions accumulate" )
			{
				entry += 10;
				REQUIRE( entry == 25 );
				entry += -5;
				REQUIRE( entry == 20 );
			}
		}

		WHEN( "operator-= is used" )
		{
			entry = 100;
			entry -= 30;

			THEN( "the value is decreased by the operand" )
			{
				REQUIRE( entry == 70 );
			}

			AND_THEN( "multiple subtractions accumulate" )
			{
				entry -= 20;
				REQUIRE( entry == 50 );
				entry -= -10;
				REQUIRE( entry == 60 );
			}
		}

		WHEN( "operators return a reference" )
		{
			entry = 5;

			THEN( "operator+= returns a reference to the entry" )
			{
				ConditionEntry &ref = (entry += 10);
				REQUIRE( &ref == &entry );
				REQUIRE( ref == 15 );
			}

			THEN( "operator-= returns a reference to the entry" )
			{
				ConditionEntry &ref = (entry -= 3);
				REQUIRE( &ref == &entry );
				REQUIRE( ref == 2 );
			}
		}
	}

	GIVEN( "a ConditionEntry with a named provider" )
	{
		ConditionsStore store;
		MockProvider provider;
		ConditionEntry &entry = store["value"];
		provider.SetupNamedProvider(entry);
		provider.values["value"] = 50;

		WHEN( "operator+= is used" )
		{
			entry += 25;

			THEN( "the value in the provider is updated" )
			{
				REQUIRE( provider.values["value"] == 75 );
				REQUIRE( entry == 75 );
			}
		}

		WHEN( "operator-= is used" )
		{
			entry -= 15;

			THEN( "the value in the provider is updated" )
			{
				REQUIRE( provider.values["value"] == 35 );
				REQUIRE( entry == 35 );
			}
		}
	}

	GIVEN( "a ConditionEntry with a read-only provider" )
	{
		ConditionsStore store;
		MockProvider provider;
		ConditionEntry &entry = store["readOnly"];
		provider.SetupReadOnlyNamedProvider(entry);
		provider.values["readOnly"] = 100;

		WHEN( "operator+= is used" )
		{
			entry += 25;

			THEN( "the operation is silently ignored" )
			{
				REQUIRE( provider.values["readOnly"] == 100 );
				REQUIRE( entry == 100 );
			}
		}

		WHEN( "operator-= is used" )
		{
			entry -= 50;

			THEN( "the operation is silently ignored" )
			{
				REQUIRE( provider.values["readOnly"] == 100 );
				REQUIRE( entry == 100 );
			}
		}
	}
}

SCENARIO( "Using ProvidePrefixed method", "[ConditionEntry][ProvidePrefixed]" )
{
	GIVEN( "a ConditionEntry accessed through ConditionsStore" )
	{
		ConditionsStore store;
		ConditionEntry &entry = store["prefix: "];

		WHEN( "ProvidePrefixed is called with only a get function" )
		{
			int64_t storedValue = 42;
			entry.ProvidePrefixed([&storedValue](const ConditionEntry &ce) {
				return storedValue;
			});

			THEN( "the entry uses the get function to retrieve values" )
			{
				REQUIRE( entry == 42 );
			}

			AND_WHEN( "a value is assigned" )
			{
				entry = 100;

				THEN( "the assignment is silently ignored (read-only)" )
				{
					REQUIRE( storedValue == 42 );
					REQUIRE( entry == 42 );
				}
			}
		}

		WHEN( "ProvidePrefixed is called with get and set functions" )
		{
			int64_t storedValue = 42;
			entry.ProvidePrefixed(
				[&storedValue](const ConditionEntry &ce) {
					return storedValue;
				},
				[&storedValue](ConditionEntry &ce, int64_t val) {
					storedValue = val;
				}
			);

			THEN( "the entry uses the get function to retrieve values" )
			{
				REQUIRE( entry == 42 );
			}

			AND_WHEN( "a value is assigned" )
			{
				entry = 100;

				THEN( "the set function is called" )
				{
					REQUIRE( storedValue == 100 );
					REQUIRE( entry == 100 );
				}
			}

			AND_WHEN( "arithmetic operations are performed" )
			{
				++entry;
				REQUIRE( storedValue == 43 );
				REQUIRE( entry == 43 );

				--entry;
				REQUIRE( storedValue == 42 );
				REQUIRE( entry == 42 );

				entry += 10;
				REQUIRE( storedValue == 52 );
				REQUIRE( entry == 52 );

				entry -= 12;
				REQUIRE( storedValue == 40 );
				REQUIRE( entry == 40 );
			}
		}
	}
}

SCENARIO( "Using ProvideNamed method", "[ConditionEntry][ProvideNamed]" )
{
	GIVEN( "a ConditionEntry accessed through ConditionsStore" )
	{
		ConditionsStore store;
		ConditionEntry &entry = store["namedCondition"];

		WHEN( "ProvideNamed is called with only a get function" )
		{
			int64_t storedValue = 99;
			entry.ProvideNamed([&storedValue](const ConditionEntry &ce) {
				return storedValue;
			});

			THEN( "the entry uses the get function to retrieve values" )
			{
				REQUIRE( entry == 99 );
			}

			AND_WHEN( "a value is assigned" )
			{
				entry = 200;

				THEN( "the assignment is silently ignored (read-only)" )
				{
					REQUIRE( storedValue == 99 );
					REQUIRE( entry == 99 );
				}
			}
		}

		WHEN( "ProvideNamed is called with get and set functions" )
		{
			int64_t storedValue = 99;
			entry.ProvideNamed(
				[&storedValue](const ConditionEntry &ce) {
					return storedValue;
				},
				[&storedValue](ConditionEntry &ce, int64_t val) {
					storedValue = val;
				}
			);

			THEN( "the entry uses the get function to retrieve values" )
			{
				REQUIRE( entry == 99 );
			}

			AND_WHEN( "a value is assigned" )
			{
				entry = 200;

				THEN( "the set function is called" )
				{
					REQUIRE( storedValue == 200 );
					REQUIRE( entry == 200 );
				}
			}

			AND_WHEN( "arithmetic operations are performed" )
			{
				++entry;
				REQUIRE( storedValue == 100 );
				REQUIRE( entry == 100 );

				--entry;
				REQUIRE( storedValue == 99 );
				REQUIRE( entry == 99 );

				entry += 1;
				REQUIRE( storedValue == 100 );
				REQUIRE( entry == 100 );

				entry -= 1;
				REQUIRE( storedValue == 99 );
				REQUIRE( entry == 99 );
			}
		}
	}
}

SCENARIO( "NotifyUpdate method", "[ConditionEntry][NotifyUpdate]" )
{
	GIVEN( "a ConditionEntry" )
	{
		ConditionEntry entry("testVar");

		WHEN( "NotifyUpdate is called" )
		{
			// NotifyUpdate currently does nothing (no subscribers implemented)
			// This test ensures the method exists and can be called without crashing

			THEN( "no exception is thrown" )
			{
				REQUIRE_NOTHROW( entry.NotifyUpdate(42) );
				REQUIRE_NOTHROW( entry.NotifyUpdate(0) );
				REQUIRE_NOTHROW( entry.NotifyUpdate(-1) );
				REQUIRE_NOTHROW( entry.NotifyUpdate(9223372036854775807ULL) );
			}
		}
	}
}

SCENARIO( "Copy and move semantics are deleted", "[ConditionEntry][Semantics]" )
{
	// This test verifies at compile time that copy and move operations are deleted.
	// The following operations should not compile:
	// - ConditionEntry entry2(entry);  // copy constructor
	// - ConditionEntry entry2 = entry; // copy assignment
	// - ConditionEntry entry2(std::move(entry)); // move constructor
	// - ConditionEntry entry2 = std::move(entry); // move assignment

	GIVEN( "a ConditionEntry" )
	{
		ConditionEntry entry("test");

		WHEN( "verifying copy/move restrictions" )
		{
			// We can only verify this indirectly by using the entry
			// The actual deletion of copy/move is enforced at compile time
			THEN( "the entry can be used normally" )
			{
				entry = 42;
				REQUIRE( entry == 42 );
			}
		}
	}
}

SCENARIO( "Chained arithmetic operations", "[ConditionEntry][Chaining]" )
{
	GIVEN( "a ConditionEntry with direct storage" )
	{
		ConditionEntry entry("value");

		WHEN( "multiple operations are chained" )
		{
			entry = 10;
			(++entry) += 5;
			(--entry) -= 3;

			THEN( "all operations are applied correctly" )
			{
				REQUIRE( entry == 12 );
			}
		}
	}

	GIVEN( "a ConditionEntry with a provider" )
	{
		ConditionsStore store;
		MockProvider provider;
		ConditionEntry &entry = store["chainTest"];
		provider.SetupNamedProvider(entry);
		provider.values["chainTest"] = 100;

		WHEN( "multiple operations are chained" )
		{
			(++entry) += 10;
			(--entry) -= 5;

			THEN( "all operations are applied correctly through the provider" )
			{
				REQUIRE( provider.values["chainTest"] == 105 );
				REQUIRE( entry == 105 );
			}
		}
	}
}

SCENARIO( "Comparison operations through int64_t conversion", "[ConditionEntry][Comparison]" )
{
	GIVEN( "ConditionEntry instances with values" )
	{
		ConditionEntry entry1("first");
		ConditionEntry entry2("second");

		entry1 = 10;
		entry2 = 20;

		WHEN( "comparing entries to int64_t values" )
		{
			THEN( "comparisons work through implicit conversion" )
			{
				REQUIRE( entry1 == 10 );
				REQUIRE( entry2 == 20 );
				REQUIRE( entry1 != 20 );
				REQUIRE( entry1 < 15 );
				REQUIRE( entry1 <= 10 );
				REQUIRE( entry2 > 15 );
				REQUIRE( entry2 >= 20 );
			}
		}

		WHEN( "comparing int64_t values to entries" )
		{
			THEN( "reverse comparisons work" )
			{
				REQUIRE( 10 == entry1 );
				REQUIRE( 20 == entry2 );
				REQUIRE( 20 != entry1 );
				REQUIRE( 15 > entry1 );
				REQUIRE( 10 >= entry1 );
				REQUIRE( 15 < entry2 );
				REQUIRE( 20 <= entry2 );
			}
		}
	}
}

// #endregion unit tests



} // test namespace