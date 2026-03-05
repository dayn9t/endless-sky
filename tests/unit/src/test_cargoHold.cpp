/* test_cargoHold.cpp
Copyright (c) 2025 by Endless Sky contributors

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
#include "../../../source/CargoHold.h"

// Include Body before Outfit to avoid incomplete type issues.
#include "../../../source/Body.h"

// Include the Outfit header for outfit-related tests.
// Note: Outfit has a default constructor that initializes mass to 0.
#include "../../../source/Outfit.h"

// ... and any system includes needed for the test file.
#include <map>
#include <string>



namespace { // test namespace

// #region mock data

// Note: Testing outfit functionality is limited because Outfit requires
// GameData to be fully loaded for meaningful use. A default-constructed
// Outfit has mass = 0, which is useful for testing zero-mass outfits.

// #endregion mock data



// #region unit tests

SCENARIO( "Creating a CargoHold with default constructor", "[CargoHold][Creation]" ) {
	GIVEN( "a default-constructed CargoHold" ) {
		CargoHold hold;
		THEN( "it has unlimited capacity by default" ) {
			CHECK( hold.Size() == -1 );
		}
		THEN( "it has unlimited bunks by default" ) {
			CHECK( hold.BunksFree() == -1 );
		}
		THEN( "it is empty" ) {
			CHECK( hold.IsEmpty() );
			CHECK( hold.CommoditiesSize() == 0 );
			CHECK( hold.OutfitsSize() == 0 );
			CHECK( hold.MissionCargoSize() == 0 );
			CHECK( hold.Passengers() == 0 );
		}
		THEN( "it has no outfits" ) {
			CHECK_FALSE( hold.HasOutfits() );
		}
		THEN( "it has no mission cargo" ) {
			CHECK_FALSE( hold.HasMissionCargo() );
		}
	}
}

SCENARIO( "Clearing a CargoHold", "[CargoHold][Clear]" ) {
	GIVEN( "a CargoHold with cargo" ) {
		CargoHold hold;
		hold.SetSize(100);
		hold.SetBunks(10);
		hold.Add("gold", 50);
		REQUIRE( hold.Size() == 100 );
		REQUIRE( hold.BunksFree() == 10 );
		REQUIRE( hold.Get("gold") == 50 );

		WHEN( "Clear() is called" ) {
			hold.Clear();
			THEN( "size is reset to 0" ) {
				CHECK( hold.Size() == 0 );
			}
			THEN( "bunks is reset to 0" ) {
				CHECK( hold.BunksFree() == 0 );
			}
			THEN( "commodities are cleared" ) {
				CHECK( hold.Commodities().empty() );
				CHECK( hold.CommoditiesSize() == 0 );
			}
			THEN( "outfits are cleared" ) {
				CHECK( hold.Outfits().empty() );
				CHECK( hold.OutfitsSize() == 0 );
			}
			THEN( "mission cargo is cleared" ) {
				CHECK( hold.MissionCargo().empty() );
				CHECK( hold.MissionCargoSize() == 0 );
			}
			THEN( "passengers are cleared" ) {
				CHECK( hold.PassengerList().empty() );
				CHECK( hold.Passengers() == 0 );
			}
			THEN( "the hold is empty" ) {
				CHECK( hold.IsEmpty() );
			}
		}
	}
}

SCENARIO( "Setting and querying cargo hold size", "[CargoHold][Size]" ) {
	GIVEN( "a CargoHold" ) {
		CargoHold hold;

		WHEN( "size is set to 100 tons" ) {
			hold.SetSize(100);
			THEN( "Size() returns 100" ) {
				CHECK( hold.Size() == 100 );
			}
			THEN( "Free() equals the size when empty" ) {
				CHECK( hold.Free() == 100 );
			}
		}

		WHEN( "size is set to 0" ) {
			hold.SetSize(0);
			THEN( "Size() returns 0" ) {
				CHECK( hold.Size() == 0 );
			}
		}

		WHEN( "size is set to unlimited (-1)" ) {
			hold.SetSize(-1);
			THEN( "Size() returns -1" ) {
				CHECK( hold.Size() == -1 );
			}
		}
	}
}

SCENARIO( "Querying used and free space", "[CargoHold][Capacity]" ) {
	GIVEN( "a CargoHold with size 100" ) {
		CargoHold hold;
		hold.SetSize(100);

		WHEN( "adding 30 tons of commodities" ) {
			hold.Add("iron", 30);
			THEN( "Used() returns 30" ) {
				CHECK( hold.Used() == 30 );
			}
			THEN( "Free() returns 70" ) {
				CHECK( hold.Free() == 70 );
			}
			THEN( "CommoditiesSize() returns 30" ) {
				CHECK( hold.CommoditiesSize() == 30 );
			}
		}

		WHEN( "adding multiple commodities" ) {
			hold.Add("iron", 20);
			hold.Add("gold", 30);
			THEN( "Used() returns total of all commodities" ) {
				CHECK( hold.Used() == 50 );
			}
			THEN( "CommoditiesSize() returns total" ) {
				CHECK( hold.CommoditiesSize() == 50 );
			}
		}
	}
}

SCENARIO( "Setting and querying bunks", "[CargoHold][Bunks]" ) {
	GIVEN( "a CargoHold" ) {
		CargoHold hold;

		WHEN( "bunks are set to 10" ) {
			hold.SetBunks(10);
			THEN( "BunksFree() returns 10" ) {
				CHECK( hold.BunksFree() == 10 );
			}
			THEN( "Passengers() returns 0" ) {
				CHECK( hold.Passengers() == 0 );
			}
		}

		WHEN( "bunks are set to unlimited (-1)" ) {
			hold.SetBunks(-1);
			THEN( "BunksFree() returns -1" ) {
				CHECK( hold.BunksFree() == -1 );
			}
		}
	}
}

SCENARIO( "Adding and removing commodities", "[CargoHold][Commodities]" ) {
	GIVEN( "a CargoHold with size 100" ) {
		CargoHold hold;
		hold.SetSize(100);

		WHEN( "adding 50 tons of gold" ) {
			int added = hold.Add("gold", 50);
			THEN( "Add() returns the amount added" ) {
				CHECK( added == 50 );
			}
			THEN( "Get() returns the correct amount" ) {
				CHECK( hold.Get("gold") == 50 );
			}
			THEN( "Free space is reduced" ) {
				CHECK( hold.Free() == 50 );
			}
		}

		WHEN( "adding more than capacity allows" ) {
			int added = hold.Add("gold", 150);
			THEN( "Add() only adds up to capacity" ) {
				CHECK( added == 100 );
			}
			THEN( "Get() returns only what was added" ) {
				CHECK( hold.Get("gold") == 100 );
			}
			THEN( "Free() returns 0" ) {
				CHECK( hold.Free() == 0 );
			}
		}

		WHEN( "removing commodity" ) {
			hold.Add("gold", 50);
			int removed = hold.Remove("gold", 20);
			THEN( "Remove() returns the amount removed" ) {
				CHECK( removed == 20 );
			}
			THEN( "Get() returns the remaining amount" ) {
				CHECK( hold.Get("gold") == 30 );
			}
		}

		WHEN( "removing more than exists" ) {
			hold.Add("gold", 30);
			int removed = hold.Remove("gold", 50);
			THEN( "Remove() only removes what exists" ) {
				CHECK( removed == 30 );
			}
			THEN( "Get() returns 0" ) {
				CHECK( hold.Get("gold") == 0 );
			}
		}

		WHEN( "getting a non-existent commodity" ) {
			THEN( "Get() returns 0" ) {
				CHECK( hold.Get("nonexistent") == 0 );
			}
		}
	}

	GIVEN( "a CargoHold with unlimited capacity" ) {
		CargoHold hold;
		// Default size is -1 (unlimited)

		WHEN( "adding large amounts" ) {
			int added = hold.Add("gold", 10000);
			THEN( "all is added" ) {
				CHECK( added == 10000 );
				CHECK( hold.Get("gold") == 10000 );
			}
		}
	}
}

SCENARIO( "Adding and removing zero-mass outfits", "[CargoHold][Outfits]" ) {
	// Note: A default-constructed Outfit has mass = 0.
	// Testing outfits with non-zero mass requires GameData to be loaded.
	GIVEN( "a CargoHold with size 100 and a zero-mass outfit" ) {
		CargoHold hold;
		hold.SetSize(100);
		Outfit zeroMassOutfit;

		WHEN( "adding zero-mass outfits" ) {
			int added = hold.Add(&zeroMassOutfit, 1000);
			THEN( "all are added (no space taken)" ) {
				CHECK( added == 1000 );
				CHECK( hold.Get(&zeroMassOutfit) == 1000 );
			}
			THEN( "OutfitsSize() is 0" ) {
				CHECK( hold.OutfitsSize() == 0 );
				CHECK( hold.OutfitsSizePrecise() == Catch::Approx(0.0) );
			}
			THEN( "Used() is 0" ) {
				CHECK( hold.Used() == 0 );
			}
			THEN( "Free() is still 100" ) {
				CHECK( hold.Free() == 100 );
			}
			THEN( "HasOutfits() returns true" ) {
				CHECK( hold.HasOutfits() );
			}
		}

		WHEN( "removing outfits" ) {
			hold.Add(&zeroMassOutfit, 10);
			int removed = hold.Remove(&zeroMassOutfit, 4);
			THEN( "Remove() returns the amount removed" ) {
				CHECK( removed == 4 );
			}
			THEN( "Get() returns the remaining count" ) {
				CHECK( hold.Get(&zeroMassOutfit) == 6 );
			}
		}

		WHEN( "removing all outfits" ) {
			hold.Add(&zeroMassOutfit, 5);
			hold.Remove(&zeroMassOutfit, 5);
			THEN( "count is zero" ) {
				CHECK( hold.Get(&zeroMassOutfit) == 0 );
			}
			THEN( "HasOutfits() returns false" ) {
				CHECK_FALSE( hold.HasOutfits() );
			}
		}

		WHEN( "removing more outfits than exist" ) {
			hold.Add(&zeroMassOutfit, 5);
			int removed = hold.Remove(&zeroMassOutfit, 10);
			THEN( "Remove() only removes what exists" ) {
				CHECK( removed == 5 );
			}
			THEN( "Get() returns 0" ) {
				CHECK( hold.Get(&zeroMassOutfit) == 0 );
			}
		}
	}

	GIVEN( "a CargoHold and multiple zero-mass outfits" ) {
		CargoHold hold;
		hold.SetSize(100);
		Outfit outfit1;
		Outfit outfit2;

		WHEN( "adding multiple outfit types" ) {
			hold.Add(&outfit1, 10);
			hold.Add(&outfit2, 3);
			THEN( "both outfits are tracked separately" ) {
				CHECK( hold.Get(&outfit1) == 10 );
				CHECK( hold.Get(&outfit2) == 3 );
			}
			THEN( "HasOutfits() returns true" ) {
				CHECK( hold.HasOutfits() );
			}
		}
	}
}

SCENARIO( "Accessing commodities and outfits maps directly", "[CargoHold][Accessors]" ) {
	GIVEN( "a CargoHold with commodities" ) {
		CargoHold hold;
		hold.SetSize(200);
		hold.Add("gold", 50);
		hold.Add("iron", 30);

		WHEN( "accessing commodities map" ) {
			const auto &commodities = hold.Commodities();
			THEN( "the map contains the expected entries" ) {
				CHECK( commodities.count("gold") == 1 );
				CHECK( commodities.count("iron") == 1 );
				CHECK( commodities.at("gold") == 50 );
				CHECK( commodities.at("iron") == 30 );
			}
		}
	}

	GIVEN( "a CargoHold with outfits" ) {
		CargoHold hold;
		Outfit outfit;
		hold.Add(&outfit, 4);

		WHEN( "accessing outfits map" ) {
			const auto &outfits = hold.Outfits();
			THEN( "the map contains the expected entries" ) {
				CHECK( outfits.count(&outfit) == 1 );
				CHECK( outfits.at(&outfit) == 4 );
			}
		}
	}
}

SCENARIO( "Checking if hold is empty", "[CargoHold][IsEmpty]" ) {
	GIVEN( "an empty CargoHold" ) {
		CargoHold hold;
		REQUIRE( hold.IsEmpty() );

		WHEN( "adding a commodity" ) {
			hold.Add("gold", 1);
			THEN( "IsEmpty() returns false" ) {
				CHECK_FALSE( hold.IsEmpty() );
			}
		}

		WHEN( "adding an outfit" ) {
			Outfit outfit;
			hold.Add(&outfit, 1);
			THEN( "IsEmpty() returns false" ) {
				CHECK_FALSE( hold.IsEmpty() );
			}
		}
	}

	GIVEN( "a CargoHold with items" ) {
		CargoHold hold;
		hold.Add("gold", 10);
		REQUIRE_FALSE( hold.IsEmpty() );

		WHEN( "removing all items" ) {
			hold.Remove("gold", 10);
			THEN( "IsEmpty() returns true" ) {
				CHECK( hold.IsEmpty() );
			}
		}
	}
}

SCENARIO( "HasOutfits behavior", "[CargoHold][HasOutfits]" ) {
	GIVEN( "an empty CargoHold" ) {
		CargoHold hold;
		REQUIRE_FALSE( hold.HasOutfits() );

		WHEN( "adding an outfit" ) {
			Outfit outfit;
			hold.Add(&outfit, 1);
			THEN( "HasOutfits() returns true" ) {
				CHECK( hold.HasOutfits() );
			}
		}
	}

	GIVEN( "a CargoHold with outfits" ) {
		CargoHold hold;
		Outfit outfit;
		hold.Add(&outfit, 5);
		REQUIRE( hold.HasOutfits() );

		WHEN( "removing all outfits" ) {
			hold.Remove(&outfit, 5);
			THEN( "HasOutfits() returns false" ) {
				CHECK_FALSE( hold.HasOutfits() );
			}
		}
	}
}

SCENARIO( "HasMissionCargo behavior", "[CargoHold][HasMissionCargo]" ) {
	GIVEN( "an empty CargoHold" ) {
		CargoHold hold;
		REQUIRE_FALSE( hold.HasMissionCargo() );
		REQUIRE( hold.MissionCargo().empty() );
	}
}

SCENARIO( "Negative Add/Remove operations", "[CargoHold][Negative]" ) {
	GIVEN( "a CargoHold with commodities" ) {
		CargoHold hold;
		hold.SetSize(100);
		hold.Add("gold", 50);

		WHEN( "calling Add with negative amount" ) {
			int result = hold.Add("gold", -10);
			THEN( "it acts like Remove" ) {
				CHECK( result == 10 );
				CHECK( hold.Get("gold") == 40 );
			}
		}

		WHEN( "calling Remove with negative amount" ) {
			int result = hold.Remove("gold", -10);
			THEN( "it acts like Add" ) {
				CHECK( result == 10 );
				CHECK( hold.Get("gold") == 60 );
			}
		}
	}

	GIVEN( "a CargoHold with outfits" ) {
		CargoHold hold;
		hold.SetSize(100);
		Outfit outfit;
		hold.Add(&outfit, 10);

		WHEN( "calling Add with negative amount" ) {
			int result = hold.Add(&outfit, -3);
			THEN( "it acts like Remove" ) {
				CHECK( result == 3 );
				CHECK( hold.Get(&outfit) == 7 );
			}
		}

		WHEN( "calling Remove with negative amount" ) {
			int result = hold.Remove(&outfit, -3);
			THEN( "it acts like Add" ) {
				CHECK( result == 3 );
				CHECK( hold.Get(&outfit) == 13 );
			}
		}
	}
}

SCENARIO( "Precise calculations with commodities only", "[CargoHold][Precise]" ) {
	GIVEN( "an empty CargoHold with size 100" ) {
		CargoHold hold;
		hold.SetSize(100);

		WHEN( "adding commodities (integer mass)" ) {
			hold.Add("gold", 50);
			THEN( "Used() and UsedPrecise() are equal" ) {
				CHECK( hold.Used() == 50 );
				CHECK( hold.UsedPrecise() == Catch::Approx(50.0) );
			}
			THEN( "Free() and FreePrecise() are equal" ) {
				CHECK( hold.Free() == 50 );
				CHECK( hold.FreePrecise() == Catch::Approx(50.0) );
			}
		}

		WHEN( "adding multiple commodities" ) {
			hold.Add("gold", 33);
			hold.Add("iron", 27);
			THEN( "Used() returns total" ) {
				CHECK( hold.Used() == 60 );
			}
		}
	}
}

SCENARIO( "Transfer operations between holds", "[CargoHold][Transfer]" ) {
	GIVEN( "two CargoHolds" ) {
		CargoHold hold1;
		CargoHold hold2;
		hold1.SetSize(100);
		hold2.SetSize(100);
		hold1.Add("gold", 50);

		WHEN( "transferring commodities to another hold" ) {
			int transferred = hold1.Transfer("gold", 30, hold2);
			THEN( "the transfer amount is returned" ) {
				CHECK( transferred == 30 );
			}
			THEN( "source hold has less" ) {
				CHECK( hold1.Get("gold") == 20 );
			}
			THEN( "destination hold has more" ) {
				CHECK( hold2.Get("gold") == 30 );
			}
		}

		WHEN( "transferring more than source has" ) {
			int transferred = hold1.Transfer("gold", 60, hold2);
			THEN( "only available amount is transferred" ) {
				CHECK( transferred == 50 );
			}
			THEN( "source hold is empty" ) {
				CHECK( hold1.Get("gold") == 0 );
			}
			THEN( "destination has all the cargo" ) {
				CHECK( hold2.Get("gold") == 50 );
			}
		}

		WHEN( "destination has limited capacity" ) {
			hold2.SetSize(30);  // Only room for 30 tons
			int transferred = hold1.Transfer("gold", 50, hold2);
			THEN( "only what fits is transferred" ) {
				CHECK( transferred == 30 );
			}
			THEN( "source retains the rest" ) {
				CHECK( hold1.Get("gold") == 20 );
			}
		}
	}

	GIVEN( "two CargoHolds with zero-mass outfits" ) {
		CargoHold hold1;
		CargoHold hold2;
		hold1.SetSize(100);
		hold2.SetSize(100);
		Outfit outfit;
		hold1.Add(&outfit, 10);

		WHEN( "transferring outfits" ) {
			int transferred = hold1.Transfer(&outfit, 4, hold2);
			THEN( "the transfer succeeds" ) {
				CHECK( transferred == 4 );
				CHECK( hold1.Get(&outfit) == 6 );
				CHECK( hold2.Get(&outfit) == 4 );
			}
		}
	}
}

SCENARIO( "Default capacity behavior", "[CargoHold][Unlimited]" ) {
	GIVEN( "a default-constructed CargoHold (unlimited capacity)" ) {
		CargoHold hold;
		REQUIRE( hold.Size() == -1 );

		WHEN( "adding large amounts of commodities" ) {
			int added = hold.Add("gold", 1000000);
			THEN( "all is accepted" ) {
				CHECK( added == 1000000 );
			}
		}

		WHEN( "checking free space" ) {
			hold.Add("gold", 100);
			THEN( "Free() returns a large negative number (unlimited minus used)" ) {
				// Free = size - Used = -1 - 100 = -101
				CHECK( hold.Free() == -101 );
			}
		}
	}
}

SCENARIO( "Get for mission cargo and passengers", "[CargoHold][Mission]" ) {
	GIVEN( "an empty CargoHold" ) {
		CargoHold hold;

		WHEN( "querying non-existent mission cargo" ) {
			THEN( "Get() returns 0 for null mission" ) {
				CHECK( hold.Get(static_cast<const Mission*>(nullptr)) == 0 );
			}
		}

		WHEN( "querying non-existent passengers" ) {
			THEN( "GetPassengers() returns 0 for null mission" ) {
				CHECK( hold.GetPassengers(static_cast<const Mission*>(nullptr)) == 0 );
			}
		}
	}
}

SCENARIO( "TransferAll operation", "[CargoHold][TransferAll]" ) {
	GIVEN( "two CargoHolds with commodities and outfits" ) {
		CargoHold hold1;
		CargoHold hold2;
		hold1.SetSize(100);
		hold2.SetSize(100);
		hold1.Add("gold", 30);
		hold1.Add("iron", 20);
		Outfit outfit;
		hold1.Add(&outfit, 5);

		WHEN( "TransferAll is called" ) {
			hold1.TransferAll(hold2);
			THEN( "all commodities are transferred" ) {
				CHECK( hold1.Get("gold") == 0 );
				CHECK( hold1.Get("iron") == 0 );
				CHECK( hold2.Get("gold") == 30 );
				CHECK( hold2.Get("iron") == 20 );
			}
			THEN( "all outfits are transferred" ) {
				CHECK( hold1.Get(&outfit) == 0 );
				CHECK( hold2.Get(&outfit) == 5 );
			}
			THEN( "source is empty" ) {
				CHECK( hold1.IsEmpty() );
			}
		}
	}

	GIVEN( "two CargoHolds with limited destination capacity" ) {
		CargoHold hold1;
		CargoHold hold2;
		hold1.SetSize(100);
		hold2.SetSize(40);  // Limited capacity
		hold1.Add("gold", 50);

		WHEN( "TransferAll is called" ) {
			hold1.TransferAll(hold2);
			THEN( "only what fits is transferred" ) {
				CHECK( hold2.Get("gold") == 40 );
			}
			THEN( "source retains the overflow" ) {
				CHECK( hold1.Get("gold") == 10 );
			}
		}
	}
}

SCENARIO( "Multiple commodity types", "[CargoHold][MultipleCommodities]" ) {
	GIVEN( "a CargoHold with multiple commodity types" ) {
		CargoHold hold;
		hold.SetSize(200);
		hold.Add("gold", 50);
		hold.Add("iron", 30);
		hold.Add("food", 20);

		WHEN( "querying total used" ) {
			THEN( "Used() returns total of all commodities" ) {
				CHECK( hold.Used() == 100 );
			}
		}

		WHEN( "querying individual commodities" ) {
			THEN( "each commodity has correct amount" ) {
				CHECK( hold.Get("gold") == 50 );
				CHECK( hold.Get("iron") == 30 );
				CHECK( hold.Get("food") == 20 );
			}
		}

		WHEN( "removing from one commodity type" ) {
			hold.Remove("iron", 10);
			THEN( "only that commodity is affected" ) {
				CHECK( hold.Get("iron") == 20 );
				CHECK( hold.Get("gold") == 50 );
				CHECK( hold.Get("food") == 20 );
			}
		}
	}
}

// #endregion unit tests



} // test namespace