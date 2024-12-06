#include "globals.h"

int accountViewLogic();
int characterMenuLogic();
void printCharTable();
void createCharacter(sqlite3 *db);
void printCharacterTable(sqlite3 *db);
void printInventoryTable(sqlite3 *db);
void updateCharacterName(sqlite3 *db);

int accountViewLogic()
{
    char response[2]; 
    int valid = 0;    

    while (!valid) 
    {
        printf("View Characters: (C) View Inventory: (I) Main Menu: (M) \n");
        scanf(" %1s", response); 

        if (strcmp(response, "C") == 0 || strcmp(response, "c") == 0) 
        {
            // Character Logic
            valid = 1; 
            return 1;
        } 
        else if (strcmp(response, "I") == 0 || strcmp(response, "i") == 0) 
        {
            // Inventory Logic
            valid = 1; 
            return 2;
        } 
        else if (strcmp(response, "M") == 0 || strcmp(response, "m") == 0) 
        {
            // Main Menu Logic
            valid = 1;
            return 3;
        } 
        else 
        {
            printf("Error: Your response is invalid. Please try again.\n");
        }
    }
    return 0;
}

int characterMenuLogic()
{

    char response[2]; 
    int valid = 0;    

    while (!valid) 
    {
        printf("Create New Character: (N) Rename Character: (R) Account Menu (A) \n");
        scanf(" %1s", response); 

        if (strcmp(response, "N") == 0 || strcmp(response, "n") == 0) 
        {
            // New Character Logic
            valid = 1; 
            return 1;
        } 
        else if (strcmp(response, "R") == 0 || strcmp(response, "r") == 0) 
        {
            // Rename Character
            valid = 1; 
            return 2;
        }
        else if (strcmp(response, "A") == 0 || strcmp(response, "a") == 0) 
        {
            // Account Menu Logic
            valid = 1; 
            return 3;
        }
        else 
        {
            printf("Error: Your response is invalid. Please try again.\n");
        }
    }
    return 0;
}

void printCharacterTable(sqlite3 *db) 
{
    char sql[512];
    snprintf(sql, sizeof(sql),
             "SELECT character.name AS Name, character.level AS Level, character.class AS Class "
             "FROM char_list "
             "JOIN character ON char_list.character_id = character.character_id "
             "WHERE char_list.user_id = %d;", user_id);

    char *errMsg = 0;

    int printHeader = 1;
    int rc = sqlite3_exec(db, sql, printCallback, &printHeader, &errMsg);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    } 
    else 
    {
        //printf("Character list displayed successfully.\n");
    }
}

void printInventoryTable(sqlite3 *db) 
{
    char sql[512];
    snprintf(sql, sizeof(sql),
             "SELECT item.name AS Item_Name, "
             "       SUM(contains.amount) AS Amount "
             "FROM owns "
             "JOIN inventory ON owns.inventory_id = inventory.inventory_id "
             "JOIN contains ON inventory.inventory_id = contains.inventory_id "
             "JOIN item ON contains.item_id = item.item_id "
             "WHERE owns.user_id = %d "
             "GROUP BY item.name;", user_id);

    char *errMsg = 0;

    // Execute SQL query using the existing printCallback function
    int printHeader = 1;
    int rc = sqlite3_exec(db, sql, printCallback, &printHeader, &errMsg);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    } 
    else 
    {
        //printf("Inventory table displayed successfully.\n");
    }
}


void createCharacter(sqlite3 *db) 
{
    char name[50], classType[50];
    char sql[512];
    char *errMsg = 0;

    printf("Enter character name: ");
    scanf("%49s", name);

    printf("Enter character class: ");
    scanf("%49s", classType);


    // Insert new character into the database
    snprintf(sql, sizeof(sql),
             "INSERT INTO character (name, class, hp, mp, xp, damage, level, gold) VALUES ('%s', '%s', '%d', '%d', '%d', '%d', '%d', '%d');",
             name, classType, 100, 50, 500, 50, 1, 100);

    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return;
    }

    // Retrieve the new character's ID
    int character_id = (int)sqlite3_last_insert_rowid(db);

    // Link the character with the user in the char_list table
    snprintf(sql, sizeof(sql),
             "INSERT INTO char_list (user_id, character_id) VALUES (%d, %d);",
             user_id, character_id);

    rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    } 
    else 
    {
        printf("Character '%s' created and linked to your account successfully.\n", name);
    }
}

void printQuestLog(sqlite3 *db)
{
    char sql[512];
    snprintf(sql, sizeof(sql),
            "SELECT q.name AS Quest_Name, "
            "q.description AS Description, "
            "CASE WHEN ql.is_completed = 1 THEN 'Completed' ELSE 'Not Completed' END AS Status "
            "FROM quest_log ql "
            "JOIN quest q ON ql.quest_id = q.quest_id "
            "WHERE ql.user_id = %d;", user_id);

    char *errMsg = 0;

    // Execute SQL query using the existing printCallback function
    int printHeader = 1;
    int rc = sqlite3_exec(db, sql, printCallback, &printHeader, &errMsg);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    } 
    else 
    {
        //printf("Quest log displayed successfully.\n");
    }
}

void updateCharacterName(sqlite3 *db) 
{
    int character_id;
    char newName[50];
    char sql[512];
    char *errMsg = 0;

    // Display the user's characters
    printf("Your Characters:\n");
    snprintf(sql, sizeof(sql),
             "SELECT character.character_id, character.name, character.class, character.level "
             "FROM char_list "
             "JOIN character ON char_list.character_id = character.character_id "
             "WHERE char_list.user_id = %d;", user_id);

    int printHeader = 1;
    int rc = sqlite3_exec(db, sql, printCallback, &printHeader, &errMsg);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return;
    }

    // Prompt the user to select a character by ID
    printf("\nEnter the ID of the character you want to rename: ");
    scanf("%d", &character_id);

    // Prompt for the new name
    printf("Enter the new name for the character: ");
    scanf("%49s", newName);

    // Check if the name is unique
    snprintf(sql, sizeof(sql), "SELECT 1 FROM character WHERE name = '%s';", newName);
    int nameExists = 0;
    rc = sqlite3_exec(db, sql, [](void *data, int argc, char **argv, char **colName) -> int {
        *(int *)data = 1; // Name exists
        return 0;
    }, &nameExists, &errMsg);

    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return;
    }

    if (nameExists) 
    {
        printf("The name '%s' is already taken. Please choose a different name.\n", newName);
        return;
    }

    // Update the character's name
    snprintf(sql, sizeof(sql),
             "UPDATE character SET name = '%s' WHERE character_id = %d;", newName, character_id);
    rc = sqlite3_exec(db, sql, 0, 0, &errMsg);

    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    } 
    else 
    {
        printf("Character name successfully updated to '%s'.\n", newName);
    }
}
