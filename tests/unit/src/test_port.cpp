/* test_port.cpp
Copyright (c) 2025 by test author

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
#include "../../../source/Port.h"

// Include a helper for creating well-formed DataNodes.
#include "datanode-factory.h"

// ... and any system includes needed for the test file.
#include <string>

namespace { // test namespace

// #region mock data

// #endregion mock data



// #region unit tests

// Test the RechargeType enum values.
SCENARIO( "Port::RechargeType enum values", "[Port][RechargeType]" ) {
	GIVEN( "the RechargeType enum" ) {
		THEN( "individual types are defined correctly" ) {
			CHECK( Port::RechargeType::None == 0 );
			CHECK( Port::RechargeType::Shields == (1 << 0) );
			CHECK( Port::RechargeType::Hull == (1 << 1) );
			CHECK( Port::RechargeType::Energy == (1 << 2) );
			CHECK( Port::RechargeType::Fuel == (1 << 3) );
		}
		THEN( "the All type is a combination of all individual types" ) {
			CHECK( Port::RechargeType::All == (Port::RechargeType::Shields
				| Port::RechargeType::Hull
				| Port::RechargeType::Energy
				| Port::RechargeType::Fuel) );
		}
		THEN( "types can be combined using bitwise OR" ) {
			int combined = Port::RechargeType::Shields | Port::RechargeType::Energy;
			CHECK( (combined & Port::RechargeType::Shields) != 0 );
			CHECK( (combined & Port::RechargeType::Energy) != 0 );
			CHECK( (combined & Port::RechargeType::Hull) == 0 );
			CHECK( (combined & Port::RechargeType::Fuel) == 0 );
		}
	}
}

// Test the ServicesType enum values.
SCENARIO( "Port::ServicesType enum values", "[Port][ServicesType]" ) {
	GIVEN( "the ServicesType enum" ) {
		THEN( "individual types are defined correctly" ) {
			CHECK( Port::ServicesType::None == 0 );
			CHECK( Port::ServicesType::Trading == (1 << 0) );
			CHECK( Port::ServicesType::JobBoard == (1 << 1) );
			CHECK( Port::ServicesType::Bank == (1 << 2) );
			CHECK( Port::ServicesType::HireCrew == (1 << 3) );
			CHECK( Port::ServicesType::OffersMissions == (1 << 4) );
		}
		THEN( "the All type is a combination of all individual types" ) {
			CHECK( Port::ServicesType::All == (Port::ServicesType::Trading
				| Port::ServicesType::JobBoard
				| Port::ServicesType::Bank
				| Port::ServicesType::HireCrew
				| Port::ServicesType::OffersMissions) );
		}
		THEN( "types can be combined using bitwise OR" ) {
			int combined = Port::ServicesType::Trading | Port::ServicesType::Bank;
			CHECK( (combined & Port::ServicesType::Trading) != 0 );
			CHECK( (combined & Port::ServicesType::Bank) != 0 );
			CHECK( (combined & Port::ServicesType::JobBoard) == 0 );
			CHECK( (combined & Port::ServicesType::HireCrew) == 0 );
			CHECK( (combined & Port::ServicesType::OffersMissions) == 0 );
		}
	}
}

// Test default construction.
SCENARIO( "Creating a default Port", "[Port][Creation]" ) {
	GIVEN( "a default constructed Port" ) {
		Port port;
		THEN( "default values are set correctly" ) {
			CHECK( port.CustomLoaded() == false );
			CHECK( port.DisplayName().empty() );
			CHECK( port.HasNews() == false );
		}
		THEN( "no recharge types are available" ) {
			CHECK( port.GetRecharges(false) == Port::RechargeType::None );
		}
		THEN( "no services are available" ) {
			CHECK( port.HasServices(false) == Port::ServicesType::None );
		}
		THEN( "access is allowed by default" ) {
			CHECK( port.CanAccess() == true );
		}
		THEN( "no bribe is required by default" ) {
			CHECK( port.RequiresBribe() == false );
		}
	}
}

// Test LoadDefaultSpaceport.
SCENARIO( "Loading a default spaceport", "[Port][Creation]" ) {
	GIVEN( "a default constructed Port" ) {
		Port port;
		WHEN( "LoadDefaultSpaceport is called" ) {
			port.LoadDefaultSpaceport();
			THEN( "the port has the default spaceport name" ) {
				CHECK( port.DisplayName() == "Space_port" );
			}
			THEN( "all recharge types are available for non-players" ) {
				CHECK( port.GetRecharges(false) == Port::RechargeType::All );
			}
			THEN( "all recharge types are available for players" ) {
				CHECK( port.GetRecharges(true) == Port::RechargeType::All );
			}
			THEN( "all services are available for non-players" ) {
				CHECK( port.HasServices(false) == Port::ServicesType::All );
			}
			THEN( "all services are available for players" ) {
				CHECK( port.HasServices(true) == true );
			}
			THEN( "news is available" ) {
				CHECK( port.HasNews() == true );
			}
			THEN( "access is allowed" ) {
				CHECK( port.CanAccess() == true );
			}
			THEN( "each individual recharge type can be queried" ) {
				CHECK( port.CanRecharge(Port::RechargeType::Shields, false) == true );
				CHECK( port.CanRecharge(Port::RechargeType::Hull, false) == true );
				CHECK( port.CanRecharge(Port::RechargeType::Energy, false) == true );
				CHECK( port.CanRecharge(Port::RechargeType::Fuel, false) == true );
				CHECK( port.CanRecharge(Port::RechargeType::All, false) == true );
			}
			THEN( "each individual service can be queried" ) {
				CHECK( port.HasService(Port::ServicesType::Trading, false) == true );
				CHECK( port.HasService(Port::ServicesType::JobBoard, false) == true );
				CHECK( port.HasService(Port::ServicesType::Bank, false) == true );
				CHECK( port.HasService(Port::ServicesType::HireCrew, false) == true );
				CHECK( port.HasService(Port::ServicesType::OffersMissions, false) == true );
				CHECK( port.HasService(Port::ServicesType::All, false) == true );
			}
		}
	}
}

// Test LoadUninhabitedSpaceport.
SCENARIO( "Loading an uninhabited spaceport", "[Port][Creation]" ) {
	GIVEN( "a default constructed Port" ) {
		Port port;
		WHEN( "LoadUninhabitedSpaceport is called" ) {
			port.LoadUninhabitedSpaceport();
			THEN( "the port has the default spaceport name" ) {
				CHECK( port.DisplayName() == "Space_port" );
			}
			THEN( "all recharge types are available for non-players" ) {
				CHECK( port.GetRecharges(false) == Port::RechargeType::All );
			}
			THEN( "all recharge types are available for players" ) {
				CHECK( port.GetRecharges(true) == Port::RechargeType::All );
			}
			THEN( "only OffersMissions service is available for non-players" ) {
				CHECK( port.HasServices(false) == Port::ServicesType::OffersMissions );
			}
			THEN( "services are limited for players" ) {
				CHECK( port.HasService(Port::ServicesType::Trading, true) == false );
				CHECK( port.HasService(Port::ServicesType::JobBoard, true) == false );
				CHECK( port.HasService(Port::ServicesType::Bank, true) == false );
				CHECK( port.HasService(Port::ServicesType::HireCrew, true) == false );
				CHECK( port.HasService(Port::ServicesType::OffersMissions, true) == true );
			}
			THEN( "news is available" ) {
				CHECK( port.HasNews() == true );
			}
			THEN( "access is allowed" ) {
				CHECK( port.CanAccess() == true );
			}
		}
	}
}

// Test Load with various configurations.
SCENARIO( "Loading a Port from DataNode", "[Port][Creation][Loading]" ) {
	GIVEN( "a default constructed Port" ) {
		Port port;
		WHEN( "loading a basic port with a name" ) {
			auto node = AsDataNode("port \"My Port\"");
			port.Load(node, nullptr);
			THEN( "the port is marked as custom loaded" ) {
				CHECK( port.CustomLoaded() == true );
			}
			THEN( "the display name is set" ) {
				CHECK( port.DisplayName() == "My Port" );
			}
		}
		WHEN( "loading a port with recharge types" ) {
			auto node = AsDataNode("port\n\trecharges shields hull");
			port.Load(node, nullptr);
			THEN( "only specified recharge types are available" ) {
				CHECK( port.CanRecharge(Port::RechargeType::Shields, false) == true );
				CHECK( port.CanRecharge(Port::RechargeType::Hull, false) == true );
				CHECK( port.CanRecharge(Port::RechargeType::Energy, false) == false );
				CHECK( port.CanRecharge(Port::RechargeType::Fuel, false) == false );
			}
		}
		WHEN( "loading a port with all recharge types" ) {
			auto node = AsDataNode("port\n\trecharges all");
			port.Load(node, nullptr);
			THEN( "all recharge types are available" ) {
				CHECK( port.CanRecharge(Port::RechargeType::Shields, false) == true );
				CHECK( port.CanRecharge(Port::RechargeType::Hull, false) == true );
				CHECK( port.CanRecharge(Port::RechargeType::Energy, false) == true );
				CHECK( port.CanRecharge(Port::RechargeType::Fuel, false) == true );
				CHECK( (port.GetRecharges(false) & Port::RechargeType::All) == Port::RechargeType::All );
			}
		}
		WHEN( "loading a port with services" ) {
			auto node = AsDataNode("port\n\tservices trading bank");
			port.Load(node, nullptr);
			THEN( "only specified services are available" ) {
				CHECK( port.HasService(Port::ServicesType::Trading, false) == true );
				CHECK( port.HasService(Port::ServicesType::Bank, false) == true );
				CHECK( port.HasService(Port::ServicesType::JobBoard, false) == false );
				CHECK( port.HasService(Port::ServicesType::HireCrew, false) == false );
				CHECK( port.HasService(Port::ServicesType::OffersMissions, false) == false );
			}
		}
		WHEN( "loading a port with all services" ) {
			auto node = AsDataNode("port\n\tservices all");
			port.Load(node, nullptr);
			THEN( "all services are available" ) {
				CHECK( port.HasService(Port::ServicesType::Trading, false) == true );
				CHECK( port.HasService(Port::ServicesType::JobBoard, false) == true );
				CHECK( port.HasService(Port::ServicesType::Bank, false) == true );
				CHECK( port.HasService(Port::ServicesType::HireCrew, false) == true );
				CHECK( port.HasService(Port::ServicesType::OffersMissions, false) == true );
			}
		}
		WHEN( "loading a port with news" ) {
			auto node = AsDataNode("port\n\tnews");
			port.Load(node, nullptr);
			THEN( "news is available" ) {
				CHECK( port.HasNews() == true );
			}
		}
		WHEN( "loading a port without news" ) {
			auto node = AsDataNode("port");
			port.Load(node, nullptr);
			THEN( "news is not available" ) {
				CHECK( port.HasNews() == false );
			}
		}
		WHEN( "loading a port with description" ) {
			auto node = AsDataNode("port\n\tdescription \"This is a test port.\"");
			port.Load(node, nullptr);
			THEN( "the default name is set when description is provided without name" ) {
				CHECK( port.DisplayName() == "Space_port" );
			}
		}
		WHEN( "loading a port with name and description" ) {
			auto node = AsDataNode("port \"Custom Port\"\n\tdescription \"This is a custom port.\"");
			port.Load(node, nullptr);
			THEN( "the custom name is used" ) {
				CHECK( port.DisplayName() == "Custom Port" );
			}
		}
		WHEN( "loading a port with recharges in child nodes" ) {
			auto node = AsDataNode("port\n\trecharges\n\t\tshields\n\t\tenergy");
			port.Load(node, nullptr);
			THEN( "recharge types from child nodes are set" ) {
				CHECK( port.CanRecharge(Port::RechargeType::Shields, false) == true );
				CHECK( port.CanRecharge(Port::RechargeType::Energy, false) == true );
				CHECK( port.CanRecharge(Port::RechargeType::Hull, false) == false );
				CHECK( port.CanRecharge(Port::RechargeType::Fuel, false) == false );
			}
		}
		WHEN( "loading a port with services in child nodes" ) {
			auto node = AsDataNode("port\n\tservices\n\t\ttrading\n\t\tjob board");
			port.Load(node, nullptr);
			THEN( "services from child nodes are set" ) {
				CHECK( port.HasService(Port::ServicesType::Trading, false) == true );
				CHECK( port.HasService(Port::ServicesType::JobBoard, false) == true );
				CHECK( port.HasService(Port::ServicesType::Bank, false) == false );
			}
		}
	}
}

// Test CanRecharge for player vs non-player.
SCENARIO( "CanRecharge behavior for player vs non-player", "[Port][Recharge]" ) {
	GIVEN( "a port with all recharges" ) {
		Port port;
		port.LoadDefaultSpaceport();
		WHEN( "querying recharge for non-player" ) {
			THEN( "all recharge types are available" ) {
				CHECK( port.CanRecharge(Port::RechargeType::Shields, false) == true );
				CHECK( port.CanRecharge(Port::RechargeType::Hull, false) == true );
				CHECK( port.CanRecharge(Port::RechargeType::Energy, false) == true );
				CHECK( port.CanRecharge(Port::RechargeType::Fuel, false) == true );
			}
		}
		WHEN( "querying recharge for player" ) {
			THEN( "all recharge types are available (no access restrictions)" ) {
				CHECK( port.CanRecharge(Port::RechargeType::Shields, true) == true );
				CHECK( port.CanRecharge(Port::RechargeType::Hull, true) == true );
				CHECK( port.CanRecharge(Port::RechargeType::Energy, true) == true );
				CHECK( port.CanRecharge(Port::RechargeType::Fuel, true) == true );
			}
		}
	}
	GIVEN( "a port with only shields recharge" ) {
		auto node = AsDataNode("port\n\trecharges shields");
		Port port;
		port.Load(node, nullptr);
		WHEN( "querying recharge types" ) {
			THEN( "only shields can be recharged" ) {
				CHECK( port.CanRecharge(Port::RechargeType::Shields, false) == true );
				CHECK( port.CanRecharge(Port::RechargeType::Hull, false) == false );
				CHECK( port.CanRecharge(Port::RechargeType::Energy, false) == false );
				CHECK( port.CanRecharge(Port::RechargeType::Fuel, false) == false );
			}
		}
	}
}

// Test HasService for player vs non-player.
SCENARIO( "HasService behavior for player vs non-player", "[Port][Services]" ) {
	GIVEN( "a port with all services" ) {
		Port port;
		port.LoadDefaultSpaceport();
		WHEN( "querying services for non-player" ) {
			THEN( "all services are available" ) {
				CHECK( port.HasService(Port::ServicesType::Trading, false) == true );
				CHECK( port.HasService(Port::ServicesType::JobBoard, false) == true );
				CHECK( port.HasService(Port::ServicesType::Bank, false) == true );
				CHECK( port.HasService(Port::ServicesType::HireCrew, false) == true );
				CHECK( port.HasService(Port::ServicesType::OffersMissions, false) == true );
			}
		}
		WHEN( "querying services for player" ) {
			THEN( "all services are available (no access restrictions)" ) {
				CHECK( port.HasService(Port::ServicesType::Trading, true) == true );
				CHECK( port.HasService(Port::ServicesType::JobBoard, true) == true );
				CHECK( port.HasService(Port::ServicesType::Bank, true) == true );
				CHECK( port.HasService(Port::ServicesType::HireCrew, true) == true );
				CHECK( port.HasService(Port::ServicesType::OffersMissions, true) == true );
			}
		}
	}
	GIVEN( "a port with only trading service" ) {
		auto node = AsDataNode("port\n\tservices trading");
		Port port;
		port.Load(node, nullptr);
		WHEN( "querying services" ) {
			THEN( "only trading is available" ) {
				CHECK( port.HasService(Port::ServicesType::Trading, false) == true );
				CHECK( port.HasService(Port::ServicesType::JobBoard, false) == false );
				CHECK( port.HasService(Port::ServicesType::Bank, false) == false );
				CHECK( port.HasService(Port::ServicesType::HireCrew, false) == false );
				CHECK( port.HasService(Port::ServicesType::OffersMissions, false) == false );
			}
		}
	}
}

// Test GetRecharges.
SCENARIO( "GetRecharges returns combined recharge types", "[Port][Recharge]" ) {
	GIVEN( "a port with specific recharge types" ) {
		auto node = AsDataNode("port\n\trecharges shields fuel");
		Port port;
		port.Load(node, nullptr);
		WHEN( "getting recharges for non-player" ) {
			int recharges = port.GetRecharges(false);
			THEN( "the returned value contains only the set types" ) {
				CHECK( (recharges & Port::RechargeType::Shields) != 0 );
				CHECK( (recharges & Port::RechargeType::Fuel) != 0 );
				CHECK( (recharges & Port::RechargeType::Hull) == 0 );
				CHECK( (recharges & Port::RechargeType::Energy) == 0 );
			}
		}
	}
	GIVEN( "a port with no recharge types" ) {
		Port port;
		WHEN( "getting recharges" ) {
			THEN( "no recharge types are returned" ) {
				CHECK( port.GetRecharges(false) == Port::RechargeType::None );
				CHECK( port.GetRecharges(true) == Port::RechargeType::None );
			}
		}
	}
}

// Test HasServices.
SCENARIO( "HasServices returns combined services", "[Port][Services]" ) {
	GIVEN( "a port with specific services" ) {
		auto node = AsDataNode("port\n\tservices trading bank");
		Port port;
		port.Load(node, nullptr);
		WHEN( "getting services for non-player" ) {
			int services = port.HasServices(false);
			THEN( "the returned value contains only the set services" ) {
				CHECK( (services & Port::ServicesType::Trading) != 0 );
				CHECK( (services & Port::ServicesType::Bank) != 0 );
				CHECK( (services & Port::ServicesType::JobBoard) == 0 );
				CHECK( (services & Port::ServicesType::HireCrew) == 0 );
				CHECK( (services & Port::ServicesType::OffersMissions) == 0 );
			}
		}
	}
	GIVEN( "a port with no services" ) {
		Port port;
		WHEN( "getting services" ) {
			THEN( "no services are returned for non-player" ) {
				CHECK( port.HasServices(false) == Port::ServicesType::None );
			}
			THEN( "HasServices for player returns false when no services" ) {
				CHECK( port.HasServices(true) == false );
			}
		}
	}
}

// Test CanAccess with condition sets.
SCENARIO( "CanAccess with conditions", "[Port][Access]" ) {
	GIVEN( "a default port" ) {
		Port port;
		WHEN( "no access conditions are set" ) {
			THEN( "access is allowed" ) {
				CHECK( port.CanAccess() == true );
			}
		}
	}
	GIVEN( "a port with access condition that always passes" ) {
		auto node = AsDataNode("port\n\tto access\n\t\tnever");
		Port port;
		port.Load(node, nullptr);
		WHEN( "checking access" ) {
			THEN( "access is denied because never evaluates to false" ) {
				CHECK( port.CanAccess() == false );
			}
		}
	}
}

// Test RequiresBribe.
SCENARIO( "RequiresBribe behavior", "[Port][Bribe]" ) {
	GIVEN( "a default port" ) {
		Port port;
		WHEN( "no bribe condition is set" ) {
			THEN( "no bribe is required" ) {
				CHECK( port.RequiresBribe() == false );
			}
		}
	}
	GIVEN( "a port with bribe condition set to never" ) {
		auto node = AsDataNode("port\n\tto bribe\n\t\tnever");
		Port port;
		port.Load(node, nullptr);
		WHEN( "checking bribe requirement" ) {
			THEN( "bribe is not required because never is false" ) {
				CHECK( port.RequiresBribe() == false );
			}
		}
	}
}

// Test CustomLoaded.
SCENARIO( "CustomLoaded indicates if port was loaded from data", "[Port][State]" ) {
	GIVEN( "a default constructed Port" ) {
		Port port;
		WHEN( "not loaded from data" ) {
			THEN( "CustomLoaded returns false" ) {
				CHECK( port.CustomLoaded() == false );
			}
		}
		WHEN( "LoadDefaultSpaceport is called" ) {
			port.LoadDefaultSpaceport();
			THEN( "CustomLoaded still returns false" ) {
				CHECK( port.CustomLoaded() == false );
			}
		}
		WHEN( "LoadUninhabitedSpaceport is called" ) {
			port.LoadUninhabitedSpaceport();
			THEN( "CustomLoaded still returns false" ) {
				CHECK( port.CustomLoaded() == false );
			}
		}
		WHEN( "Load is called with a DataNode" ) {
			auto node = AsDataNode("port \"Test\"");
			port.Load(node, nullptr);
			THEN( "CustomLoaded returns true" ) {
				CHECK( port.CustomLoaded() == true );
			}
		}
	}
}

// Test Description.
SCENARIO( "Description returns the port's description", "[Port][Description]" ) {
	GIVEN( "a default constructed Port" ) {
		Port port;
		WHEN( "no description is set" ) {
			THEN( "description is empty" ) {
				CHECK( port.Description().IsEmpty() );
			}
		}
	}
}

// Test "add" keyword in port definition.
SCENARIO( "Loading a Port with 'add' keyword", "[Port][Loading]" ) {
	GIVEN( "a default constructed Port" ) {
		Port port;
		WHEN( "loading a port with 'add' keyword" ) {
			auto node = AsDataNode("add port \"Added Port\"");
			port.Load(node, nullptr);
			THEN( "the port is marked as custom loaded" ) {
				CHECK( port.CustomLoaded() == true );
			}
			THEN( "the display name is set correctly" ) {
				CHECK( port.DisplayName() == "Added Port" );
			}
		}
	}
}

// Test multiple recharges with combined values.
SCENARIO( "Combining multiple recharge types", "[Port][Recharge]" ) {
	GIVEN( "a port with multiple recharge types" ) {
		auto node = AsDataNode("port\n\trecharges shields\n\trecharges energy fuel");
		Port port;
		port.Load(node, nullptr);
		WHEN( "checking available recharges" ) {
			THEN( "all specified types are available" ) {
				CHECK( port.CanRecharge(Port::RechargeType::Shields, false) == true );
				CHECK( port.CanRecharge(Port::RechargeType::Energy, false) == true );
				CHECK( port.CanRecharge(Port::RechargeType::Fuel, false) == true );
				CHECK( port.CanRecharge(Port::RechargeType::Hull, false) == false );
			}
		}
	}
}

// Test multiple services with combined values.
SCENARIO( "Combining multiple service types", "[Port][Services]" ) {
	GIVEN( "a port with multiple service types" ) {
		auto node = AsDataNode("port\n\tservices trading\n\tservices job board hire crew");
		Port port;
		port.Load(node, nullptr);
		WHEN( "checking available services" ) {
			THEN( "all specified services are available" ) {
				CHECK( port.HasService(Port::ServicesType::Trading, false) == true );
				CHECK( port.HasService(Port::ServicesType::JobBoard, false) == true );
				CHECK( port.HasService(Port::ServicesType::HireCrew, false) == true );
				CHECK( port.HasService(Port::ServicesType::Bank, false) == false );
				CHECK( port.HasService(Port::ServicesType::OffersMissions, false) == false );
			}
		}
	}
}

// Test bitwise operations on RechargeType.
SCENARIO( "RechargeType bitwise operations", "[Port][RechargeType]" ) {
	GIVEN( "various RechargeType combinations" ) {
		WHEN( "combining types with OR" ) {
			int combined = Port::RechargeType::Shields | Port::RechargeType::Hull;
			THEN( "individual bits are set correctly" ) {
				CHECK( (combined & Port::RechargeType::Shields) != 0 );
				CHECK( (combined & Port::RechargeType::Hull) != 0 );
				CHECK( (combined & Port::RechargeType::Energy) == 0 );
				CHECK( (combined & Port::RechargeType::Fuel) == 0 );
			}
		}
		WHEN( "using All value" ) {
			THEN( "All contains all types" ) {
				CHECK( (Port::RechargeType::All & Port::RechargeType::Shields) != 0 );
				CHECK( (Port::RechargeType::All & Port::RechargeType::Hull) != 0 );
				CHECK( (Port::RechargeType::All & Port::RechargeType::Energy) != 0 );
				CHECK( (Port::RechargeType::All & Port::RechargeType::Fuel) != 0 );
			}
		}
	}
}

// Test bitwise operations on ServicesType.
SCENARIO( "ServicesType bitwise operations", "[Port][ServicesType]" ) {
	GIVEN( "various ServicesType combinations" ) {
		WHEN( "combining types with OR" ) {
			int combined = Port::ServicesType::Trading | Port::ServicesType::JobBoard;
			THEN( "individual bits are set correctly" ) {
				CHECK( (combined & Port::ServicesType::Trading) != 0 );
				CHECK( (combined & Port::ServicesType::JobBoard) != 0 );
				CHECK( (combined & Port::ServicesType::Bank) == 0 );
				CHECK( (combined & Port::ServicesType::HireCrew) == 0 );
				CHECK( (combined & Port::ServicesType::OffersMissions) == 0 );
			}
		}
		WHEN( "using All value" ) {
			THEN( "All contains all services" ) {
				CHECK( (Port::ServicesType::All & Port::ServicesType::Trading) != 0 );
				CHECK( (Port::ServicesType::All & Port::ServicesType::JobBoard) != 0 );
				CHECK( (Port::ServicesType::All & Port::ServicesType::Bank) != 0 );
				CHECK( (Port::ServicesType::All & Port::ServicesType::HireCrew) != 0 );
				CHECK( (Port::ServicesType::All & Port::ServicesType::OffersMissions) != 0 );
			}
		}
	}
}

// #endregion unit tests



} // test namespace