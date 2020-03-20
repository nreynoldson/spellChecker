/*
 * CS 261 Data Structures
 * Assignment 5: Spell checker
 * Name: Nicole	Reynoldson
 * Date: 3/3/2020
 */

#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */
char* nextWord(FILE* file)
{
    int maxLength = 16;
    int length = 0;
    char* word = malloc(sizeof(char) * maxLength);
    while (1)
    {
        char c = fgetc(file);
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            c == '\'')
        {
            if (length + 1 >= maxLength)
            {
                maxLength *= 2;
                word = realloc(word, maxLength);
            }
            word[length] = c;
            length++;
        }
        else if (length > 0 || c == EOF)
        {
            break;
        }
    }
    if (length == 0)
    {
        free(word);
        return NULL;
    }
    word[length] = '\0';
    return word;
}

/**
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map)
{
	char* word = nextWord(file);

	while (word != NULL)
	{
		hashMapPut(map, word, 0);
		free(word);
		word = nextWord(file);
	}
	free(word);
}

/**
 * Returns the value of the smallest parameter.
 * @param three ints
 */
int findMin(int a, int b, int c)
{
	if (a <= b && a <= c)
		return a;
	else if (b <= a && b <= c)
		return b;
	else
		return c;
}


/**
 * Calculates the Levenshtein Distance between two strings
 * @param two const char*
 */
int LDistance(const char* r, const char* c)
{
	int rows = strlen(r) + 1;
	int cols = strlen(c) + 1;

	//Dynamically allocate a matrix arr[rows][cols]
	int ** arr = (int **)malloc(sizeof(int *) * rows);
	assert(arr != 0);
	for (int i = 0; i < rows; i++) {
		arr[i] = (int*)malloc(cols * sizeof(int));
		assert(arr[i] != NULL);

	}

	// Initialize the first column
	for (int i = 0; i < rows; i++) 
		arr[i][0] = i;
	
	//Initialize the first row
	for (int j = 0; j < cols; j++)
		arr[0][j] = j;


	int left, up, diag;
	for (int i = 1; i < rows; i++) {
		for (int j = 1; j < cols; j++) {

			// Calculate position of each to check
			left = arr[i][j - 1];
			up = arr[i - 1][j];
			diag = arr[i - 1][j - 1];

			int cost;
			//If the chracters don't match, replace, remove or insert
			if (r[i - 1] == c[j - 1])
				cost = 0;
			//If characters do match, no edits needed
			else
				cost = 1;

			arr[i][j] = findMin(left + 1, up + 1, diag + cost);

		}
	}

	int editDistance = arr[rows - 1][cols - 1];

	for (int i = 0; i < rows; i++)
		free(arr[i]);
	free(arr);

	return editDistance;

}


/**
 * Returns the index of the element containing the largest value
 * @param array of hashlinks
 * @param array length
 */
int calculateMaxIdx(HashLink** array, int arrLen)
{
	int max = 0;

	for (int i = 0; i < arrLen; i++) {
		if (array[i]->value > array[max]->value) {
			max = i;
		}

	}

	return max;
}


/**
 * Converts a string to lower case
 * @param char* string
 */
char* stringLower(char* str)
{
	char* current = str;

	while (*current != '\0') {
		*current = tolower(*current);
		current++;
	}

	return str;
}


/**
 * Checks the spelling of the word provded by the user. If the word is spelled incorrectly,
 * print the 5 closest words as determined by a metric like the Levenshtein distance.
 * Otherwise, indicate that the provded word is spelled correctly. Use dictionary.txt to
 * create the dictionary.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char** argv)
{
    // FIXME: implement
   HashMap* map = hashMapNew(1000);

   FILE* file = fopen("dictionary.txt", "r");
   clock_t timer = clock();
   loadDictionary(file, map);
   timer = clock() - timer;
   printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC); 
   fclose(file);
	
    char inputBuffer[256];
    int quit = 0;
    while (!quit)
    {
        printf("Enter a word or \"quit\" to quit: ");
        scanf("%s", inputBuffer);
	
		char* input = stringLower(inputBuffer);
		if (hashMapContainsKey(map, input))
			printf("Spelled correctly!\n\n");
	
		else {
			timer = clock();


			int count = 0, max;
			HashLink* suggestions[5];
			struct HashLink* current;

			printf("The inputted word %s, is spelled incorrectly.\n", inputBuffer);

			// Calculate Levenshtein differences for dictionary
			for (int i = 0; i < hashMapCapacity(map); i++) {
				current = map->table[i];
				while (current != NULL)
				{
					current->value = LDistance(input, current->key);
					current = current->next;
				}
			}

			for (int i = 0; i < hashMapCapacity(map); i++) {
				current = map->table[i];
				while (current != NULL)
				{
					//Fill the array with initial values
					if (count < 5) {
						suggestions[count] = current;
						count++;
					}
				
					//If there are any values less than the max in the array, replace it
					else {
						max = calculateMaxIdx(suggestions, 5);
						if (current->value < suggestions[max]->value)
						{
							suggestions[max] = current;
						}

					}
				
					current = current->next;
				}
			}


			printf("Did you mean: ");
			for (int i = 0; i < 5; i++)
			{
				printf("%s", suggestions[i]->key);
				if (i == 3)
					printf(" or ");
				else if (i == 4)
					printf("?\n\n");
				else
					printf(", ");
			}

		}
	

        if (strcmp(inputBuffer, "quit") == 0)
        {
            quit = 1;
        }
    }

    hashMapDelete(map);

    return 0;
}
