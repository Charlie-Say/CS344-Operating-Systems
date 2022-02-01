/*
Assignment 2: Files & Directories

Instructions:
Write a program that

- Reads directory entries
- Finds a file in the current directory based on user specified criteria
- Reads and processes the data in the chosen file
- Creates a directory
- Creates new files in the newly created directory and writes processed data to these files


Charlie Say
ONID: sayc
OSU ID: 934377368
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <inttypes.h>
#include <dirent.h>
#include <fcntl.h>

#define PREFIX "movies_"

// Create struct for movie file data
struct movie
{
    char title[1000];
    int year;
    char lang[1000];
    float rating;
};


// Sorrt for processing file
int sort_Year(const void *a, const void *b)
{
    // Compare function for qsort: year
    // Help Source: https://stackoverflow.com/questions/6105513/need-help-using-qsort-with-an-array-of-structs
    struct movie *movieA = (struct movie *)a;
    struct movie *movieB = (struct movie *)b;

    return (movieB->year - movieA->year);
}


// Check directory if file exists
int checkIfFileExists(const char* filename){
    struct stat buffer;
    int exist = stat(filename, &buffer);
    if(exist == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


// Create directory and files with movie by year
void createDir(char input_file[1024])
{
    FILE* file = fopen(input_file, "r");
    FILE* fp;

    struct movie movie[1000];
    char line[1000];
    int count = 0;
    int year;
    int i;

    char str[20];
    char perm[] = "chmod 750 ";
    char prefix[] = "sayc.movies.";
    uint64_t num;
    srand(time(0));         // Randomize numbers every time the application runs
    num = rand();
    num = (num << 32) | rand();
    num = num % 99999;

    sprintf(str, "%d", num);        // Convert random num to string

    strcat(prefix, str);        // Concat random-num-string to string prefix "sayc.movies."
    char *dirname = prefix;
    mkdir(dirname, 0750);       // Make directory

    // strcat(perm, prefix);
    // system(perm);

    printf("\nCreated directory with name %s\n\n", dirname);

    // ERROR for file
    if(file == NULL){
        printf("\n");
        printf("ERROR FILE NOT FOUND: %s", input_file);
        printf("\n");
    }

    // Get line by line in file
    while (fgets(line, sizeof(line), file))
    {
        if(count >= 1)
        {
            // Split lines into tokens
            char* tok = strtok(line, ",\n");
            int i = 0;
            // String copy the values of each token
            while(tok != NULL)
            {
                if(i == 0)
                {
                    strcpy(movie[count].title,tok);
                }
                if(i == 1)
                {
                    movie[count].year = atoi(tok);      // Convert string to integer
                }
                if(i == 2)
                {
                    strcpy(movie[count].lang, tok);
                }
                if(i == 3)
                {
                    movie[count].rating = atof(tok);    // Convert string to float
                }
                tok = strtok(NULL, ",\n");
                i++;
            }
        }
        count++;    // Count the number of lines
    }

    qsort(movie, count, sizeof(movie[0]), sort_Year);       // Qsort movies by year

    char subfix[] = ".txt";
    for(i = 0; i < count - 1; i++)
    {
        // Create string for the file name
        sprintf(str, "./%s/%d", dirname, movie[i].year);
        strcat(str, subfix);
        char *txtfile = str;

        // Check if file already exists in directory
        if(checkIfFileExists(txtfile))
        {
            // If file exists, append data to file
            fp = fopen(txtfile, "a");
            fputs(movie[i].title, fp);
            fputs("\n", fp);
            fclose(fp);
        }
        else
        {
            // If file does not exist, create new file
            fp = fopen(txtfile, "w+");
            fputs(movie[i].title, fp); 
            fputs("\n", fp);
            printf("File Created: %s\n", txtfile);
            fclose(fp);
            chmod(txtfile, S_IRUSR|S_IWUSR|S_IRGRP);
        }
    }
    printf("\n");
    fclose(file);
}


// Find largest file
char* largestFile()
{
    // Open the current directory
    DIR* currDir = opendir(".");
    struct dirent *aDir;
    struct stat dirStat;        // Struct for directory data
    struct stat st;             // Struct for file data
    char entryName[256];
    int container = 0;
    char* fileName;
    char* str;
    int len;
    char* last_four;        // Pointer to the last 4 characters of file string
    char* substr = ".csv";

    // Go through all the entries
    while((aDir = readdir(currDir)) != NULL)
    {
        // Create pointer for last four characters of file string
        str = aDir->d_name;
        len = strlen(str);
        last_four = &str[len-4];

        // String compare the PREFIX "movies_" with characters of file name with length of PREFIX
        if(strncmp(PREFIX, aDir->d_name, strlen(PREFIX)) == 0)
        {
            // String compare last four characters of file with substring ".csv"
            if (!strcmp(last_four, substr))
            {
                // Copy pointer of file name to match with data in struct
                stat(aDir->d_name, &dirStat);
                memset(entryName, '\0', sizeof(entryName));
                strcpy(entryName, aDir->d_name);

                if (stat(entryName, &st) == 0)
                {
                    // Check if file is larger than previous file
                    if(st.st_size > container)
                    {
                        container = st.st_size;
                        fileName = aDir->d_name;
                    }
                }
            }
        }
    }
    // Close the directory
    closedir(currDir);

    return fileName;
}


// Find smallest file
char* smallestFile()
{
    // Open the current directory
    DIR* currDir = opendir(".");
    struct dirent *aDir;
    struct stat dirStat;
    struct stat st;
    char entryName[256];
    int container = 5000;
    char *fileName;
    char* str;
    int len;
    char *last_four;
    char* substr = ".csv";

    // Go through all the entries
    while((aDir = readdir(currDir)) != NULL)
    {
        // Create pointer for last four characters of file string
        str = aDir->d_name;
        len = strlen(str);
        last_four = &str[len-4];

        // String compare the PREFIX "movies_" with characters of file name with length of PREFIX
        if(strncmp(PREFIX, aDir->d_name, strlen(PREFIX)) == 0)
        {
            // String compare last four characters of file with substring ".csv"
            if (!strcmp(last_four, substr))
            {
                stat(aDir->d_name, &dirStat);
                memset(entryName, '\0', sizeof(entryName));
                strcpy(entryName, aDir->d_name);

                if (stat(entryName, &st) == 0)
                {
                    // Check if file is larger than previous file
                    if(st.st_size < container)
                    {
                        container = st.st_size;
                        fileName = aDir->d_name;
                    }
                }
            }
        }
    }
    // Close the directory
    closedir(currDir);

    return fileName;
}


// User selects option 1 from main menu
void caseOne()
{
    int option;
    char completefn[1024];
    char* filePtr;
    char fn[1024];

    while (true)
    {
        // Give options to user
        printf("\n");
        printf("Which file you want to process?\n\n");
        printf("Enter 1 to pick the largest file\n");
        printf("Enter 2 to pick the smallest file\n");
        printf("Enter 3 to specify the name of a file\n\n");
        printf("Enter a choice from 1 to 3: ");
        scanf("%d", &option);

        if(option == 1)
        {
            // Largest File
            filePtr = largestFile();
            strcpy(fn, filePtr);
            printf("\n\nNow processing the chosen file named %s\n", fn);
            createDir(fn);
            return;
        }
        if(option == 2)
        {
            // Smallest File
            filePtr = smallestFile();
            strcpy(fn, filePtr);
            printf("\n\nNow processing the chosen file named %s\n", fn);
            createDir(fn);
            return;
        }
        if(option == 3)
        {
            // Custom File
            printf("Enter the complete file name: ");
            scanf("%s", &completefn);

            // Open file
            FILE *file = fopen(completefn, "r");
            // Error if file not found
            if (file == NULL)
            {
                // ERROR handler for valid file
                printf("\n");
                printf("ERROR FILE NOT FOUND: %s", completefn);
                printf("\n");
                continue;
            }
            else
            {
                printf("\n\nNow processing the chosen file named %s\n", completefn);
                createDir(completefn);
                return;
            }
        }
        if(option != 1 || option != 2 || option != 3)
        {
            // ERROR message for valid user option
            printf("Not a valid option. Please try again.\n\n");
        }
    }
}


int main()
{
    struct movie movie[1000];
    int option;
    int year;

    while (true)
    {
        // Options for user
        printf("1. Select file to process\n");
        printf("2. Exit from the program\n\n");
        printf("Enter a choice 1 or 2: ");
        scanf("%d", &option);

        if(option == 1)
        {
            caseOne();
        }
        else
        {
            printf("Not a valid option. Please try again.\n\n");
        }
        if(option == 2)
        {
            printf("Exiting the program.");
            exit(0);
        }

    }
    return 0;
}