#include "StdAfx.h"
#include "ConfigFileManager.h"
#include "dotNetInstallerDlg.h"
#include <Version/Version.h>

ConfigFileManager::ConfigFileManager()
{
}

bool ConfigFileManager::OnVersionError(const std::wstring& version, const std::wstring& filename)
{
    std::wstring version_message = DVLib::FormatMessageW(
        L"Configuration %s version %s does not match bootstrapper version.\r\n" \
        L"Open and re-save configuration.xml with editor version %s.\r\n" \
        L"Continue with installation?", filename.c_str(), version.c_str(), TEXT(VERSION_VALUE));

    return (IDYES != DniMessageBox::Show(version_message, MB_YESNO|MB_ICONQUESTION, IDYES));
}

bool ConfigFileManager::OnDownload(const ConfigurationPtr& config)
{
    ReferenceConfiguration * p = reinterpret_cast<ReferenceConfiguration *>(get(config));
    LOG(L"Downloading reference configuration to '" << p->filename << L"'");
    dlg.RunDownloadConfiguration(p->downloaddialog);
    return true;
}

bool ConfigFileManager::OnRunConfiguration(const ConfigurationPtr& configuration)
{
    return dlg.RunInstallConfiguration(configuration, configuration != (* this)[size() - 1]);
}

void ConfigFileManager::Load()
{
    ConfigFiles::Load();
}

bool ConfigFileManager::OnLoad()
{
    if (! InstallerCommandLineInfo::Instance->configFile.empty())
    {
        CHECK_BOOL(DVLib::FileExists(InstallerCommandLineInfo::Instance->configFile),
            L"Missing '" << InstallerCommandLineInfo::Instance->configFile << L"'");

        config.LoadFile(InstallerCommandLineInfo::Instance->configFile);
        return true;
    }
    else if (NULL != DVLib::ResourceExists(AfxGetApp()->m_hInstance, TEXT("RES_CONFIGURATION"), TEXT("CUSTOM")))
    {
        config.LoadResource(AfxGetApp()->m_hInstance, TEXT("RES_CONFIGURATION"), TEXT("CUSTOM"));
        return true;
    }

    return false;
}

int ConfigFileManager::Run()
{
    ConfigFiles::Run();
    return dlg.GetRecordedError();
}
 
ConfigFiles::LanguageSelection ConfigFileManager::OnSelectLanguage()
{
    if (InstallerCommandLineInfo::Instance->DisplayConfig())
		return LanguageSelection_NotSelected;

    if (config.show_language_selector && ! InstallUILevelSetting::Instance->IsSilent())
    {
        CLanguageSelectorDialog lsdlg(config);
		if (lsdlg.DoModal() == IDOK)
			return LanguageSelection_Selected;
		else
			// Close or cancel of the setup process at the language selection 
			// stage is a normal situation and it does not throw error!
			return LanguageSelection_Cancel;
		//else
		//	ConfigFileManager::end();
        //switch(lsdlg.DoModal())
        //{
        //case IDOK:
        //   return true;
        //case IDCANCEL:
        //    THROW_EX(L"Language selection cancelled by user");
        //}
    }

    return LanguageSelection_NotSelected;
}

std::wstring ConfigFileManager::GetString() const
{
    return ConfigFiles::GetString();
}

std::vector<ConfigurationPtr> ConfigFileManager::DownloadReferenceConfigurations(
    LCID oslcid, const std::vector<ConfigurationPtr>& configs, int level)
{
    if (InstallerCommandLineInfo::Instance->DisplayConfig())
        return config; // return this configuration, all components

    return ConfigFiles::DownloadReferenceConfigurations(oslcid, configs, level);
}
