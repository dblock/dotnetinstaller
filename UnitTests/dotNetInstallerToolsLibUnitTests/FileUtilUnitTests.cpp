#include "StdAfx.h"
#include "FileUtilUnitTests.h"
#include <shlwapi.h>

using namespace DVLib::UnitTests;

CPPUNIT_TEST_SUITE_REGISTRATION(FileUtilUnitTests);

void FileUtilUnitTests::testFileExists()
{
	std::string path = DVLib::GetTemporaryFileNameA();
	std::cout << std::endl << "Temporary filename: " << path;
	
	std::string directory = DVLib::GetFileDirectoryA(path);
	std::cout << std::endl << "Directory: " << directory;
	
	CPPUNIT_ASSERT(! DVLib::FileExists(directory));
	CPPUNIT_ASSERT(DVLib::FileExists(path));
	DVLib::FileDelete(path);

	CPPUNIT_ASSERT(! DVLib::FileExists("C:\\"));	
	CPPUNIT_ASSERT(! DVLib::FileExists(""));

	std::string filename = DVLib::GenerateGUIDStringA();
	std::string guidpath = "C:\\" + filename;
	std::cout << std::endl << "Guid path: " << guidpath;
	CPPUNIT_ASSERT(! DVLib::FileExists(guidpath));
}

void FileUtilUnitTests::testGetTemporaryFileName()
{
	std::string path = DVLib::GetTemporaryFileNameA();
	std::cout << std::endl << "Temporary filename: " << path;
	std::string directory = DVLib::GetFileDirectoryA(path);
	std::cout << std::endl << "Directory: " << directory;
	CPPUNIT_ASSERT(directory.length() > 0);
	std::string filename = DVLib::GetFileNameA(path);
	std::cout << std::endl << "File: " << filename;
	CPPUNIT_ASSERT(filename.length() > 0);
	std::stringstream s_path;
	s_path << directory << filename;
	std::cout << std::endl << "Joined directory: " << s_path.str();
	CPPUNIT_ASSERT(path == s_path.str());
	CPPUNIT_ASSERT(DVLib::FileExists(path));
	DVLib::FileDelete(path);
	CPPUNIT_ASSERT(! DVLib::FileExists(path));
}

void FileUtilUnitTests::testFileDelete()
{
	std::string path = DVLib::GetTemporaryFileNameA();
	std::cout << std::endl << "Temporary filename: " << path;
	CPPUNIT_ASSERT(DVLib::FileExists(path));
	CPPUNIT_ASSERT(::PathFileExistsA(path.c_str()));
	DVLib::FileDelete(path);
	CPPUNIT_ASSERT(! DVLib::FileExists(path));
	CPPUNIT_ASSERT(FALSE == ::PathFileExistsA(path.c_str()));
}

void FileUtilUnitTests::testFileCopy()
{
	std::string path = DVLib::GetTemporaryFileNameA();
	std::cout << std::endl << "Temporary filename: " << path;
	CPPUNIT_ASSERT(DVLib::FileExists(path));
	std::string path_copy = path + ".copy";
	CPPUNIT_ASSERT(! DVLib::FileExists(path_copy));
	DVLib::FileCopy(path, path_copy, true);
	CPPUNIT_ASSERT(DVLib::FileExists(path_copy));
	DVLib::FileDelete(path);
	DVLib::FileDelete(path_copy);
}

void FileUtilUnitTests::testFileMove()
{
	std::string path = DVLib::GetTemporaryFileNameA();
	std::cout << std::endl << "Temporary filename: " << path;
	CPPUNIT_ASSERT(DVLib::FileExists(path));
	std::string path_copy = path + ".move";
	CPPUNIT_ASSERT(! DVLib::FileExists(path_copy));
	DVLib::FileMove(path, path_copy, true);
	CPPUNIT_ASSERT(DVLib::FileExists(! path));
	CPPUNIT_ASSERT(DVLib::FileExists(path_copy));
	DVLib::FileDelete(path_copy);
}

void FileUtilUnitTests::testGetFileSize()
{
    std::wstring moduledir = DVLib::GetModuleDirectoryW();
	std::list<std::wstring> files = DVLib::GetFiles(moduledir, L"*.xml", DVLib::GET_FILES_FILES | DVLib::GET_FILES_RECURSIVE);
    for each (const std::wstring& file in files)
    {
        long size = DVLib::GetFileSize(file);
        std::wcout << std::endl << L" " << file << L": " << DVLib::towstring(size) << L" byte(s)";
        CPPUNIT_ASSERT(size >= 0);
    }
}

void FileUtilUnitTests::testFileWrite()
{
    std::wstring tmpfile = DVLib::DirectoryCombine(
        DVLib::GetTemporaryDirectoryW(), DVLib::GenerateGUIDStringW());
	std::wcout << std::endl << L"Temporary filename: " << tmpfile;
    // empty data
    std::vector<char> write_data;
    DVLib::FileWrite(tmpfile, write_data);
    std::vector<char> read_data = DVLib::FileReadToEnd(tmpfile);
    CPPUNIT_ASSERT(read_data.size() == 0); // temporary files are created empty
    // some data
    std::string write_guid = DVLib::GenerateGUIDStringA();
    std::cout << std::endl << "Write GUID: " << write_guid;
    std::vector<char> write_guid_vector(write_guid.begin(), write_guid.end());
    DVLib::FileWrite(tmpfile, write_guid_vector);
    std::vector<char> read_guid = DVLib::FileReadToEnd(tmpfile);
	std::wcout << std::endl << L"Size: " << read_guid.size();    
    CPPUNIT_ASSERT(read_guid.size() > 0);
    CPPUNIT_ASSERT(read_guid.size() == DVLib::GetFileSize(tmpfile));
    std::string read_guid_string(read_guid.begin(), read_guid.end());
    std::cout << std::endl << "Read GUID: " << read_guid_string;
    CPPUNIT_ASSERT(write_guid == read_guid_string);
    // delete temp file
    DVLib::FileDelete(tmpfile);
}

void FileUtilUnitTests::testFileCreate()
{
    std::wstring tmpfile = DVLib::DirectoryCombine(
        DVLib::GetTemporaryDirectoryW(), DVLib::GenerateGUIDStringW());
	std::wcout << std::endl << L"Temporary filename: " << tmpfile;
	CPPUNIT_ASSERT(! DVLib::FileExists(tmpfile));
    // create the file
    DVLib::FileCreate(tmpfile);
    CPPUNIT_ASSERT(DVLib::GetFileSize(tmpfile) == 0);
    // populate the temp file with data, then re-create, ensure that it is empty
    std::vector<char> data;
    data.push_back('x');
    DVLib::FileWrite(tmpfile, data);
    CPPUNIT_ASSERT(DVLib::GetFileSize(tmpfile) == 1);
    DVLib::FileCreate(tmpfile);
	CPPUNIT_ASSERT(DVLib::FileExists(tmpfile));
    CPPUNIT_ASSERT(DVLib::GetFileSize(tmpfile) == 0);
    DVLib::FileDelete(tmpfile);
}

void FileUtilUnitTests::testFileReadToEnd()
{
	std::wstring tmpfile = DVLib::GetTemporaryFileNameW();
	std::wcout << std::endl << L"Temporary filename: " << tmpfile;
	CPPUNIT_ASSERT(DVLib::FileExists(tmpfile));
    std::vector<char> data = DVLib::FileReadToEnd(tmpfile);
    CPPUNIT_ASSERT(data.size() == 0); // temporary files are created empty
    // ansi data
    std::string guid = DVLib::GenerateGUIDStringA();
	std::ofstream f(DVLib::wstring2string(tmpfile).c_str());
    f << guid.c_str();
    f.close();
    // read as byte data
    std::vector<char> guiddata = DVLib::FileReadToEnd(tmpfile);
	std::wcout << std::endl << L"Size: " << guiddata.size();    
    CPPUNIT_ASSERT(guiddata.size() > 0);
    CPPUNIT_ASSERT(guiddata.size() == DVLib::GetFileSize(tmpfile));
    // delete temp file
    DVLib::FileDelete(tmpfile);
}

void FileUtilUnitTests::testGetFileVersionInfo()
{
	std::wstring userexepath = DVLib::DirectoryCombine(DVLib::GetSystemDirectoryW(), L"user.exe");
	DVLib::FileVersionInfo versioninfo = DVLib::GetFileVersionInfo(userexepath);
	std::wcout << std::endl << L"user.exe version: " 
		<< HIWORD(versioninfo.fixed_info.dwFileVersionMS) << L"."
		<< LOWORD(versioninfo.fixed_info.dwFileVersionMS) << L"."
		<< HIWORD(versioninfo.fixed_info.dwFileVersionLS) << L"."
		<< LOWORD(versioninfo.fixed_info.dwFileVersionLS);
	CPPUNIT_ASSERT(versioninfo.fixed_info.dwFileVersionMS > 0);
	std::wcout << std::endl << L"user.exe cp/language: "
		<< versioninfo.translation_info.wCodePage << L":" << versioninfo.translation_info.wLanguage;
	CPPUNIT_ASSERT(versioninfo.translation_info.wCodePage > 256);
	CPPUNIT_ASSERT(versioninfo.translation_info.wLanguage > 256);
}

void FileUtilUnitTests::testGetFileVersion()
{
	std::wstring userexepath = DVLib::DirectoryCombine(DVLib::GetSystemDirectoryW(), L"user.exe");
	std::wstring version = DVLib::GetFileVersion(userexepath);
	CPPUNIT_ASSERT(version.length());
	CPPUNIT_ASSERT(DVLib::split(version, L".").size() == 4);	
}

void FileUtilUnitTests::testLoadResourceData()
{
	std::vector<char> data = DVLib::LoadResourceData<char>(NULL, L"RES_TEST", L"CUSTOM");
	CPPUNIT_ASSERT(data.size() == 3390);
}

void FileUtilUnitTests::testResourceExists()
{
	CPPUNIT_ASSERT(DVLib::ResourceExists(NULL, L"RES_TEST", L"CUSTOM"));
	CPPUNIT_ASSERT(! DVLib::ResourceExists(NULL, L"RES_TEST", L"TypeDoesntExist"));
	CPPUNIT_ASSERT(! DVLib::ResourceExists(NULL, L"ResourceDoesntExist", L"CUSTOM"));
}

void FileUtilUnitTests::testwstring2fileversion()
{
	struct TestData
	{
		LPCWSTR version_s;
		DVLib::FileVersion version;
	};

	TestData testdata[] = 
	{
		{ L"", { 0, 0, 0, 0 } },
		{ L"1", { 1, 0, 0, 0 } },
		{ L"0.1", { 0, 1, 0, 0 } },
		{ L"0.1.0", { 0, 1, 0, 0 } },
		{ L"0.1.2.3", { 0, 1, 2, 3 } },
		{ L"1.2.3.4", { 1, 2, 3, 4 } },
		{ L"19.456.12345.5", { 19, 456, 12345, 5 } }
	};

	for (int i = 0; i < ARRAYSIZE(testdata); i++)
	{
		DVLib::FileVersion version = DVLib::wstring2fileversion(testdata[i].version_s);
		std::wcout << std::endl << testdata[i].version_s << L" => "
			<< version.major << L"." << version.minor << L"." 
			<< version.build << L"." << version.rev;
		CPPUNIT_ASSERT(version.major == testdata[i].version.major);
		CPPUNIT_ASSERT(version.minor == testdata[i].version.minor);
		CPPUNIT_ASSERT(version.build == testdata[i].version.build);
		CPPUNIT_ASSERT(version.rev == testdata[i].version.rev);
	}
}

void FileUtilUnitTests::testfileversion2wstring()
{
	FileVersion zero = { 0, 0, 0, 0};
	CPPUNIT_ASSERT(DVLib::fileversion2wstring(zero) == L"0.0.0.0");
	FileVersion notzero = { 12, 345, 6789, 100};
	CPPUNIT_ASSERT(DVLib::fileversion2wstring(notzero) == L"12.345.6789.100");
}
