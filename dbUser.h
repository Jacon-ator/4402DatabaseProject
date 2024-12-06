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
        int *user_id = (int *)data; // Cast data to an integer pointer
        if (argc > 0 && argv[0]) {
            *user_id = atoi(argv[0]); // Store the user_id as an integer
        }
    }
    return 0;
}

int checkUniqueness(sqlite3 *db, const char *table, const char *column, const char *value) 
{
    char sql[256];
    char *errMsg = 0;
    int exists = 0; // Flag to indicate if the value exists

    // Construct the SQL query
    snprintf(sql, sizeof(sql),
             "SELECT 1 FROM %s WHERE %s = '%s';",
             table, column, value);

    // Execute the SQL query
    int rc = sqlite3_exec(db, sql, [](void *data, int argc, char **argv, char **colName) -> int {
        *(int *)data = 1; // Set exists to true if a row is found
        return 0;
    }, &exists, &errMsg);

    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return -1; // Return -1 to indicate an error
    }

    return exists; // Return 1 if the value exists, 0 otherwise
}

int loginLogic(sqlite3 *db) 
{
    char username[50], password[50];
    char sql[256];
    char *errMsg = 0;
    int user_id = -1; // Variable to store the user ID

    while (user_id == -1) 
    {
        // Prompt for username and password
        printf("Enter username: ");
        scanf("%49s", username);
        printf("Enter password: ");
        scanf("%49s", password);

        // Construct SQL query
        snprintf(sql, sizeof(sql),
                 "SELECT user_id FROM user WHERE username = '%s' AND password = '%s';",
                 username, password);

        // Execute query and pass user_id as data
        int rc = sqlite3_exec(db, sql, userCallback, &user_id, &errMsg);
        if (rc != SQLITE_OK) 
        {
            fprintf(stderr, "SQL error: %s\n", errMsg);
            sqlite3_free(errMsg);
            return -1; // Return -1 on error
        }

        // Check if the user was found
        if (user_id != -1) 
        {
            printf("Login successful! Welcome, %s.\n", username);
        } 
        else 
        {
            printf("Invalid username or password. Please try again.\n");
        }
    }

    return user_id; // Return the user ID to the caller
}

void createAccountLogic(sqlite3 *db) 
{
    char username[50], password[50], email[100], firstName[50], lastName[50], phone[20];
    char sql[512];
    char *errMsg = 0;

    // Prompt for a unique email
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
            break; // Email is unique
        }
    }

    // Prompt for a unique username
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
            break; // Username is unique
        }
    }

    // Prompt for other user details
    printf("Enter a password: ");
    scanf("%49s", password);

    printf("Enter your first name: ");
    scanf("%49s", firstName);

    printf("Enter your last name: ");
    scanf("%49s", lastName);

    printf("Enter your phone number (or type 'NULL' if you don't want to provide one): ");
    scanf("%19s", phone);

    // Insert new user into the database
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

    // Capture the new user's ID
    user_id = (int)sqlite3_last_insert_rowid(db);
    printf("Account successfully created! Please Log In. \n");

    // Add a blank quest log for the new user
    blankQuestLog(db);

    // Redirect to login logic
    loginLogic(db);
}


void blankQuestLog(sqlite3 *db)
{
    int amountOfQuest = 4; // Change based on amount of quest in game
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

        // Start transaction
        int rc = sqlite3_exec(db, "BEGIN TRANSACTION;", 0, 0, &errMsg);
        if (rc != SQLITE_OK)
        {
            fprintf(stderr, "SQL error: %s\n", errMsg);
            sqlite3_free(errMsg);
            return;
        }

        // Delete from quest_log
        snprintf(sql, sizeof(sql), "DELETE FROM quest_log WHERE user_id = %d;", user_id);
        rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
        if (rc != SQLITE_OK)
        {
            fprintf(stderr, "SQL error: %s\n", errMsg);
            sqlite3_free(errMsg);
            sqlite3_exec(db, "ROLLBACK;", 0, 0, 0); // Rollback transaction on failure
            return;
        }

        // Delete from char_list
        snprintf(sql, sizeof(sql), "DELETE FROM char_list WHERE user_id = %d;", user_id);
        rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
        if (rc != SQLITE_OK)
        {
            fprintf(stderr, "SQL error: %s\n", errMsg);
            sqlite3_free(errMsg);
            sqlite3_exec(db, "ROLLBACK;", 0, 0, 0); // Rollback transaction on failure
            return;
        }

        // Delete from user table
        snprintf(sql, sizeof(sql), "DELETE FROM user WHERE user_id = %d;", user_id);
        rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
        if (rc != SQLITE_OK)
        {
            fprintf(stderr, "SQL error: %s\n", errMsg);
            sqlite3_free(errMsg);
            sqlite3_exec(db, "ROLLBACK;", 0, 0, 0); // Rollback transaction on failure
            return;
        }

        // Commit transaction
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
