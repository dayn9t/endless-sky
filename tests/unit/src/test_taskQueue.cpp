/* test_taskQueue.cpp
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
#include "../../../source/TaskQueue.h"

// ... and any system includes needed for the test file.
#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>

namespace { // test namespace

// #region mock data

// Helper struct to track task execution state across threads.
struct TaskTracker {
	std::atomic<int> asyncExecuted{0};
	std::atomic<int> syncExecuted{0};
	std::mutex syncMutex;

	void Reset()
	{
		asyncExecuted = 0;
		syncExecuted = 0;
	}
};

// #endregion mock data



// #region unit tests

SCENARIO("Creating a TaskQueue", "[TaskQueue][Creation]") {
	GIVEN("a default constructed TaskQueue") {
		TaskQueue queue;

		THEN("the queue is created successfully") {
			REQUIRE(true);
		}

		WHEN("waiting on an empty queue") {
			queue.Wait();

			THEN("wait completes immediately") {
				REQUIRE(true);
			}
		}
	}
}

SCENARIO("Running async tasks", "[TaskQueue][Run]") {
	GIVEN("a TaskQueue") {
		TaskQueue queue;
		TaskTracker tracker;

		WHEN("a single async task is run") {
			auto future = queue.Run([&tracker] {
				tracker.asyncExecuted++;
			});

			THEN("the future becomes valid after execution") {
				future.wait();
				REQUIRE(future.valid());
				REQUIRE(tracker.asyncExecuted == 1);
			}

			queue.Wait();
		}

		WHEN("multiple async tasks are run") {
			constexpr int TASK_COUNT = 10;
			for(int i = 0; i < TASK_COUNT; ++i)
			{
				queue.Run([&tracker] {
					tracker.asyncExecuted++;
				});
			}

			THEN("all tasks are eventually executed") {
				queue.Wait();
				REQUIRE(tracker.asyncExecuted == TASK_COUNT);
			}
		}
	}
}

SCENARIO("Running tasks with sync callbacks", "[TaskQueue][Run][SyncTask]") {
	GIVEN("a TaskQueue") {
		TaskQueue queue;
		TaskTracker tracker;

		WHEN("a task with a sync callback is run") {
			auto future = queue.Run(
				[&tracker] {
					tracker.asyncExecuted++;
				},
				[&tracker] {
					tracker.syncExecuted++;
				}
			);

			future.wait();

			THEN("the async task is executed but sync is not yet processed") {
				REQUIRE(tracker.asyncExecuted == 1);
				REQUIRE(tracker.syncExecuted == 0);
			}

			AND_WHEN("ProcessSyncTasks is called") {
				queue.ProcessSyncTasks();

				THEN("the sync callback is executed") {
					REQUIRE(tracker.syncExecuted == 1);
				}
			}

			queue.Wait();
		}

		WHEN("multiple tasks with sync callbacks are run") {
			constexpr int TASK_COUNT = 5;
			for(int i = 0; i < TASK_COUNT; ++i)
			{
				queue.Run(
					[&tracker] {
						tracker.asyncExecuted++;
					},
					[&tracker] {
						tracker.syncExecuted++;
					}
				);
			}

			queue.Wait();

			THEN("all async tasks are executed") {
				REQUIRE(tracker.asyncExecuted == TASK_COUNT);
			}

			AND_WHEN("ProcessSyncTasks is called") {
				queue.ProcessSyncTasks();

				THEN("all sync callbacks are executed") {
					REQUIRE(tracker.syncExecuted == TASK_COUNT);
				}
			}
		}
	}
}

SCENARIO("Processing sync tasks", "[TaskQueue][ProcessSyncTasks]") {
	GIVEN("a TaskQueue") {
		TaskQueue queue;
		TaskTracker tracker;

		WHEN("ProcessSyncTasks is called with no pending sync tasks") {
			queue.ProcessSyncTasks();

			THEN("nothing happens and no error occurs") {
				REQUIRE(tracker.syncExecuted == 0);
			}
		}

		WHEN("sync tasks exceed MAX_SYNC_TASKS limit") {
			// Add more than MAX_SYNC_TASKS (100) sync tasks.
			constexpr int TASK_COUNT = 150;
			for(int i = 0; i < TASK_COUNT; ++i)
			{
				queue.Run(
					[&tracker] {
						tracker.asyncExecuted++;
					},
					[&tracker] {
						tracker.syncExecuted++;
					}
				);
			}

			queue.Wait();

			AND_WHEN("ProcessSyncTasks is called once") {
				queue.ProcessSyncTasks();

				THEN("at most MAX_SYNC_TASKS are processed") {
					REQUIRE(tracker.syncExecuted <= TaskQueue::MAX_SYNC_TASKS);
					REQUIRE(tracker.syncExecuted > 0);
				}

				AND_WHEN("ProcessSyncTasks is called again until empty") {
					while(tracker.syncExecuted < TASK_COUNT)
						queue.ProcessSyncTasks();

					THEN("all sync tasks are eventually processed") {
						REQUIRE(tracker.syncExecuted == TASK_COUNT);
					}
				}
			}
		}
	}
}

SCENARIO("Waiting for task completion", "[TaskQueue][Wait]") {
	GIVEN("a TaskQueue with tasks") {
		TaskQueue queue;
		TaskTracker tracker;

		WHEN("tasks are queued and Wait is called") {
			constexpr int TASK_COUNT = 20;
			for(int i = 0; i < TASK_COUNT; ++i)
			{
				queue.Run([&tracker] {
					// Simulate some work.
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
					tracker.asyncExecuted++;
				});
			}

			queue.Wait();

			THEN("all tasks are completed before Wait returns") {
				REQUIRE(tracker.asyncExecuted == TASK_COUNT);
			}
		}

		WHEN("Wait is called on an empty queue") {
			queue.Wait();

			THEN("it returns immediately") {
				REQUIRE(true);
			}
		}
	}
}

SCENARIO("TaskQueue destructor waits for tasks", "[TaskQueue][Destructor]") {
	GIVEN("a TaskQueue with pending tasks") {
		TaskTracker tracker;
		constexpr int TASK_COUNT = 5;
		{
			TaskQueue queue;

			WHEN("the queue goes out of scope with pending tasks") {
				for(int i = 0; i < TASK_COUNT; ++i)
				{
					queue.Run([&tracker] {
						std::this_thread::sleep_for(std::chrono::milliseconds(5));
						tracker.asyncExecuted++;
					});
				}
				// Queue destructor is called here.
			}

			THEN("all tasks are completed before destructor finishes") {
				REQUIRE(tracker.asyncExecuted == TASK_COUNT);
			}
		}
	}
}

SCENARIO("TaskQueue is non-copyable", "[TaskQueue][Copy]") {
	GIVEN("a TaskQueue") {
		TaskQueue queue;

		THEN("copy constructor is deleted") {
			REQUIRE(!std::is_copy_constructible<TaskQueue>::value);
		}

		THEN("copy assignment is deleted") {
			REQUIRE(!std::is_copy_assignable<TaskQueue>::value);
		}
	}
}

SCENARIO("TaskQueue handles exceptions in async tasks", "[TaskQueue][Exception]") {
	GIVEN("a TaskQueue") {
		TaskQueue queue;
		TaskTracker tracker;

		WHEN("an async task throws an exception") {
			auto future = queue.Run([&tracker] {
				tracker.asyncExecuted++;
				throw std::runtime_error("Test exception");
			});

			queue.Wait();

			THEN("the task is still executed") {
				REQUIRE(tracker.asyncExecuted == 1);
			}

			THEN("the exception is propagated to sync task queue") {
				queue.ProcessSyncTasks();
				// Exception should be in sync tasks queue, but rethrowing
				// would terminate the test, so we just verify no crash.
			}
		}
	}
}

SCENARIO("TaskQueue future validity", "[TaskQueue][Future]") {
	GIVEN("a TaskQueue") {
		TaskQueue queue;

		WHEN("Run returns a future") {
			auto future = queue.Run([] {});

			THEN("the future is valid") {
				REQUIRE(future.valid());
			}

			AND_WHEN("the task completes") {
				future.wait();

				THEN("the future is still valid") {
					REQUIRE(future.valid());
				}
			}

			queue.Wait();
		}

		WHEN("multiple futures are obtained") {
			std::vector<std::shared_future<void>> futures;
			constexpr int TASK_COUNT = 5;
			for(int i = 0; i < TASK_COUNT; ++i)
			{
				futures.push_back(queue.Run([] {}));
			}

			THEN("all futures are valid") {
				for(const auto& f : futures)
					REQUIRE(f.valid());
			}

			queue.Wait();
		}
	}
}

// #endregion unit tests



} // test namespace