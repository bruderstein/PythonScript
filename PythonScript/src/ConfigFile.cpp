#include "stdafx.h"

#include "ConfigFile.h"
#include "resource.h"
#include "WcharMbcsConverter.h"

ConfigFile* ConfigFile::s_instance;

ConfigFile* ConfigFile::create(const TCHAR *configDir, const TCHAR *pluginDir, HINSTANCE hInst)
{
	ConfigFile::s_instance = new ConfigFile(configDir, pluginDir, hInst);
	return ConfigFile::s_instance;
}


ConfigFile::ConfigFile(const TCHAR *configDir, const TCHAR *pluginDir, HINSTANCE hInst)
	: m_hInst (hInst),
      m_configFilename(configDir),
	  m_pluginDir(pluginDir),
	  m_machineScriptsDir(pluginDir),
	  m_userScriptsDir(configDir),
	  m_configDir(configDir)
{
	m_configFilename.append(_T("\\PythonScriptStartup.cnf"));

	m_machineScriptsDir.append(_T("\\PythonScript\\scripts"));
	m_userScriptsDir.append(_T("\\PythonScript\\scripts"));

	initSettings();
	readConfig();
}


ConfigFile::~ConfigFile()
{
	m_hInst = NULL;
	// TODO: Clean up
	// DeleteImage
	//

}

void ConfigFile::initSettings()
{
	setSetting(_T("ADDEXTRALINETOOUTPUT"), _T("0"));
	setSetting(_T("COLORIZEOUTPUT"), _T("-1"));
	setSetting(_T("OPENCONSOLEONERROR"), _T("0"));
	setSetting(_T("PREFERINSTALLEDPYTHON"), _T("0"));
	setSetting(_T("STARTUP"), _T("LAZY"));
}

void ConfigFile::readConfig()
{
	//just char(UTF8) as TCHAR is not working as expected, because stream is converted to char implicitly
	//see also https://www.codeproject.com/Articles/38242/Reading-UTF-with-C-streams
	std::ifstream startupFile(m_configFilename.c_str());

	char buffer[500];


	HBITMAP hIcon;

	while (startupFile.good())
	{
		tstring scriptFullPath = _T("");
		startupFile.getline(buffer, 500);
		char *context;
		char *element = strtok_s(buffer, "/", &context);
		if (element)
		{
			// Menu item
			if (0 == strcmp(element, "ITEM"))
			{
				element = strtok_s(NULL, "/", &context);
				if ((element[1] == ':') || (element[1] == '\\'))
				{
					scriptFullPath = WcharMbcsConverter::char2tchar(element).get();
				}
				else
				{
					scriptFullPath.append(m_userScriptsDir).append(tstring(WcharMbcsConverter::char2tchar(element).get()));
				}
				m_menuItems.push_back(scriptFullPath);
				m_menuScripts.push_back(scriptFullPath);
			}

			// Toolbar item
			else if (0 == strcmp(element, "TOOLBAR"))
			{
				tstring iconFullPath = _T("");
				element = strtok_s(NULL, "/", &context);
				if ((element[1] == ':') || (element[1] == '\\'))
				{
					scriptFullPath = WcharMbcsConverter::char2tchar(element).get();
				}
				else
				{
					scriptFullPath.append(m_userScriptsDir).append(tstring(WcharMbcsConverter::char2tchar(element).get()));
				}

				char *iconPath = strtok_s(NULL, "/", &context);
				if (!iconPath || !(*iconPath))
				{
					hIcon = static_cast<HBITMAP>(LoadImage(m_hInst, MAKEINTRESOURCE(IDB_PYTHON), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE));
					iconPath = NULL;
				}
				else
				{
					if ((iconPath[1] == ':') || (iconPath[1] == '\\'))
					{
						iconFullPath = WcharMbcsConverter::char2tchar(iconPath).get();
					}
					else
					{
						iconFullPath.append(m_userScriptsDir).append(tstring(WcharMbcsConverter::char2tchar(iconPath).get()));
					}
					hIcon = static_cast<HBITMAP>(LoadImage(NULL, iconFullPath.c_str(), IMAGE_BITMAP, 16, 16, LR_LOADMAP3DCOLORS | LR_LOADFROMFILE));			
				}
				m_toolbarItems.push_back(std::pair<tstring, std::pair<HBITMAP, tstring> >(scriptFullPath, std::pair<HBITMAP, tstring>(hIcon, iconPath ? iconFullPath : tstring())));
			}
			else if (0 == strcmp(element, "SETTING"))
			{
				element = strtok_s(NULL, "/", &context);
				char *settingValue = strtok_s(NULL, "/", &context);
				m_settings.insert_or_assign(tstring(WcharMbcsConverter::char2tchar(element).get()), tstring(WcharMbcsConverter::char2tchar(settingValue).get()));
			}
		}

	}
	startupFile.close();
}

void ConfigFile::clearItems()
{
	m_menuItems.erase(m_menuItems.begin(), m_menuItems.end());
	m_menuScripts.erase(m_menuScripts.begin(), m_menuScripts.end());
	m_toolbarItems.erase(m_toolbarItems.begin(), m_toolbarItems.end());
}

void ConfigFile::save()
{
	//just char(UTF8) as TCHAR is not working as expected, because stream is converted to char implicitly
	//see also https://www.codeproject.com/Articles/38242/Reading-UTF-with-C-streams
	std::string userScriptsDir(WcharMbcsConverter::tchar2char((m_userScriptsDir).c_str()).get());

	std::ofstream startupFile(m_configFilename.c_str(), std::ios_base::out | std::ios_base::trunc);
	for(MenuItemsTD::iterator it = m_menuItems.begin(); it != m_menuItems.end(); ++it)
	{
		std::string scriptFullPath = WcharMbcsConverter::tchar2char((*it).c_str()).get();
		if (scriptFullPath.find(userScriptsDir, 0) == 0)
		{
			startupFile << "ITEM/" << scriptFullPath.replace(0, userScriptsDir.length(), "") << "\n";
		}
		else
		{
			startupFile << "ITEM/" << scriptFullPath << "\n";
		}
			
	}

	for(ToolbarItemsTD::iterator it = m_toolbarItems.begin(); it != m_toolbarItems.end(); ++it)
	{
		std::string scriptFullPath = WcharMbcsConverter::tchar2char((it->first).c_str()).get();
		if (scriptFullPath.find(userScriptsDir,0) == 0)
		{
			scriptFullPath = scriptFullPath.replace(0, userScriptsDir.length(), "");
		}

		std::string iconFullPath = WcharMbcsConverter::tchar2char((it->second.second).c_str()).get();
		if (iconFullPath.find(userScriptsDir, 0) == 0)
		{
			iconFullPath = iconFullPath.replace(0, userScriptsDir.length(), "");
		}

		startupFile << "TOOLBAR/" << scriptFullPath << "/" << iconFullPath << "\n";
	}

	for(SettingsTD::iterator it = m_settings.begin(); it != m_settings.end(); ++it)
	{
		startupFile << "SETTING/" << WcharMbcsConverter::tchar2char((it->first).c_str()).get() << "/" << WcharMbcsConverter::tchar2char((it->second).c_str()).get() << "\n";
	}

	startupFile.close();

}


void ConfigFile::addMenuItem(const tstring scriptPath)
{
	m_menuItems.push_back(scriptPath);
	m_menuScripts.push_back(scriptPath);
}

void ConfigFile::addToolbarItem(const tstring scriptPath, const tstring iconPath)
{
	HBITMAP hIcon = static_cast<HBITMAP>(LoadImage(m_hInst, iconPath.c_str(), IMAGE_BITMAP, 16, 16, LR_LOADMAP3DCOLORS));
	m_toolbarItems.push_back(std::pair<tstring, std::pair<HBITMAP, tstring> >(scriptPath, std::pair<HBITMAP, tstring>(hIcon, iconPath)));
}

void ConfigFile::setSetting(const tstring& settingName, const tstring settingValue)
{
	m_settings[settingName] = settingValue;
}

const tstring& ConfigFile::getSetting(const TCHAR *settingName)
{
	return m_settings[tstring(settingName)];
}

const tstring& ConfigFile::getMenuScript(idx_t index) const
{
	if (m_menuScripts.size() > index)
	{
		return m_menuScripts[index];
	}
	else
	{
		return m_emptyString;
	}
}
