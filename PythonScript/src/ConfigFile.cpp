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
	  m_hInst (hInst)
{
	m_configFilename.append(_T("\\PythonScriptStartup.cnf"));

	readConfig();
}


ConfigFile::~ConfigFile()
{
	// TODO: Clean up 
}


void ConfigFile::readConfig()
{
	basic_ifstream<TCHAR> startupFile(m_configFilename.c_str());
	
	TCHAR buffer[500];
	
	
	HICON defaultIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_PYTHON));
	HICON hIcon;

	while (startupFile.good())
	{
		startupFile.getline(buffer, 500);
		TCHAR *context;
		TCHAR *element = _tcstok_s(buffer, _T("/"), &context);

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
			if (!iconPath && *iconPath)
			{
				hIcon = defaultIcon;
			}
			else 
			{
				hIcon = static_cast<HICON>(LoadImage(NULL, iconPath, IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));
			}

			
			m_toolbarItems.push_back(pair<tstring, HICON>(tstring(element), hIcon));

		}

	}
	startupFile.close();
}

