#ifndef _CONFIGFILE_H 
#define _CONFIGFILE_H

#include "stdafx.h"

class ConfigFile
{
public:
	typedef std::vector< tstring > MenuItemsTD;
	typedef std::vector< std::pair<tstring, HICON> > ToolbarItemsTD;
	
	ConfigFile(const TCHAR *configDir, const TCHAR *pluginDir, HINSTANCE hInst);
	virtual ~ConfigFile();

	static ConfigFile* create(const TCHAR *configDir, const TCHAR *pluginDir, HINSTANCE hInst);
	static ConfigFile* getInstance()   { return s_instance; };

	tstring getSetting(const TCHAR* settingName);

	// TODO: Need to make these pointers
	MenuItemsTD getMenuItems()  { return m_menuItems; };
	ToolbarItemsTD getToolbarItems()  { return m_toolbarItems; };
	std::string& getMenuScript(int index) { return m_menuScripts[index]; }
	

protected:
	explicit ConfigFile(const TCHAR *configDir);
	virtual void readConfig();

private:
	HINSTANCE m_hInst;
	static ConfigFile* s_instance;

	tstring m_configFilename;
	tstring m_pluginDir;

	MenuItemsTD m_menuItems;
	std::vector< std::string > m_menuScripts;

	ToolbarItemsTD m_toolbarItems;
	std::map< tstring, HICON > m_icons;
	std::map< tstring, tstring > m_settings;

};

#endif