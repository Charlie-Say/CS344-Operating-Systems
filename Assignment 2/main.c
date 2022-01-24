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

// #include <conio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <inttypes.h>
#include <dirent.h>
#include <fcntl.h>

#define PREFIX "movies_"

// int checkIfFileExists(const char *filename);



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


int checkIfFileExists(const char* filename){
    struct stat buffer;
    int exist = stat(filename,&buffer);
    if(exist == 0)
        return 1;
    else  
        return 0;
}

void createDir(char* input_file)
{
    FILE *file = fopen("movies_sample_1.csv", "r");
    FILE *fp;

    struct movie movie[1000];
    // char* input_file = "movies_smallest.csv";
    // char* input_file = filename;
    char line[1000];
    int count = 0;
    int option;
    int year;
    int i;


    int flag = 0;

    int n = 0;
    char str[20];
    char numcon[10];
    char perm[] = "chmod 750 ";
    char command[50];
    char prefix[] = "sayc.movies.";
    uint64_t num;
    srand(time(0));
    num = rand();
    num = (num << 32) | rand();
    num = num % 99999;

    sprintf(str, "%d", num);

    strcat(prefix, str);
    char *dirname = prefix;
    mkdir(dirname, 0750);

    strcat(perm, prefix);
    // printf("%s", perm);
    system(perm);

    printf("\nCreated directory with name %s\n", dirname);

    // ------------------------------------------------------------

    if(file == NULL){
        printf("\n");
        printf("File not found: %s", input_file);
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


    qsort(movie, count, sizeof(movie[0]), sort_Year);

    char subfix[] = ".txt";

    for(i = 0; i < count - 1; i++)
    {
        sprintf(str, "./%s/%d", dirname, movie[i].year);
        strcat(str, subfix);
        printf("%s\n", str);

        char *txtfile = str;
        printf("%s\n", txtfile);

        if(checkIfFileExists(txtfile))
        {
            // printf("file exists for this movie\n");

            fp = fopen(txtfile, "a");
            fputs(movie[i].title, fp);
            fputs("\n", fp);

            fclose(fp);
        }
        else
        {
            // printf("file does not exists\n");

            fp = fopen(txtfile, "w+");
            fputs(movie[i].title, fp); 
            fputs("\n", fp);

            fclose(fp);
        }
    }
    printf("\n");

    fclose(file);

}

char* largestFile()
{
    // Open the current directory
    DIR* currDir = opendir(".");
    struct dirent *aDir;
    time_t lastModifTime;
    struct stat dirStat;
    struct stat st;
    int i = 0;
    char entryName[256];

    int container = 0;
    int inodeNum;
    char *fileName;

    // Go through all the entries
    while((aDir = readdir(currDir)) != NULL)
    {
        if(strncmp(PREFIX, aDir->d_name, strlen(PREFIX)) == 0)
        {
            // Get meta-data for the current entry
            stat(aDir->d_name, &dirStat);
            memset(entryName, '\0', sizeof(entryName));
            strcpy(entryName, aDir->d_name);
            // printf("%s\n", entryName);

            if (stat(entryName, &st) == 0)
            {
                if(st.st_size > container)
                {
                    container = st.st_size;
                    inodeNum = st.st_ino;
                    // strcpy(fileName, aDir->d_name);
                    fileName = aDir->d_name;

                    // printf("%d\n", container);
                    // printf("%d\n", inodeNum);
                    // printf("%s\n", fileName);
                }
            }
        }
    }
    // Close the directory
    closedir(currDir);

    return fileName;




}


// size_t getFilesize(char* filename) {
//     struct stat st;
//     if(stat(filename, &st) != 0) {
//         return 0;
//     }
//     return st.st_size;
// }



void smallestFile()
{
    system("find ./ -name 'movies_*.csv");
}

void caseOne()
{
    int option;
    char completefn[100];
    char* filePtr;

    while (true)
    {
        int check;

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
            // Largest File
            filePtr = largestFile();
            // printf("%s", filePtr);
            createDir(filePtr);



            // createDir("movies_smallest.csv");

            continue;

        case 2:
            // Smallest File
            createDir("movies_smallest.csv");

            continue;

        case 3:
            // Custom File
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

                // createDir("movies_smallest.csv");
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