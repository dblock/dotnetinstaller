#include "StdAfx.h"
#include "OsUtil.h"
#include "StringUtil.h"
#include "ExceptionMacros.h"
#include "ErrorUtil.h"
#include "PathUtil.h"
#include "FileUtil.h"
#include "RegistryUtil.h"

DVLib::OperatingSystem DVLib::GetOperatingSystemVersion()
{
	DVLib::OperatingSystem os = winNone;
	OSVERSIONINFOEX osvi = { 0 };

	// use GetVersionEx, fallback on GetVersion when unavaialble
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if(! ::GetVersionEx(reinterpret_cast<LPOSVERSIONINFO>(& osvi)))
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		CHECK_WIN32_BOOL(GetVersionEx(reinterpret_cast<LPOSVERSIONINFO>(& osvi)),
			L"GetVersionEx");
	}

    SYSTEM_INFO si = { 0 };
	::GetSystemInfo(& si);

	switch (osvi.dwPlatformId)
	{
		// Test for the Windows NT product family.
		case VER_PLATFORM_WIN32_NT:
			// Newer Windows version
			if (((osvi.dwMajorVersion == 6 && osvi.dwMinorVersion > 2) || (osvi.dwMajorVersion > 6)) && osvi.wProductType == VER_NT_WORKSTATION)
			{
				os = winMax;
			}
			// Windows 8 Server
			else if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2 && osvi.wProductType != VER_NT_WORKSTATION)
			{
				os = win8Server;
			}
			// Windows 8
			else if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2 && osvi.wProductType == VER_NT_WORKSTATION)
			{
				os = win8;
			}
			// Windows 7
			else if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1 && osvi.wProductType == VER_NT_WORKSTATION)
			{
				os = win7;

				if (osvi.wServicePackMajor >= 1)
					os = win7sp1;
			}
			// Windows Server 2008 R2
			else if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1 && osvi.wProductType != VER_NT_WORKSTATION)
			{
				os = winServer2008R2;
			}
			// Windows Server 2008
			else if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 && osvi.wProductType != VER_NT_WORKSTATION)
			{
				os = winServer2008;

				if (osvi.wServicePackMajor >= 2)
					os = winServer2008sp2;
			}
			else if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 && osvi.wProductType == VER_NT_WORKSTATION)
			{
				os = winVista;

				if (osvi.wServicePackMajor == 1) 
					os = winVistaSp1;
				else if (osvi.wServicePackMajor >= 2) 
					os = winVistaSp2;
			}
			// Windows Server 2003 versions
			else if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 && 
				osvi.wProductType != VER_NT_WORKSTATION && GetSystemMetrics(89 /* SM_SERVERR2 */) == 0)
			{
				os = winServer2003;

				if (osvi.wServicePackMajor == 1)
					os = winServer2003sp1;
				else if (osvi.wServicePackMajor >= 2)
					os = winServer2003sp2;
			}
			else if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 && 
				osvi.wProductType != VER_NT_WORKSTATION && GetSystemMetrics(89 /* SM_SERVERR2 */) != 0)
			{
				os = winServer2003R2;

				if (osvi.wServicePackMajor == 1)
					os = winServer2003R2sp1;
				else if (osvi.wServicePackMajor >= 2)
					os = winServer2003R2sp2;
			}
			// Windows XP 64 bit Professional
			else if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 && 
				osvi.wProductType == VER_NT_WORKSTATION)
			{
				os = winXP;

				if (osvi.wServicePackMajor == 1)
					os = winXPsp1;
				else if (osvi.wServicePackMajor == 2)
					os = winXPsp2;
				else if (osvi.wServicePackMajor >= 3)
					os = winXPsp3;
			}
			// Windows XP 32 bit versions
			else if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
			{
				os = winXP;

				if (osvi.wServicePackMajor == 1)
					os = winXPsp1;
				else if (osvi.wServicePackMajor == 2)
					os = winXPsp2;
				else if (osvi.wServicePackMajor >= 3)
					os = winXPsp3;
			}
			// Windows 2000 versions
			else if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
			{
				os = win2000;

				if (osvi.wServicePackMajor == 1)
					os = win2000sp1;
				else if (osvi.wServicePackMajor == 2)
					os = win2000sp2;
				else if (osvi.wServicePackMajor == 3)
					os = win2000sp3;
				else if (osvi.wServicePackMajor >= 4)
					os = win2000sp4;
			}
			// Windows NT versions
			else if ( osvi.dwMajorVersion == 4 )
			{
				os = winNT4;
				// check if Sp6a
				if(0 == _wcsicmp(osvi.szCSDVersion, L"Service Pack 6"))
				{
					// Test for SP6 versus SP6a.
					if (DVLib::RegistryKeyExists(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Hotfix\\Q246009"))
					{
						os = winNT4sp6a;
					}
					else // Windows NT 4.0 prior to SP6a
					{
						os = winNT4sp6;
					}
				}
			}

			break;
		// Test for the Windows 95 product family.
		case VER_PLATFORM_WIN32_WINDOWS:
			if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
			{
				os = winME;
			}
			else if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
			{
				os = win98;
				//test windows 98 se
				if ( osvi.szCSDVersion[1] == 'A' )
					os = win98se;
			}
			else if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
			{
				os = win95;
				//test Win95 osr2
				if ( osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B' )
					os = win95osr2;
			} 
			break;
	}

	CHECK_BOOL(os != winNone, 
		L"Unsupported operating system, major=" << osvi.dwMajorVersion 
			<< L", version=" << osvi.dwMinorVersion << L"." << osvi.dwMinorVersion
			<< L", sp=" << osvi.wServicePackMajor << L"." << osvi.wServicePackMinor
			<< L", type=" << osvi.wProductType);
	
	return os;
}

std::wstring DVLib::GetOperatingSystemVersionString()
{
	return os2wstring(GetOperatingSystemVersion());
}

bool DVLib::IsInOperatingSystemInRange(OperatingSystem os, const std::wstring& os_filter, OperatingSystem l, OperatingSystem r)
{
	if (!os_filter.empty())
	{
		if (l != winNone || r != winNone)
		{
			THROW_EX(L"Conflicting os_filter=" << os_filter << 
				L", os_filter_min=" << DVLib::os2wstring(l) << L", os_filter_max=" << DVLib::os2wstring(r));
		}

		return IsOperatingSystemID(os, os_filter);
	}
	else if (l != winNone && r != winNone)
	{
		return os >= l && os <= r;
	}
	else if (l != winNone)
	{
		return os >= l;
	}
	else if (r != winNone)
	{
		return os <= r;
	}
	
	return true;
}

LCID DVLib::GetOperatingSystemLCID(LcidType lcidtype)
{
	switch(lcidtype)
	{
	case LcidMuiUser:
		return MuiGetSystemDefaultUILCID();
	case LcidMuiSystem:
		return MuiGetUserDefaultUILCID();
	case LcidSystem:
		return ::GetSystemDefaultLCID();
	case LcidUser:
		return ::GetUserDefaultLCID();
	case LcidUserExe:
	default:
		// see http://support.microsoft.com/kb/q181604/
		std::wstring userexepath = DVLib::DirectoryCombine(DVLib::GetSystemDirectoryW(), L"user.exe");
		DVLib::FileVersionInfo versioninfo = DVLib::GetFileVersionInfo(userexepath);
		return versioninfo.translation_info.wLanguage;
	}
}

bool DVLib::IsOperatingSystemLCID(LcidType lcidtype, const std::wstring& lcid)
{
	return IsOperatingSystemLCIDValue(GetOperatingSystemLCID(lcidtype), lcid);
}

// \todo: this should probably move to dotNetInstallerLib since lcid is dotNetInstaller-format-specific
bool DVLib::IsOperatingSystemLCIDValue(LCID lcid_in, const std::wstring& filter)
{
	if (filter.empty())
		return true;

	std::vector<std::wstring> lcids = DVLib::split(filter, L",");

	std::vector<LCID> lcid_or;
	std::vector<LCID> lcid_andnot;

	for (size_t i = 0; i < lcids.size(); i++)
	{
		if (lcids[i].empty())
			continue; // tolerate an empty value

		if (lcids[i][0] == L'!')
		{
			lcid_andnot.push_back(DVLib::wstring2long(lcids[i].substr(1)));
		}
		else
		{
			lcid_or.push_back(DVLib::wstring2long(lcids[i]));
		}
	}

	if (lcid_or.size() > 0 && lcid_andnot.size() >0)
	{
		THROW_EX(L"Ambiguous LCID filter: " << filter);
	}

	if (lcid_or.size() > 0)
	{
		for each(LCID lcid in lcid_or)
		{
			if (lcid == lcid_in)
				return true;
		}

		return false;
	} 
	else 
	{
		for each(LCID lcid in lcid_andnot)
		{
			if (lcid == lcid_in)
				return false;
		}

		return true;
	}
}

bool DVLib::IsOperatingSystemID(OperatingSystem os_in, const std::wstring& filter)
{
	if (filter.empty())
	{
		return true;
	}

	std::vector<std::wstring> oss = DVLib::split(filter, L",");

	size_t filters = 0;
	std::vector<OperatingSystem> os_or;
	std::vector<OperatingSystem> os_andnot;
	std::vector<OperatingSystem> os_andge; // And greater than or equal filter
	std::vector<OperatingSystem> os_orlt;  // Or less than filter

	for (size_t i = 0; i < oss.size(); i++)
	{
		if (oss[i].empty())
		{
			continue; // tolerate an empty value
		}

		filters++; // Total number of filters (excluding empty ones)

		if (oss[i][0] == L'!')
		{
			os_andnot.push_back(static_cast<OperatingSystem>(DVLib::oscode2os(oss[i].substr(1))));
		}
		else if (oss[i][0] == L'+')
		{
			os_andge.push_back(static_cast<OperatingSystem>(DVLib::oscode2os(oss[i].substr(1))));
		}
		else if (oss[i][0] == L'-')
		{
			os_orlt.push_back(static_cast<OperatingSystem>(DVLib::oscode2os(oss[i].substr(1))));
		}
		else
		{
			os_or.push_back(static_cast<OperatingSystem>(DVLib::oscode2os(oss[i])));
		}
	}

	if ((os_or.size() > 0 && os_or.size() != filters) || 
		(os_andnot.size() > 0 && os_andnot.size() != filters) || 
		(os_andge.size() > 0 && os_andge.size() != filters) || 
		(os_orlt.size() > 0 && os_orlt.size() != filters))
	{
		THROW_EX(L"Ambiguous OS filter: " << filter);
	}

	if (os_or.size() > 0)
	{
		for each (OperatingSystem os in os_or)
		{
			if (os == os_in)
			{
				return true;
			}
		}

		return false;
	} 
	else if (os_andge.size() > 0)
	{
		OperatingSystem max_filter = winNone;
		bool match = false;

		// The os must be greater than or equal to all the values in the filter
		for each (OperatingSystem os in os_andge)
		{
			// Match the input OS then check the filter value
			switch (OperatingSystemType(os_in))
			{
				case winNone:
					THROW_EX(L"Unsupported OS filter: " << filter);
					break;
				case winNT4:
					if (OperatingSystemType(os) == winNT4 && os_in >= os)
					{
						match = true;
					}
					break;
				case win2000:
					if (OperatingSystemType(os) == win2000 && os_in >= os)
					{
						match = true;
					}
					break;
				case winXP:
					if (OperatingSystemType(os) == winXP && os_in >= os)
					{
						match = true;
					}
					break;
				case winServer2003:
					if (OperatingSystemType(os) == winServer2003 && os_in >= os)
					{
						match = true;
					}
					break;
				case winServer2003R2:
					if (OperatingSystemType(os) == winServer2003R2 && os_in >= os)
					{
						match = true;
					}
					break;
				case winVista:
					if (OperatingSystemType(os) == winVista && os_in >= os)
					{
						match = true;
					}
					break;
				case winServer2008:
					if (OperatingSystemType(os) == winServer2008 && os_in >= os)
					{
						match = true;
					}
					break;
				case win7:
					if (OperatingSystemType(os) == win7 && os_in >= os)
					{
						match = true;
					}
				case win8:
					if (OperatingSystemType(os) == win8 && os_in >= os)
					{
						match = true;
					}
				case win8Server:
					if (OperatingSystemType(os) == win8Server && os_in >= os)
					{
						match = true;
					}
					break;
			}

			// Save the maximum filter os
			if (os > max_filter)
			{
				max_filter = os;
			}
		}

		// Handle the case where the os is later than any filters
		if (os_in >= max_filter)
		{
			match = true;
		}

		return match;
	} 
	else if (os_orlt.size() > 0)
	{
		OperatingSystem min_filter = winMax;
		bool match = false;

		// The os must be less than any of the values in the filter
		for each (OperatingSystem os in os_orlt)
		{
			// Match the input OS then check the filter value
			switch (OperatingSystemType(os_in))
			{
				case winNone:
					THROW_EX(L"Unsupported OS filter: " << filter);
					break;
				case winNT4:
					if (OperatingSystemType(os) == winNT4 && os_in < os)
					{
						match = true;
					}
					break;
				case win2000:
					if (OperatingSystemType(os) == win2000 && os_in < os)
					{
						match = true;
					}
					break;
				case winXP:
					if (OperatingSystemType(os) == winXP && os_in < os)
					{
						match = true;
					}
					break;
				case winServer2003:
					if (OperatingSystemType(os) == winServer2003 && os_in < os)
					{
						match = true;
					}
					break;
				case winServer2003R2:
					if (OperatingSystemType(os) == winServer2003R2 && os_in < os)
					{
						match = true;
					}
					break;
				case winVista:
					if (OperatingSystemType(os) == winVista && os_in < os)
					{
						match = true;
					}
					break;
				case winServer2008:
					if (OperatingSystemType(os) == winServer2008 && os_in < os)
					{
						match = true;
					}
					break;
				case win7:
					if (OperatingSystemType(os) == win7 && os_in < os)
					{
						match = true;
					}
				case win8:
					if (OperatingSystemType(os) == win8 && os_in < os)
					{
						match = true;
					}
				case win8Server:
					if (OperatingSystemType(os) == win8Server && os_in < os)
					{
						match = true;
					}
					break;
			}

			// Save the minimum filter os
			if (os < min_filter)
			{
				min_filter = os;
			}
		}

		// Handle the case where the os is older than any filters
		if (os_in < min_filter)
		{
			match = true;
		}

		return match;
	} 
	else 
	{
		for each (OperatingSystem os in os_andnot)
		{
			if (os == os_in)
			{
				return false;
			}
		}

		return true;
	}
}

// Get the operating system type
DVLib::OperatingSystem DVLib::OperatingSystemType(OperatingSystem os)
{
	if (os >= winNT4 && os <= winNT4Max)
	{
		return winNT4;
	}
	else if (os >= win2000 && os <= win2000Max)
	{
		return win2000;
	}
	else if (os >= winXP && os <= winXPMax)
	{
		return winXP;
	}
	else if (os >= winServer2003 && os <= winServer2003Max)
	{
		// Special case for Windows 2003 due to the R2 variants
		if (os == winServer2003R2 || os == winServer2003R2sp1 || os == winServer2003R2sp2)
		{
			return winServer2003R2;
		}
		else if (os == winServer2003 || os == winServer2003sp1 || os == winServer2003sp2)
		{
			return winServer2003;
		}
		else
		{
			// If there is a new service pack this could be R2 or non-R2
			return winServer2003;
		}
	}
	else if (os >= winVista && os <= winVistaMax)
	{
		return winVista;
	}
	else if (os >= winServer2008 && os <= winServer2008Max)
	{
		return winServer2008;
	}
	else if (os >= win7 && os <= win7Max)
	{
		return win7;
	}
	else if (os >= win8 && os <= win8Max)
	{
		return win8;
	}
	else if (os >= win8Server && os <= win8ServerMax)
	{
		return win8Server;
	}
	else if (os > win8Max)
	{
		return winMax;
	}
	else
	{
		return winNone;
	}
}

WORD DVLib::wstring2pa(const std::wstring& pa)
{
	for (int i = 0; i < ARRAYSIZE(DVLib::processor_architectures); i++)
	{
		if (pa == DVLib::processor_architectures[i].name)
		{
			return DVLib::processor_architectures[i].pa;
		}
	}

	THROW_EX("Invalid processor architecture: " << pa);
}

std::wstring DVLib::pa2wstring(WORD pa)
{
    for (int i = 0; i < ARRAYSIZE(DVLib::processor_architectures); i++)
    {
		if (pa == DVLib::processor_architectures[i].pa)
			return DVLib::processor_architectures[i].name;
    }

	THROW_EX("Invalid processor architecture: " << pa);
}

bool DVLib::IsWow64()
{
#ifdef _X86_
    BOOL bIsWow64 = FALSE;

	typedef BOOL (WINAPI * LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
    LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(
        GetModuleHandle(TEXT("kernel32")), "IsWow64Process");
  
    if (NULL != fnIsWow64Process)
    {
        fnIsWow64Process(GetCurrentProcess(), & bIsWow64);
    }

	return bIsWow64 ? true : false;
#else
	return false;
#endif
}

bool GetNativeSystemInfo(LPSYSTEM_INFO p)
{
	typedef void (WINAPI * LPFN_GETSYSTEMINFO)(LPSYSTEM_INFO);
    LPFN_GETSYSTEMINFO fnGetNativeSystemInfo = (LPFN_GETSYSTEMINFO) GetProcAddress(
        GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo");

    if (NULL == fnGetNativeSystemInfo)
		return false;

	fnGetNativeSystemInfo(p);
	return true;
}

WORD DVLib::GetProcessorArchitecture()
{
    SYSTEM_INFO info = {0};
	::GetSystemInfo(&info);

    if (info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
    {
        if (IsWow64())
        {
            GetNativeSystemInfo(& info);
        }
    }

    return info.wProcessorArchitecture;
}
	
bool DVLib::IsProcessorArchitecture(WORD pa, const std::wstring& pa_list)
{
	if (pa_list.empty())
		return true;

	std::vector<std::wstring> pa_vector = DVLib::split(pa_list, L",");

	for (size_t i = 0; i < pa_vector.size(); i++)
	{
		// tolerate blanks
		if (pa_vector[i].empty())
			continue;

		bool not = false;
		std::wstring pa_s;
		if (pa_vector[i][0] == L'!')
		{
			not = true;
			pa_s = pa_vector[i].substr(1);
		}
		else
		{
			pa_s = pa_vector[i];
		}

		WORD pa_c = wstring2pa(pa_s);
		if (not && pa_c == pa)
			return false;
		else if (! not && pa_c == pa)
			return true;
	}

	return false;
}


bool DVLib::Wow64DisableWow64FsRedirection(LPVOID * old_value)
{
	CHECK_BOOL(NULL != old_value,
		L"Wow64DisableWow64FsRedirection: missing old_value");

#ifdef _X86_
	typedef BOOL (WINAPI * LPFN_WOW64DISABLEWOW64FSREDIRECTION)(PVOID *);
	
	LPFN_WOW64DISABLEWOW64FSREDIRECTION fnWow64DisableWow64FsRedirection =
		(LPFN_WOW64DISABLEWOW64FSREDIRECTION)GetProcAddress(
			GetModuleHandle(TEXT("kernel32.dll")), "Wow64DisableWow64FsRedirection");

	if ( IsWow64())
	{
		CHECK_BOOL(NULL != fnWow64DisableWow64FsRedirection,
			L"Missing Wow64DisableWow64FsRedirection");

		CHECK_WIN32_BOOL(fnWow64DisableWow64FsRedirection(old_value),
			L"Wow64DisableWow64FsRedirection");
		
		return true;
	}
	else
	{
		return false;
	}
#else
	// dummy for AMD64, IA64
	return false;
#endif
}

bool DVLib::Wow64RevertWow64FsRedirection(LPVOID old_value)
{
#ifdef _X86_

	typedef BOOL (WINAPI * LPFN_WOW64REVERTWOW64FSREDIRECTION)(PVOID);

	LPFN_WOW64REVERTWOW64FSREDIRECTION fnWow64RevertWow64FsRedirection =
		(LPFN_WOW64REVERTWOW64FSREDIRECTION)GetProcAddress(
			GetModuleHandle(TEXT("kernel32.dll")),"Wow64RevertWow64FsRedirection");

	if (IsWow64())
	{
		CHECK_BOOL(NULL != fnWow64RevertWow64FsRedirection,
			L"Missing Wow64RevertWow64FsRedirection");

		CHECK_WIN32_BOOL(fnWow64RevertWow64FsRedirection(old_value),
			L"Wow64RevertWow64FsRedirection");

		return true;
	}
	else
	{
		return false;
	}
#else
	return false;
#endif
}


void DVLib::ExitWindowsSystem(DWORD ulFlags, DWORD ulReason)
{
    HANDLE hprocess = NULL; // handle to process token 
	TOKEN_PRIVILEGES tkp = { 0 }; // pointer to token structure     
	
    // get the current process token handle so we can get shutdown privilege         
	CHECK_WIN32_BOOL(::OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hprocess),
		L"OpenProcessToken");

	auto_handle hprocess_ptr(hprocess);

	// get the LUID for shutdown privilege         
	CHECK_WIN32_BOOL(::LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, & tkp.Privileges[0].Luid),
		L"LookupPrivilegeValue(SE_SHUTDOWN_NAME)"); 
         
	tkp.PrivilegeCount = 1;  // one privilege to set    
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
         
	// set shutdown privilege for this process
	CHECK_WIN32_BOOL(::AdjustTokenPrivileges(hprocess, FALSE, & tkp, 0, NULL, 0),
		L"AdjustTokenPrivileges");

	CHECK_WIN32_BOOL(::ExitWindowsEx(ulFlags, ulReason),
		L"ExitWindowsEx");
}


DVLib::LcidType DVLib::wstring2lcidtype(const std::wstring& name, LcidType defaultvalue)
{
	if (name.empty()) 
	{
		return defaultvalue;
	}

	for (int i = 0; i < ARRAYSIZE(LcidType2wstringMap); i++)
	{
		if (LcidType2wstringMap[i].name == name)
			return LcidType2wstringMap[i].lcidtype;
	}

	THROW_EX(L"Invalid LCID type: " << name);
}

std::wstring DVLib::lcidtype2wstring(LcidType lcidtype)
{
	for (int i = 0; i < ARRAYSIZE(LcidType2wstringMap); i++)
	{
		if (LcidType2wstringMap[i].lcidtype == lcidtype)
			return LcidType2wstringMap[i].name;
	}

	THROW_EX(L"Invalid LCID type: " << lcidtype);
}

DVLib::OperatingSystem DVLib::oscode2os(const std::wstring& oscode)
{
	if (oscode.empty())
		return winNone;

	for (int i = 0; i < ARRAYSIZE(Os2StringMap); i++)
	{
		if (Os2StringMap[i].oscode == oscode)
		{
			return Os2StringMap[i].os;
		}
	}

	THROW_EX(L"Unsupported operating system code: " << oscode);
}

std::wstring DVLib::os2wstring(OperatingSystem os)
{
	if (os == winNone)
		return L"";

	for (int i = 0; i < ARRAYSIZE(Os2StringMap); i++)
	{
		if (Os2StringMap[i].os == os)
		{
			return Os2StringMap[i].name;
		}
	}

	THROW_EX(L"Unsupported operating system, os=" << (int) os);
}

std::wstring DVLib::GetISOLocale(LCID lcid)
{
	std::wstringstream ss;
	ss << GetLocale(lcid, LOCALE_SISO639LANGNAME);
	ss << L"-";
	ss << GetLocale(lcid, LOCALE_SISO3166CTRYNAME);
	return ss.str();
}

std::wstring DVLib::GetLocale(LCID lcid, int format)
{
	int size = 0;
	CHECK_WIN32_BOOL((size = ::GetLocaleInfoW(lcid, format, NULL, 0)) > 0,
		L"GetLocaleInfoW");
	std::wstring locale;
	locale.resize(size);
	CHECK_WIN32_BOOL(::GetLocaleInfoW(lcid, format, & * locale.begin(), locale.size()) > 0,
		L"GetLocaleInfoW");
	locale.resize(size - 1);
	return locale;
}

LCID DVLib::MuiGetSystemDefaultUILCID()
{
	typedef LANGID (WINAPI * LPFN_GetSystemDefaultUILanguage)();

	LPFN_GetSystemDefaultUILanguage fnGetSystemDefaultUILanguage =
		(LPFN_GetSystemDefaultUILanguage) GetProcAddress(
			GetModuleHandle(L"kernel32.dll"), "GetSystemDefaultUILanguage");

	CHECK_BOOL(NULL != fnGetSystemDefaultUILanguage,
		L"Missing GetSystemDefaultUILanguage");

	LANGID id = fnGetSystemDefaultUILanguage();
	return MAKELCID(id, SORT_DEFAULT);
}

LCID DVLib::MuiGetUserDefaultUILCID()
{
	typedef LANGID (WINAPI * LPFN_GetUserDefaultUILanguage)();

	LPFN_GetUserDefaultUILanguage fnGetUserDefaultUILanguage =
		(LPFN_GetUserDefaultUILanguage) GetProcAddress(
			GetModuleHandle(L"kernel32.dll"), "GetUserDefaultUILanguage");

	CHECK_BOOL(NULL != fnGetUserDefaultUILanguage,
		L"Missing GetUserDefaultUILanguage");

	LANGID id = fnGetUserDefaultUILanguage();
	return MAKELCID(id, SORT_DEFAULT);
}
