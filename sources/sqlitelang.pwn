/*
 * SQLite based include that easily allows to manipulate multilanguage strings.
 * Copyright (C) 2013 Giampaolo Falqui
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,  
 * but WITHOUT ANY WARRANTY; without even the implied warranty of  
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
*/

/*
 * Available functions:
 *
 * SQLiteLang_Initialize(file[])
 * SQLiteLang_Terminate()
 * SQLiteLang_AddString(identifier[], lang[], string[])
 * SQLiteLang_AddLanguage(lang[])
 * SQLiteLang_DeleteLanguage(lang[])
 * SQLiteLang_DeleteString(identifier[])
 * SQLiteLang_DeleteLanguageString(identifier[], lang[])
 * SQLiteLang_ModifyLanguageString(identifier[], lang[], new_string[])
 * SQLiteLang_ShowLanguageString(identifier[], lang[])
 * SQLiteLang_ShowPlayerString(playerid, identifier[])
 * SQLiteLang_SetDefaultLang(lang[])
 * SQLiteLang_SetPlayerLang(playerid, lang[])
*/

/*
 * Includes
*/

#include 		<a_samp>
#include 		<sqlitei>

/*
 * Variables Declaration
*/

enum SQLiteLang_playerInformation {
	playerLang = 0,
};

enum SQLiteLang_internalInformation {
	bool: systemStatus,
	bool: debugStatus,
	DB: databaseHandler,
	databaseDirectory[512],		
};

new SQLiteLang_playerVariables[MAX_PLAYERS][SQLiteLang_playerInformation];
new SQLiteLang_internalVariables[SQLiteLang_internalInformation];

/*
 * Hooks
*/

// OnGameModeInit
public OnGameModeInit(playerid)
{
    #if defined SQLiteLang_OnGameModeInit
		SQLiteLang_OnGameModeInit(playerid);			
    #endif
    return 1;
}

public SQLiteLang_OnGameModeInit() 
{
	// By default, SQLiteLang is turned off.
	SQLiteLang_internalVariables[systemStatus] = false;
	
	// By default, SQLiteLang debugging mode is turned off.
	SQLiteLang_internalVariables[debugStatus] = false;	
	
	// By default, the directory is set as the following.
	SQLiteLang_internalVariables[databaseDirectory] = "SQLiteLang/Database.db";
	return 1;
}

#if defined _ALS_OnGameModeInit
    #undef OnGameModeInit
#else
    #define _ALS_OnGameModeInit
#endif
#define OnGameModeInit SQLiteLang_OnGameModeInit
#if defined SQLiteLang_OnGameModeInit
    forward SQLiteLang_OnGameModeInit(playerid);
#endif

// OnPlayerConnect
public OnPlayerConnect(playerid)
{
    #if defined SQLiteLang_OnPlayerConnect	
		if(SQLiteLang_internalVariables[systemStatus])
			SQLiteLang_OnPlayerConnect(playerid);			
    #endif
    return 1;
}

public SQLiteLang_OnPlayerConnect(playerid) 
{
	SendClientMessage(playerid, 000000, "{A4B2B3}[SQLiteLang] {FFFFFF}This server uses \"SQLiteLang\". Use /sqlitelang to select an available language.");
	return 1;
}

#if defined _ALS_OnPlayerConnect
    #undef OnPlayerConnect
#else
    #define _ALS_OnPlayerConnect
#endif
#define OnPlayerConnect SQLiteLang_OnPlayerConnect
#if defined SQLiteLang_OnPlayerConnect
    forward SQLiteLang_OnPlayerConnect(playerid);
#endif

/*
 * Functions
*/

/*
 * SQLiteLang_Initialize
 * This function declares the name of the database and saves it into a variable (both file and directory).
 * If the database does not exist, a new one will be created by default in the folder 'SQLiteLang'.
 *
 * optional: file[] ("SQLiteLang/Database.db") = Directory and file name of the database.
 * optional: debug (false) = Turning this on will activate the debugging mode.
*/
SQLiteLang_Initialize(file[] = "SQLiteLang/Database.db", debug = false)
{
	if(debug)
		SQLiteLang_internalVariables[debugStatus] = true;
		
	if(strcmp(file, "SQLiteLang/Database.db", false)) 
	{				
		strcat(SQLiteLang_internalVariables[databaseDirectory], file, 512);	
		
		if(SQLiteLang_internalVariables[debugStatus])
			printf("SQLiteLang_DEBUG: SQLiteLang initialization started. Custom directory selected: \"%s\".", SQLiteLang_internalVariables[databaseDirectory]);		
	}
		
	if(!fexist("SQLiteLang/Database.db")) 
	{
		if(SQLiteLang_internalVariables[debugStatus])
			print("SQLiteLang_DEBUG: The database does not exist, automatic creation has been started.");
		
		SQLiteLang_internalVariables[databaseHandler] = db_open("SQLiteLang/Database.db");
		
		if(!fexist("SQLiteLang/Database.db"))
		{	
			if(SQLiteLang_internalVariables[debugStatus])
				print("SQLiteLang_DEBUG: The system failed to create the database (the folder 'SQLiteLang' does not exist).");								
		}
		else
		{
			db_exec(SQLiteLang_internalVariables[databaseHandler], "CREATE TABLE IF NOT EXISTS `strings` (`internal_identifier` INTEGER PRIMARY KEY NOT NULL UNIQUE)");
			db_exec(SQLiteLang_internalVariables[databaseHandler], "ALTER TABLE `strings` ADD `identifier` VARCHAR");
			db_exec(SQLiteLang_internalVariables[databaseHandler], "ALTER TABLE `strings` ADD `lang` VARCHAR");
			db_exec(SQLiteLang_internalVariables[databaseHandler], "ALTER TABLE `strings` ADD `text` VARCHAR");	
			
			db_exec(SQLiteLang_internalVariables[databaseHandler], "CREATE TABLE IF NOT EXISTS `languages` (`internal_identifier` INTEGER PRIMARY KEY NOT NULL UNIQUE)");
			db_exec(SQLiteLang_internalVariables[databaseHandler], "ALTER TABLE `languages` ADD `lang_name` VARCHAR");
			
			print("SQLiteLang_DEBUG: SQLiteLang successfully started.");
			SQLiteLang_internalVariables[systemStatus] = true;
		}
	}
	else
	{
		print("SQLiteLang_DEBUG: SQLiteLang successfully started.");
		SQLiteLang_internalVariables[systemStatus] = true;
	}
	
	return 1;
}

/*
 * SQLiteLang_Terminate
 * This function terminates the usage of the include and removes the saved name and directory of the database internally.
*/
SQLiteLang_Terminate()
{
	return 1;
}

/*
 * SQLiteLang_AddString
 * This function adds new strings to the database.
 * 
 * identifier[] = The identifier is used to identify, as the parameter says, every string of text (ie. "welcome_string").
 * lang[] = The lang is used to identify the language of the string (ie. "en").
 * string[] = The string is the actual string of text to show in different languages.
*/
SQLiteLang_AddString(identifier[], lang[], string[])
{
	return 1;
}

/*
 * SQLiteLang_AddLanguage
 * This function adds a new language to the database.
 *
 * lang[] = The lang is used to identify the language of the string (ie. "en").
*/
SQLiteLang_AddLanguage(lang[])
{
	return 1;
}

/*
 * SQLiteLang_DeleteLanguage
 * This function deletes a specific language from database.
 * Be careful, deleting a language will delete all the language strings already added in the database.
 * Furthermore, you may not delete the default language, if set.
 *
 * lang[] = The lang is used to identify the language of the string (ie. "en").
*/
SQLiteLang_DeleteLanguage(lang[])
{
	return 1;
}

/*
 * SQLiteLang_DeleteString
 * This function deletes all the language strings of a specific identifier from the database.
 * 
 * identifier[] = The identifier is used to identify, as the parameter says, every string of text (ie. "welcome_string").
*/
SQLiteLang_DeleteString(identifier[])
{
	return 1;
}

/*
 * SQLiteLang_DeleteLanguageString
 * This function deletes a specific language string of a specific identifier from the database.
 * 
 * identifier[] = The identifier is used to identify, as the parameter says, every string of text (ie. "welcome_string").
 * lang[] = The lang is used to identify the language of the string (ie. "en").
*/
SQLiteLang_DeleteLanguageString(identifier[], lang[])
{
	return 1;
}

/*
 * SQLiteLang_ModifyLanguageString
 * This function modify a specific language string in database.
 * 
 * identifier[] = The identifier is used to identify, as the parameter says, every string of text (ie. "welcome_string").
 * lang[] = The lang is used to identify the language of the string (ie. "en").
 * new_string[] = The string is the new actual string of text to show in different languages that will replace the old one.
*/
SQLiteLang_ModifyLanguageString(identifier[], lang[], new_string[])
{
	return 1;
}

/*
 * SQLiteLang_ShowLanguageString
 * This function shows a specific language string taken from the database.
 * 
 * identifier[] = The identifier is used to identify, as the parameter says, every string of text (ie. "welcome_string").
 * lang[] = The lang is used to identify the language of the string (ie. "en").
*/
SQLiteLang_ShowLanguageString(identifier[], lang[])
{
	return 1;
}

/*
 * SQLiteLang_ShowPlayerString
 * This function shows a specific language string taken from the database.
 * 
 * playerid = playerid of the player.
 * identifier[] = The identifier is used to identify, as the parameter says, every string of text (ie. "welcome_string").
*/
SQLiteLang_ShowPlayerString(playerid, identifier[])
{
	return 1;
}

/*
 * SQLiteLang_SetDefaultLang
 * This function sets the default language for users that haven't selected an available language.
 * 
 * identifier[] = The identifier is used to identify, as the parameter says, every string of text (ie. "welcome_string").
 * lang[] = The lang is used to identify the language of the string (ie. "en").
*/
SQLiteLang_SetDefaultLang(lang[])
{
	return 1;
}

/*
 * SQLiteLang_SetPlayerLang
 * This function sets the language of a player.
 * 
 * playerid = playerid of the player.
 * lang[] = The lang is used to identify the language of the string (ie. "en").
*/
SQLiteLang_SetPlayerLang(playerid, lang[])
{
	return 1;
}