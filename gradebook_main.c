#include <stdio.h>
#include <string.h>

#include "gradebook.h"

#define MAX_CMD_LEN 128

int main(int argc, char **argv) {
    gradebook_t *book = NULL;

    if (argc > 1) {
        char *file_name = argv[1];
        int len = strlen(file_name);
        if (len >= 4 && strcmp(file_name + len - 4, ".txt") == 0) {
            gradebook_t *new_book = read_gradebook_from_text(file_name);
            if (new_book == NULL) {
                printf("Failed to read gradebook from text file\n");
            } 
            else {
                book = new_book;
                printf("Gradebook loaded from text file\n");
            }
        } 
        else if (len >= 4 && strcmp(file_name + len - 4, ".bin") == 0) {
            gradebook_t *new_book = read_gradebook_from_binary(file_name);
            if (new_book == NULL) {
                printf("Failed to read gradebook from binary file\n");
            } 
            else {
                book = new_book;
                printf("Gradebook loaded from binary file\n");
            }
        } 
        else {
            printf("Error: Unknown gradebook file extension\n");
        }
    }

    printf("CSCI 2021 Gradebook System\n");
    printf("Commands:\n");
    printf("  create <name>:          creates a new class with specified name\n");
    printf("  class:                  shows the name of the class\n");
    printf("  add <name> <score>:     adds a new score\n");
    printf("  lookup <name>:          searches for a score by student name\n");
    printf("  clear:                  resets current gradebook\n");
    printf("  print:                  shows all scores, sorted by student name\n");
    printf("  write_text:             saves all scores to text file\n");
    printf("  read_text <file_name>:  loads scores from text file\n");
    printf("  write_bin:              saves all scores to binary file\n");
    printf("  read_bin <file_name>:   loads scores from binary file\n");
    printf("  exit:                   exits the program\n");

    char cmd[MAX_CMD_LEN];
    while (1) {
        printf("gradebook> ");
        if (scanf("%s", cmd) == EOF) {
            printf("\n");
            break;
        }

        if (strcmp("exit", cmd) == 0) {
            break;
        }

        else if (strcmp("create", cmd) == 0) {
            scanf("%s", cmd); // Read in class name
            if (book != NULL) {
                printf("Error: You already have a gradebook.\n");
                printf("You can remove it with the \'clear\' command\n");
            } else {
                book = create_gradebook(cmd);
                if (book == NULL) {
                    printf("Gradebook creation failed\n");
                }
            }
        }

// CLASS NAME (COMPLETE)
        else if (strcmp("class", cmd)==0){
            if (book == NULL) {
                printf("Error: You must create or load a gradebook first\n");
            }
            else{
                printf("%s\n", get_gradebook_name(book));
            }
        }

        // ADD SCORE (COMPLETE)
        else if (strcmp("add", cmd) == 0){
            int score;
            scanf("%s %d", cmd, &score);
            if (book == NULL) {
                printf("Error: You must create or load a gradebook first\n");
            }
            else if(score<0){
                printf("Error: You must enter a score in the valid range (0 <= score)\n");
            }
            else{
                add_score(book, cmd, score);
            }
        }

        // LOOKUP STUDENT'S SCORE (COMPLETE)
        else if (strcmp("lookup", cmd) == 0){
            scanf("%s", cmd);
            if (book == NULL) {
                printf("Error: You must create or load a gradebook first\n");
            }
            else{
                int lookup = find_score(book, cmd);
                if (lookup == -1){
                    printf("No score for '%s' found\n", cmd);
                }
                else{
                    printf("%s: %d\n", cmd, lookup);
                }

            }
        }

        //CLEAR CURRENT GRADEBOOK (NOT COMPLETE!!!!!!)
        else if (strcmp("clear", cmd) == 0) {
            if (book == NULL) {
                printf("Error: No gradebook to clear\n");
            } 
            else {
                free_gradebook(book);
                book = NULL;
            }
        }


        // PRINTS ALL SCORES SORTED BY NAME OF STUDENTS (COMPLETE)
        else if (strcmp("print", cmd) == 0){
            if (book == NULL) {
                printf("Error: You must create or load a gradebook first\n");
            }
            else{
                print_gradebook(book);
            }
        }

        // WRITE SCORE TO TEXT FILE (NOT COMPLETE!!!!!!)
        else if (strcmp("write_text", cmd) == 0){
            if (book == NULL){
                printf("Error: You must create or load a gradebook first\n");
            }
            else{
                int i = write_gradebook_to_text(book);
                if (i == 0){
                    printf("Gradebook successfully written to %s.txt\n",book->class_name);
                }
                else{
                    printf("Failed to write gradebook to text file\n");
                }
            }
        }
        
        // READS SCORES FROM TEXT FILE (NOT COMPLETE!!!!!!)
        else if (strcmp("read_text", cmd) == 0){
            char file_name[MAX_NAME_LEN + 5];
            scanf("%s", file_name);
            if(book != NULL){
                printf("Error: You must clear current gradebook first\n");
            }
            else{
                gradebook_t *new_book = read_gradebook_from_text(file_name);
                if (new_book == NULL) {
                    printf("Failed to read gradebook from text file\n");
                }
                else{
                    book = new_book;
                    printf("Gradebook loaded from text file\n");
                }
            }
        }

        // WRITE SCORE TO BINARY FILE (NOT COMPLETE!!!!!!)
        else if (strcmp("write_bin", cmd) == 0) {
            if (book == NULL) {
                printf("Error: No gradebook to write\n");
            } else {
                int result = write_gradebook_to_binary(book);
                if (result == 0) {
                    printf("Gradebook successfully written to %s.bin\n", book->class_name);
                } else {
                    printf("Failed to write gradebook to binary file\n");
                }
            }
        }

        //READ SCORES FROM BINARY FILE (NOT COMPLETE!!!!!)
        else if (strcmp("read_bin", cmd) == 0) {
            scanf("%s", cmd);
            if (book != NULL) {
                printf("Error: You must clear current gradebook first\n");
            } else {
                gradebook_t *new_book = read_gradebook_from_binary(cmd);
                if (new_book == NULL) {
                    printf("Failed to read gradebook from binary file\n");
                } else {
                    book = new_book;
                    printf("Gradebook loaded from binary file\n");
                }
            }
        }

        else {
            printf("Unknown command %s\n", cmd);
        }
    }

    if (book != NULL) {
        free_gradebook(book);
    }
    return 0;
}
