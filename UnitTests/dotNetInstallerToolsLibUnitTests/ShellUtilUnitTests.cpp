#include "StdAfx.h"
#include "ShellUtilUnitTests.h"
#include "FindWindow.h"

using namespace DVLib::UnitTests;

CPPUNIT_TEST_SUITE_REGISTRATION(ShellUtilUnitTests);

void ShellUtilUnitTests::testGetEnvironmentVariable()
{
	wchar_t computername_s[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD computername_size = ARRAYSIZE(computername_s);
	CPPUNIT_ASSERT(::GetComputerNameW(computername_s, & computername_size));
	std::wstring computername = DVLib::GetEnvironmentVariable(L"COMPUTERNAME");
	std::wcout << std::endl << L"Computer name: " << computername;
	CPPUNIT_ASSERT(computername.length() > 0);
	CPPUNIT_ASSERT(computername.length() == wcslen(computername.c_str()));
	CPPUNIT_ASSERT(computername == computername_s);
}

void ShellUtilUnitTests::testExpandEnvironmentVariables()
{
	CPPUNIT_ASSERT(DVLib::ExpandEnvironmentVariables(L"") == L"");
	CPPUNIT_ASSERT(DVLib::ExpandEnvironmentVariables(L"%%") == L"%%");
	CPPUNIT_ASSERT(DVLib::ExpandEnvironmentVariables(L"%%%") == L"%%%");

	std::wstring guid = DVLib::GenerateGUIDStringW();
	CPPUNIT_ASSERT(DVLib::ExpandEnvironmentVariables(L"%" + guid + L"%") == L"%" + guid + L"%");
	CPPUNIT_ASSERT(DVLib::ExpandEnvironmentVariables(L"%cd%") == L"%cd%");
	CPPUNIT_ASSERT(DVLib::ExpandEnvironmentVariables(L"%COMPUTERNAME%") == DVLib::GetEnvironmentVariableW(L"COMPUTERNAME"));
	CPPUNIT_ASSERT(DVLib::ExpandEnvironmentVariables(L"%COMPUTERNAME%%COMPUTERNAME%") == DVLib::GetEnvironmentVariableW(L"COMPUTERNAME") + DVLib::GetEnvironmentVariableW(L"COMPUTERNAME"));
	CPPUNIT_ASSERT(DVLib::ExpandEnvironmentVariables(L"%COMPUTERNAME") == L"%COMPUTERNAME");
	CPPUNIT_ASSERT(DVLib::ExpandEnvironmentVariables(L"COMPUTERNAME%") == L"COMPUTERNAME%");
	CPPUNIT_ASSERT(DVLib::ExpandEnvironmentVariables(L"{%COMPUTERNAME%}") == L"{" + DVLib::GetEnvironmentVariableW(L"COMPUTERNAME") + L"}");
}

void ShellUtilUnitTests::testDetachCmd()
{
	// test timer runs for 2 seconds
	std::wstring testTimerExe = DVLib::DirectoryCombine(
#ifdef DEBUG
		DVLib::GetModuleDirectoryW(), L"..\\..\\TestTimer\\bin\\Debug\\TestTimer.exe"
#else
		DVLib::GetModuleDirectoryW(), L"..\\..\\TestTimer\\bin\\Release\\TestTimer.exe"
#endif
		);
	CPPUNIT_ASSERT(DVLib::FileExists(testTimerExe));
	// detach without pi
	DWORD c1 = ::GetTickCount();
	DVLib::DetachCmd(testTimerExe);
	CPPUNIT_ASSERT((::GetTickCount() - c1) < 2 * 1000);
	// run with process information
	PROCESS_INFORMATION pi = { 0 };
	DWORD c2 = ::GetTickCount();
	DVLib::DetachCmd(testTimerExe, & pi);
	auto_handle pi_thread(pi.hThread);
	auto_handle pi_process(pi.hProcess);
	CPPUNIT_ASSERT(pi.dwProcessId > 0);
	CPPUNIT_ASSERT((::GetTickCount() - c2) < 2 * 1000);
}

void ShellUtilUnitTests::testRunCmd()
{
	// run without pi
	DVLib::RunCmd(L"cmd.exe /C exit /b 0");
	// with process information
	PROCESS_INFORMATION pi = { 0 };
	DVLib::RunCmd(L"cmd.exe /C exit /b 0", & pi);
	auto_handle pi_thread(pi.hThread);
	auto_handle pi_process(pi.hProcess);
	CPPUNIT_ASSERT(pi.dwProcessId > 0);
	CPPUNIT_ASSERT(WAIT_OBJECT_0 == ::WaitForSingleObject(pi.hProcess, INFINITE));
}

void ShellUtilUnitTests::testExecCmd()
{
	CPPUNIT_ASSERT(0 == DVLib::ExecCmd(L"cmd.exe /C"));
	CPPUNIT_ASSERT(123 == DVLib::ExecCmd(L"cmd.exe /C exit /b 123"));

	// hide window
	CPPUNIT_ASSERT(456 == DVLib::ExecCmd(L"cmd.exe /C exit /b 456", L"", SW_HIDE));
}

void ShellUtilUnitTests::testShellCmd()
{
	DVLib::ShellCmd(L"cmd.exe /C");
	DVLib::ShellCmd(L"\"" + DVLib::GetEnvironmentVariable(L"SystemRoot") + L"\\system32\\cmd.exe\" /C");
	HANDLE hProcess;
	DVLib::ShellCmd(L"\"cmd.exe\" /C dir", NULL, &hProcess);
	auto_handle pi_process(hProcess);
	CPPUNIT_ASSERT(hProcess != NULL);
	CPPUNIT_ASSERT(WAIT_OBJECT_0 == ::WaitForSingleObject(hProcess, INFINITE));
}

void ShellUtilUnitTests::testRunCmdWithHiddenWindow()
{
	// Arrange
	int nShow = SW_HIDE;

	// Act
	PROCESS_INFORMATION pi = { 0 };
	DVLib::RunCmd(L"cmd.exe /C ping -n 6 127.0.0.1 > nul && exit /b 0", & pi, 0, L"", nShow);
	auto_handle pi_thread(pi.hThread);
	auto_handle pi_process(pi.hProcess);

	// Assert
	CPPUNIT_ASSERT(pi.dwProcessId > 0);
	CPPUNIT_ASSERT(NULL == FindWindow::FindWindowFromProcessId(pi.dwProcessId));
	CPPUNIT_ASSERT(WAIT_OBJECT_0 == ::WaitForSingleObject(pi.hProcess, INFINITE));
}

void ShellUtilUnitTests::testShellCmdWithHiddenWindow()
{
	// Arrange
	int nShow = SW_HIDE;

	HANDLE hProcess;

	// Act
	DVLib::ShellCmd(L"\"cmd.exe\" /C ping -n 6 127.0.0.1 > nul", NULL, &hProcess, NULL, L"", nShow);
	auto_handle pi_process(hProcess);

	// Assert
	CPPUNIT_ASSERT(hProcess != NULL);
	CPPUNIT_ASSERT(NULL == FindWindow::FindWindowFromProcess(hProcess));
	CPPUNIT_ASSERT(WAIT_OBJECT_0 == ::WaitForSingleObject(hProcess, INFINITE));
}

void ShellUtilUnitTests::testRunCmdWithoutWorkingDirectorySpecified()
{
	// Arrange
	const std::wstring working_directory = DVLib::GetCurrentDirectoryW();
	PROCESS_INFORMATION pi = { 0 };
	const std::wstring command = DVLib::FormatMessage(
		L"cmd.exe /C if '%%cd%%'=='%s' (exit /b 0) else (echo '%%cd%%'!='%s' && exit /b 1)",
		working_directory.c_str(),
		working_directory.c_str());

	// Act
	DVLib::RunCmd(command, &pi, 0);
	auto_handle pi_thread(pi.hThread);
	auto_handle pi_process(pi.hProcess);

	// Assert
	CPPUNIT_ASSERT(pi.dwProcessId > 0);
	CPPUNIT_ASSERT(WAIT_OBJECT_0 == ::WaitForSingleObject(pi.hProcess, INFINITE));

	DWORD exitCode = 0;
	CHECK_WIN32_BOOL(::GetExitCodeProcess(pi.hProcess, &exitCode),
		L"GetExitCodeProcess");
	CPPUNIT_ASSERT(exitCode == 0);
}

void ShellUtilUnitTests::testRunCmdWithWorkingDirectorySpecified()
{
	// Arrange
	const std::wstring working_directory = DVLib::GetTemporaryDirectoryW();

	// Act
	PROCESS_INFORMATION pi = { 0 };
	DVLib::RunCmd(L"cmd.exe /C if '%%cd%%'=='%%temp%%' (exit /b 0) else (echo '%%cd%%'!='%%temp%%' && exit /b 1)", &pi, 0, working_directory);
	auto_handle pi_thread(pi.hThread);
	auto_handle pi_process(pi.hProcess);

	// Assert
	CPPUNIT_ASSERT(pi.dwProcessId > 0);
	CPPUNIT_ASSERT(WAIT_OBJECT_0 == ::WaitForSingleObject(pi.hProcess, INFINITE));

	DWORD exitCode = 0;
	CHECK_WIN32_BOOL(::GetExitCodeProcess(pi.hProcess, &exitCode),
		L"GetExitCodeProcess");
	CPPUNIT_ASSERT(exitCode == 0);
}

void ShellUtilUnitTests::testShellCmdWithoutWorkingDirectorySpecified()
{
	// Arrange
	const std::wstring working_directory = DVLib::GetCurrentDirectoryW();
	const std::wstring command = DVLib::FormatMessage(
		L"cmd.exe /C if '%%cd%%'=='%s' (exit /b 0) else (echo '%%cd%%'!='%s' && exit /b 1)",
		working_directory.c_str(),
		working_directory.c_str());
	HANDLE hProcess;

	// Act
	DVLib::ShellCmd(command.c_str(), NULL, &hProcess, NULL);
	auto_handle pi_process(hProcess);

	// Assert
	CPPUNIT_ASSERT(hProcess != NULL);
	CPPUNIT_ASSERT(WAIT_OBJECT_0 == ::WaitForSingleObject(hProcess, INFINITE));

	DWORD exitCode = 0;
	CHECK_WIN32_BOOL(::GetExitCodeProcess(hProcess, &exitCode),
		L"GetExitCodeProcess");
	CPPUNIT_ASSERT(exitCode == 0);
}

void ShellUtilUnitTests::testShellCmdWithWorkingDirectorySpecified()
{
	// Arrange
	const std::wstring working_directory = DVLib::GetTemporaryDirectoryW();
	HANDLE hProcess;

	// Act
	DVLib::ShellCmd(L"\"cmd.exe\" /C if '%%cd%%'=='%%temp%%' (exit /b 0) else (echo '%%cd%%'!='%%temp%%' && exit /b 1)", NULL, &hProcess, NULL, working_directory);
	auto_handle pi_process(hProcess);

	// Assert
	CPPUNIT_ASSERT(hProcess != NULL);
	CPPUNIT_ASSERT(WAIT_OBJECT_0 == ::WaitForSingleObject(hProcess, INFINITE));

	DWORD exitCode = 0;
	CHECK_WIN32_BOOL(::GetExitCodeProcess(hProcess, &exitCode),
		L"GetExitCodeProcess");
	CPPUNIT_ASSERT(exitCode == 0);
}