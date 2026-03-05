/* test_ui.cpp
Copyright (c) 2025 by Benjamin Hauch

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
#include "../../../source/UI.h"

// ... and any system includes needed for the test file.
#include "../../../source/Panel.h"
#include <memory>

namespace { // test namespace

// #region mock data

// A minimal mock Panel for testing UI.
// It implements the pure virtual Draw() method and provides
// tracking for step counts and other testable state.
class MockPanel : public Panel {
public:
	MockPanel() = default;
	explicit MockPanel(bool fullScreen) { SetIsFullScreen(fullScreen); }

	// Implement the pure virtual Draw method.
	void Draw() override {}

	// Track Step() calls for testing.
	void Step() override { stepCount++; }
	int GetStepCount() const { return stepCount; }

	// Track key events.
	bool KeyDown(SDL_Keycode key, Uint16 mod, const Command &command, bool isNewPress) override {
		keyCount++;
		lastKey = key;
		return true;
	}
	int GetKeyCount() const { return keyCount; }
	SDL_Keycode GetLastKey() const { return lastKey; }

	// Track click events.
	bool Click(int x, int y, MouseButton button, int clicks) override {
		clickCount++;
		return true;
	}
	int GetClickCount() const { return clickCount; }

private:
	int stepCount = 0;
	int keyCount = 0;
	int clickCount = 0;
	SDL_Keycode lastKey = SDLK_UNKNOWN;
};

// #endregion mock data



// #region unit tests

SCENARIO("Creating a UI", "[UI][Creation]") {
	GIVEN("a default constructed UI") {
		UI ui;

		THEN("the UI is created successfully") {
			REQUIRE(true);
		}

		THEN("the stack is empty") {
			CHECK(ui.Stack().empty());
		}

		THEN("IsEmpty returns true") {
			CHECK(ui.IsEmpty());
		}

		THEN("IsDone returns false") {
			CHECK_FALSE(ui.IsDone());
		}

		THEN("CanSave returns false") {
			CHECK_FALSE(ui.CanSave());
		}

		THEN("Top returns nullptr") {
			CHECK(ui.Top() == nullptr);
		}

		THEN("Root returns nullptr") {
			CHECK(ui.Root() == nullptr);
		}
	}
}

SCENARIO("UI panel stack management", "[UI][Stack]") {
	GIVEN("an empty UI") {
		UI ui;

		WHEN("a panel is pushed with raw pointer") {
			auto *rawPanel = new MockPanel();
			ui.Push(rawPanel);

			THEN("the panel is queued to be pushed") {
				CHECK_FALSE(ui.IsEmpty());
			}

			AND_WHEN("StepAll is called") {
				ui.StepAll();

				THEN("the panel is in the stack") {
					REQUIRE(ui.Stack().size() == 1);
					CHECK(ui.Stack().back().get() == rawPanel);
				}

				THEN("IsTop returns true for the pushed panel") {
					CHECK(ui.IsTop(rawPanel));
				}

				THEN("Top returns the panel") {
					CHECK(ui.Top().get() == rawPanel);
				}

				THEN("Root returns the panel") {
					CHECK(ui.Root().get() == rawPanel);
				}
			}
		}

		WHEN("a panel is pushed with shared_ptr") {
			auto panel = std::make_shared<MockPanel>();
			ui.Push(panel);

			THEN("the panel is queued to be pushed") {
				CHECK_FALSE(ui.IsEmpty());
			}

			AND_WHEN("StepAll is called") {
				ui.StepAll();

				THEN("the panel is in the stack") {
					REQUIRE(ui.Stack().size() == 1);
					CHECK(ui.Stack().back() == panel);
				}
			}
		}
	}
}

SCENARIO("Pushing multiple panels", "[UI][Stack]") {
	GIVEN("a UI with one panel") {
		UI ui;
		auto panel1 = std::make_shared<MockPanel>();
		ui.Push(panel1);
		ui.StepAll();

		WHEN("a second panel is pushed") {
			auto panel2 = std::make_shared<MockPanel>();
			ui.Push(panel2);
			ui.StepAll();

			THEN("both panels are in the stack") {
				REQUIRE(ui.Stack().size() == 2);
				CHECK(ui.Stack()[0] == panel1);
				CHECK(ui.Stack()[1] == panel2);
			}

			THEN("the second panel is on top") {
				CHECK(ui.IsTop(panel2.get()));
				CHECK_FALSE(ui.IsTop(panel1.get()));
			}

			THEN("Top returns the second panel") {
				CHECK(ui.Top() == panel2);
			}

			THEN("Root returns the first panel") {
				CHECK(ui.Root() == panel1);
			}
		}
	}
}

SCENARIO("Popping panels", "[UI][Stack][Pop]") {
	GIVEN("a UI with two panels") {
		UI ui;
		auto panel1 = std::make_shared<MockPanel>();
		auto panel2 = std::make_shared<MockPanel>();
		ui.Push(panel1);
		ui.Push(panel2);
		ui.StepAll();

		WHEN("the top panel is popped") {
			ui.Pop(panel2.get());
			ui.StepAll();

			THEN("only the bottom panel remains") {
				REQUIRE(ui.Stack().size() == 1);
				CHECK(ui.Stack().back() == panel1);
			}

			THEN("the bottom panel becomes the top") {
				CHECK(ui.IsTop(panel1.get()));
			}
		}

		WHEN("a non-top panel is popped") {
			ui.Pop(panel1.get());
			ui.StepAll();

			THEN("only the top panel remains") {
				REQUIRE(ui.Stack().size() == 1);
				CHECK(ui.Stack().back() == panel2);
			}
		}
	}
}

SCENARIO("PopThrough operation", "[UI][Stack][PopThrough]") {
	GIVEN("a UI with three panels") {
		UI ui;
		auto panel1 = std::make_shared<MockPanel>();
		auto panel2 = std::make_shared<MockPanel>();
		auto panel3 = std::make_shared<MockPanel>();
		ui.Push(panel1);
		ui.Push(panel2);
		ui.Push(panel3);
		ui.StepAll();

		WHEN("PopThrough is called on the middle panel") {
			ui.PopThrough(panel2.get());
			ui.StepAll();

			THEN("the middle and top panels are removed") {
				REQUIRE(ui.Stack().size() == 1);
				CHECK(ui.Stack().back() == panel1);
			}
		}

		WHEN("PopThrough is called on the bottom panel") {
			ui.PopThrough(panel1.get());
			ui.StepAll();

			THEN("all panels are removed") {
				CHECK(ui.Stack().empty());
				CHECK(ui.IsEmpty());
			}
		}

		WHEN("PopThrough is called on the top panel") {
			ui.PopThrough(panel3.get());
			ui.StepAll();

			THEN("only the top panel is removed") {
				REQUIRE(ui.Stack().size() == 2);
				CHECK(ui.Stack()[0] == panel1);
				CHECK(ui.Stack()[1] == panel2);
			}
		}
	}
}

SCENARIO("UI Reset operation", "[UI][Reset]") {
	GIVEN("a UI with panels and isDone flag set") {
		UI ui;
		ui.Push(std::make_shared<MockPanel>());
		ui.StepAll();
		ui.Quit();
		ui.CanSave(true);

		REQUIRE_FALSE(ui.Stack().empty());
		REQUIRE(ui.IsDone());
		REQUIRE(ui.CanSave());

		WHEN("Reset is called") {
			ui.Reset();

			THEN("the stack is cleared") {
				CHECK(ui.Stack().empty());
			}

			THEN("IsEmpty returns true") {
				CHECK(ui.IsEmpty());
			}

			THEN("isDone is cleared") {
				CHECK_FALSE(ui.IsDone());
			}

			THEN("canSave is cleared") {
				CHECK_FALSE(ui.CanSave());
			}
		}
	}
}

SCENARIO("UI Quit and IsDone", "[UI][Quit]") {
	GIVEN("a UI instance") {
		UI ui;

		REQUIRE_FALSE(ui.IsDone());

		WHEN("Quit is called") {
			ui.Quit();

			THEN("IsDone returns true") {
				CHECK(ui.IsDone());
			}
		}
	}
}

SCENARIO("UI CanSave flag", "[UI][CanSave]") {
	GIVEN("a UI instance") {
		UI ui;

		REQUIRE_FALSE(ui.CanSave());

		WHEN("CanSave is set to true") {
			ui.CanSave(true);

			THEN("CanSave returns true") {
				CHECK(ui.CanSave());
			}
		}

		WHEN("CanSave is set to false") {
			ui.CanSave(true);
			ui.CanSave(false);

			THEN("CanSave returns false") {
				CHECK_FALSE(ui.CanSave());
			}
		}
	}
}

SCENARIO("UI IsTop behavior", "[UI][IsTop]") {
	GIVEN("a UI with pushed but not yet stepped panels") {
		UI ui;
		auto panel = std::make_shared<MockPanel>();
		ui.Push(panel);

		WHEN("checking IsTop before StepAll") {
			THEN("IsTop returns false because panel is not yet in the main stack") {
				CHECK_FALSE(ui.IsTop(panel.get()));
			}
		}

		AND_WHEN("StepAll is called") {
			ui.StepAll();

			THEN("IsTop returns true") {
				CHECK(ui.IsTop(panel.get()));
			}
		}
	}
}

SCENARIO("UI Top behavior with pending pushes", "[UI][Top]") {
	GIVEN("a UI with one panel in stack and one pending") {
		UI ui;
		auto panel1 = std::make_shared<MockPanel>();
		ui.Push(panel1);
		ui.StepAll();

		auto panel2 = std::make_shared<MockPanel>();
		ui.Push(panel2);

		WHEN("Top is called before StepAll") {
			THEN("Top returns the pending panel") {
				CHECK(ui.Top() == panel2);
			}
		}

		AND_WHEN("StepAll is called") {
			ui.StepAll();

			THEN("Top still returns the top panel") {
				CHECK(ui.Top() == panel2);
			}
		}
	}
}

SCENARIO("UI Root behavior", "[UI][Root]") {
	GIVEN("an empty UI") {
		UI ui;

		WHEN("no panels exist") {
			THEN("Root returns nullptr") {
				CHECK(ui.Root() == nullptr);
			}
		}
	}

	GIVEN("a UI with pending panels but empty stack") {
		UI ui;
		auto panel = std::make_shared<MockPanel>();
		ui.Push(panel);

		WHEN("Root is called before StepAll") {
			THEN("Root returns the first pending panel") {
				CHECK(ui.Root() == panel);
			}
		}
	}

	GIVEN("a UI with panels in stack") {
		UI ui;
		auto panel1 = std::make_shared<MockPanel>();
		auto panel2 = std::make_shared<MockPanel>();
		ui.Push(panel1);
		ui.Push(panel2);
		ui.StepAll();

		WHEN("Root is called") {
			THEN("Root returns the bottom panel") {
				CHECK(ui.Root() == panel1);
			}
		}
	}
}

SCENARIO("UI TaskQueue access", "[UI][TaskQueue]") {
	GIVEN("a UI instance") {
		UI ui;

		WHEN("accessing SyncQueue") {
			THEN("a valid reference is returned") {
				TaskQueue &queue = ui.SyncQueue();
				CHECK(&queue != nullptr);
			}
		}

		WHEN("accessing AsyncQueue") {
			THEN("a valid reference is returned") {
				TaskQueue &queue = ui.AsyncQueue();
				CHECK(&queue != nullptr);
			}
		}

		WHEN("adding tasks to SyncQueue") {
			bool executed = false;
			ui.SyncQueue().Run([&executed] {
				executed = true;
			});
			ui.SyncQueue().Wait();
			ui.SyncQueue().ProcessSyncTasks();

			THEN("the task executes") {
				CHECK(executed);
			}
		}
	}
}

SCENARIO("UI IsEmpty with various states", "[UI][IsEmpty]") {
	GIVEN("an empty UI") {
		UI ui;

		THEN("IsEmpty returns true") {
			CHECK(ui.IsEmpty());
		}

		WHEN("a panel is pushed") {
			ui.Push(std::make_shared<MockPanel>());

			THEN("IsEmpty returns false due to pending push") {
				CHECK_FALSE(ui.IsEmpty());
			}
		}
	}

	GIVEN("a UI with panels in stack") {
		UI ui;
		ui.Push(std::make_shared<MockPanel>());
		ui.StepAll();

		THEN("IsEmpty returns false") {
			CHECK_FALSE(ui.IsEmpty());
		}

		WHEN("all panels are popped") {
			ui.Pop(ui.Stack().back().get());
			ui.StepAll();

			THEN("IsEmpty returns true") {
				CHECK(ui.IsEmpty());
			}
		}
	}
}

SCENARIO("UI handles full-screen panels", "[UI][FullScreen]") {
	GIVEN("a UI with a full-screen panel at the bottom and a regular panel on top") {
		UI ui;
		auto fullScreenPanel = std::make_shared<MockPanel>(true);
		auto regularPanel = std::make_shared<MockPanel>(false);
		ui.Push(fullScreenPanel);
		ui.Push(regularPanel);
		ui.StepAll();

		THEN("both panels are in the stack") {
			REQUIRE(ui.Stack().size() == 2);
		}
	}
}

SCENARIO("UI Stack const access", "[UI][Stack]") {
	GIVEN("a UI with panels") {
		UI ui;
		auto panel1 = std::make_shared<MockPanel>();
		auto panel2 = std::make_shared<MockPanel>();
		ui.Push(panel1);
		ui.Push(panel2);
		ui.StepAll();

		WHEN("accessing the stack") {
			const auto &stack = ui.Stack();

			THEN("the stack contains the panels in order") {
				REQUIRE(stack.size() == 2);
				CHECK(stack[0] == panel1);
				CHECK(stack[1] == panel2);
			}
		}
	}
}

SCENARIO("UI StepAll processes panels", "[UI][StepAll]") {
	GIVEN("a UI with a mock panel") {
		UI ui;
		auto mockPanel = std::make_shared<MockPanel>();
		ui.Push(mockPanel);

		WHEN("StepAll is called") {
			ui.StepAll();

			THEN("the panel's Step method is called") {
				CHECK(mockPanel->GetStepCount() == 1);
			}

			AND_WHEN("StepAll is called again") {
				ui.StepAll();

				THEN("the panel's Step method is called again") {
					CHECK(mockPanel->GetStepCount() == 2);
				}
			}
		}
	}
}

SCENARIO("UI handles multiple push/pop in one step", "[UI][Deferred]") {
	GIVEN("a UI with one panel") {
		UI ui;
		auto panel1 = std::make_shared<MockPanel>();
		ui.Push(panel1);
		ui.StepAll();

		WHEN("a panel is pushed and another popped before StepAll") {
			auto panel2 = std::make_shared<MockPanel>();
			ui.Push(panel2);
			ui.Pop(panel1.get());
			ui.StepAll();

			THEN("only the new panel remains") {
				REQUIRE(ui.Stack().size() == 1);
				CHECK(ui.Stack().back() == panel2);
			}
		}
	}
}

SCENARIO("UI handles null panel push", "[UI][EdgeCase]") {
	GIVEN("an empty UI") {
		UI ui;

		WHEN("a null shared_ptr is pushed") {
			std::shared_ptr<Panel> nullPanel;
			ui.Push(nullPanel);
			ui.StepAll();

			THEN("the stack remains empty") {
				CHECK(ui.Stack().empty());
			}
		}
	}
}

SCENARIO("UI handles pop of non-existent panel", "[UI][EdgeCase]") {
	GIVEN("a UI with one panel") {
		UI ui;
		auto panel1 = std::make_shared<MockPanel>();
		ui.Push(panel1);
		ui.StepAll();

		WHEN("popping a panel that was never pushed") {
			auto otherPanel = std::make_shared<MockPanel>();
			ui.Pop(otherPanel.get());
			ui.StepAll();

			THEN("the stack is unchanged") {
				REQUIRE(ui.Stack().size() == 1);
				CHECK(ui.Stack().back() == panel1);
			}
		}
	}
}

SCENARIO("UI handles double pop of same panel", "[UI][EdgeCase]") {
	GIVEN("a UI with one panel") {
		UI ui;
		auto panel = std::make_shared<MockPanel>();
		ui.Push(panel);
		ui.StepAll();

		WHEN("the same panel is popped twice before StepAll") {
			ui.Pop(panel.get());
			ui.Pop(panel.get());
			ui.StepAll();

			THEN("the panel is removed once") {
				CHECK(ui.Stack().empty());
			}
		}
	}
}

SCENARIO("UI handles many panels", "[UI][EdgeCase]") {
	GIVEN("an empty UI") {
		UI ui;

		WHEN("many panels are pushed") {
			constexpr int PANEL_COUNT = 100;
			std::vector<std::shared_ptr<MockPanel>> panels;
			for(int i = 0; i < PANEL_COUNT; ++i)
				panels.push_back(std::make_shared<MockPanel>());

			for(auto &p : panels)
				ui.Push(p);

			ui.StepAll();

			THEN("all panels are in the stack") {
				REQUIRE(ui.Stack().size() == PANEL_COUNT);
			}

			AND_WHEN("all panels are popped") {
				for(auto &p : panels)
					ui.Pop(p.get());
				ui.StepAll();

				THEN("the stack is empty") {
					CHECK(ui.Stack().empty());
				}
			}
		}
	}
}

SCENARIO("UI panel lifecycle", "[UI][Lifecycle]") {
	GIVEN("a UI with a panel pushed via raw pointer") {
		UI ui;
		MockPanel *rawPointer = nullptr;
		int useCountBefore = 0;

		{
			auto panel = std::make_shared<MockPanel>();
			useCountBefore = panel.use_count();
			rawPointer = panel.get();
			ui.Push(panel);
			ui.StepAll();
			// panel shared_ptr goes out of scope here, but UI holds a copy
		}

		WHEN("the panel is still in the UI") {
			THEN("the panel is still alive because UI holds a reference") {
				REQUIRE(ui.Stack().size() == 1);
				CHECK(ui.Stack().back().get() == rawPointer);
			}
		}

		AND_WHEN("the panel is popped and stepped") {
			ui.Pop(rawPointer);
			ui.StepAll();

			THEN("the stack is empty") {
				CHECK(ui.Stack().empty());
			}
		}
	}
}

SCENARIO("UI::UISound enum values", "[UI][UISound]") {
	GIVEN("the UISound enum") {
		THEN("all expected values are present") {
			CHECK(static_cast<int>(UI::UISound::NONE) == 0);
			CHECK(static_cast<int>(UI::UISound::SOFT) == 1);
			CHECK(static_cast<int>(UI::UISound::NORMAL) == 2);
			CHECK(static_cast<int>(UI::UISound::SOFT_BUZZ) == 3);
			CHECK(static_cast<int>(UI::UISound::TARGET) == 4);
			CHECK(static_cast<int>(UI::UISound::FAILURE) == 5);
		}
	}
}

// #endregion unit tests



} // test namespace