/* test_categoryList.cpp
Copyright (c) 2022 by Amazinite

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
#include "../../../source/CategoryList.h"

// Include a helper for creating well-formed DataNodes (to enable creating non-empty CategoryLists).
#include "datanode-factory.h"

// ... and any system includes needed for the test file.
#include <algorithm>
#include <limits>
#include <string>
#include <vector>

namespace { // test namespace

// #region mock data

// Insert file-local data here, e.g. classes, structs, or fixtures that will be useful
// to help test this class/method.

// #endregion mock data



// #region unit tests
SCENARIO( "Creating a CategoryList" , "[CategoryList][Creation]" ) {
	GIVEN( "a category list" ) {
		auto list = CategoryList{};
		// A manually sorted list to compare against.
		std::vector<std::string> sorted;
		// A helper function for determining if a string is equal to a Category's name.
		auto equal = [](const std::string &s, const CategoryList::Category &c) noexcept -> bool { return s == c.Name(); };

		WHEN( "sorted contents are given to the list without precedence" ) {
			list.Load(AsDataNode("category test\n\tfirst\n\tsecond\n\tthird"));
			sorted.push_back("first"); // Precedence = 0, as that is the default precedence of the first added category.
			sorted.push_back("second"); // Precedence = 1, as each new category uses the last used precedence + 1.
			sorted.push_back("third"); // Precedence = 2

			THEN( "the list is already sorted" ) {
				// sorted = first, second, third
				CHECK( std::equal(sorted.begin(), sorted.end(), list.begin(), equal) );
			}

			THEN( "sorting the list does not change its ordering" ) {
				list.Sort();
				// sorted = first, second, third
				CHECK( std::equal(sorted.begin(), sorted.end(), list.begin(), equal) );
			}
		}

		WHEN( "sorted contents are given to the list with precedence" ) {
			list.Load(AsDataNode("category test\n\tfirst 10\n\tsecond 20\n\tthird 30"));
			sorted.push_back("first"); // Precedence = 10
			sorted.push_back("second"); // Precedence = 20
			sorted.push_back("third"); // Precedence = 30

			THEN( "the list is already sorted" ) {
				// sorted = first, second, third
				CHECK( std::equal(sorted.begin(), sorted.end(), list.begin(), equal) );
			}

			THEN( "sorting the list does not change its ordering" ) {
				list.Sort();
				// sorted = first, second, third
				CHECK( std::equal(sorted.begin(), sorted.end(), list.begin(), equal) );
			}
		}

		WHEN( "unsorted contents are given to the list with precedence" ) {
			list.Load(AsDataNode("category test\n\tfirst 7\n\tsecond 2\n\tthird 4"));
			sorted.push_back("second"); // Precedence = 2
			sorted.push_back("third"); // Precedence = 4
			sorted.push_back("first"); // Precedence = 7

			THEN( "the list is unsorted" ) {
				// sorted = second, third, first
				CHECK_FALSE( std::equal(sorted.begin(), sorted.end(), list.begin(), equal) );
			}

			THEN( "sorting the list correctly changes its ordering" ) {
				list.Sort();
				// sorted = second, third, first
				CHECK( std::equal(sorted.begin(), sorted.end(), list.begin(), equal) );

				AND_WHEN( "a list is loaded again without precedence" ) {
					list.Load(AsDataNode("category test\n\tfourth\n\tfifth"));

					THEN( "the new categories are at the end of the list in the order they were added" ) {
						sorted.push_back("fourth"); // Precedence = 5, as the last used precedence was 4.
						sorted.push_back("fifth"); // Precedence = 6
						// sorted = second, third, first, fourth, fifth
						CHECK( std::equal(sorted.begin(), sorted.end(), list.begin(), equal) );
					}

					THEN( "sorting the list moves the new categories into the correct positions" ) {
						sorted[2] = "fourth"; // Precedence = 5, as the last used precedence was 4.
						sorted.push_back("fifth"); // Precedence = 6
						sorted.push_back("first"); // Precedence = 7
						// sorted = second, third, fourth, fifth, first
						list.Sort();
						CHECK( std::equal(sorted.begin(), sorted.end(), list.begin(), equal) );
					}
				}

				AND_WHEN( "a list is loaded again with precedence" ) {
					list.Load(AsDataNode("category test\n\tfourth 1\n\tfifth 3"));

					THEN( "the new categories are at the end of the list in the order they were added" ) {
						sorted.push_back("fourth"); // Precedence = 1
						sorted.push_back("fifth"); // Precedence = 3
						// sorted = second, third, first, fourth, fifth
						CHECK( std::equal(sorted.begin(), sorted.end(), list.begin(), equal) );
					}

					THEN( "sorting the list moves the new categories into the correct positions" ) {
						sorted[0] = "fourth"; // Precedence = 1
						sorted[1] = "second"; // Precedence = 2
						sorted[2] = "fifth"; // Precedence = 3
						sorted.push_back("third"); // Precedence = 4
						sorted.push_back("first"); // Precedence = 7
						list.Sort();
						// sorted = fourth, second, fifth, third, first
						CHECK( std::equal(sorted.begin(), sorted.end(), list.begin(), equal) );
					}
				}

				AND_WHEN( "a list is loaded again with two categories of the same precedence" ) {
					list.Load(AsDataNode("category test\n\tfourth 7\n\tfifth 7"));

					THEN( "the new categories are at the end of the list in the order they were added" ) {
						sorted.push_back("fourth"); // Precedence = 7
						sorted.push_back("fifth"); // Precedence = 7
						// sorted = second, third, first, fourth, fifth
						CHECK( std::equal(sorted.begin(), sorted.end(), list.begin(), equal) );
					}

					THEN( "after sorting, the categories with the same precedence become alphabetically ordered" ) {
						sorted[2] = "fifth"; // Precedence = 7
						sorted.push_back("first"); // Precedence = 7
						sorted.push_back("fourth"); // Precedence = 7
						list.Sort();
						// sorted = second, third, fifth, first, fourth
						CHECK( std::equal(sorted.begin(), sorted.end(), list.begin(), equal) );
					}
				}

				AND_WHEN( "a list is loaded again with a category that already exists but with a different precedence" ) {
					list.Load(AsDataNode("category test\n\tthird 1"));

					THEN( "the duplicate category's position is unchanged but its precedence is updated" ) {
						// sorted = second, third, first
						CHECK( std::equal(sorted.begin(), sorted.end(), list.begin(), equal) );
					}

					THEN( "after sorting, the duplicate category's position is corrected" ) {
						sorted[0] = "third"; // Precedence = 1
						sorted[1] = "second"; // Precedence = 2
						sorted[2] = "first"; // Precedence = 6
						list.Sort();
						// sorted = third, second, first
						CHECK( std::equal(sorted.begin(), sorted.end(), list.begin(), equal) );
					}
				}
			}
		}
	}
}

SCENARIO( "Testing CategoryList::Contains" , "[CategoryList][Contains]" ) {
	GIVEN( "an empty category list" ) {
		auto list = CategoryList{};

		THEN( "it does not contain any category" ) {
			CHECK_FALSE( list.Contains("nonexistent") );
			CHECK_FALSE( list.Contains("") );
		}
	}

	GIVEN( "a category list with some items" ) {
		auto list = CategoryList{};
		list.Load(AsDataNode("category test\n\talpha\n\tbeta\n\tgamma"));

		THEN( "it contains the added categories" ) {
			CHECK( list.Contains("alpha") );
			CHECK( list.Contains("beta") );
			CHECK( list.Contains("gamma") );
		}

		THEN( "it does not contain categories that were not added" ) {
			CHECK_FALSE( list.Contains("delta") );
			CHECK_FALSE( list.Contains("Alpha") ); // Case-sensitive
			CHECK_FALSE( list.Contains("ALPHA") );
		}

		WHEN( "the list is cleared" ) {
			list.Clear();

			THEN( "it no longer contains any category" ) {
				CHECK_FALSE( list.Contains("alpha") );
				CHECK_FALSE( list.Contains("beta") );
				CHECK_FALSE( list.Contains("gamma") );
			}
		}
	}
}

SCENARIO( "Testing CategoryList::GetCategory" , "[CategoryList][GetCategory]" ) {
	GIVEN( "an empty category list" ) {
		auto list = CategoryList{};

		THEN( "GetCategory returns a default category for nonexistent names" ) {
			auto cat = list.GetCategory("nonexistent");
			CHECK( cat.Name() == "" );
		}

		THEN( "GetCategory handles empty string" ) {
			auto cat = list.GetCategory("");
			CHECK( cat.Name() == "" );
		}
	}

	GIVEN( "a category list with items" ) {
		auto list = CategoryList{};
		list.Load(AsDataNode("category test\n\tfirst 5\n\tsecond 10"));

		THEN( "GetCategory returns the correct category" ) {
			auto first = list.GetCategory("first");
			CHECK( first.Name() == "first" );

			auto second = list.GetCategory("second");
			CHECK( second.Name() == "second" );
		}

		THEN( "GetCategory returns a default category for nonexistent names" ) {
			auto cat = list.GetCategory("nonexistent");
			CHECK( cat.Name() == "" );
		}
	}
}

SCENARIO( "Testing CategoryList::Clear" , "[CategoryList][Clear]" ) {
	GIVEN( "an empty category list" ) {
		auto list = CategoryList{};

		WHEN( "clear is called" ) {
			list.Clear();

			THEN( "the list remains empty" ) {
				CHECK( list.begin() == list.end() );
				CHECK_FALSE( list.Contains("anything") );
			}
		}
	}

	GIVEN( "a category list with items" ) {
		auto list = CategoryList{};
		list.Load(AsDataNode("category test\n\tfirst\n\tsecond\n\tthird"));

		WHEN( "clear is called" ) {
			list.Clear();

			THEN( "the list becomes empty" ) {
				CHECK( list.begin() == list.end() );
				CHECK_FALSE( list.Contains("first") );
				CHECK_FALSE( list.Contains("second") );
				CHECK_FALSE( list.Contains("third") );
			}

			THEN( "new items can be added after clear with precedence reset" ) {
				list.Load(AsDataNode("category test\n\tnewfirst\n\tnewsecond"));
				CHECK( list.Contains("newfirst") );
				CHECK( list.Contains("newsecond") );
				CHECK_FALSE( list.Contains("first") );

				// Precedence should start from 0 after clear
				std::vector<std::string> sorted;
				sorted.push_back("newfirst"); // Precedence = 0
				sorted.push_back("newsecond"); // Precedence = 1
				auto equal = [](const std::string &s, const CategoryList::Category &c) noexcept -> bool { return s == c.Name(); };
				CHECK( std::equal(sorted.begin(), sorted.end(), list.begin(), equal) );
			}
		}

		WHEN( "clear is called multiple times" ) {
			list.Clear();
			list.Clear();

			THEN( "the list remains empty and well-formed" ) {
				CHECK( list.begin() == list.end() );
			}
		}
	}
}

SCENARIO( "Testing CategoryList iterators" , "[CategoryList][Iterator]" ) {
	GIVEN( "an empty category list" ) {
		auto list = CategoryList{};

		THEN( "begin equals end" ) {
			CHECK( list.begin() == list.end() );
		}

		THEN( "the list can be iterated without error" ) {
			int count = 0;
			for( const auto &cat : list )
				(void)cat, ++count;
			CHECK( count == 0 );
		}
	}

	GIVEN( "a category list with items" ) {
		auto list = CategoryList{};
		list.Load(AsDataNode("category test\n\tfirst\n\tsecond\n\tthird"));

		THEN( "begin does not equal end" ) {
			CHECK( list.begin() != list.end() );
		}

		THEN( "the list can be iterated and items accessed in order" ) {
			auto it = list.begin();
			CHECK( it->Name() == "first" );
			++it;
			CHECK( it->Name() == "second" );
			++it;
			CHECK( it->Name() == "third" );
			++it;
			CHECK( it == list.end() );
		}

		THEN( "const iterators work correctly" ) {
			const auto &constList = list;
			auto it = constList.begin();
			CHECK( it->Name() == "first" );
		}

		THEN( "range-based for loop works correctly" ) {
			std::vector<std::string> names;
			for( const auto &cat : list )
				names.push_back(cat.Name());

			CHECK( names.size() == 3 );
			CHECK( names[0] == "first" );
			CHECK( names[1] == "second" );
			CHECK( names[2] == "third" );
		}
	}
}

SCENARIO( "Testing Category::operator< and sorting" , "[CategoryList][Category][Sorting]" ) {
	GIVEN( "categories with different precedences" ) {
		auto list = CategoryList{};
		list.Load(AsDataNode("category test\n\tzebra 1\n\talpha 2\n\tbeta 0"));

		WHEN( "sorted" ) {
			list.Sort();

			THEN( "categories are ordered by precedence first" ) {
				auto it = list.begin();
				CHECK( it->Name() == "beta" );   // Precedence 0
				++it;
				CHECK( it->Name() == "zebra" );  // Precedence 1
				++it;
				CHECK( it->Name() == "alpha" );  // Precedence 2
			}
		}
	}

	GIVEN( "categories with the same precedence" ) {
		auto list = CategoryList{};
		list.Load(AsDataNode("category test\n\tzebra 5\n\talpha 5\n\tbeta 5"));

		WHEN( "sorted" ) {
			list.Sort();

			THEN( "categories with same precedence are sorted alphabetically" ) {
				auto it = list.begin();
				CHECK( it->Name() == "alpha" );  // Alphabetically first
				++it;
				CHECK( it->Name() == "beta" );   // Alphabetically second
				++it;
				CHECK( it->Name() == "zebra" );  // Alphabetically last
			}
		}
	}

	GIVEN( "categories with mixed same and different precedences" ) {
		auto list = CategoryList{};
		list.Load(AsDataNode("category test\n\tzebra 2\n\talpha 1\n\tbeta 2\n\tgamma 0"));

		WHEN( "sorted" ) {
			list.Sort();

			THEN( "categories are sorted by precedence, then alphabetically within same precedence" ) {
				auto it = list.begin();
				CHECK( it->Name() == "gamma" ); // Precedence 0
				++it;
				CHECK( it->Name() == "alpha" ); // Precedence 1
				++it;
				CHECK( it->Name() == "beta" );   // Precedence 2, alphabetically first
				++it;
				CHECK( it->Name() == "zebra" );  // Precedence 2, alphabetically second
			}
		}
	}
}

SCENARIO( "Testing edge cases and boundary conditions" , "[CategoryList][EdgeCase]" ) {
	GIVEN( "a category list" ) {
		auto list = CategoryList{};

		WHEN( "loading a single category" ) {
			list.Load(AsDataNode("category test\n\tsingle"));

			THEN( "the list contains exactly one category" ) {
				CHECK( list.Contains("single") );
				auto it = list.begin();
				CHECK( it != list.end() );
				CHECK( it->Name() == "single" );
				++it;
				CHECK( it == list.end() );
			}

			THEN( "sorting a single element list works correctly" ) {
				list.Sort();
				CHECK( list.Contains("single") );
			}
		}

		WHEN( "loading categories with negative precedence" ) {
			list.Load(AsDataNode("category test\n\talpha -5\n\tbeta -10\n\tgamma 0"));

			THEN( "negative precedences are handled correctly" ) {
				list.Sort();
				auto it = list.begin();
				CHECK( it->Name() == "beta" );   // Precedence -10
				++it;
				CHECK( it->Name() == "alpha" );  // Precedence -5
				++it;
				CHECK( it->Name() == "gamma" );  // Precedence 0
			}
		}

		WHEN( "loading categories with large precedence values" ) {
			list.Load(AsDataNode("category test\n\talpha 999999\n\tbeta 1\n\tgamma 1000000"));

			THEN( "large precedence values are handled correctly" ) {
				list.Sort();
				auto it = list.begin();
				CHECK( it->Name() == "beta" );    // Precedence 1
				++it;
				CHECK( it->Name() == "alpha" );   // Precedence 999999
				++it;
				CHECK( it->Name() == "gamma" );   // Precedence 1000000
			}
		}

		WHEN( "loading an empty category node" ) {
			list.Load(AsDataNode("category test"));

			THEN( "the list remains empty" ) {
				CHECK( list.begin() == list.end() );
			}
		}

		WHEN( "loading duplicate categories with different precedences multiple times" ) {
			list.Load(AsDataNode("category test\n\talpha 5"));
			list.Load(AsDataNode("category test\n\talpha 10"));
			list.Load(AsDataNode("category test\n\talpha 1"));

			THEN( "the last precedence is used" ) {
				CHECK( list.Contains("alpha") );
				list.Sort();
				// With precedence 1, alpha should be first
				auto it = list.begin();
				CHECK( it->Name() == "alpha" );
			}
		}

		WHEN( "loading categories with similar names (case sensitivity)" ) {
			list.Load(AsDataNode("category test\n\tAlpha\n\talpha\n\tALPHA"));

			THEN( "categories are case-sensitive and distinct" ) {
				CHECK( list.Contains("Alpha") );
				CHECK( list.Contains("alpha") );
				CHECK( list.Contains("ALPHA") );

				int count = 0;
				for( const auto &cat : list )
					++count;
				CHECK( count == 3 );
			}
		}

		WHEN( "loading categories with special characters in names" ) {
			list.Load(AsDataNode("category test\n\tfirst-category\n\tsecond_category\n\tthird.category"));

			THEN( "special characters are preserved in names" ) {
				CHECK( list.Contains("first-category") );
				CHECK( list.Contains("second_category") );
				CHECK( list.Contains("third.category") );
			}
		}
	}
}

SCENARIO( "Testing CategoryList::Load behavior" , "[CategoryList][Load]" ) {
	GIVEN( "an empty category list" ) {
		auto list = CategoryList{};

		WHEN( "loading multiple times preserves existing items" ) {
			list.Load(AsDataNode("category test\n\tfirst"));
			list.Load(AsDataNode("category test\n\tsecond"));

			THEN( "both items are in the list" ) {
				CHECK( list.Contains("first") );
				CHECK( list.Contains("second") );
			}

			THEN( "precedence continues to increment across loads" ) {
				list.Sort();
				auto it = list.begin();
				CHECK( it->Name() == "first" );  // Precedence 0
				++it;
				CHECK( it->Name() == "second" ); // Precedence 1
			}
		}

		WHEN( "loading with explicit zero precedence" ) {
			list.Load(AsDataNode("category test\n\tfirst 0\n\tsecond 0"));

			THEN( "categories with zero precedence are sorted alphabetically" ) {
				list.Sort();
				auto it = list.begin();
				CHECK( it->Name() == "first" );  // Alphabetically first, precedence 0
				++it;
				CHECK( it->Name() == "second" ); // Alphabetically second, precedence 0
			}
		}

		WHEN( "loading categories in reverse alphabetical order with same precedence" ) {
			list.Load(AsDataNode("category test\n\tzebra 1\n\tyellow 1\n\txray 1"));

			THEN( "sorting orders them alphabetically" ) {
				list.Sort();
				auto it = list.begin();
				CHECK( it->Name() == "xray" );
				++it;
				CHECK( it->Name() == "yellow" );
				++it;
				CHECK( it->Name() == "zebra" );
			}
		}
	}
}

SCENARIO( "Testing Category name access" , "[CategoryList][Category][Name]" ) {
	GIVEN( "a category with a name" ) {
		auto list = CategoryList{};
		list.Load(AsDataNode("category test\n\tTestName"));

		THEN( "the name can be accessed via Name() method" ) {
			auto cat = list.GetCategory("TestName");
			CHECK( cat.Name() == "TestName" );
		}

		THEN( "the name reference is valid" ) {
			const auto &cat = list.GetCategory("TestName");
			const std::string &nameRef = cat.Name();
			CHECK( nameRef == "TestName" );
		}
	}
}
// #endregion unit tests



} // test namespace
