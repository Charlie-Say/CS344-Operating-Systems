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

#include <conio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <inttypes.h>

struct movie
{
    char title[1000];
    int year;
    char lang[1000];
    float rating;
};

int sort_Year(const void *a, const void *b)
{
    // Compare function for qsort: year
    // Help Source: https://stackoverflow.com/questions/6105513/need-help-using-qsort-with-an-array-of-structs
    struct movie *movieA = (struct movie *)a;
    struct movie *movieB = (struct movie *)b;

    return (movieB->year - movieA->year);
}

void moviesEachYear(struct movie movie[], int n)
{
    // Sort by year
    qsort(movie, n + 1, sizeof(movie[0]), sort_Year);

    // Print the first year of every year from the sorted arrays
    int curr_year = 0;
    int i;
    for (i = 0; i < n - 1; i++)
    {
        // If current year is not equal to the pointer's year, display the year
        // If current year is equal, change pointer to the the next year in the list
        if (curr_year != movie[i].year)
        {
            printf("%d %.1f %s\n", movie[i].year, movie[i].rating, movie[i].title);
        }
        curr_year = movie[i].year;
    }
    printf("\n");
}

void getFile(struct movie movie[], FILE *file)
{
    int count = 0;
    char line[1000];
    // Get line by line in file
    while (fgets(line, sizeof(line), file))
    {
        if (count >= 1)
        {
            // Split lines into tokens
            char *tok = strtok(line, ",\n");
            int i = 0;
            // String copy the values of each token
            while (tok != NULL)
            {
                if (i == 0)
                {
                    strcpy(movie[count].title, tok);
                }
                if (i == 1)
                {
                    movie[count].year = atoi(tok); // Convert string to integer
                }
                if (i == 2)
                {
                    strcpy(movie[count].lang, tok);
                }
                if (i == 3)
                {
                    movie[count].rating = atof(tok); // Convert string to float
                }
                tok = strtok(NULL, ",\n");
                i++;
            }
        }
        count++; // Count the number of lines
    }
}

char processFile(FILE *file)
{
    return 0;
}

void createDir()
{
    int check;
    char str[20];
    char prefix[] = "sayc.movies.";
    uint64_t num;
    num = rand();
    num = (num << 32) | rand();
    num = num % 99999;
    itoa(num, str, 10);
    strcat(prefix, str);
    char *dirname = prefix;

    mkdir(dirname);
}

void caseOne()
{
    int option;
    char completefn[100];
    while (true)
    {
        printf("\n");
        printf("Which file you want to process?\n\n");
        printf("Enter 1 to pick the largest file\n");
        printf("Enter 2 to pick the smallest file\n");
        printf("Enter 3 to specify the name of a file\n\n");
        printf("Enter a choice from 1 to 3: ");
        scanf("%d", &option);
        printf("\n");
        switch (option)
        {
        case 1:
            printf("hello\n");

            continue;

        case 2:
            createDir();
            continue;
        case 3:
            printf("Enter the complete file name: ");
            scanf("%s", &completefn);

            // Open file
            FILE *file = fopen(completefn, "r");
            // Error if file not found
            if (file == NULL)
            {
                printf("\n");
                printf("File not found: %s", completefn);
                printf("\n");
                continue;
            }
            else
            {
                printf("\n\nNow processing the chosen file named %s\n\n", completefn);
            }

            continue;
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
        printf("1. Select file to process\n");
        printf("2. Exit from the program\n\n");
        printf("Enter a choice 1 or 2: ");
        scanf("%d", &option);

        // Help Source: https://www.tutorialspoint.com/cprogramming/switch_statement_in_c.htm
        switch (option)
        {

        case 1:
            caseOne();

        case 2:
            exit(0);
        }
    }
    return 0;
}