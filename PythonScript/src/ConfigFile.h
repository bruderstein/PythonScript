#ifndef _CONFIGFILE_H
#define _CONFIGFILE_H

class ConfigFile
{
public:
	typedef std::vector< tstring > MenuItemsTD;
	typedef std::vector< std::pair<tstring, std::pair<HBITMAP, tstring> > > ToolbarItemsTD;
	typedef std::map< tstring, tstring > SettingsTD;

	ConfigFile(const TCHAR *configDir, const TCHAR *pluginDir, HINSTANCE hInst);
	virtual ~ConfigFile();

	static ConfigFile* create(const TCHAR *configDir, const TCHAR *pluginDir, HINSTANCE hInst);
	static ConfigFile* getInstance()   { return s_instance; };



	// TODO: Need to make these pointers
	MenuItemsTD getMenuItems()  { return m_menuItems; };
	ToolbarItemsTD getToolbarItems()  { return m_toolbarItems; };
	const tstring& getMenuScript(idx_t index) const;

	void addMenuItem(const tstring scriptPath);
	void addToolbarItem(const tstring scriptPath, const tstring iconPath);
	void setSetting(const tstring& settingName, const tstring settingValue);
	const tstring& getSetting(const TCHAR* settingName);

	void clearItems();
	void save();

	void refresh() { clearItems(); readConfig(); };


	const tstring& getMachineScriptsDir() { return m_machineScriptsDir; };
	const tstring& getUserScriptsDir() { return m_userScriptsDir; };
	const tstring& getConfigDir() { return m_configDir; };

protected:
	void readConfig();

private:
	ConfigFile(); // default constructor disabled

	HINSTANCE m_hInst;
	static ConfigFile* s_instance;

	tstring m_configFilename;
	tstring m_pluginDir;

	tstring m_machineScriptsDir;
	tstring m_userScriptsDir;
	tstring m_configDir;

	MenuItemsTD m_menuItems;
	std::vector< tstring > m_menuScripts;

	// Used in case an invalid script number is requested
	// so we can return a reference to this puppy instead.
	tstring m_emptyString;


	ToolbarItemsTD m_toolbarItems;
	std::map< tstring, HICON > m_icons;
	SettingsTD m_settings;

};

#endif
