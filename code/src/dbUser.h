#include <stdlib.h>
#include "globals.h"

int userCallback(void *data, int argc, char **argv, char **colName);
int checkUniqueness(sqlite3 *db, const char *table, const char *column, const char *value);
int welcomeLogic();
int loginLogic(sqlite3 *db);
void createAccountLogic(sqlite3 *db);
void blankQuestLog(sqlite3 *db);
void deleteAccount(sqlite3 *db);

int userCallback(void *data, int argc, char **argv, char **colName) 
{
    if (data) {
        int *user_id = (int *)data;
        if (argc > 0 && argv[0]) {
            *user_id = atoi(argv[0]); 
        }
    }
    return 0;
}

int checkUniqueness(sqlite3 *db, const char *table, const char *column, const char *value) 
{
    char sql[256];
    char *errMsg = 0;
    int exists = 0;

    snprintf(sql, sizeof(sql),
             "SELECT 1 FROM %s WHERE %s = '%s';",
             table, column, value);

    int rc = sqlite3_exec(db, sql, [](void *data, int argc, char **argv, char **colName) -> int {
        *(int *)data = 1;
        return 0;
    }, &exists, &errMsg);

    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return -1; 
    }

    return exists;
}

int loginLogic(sqlite3 *db) 
{
    char username[50], password[50];
    char sql[256];
    char *errMsg = 0;
    int user_id = -1; 

    while (user_id == -1) 
    {
        printf("Enter username: ");
        scanf("%49s", username);
        printf("Enter password: ");
        scanf("%49s", password);

        snprintf(sql, sizeof(sql),
                 "SELECT user_id FROM user WHERE username = '%s' AND password = '%s';",
                 username, password);

        int rc = sqlite3_exec(db, sql, userCallback, &user_id, &errMsg);
        if (rc != SQLITE_OK) 
        {
            fprintf(stderr, "SQL error: %s\n", errMsg);
            sqlite3_free(errMsg);
            return -1; 
        }

        if (user_id != -1) 
        {
            printf("Login successful! Welcome, %s.\n", username);
        } 
        else 
        {
            printf("Invalid username or password. Please try again.\n");
        }
    }

    return user_id;
}

void createAccountLogic(sqlite3 *db) 
{
    char username[50], password[50], email[100], firstName[50], lastName[50], phone[20];
    char sql[512];
    char *errMsg = 0;

    while (1) 
    {
        printf("Enter your email: ");
        scanf("%99s", email);

        if (checkUniqueness(db, "user", "email", email) == 1) 
        {
            printf("Email '%s' is already registered. Please use a different email.\n", email);
        } 
        else 
        {
            break;
        }
    }

    while (1) 
    {
        printf("Enter a username: ");
        scanf("%49s", username);

        if (checkUniqueness(db, "user", "username", username) == 1) 
        {
            printf("Username '%s' is already taken. Please choose a different username.\n", username);
        } 
        else 
        {
            break; 
        }
    }

    printf("Enter a password: ");
    scanf("%49s", password);

    printf("Enter your first name: ");
    scanf("%49s", firstName);

    printf("Enter your last name: ");
    scanf("%49s", lastName);

    printf("Enter your phone number (or type 'NULL' if you don't want to provide one): ");
    scanf("%19s", phone);

    snprintf(sql, sizeof(sql),
             "INSERT INTO user (username, password, email, first_name, last_name, phone_num) "
             "VALUES ('%s', '%s', '%s', '%s', '%s', %s);",
             username, password, email, firstName, lastName,
             (strcmp(phone, "NULL") == 0 ? "NULL" : phone));

    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return;
    }

    user_id = (int)sqlite3_last_insert_rowid(db);
    printf("Account successfully created! Please Log In. \n");

    blankQuestLog(db);

    loginLogic(db);
}


void blankQuestLog(sqlite3 *db)
{
    int amountOfQuest = 4; 
    for (int i = 1; i <= amountOfQuest; i++)
    {
        char sql[512];
        snprintf(sql, sizeof(sql),
                "INSERT INTO quest_log (user_id, quest_id, is_completed)"
                "VALUES (%d, %d, 0);", user_id, i);

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
            //printf("Blank quest log successfully added.\n");
        }
    }      
}

void deleteAccount(sqlite3 *db)
{
    char confirmation[2];
    printf("Are you sure you want to delete your account? This action cannot be undone. (Y/N): ");
    scanf(" %1s", confirmation);

    if (strcmp(confirmation, "Y") == 0 || strcmp(confirmation, "y") == 0)
    {
        char sql[512];
        char *errMsg = 0;

        int rc = sqlite3_exec(db, "BEGIN TRANSACTION;", 0, 0, &errMsg);
        if (rc != SQLITE_OK)
        {
            fprintf(stderr, "SQL error: %s\n", errMsg);
            sqlite3_free(errMsg);
            return;
        }

        snprintf(sql, sizeof(sql), "DELETE FROM quest_log WHERE user_id = %d;", user_id);
        rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
        if (rc != SQLITE_OK)
        {
            fprintf(stderr, "SQL error: %s\n", errMsg);
            sqlite3_free(errMsg);
            sqlite3_exec(db, "ROLLBACK;", 0, 0, 0); 
            return;
        }

        snprintf(sql, sizeof(sql), "DELETE FROM char_list WHERE user_id = %d;", user_id);
        rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
        if (rc != SQLITE_OK)
        {
            fprintf(stderr, "SQL error: %s\n", errMsg);
            sqlite3_free(errMsg);
            sqlite3_exec(db, "ROLLBACK;", 0, 0, 0); 
            return;
        }

        snprintf(sql, sizeof(sql), "DELETE FROM user WHERE user_id = %d;", user_id);
        rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
        if (rc != SQLITE_OK)
        {
            fprintf(stderr, "SQL error: %s\n", errMsg);
            sqlite3_free(errMsg);
            sqlite3_exec(db, "ROLLBACK;", 0, 0, 0);
            return;
        }

        rc = sqlite3_exec(db, "COMMIT;", 0, 0, &errMsg);
        if (rc != SQLITE_OK)
        {
            fprintf(stderr, "SQL error: %s\n", errMsg);
            sqlite3_free(errMsg);
        }
        else
        {
            printf("Account successfully deleted. Goodbye!\n");
        }
    }
    else
    {
        printf("Account deletion canceled.\n");
    }
}
