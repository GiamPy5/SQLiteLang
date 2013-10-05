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
 * (DONE!) SQLiteLang_Initialize(file[])
 * (DONE!) SQLiteLang_Terminate()
 *
 * (DONE!) SQLiteLang_AddIdentifierString(identifier[])
 * TODO: SQLiteLang_DeleteIdentifier(identifier[])
 *
 * TODO: SQLiteLang_AddLanguageString(identifier[], lang[], string[])
 * TODO: SQLiteLang_AddLanguage(lang[])
 *
 * TODO: SQLiteLang_DeleteLanguage(lang[])
 * TODO: SQLiteLang_DeleteLanguageString(identifier[], lang[])
 *
 * TODO: SQLiteLang_ModifyLanguage(old_lang[], new_lang[])
 * TODO: SQLiteLang_ModifyLanguageString(identifier[], lang[], new_string[])
 *
 * TODO: SQLiteLang_ShowLanguageString(identifier[], lang[])
 * TODO: SQLiteLang_ShowPlayerString(playerid, identifier[])
 *
 * TODO: SQLiteLang_SetDefaultLang(lang[])
 *
 * TODO: SQLiteLang_SetPlayerLang(playerid, lang[])
 *
 * (DONE!) SQLiteLang_IsIdentifierValid(identifier[])
 * (DONE!) SQLiteLang_IsLanguageValid(lang[])
 * SQLiteLang_IsLanguageStringValid(identifier[], lang[])
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
    return 1;
}

public SQLiteLang_OnGameModeInit() 
{
	// By default, SQLiteLang is turned off.
	SQLiteLang_internalVariables[systemStatus] = false;
	
	// By default, SQLiteLang debugging mode is turned off.
	SQLiteLang_internalVariables[debugStatus] = false;	
	
	// By default, the directory is set as the following.
	strcat(SQLiteLang_internalVariables[databaseDirectory], "SQLiteLang/Database.db");
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
SQLiteLang_Initialize(file[] = "SQLiteLang/Database.db", init_debug = false)
{
	if(SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang: Inizialization failed (system already started).");
		return 0;
	}
		
	if(init_debug)
		SQLiteLang_internalVariables[debugStatus] = true;
	
	strcat(SQLiteLang_internalVariables[databaseDirectory], file, 512);	
	
	if(strcmp(file, "SQLiteLang/Database.db", false)) 
	{	
		if(SQLiteLang_internalVariables[debugStatus])
			printf("SQLiteLang_DEBUG: SQLiteLang initialization started. Custom directory selected: \"%s\".", SQLiteLang_internalVariables[databaseDirectory]);		
	}
	else
	{
		if(SQLiteLang_internalVariables[debugStatus])
			print("SQLiteLang_DEBUG: SQLiteLang initialization started. Default directory selected: \"SQLiteLang/Database.db\".");		
	}	
		
	if(!fexist(SQLiteLang_internalVariables[databaseDirectory])) 
	{
		if(SQLiteLang_internalVariables[debugStatus])
			print("SQLiteLang_DEBUG: The database does not exist, automatic creation has been started.");
		
		SQLiteLang_internalVariables[databaseHandler] = db_open(SQLiteLang_internalVariables[databaseDirectory]);
		
		if(!fexist(SQLiteLang_internalVariables[databaseDirectory]))
		{	
			if(SQLiteLang_internalVariables[debugStatus])
				print("SQLiteLang_DEBUG: The system failed to create the database (the folder 'SQLiteLang' does not exist).");
			return 0;
		}
		else
		{
			db_exec(SQLiteLang_internalVariables[databaseHandler], "CREATE TABLE IF NOT EXISTS `strings` (`internal_identifier` INTEGER PRIMARY KEY NOT NULL UNIQUE)");
			db_exec(SQLiteLang_internalVariables[databaseHandler], "ALTER TABLE `strings` ADD `identifier` VARCHAR");
			db_exec(SQLiteLang_internalVariables[databaseHandler], "ALTER TABLE `strings` ADD `lang` VARCHAR");
			db_exec(SQLiteLang_internalVariables[databaseHandler], "ALTER TABLE `strings` ADD `text` VARCHAR");	
		
			db_exec(SQLiteLang_internalVariables[databaseHandler], "CREATE TABLE IF NOT EXISTS `identifiers` (`internal_identifier` INTEGER PRIMARY KEY NOT NULL UNIQUE)");
			db_exec(SQLiteLang_internalVariables[databaseHandler], "ALTER TABLE `identifiers` ADD `identifier_name` VARCHAR");
		
			db_exec(SQLiteLang_internalVariables[databaseHandler], "CREATE TABLE IF NOT EXISTS `languages` (`internal_identifier` INTEGER PRIMARY KEY NOT NULL UNIQUE)");
			db_exec(SQLiteLang_internalVariables[databaseHandler], "ALTER TABLE `languages` ADD `lang_name` VARCHAR");
			
			print("SQLiteLang: Initialization completed successfully.");
			SQLiteLang_internalVariables[systemStatus] = true;
			
			db_close(SQLiteLang_internalVariables[databaseHandler]);
		}
	}
	else
	{
		print("SQLiteLang: Initialization completed successfully.");
		SQLiteLang_internalVariables[systemStatus] = true;
	}
	
	return 1;
}

/*
 * SQLiteLang_Terminate
 * This function terminates the usage of the include and resets the saved name and directory of the database internally to default.
*/
SQLiteLang_Terminate()
{
	if(!SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang: Termination failed (system not started).");
		return 0;
	} 
		
	SQLiteLang_internalVariables[systemStatus] = false;
	
	print("SQLiteLang: Termination completed successfully.");
	return 1;
}
	
/*
 * SQLiteLang_AddIdentifierString
 * This function adds a new string identifier to the database.
 * 
 * identifier[] = The identifier is used to identify, as the parameter says, every string of text (ie. "welcome_string").
*/
SQLiteLang_AddIdentifierString(identifier[])
{
	if(!SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang: SQLiteLang_AddString may not be used (system not started).");
		return 0;
	}
	
	if(SQLiteLang_IsIdentifierValid(identifier)) 
	{
		if(SQLiteLang_internalVariables[debugStatus])
			printf("SQLiteLang_DEBUG: The identifier \"%s\" already exists in the database.", identifier);
		return 0;
	}
	
	SQLiteLang_internalVariables[databaseHandler] = db_open(SQLiteLang_internalVariables[databaseDirectory]);
	
	new DBStatement: insertIdentifierStatement = db_prepare(SQLiteLang_internalVariables[databaseHandler], "INSERT INTO `identifiers` (`identifier_name`) VALUES (?)");
	stmt_bind_value(insertIdentifierStatement, 0, DB::TYPE_STRING, identifier);
			
	if(!stmt_execute(insertIdentifierStatement)) 
	{
		if(SQLiteLang_internalVariables[debugStatus])
			print("SQLiteLang_DEBUG: Failed to add a new identifier (database corrupted).");
		return 0;
	}
			
	printf("SQLiteLang: New identifier added (identifier: '%s').", identifier);
			
	stmt_close(insertIdentifierStatement);
	return 1;
}

/*
 * SQLiteLang_AddLanguageString
 * This function adds a new language string to the database.
 * 
 * identifier[] = The identifier is used to identify, as the parameter says, every string of text (ie. "welcome_string").
 * lang[] = The lang is used to identify the language of the string (ie. "en").
 * string[] = The string is the actual string of text to show in different languages.
*/
SQLiteLang_AddLanguageString(identifier[], lang[], string[])
{
	if(!SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang: SQLiteLang_AddString may not be used (system not started).");
		return 0;
	}
	
	// Opening a connection to the database.
	SQLiteLang_internalVariables[databaseHandler] = db_open(SQLiteLang_internalVariables[databaseDirectory]);

	// Preparing the query 'if the identifier selected exists'.
	new DBStatement: retrieveIdentifierStatement = db_prepare(SQLiteLang_internalVariables[databaseHandler], "SELECT `identifier_name` FROM `identifiers` WHERE `identifier_name` = ?");
	stmt_bind_value(retrieveIdentifierStatement, 0, DB::TYPE_STRING, identifier);

	// Executing the query 'if the identifier selected exists'.
	if(stmt_execute(retrieveIdentifierStatement)) 
	{
		// If the identifier exists then we check if the language exists.
		if(stmt_rows_left(retrieveIdentifierStatement)) 
		{
			stmt_close(retrieveIdentifierStatement);
			
			// Preparing the query 'if the language exists'.
			new DBStatement: retrieveLanguageStatement = db_prepare(SQLiteLang_internalVariables[databaseHandler], "SELECT `lang_name` FROM `languages` WHERE `lang_name` = ?");
			stmt_bind_value(retrieveLanguageStatement, 0, DB::TYPE_STRING, lang);
			
			// Executing the query 'if the language exists'.
			if(stmt_execute(retrieveLanguageStatement)) 
			{
				// If the language exists then
				if(stmt_rows_left(retrieveLanguageStatement)) 
				{
					stmt_close(retrieveLanguageStatement);
					
					new DBStatement: insertStringStatement = db_prepare(SQLiteLang_internalVariables[databaseHandler], "INSERT INTO `strings` (`identifier`, `lang`, `string`) VALUES (?, ?, ?)");
					stmt_bind_value(insertStringStatement, 0, DB::TYPE_STRING, identifier);
					stmt_bind_value(insertStringStatement, 1, DB::TYPE_STRING, lang);
					stmt_bind_value(insertStringStatement, 2, DB::TYPE_STRING, string);
					
					if(!stmt_execute(insertStringStatement)) 
					{
						print("SQLiteLang: Failed to executive SQLite query (database corrupted).");
						return 0;
					}
					
					printf("SQLiteLang: New string added (identifier: '%s' - lang: '%s' - string: '%s').", identifier, lang, string);
					
					stmt_close(insertStringStatement);
					return 1;
				}
				else
				{
					stmt_close(retrieveLanguageStatement);
					
					printf("SQLiteLang: The language \"%s\" does not exist in the database. You must create it before adding new strings.", lang);
					db_close(SQLiteLang_internalVariables[databaseHandler]);
					return 0;
				}
			}
			else 
			{
				print("SQLiteLang: Failure to execute the SQLite query (database corrupted).");
				db_close(SQLiteLang_internalVariables[databaseHandler]);
				return 0;
			}
		}
		else
		{
			stmt_close(retrieveIdentifierStatement);
			
			printf("SQLiteLang: The identifier \"%s\" does not exist in the database. You must create it before adding new strings.", identifier);
			db_close(SQLiteLang_internalVariables[databaseHandler]);
			return 0;
		}
	}
		
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
	if(!SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang: SQLiteLang_AddLanguage may not be used (system not started).");
		return 0;
	}
		
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
	if(!SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang: SQLiteLang_DeleteLanguage may not be used (system not started).");
		return 0;
	}
		
	return 1;
}

/*
 * SQLiteLang_DeleteIdentifier
 * This function deletes all the language strings of a specific identifier from the database.
 * 
 * identifier[] = The identifier is used to identify, as the parameter says, every string of text (ie. "welcome_string").
*/
SQLiteLang_DeleteIdentifier(identifier[])
{
	if(!SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang: SQLiteLang_DeleteIdentifier may not be used (system not started).");
		return 0;
	}
		
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
	if(!SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang: SQLiteLang_DeleteLanguageString may not be used (system not started).");
		return 0;
	}
		
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
	if(!SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang: SQLiteLang_ModifyLanguageString may not be used (system not started).");
		return 0;
	}
		
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
	if(!SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang: SQLiteLang_ShowLanguageString may not be used (system not started).");
		return 0;
	}
		
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
	if(!SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang: SQLiteLang_ShowPlayerString may not be used (system not started).");
		return 0;
	}
		
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
	if(!SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang: SQLiteLang_SetDefaultLang may not be used (system not started).");
		return 0;
	}
		
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
	if(!SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang: SQLiteLang_SetPlayerLang may not be used (system not started).");
		return 0;
	}
		
	return 1;
}

/*
 * SQLiteLang_IsIdentifierValid
 * This function checks if an identifier exists.
 * 
 * identifier[] = The identifier is used to identify, as the parameter says, every string of text (ie. "welcome_string").
*/
SQLiteLang_IsIdentifierValid(identifier[])
{
	if(!SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang: SQLiteLang_IsIdentifierValid may not be used (system not started).");
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
			
			if(SQLiteLang_internalVariables[debugStatus])
				printf("SQLiteLang_DEBUG: The identifier \"%s\" exists in the database.", identifier);
				
			db_close(SQLiteLang_internalVariables[databaseHandler]);
			return 1;
		}
		else
		{
			stmt_close(retrieveIdentifierStatement);
			
			if(SQLiteLang_internalVariables[debugStatus])
				printf("SQLiteLang_DEBUG: The identifier \"%s\" does not exist in the database.", identifier);
				
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
 * lang[] = The lang is used to identify the language of the string (ie. "en").
*/
SQLiteLang_IsLanguageValid(lang[])
{
	if(!SQLiteLang_internalVariables[systemStatus]) 
	{
		print("SQLiteLang: SQLiteLang_IsLanguageValid may not be used (system not started).");
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
			
			if(SQLiteLang_internalVariables[debugStatus])
				printf("SQLiteLang_DEBUG: The language \"%s\" exists in the database.", lang);
				
			db_close(SQLiteLang_internalVariables[databaseHandler]);
			return 1;
		}
		else
		{
			stmt_close(retrieveIdentifierStatement);
			
			if(SQLiteLang_internalVariables[debugStatus])
				printf("SQLiteLang_DEBUG: The language \"%s\" does not exist in the database.", lang);
				
			db_close(SQLiteLang_internalVariables[databaseHandler]);
			return 0;
		}
	}
	
	return 1;
}