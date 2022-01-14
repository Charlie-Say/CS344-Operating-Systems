/*
Assignment 1: Movies

Instructions:
Write a program that

- Reads a CSV file with movie data that is provided to the program as an argument
- Processes the data in the file to create structs to hold data for each movie
- Creates a linked list of all these structs
- Gives user choices to ask questions about the movies in the data
- Prints out the data about the movies per user choice


Charlie Say
ONID: sayc
OSU ID: 934377368
*/

#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


struct movie{
    char title[1000];
    int year;
    char lang[1000];
    float rating;
};


void
display_ByYear(struct movie movie[], int count, int year)
{
    // Loop through all years to find match
    int flag = 0;
    for(int i = 0; i < count ;i++){
        if(movie[i].year == year){
            printf("%s\n", movie[i].title);
            flag = 1;
        }
    }

    // Error output
    if(flag == 0) printf("\nNo data about movies released in year %d\n\n", year);
    printf("\n");
}


void
display_ByLanguage(struct movie movie[], int count, char lang[])
{
    int flag = 0;

    // Loop through all languages for each movie
    for(int i = 0; i < count ;i++)
    {
        char lang_str[100];
        char *context = NULL;
        char *token;
        int h = 0;
        int k;
        int j;

        strcpy(lang_str, movie[i].lang);

        // Remove ';' from language string and replace with ' '
        while(lang_str[h] != '\0')
        {
            if(lang_str[h]==';')
            {
                lang_str[h]=' ';
            }
            h++;
        }

        // Remove '[' and ']' from string
        int len = strlen(lang_str);
        for(k=0; k < len; k++)
        {
            if(lang_str[k] == '[' || lang_str[k] == ']')
            {
                for(j=k; j < len; j++)
                {
                    lang_str[j] = lang_str[j+1];
                }
                len--;
                k--;
            }
        }

        // Tokenize language string
        // Help Source: https://stackoverflow.com/questions/15961253/c-correct-usage-of-strtok-r
        for(token = strtok_r(lang_str, " ", &context);
        token != NULL;
        token = strtok_r(NULL, " ", &context))
        {
            if(strcmp(token, lang) == 0)
            {
                printf("%d %s\n",movie[i].year, movie[i].title);
                flag = 1;
            }
        }
    }
}


int
sort_Year(const void* a, const void* b)
{
    // Compare function for qsort: year
    //Help Source: https://stackoverflow.com/questions/6105513/need-help-using-qsort-with-an-array-of-structs
    struct movie *movieA = (struct movie *)a;
    struct movie *movieB = (struct movie *)b;

    return ( movieB->year - movieA->year );
}


int
sort_Ratings(const void* a, const void* b)
{
    // Compare function for qsort: ratings
    //Help Source: https://stackoverflow.com/questions/6105513/need-help-using-qsort-with-an-array-of-structs
    struct movie *movieA = (struct movie *)a;
    struct movie *movieB = (struct movie *)b;

    if(movieB->year == movieA->year)
    {
        return (movieB->rating - movieA->rating);
    }
    else
    {
        return (movieB->year - movieA->year);
    }

}


void
display_HighestRating(struct movie movie[], int n)
{
    // Sort by year
    qsort(movie, n + 1, sizeof(movie[0]), sort_Year);
    // Sort by rating
    qsort(movie, n + 1, sizeof(movie[0]), sort_Ratings);

    // Print the first year of every year from the sorted arrays
    int curr_year = -1;
    for(int i = 0; i < n - 1; i++)
    {
        if(movie[i].year != curr_year)
        {
            printf("%d %.1f %s\n", movie[i].year, movie[i].rating, movie[i].title);
        }
        curr_year = movie[i].year;
    }
    printf("\n");
}


int
main(int argc, char** argv)
{
    if(argc == 1)
    {
        printf("No File Found.\n");
    }
    else
    {
        struct movie movie[1000];
        char* input_file = argv[1];
        char line[1000];
        char lang[1000];
        int count = 0;
        int option;
        int year;

        // Open file
        FILE* file = fopen(input_file, "r");
        // Error if file not found
        if(file == NULL){
            printf("\n");
            printf("File not found: %s", input_file);
            printf("\n");
            return 0;
        }

        //Get line by line in file
        while (fgets(line, sizeof(line), file))
        {
            if(count >= 1)
            {
                char* tok = strtok(line, ",\n");
                int k = 1;
                while(tok != NULL)
                {
                    if(k == 1)  strcpy(movie[count - 1].title,tok);
                    if(k == 2)  movie[count - 1].year = atoi(tok);
                    if(k == 3)  strcpy(movie[count - 1].lang, tok);
                    if(k == 4)  movie[count - 1].rating = atof(tok);
                    tok = strtok(NULL, ",\n");
                    k++;
                }
            }
            count++;
        }

        printf("\n\nProcessed file %s and parsed data for %d movies\n\n", input_file, count - 1);   //Remove collumn names

        while(true){
            printf("1. Show movies released in the specified year\n");
            printf("2. Show highest rated movie for each year\n");
            printf("3. Show the title and year of release of all movies in a specific language\n");
            printf("4. Exit from the program\n\n");
            printf("Enter a choice from 1 to 4: ");
            scanf("%d", &option);

            switch(option){

                case 1:
                    printf("\n");
                    printf("Enter the year for which you want to see movies: ");
                    scanf("%d", &year);
                    printf("\n");
                    printf("--------------------------------\n\n");
                    display_ByYear(movie, count, year);
                    printf("--------------------------------\n");
                    break;

                case 2:
                    printf("\n");
                    printf("--------------------------------\n\n");
                    display_HighestRating(movie, count);
                    printf("--------------------------------\n");
                    break;

                case 3:
                    printf("\n");
                    printf("Enter the language for which you want to see movies: ");
                    scanf("%s", lang);
                    printf("--------------------------------\n\n");
                    display_ByLanguage(movie, count, lang);
                    printf("\n");
                    printf("--------------------------------\n");
                    break;

                case 4:
                    printf("\n");
                    printf("--------------------------------\n\n");
                    printf("Exiting the program.\n");
                    printf("\n");
                    printf("--------------------------------\n");
                    exit(0);
            }
        }
    }
    return 0;
}