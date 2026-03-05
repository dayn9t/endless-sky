/* test_files.cpp
Copyright (c) 2025 by TomGoodIdea

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
#include "../../../source/Files.h"

// ... and any system includes needed for the test file.
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace { // test namespace

// #region mock data

// Helper class to create and manage a temporary directory for tests.
class TempDirectory {
public:
	TempDirectory() : path_(std::filesystem::temp_directory_path() / "es_test_files_XXXXXX")
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

// Helper to create a file with content.
void CreateFileWithContent(const std::filesystem::path &path, const std::string &content = "")
{
	std::ofstream file(path, std::ios::binary);
	file << content;
}

// #endregion mock data



// #region unit tests

// Test Files::Name - static method that doesn't require initialization
SCENARIO( "Files::Name extracts filename from path", "[Files][Name]" ) {
	GIVEN( "Various file paths" ) {
		WHEN( "extracting filename from a simple path" ) {
			THEN( "returns just the filename" ) {
				CHECK( Files::Name("/path/to/file.txt") == "file.txt" );
			}
		}
		WHEN( "extracting filename from a path with no directory" ) {
			THEN( "returns the filename itself" ) {
				CHECK( Files::Name("file.txt") == "file.txt" );
			}
		}
		WHEN( "extracting filename from an empty path" ) {
			THEN( "returns empty string" ) {
				CHECK( Files::Name("").empty() );
			}
		}
		WHEN( "extracting filename from a path with trailing separator" ) {
			THEN( "returns empty string" ) {
				CHECK( Files::Name("/path/to/directory/").empty() );
			}
		}
		WHEN( "extracting filename from a path with multiple extensions" ) {
			THEN( "returns the full filename including all extensions" ) {
				CHECK( Files::Name("/path/to/archive.tar.gz") == "archive.tar.gz" );
			}
		}
	}
}

// Test Files::IsParent - static method that doesn't require initialization
SCENARIO( "Files::IsParent checks parent-child relationship", "[Files][IsParent]" ) {
	GIVEN( "Two paths to compare" ) {
		WHEN( "first path is a parent of second" ) {
			THEN( "returns true" ) {
				CHECK( Files::IsParent("/home", "/home/user") );
				CHECK( Files::IsParent("/home/user", "/home/user/documents") );
				CHECK( Files::IsParent("/a", "/a/b/c/d") );
			}
		}
		WHEN( "paths are equal" ) {
			THEN( "returns true (a path is its own parent)" ) {
				CHECK( Files::IsParent("/home", "/home") );
				CHECK( Files::IsParent("/same/path", "/same/path") );
			}
		}
		WHEN( "first path is not a parent of second" ) {
			THEN( "returns false" ) {
				CHECK_FALSE( Files::IsParent("/home/user", "/home") );
				CHECK_FALSE( Files::IsParent("/a/b", "/a/c") );
				CHECK_FALSE( Files::IsParent("/different", "/path") );
			}
		}
		WHEN( "first path is longer than second" ) {
			THEN( "returns false" ) {
				CHECK_FALSE( Files::IsParent("/a/b/c", "/a/b") );
				CHECK_FALSE( Files::IsParent("/long/path/here", "/short") );
			}
		}
		WHEN( "paths have partial match but are not parent-child" ) {
			THEN( "returns false" ) {
				CHECK_FALSE( Files::IsParent("/home/user1", "/home/user2") );
				CHECK_FALSE( Files::IsParent("/abc", "/abcdef") );
			}
		}
		WHEN( "using empty paths" ) {
			THEN( "empty path is parent of everything" ) {
				// An empty path is treated as a valid prefix.
				CHECK( Files::IsParent("", "/any/path") );
			}
		}
	}
}

// Test Files::Exists with filesystem
SCENARIO( "Files::Exists checks file existence", "[Files][Exists]" ) {
	GIVEN( "A temporary directory" ) {
		TempDirectory tempDir;

		WHEN( "checking a file that exists" ) {
			auto filePath = tempDir / "existing_file.txt";
			CreateFileWithContent(filePath, "test content");

			THEN( "returns true" ) {
				CHECK( Files::Exists(filePath) );
			}
		}

		WHEN( "checking a file that does not exist" ) {
			THEN( "returns false" ) {
				CHECK_FALSE( Files::Exists(tempDir / "nonexistent.txt") );
			}
		}

		WHEN( "checking a directory that exists" ) {
			auto subDir = tempDir / "subdirectory";
			std::filesystem::create_directory(subDir);

			THEN( "returns true" ) {
				CHECK( Files::Exists(subDir) );
			}
		}
	}
}

// Test Files::CreateFolder
SCENARIO( "Files::CreateFolder creates directories", "[Files][CreateFolder]" ) {
	GIVEN( "A temporary directory" ) {
		TempDirectory tempDir;

		WHEN( "creating a new directory" ) {
			auto newDir = tempDir / "new_folder";

			THEN( "directory is created successfully" ) {
				REQUIRE_NOTHROW( Files::CreateFolder(newDir) );
				CHECK( Files::Exists(newDir) );
				CHECK( std::filesystem::is_directory(newDir) );
			}
		}

		WHEN( "creating a directory that already exists" ) {
			auto existingDir = tempDir / "existing_folder";
			Files::CreateFolder(existingDir);

			THEN( "does not throw and directory still exists" ) {
				REQUIRE_NOTHROW( Files::CreateFolder(existingDir) );
				CHECK( Files::Exists(existingDir) );
			}
		}
	}
}

// Test Files::List
SCENARIO( "Files::List lists files in directory", "[Files][List]" ) {
	GIVEN( "A temporary directory with files and subdirectories" ) {
		TempDirectory tempDir;

		// Create test structure
		CreateFileWithContent(tempDir / "file1.txt", "content1");
		CreateFileWithContent(tempDir / "file2.txt", "content2");
		std::filesystem::create_directory(tempDir / "subdir1");
		std::filesystem::create_directory(tempDir / "subdir2");
		CreateFileWithContent(tempDir / "subdir1" / "nested.txt", "nested content");

		WHEN( "listing files in the directory" ) {
			auto files = Files::List(tempDir.Path());

			THEN( "only regular files are returned, not directories" ) {
				REQUIRE( files.size() == 2 );
				// Files should be sorted
				CHECK( Files::Name(files[0]) == "file1.txt" );
				CHECK( Files::Name(files[1]) == "file2.txt" );
			}
		}

		WHEN( "listing files in a non-existent directory" ) {
			auto files = Files::List(tempDir / "nonexistent");

			THEN( "returns empty list" ) {
				CHECK( files.empty() );
			}
		}

		WHEN( "listing files in an empty directory" ) {
			auto emptyDir = tempDir / "empty";
			std::filesystem::create_directory(emptyDir);
			auto files = Files::List(emptyDir);

			THEN( "returns empty list" ) {
				CHECK( files.empty() );
			}
		}
	}
}

// Test Files::ListDirectories
SCENARIO( "Files::ListDirectories lists directories", "[Files][ListDirectories]" ) {
	GIVEN( "A temporary directory with files and subdirectories" ) {
		TempDirectory tempDir;

		// Create test structure
		CreateFileWithContent(tempDir / "file1.txt", "content");
		std::filesystem::create_directory(tempDir / "subdir1");
		std::filesystem::create_directory(tempDir / "subdir2");

		WHEN( "listing directories" ) {
			auto dirs = Files::ListDirectories(tempDir.Path());

			THEN( "only directories are returned, not files" ) {
				REQUIRE( dirs.size() == 2 );
				// Directories should be sorted
				CHECK( Files::Name(dirs[0]) == "subdir1" );
				CHECK( Files::Name(dirs[1]) == "subdir2" );
			}
		}

		WHEN( "listing directories in a non-existent directory" ) {
			auto dirs = Files::ListDirectories(tempDir / "nonexistent");

			THEN( "returns empty list" ) {
				CHECK( dirs.empty() );
			}
		}
	}
}

// Test Files::RecursiveList
SCENARIO( "Files::RecursiveList lists all files recursively", "[Files][RecursiveList]" ) {
	GIVEN( "A temporary directory with nested structure" ) {
		TempDirectory tempDir;

		// Create nested structure
		CreateFileWithContent(tempDir / "root.txt", "root");
		std::filesystem::create_directory(tempDir / "level1");
		CreateFileWithContent(tempDir / "level1" / "file1.txt", "level1");
		std::filesystem::create_directory(tempDir / "level1" / "level2");
		CreateFileWithContent(tempDir / "level1" / "level2" / "file2.txt", "level2");

		WHEN( "recursively listing files" ) {
			auto files = Files::RecursiveList(tempDir.Path());

			THEN( "all files in all subdirectories are returned" ) {
				REQUIRE( files.size() == 3 );

				// Extract just the filenames for easier checking
				std::set<std::string> filenames;
				for(const auto &f : files)
					filenames.insert(Files::Name(f));

				CHECK( filenames.contains("root.txt") );
				CHECK( filenames.contains("file1.txt") );
				CHECK( filenames.contains("file2.txt") );
			}
		}

		WHEN( "recursively listing from a non-existent directory" ) {
			auto files = Files::RecursiveList(tempDir / "nonexistent");

			THEN( "returns empty list" ) {
				CHECK( files.empty() );
			}
		}
	}
}

// Test Files::Copy
SCENARIO( "Files::Copy copies files", "[Files][Copy]" ) {
	GIVEN( "A temporary directory with a source file" ) {
		TempDirectory tempDir;

		auto sourcePath = tempDir / "source.txt";
		std::string content = "This is the source content.";
		CreateFileWithContent(sourcePath, content);

		WHEN( "copying to a new location" ) {
			auto destPath = tempDir / "destination.txt";
			bool result = Files::Copy(sourcePath, destPath);

			THEN( "copy succeeds and content is identical" ) {
				CHECK( result );
				CHECK( Files::Exists(destPath) );
				CHECK( Files::Read(destPath) == content );
			}
		}

		WHEN( "copying to a location that already exists" ) {
			auto destPath = tempDir / "existing.txt";
			CreateFileWithContent(destPath, "Old content");

			bool result = Files::Copy(sourcePath, destPath);

			THEN( "file is overwritten" ) {
				CHECK( result );
				CHECK( Files::Read(destPath) == content );
			}
		}

		WHEN( "copying a non-existent file" ) {
			auto destPath = tempDir / "destination.txt";
			bool result = Files::Copy(tempDir / "nonexistent.txt", destPath);

			THEN( "copy fails" ) {
				CHECK_FALSE( result );
			}
		}
	}
}

// Test Files::Move
SCENARIO( "Files::Move moves files", "[Files][Move]" ) {
	GIVEN( "A temporary directory with a source file" ) {
		TempDirectory tempDir;

		auto sourcePath = tempDir / "source.txt";
		std::string content = "Content to move.";
		CreateFileWithContent(sourcePath, content);

		WHEN( "moving to a new location" ) {
			auto destPath = tempDir / "moved.txt";
			Files::Move(sourcePath, destPath);

			THEN( "file is moved and original no longer exists" ) {
				CHECK( Files::Exists(destPath) );
				CHECK_FALSE( Files::Exists(sourcePath) );
				CHECK( Files::Read(destPath) == content );
			}
		}

		WHEN( "moving to a different directory" ) {
			auto newDir = tempDir / "new_directory";
			std::filesystem::create_directory(newDir);
			auto destPath = newDir / "moved.txt";

			Files::Move(sourcePath, destPath);

			THEN( "file is moved across directories" ) {
				CHECK( Files::Exists(destPath) );
				CHECK_FALSE( Files::Exists(sourcePath) );
			}
		}
	}
}

// Test Files::Delete
SCENARIO( "Files::Delete removes files and directories", "[Files][Delete]" ) {
	GIVEN( "A temporary directory" ) {
		TempDirectory tempDir;

		WHEN( "deleting a file" ) {
			auto filePath = tempDir / "to_delete.txt";
			CreateFileWithContent(filePath, "content");
			REQUIRE( Files::Exists(filePath) );

			Files::Delete(filePath);

			THEN( "file no longer exists" ) {
				CHECK_FALSE( Files::Exists(filePath) );
			}
		}

		WHEN( "deleting a directory with contents" ) {
			auto dirPath = tempDir / "directory_to_delete";
			std::filesystem::create_directory(dirPath);
			CreateFileWithContent(dirPath / "file.txt", "content");
			REQUIRE( Files::Exists(dirPath) );

			Files::Delete(dirPath);

			THEN( "directory and all contents are removed" ) {
				CHECK_FALSE( Files::Exists(dirPath) );
			}
		}

		WHEN( "deleting a non-existent file" ) {
			THEN( "does not throw" ) {
				REQUIRE_NOTHROW( Files::Delete(tempDir / "nonexistent.txt") );
			}
		}
	}
}

// Test Files::Open and Files::Read
SCENARIO( "Files::Open opens files for reading and writing", "[Files][Open]" ) {
	GIVEN( "A temporary directory with a test file" ) {
		TempDirectory tempDir;

		auto filePath = tempDir / "test.txt";
		std::string content = "Hello, World!";
		CreateFileWithContent(filePath, content);

		WHEN( "opening an existing file for reading" ) {
			auto stream = Files::Open(filePath, false);

			THEN( "stream is valid and readable" ) {
				REQUIRE( stream );
				CHECK( static_cast<bool>(*stream) );
				CHECK( Files::Read(stream) == content );
			}
		}

		WHEN( "opening a non-existent file for reading" ) {
			auto stream = Files::Open(tempDir / "nonexistent.txt", false);

			THEN( "returns null stream" ) {
				CHECK_FALSE( stream );
			}
		}

		WHEN( "opening a file for writing" ) {
			auto stream = Files::Open(filePath, true);

			THEN( "stream is valid and writable" ) {
				REQUIRE( stream );
				std::string newContent = "New content";
				Files::Write(stream, newContent);
				stream.reset();

				// Verify content was written
				CHECK( Files::Read(filePath) == newContent );
			}
		}
	}
}

// Test Files::Read with path
SCENARIO( "Files::Read reads file contents", "[Files][Read]" ) {
	GIVEN( "A temporary directory with test files" ) {
		TempDirectory tempDir;

		WHEN( "reading an existing file" ) {
			auto filePath = tempDir / "read_test.txt";
			std::string content = "Test content\nwith multiple\nlines.";
			CreateFileWithContent(filePath, content);

			THEN( "returns the file content" ) {
				CHECK( Files::Read(filePath) == content );
			}
		}

		WHEN( "reading an empty file" ) {
			auto filePath = tempDir / "empty.txt";
			CreateFileWithContent(filePath, "");

			THEN( "returns empty string" ) {
				CHECK( Files::Read(filePath).empty() );
			}
		}

		WHEN( "reading a non-existent file" ) {
			THEN( "returns empty string" ) {
				CHECK( Files::Read(tempDir / "nonexistent.txt").empty() );
			}
		}

		WHEN( "reading a null stream" ) {
			std::shared_ptr<std::iostream> nullStream;

			THEN( "returns empty string" ) {
				CHECK( Files::Read(nullStream).empty() );
			}
		}
	}
}

// Test Files::Write
SCENARIO( "Files::Write writes to files", "[Files][Write]" ) {
	GIVEN( "A temporary directory" ) {
		TempDirectory tempDir;

		WHEN( "writing to a new file" ) {
			auto filePath = tempDir / "write_test.txt";
			std::string content = "Content to write";
			Files::Write(filePath, content);

			THEN( "file is created with correct content" ) {
				CHECK( Files::Exists(filePath) );
				CHECK( Files::Read(filePath) == content );
			}
		}

		WHEN( "writing to an existing file" ) {
			auto filePath = tempDir / "overwrite.txt";
			CreateFileWithContent(filePath, "Old content");

			std::string newContent = "New content";
			Files::Write(filePath, newContent);

			THEN( "file is overwritten" ) {
				CHECK( Files::Read(filePath) == newContent );
			}
		}

		WHEN( "writing empty content" ) {
			auto filePath = tempDir / "empty_write.txt";
			Files::Write(filePath, "");

			THEN( "file is created but empty" ) {
				CHECK( Files::Exists(filePath) );
				CHECK( Files::Read(filePath).empty() );
			}
		}

		WHEN( "writing to a null stream" ) {
			std::shared_ptr<std::iostream> nullStream;

			THEN( "does not throw" ) {
				REQUIRE_NOTHROW( Files::Write(nullStream, "test") );
			}
		}
	}
}

// Test Files::Timestamp
SCENARIO( "Files::Timestamp gets file modification time", "[Files][Timestamp]" ) {
	GIVEN( "A temporary directory with a test file" ) {
		TempDirectory tempDir;

		auto filePath = tempDir / "timestamp_test.txt";
		CreateFileWithContent(filePath, "content");

		WHEN( "getting timestamp of an existing file" ) {
			auto timestamp = Files::Timestamp(filePath);

			THEN( "returns a valid file time" ) {
				// Check that the timestamp is not "epoch" (i.e., it was set)
				// Note: Some filesystem clocks may be out of sync, so we just check
				// that we get some value (could be negative in edge cases)
				CHECK( timestamp.time_since_epoch().count() != 0 );
			}
		}
	}
}

// Test boundary conditions
SCENARIO( "Files handles boundary conditions", "[Files][Boundary]" ) {
	GIVEN( "Various edge cases" ) {
		WHEN( "using empty path with Name" ) {
			THEN( "returns empty string" ) {
				CHECK( Files::Name("").empty() );
			}
		}

		WHEN( "using path with special characters" ) {
			TempDirectory tempDir;
			auto specialPath = tempDir / "file with spaces.txt";
			CreateFileWithContent(specialPath, "content");

			THEN( "handles spaces in filename correctly" ) {
				CHECK( Files::Exists(specialPath) );
				CHECK( Files::Name(specialPath) == "file with spaces.txt" );
			}
		}

		WHEN( "using very long path" ) {
			THEN( "Name still extracts filename correctly" ) {
				std::string longPath = "/very/long/path/with/many/directory/levels/";
				longPath += "filename.txt";
				CHECK( Files::Name(longPath) == "filename.txt" );
			}
		}
	}
}

// Test files with binary content
SCENARIO( "Files handles binary content", "[Files][Binary]" ) {
	GIVEN( "A temporary directory" ) {
		TempDirectory tempDir;

		WHEN( "writing and reading binary content" ) {
			auto filePath = tempDir / "binary.bin";
			// Create binary content with null bytes and various byte values
			std::string binaryContent;
			binaryContent += '\x00';
			binaryContent += '\xFF';
			binaryContent += "text";
			binaryContent += '\x01';
			binaryContent += '\x7F';

			Files::Write(filePath, binaryContent);

			THEN( "binary content is preserved exactly" ) {
				CHECK( Files::Read(filePath) == binaryContent );
			}
		}

		WHEN( "copying binary file" ) {
			auto sourcePath = tempDir / "binary_source.bin";
			auto destPath = tempDir / "binary_dest.bin";

			std::string binaryContent = "\x00\x01\x02\xFF\xFE\xFD";
			CreateFileWithContent(sourcePath, binaryContent);

			Files::Copy(sourcePath, destPath);

			THEN( "binary content is copied exactly" ) {
				CHECK( Files::Read(destPath) == binaryContent );
			}
		}
	}
}

// Test concurrent file operations
SCENARIO( "Files handles multiple operations on same file", "[Files][Concurrent]" ) {
	GIVEN( "A temporary directory with a file" ) {
		TempDirectory tempDir;
		auto filePath = tempDir / "multi_op.txt";

		WHEN( "opening same file multiple times" ) {
			CreateFileWithContent(filePath, "original");

			auto stream1 = Files::Open(filePath, false);
			auto stream2 = Files::Open(filePath, false);

			THEN( "both streams are valid" ) {
				CHECK( stream1 );
				CHECK( stream2 );
			}
		}

		WHEN( "reading file while it's open for writing" ) {
			CreateFileWithContent(filePath, "original");

			auto writeStream = Files::Open(filePath, true);
			Files::Write(writeStream, "modified");

			THEN( "content is updated" ) {
				CHECK( Files::Read(filePath) == "modified" );
			}
		}
	}
}

// #endregion unit tests



} // test namespace