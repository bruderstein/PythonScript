#ifndef _CONFIGFILE_H 
#define _CONFIGFILE_H

#include "stdafx.h"

class ConfigFile
{
public:
	typedef std::vector< tstring > MenuItemsTD;
	typedef std::vector< std::pair<tstring, std::pair<HICON, tstring> > > ToolbarItemsTD;
	typedef std::map< tstring, tstring > SettingsTD;

	ConfigFile(const TCHAR *configDir, const TCHAR *pluginDir, HINSTANCE hInst);
	virtual ~ConfigFile();

	static ConfigFile* create(const TCHAR *configDir, const TCHAR *pluginDir, HINSTANCE hInst);
	static ConfigFile* getInstance()   { return s_instance; };

	tstring getSetting(const TCHAR* settingName);

	// TODO: Need to make these pointers
	MenuItemsTD getMenuItems()  { return m_menuItems; };
	ToolbarItemsTD getToolbarItems()  { return m_toolbarItems; };
	std::string& getMenuScript(int index) { return m_menuScripts[index]; }
	
	void addMenuItem(const tstring scriptPath);
	void addToolbarItem(const tstring scriptPath, const tstring iconPath);
	void setSetting(const tstring& settingName, const tstring settingValue);

	void clearItems();
	void save();

	void refresh() { clearItems(); readConfig(); };


	const tstring& getMachineScriptsDir() { return m_machineScriptsDir; };
	const tstring& getUserScriptsDir() { return m_userScriptsDir; };


protected:
	explicit ConfigFile(const TCHAR *configDir);
	virtual void readConfig();

private:
	HINSTANCE m_hInst;
	static ConfigFile* s_instance;

	tstring m_configFilename;
	tstring m_pluginDir;

	tstring m_machineScriptsDir;
	tstring m_userScriptsDir;

	MenuItemsTD m_menuItems;
	std::vector< std::string > m_menuScripts;

	ToolbarItemsTD m_toolbarItems;
	std::map< tstring, HICON > m_icons;
	SettingsTD m_settings;

};

#endif