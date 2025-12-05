#include <stdio.h>      // Standard Input/Output library for file operations (fopen, fprintf, etc.)
#include <string.h>     // String manipulation library (strcmp, strcpy, strlen)
#include <ctype.h>      // Character handling library (isalpha, tolower)
#include <stdlib.h>     // Standard library for general functions

// Define constants for array sizes to prevent buffer overflows
#define MAX_WORDS 2000  // Maximum number of unique words the program can track
#define MAX_LEN 50      // Maximum length allowed for a single word

// Structure to represent a word and its frequency
typedef struct {
    char word[MAX_LEN];   // The string storing the actual word
    int count;            // Counter for how many times this word appears
} WordFreq;

// --- Function Prototypes ---
// Declaring functions before main so the compiler knows they exist
void cleanWord(char *word);
int loadToxicWords(char toxic[][MAX_LEN]);
int loadStopWords(char stop[][MAX_LEN]);
int isToxic(char *word, char toxic[][MAX_LEN], int tCount);
int isStop(char *word, char stop[][MAX_LEN], int sCount);
int findWord(WordFreq list[], int size, char *word);
void DescendingBubble(WordFreq list[], int size);
int Partition(WordFreq list[], int start, int end);
void DescendingQuick(WordFreq list[], int start, int end);

// Display and Analysis Functions
void Display_GenWordSta(int total, int size, int WordLenSum, int sentencecount, WordFreq list[]);
void Display_ToxicWordSta(int toxicCount, int total);
void Display_Top5FreWord(WordFreq list[], int size, WordFreq toxiclist[], int toxicsize);
void AnalyzeFile(char *filename, WordFreq list[], int *size, WordFreq toxiclist[], int *toxicsize, 
                 int *total, int *sentencecount, int *WordLenSum, int *toxicCount, 
                 char toxicDB[][MAX_LEN], int tCount, char stopDB[][MAX_LEN], int sCount);
void SaveOut(int total, int size, int WordLenSum, int sentencecount, WordFreq list[], 
             int toxicCount, WordFreq toxiclist[], int toxicsize);

// --- Helper Function Implementations ---

// Function: cleanWord
// Purpose: Normalizes a word by removing punctuation and converting it to lowercase.
// This ensures that "Hello!" and "hello" are treated as the same word.
void cleanWord(char *word) {
    int j = 0;
    for (int i = 0; word[i]; i++) {
        // Keep only alphabetic characters
        if (isalpha(word[i]))            
            word[j++] = tolower(word[i]); // Convert to lowercase
    }
    word[j] = '\0'; // Null-terminate the string to mark the end
}

// Function: loadToxicWords
// Purpose: Reads toxic words from 'toxicwords.txt' into a 2D array.
// Returns: The number of toxic words successfully loaded.
int loadToxicWords(char toxic[][MAX_LEN]) {
    FILE *fp = fopen("toxicwords.txt", "r");
    if (!fp) { 
        printf("Error: Cannot open toxicwords.txt. Please ensure the file exists.\n");
        return 0; 
    }
    int count = 0;
    // Read words one by one until the array is full or file ends
    while (fscanf(fp, "%s", toxic[count]) == 1 && count < 100) { 
        count++;
    }
    fclose(fp);
    return count;
}

// Function: loadStopWords
// Purpose: Reads common words (stopwords) from 'stopwords.txt' into a 2D array.
// These words (e.g., "the", "and") will be ignored during analysis.
int loadStopWords(char stop[][MAX_LEN]) {
    FILE *fp = fopen("stopwords.txt", "r"); 
    if (!fp) { 
        printf("Error: Cannot open stopwords.txt. Please ensure the file exists.\n"); 
        return 0;
    }
    int count = 0;
    while (fscanf(fp, "%s", stop[count]) == 1 && count < 100) {
        count++;
    }
    fclose(fp);
    return count;
}

// Function: isToxic
// Purpose: Checks if a given word exists in the toxic words dictionary.
// Returns: 1 if the word is toxic, 0 otherwise.
int isToxic(char *word, char toxic[][MAX_LEN], int tCount) {
    for (int i = 0; i < tCount; i++)
        if (strcmp(word, toxic[i]) == 0) return 1; // Match found
    return 0;
}

// Function: isStop
// Purpose: Checks if a given word exists in the stopwords dictionary.
// Returns: 1 if the word is a stopword, 0 otherwise.
int isStop(char *word, char stop[][MAX_LEN], int sCount) {
    for (int i = 0; i < sCount; i++)
        if (strcmp(word, stop[i]) == 0) return 1; // Match found
    return 0;
}

// Function: findWord
// Purpose: Searches for a word in the frequency list.
// Returns: The index of the word if found, or -1 if it is new.
int findWord(WordFreq list[], int size, char *word) {
    for (int i = 0; i < size; i++)
        if (strcmp(list[i].word, word) == 0) return i;
    return -1;
}

// --- Sorting Algorithms ---

// Function: DescendingBubble
// Purpose: Sorts the WordFreq array in descending order based on frequency (count).
// Algorithm: Bubble Sort (Simple comparison-based sort).
void DescendingBubble(WordFreq list[], int size) {
    for (int pass = 0; pass < size - 1; ++pass) {
        for (int i = 0; i < size - 1 - pass; ++i) {
            // Swap if the current element is smaller than the next one
            if (list[i].count < list[i+1].count) {
                WordFreq hold = list[i+1];
                list[i+1] = list[i];
                list[i] = hold;
            }
        }
    }
}

// Function: Partition
// Purpose: Helper function for Quick Sort. Organizes the array around a pivot.
int Partition(WordFreq list[], int start, int end) {
    int pivot = list[end].count; // Select the last element as pivot
    int i = start - 1;
    for (int j = start; j < end; j++) {
        if (list[j].count > pivot) { // Sort descending (High to Low)
            i++;
            WordFreq hold = list[j];
            list[j] = list[i];
            list[i] = hold;
        }
    }
    // Place pivot in the correct position
    WordFreq hold = list[i+1];
    list[i+1] = list[end];
    list[end] = hold;
    return i + 1;
}

// Function: DescendingQuick
// Purpose: Sorts the WordFreq array using the Quick Sort algorithm.
// Note: This is generally faster than Bubble Sort for large datasets.
void DescendingQuick(WordFreq list[], int start, int end) {
    if (start < end) {
        int pivot = Partition(list, start, end);
        // Recursively sort elements before and after the pivot
        DescendingQuick(list, start, pivot - 1);
        DescendingQuick(list, pivot + 1, end);
    }
}

// --- Core Analysis Logic ---

// Function: AnalyzeFile
// Purpose: The main processing engine. It reads a file, cleans words, checks against dictionaries,
// and updates the main data structures via pointers.
void AnalyzeFile(char *filename, WordFreq list[], int *size, WordFreq toxiclist[], int *toxicsize, 
                 int *total, int *sentencecount, int *WordLenSum, int *toxicCount, 
                 char toxicDB[][MAX_LEN], int tCount, char stopDB[][MAX_LEN], int sCount) {
    
    // Step 1: Open the input file
    FILE *incsv = fopen(filename, "r");
    if (!incsv) {
        printf("Error: Cannot open input file: %s\n", filename);
        return;
    }
    
    // Create a temporary file to normalize input (handles CSV to TXT conversion)
    FILE *tempFile = fopen("temp_processed.txt", "w");
    if (!tempFile) { fclose(incsv); return; }

    char sentence[500], type[50];
    
    // Check if the file is a CSV by looking at the extension
    int isCSV = (strstr(filename, ".csv") != NULL);

    if (isCSV) {
        // If CSV, extract the first column (assuming format: Comment,Type)
        while (fscanf(incsv, " %[^,],%s", sentence, type) == 2) {
             fprintf(tempFile, "%s. ", sentence); 
        }
    } else {
        // If TXT, copy content directly char by char
        char ch;
        while((ch = fgetc(incsv)) != EOF) fputc(ch, tempFile);
    }
    
    fclose(incsv);
    fclose(tempFile);

    // Step 2: Analyze the standardized temporary file
    FILE *fp = fopen("temp_processed.txt", "r");
    if (!fp) return;

    // Reset counters to ensure fresh analysis for the new file
    *total = 0; 
    *sentencecount = 0; 
    *WordLenSum = 0; 
    *size = 0; 
    *toxicCount = 0; 
    *toxicsize = 0;
    
    char rawWord[100];
    char cleanBuffer[100];

    // Read word by word
    while (fscanf(fp, "%99s", rawWord) == 1) {
        int len = strlen(rawWord);
        
        // Heuristic: Count sentences based on punctuation marks (. ? !)
        if (rawWord[len - 1] == '.' || rawWord[len - 1] == '?' || rawWord[len - 1] == '!') {
            (*sentencecount)++;
        }

        // Clean the word (remove punctuation, lower case)
        strcpy(cleanBuffer, rawWord);
        cleanWord(cleanBuffer);

        // Skip if the word became empty after cleaning
        if (strlen(cleanBuffer) == 0) continue;

        // Update general statistics
        (*total)++;
        (*WordLenSum) += strlen(cleanBuffer);

        // Check for stopwords (if found, skip to next word)
        if (isStop(cleanBuffer, stopDB, sCount)) continue; 

        // Update General Word Frequency List
        int pos = findWord(list, *size, cleanBuffer);
        if (pos == -1) {
            // New word: add to list
            strcpy(list[*size].word, cleanBuffer);
            list[*size].count = 1;
            (*size)++;
        } else {
            // Existing word: increment count
            list[pos].count++;
        }

        // Update Toxic Word Statistics
        if (isToxic(cleanBuffer, toxicDB, tCount)) {
            (*toxicCount)++;
            int tpos = findWord(toxiclist, *toxicsize, cleanBuffer);
            if (tpos == -1) {
                // New toxic word detected
                strcpy(toxiclist[*toxicsize].word, cleanBuffer);
                toxiclist[*toxicsize].count = 1;
                (*toxicsize)++;
            } else {
                toxiclist[tpos].count++;
            }
        }
    }
    fclose(fp);
    printf("\nSuccess: File processed! You can now view statistics.\n");
}

// --- Display & Save Functions ---

// Function: Display_GenWordSta
// Purpose: Prints general statistics like total word count, unique words, and lexical diversity.
void Display_GenWordSta(int total, int size, int WordLenSum, int sentencecount, WordFreq list[]) {
    if (total == 0) { printf("No data available. Please load a file first (Option 1).\n"); return; }
    
    printf("\n--- General Statistics ---\n");
    printf("Total words processed: %d\n", total);
    printf("Number of Unique Words: %d\n", size);
    
    // Lexical diversity = (Unique Words / Total Words) * 100
    if (total > 0) printf("Lexical diversity: %.2f%%\n", (size * 100.0) / total);
    
    // Average word length
    if (total > 0) printf("Average word length: %.2f characters\n", (double)WordLenSum / total);
    
    // Average sentence length
    if (sentencecount > 0) printf("Average sentence length: %.2f words\n", (double)total / sentencecount);
}

// Function: Display_ToxicWordSta
// Purpose: Prints statistics specifically related to toxic content detected.
void Display_ToxicWordSta(int toxicCount, int total) {
    if (total == 0) { printf("No data available.\n"); return; }
    printf("\n--- Toxic Word Analysis ---\n");
    printf("Total Toxic words found: %d\n", toxicCount);
    // Calculate what percentage of total words are toxic
    printf("Toxic content proportion: %.2f%%\n", (toxicCount * 100.0) / total);
}

// Function: Display_Top5FreWord
// Purpose: Sorts the lists and displays the top 5 most frequent general and toxic words.
void Display_Top5FreWord(WordFreq list[], int size, WordFreq toxiclist[], int toxicsize) {
    if (size == 0) { printf("No data available.\n"); return; }

    // Sort general words using Bubble Sort
    DescendingBubble(list, size);
    printf("\nTop 5 Most Frequent Words:\n");
    for (int j = 0; j < 5 && j < size; j++)
        printf("%d. %s: %d\n", j + 1, list[j].word, list[j].count);

    // Sort toxic words using Quick Sort (demonstrating multiple algorithms)
    DescendingQuick(toxiclist, 0, toxicsize - 1);
    printf("\nTop 5 Most Frequent Toxic Words:\n");
    for (int k = 0; k < 5 && k < toxicsize; k++)
        printf("%d. %s: %d\n", k + 1, toxiclist[k].word, toxiclist[k].count);
}

// Function: SaveOut
// Purpose: Writes the current analysis results to a file named 'analysis_report.txt'.
// This ensures data persistence.
void SaveOut(int total, int size, int WordLenSum, int sentencecount, WordFreq list[], 
             int toxicCount, WordFreq toxiclist[], int toxicsize) {
    
    if (total == 0) { printf("No data to save.\n"); return; }

    FILE *sfPtr = fopen("analysis_report.txt", "w");
    if (sfPtr == NULL) {
        printf("Error: File could not be opened for writing.\n");
        return;
    }

    // Write Summary Statistics
    fprintf(sfPtr, "1. The summary statistics:\n");
    fprintf(sfPtr, "Total words: %d\n", total);
    fprintf(sfPtr, "Unique Words: %d\n", size);
    if (total > 0) fprintf(sfPtr, "Lexical diversity: %.2f%%\n", (size * 100.0) / total);
    
    // Write Toxic Analysis
    fprintf(sfPtr, "\n2. Toxic word analysis:\n");
    fprintf(sfPtr, "Toxic words: %d\n", toxicCount);
    if (total > 0) fprintf(sfPtr, "Proportion: %.2f%%\n", (toxicCount * 100.0) / total);

    // Write Top Words (ensure they are sorted before saving)
    fprintf(sfPtr, "\n3. Top Words:\n");
    DescendingBubble(list, size);
    for (int j = 0; j < 5 && j < size; j++)
        fprintf(sfPtr, "Top Word %d: %s (%d)\n", j + 1, list[j].word, list[j].count);

    fprintf(sfPtr, "\nTop Toxic Words:\n");
    DescendingBubble(toxiclist, toxicsize);
    for (int k = 0; k < 5 && k < toxicsize; k++)
        fprintf(sfPtr, "Top Toxic Word %d: %s (%d)\n", k + 1, toxiclist[k].word, toxiclist[k].count);

    fclose(sfPtr);
    printf("Report successfully saved to 'analysis_report.txt'.\n");
}

// --- Main Function ---

int main() {
    int option = 0;
    char filename[100];
    
    // Load dictionaries into memory at startup
    char toxicDB[100][MAX_LEN];
    char stopDB[100][MAX_LEN];
    int tCount = loadToxicWords(toxicDB);
    int sCount = loadStopWords(stopDB);

    printf("Initialization: Loaded %d toxic words and %d stopwords.\n", tCount, sCount);

    // Initialize core data structures
    // These variables hold the state of the analysis throughout the program execution
    WordFreq list[MAX_WORDS];
    WordFreq toxiclist[MAX_WORDS];
    int size = 0, toxicsize = 0;
    int total = 0, sentencecount = 0, WordLenSum = 0, toxicCount = 0;

    int dataLoaded = 0; // Flag to track if file data has been processed

    // Main Menu Loop
    while (1) {
        printf("\n========================================\n");
        printf("   Cyberbullying Text Analyzer Menu\n");
        printf("========================================\n");
        printf("1. Load text file for analysis\n");
        printf("2. Display general word statistics\n");
        printf("3. Display toxic word analysis\n");
        printf("4. Sort and display top 5 words\n");
        printf("5. Save results to output file\n");
        printf("6. Exit program\n");
        printf("Enter option: ");
        
        // Validate input (check if user entered a number)
        if (scanf("%d", &option) != 1) { 
            while(getchar() != '\n'); // Clear input buffer
            printf("Invalid input. Please enter a number.\n");
            continue;
        }

        // Exit condition
        if (option == 6) { 
            printf("Exiting program. Goodbye!\n");
            break; 
        }

        // Enforce file loading before analysis
        if (option != 1 && !dataLoaded) {
            printf("\n[WARNING] Please load a file first (Option 1).\n");
            continue;
        }

        // Execute user command
        switch (option) {
            case 1:
                printf("Enter filename (e.g., comments.csv or test.txt): ");
                scanf("%s", filename);
                // Pass pointers to variables so AnalyzeFile can update them
                AnalyzeFile(filename, list, &size, toxiclist, &toxicsize, 
                            &total, &sentencecount, &WordLenSum, &toxicCount, 
                            toxicDB, tCount, stopDB, sCount);
                if (total > 0) dataLoaded = 1;
                break;
            case 2:
                Display_GenWordSta(total, size, WordLenSum, sentencecount, list);
                break;
            case 3:
                Display_ToxicWordSta(toxicCount, total);
                break;
            case 4:
                Display_Top5FreWord(list, size, toxiclist, toxicsize);
                break;
            case 5:
                SaveOut(total, size, WordLenSum, sentencecount, list, toxicCount, toxiclist, toxicsize);
                break;
            default:
                printf("Incorrect Input. Please try again!\n");
        }
    }
    return 0;
}