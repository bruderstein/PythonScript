#include "stdafx.h"

#include "ConfigFile.h"
#include "resource.h"
#include "WcharMbcsConverter.h"

using namespace std;

ConfigFile* ConfigFile::s_instance;

ConfigFile* ConfigFile::create(const TCHAR *configDir, const TCHAR *pluginDir, HINSTANCE hInst)
{
	ConfigFile::s_instance = new ConfigFile(configDir, pluginDir, hInst);
	return ConfigFile::s_instance;
}


ConfigFile::ConfigFile(const TCHAR *configDir, const TCHAR *pluginDir, HINSTANCE hInst)
	: m_configFilename(configDir),
	  m_pluginDir(pluginDir),
	  m_hInst (hInst),
	  m_machineScriptsDir(pluginDir),
	  m_userScriptsDir(configDir),
	  m_configDir(configDir)
{
	m_configFilename.append(_T("\\PythonScriptStartup.cnf"));
	
	m_machineScriptsDir.append(_T("\\PythonScript\\scripts"));
	m_userScriptsDir.append(_T("\\PythonScript\\scripts"));

	readConfig();
}


ConfigFile::~ConfigFile()
{
	// TODO: Clean up 
	// DeleteImage
	// 
	
}


void ConfigFile::readConfig()
{
	basic_ifstream<TCHAR> startupFile(m_configFilename.c_str());
	
	TCHAR buffer[500];
	
	
	HBITMAP hIcon;

	while (startupFile.good())
	{
		startupFile.getline(buffer, 500);
		TCHAR *context;
		TCHAR *element = _tcstok_s(buffer, _T("/"), &context);
		if (element)
		{

			// Menu item
			if (0 == _tcscmp(element, _T("ITEM")))
			{
				element = _tcstok_s(NULL, _T("/"), &context);
				m_menuItems.push_back(tstring(element));
				m_menuScripts.push_back(string(WcharMbcsConverter::tchar2char(element).get()));
			}
		
			// Toolbar item
			else if (0 == _tcscmp(element, _T("TOOLBAR")))
			{
				element = _tcstok_s(NULL, _T("/"), &context);
				TCHAR *iconPath = _tcstok_s(NULL, _T("/"), &context);
				if (!iconPath || !(*iconPath))
				{
					hIcon = static_cast<HBITMAP>(LoadImage(m_hInst, MAKEINTRESOURCE(IDB_PYTHON), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE));
					iconPath = NULL;
				}
				else 
				{
					hIcon = static_cast<HBITMAP>(LoadImage(NULL, iconPath, IMAGE_BITMAP, 16, 16, LR_LOADMAP3DCOLORS | LR_LOADFROMFILE));
				}

			
				m_toolbarItems.push_back(pair<tstring, pair<HBITMAP, tstring> >(tstring(element), pair<HBITMAP, tstring>(hIcon, iconPath ? tstring(iconPath) : tstring())));
			}
			else if (0 == _tcscmp(element, _T("SETTING")))
			{
				element = _tcstok_s(NULL, _T("/"), &context);
				TCHAR *settingValue = _tcstok_s(NULL, _T("/"), &context);
				m_settings.insert(pair<tstring, tstring>(tstring(element), tstring(settingValue)));
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
	basic_ofstream<TCHAR> startupFile(m_configFilename.c_str(), ios_base::out | ios_base::trunc);
	for(MenuItemsTD::iterator it = m_menuItems.begin(); it != m_menuItems.end(); ++it)
	{
		startupFile << "ITEM/" << (*it) << "\n";
	}

	for(ToolbarItemsTD::iterator it = m_toolbarItems.begin(); it != m_toolbarItems.end(); ++it)
	{
		startupFile << _T("TOOLBAR/") << it->first << _T("/") << it->second.second << _T("\n");
	}

	for(SettingsTD::iterator it = m_settings.begin(); it != m_settings.end(); ++it)
	{
		startupFile << _T("SETTING/") << it->first << _T("/") << it->second << _T("\n");
	}

	startupFile.close();

}


void ConfigFile::addMenuItem(const tstring scriptPath)
{
	m_menuItems.push_back(scriptPath);
	m_menuScripts.push_back(string(WcharMbcsConverter::tchar2char(scriptPath.c_str()).get()));
}

void ConfigFile::addToolbarItem(const tstring scriptPath, const tstring iconPath)
{
	HBITMAP hIcon = static_cast<HBITMAP>(LoadImage(m_hInst, iconPath.c_str(), IMAGE_BITMAP, 16, 16, LR_LOADMAP3DCOLORS));
	m_toolbarItems.push_back(pair<tstring, pair<HBITMAP, tstring> >(scriptPath, pair<HBITMAP, tstring>(hIcon, iconPath)));
}

void ConfigFile::setSetting(const tstring& settingName, const tstring settingValue)
{
	m_settings[settingName] = settingValue;
}

const tstring& ConfigFile::getSetting(const TCHAR *settingName)
{
	return m_settings[tstring(settingName)];
}
