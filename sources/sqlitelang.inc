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
 * (DONE!) SQLiteLang_Initialize(file[], bool: init_debug)
 * (DONE!) SQLiteLang_Terminate()
 *
 * (DONE!) SQLiteLang_AddIdentifier(identifier[], description[])
 * (DONE!) SQLiteLang_DeleteIdentifier(identifier[])
 * (DONE!) SQLiteLang_ModifyIdentifier(old_identifier[], new_identifier[])
 * (DONE!) SQLiteLang_ModifyIdentifierDesc(identifier[], new_desc[])
 *
 * (DONE!) SQLiteLang_AddLanguage(lang[], description[])
 * (DONE!) SQLiteLang_DeleteLanguage(lang[])
 * TODO: SQLiteLang_ModifyLanguage(old_lang[], new_lang[])
 * TODO: SQLiteLang_ModifyLanguageDesc(lang[], new_desc[])
 *
 * (DONE!) SQLiteLang_AddString(identifier[], lang[], string[])
 * (DONE!) SQLiteLang_DeleteString(identifier[], lang[])
 * (DONE!) SQLiteLang_ModifyString(identifier[], lang[], new_string[])
 *
 * (DONE!) SQLiteLang_ShowLanguageString(identifier[], lang[])
 * (DONE!) SQLiteLang_ShowPlayerString(playerid, identifier[])
 *
 * (DONE!) SQLiteLang_SetDefaultLang(lang[])
 *
 * (DONE!) SQLiteLang_SetPlayerLang(playerid, lang[])
 *
 * (DONE!) SQLiteLang_IsIdentifierValid(identifier[])
 * (DONE!) SQLiteLang_IsLanguageValid(lang[])
 * (DONE!) SQLiteLang_IsStringValid(identifier[], lang[])
*/

/*
 * Includes
*/

#include 		<a_samp>
#include 		<sqlitei>

/*
 * Macros
*/

#if !defined isnull
	#define isnull(%1) \
		((%1[0] == 0) || (%1[0] == 1 && %1[1] == 0))
#endif

/*
 * Variables Declaration
*/

enum SQLiteLang_playerInformation {
	playerLang[128],
};

enum SQLiteLang_internalInformation {
	bool: systemStatus,
	bool: debugStatus,
	DB: databaseHandler,
	defaultLanguage[128],
	databaseDirectory[128],		
};

new SQLiteLang_playerVariables[MAX_PLAYERS][SQLiteLang_playerInformation];
new SQLiteLang_internalVariables[SQLiteLang_internalInformation];

/*
 * Hooks
*/

// OnGameModeInit
public OnGameModeInit()
{
    #if defined SQLiteLang_OnGameModeInit
		SQLiteLang_OnGameModeInit();			
    #endif
	
	// By default, SQLiteLang is turned off.
	SQLiteLang_internalVariables[systemStatus] = false;
	
	// By default, SQLiteLang debugging mode is turned off.
	SQLiteLang_internalVariables[debugStatus] = false;	
	
	// By default, the directory is set as the following.
	strcat(SQLiteLang_internalVariables[databaseDirectory], "SQLiteLang/Database/Database.db");	
    return 1;
}

#if defined _ALS_OnGameModeInit
    #undef OnGameModeInit
#else
    #define _ALS_OnGameModeInit
#endif
#define OnGameModeInit SQLiteLang_OnGameModeInit
#if defined SQLiteLang_OnGameModeInit
    forward SQLiteLang_OnGameModeInit();
#endif

// OnPlayerConnect
public OnPlayerConnect(playerid)
{	
    #if defined SQLiteLang_OnPlayerConnect		
		SQLiteLang_OnPlayerConnect(playerid);			
    #endif
	
	// Default language set to the player.
	strcat(SQLiteLang_playerVariables[playerid][playerLang], SQLiteLang_internalVariables[defaultLanguage]);
		
	if(SQLiteLang_internalVariables[systemStatus]) SendClientMessage(playerid, 000000, "{A4B2B3}[SQLiteLang] {FFFFFF}This server uses \"SQLiteLang\". Use /sqlitelang to select an available language.");
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
 * optional: file[] ("SQLiteLang/Database/Database.db") = Directory and file name of the database.
 * optional: bool:init_debug (false) = Turning this on will activate the debugging mode.
*/
stock SQLiteLang_Initialize(file[] = "SQLiteLang/Database/Database.db", bool: init_debug = false)
{
	if(SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang (SQLiteLang_Initialize): Inizialization failed (system already started).");
		return 0;
	}
	
	// If init_debug is true, debug mode will be activated.
	if(init_debug) SQLiteLang_internalVariables[debugStatus] = true;
	
	// Adding the directory into the internal variable.
	strcat(SQLiteLang_internalVariables[databaseDirectory], file, 512);	
	
	// Checking if a custom directory has been selected.
	if(strcmp(file, "SQLiteLang/Database/Database.db", false)) 
	{
		if(SQLiteLang_internalVariables[debugStatus]) printf("SQLiteLang_DEBUG (SQLiteLang_Initialize): SQLiteLang initialization started. Custom directory selected: \"%s\".", SQLiteLang_internalVariables[databaseDirectory]);		
	}
	else
	{
		if(SQLiteLang_internalVariables[debugStatus]) print("SQLiteLang_DEBUG (SQLiteLang_Initialize): SQLiteLang initialization started. Default directory selected: \"SQLiteLang/Database/Database.db\".");			
	}
	
	// Checking if the database exists.
	if(!fexist(SQLiteLang_internalVariables[databaseDirectory])) 
	{
		if(SQLiteLang_internalVariables[debugStatus]) print("SQLiteLang_DEBUG (SQLiteLang_Initialize): The database does not exist, automatic creation has been started.");
		
		SQLiteLang_internalVariables[databaseHandler] = db_open(SQLiteLang_internalVariables[databaseDirectory]);
		
		// If the database still does not exist it means that the folder 'SQLiteLang' is not created, so the process is interrupted.
		if(!fexist(SQLiteLang_internalVariables[databaseDirectory]))
		{	
			if(SQLiteLang_internalVariables[debugStatus]) print("SQLiteLang_DEBUG (SQLiteLang_Initialize): The system failed to create the database (the folder 'SQLiteLang' does not exist).");
				
			// Turning off the debug mode, if it was active, as the initialization has failed.
			SQLiteLang_internalVariables[debugStatus] = false;
			return 0;
		}
		else
		{
			// The database is created and we start building it.
			db_exec(SQLiteLang_internalVariables[databaseHandler], "CREATE TABLE IF NOT EXISTS `strings` (`internal_identifier` INTEGER PRIMARY KEY NOT NULL UNIQUE)");
			db_exec(SQLiteLang_internalVariables[databaseHandler], "ALTER TABLE `strings` ADD `identifier` VARCHAR");
			db_exec(SQLiteLang_internalVariables[databaseHandler], "ALTER TABLE `strings` ADD `lang` VARCHAR");
			db_exec(SQLiteLang_internalVariables[databaseHandler], "ALTER TABLE `strings` ADD `text` VARCHAR");			
			db_exec(SQLiteLang_internalVariables[databaseHandler], "INSERT INTO `strings` (`identifier`, `lang`, `text`) VALUES ('first_string', 'en', 'This is your first string!')");
		
			db_exec(SQLiteLang_internalVariables[databaseHandler], "CREATE TABLE IF NOT EXISTS `identifiers` (`internal_identifier` INTEGER PRIMARY KEY NOT NULL UNIQUE)");
			db_exec(SQLiteLang_internalVariables[databaseHandler], "ALTER TABLE `identifiers` ADD `identifier_name` VARCHAR");
			db_exec(SQLiteLang_internalVariables[databaseHandler], "ALTER TABLE `identifiers` ADD `identifier_description` VARCHAR");
			db_exec(SQLiteLang_internalVariables[databaseHandler], "INSERT INTO `identifiers` (`identifier_name`, `identifier_description`) VALUES ('first_string', 'The description of your first identifier.')");
		
			db_exec(SQLiteLang_internalVariables[databaseHandler], "CREATE TABLE IF NOT EXISTS `languages` (`internal_identifier` INTEGER PRIMARY KEY NOT NULL UNIQUE)");
			db_exec(SQLiteLang_internalVariables[databaseHandler], "ALTER TABLE `languages` ADD `lang_name` VARCHAR");
			db_exec(SQLiteLang_internalVariables[databaseHandler], "ALTER TABLE `languages` ADD `lang_description` VARCHAR");
			db_exec(SQLiteLang_internalVariables[databaseHandler], "INSERT INTO `languages` (`lang_name`, `lang_description`) VALUES ('en', 'English Language')");
			
			db_exec(SQLiteLang_internalVariables[databaseHandler], "CREATE TABLE IF NOT EXISTS `settings` (`internal_identifier` INTEGER PRIMARY KEY NOT NULL UNIQUE)");
			db_exec(SQLiteLang_internalVariables[databaseHandler], "ALTER TABLE `settings` ADD `default_language` VARCHAR");
			db_exec(SQLiteLang_internalVariables[databaseHandler], "INSERT INTO `settings` (`default_language`) VALUES ('en')");
			
			print("SQLiteLang (SQLiteLang_Initialize): Initialization completed successfully.");
			SQLiteLang_internalVariables[systemStatus] = true;
			
			// We close the database as we don't need it anymore.
			db_close(SQLiteLang_internalVariables[databaseHandler]);
		}
	}
	else
	{
		// The database has been found and we declare the system as active.		
		print("SQLiteLang (SQLiteLang_Initialize): Initialization completed successfully.");
		SQLiteLang_internalVariables[systemStatus] = true;
	}
	
	return 1;
}

/*
 * SQLiteLang_Terminate
 * This function terminates the usage of the include and resets the saved name and directory of the database internally to default.
*/
stock SQLiteLang_Terminate()
{
	if(!SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang (SQLiteLang_Terminate): Termination failed (system not started).");
		return 0;
	} 
	
	// Declaring the database as turned off.
	SQLiteLang_internalVariables[systemStatus] = false;
	
	// Clearing the internal variable that contains the database directory.
	strcat(SQLiteLang_internalVariables[databaseDirectory], "SQLiteLang/Database/Database.db");
	
	print("SQLiteLang (SQLiteLang_Terminate): Termination completed successfully.");
	return 1;
}
	
/*
 * SQLiteLang_AddIdentifier
 * This function adds a new string identifier to the database.
 * 
 * identifier[] = The identifier is used to identify, as the parameter says, every string of text (ie. "welcome_string").
 * description[] = The description of the identifier (ie. "string that welcomes new players in the server").
*/
stock SQLiteLang_AddIdentifier(identifier[], description[])
{
	if(!SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang (SQLiteLang_AddIdentifier): Usage failed (system not started).");
		return 0;
	}
	
	// Checking if the identifier exists.
	if(SQLiteLang_IsIdentifierValid(identifier))
	{
		if(SQLiteLang_internalVariables[debugStatus]) printf("SQLiteLang_DEBUG (SQLiteLang_AddIdentifier): The identifier \"%s\" already exists in the database.", identifier);
		return 0;
	}
	
	SQLiteLang_internalVariables[databaseHandler] = db_open(SQLiteLang_internalVariables[databaseDirectory]);
	
	// Preparing the query to add the new identifier.
	new DBStatement: _insertIdentifierStatement = db_prepare(SQLiteLang_internalVariables[databaseHandler], "INSERT INTO `identifiers` (`identifier_name`, `identifier_description`) VALUES (?, ?)");
	stmt_bind_value(_insertIdentifierStatement, 0, DB::TYPE_STRING, identifier);
	stmt_bind_value(_insertIdentifierStatement, 1, DB::TYPE_STRING, description);
		
	// If the query has failed it means that the database is corrupted.
	if(!stmt_execute(_insertIdentifierStatement)) 
	{
		if(SQLiteLang_internalVariables[debugStatus]) print("SQLiteLang_DEBUG (SQLiteLang_AddIdentifier): Failed to add a new identifier (database corrupted).");
		return 0;
	}
			
	printf("SQLiteLang (SQLiteLang_AddIdentifier): New identifier added (identifier: '%s' - description: '%s').", identifier, description);
	
	// Closing the statement.
	stmt_close(_insertIdentifierStatement);
	
	// Closing the database as we don't need it anymore.
	db_close(SQLiteLang_internalVariables[databaseHandler]);
	return 1;
}

/*
 * SQLiteLang_AddString
 * This function adds a new string to the database.
 * 
 * identifier[] = The identifier is used to identify, as the parameter says, every string of text (ie. "welcome_string").
 * lang[] = The lang is used to identify the language of the string (ie. "en" or "IT" or "spanish").
 * string[] = The string is the actual string of text to show in different languages.
*/
stock SQLiteLang_AddString(identifier[], lang[], string[])
{
	if(!SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang (SQLiteLang_AddString): Usage failed (system not started).");
		return 0;
	}

	// If the identifier does not exist.
	if(!SQLiteLang_IsIdentifierValid(identifier)) 
	{
		printf("SQLiteLang (SQLiteLang_AddString): The identifier \"%s\" does not exist.", identifier);
		return 0;
	}
	
	// If the language does not exist.
	if(!SQLiteLang_IsLanguageValid(lang)) 
	{
		printf("SQLiteLang (SQLiteLang_AddString): The language \"%s\" does not exist.", lang);
		return 0;
	}
	
	// If the string already exists.
	if(SQLiteLang_IsStringValid(identifier, lang))
	{
		printf("SQLiteLang (SQLiteLang_AddString): The string \"%s (%s)\" already exists. You may only modify or delete it.", identifier, lang);
		return 0;
	}
	
	// Opening a connection to the database.
	SQLiteLang_internalVariables[databaseHandler] = db_open(SQLiteLang_internalVariables[databaseDirectory]);	
				
	// Preparing the query to add a new string in the database.
	new DBStatement: _insertStringStatement = db_prepare(SQLiteLang_internalVariables[databaseHandler], "INSERT INTO `strings` (`identifier`, `lang`, `text`) VALUES (?, ?, ?)");
	stmt_bind_value(_insertStringStatement, 0, DB::TYPE_STRING, identifier);
	stmt_bind_value(_insertStringStatement, 1, DB::TYPE_STRING, lang);
	stmt_bind_value(_insertStringStatement, 2, DB::TYPE_STRING, string);
				
	// If the query has failed it means that the database is corrupted.
	if(!stmt_execute(_insertStringStatement)) 
	{
		print("SQLiteLang (SQLiteLang_AddString): Failed to executive SQLite query (database corrupted).");
		return 0;
	}
						
	printf("SQLiteLang (SQLiteLang_AddString): New string added (identifier: '%s' - lang: '%s' - string: '%s').", identifier, lang, string);
				
	// Closing the statement.
	stmt_close(_insertStringStatement);
				
	// Closing the database as we don't need it anymore.
	db_close(SQLiteLang_internalVariables[databaseHandler]);
	return 1;
}

/*
 * SQLiteLang_AddLanguage
 * This function adds a new language to the database.
 *
 * lang[] = The lang is used to identify the language of the string (ie. "en" or "IT" or "spanish").
 * description[] = The description of the language (ie. "English" or "Italian Language" or "{FFFFFF}Spanish").
*/
stock SQLiteLang_AddLanguage(lang[], description[])
{
	if(!SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang (SQLiteLang_AddLanguage): Usage failed (system not started).");
		return 0;
	}
	
	if(SQLiteLang_IsLanguageValid(lang)) 
	{
		if(SQLiteLang_internalVariables[debugStatus]) printf("SQLiteLang_DEBUG (SQLiteLang_AddLanguage): The language \"%s\" already exists in the database.", lang);
		return 0;
	}
	
	// Opening the database.
	SQLiteLang_internalVariables[databaseHandler] = db_open(SQLiteLang_internalVariables[databaseDirectory]);
	
	// Preparing the query to insert the new language in the database.
	new DBStatement: _insertLanguageStatement = db_prepare(SQLiteLang_internalVariables[databaseHandler], "INSERT INTO `languages` (`lang_name`, `lang_description`) VALUES (?, ?)");
	stmt_bind_value(_insertLanguageStatement, 0, DB::TYPE_STRING, lang);
	stmt_bind_value(_insertLanguageStatement, 1, DB::TYPE_STRING, description);
	
	// If the query fails to execute it means that the database is corrupted.
	if(!stmt_execute(_insertLanguageStatement)) 
	{
		if(SQLiteLang_internalVariables[debugStatus]) print("SQLiteLang_DEBUG (SQLiteLang_AddLanguage): Failed to add a new language (database corrupted).");
		return 0;
	}
			
	printf("SQLiteLang (SQLiteLang_AddLanguage): New language added (lang: '%s' - description: '%s').", lang, description);
		
	// Closing the statement.
	stmt_close(_insertLanguageStatement);	
	
	// Closing the database as we don't need it anymore.
	db_close(SQLiteLang_internalVariables[databaseHandler]);	
	return 1;
}

/*
 * SQLiteLang_DeleteLanguage
 * This function deletes a specific language from database.
 * Be careful, deleting a language will delete all the language strings already added in the database.
 * Furthermore, you may not delete the default language, if set.
 *
 * lang[] = The lang is used to identify the language of the string (ie. "en" or "IT" or "spanish").
*/
stock SQLiteLang_DeleteLanguage(lang[])
{
	if(!SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang (SQLiteLang_DeleteLanguage): Usage failed (system not started).");
		return 0;
	}
	
	// If the language does not exist then we interrupt the execution of the code.
	if(!SQLiteLang_IsLanguageValid(lang)) 
	{
		if(SQLiteLang_internalVariables[debugStatus]) printf("SQLiteLang_DEBUG (SQLiteLang_DeleteLanguage): The language \"%s\" does not exist in the database.", lang);
		return 0;
	}
	
	// Opening the database.
	SQLiteLang_internalVariables[databaseHandler] = db_open(SQLiteLang_internalVariables[databaseDirectory]);
	
	// Preparing the query to delete the language from the 'languages' table.
	new DBStatement: _deleteLanguageStatement = db_prepare(SQLiteLang_internalVariables[databaseHandler], "DELETE FROM `languages` WHERE `lang_name` = ?");
	stmt_bind_value(_deleteLanguageStatement, 0, DB::TYPE_STRING, lang);
	
	// If the query has failed to execute it means that the database is corrupted.
	if(!stmt_execute(_deleteLanguageStatement)) 
	{
		if(SQLiteLang_internalVariables[debugStatus]) printf("SQLiteLang_DEBUG (SQLiteLang_DeleteLanguage): Failed to delete the language \"%s\" (database corrupted).", lang);
			
		// Closing the database as we don't need it anymore.
		db_close(SQLiteLang_internalVariables[databaseHandler]);			
		return 0;
	}
			
	printf("SQLiteLang (SQLiteLang_DeleteLanguage): Language deleted (lang: '%s').", lang);
			
	stmt_close(_deleteLanguageStatement);
	
	// Here we are not closing the database as we need to make another query immediatly after that.
	
	new DBStatement: _deleteLanguageStringsStatement = db_prepare(SQLiteLang_internalVariables[databaseHandler], "DELETE FROM `strings` WHERE `lang` = ?");
	stmt_bind_value(_deleteLanguageStringsStatement, 0, DB::TYPE_STRING, lang);
	
	if(!stmt_execute(_deleteLanguageStringsStatement)) 
	{
		if(SQLiteLang_internalVariables[debugStatus]) printf("SQLiteLang_DEBUG (SQLiteLang_DeleteLanguage): Failed to delete the language strings \"%s\" (database corrupted).", lang);
			
		// Closing the database as we don't need it anymore.
		db_close(SQLiteLang_internalVariables[databaseHandler]);			
		return 0;
	}	
	
	stmt_close(_deleteLanguageStringsStatement);
	
	// Closing the database as we don't need it anymore.
	db_close(SQLiteLang_internalVariables[databaseHandler]);	
	return 1;
}

/*
 * SQLiteLang_ModifyIdentifier
 * This function modifies an identifier name to new one.
 * 
 * old_identifier[] = The identifier to be modified.
 * new_identifier[] = The new name of the identifier.
*/
stock SQLiteLang_ModifyIdentifier(old_identifier[], new_identifier[])
{
	if(!SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang (SQLiteLang_ModifyIdentifier): Usage failed (system not started).");
		return 0;
	}
		
	// If the identifier does not exist then we interrupt the execution of the code.
	if(!SQLiteLang_IsIdentifierValid(old_identifier)) 
	{
		if(SQLiteLang_internalVariables[debugStatus]) printf("SQLiteLang_DEBUG (SQLiteLang_ModifyIdentifier): The identifier \"%s\" does not exist in the database.", old_identifier);
		return 0;
	}

	// Opening the database.
	SQLiteLang_internalVariables[databaseHandler] = db_open(SQLiteLang_internalVariables[databaseDirectory]);
	
	// Preparing the query to delete the identifier from the 'identifiers' table.
	new DBStatement: _modifyIdentifierStatement = db_prepare(SQLiteLang_internalVariables[databaseHandler], "UPDATE `identifiers` SET `identifier_name` = ? WHERE `identifier_name` = ?");
	stmt_bind_value(_modifyIdentifierStatement, 0, DB::TYPE_STRING, new_identifier);
	stmt_bind_value(_modifyIdentifierStatement, 1, DB::TYPE_STRING, old_identifier);
	
	// If the query has failed to execute it means that the database is corrupted.
	if(!stmt_execute(_modifyIdentifierStatement)) 
	{
		if(SQLiteLang_internalVariables[debugStatus]) printf("SQLiteLang_DEBUG (SQLiteLang_ModifyIdentifier): Failed to modify the identifier from \"%s\" to \"%s\" (database corrupted).", old_identifier, new_identifier);
			
		// Closing the database as we don't need it anymore.
		db_close(SQLiteLang_internalVariables[databaseHandler]);			
		return 0;
	}
			
	printf("SQLiteLang (SQLiteLang_ModifyIdentifier): Identifier modified (before: '%s' - now: '%s').", old_identifier, new_identifier);
			
	stmt_close(_modifyIdentifierStatement);
	
	// Preparing the query to delete the identifier from the 'identifiers' table.
	new DBStatement: _modifyIdentifierStringsStatement = db_prepare(SQLiteLang_internalVariables[databaseHandler], "UPDATE `strings` SET `identifier` = ? WHERE `identifier` = ?");
	stmt_bind_value(_modifyIdentifierStringsStatement, 0, DB::TYPE_STRING, new_identifier);
	stmt_bind_value(_modifyIdentifierStringsStatement, 1, DB::TYPE_STRING, old_identifier);	
	
	// If the query has failed to execute it means that the database is corrupted.
	if(!stmt_execute(_modifyIdentifierStringsStatement)) 
	{
		if(SQLiteLang_internalVariables[debugStatus]) printf("SQLiteLang_DEBUG (SQLiteLang_ModifyIdentifier): Failed to modify the identifier strings from \"%s\" to \"%s\" (database corrupted).", old_identifier, new_identifier);
			
		// Closing the database as we don't need it anymore.
		db_close(SQLiteLang_internalVariables[databaseHandler]);			
		return 0;
	}
			
	printf("SQLiteLang (SQLiteLang_ModifyIdentifier): Identifier strings modified (before: '%s' - now: '%s').", old_identifier, new_identifier);
			
	stmt_close(_modifyIdentifierStringsStatement);	
	
	// Closing the database as we don't need it anymore.
	db_close(SQLiteLang_internalVariables[databaseHandler]);
	return 1;
}

/*
 * SQLiteLang_ModifyIdentifierDesc
 * This function modifies an identifier's description to a new one.
 * 
 * identifier[] = The identifier is used to identify, as the parameter says, every string of text (ie. "welcome_string").
 * new_desc[] = The new description of the identifier.
*/
stock SQLiteLang_ModifyIdentifierDesc(identifier[], new_desc[])
{
	if(!SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang (SQLiteLang_ModifyIdentifierDesc): Usage failed (system not started).");
		return 0;
	}
		
	// If the identifier does not exist then we interrupt the execution of the code.
	if(!SQLiteLang_IsIdentifierValid(identifier)) 
	{
		if(SQLiteLang_internalVariables[debugStatus]) printf("SQLiteLang_DEBUG (SQLiteLang_ModifyIdentifierDesc): The identifier \"%s\" does not exist in the database.", identifier);
		return 0;
	}

	// Opening the database.
	SQLiteLang_internalVariables[databaseHandler] = db_open(SQLiteLang_internalVariables[databaseDirectory]);
	
	// Preparing the query to delete the identifier from the 'identifiers' table.
	new DBStatement: _modifyIdentifierDescriptionStatement = db_prepare(SQLiteLang_internalVariables[databaseHandler], "UPDATE `identifiers` SET `identifier_description` = ? WHERE `identifier_name` = ?");
	stmt_bind_value(_modifyIdentifierDescriptionStatement, 0, DB::TYPE_STRING, new_desc);
	stmt_bind_value(_modifyIdentifierDescriptionStatement, 1, DB::TYPE_STRING, identifier);
	
	// If the query has failed to execute it means that the database is corrupted.
	if(!stmt_execute(_modifyIdentifierDescriptionStatement)) 
	{
		if(SQLiteLang_internalVariables[debugStatus]) printf("SQLiteLang_DEBUG (SQLiteLang_ModifyIdentifierDesc): Failed to modify the identifier (%s) description to \"%s\" (database corrupted).", identifier, new_desc);
			
		// Closing the database as we don't need it anymore.
		db_close(SQLiteLang_internalVariables[databaseHandler]);			
		return 0;
	}
			
	printf("SQLiteLang (SQLiteLang_ModifyIdentifierDesc): Identifier description modified (identifier: '%s' - new description: '%s').", identifier, new_desc);
			
	stmt_close(_modifyIdentifierDescriptionStatement);
	
	// Closing the database as we don't need it anymore.
	db_close(SQLiteLang_internalVariables[databaseHandler]);
	return 1;
}
	
/*
 * SQLiteLang_DeleteIdentifier
 * This function deletes all the language strings of a specific identifier from the database.
 * 
 * identifier[] = The identifier is used to identify, as the parameter says, every string of text (ie. "welcome_string").
*/
stock SQLiteLang_DeleteIdentifier(identifier[])
{
	if(!SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang (SQLiteLang_DeleteIdentifier): Usage failed (system not started).");
		return 0;
	}
		
	// If the identifier does not exist then we interrupt the execution of the code.
	if(!SQLiteLang_IsIdentifierValid(identifier)) 
	{
		if(SQLiteLang_internalVariables[debugStatus]) printf("SQLiteLang_DEBUG (SQLiteLang_DeleteIdentifier): The identifier \"%s\" does not exist in the database.", identifier);
		return 0;
	}
	
	// Opening the database.
	SQLiteLang_internalVariables[databaseHandler] = db_open(SQLiteLang_internalVariables[databaseDirectory]);
	
	// Preparing the query to delete the identifier from the 'identifiers' table.
	new DBStatement: _deleteIdentifierStatement = db_prepare(SQLiteLang_internalVariables[databaseHandler], "DELETE FROM `identifiers` WHERE `identifier_name` = ?");
	stmt_bind_value(_deleteIdentifierStatement, 0, DB::TYPE_STRING, identifier);
	
	// If the query has failed to execute it means that the database is corrupted.
	if(!stmt_execute(_deleteIdentifierStatement)) 
	{
		if(SQLiteLang_internalVariables[debugStatus]) printf("SQLiteLang_DEBUG (SQLiteLang_DeleteIdentifier): Failed to delete the identifier \"%s\" (database corrupted).", identifier);
			
		// Closing the database as we don't need it anymore.
		db_close(SQLiteLang_internalVariables[databaseHandler]);			
		return 0;
	}
			
	printf("SQLiteLang (SQLiteLang_DeleteIdentifier): Identifier deleted (identifier: '%s').", identifier);
			
	stmt_close(_deleteIdentifierStatement);
	
	// Here we are not closing the database as we need to make another query immediatly after that.
	
	new DBStatement: _deleteLanguageStringsStatement = db_prepare(SQLiteLang_internalVariables[databaseHandler], "DELETE FROM `strings` WHERE `identifier` = ?");
	stmt_bind_value(_deleteLanguageStringsStatement, 0, DB::TYPE_STRING, identifier);
	
	if(!stmt_execute(deleteLanguageStringsStatement)) 
	{
		if(SQLiteLang_internalVariables[debugStatus]) printf("SQLiteLang_DEBUG (SQLiteLang_DeleteLanguage): Failed to delete the language strings with the identifier \"%s\" (database corrupted).", identifier);
			
		// Closing the database as we don't need it anymore.
		db_close(SQLiteLang_internalVariables[databaseHandler]);			
		return 0;
	}
	
	printf("SQLiteLang (SQLiteLang_DeleteIdentifier): Identifier strings deleted (identifier: '%s').", identifier);
	
	stmt_close(_deleteLanguageStringsStatement);
	
	// Closing the database as we don't need it anymore.
	db_close(SQLiteLang_internalVariables[databaseHandler]);					
	return 1;
}

/*
 * SQLiteLang_DeleteString
 * This function deletes a specific language string of a specific identifier from the database.
 * 
 * identifier[] = The identifier is used to identify, as the parameter says, every string of text (ie. "welcome_string").
 * lang[] = The lang is used to identify the language of the string (ie. "en" or "IT" or "spanish").
*/
stock SQLiteLang_DeleteString(identifier[], lang[])
{
	if(!SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang (SQLiteLang_DeleteString): Usage failed (system not started).");
		return 0;
	}
		
	// If the identifier does not exist.
	if(!SQLiteLang_IsIdentifierValid(identifier)) 
	{
		printf("SQLiteLang (SQLiteLang_DeleteString): The identifier \"%s\" does not exist.", identifier);
		return 0;
	}
	
	// If the language does not exist.
	if(!SQLiteLang_IsLanguageValid(lang)) 
	{
		printf("SQLiteLang (SQLiteLang_DeleteString): The language \"%s\" does not exist.", lang);
		return 0;
	}		
		
	// If the string does not exist then we interrupt the execution of the code.
	if(!SQLiteLang_IsStringValid(identifier, lang)) 
	{
		printf("SQLiteLang (SQLiteLang_DeleteString): The string \"%s (%s)\" does not exist in the database.", identifier, lang);
		return 0;
	}
	
	// Opening the database.
	SQLiteLang_internalVariables[databaseHandler] = db_open(SQLiteLang_internalVariables[databaseDirectory]);
	
	// Preparing the query to delete the identifier from the 'identifiers' table.
	new DBStatement: _deleteStringStatement = db_prepare(SQLiteLang_internalVariables[databaseHandler], "DELETE FROM `strings` WHERE `identifier` = ? AND `lang` = ?");
	stmt_bind_value(_deleteStringStatement, 0, DB::TYPE_STRING, identifier);
	stmt_bind_value(_deleteStringStatement, 1, DB::TYPE_STRING, lang);
	
	// If the query has failed to execute it means that the database is corrupted.
	if(!stmt_execute(_deleteStringStatement)) 
	{
		if(SQLiteLang_internalVariables[debugStatus]) printf("SQLiteLang_DEBUG (SQLiteLang_DeleteString): Failed to delete the string \"%s (%s)\" (database corrupted).", identifier, lang);
			
		// Closing the database as we don't need it anymore.
		db_close(SQLiteLang_internalVariables[databaseHandler]);			
		return 0;
	}
			
	printf("SQLiteLang (SQLiteLang_DeleteString): String deleted (identifier: '%s' - lang: '%s').", identifier, lang);
			
	stmt_close(_deleteStringStatement);
	
	// Closing the database as we don't need it anymore.
	db_close(SQLiteLang_internalVariables[databaseHandler]);
	return 1;
}

/*
 * SQLiteLang_ModifyString
 * This function modify a specific language string in database.
 * 
 * identifier[] = The identifier is used to identify, as the parameter says, every string of text (ie. "welcome_string").
 * lang[] = The lang is used to identify the language of the string (ie. "en" or "IT" or "spanish").
 * new_string[] = The string is the new actual string of text to show in different languages that will replace the old one.
*/
stock SQLiteLang_ModifyString(identifier[], lang[], new_string[])
{
	if(!SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang (SQLiteLang_ModifyString): Usage failed (system not started).");
		return 0;
	}
	
	// If the identifier does not exist.
	if(!SQLiteLang_IsIdentifierValid(identifier)) 
	{
		printf("SQLiteLang (SQLiteLang_ModifyString): The identifier \"%s\" does not exist.", identifier);
		return 0;
	}
	
	// If the language does not exist.
	if(!SQLiteLang_IsLanguageValid(lang)) 
	{
		printf("SQLiteLang (SQLiteLang_ModifyString): The language \"%s\" does not exist.", lang);
		return 0;
	}		
	
	// If the string does not exist then we interrupt the execution of the code.
	if(!SQLiteLang_IsStringValid(identifier, lang)) 
	{
		printf("SQLiteLang (SQLiteLang_ModifyString): The string \"%s (%s)\" does not exist in the database.", identifier, lang);
		return 0;
	}
	
	// Opening the database.
	SQLiteLang_internalVariables[databaseHandler] = db_open(SQLiteLang_internalVariables[databaseDirectory]);
	
	// Preparing the query to delete the identifier from the 'identifiers' table.
	new DBStatement: _modifyStringStatement = db_prepare(SQLiteLang_internalVariables[databaseHandler], "UPDATE `strings` SET `text` = ? WHERE `identifier` = ? AND `lang` = ?");
	stmt_bind_value(_modifyStringStatement, 0, DB::TYPE_STRING, new_string);
	stmt_bind_value(_modifyStringStatement, 1, DB::TYPE_STRING, identifier);
	stmt_bind_value(_modifyStringStatement, 2, DB::TYPE_STRING, lang);
	
	// If the query has failed to execute it means that the database is corrupted.
	if(!stmt_execute(_modifyStringStatement)) 
	{
		if(SQLiteLang_internalVariables[debugStatus]) printf("SQLiteLang_DEBUG (SQLiteLang_DeleteString): Failed to modify the string \"%s (%s)\" (database corrupted).", identifier, lang);
			
		// Closing the database as we don't need it anymore.
		db_close(SQLiteLang_internalVariables[databaseHandler]);			
		return 0;
	}
			
	printf("SQLiteLang (SQLiteLang_DeleteString): String modified (identifier: '%s' - lang: '%s' - text: '%s').", identifier, lang, new_string);
			
	stmt_close(_modifyStringStatement);
	
	// Closing the database as we don't need it anymore.
	db_close(SQLiteLang_internalVariables[databaseHandler]);	
	return 1;
}

/*
 * SQLiteLang_ShowLanguageString
 * This function shows a specific language string taken from the database.
 * 
 * identifier[] = The identifier is used to identify, as the parameter says, every string of text (ie. "welcome_string").
 * optional: lang[] (default language) = The lang is used to identify the language of the string (ie. "en" or "IT" or "spanish").
*/
stock SQLiteLang_ShowLanguageString(identifier[], lang[] = "")
{
	if(isnull(lang)) strcat(lang, SQLiteLang_internalVariables[defaultLanguage], 128);
	
	new _return[128] = "UNDEFINED";
	
	if(!SQLiteLang_internalVariables[systemStatus])
	{
		print("SQLiteLang (SQLiteLang_ShowLanguageString): Usage failed (system not started).");
		
		_return = "SYSTEM_NOT_STARTED";
		return _return;
	}
	
	// If the identifier does not exist.
	if(!SQLiteLang_IsIdentifierValid(identifier)) 
	{
		printf("SQLiteLang (SQLiteLang_ShowLanguageString): The identifier \"%s\" does not exist.", identifier);
		
		_return = "IDENTIFIER_INVALID";
		return _return;
	}
	
	// If the language does not exist.
	if(!SQLiteLang_IsLanguageValid(lang)) 
	{
		printf("SQLiteLang (SQLiteLang_ShowLanguageString): The language \"%s\" does not exist.", lang);
		
		_return = "LANGUAGE_INVALID";
		return _return;
	}		
	
	// If the string does not exist then we interrupt the execution of the code.
	if(!SQLiteLang_IsStringValid(identifier, lang)) 
	{
		printf("SQLiteLang (SQLiteLang_ShowLanguageString): The string \"%s (%s)\" does not exist in the database.", identifier, lang);
		
		_return = "STRING_INVALID";
		return _return;
	}

	SQLiteLang_internalVariables[databaseHandler] = db_open(SQLiteLang_internalVariables[databaseDirectory]);
	
	new _queryInput[512];
	format(_queryInput, sizeof(_queryInput), "SELECT `text` FROM `strings` WHERE `identifier` = '%s' AND `lang` = '%s'", identifier, lang);
	new DBResult: _queryResult = db_query(SQLiteLang_internalVariables[databaseHandler], _queryInput);
	
	if(db_num_rows(_queryResult))
	{		
		db_get_field(_queryResult, 0, _return, sizeof(_return));		
		if(SQLiteLang_internalVariables[debugStatus]) printf("SQLiteLang (SQLiteLang_ShowLanguageString): We've found: \"%s (%d)\".", _return, strlen(_return));		
		return _return;
	}
	else
	{
		print("SQLiteLang (SQLiteLang_ShowLanguageString): Something went wrong in the query execution.");
		
		_return = "NOT_DEFINED_ERROR";
		return _return;
	}
}

/*
 * SQLiteLang_ShowPlayerString
 * This function shows a specific language string taken from the database.
 * 
 * playerid = playerid of the player.
 * identifier[] = The identifier is used to identify, as the parameter says, every string of text (ie. "welcome_string").
*/
stock SQLiteLang_ShowPlayerString(playerid, identifier[])
{
	new _return[128] = "UNDEFINED";
	
	if(!SQLiteLang_internalVariables[systemStatus])
	{
		print("SQLiteLang (SQLiteLang_ShowPlayerString): Usage failed (system not started).");
		
		_return = "SYSTEM_NOT_STARTED";
		return _return;
	}
	
	// If the identifier does not exist.
	if(!SQLiteLang_IsIdentifierValid(identifier)) 
	{
		printf("SQLiteLang (SQLiteLang_ShowPlayerString): The identifier \"%s\" does not exist.", identifier);
		
		_return = "IDENTIFIER_INVALID";
		return _return;
	}	
	
	// If the language does not exist.
	if(!SQLiteLang_IsLanguageValid(SQLiteLang_playerVariables[playerid][playerLang])) 
	{
		printf("SQLiteLang (SQLiteLang_ShowPlayerString): The language \"%s\" does not exist.", SQLiteLang_playerVariables[playerid][playerLang]);
		
		_return = "LANGUAGE_INVALID";
		return _return;
	}
	
	// If the string does not exist then we interrupt the execution of the code.
	if(!SQLiteLang_IsStringValid(identifier, SQLiteLang_playerVariables[playerid][playerLang])) 
	{
		printf("SQLiteLang (SQLiteLang_ShowPlayerString): The string \"%s (%s)\" does not exist in the database.", identifier, SQLiteLang_playerVariables[playerid][playerLang]);
		
		_return = "STRING_INVALID";
		return _return;
	}

	SQLiteLang_internalVariables[databaseHandler] = db_open(SQLiteLang_internalVariables[databaseDirectory]);
	
	new _queryInput[512];
	format(_queryInput, sizeof(_queryInput), "SELECT `text` FROM `strings` WHERE `identifier` = '%s' AND `lang` = '%s'", identifier, SQLiteLang_playerVariables[playerid][playerLang]);
	new DBResult: _queryResult = db_query(SQLiteLang_internalVariables[databaseHandler], _queryInput);
	
	if(db_num_rows(_queryResult))
	{		
		db_get_field(_queryResult, 0, _return, sizeof(_return));
		
		if(SQLiteLang_internalVariables[debugStatus]) printf("SQLiteLang_DEBUG (SQLiteLang_ShowPlayerString): We've found: \"%s (%d)\".", _return, strlen(_return));		
		return _return;
	}
	else
	{
		print("SQLiteLang (SQLiteLang_ShowPlayerString): Something went wrong in the query execution.");
		
		_return = "NOT_DEFINED_ERROR";
		return _return;
	}	
	return 1;
}

/*
 * SQLiteLang_SetDefaultLang
 * This function sets the default language for users that haven't selected an available language.
 * 
 * identifier[] = The identifier is used to identify, as the parameter says, every string of text (ie. "welcome_string").
 * lang[] = The lang is used to identify the language of the string (ie. "en" or "IT" or "spanish").
*/
stock SQLiteLang_SetDefaultLang(lang[])
{
	if(!SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang (SQLiteLang_SetDefaultLang): Usage failed (system not started).");
		return 0;
	}
		
	if(!SQLiteLang_IsLanguageValid(lang)) 
	{
		printf("SQLiteLang (SQLiteLang_SetDefaultLang): The language \"%s\" does not exist.", lang);		
		return 0;
	}		
	
	// Opening the database.
	SQLiteLang_internalVariables[databaseHandler] = db_open(SQLiteLang_internalVariables[databaseDirectory]);
	
	// Preparing the query to delete the identifier from the 'identifiers' table.
	new DBStatement: _modifyDefaultLanguageStatement = db_prepare(SQLiteLang_internalVariables[databaseHandler], "UPDATE `settings` SET `default_language` = ?");
	stmt_bind_value(_modifyDefaultLanguageStatement, 0, DB::TYPE_STRING, lang);
	
	// If the query has failed to execute it means that the database is corrupted.
	if(!stmt_execute(_modifyDefaultLanguageStatement)) 
	{
		if(SQLiteLang_internalVariables[debugStatus]) printf("SQLiteLang_DEBUG (SQLiteLang_SetDefaultLang): Failed to modify the default language to \"(%s)\" (database corrupted).", lang);
			
		// Closing the database as we don't need it anymore.
		db_close(SQLiteLang_internalVariables[databaseHandler]);			
		return 0;
	}
			
	printf("SQLiteLang (SQLiteLang_SetDefaultLang): Default language modified (lang: '%s').", lang);
			
	stmt_close(_modifyDefaultLanguageStatement);
	
	// Closing the database as we don't need it anymore.
	db_close(SQLiteLang_internalVariables[databaseHandler]);		
	return 1;
}

/*
 * SQLiteLang_SetPlayerLang
 * This function sets the language of a player.
 * 
 * playerid = playerid of the player.
 * lang[] = The lang is used to identify the language of the string (ie. "en" or "IT" or "spanish").
*/
stock SQLiteLang_SetPlayerLang(playerid, lang[])
{
	if(!SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang (SQLiteLang_SetPlayerLang): Usage failed (system not started).");
		return 0;
	}
	
	if(!SQLiteLang_IsLanguageValid(lang)) 
	{
		printf("SQLiteLang (SQLiteLang_SetPlayerLang): The language \"%s\" does not exist.", lang);		
		return 0;
	}	
	
	if(SQLiteLang_internalVariables[debugStatus]) printf("SQLiteLang_DEBUG (SQLiteLang_SetPlayerLang): Player (%d) language set to \"%s\".", playerid, lang);		
	
	// Setting the player language to 'lang'.
	strcat(SQLiteLang_playerVariables[playerid][playerLang], lang, 128);
	return 1;
}

/*
 * SQLiteLang_IsIdentifierValid
 * This function checks if an identifier exists.
 * 
 * identifier[] = The identifier is used to identify, as the parameter says, every string of text (ie. "welcome_string").
*/
stock SQLiteLang_IsIdentifierValid(identifier[])
{
	if(!SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang (SQLiteLang_IsIdentifierValid): Usage failed (system not started).");
		return 0;
	}
	
	SQLiteLang_internalVariables[databaseHandler] = db_open(SQLiteLang_internalVariables[databaseDirectory]);
	
	new DBStatement: retrieveIdentifierStatement = db_prepare(SQLiteLang_internalVariables[databaseHandler], "SELECT `identifier_name` FROM `identifiers` WHERE `identifier_name` = ?");
	stmt_bind_value(retrieveIdentifierStatement, 0, DB::TYPE_STRING, identifier);
	
	if(stmt_execute(retrieveIdentifierStatement))
	{
		if(stmt_rows_left(retrieveIdentifierStatement))
		{
			stmt_close(retrieveIdentifierStatement);
			
			if(SQLiteLang_internalVariables[debugStatus]) printf("SQLiteLang_DEBUG (SQLiteLang_IsIdentifierValid): The identifier \"%s\" exists in the database.", identifier);
				
			db_close(SQLiteLang_internalVariables[databaseHandler]);
			return 1;
		}
		else
		{
			stmt_close(retrieveIdentifierStatement);
			
			if(SQLiteLang_internalVariables[debugStatus]) printf("SQLiteLang_DEBUG (SQLiteLang_IsIdentifierValid): The identifier \"%s\" does not exist in the database.", identifier);
				
			db_close(SQLiteLang_internalVariables[databaseHandler]);
			return 0;
		}
	}
	
	return 1;
}

/*
 * SQLiteLang_IsLanguageValid
 * This function checks if a language exists.
 * 
 * lang[] = The lang is used to identify the language of the string (ie. "en" or "IT" or "spanish").
*/
stock SQLiteLang_IsLanguageValid(lang[])
{
	if(!SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang (SQLiteLang_IsLanguageValid): Usage failed (system not started).");
		return 0;
	}
	
	SQLiteLang_internalVariables[databaseHandler] = db_open(SQLiteLang_internalVariables[databaseDirectory]);
	
	new DBStatement: retrieveLanguageStatement = db_prepare(SQLiteLang_internalVariables[databaseHandler], "SELECT `lang_name` FROM `languages` WHERE `lang_name` = ?");
	stmt_bind_value(retrieveLanguageStatement, 0, DB::TYPE_STRING, lang);
	
	if(stmt_execute(retrieveLanguageStatement))
	{
		if(stmt_rows_left(retrieveLanguageStatement))
		{
			stmt_close(retrieveLanguageStatement);
			
			if(SQLiteLang_internalVariables[debugStatus]) printf("SQLiteLang_DEBUG (SQLiteLang_IsLanguageValid): The language \"%s\" exists in the database.", lang);
				
			db_close(SQLiteLang_internalVariables[databaseHandler]);
			return 1;
		}
		else
		{
			stmt_close(retrieveLanguageStatement);
			
			if(SQLiteLang_internalVariables[debugStatus]) printf("SQLiteLang_DEBUG (SQLiteLang_IsLanguageValid): The language \"%s\" does not exist in the database.", lang);
				
			db_close(SQLiteLang_internalVariables[databaseHandler]);
			return 0;
		}
	}
	
	return 1;
}

/*
 * SQLiteLang_IsStringValid
 * This function checks if a string already exists.
 * 
 * identifier[] = The identifier is used to identify, as the parameter says, every string of text (ie. "welcome_string").
 * lang[] = The lang is used to identify the language of the string (ie. "en" or "IT" or "spanish").
*/
stock SQLiteLang_IsStringValid(identifier[], lang[])
{
	if(!SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang (SQLiteLang_IsStringValid): Usage failed (system not started).");
		return 0;
	}
	
	SQLiteLang_internalVariables[databaseHandler] = db_open(SQLiteLang_internalVariables[databaseDirectory]);
			
	new DBStatement: retrieveStringStatement = db_prepare(SQLiteLang_internalVariables[databaseHandler], "SELECT `text` FROM `strings` WHERE `identifier` = ? AND `lang` = ?");
	stmt_bind_value(retrieveStringStatement, 0, DB::TYPE_STRING, identifier);
	stmt_bind_value(retrieveStringStatement, 1, DB::TYPE_STRING, lang);
	
	if(stmt_execute(retrieveStringStatement))
	{
		if(stmt_rows_left(retrieveStringStatement))
		{		
			stmt_close(retrieveStringStatement);
			
			if(SQLiteLang_internalVariables[debugStatus]) printf("SQLiteLang_DEBUG (SQLiteLang_IsStringValid): The language string \"%s (%s)\" exists in the database.", identifier, lang);				
			
			db_close(SQLiteLang_internalVariables[databaseHandler]);
			return 1;
		}
		else
		{
			stmt_close(retrieveStringStatement);
				
			if(SQLiteLang_internalVariables[debugStatus]) printf("SQLiteLang_DEBUG (SQLiteLang_IsStringValid): The language string \"%s (%s)\" does not exist in the database.", identifier, lang);
					
			db_close(SQLiteLang_internalVariables[databaseHandler]);
			return 0;
		}
	}
	else 
	{
		print("SQLiteLang (SQLiteLang_IsStringValid): Failed to executive SQLite query (database corrupted).");
		return 0;
	}
}