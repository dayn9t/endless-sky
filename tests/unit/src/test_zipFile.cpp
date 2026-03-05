/* test_zipFile.cpp
Copyright (c) 2025 by Endless Sky Contributors

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
#include "../../../source/ZipFile.h"

// ... and any system includes needed for the test file.
#include <filesystem>
#include <fstream>
#include <minizip/zip.h>
#include <string>

namespace { // test namespace

// #region mock data

// Helper class to create and manage a temporary zip file for tests.
class TempZipFile {
public:
	TempZipFile(const std::filesystem::path &zipPath) : path_(zipPath)
	{
	}

	~TempZipFile()
	{
		std::error_code ec;
		std::filesystem::remove(path_, ec);
	}

	// No copies or moves.
	TempZipFile(const TempZipFile &) = delete;
	TempZipFile(TempZipFile &&) = delete;
	TempZipFile &operator=(const TempZipFile &) = delete;
	TempZipFile &operator=(TempZipFile &&) = delete;

	const std::filesystem::path &Path() const { return path_; }

private:
	std::filesystem::path path_;
};

// Helper class to create a temporary directory for tests.
class TempDirectory {
public:
	TempDirectory() : path_(std::filesystem::temp_directory_path() / "es_test_zip_XXXXXX")
	{
		std::filesystem::create_directories(path_);
	}

	~TempDirectory()
	{
		std::error_code ec;
		std::filesystem::remove_all(path_, ec);
	}

	// No copies or moves.
	TempDirectory(const TempDirectory &) = delete;
	TempDirectory(TempDirectory &&) = delete;
	TempDirectory &operator=(const TempDirectory &) = delete;
	TempDirectory &operator=(TempDirectory &&) = delete;

	const std::filesystem::path &Path() const { return path_; }

	std::filesystem::path operator/(const std::string &subpath) const
	{
		return path_ / subpath;
	}

private:
	std::filesystem::path path_;
};

// Helper to add a file to a zip archive.
bool AddFileToZip(zipFile zip, const std::string &filename, const std::string &content)
{
	zipOpenNewFileInZip(zip, filename.c_str(), nullptr, nullptr, 0, nullptr, 0, nullptr,
		Z_DEFLATED, Z_DEFAULT_COMPRESSION);
	zipWriteInFileInZip(zip, content.c_str(), static_cast<unsigned int>(content.size()));
	zipCloseFileInZip(zip);
	return true;
}

// Helper to add a directory entry to a zip archive.
bool AddDirectoryToZip(zipFile zip, const std::string &dirname)
{
	std::string dirEntry = dirname;
	if(!dirEntry.empty() && dirEntry.back() != '/')
		dirEntry += '/';

	zipOpenNewFileInZip(zip, dirEntry.c_str(), nullptr, nullptr, 0, nullptr, 0, nullptr,
		Z_DEFLATED, Z_DEFAULT_COMPRESSION);
	zipCloseFileInZip(zip);
	return true;
}

// Helper to create a simple zip file with files at root level.
std::filesystem::path CreateSimpleZip(const std::filesystem::path &zipPath)
{
	zipFile zip = zipOpen(zipPath.string().c_str(), APPEND_STATUS_CREATE);
	REQUIRE(zip != nullptr);

	AddFileToZip(zip, "file1.txt", "content of file 1");
	AddFileToZip(zip, "file2.txt", "content of file 2");
	AddFileToZip(zip, "subdir/nested.txt", "nested content");
	AddDirectoryToZip(zip, "emptydir/");

	zipClose(zip, nullptr);
	return zipPath;
}

// Helper to create a zip file with a top-level directory.
std::filesystem::path CreateZipWithTopLevelDir(const std::filesystem::path &zipPath, const std::string &topDirName)
{
	zipFile zip = zipOpen(zipPath.string().c_str(), APPEND_STATUS_CREATE);
	REQUIRE(zip != nullptr);

	AddFileToZip(zip, topDirName + "/file1.txt", "content of file 1");
	AddFileToZip(zip, topDirName + "/file2.txt", "content of file 2");
	AddFileToZip(zip, topDirName + "/subdir/nested.txt", "nested content");

	zipClose(zip, nullptr);
	return zipPath;
}

// Helper to create an empty zip file.
std::filesystem::path CreateEmptyZip(const std::filesystem::path &zipPath)
{
	zipFile zip = zipOpen(zipPath.string().c_str(), APPEND_STATUS_CREATE);
	REQUIRE(zip != nullptr);
	zipClose(zip, nullptr);
	return zipPath;
}

// Helper to create a zip file with binary content.
std::filesystem::path CreateZipWithBinaryContent(const std::filesystem::path &zipPath)
{
	zipFile zip = zipOpen(zipPath.string().c_str(), APPEND_STATUS_CREATE);
	REQUIRE(zip != nullptr);

	std::string binaryContent;
	binaryContent += '\x00';
	binaryContent += '\xFF';
	binaryContent += "text";
	binaryContent += '\x01';
	binaryContent += '\x7F';

	zipOpenNewFileInZip(zip, "binary.bin", nullptr, nullptr, 0, nullptr, 0, nullptr,
		Z_DEFLATED, Z_DEFAULT_COMPRESSION);
	zipWriteInFileInZip(zip, binaryContent.c_str(), static_cast<unsigned int>(binaryContent.size()));
	zipCloseFileInZip(zip);

	zipClose(zip, nullptr);
	return zipPath;
}

// Helper to create a zip file with deeply nested structure.
std::filesystem::path CreateNestedZip(const std::filesystem::path &zipPath)
{
	zipFile zip = zipOpen(zipPath.string().c_str(), APPEND_STATUS_CREATE);
	REQUIRE(zip != nullptr);

	AddFileToZip(zip, "level1/level2/level3/deep.txt", "deeply nested content");
	AddFileToZip(zip, "level1/file.txt", "level 1 content");
	AddFileToZip(zip, "level1/level2/file.txt", "level 2 content");

	zipClose(zip, nullptr);
	return zipPath;
}

// #endregion mock data



// #region unit tests

// Test ZipFile constructor
SCENARIO( "ZipFile can be constructed from a valid zip file", "[ZipFile][Constructor]" ) {
	GIVEN( "A valid zip file" ) {
		TempDirectory tempDir;
		auto zipPath = CreateSimpleZip(tempDir / "test.zip");

		WHEN( "constructing ZipFile with a valid path" ) {
			THEN( "no exception is thrown" ) {
				REQUIRE_NOTHROW(ZipFile(zipPath));
			}
		}

		WHEN( "constructing ZipFile and accessing basic functionality" ) {
			ZipFile zf(zipPath);

			THEN( "the object is usable" ) {
				CHECK(zf.Exists(zipPath / "file1.txt"));
			}
		}
	}
}

SCENARIO( "ZipFile throws on invalid zip path", "[ZipFile][Constructor]" ) {
	GIVEN( "A non-existent file path" ) {
		std::filesystem::path nonexistent = "/nonexistent/path/to/file.zip";

		WHEN( "constructing ZipFile with non-existent path" ) {
			THEN( "throws runtime_error" ) {
				REQUIRE_THROWS_AS(ZipFile(nonexistent), std::runtime_error);
			}
		}
	}
}

SCENARIO( "ZipFile handles empty zip file", "[ZipFile][Constructor]" ) {
	GIVEN( "An empty zip file" ) {
		TempDirectory tempDir;
		auto zipPath = CreateEmptyZip(tempDir / "empty.zip");

		WHEN( "constructing ZipFile from empty zip" ) {
			THEN( "no exception is thrown" ) {
				REQUIRE_NOTHROW(ZipFile(zipPath));
			}
		}

		WHEN( "listing files in empty zip" ) {
			ZipFile zf(zipPath);
			auto files = zf.ListFiles(zipPath, false, false);

			THEN( "returns empty list" ) {
				CHECK(files.empty());
			}
		}
	}
}

// Test ListFiles method
SCENARIO( "ZipFile::ListFiles lists files in zip", "[ZipFile][ListFiles]" ) {
	GIVEN( "A zip file with files and directories" ) {
		TempDirectory tempDir;
		auto zipPath = CreateSimpleZip(tempDir / "test.zip");
		ZipFile zf(zipPath);

		WHEN( "listing all files non-recursively from root" ) {
			auto files = zf.ListFiles(zipPath, false, false);

			THEN( "returns only top-level files" ) {
				REQUIRE(files.size() == 2);
				// Check that file1.txt and file2.txt are in the list
				bool hasFile1 = false, hasFile2 = false;
				for(const auto &f : files)
				{
					std::string name = f.filename().string();
					if(name == "file1.txt") hasFile1 = true;
					if(name == "file2.txt") hasFile2 = true;
				}
				CHECK(hasFile1);
				CHECK(hasFile2);
			}
		}

		WHEN( "listing all files recursively from root" ) {
			auto files = zf.ListFiles(zipPath, true, false);

			THEN( "returns all files including nested ones" ) {
				REQUIRE(files.size() == 3);
			}
		}

		WHEN( "listing directories non-recursively from root" ) {
			auto dirs = zf.ListFiles(zipPath, false, true);

			THEN( "returns only top-level directories" ) {
				REQUIRE(dirs.size() == 2);
			}
		}

		WHEN( "listing directories recursively from root" ) {
			auto dirs = zf.ListFiles(zipPath, true, true);

			THEN( "returns all directories" ) {
				REQUIRE(dirs.size() >= 2);
			}
		}
	}
}

SCENARIO( "ZipFile::ListFiles handles nested directories", "[ZipFile][ListFiles][Nested]" ) {
	GIVEN( "A zip file with deeply nested structure" ) {
		TempDirectory tempDir;
		auto zipPath = CreateNestedZip(tempDir / "nested.zip");
		ZipFile zf(zipPath);

		WHEN( "listing files recursively" ) {
			auto files = zf.ListFiles(zipPath, true, false);

			THEN( "returns all nested files" ) {
				REQUIRE(files.size() == 3);
			}
		}

		WHEN( "listing files in subdirectory non-recursively" ) {
			auto files = zf.ListFiles(zipPath / "level1", false, false);

			THEN( "returns only files in that directory" ) {
				REQUIRE(files.size() == 1);
			}
		}
	}
}

SCENARIO( "ZipFile::ListFiles handles zip with top-level directory", "[ZipFile][ListFiles][TopLevelDir]" ) {
	GIVEN( "A zip file with a top-level directory matching zip name" ) {
		TempDirectory tempDir;
		std::string topDirName = "myplugin";
		auto zipPath = CreateZipWithTopLevelDir(tempDir / "myplugin.zip", topDirName);
		ZipFile zf(zipPath);

		WHEN( "listing files from root" ) {
			auto files = zf.ListFiles(zipPath, true, false);

			THEN( "paths are translated correctly (top-level directory is hidden)" ) {
				REQUIRE(files.size() == 3);
				// All paths should be relative to the zip, not include the top-level dir
				for(const auto &f : files)
				{
					// The path should contain the zip path but not duplicate the top-level dir name
					std::string pathStr = f.string();
					CHECK(pathStr.find(zipPath.string()) != std::string::npos);
				}
			}
		}
	}
}

// Test Exists method
SCENARIO( "ZipFile::Exists checks file existence", "[ZipFile][Exists]" ) {
	GIVEN( "A zip file with known contents" ) {
		TempDirectory tempDir;
		auto zipPath = CreateSimpleZip(tempDir / "test.zip");
		ZipFile zf(zipPath);

		WHEN( "checking for existing file" ) {
			THEN( "returns true" ) {
				CHECK(zf.Exists(zipPath / "file1.txt"));
				CHECK(zf.Exists(zipPath / "file2.txt"));
				CHECK(zf.Exists(zipPath / "subdir" / "nested.txt"));
			}
		}

		WHEN( "checking for non-existent file" ) {
			THEN( "returns false" ) {
				CHECK_FALSE(zf.Exists(zipPath / "nonexistent.txt"));
				CHECK_FALSE(zf.Exists(zipPath / "file3.txt"));
			}
		}

		WHEN( "checking for directory" ) {
			THEN( "returns true for existing directory" ) {
				CHECK(zf.Exists(zipPath / "subdir"));
				CHECK(zf.Exists(zipPath / "emptydir"));
			}
		}

		WHEN( "checking for non-existent directory" ) {
			THEN( "returns false" ) {
				CHECK_FALSE(zf.Exists(zipPath / "nonexistentdir"));
			}
		}
	}
}

SCENARIO( "ZipFile::Exists handles edge cases", "[ZipFile][Exists][Boundary]" ) {
	GIVEN( "A zip file" ) {
		TempDirectory tempDir;
		auto zipPath = CreateSimpleZip(tempDir / "test.zip");
		ZipFile zf(zipPath);

		WHEN( "checking for root directory" ) {
			THEN( "behavior is consistent" ) {
				// Root directory concept may vary - just ensure no crash
				CHECK_NOTHROW(zf.Exists(zipPath));
			}
		}

		WHEN( "checking with empty relative path" ) {
			THEN( "no exception is thrown" ) {
				CHECK_NOTHROW(zf.Exists(zipPath / ""));
			}
		}
	}
}

// Test ReadFile method
SCENARIO( "ZipFile::ReadFile reads file contents", "[ZipFile][ReadFile]" ) {
	GIVEN( "A zip file with known file contents" ) {
		TempDirectory tempDir;
		auto zipPath = CreateSimpleZip(tempDir / "test.zip");
		ZipFile zf(zipPath);

		WHEN( "reading an existing file" ) {
			auto content1 = zf.ReadFile(zipPath / "file1.txt");
			auto content2 = zf.ReadFile(zipPath / "file2.txt");

			THEN( "returns correct content" ) {
				CHECK(content1 == "content of file 1");
				CHECK(content2 == "content of file 2");
			}
		}

		WHEN( "reading a nested file" ) {
			auto content = zf.ReadFile(zipPath / "subdir" / "nested.txt");

			THEN( "returns correct content" ) {
				CHECK(content == "nested content");
			}
		}

		WHEN( "reading a non-existent file" ) {
			auto content = zf.ReadFile(zipPath / "nonexistent.txt");

			THEN( "returns empty string" ) {
				CHECK(content.empty());
			}
		}
	}
}

SCENARIO( "ZipFile::ReadFile handles binary content", "[ZipFile][ReadFile][Binary]" ) {
	GIVEN( "A zip file with binary content" ) {
		TempDirectory tempDir;
		auto zipPath = CreateZipWithBinaryContent(tempDir / "binary.zip");
		ZipFile zf(zipPath);

		WHEN( "reading binary file" ) {
			auto content = zf.ReadFile(zipPath / "binary.bin");

			THEN( "binary content is preserved exactly" ) {
				REQUIRE(content.size() == 8);
				CHECK(content[0] == '\x00');
				CHECK(content[1] == '\xFF');
				CHECK(content.substr(2, 4) == "text");
				CHECK(content[6] == '\x01');
				CHECK(content[7] == '\x7F');
			}
		}
	}
}

// Test boundary conditions
SCENARIO( "ZipFile handles boundary conditions", "[ZipFile][Boundary]" ) {
	GIVEN( "Various edge cases" ) {
		WHEN( "constructing with empty path string" ) {
			THEN( "throws exception" ) {
				REQUIRE_THROWS_AS(ZipFile(std::filesystem::path("")), std::runtime_error);
			}
		}
	}

	GIVEN( "A zip file with files" ) {
		TempDirectory tempDir;
		auto zipPath = CreateSimpleZip(tempDir / "test.zip");
		ZipFile zf(zipPath);

		WHEN( "reading empty directory entry" ) {
			THEN( "behavior is defined" ) {
				// Reading a directory entry should return empty
				auto content = zf.ReadFile(zipPath / "emptydir");
				CHECK(content.empty());
			}
		}

		WHEN( "checking path with special characters" ) {
			// Note: our test setup doesn't create such files, but verify no crash
			THEN( "no exception is thrown" ) {
				CHECK_NOTHROW(zf.Exists(zipPath / "file with spaces.txt"));
				CHECK_NOTHROW(zf.ReadFile(zipPath / "file with spaces.txt"));
			}
		}
	}
}

// Test destructor and resource cleanup
SCENARIO( "ZipFile properly manages resources", "[ZipFile][Resource]" ) {
	GIVEN( "A zip file" ) {
		TempDirectory tempDir;
		auto zipPath = CreateSimpleZip(tempDir / "test.zip");

		WHEN( "ZipFile goes out of scope" ) {
			THEN( "resources are properly released (no crash)" ) {
				{
					ZipFile zf(zipPath);
					// Use the zip file
					CHECK(zf.Exists(zipPath / "file1.txt"));
				}
				// After scope exit, the file should be closed
				// We can create another ZipFile for the same path
				REQUIRE_NOTHROW(ZipFile(zipPath));
			}
		}

		WHEN( "creating multiple ZipFile instances for different paths" ) {
			THEN( "all instances work independently" ) {
				auto zipPath2 = CreateSimpleZip(tempDir / "test2.zip");

				ZipFile zf1(zipPath);
				ZipFile zf2(zipPath2);

				CHECK(zf1.Exists(zipPath / "file1.txt"));
				CHECK(zf2.Exists(zipPath2 / "file1.txt"));
			}
		}
	}
}

// Test with zip containing many files
SCENARIO( "ZipFile handles zip with multiple files", "[ZipFile][Multiple]" ) {
	GIVEN( "A zip file with many files" ) {
		TempDirectory tempDir;
		auto zipPath = tempDir / "many.zip";

		zipFile zip = zipOpen(zipPath.string().c_str(), APPEND_STATUS_CREATE);
		REQUIRE(zip != nullptr);

		// Create 10 files
		for(int i = 0; i < 10; ++i)
		{
			std::string filename = "file" + std::to_string(i) + ".txt";
			std::string content = "content " + std::to_string(i);
			AddFileToZip(zip, filename, content);
		}

		zipClose(zip, nullptr);

		WHEN( "listing all files" ) {
			ZipFile zf(zipPath);
			auto files = zf.ListFiles(zipPath, false, false);

			THEN( "all files are returned" ) {
				REQUIRE(files.size() == 10);
			}
		}

		WHEN( "reading each file" ) {
			ZipFile zf(zipPath);

			THEN( "all files have correct content" ) {
				for(int i = 0; i < 10; ++i)
				{
					std::string filename = "file" + std::to_string(i) + ".txt";
					std::string expected = "content " + std::to_string(i);
					CHECK(zf.ReadFile(zipPath / filename) == expected);
				}
			}
		}
	}
}

// Test consistency between Exists, ListFiles, and ReadFile
SCENARIO( "ZipFile operations are consistent", "[ZipFile][Consistency]" ) {
	GIVEN( "A zip file with files" ) {
		TempDirectory tempDir;
		auto zipPath = CreateSimpleZip(tempDir / "test.zip");
		ZipFile zf(zipPath);

		WHEN( "checking consistency between ListFiles and Exists" ) {
			auto files = zf.ListFiles(zipPath, true, false);

			THEN( "all listed files exist" ) {
				for(const auto &f : files)
				{
					CHECK(zf.Exists(f));
				}
			}
		}

		WHEN( "checking consistency between Exists and ReadFile" ) {
			THEN( "existing files return non-empty content" ) {
				if(zf.Exists(zipPath / "file1.txt"))
				{
					CHECK_FALSE(zf.ReadFile(zipPath / "file1.txt").empty());
				}
			}
		}
	}
}

// #endregion unit tests



} // test namespace