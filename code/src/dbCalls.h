#include <cstring>

int welcomeLogic();
int mainMenuLogic();
int openDB(sqlite3 **db, const char *dbFile);
void closeDB(sqlite3 *db);
int printCallback(void *NotUsed, int argc, char **argv, char **colName);
void printTable (sqlite3 *db, const char *tableName);
void printMarket(sqlite3 *db);

int welcomeLogic() 
{
    char response[2]; 
    int valid = 0;    

    while (!valid) 
    {
        printf("Welcome to Quest Unlimited! Log In (L) or Create An Account (C): \n");
        scanf(" %1s", response); 

        if (strcmp(response, "L") == 0 || strcmp(response, "l") == 0) 
        {
            //printf("Logging in...\n");
            valid = 1; 
            return 2;
        } 
        else if (strcmp(response, "C") == 0 || strcmp(response, "c") == 0) 
        {
            //printf("Creating an account...\n");
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

int mainMenuLogic() 
{
    char response[2]; 
    int valid = 0;    

    while (!valid) 
    {
        printf("Account: (A) Battle: (B) Quest Log: (Q) Marketplace: (P) Delete Account (D) Exit: (E)\n");
        scanf(" %1s", response); 

        if (strcmp(response, "A") == 0 || strcmp(response, "a") == 0) 
        {
            // Account Logic
            valid = 1; 
            return 1;
        } 
        else if (strcmp(response, "B") == 0 || strcmp(response, "b") == 0) 
        {
            // Battle Logic
            valid = 1; 
            return 2;
        }
        else if (strcmp(response, "Q") == 0 || strcmp(response, "q") == 0) 
        {
            // Quest Log Logic
            valid = 1; 
            return 3;
        }
        else if (strcmp(response, "P") == 0 || strcmp(response, "p") == 0) 
        {
            // Marketplace Logic
            valid = 1; 
            return 4;
        }   
        else if (strcmp(response, "D") == 0 || strcmp(response, "d") == 0) 
        {
            // Delete Account
            valid = 1; 
            return 5;
        } 
        else if (strcmp(response, "E") == 0 || strcmp(response, "e") == 0) 
        {
            // Exit Logic
            valid = 1; 
            return 6;
        } 
        else 
        {
            printf("Error: Your response is invalid. Please try again.\n");
        }
    }
    return 0;
}

int openDB(sqlite3 **db, const char *dbFile) 
{
    int rc = sqlite3_open(dbFile, db);
    //printf("Attempting to open database at: %s\n", dbFile);
    if (rc) 
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(*db));
    } 
    
    //fprintf(stdout, "Opened database successfully\n");
    return 0;
}

void closeDB(sqlite3 *db)
{
    sqlite3_close(db);
    //fprintf(stdout, "Database closed sucessfully\n");
}

int printCallback(void *data, int argc, char **argv, char **colName) 
{
    int *printHeader = (int *)data;

    // Print header
    if (*printHeader) {
        printf("+");
        for (int i = 0; i < argc; i++) {
            printf("---------------+");
        }
        printf("\n|");
        for (int i = 0; i < argc; i++) {
            printf("%-15s|", colName[i]);
        }
        printf("\n+");
        for (int i = 0; i < argc; i++) {
            printf("---------------+");
        }
        printf("\n");
        *printHeader = 0;
    }

    // Print row values
    printf("|");
    for (int i = 0; i < argc; i++) {
        printf("%-15s|", argv[i] ? argv[i] : "NULL");
    }
    printf("\n");

    return 0;
}

void printTable(sqlite3 *db, const char *tableName)
{
    char sql[256];
    snprintf(sql, sizeof(sql), "SELECT * FROM %s;", tableName);

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
        fprintf(stdout, "Table '%s' printed successfully.\n", tableName);
    }
}

void printMarket(sqlite3 *db) 
{
    char sql[] = 
        "SELECT "
        "    item.name AS Item, "
        "    market.price AS Price, "
        "    market.amount AS Amount, "
        "    item.rarity AS Rarity, "
        "    market.is_sold AS Availability "
        "FROM "
        "    selling "
        "JOIN "
        "    item "
        "ON "
        "    selling.item_id = item.item_id "
        "JOIN "
        "    market "
        "ON "
        "    selling.market_id = market.market_id "
        "WHERE "
        "    selling.item_id AND selling.market_id;";

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
        //printf("Market data printed successfully.\n");
    }
}
