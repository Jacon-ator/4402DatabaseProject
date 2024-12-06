#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include "dbCalls.h"
#include "dbUser.h"
#include "dbAccount.h"
#include "globals.h"

const char *dbString = "/workspaces/Code/database/4402Project.db";
int user_id = -1;

int main() 
{
    sqlite3 *db;
    const char *dbFile = "/workspaces/Code/database/4402Project.db";

    if (openDB(&db, dbFile)) 
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    /* Welcome & Account Logic */
    int rcWelcome = welcomeLogic();

    if (rcWelcome == 2) 
    {
        user_id = loginLogic(db);
    } 
    else if (rcWelcome == 3) 
    {
        createAccountLogic(db);
    } 
    else 
    {
        fprintf(stderr, "Error with Welcome Logic\n");
    }

    /* Main Menu Loop */
    int running = 1;
    while (running) 
    {
        int rcMainMenu = mainMenuLogic();

        switch (rcMainMenu) 
        {
            case 1: 
            {
                // Account Logic
                int rcAccount = accountViewLogic();
                switch (rcAccount) 
                {
                    case 1: 
                    {
                        // View Characters
                        printCharacterTable(db);

                        char response[2];
                        printf("Create Character: (C) Rename Character: (R) Account Menu: (A) \n");
                        scanf(" %1s", response);

                        if (strcmp(response, "C") == 0 || strcmp(response, "c") == 0) 
                        {
                            createCharacter(db);
                        } 
                        else if (strcmp(response, "R") == 0 || strcmp(response, "r") == 0) 
                        {
                            updateCharacterName(db);
                        }
                        else if (strcmp(response, "A") == 0 || strcmp(response, "a") == 0) 
                        {
                            break;
                        }

                        break;
                    }
                    case 2:
                    {
                        // View Inventory
                        printInventoryTable(db);
                        break;
                    }
                    case 3:
                        // Main Menu
                        break;
                    default:
                        fprintf(stderr, "Error: Unexpected account logic response.\n");
                        break;
                }
                break;
            }

            case 2:
            {
                // Battle Logic
                printf("Battling will be implemented in a future update! Please stay tuned.\n");
                break;
            }

            case 3:
            {
                // Quest Log Logic
                printQuestLog(db);
                break;
            }

            case 4:
            {
                // Marketplace Logic
                printMarket(db);
                break;
            }
             
             case 5:
            {
                deleteAccount(db);
                running = 0;
                break;
            }

            case 6:
            {
                // Exit Logic
                running = 0;
                break;
            }

            default:
                fprintf(stderr, "Error with Main Menu Logic\n");
                break;
        }
    }

    closeDB(db);
    return 0;
}
