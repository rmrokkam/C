#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gradebook.h"

// This is the (somewhat famous) djb2 hash
unsigned hash(const char *str) {
    unsigned hash_val = 5381;
    int i = 0;
    while (str[i] != '\0') {
        hash_val = ((hash_val << 5) + hash_val) + str[i];
        i++;
    }
    return hash_val % NUM_BUCKETS;
}

gradebook_t *create_gradebook(const char *class_name) {
    gradebook_t *new_book = malloc(sizeof(gradebook_t));
    if (new_book == NULL) {
        return NULL;
    }

    strcpy(new_book->class_name, class_name);
    for (int i = 0; i < NUM_BUCKETS; i++) {
        new_book->buckets[i] = NULL;
    }
    new_book->size = 0;

    return new_book;
}

const char *get_gradebook_name(const gradebook_t *book) {
    return book->class_name;
}


int add_score(gradebook_t *book, const char *name, int score) {
    // Create a new node
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    if (new_node == NULL) {
        return -1; // Memory allocation failed
    }
    // Initialize the new node
    strncpy(new_node->name, name, MAX_NAME_LEN);
    new_node->name[MAX_NAME_LEN - 1] = '\0';
    new_node->score = score;
    new_node->next = NULL;
    unsigned hash_value = hash(name);
    // Insert the new node at the head of the linked list
    if (book->buckets[hash_value] == NULL) {
        book->buckets[hash_value] = new_node;
    } else {
        node_t *current = book->buckets[hash_value];
        while (current->next != NULL) {
            current = current->next; // Go to the end of the list
        }
        current->next = new_node; // Insert the new node at the end of the list
    }
    book->size++;
    return 0; // Score successfully added
}


int find_score(const gradebook_t *book, const char *name) {
    unsigned hash_value = hash(name);
    node_t *new_node = book->buckets[hash_value]; // Get the bucket indexed at the hash value
    while (new_node != NULL){ 
        if (strcmp(new_node->name, name) == 0){
            return new_node->score; // Found the student, return their score
        }
        new_node = new_node->next; // Move to the next node in the bucket
    }
    return -1; // Student not found in the gradebook
}


void print_gradebook(const gradebook_t *book) {
    printf("Scores for all students in %s:\n", book->class_name); // Print the class name
    for (int i = 0; i < NUM_BUCKETS; i++){
        node_t *new_node = book->buckets[i]; // Get the first node in the current bucket
        while(new_node != NULL){
            printf("%s: %d\n", new_node->name, new_node->score); // Print the student name and score
            new_node = new_node->next; // Move to the next node in the bucket
        }
    }
}


void free_gradebook(gradebook_t *book) {
    for (int i = 0; i < NUM_BUCKETS; i++) {
        node_t *new_node = book->buckets[i];
        // Free the nodes within the gradebook
        while (new_node != NULL) {
            node_t *temp_node = new_node;
            new_node = new_node->next;
            free(temp_node);
        }
        book->buckets[i] = NULL;
    }
    // Free the gradebook
    free(book);
}


int write_gradebook_to_text(const gradebook_t *book) {
    char file_name[MAX_NAME_LEN + strlen(".txt")];
    strcpy(file_name, book->class_name);
    strcat(file_name, ".txt");
    FILE *f = fopen(file_name, "w");
    if (f == NULL) {
        return -1;
    }

    fprintf(f, "%u\n", book->size);
    for (int i = 0; i < NUM_BUCKETS; i++) {
        node_t *current = book->buckets[i];
        while (current != NULL) {
            fprintf(f, "%s %d\n", current->name, current->score);
            current = current->next;
        }
    }
    fclose(f);
    return 0;
}


gradebook_t *read_gradebook_from_text(const char *file_name) {
    FILE *file = fopen(file_name, "r");
    if (file == NULL) {
        return NULL; // File opening failed
    }
    // Read the number of entries in the gradebook
    int num_entries;
    if (fscanf(file, "%u\n", &num_entries) != 1) {
        fclose(file);
        return NULL; // Failed to read the number of entries
    }
    // Extract the book name from the file name
    size_t file_name_length = strlen(file_name);
    size_t book_name_length = file_name_length - 4;
    char book_name[book_name_length + 1];
    size_t i = 0;
    while (i < book_name_length){
        book_name[i] = file_name[i];
        i++;
    }
    book_name[book_name_length] = '\0';
    // Create a new gradebook
    gradebook_t *new_book = create_gradebook(book_name);
    if (new_book == NULL) {
        fclose(file);
        return NULL; // Failed to create the gradebook
    }
    // Read and add scores to the gradebook
    for (unsigned i = 0; i < num_entries; i++) {
        char name[MAX_NAME_LEN];
        int score;
        // Read the student name and score from the file
        fscanf(file, "%s %d\n", name, &score);
        // Add the score to the gradebook
        int result = add_score(new_book, name, score);
        if (result != 0) {
            fclose(file);
            free_gradebook(new_book); // Free the newly created gradebook
            return NULL; // Failed to add the score to the gradebook
        }
    }
    fclose(file);
    return new_book;
}


int write_gradebook_to_binary(const gradebook_t *book) {
    // Create the file name with the .bin extension
    char file_name[MAX_NAME_LEN + strlen(".bin")];
    strcpy(file_name, book->class_name);
    strcat(file_name, ".bin");
    FILE *file = fopen(file_name, "wb");
    if (file == NULL) {
        return -1; // Failed to open the file
    }
    // Write the size of the gradebook
    unsigned num_entries = book->size;
    fwrite(&num_entries, sizeof(unsigned), 1, file);
    // Write each score entry to the binary file
    for (unsigned i = 0; i < NUM_BUCKETS; i++) {
        node_t *current = book->buckets[i];
        while (current != NULL) {
            // Write the length of the student's name
            unsigned name_len = strlen(current->name);
            fwrite(&name_len, sizeof(unsigned), 1, file);
            // Write the student's name
            fwrite(current->name, sizeof(char), name_len, file);
            // Write the student's score
            fwrite(&(current->score), sizeof(int), 1, file);
            current = current->next;
        }
    }
    fclose(file);
    return 0; // Gradebook successfully written to binary file
}


gradebook_t *read_gradebook_from_binary(const char *file_name) {
    FILE *file = fopen(file_name, "rb");
    if (file == NULL) {
        return NULL; // Failed to open the file
    }
    // Read the number of entries in the gradebook
    unsigned num_entries;
    if (fread(&num_entries, sizeof(unsigned), 1, file) != 1) {
        fclose(file);
        return NULL; // Failed to read the number of entries
    }
    // Get the class name from the file name
    size_t file_name_length = strlen(file_name);
    size_t book_name_length = file_name_length - 4;
    char book_name[book_name_length + 1];
    strncpy(book_name, file_name, book_name_length);
    book_name[book_name_length] = '\0';
    // Create a new gradebook
    gradebook_t *new_book = create_gradebook(book_name);
    if (new_book == NULL) {
        fclose(file);
        return NULL; // Failed to create a new gradebook
    }
    // Read and add scores to the gradebook
    for (unsigned i = 0; i < num_entries; i++) {
        unsigned name_length;
        // Read the name lengths, names, and scores in the gradebook
        if (fread(&name_length, sizeof(unsigned), 1, file) != 1) {
            fclose(file);
            free_gradebook(new_book);
            return NULL; // Failed to read the name length
        }
        char name[MAX_NAME_LEN];
        if (fread(name, sizeof(char), name_length, file) != name_length) {
            fclose(file);
            free_gradebook(new_book);
            return NULL; // Failed to read the name
        }
        name[name_length] = '\0';
        int score;
        if (fread(&score, sizeof(int), 1, file) != 1) {
            fclose(file);
            free_gradebook(new_book);
            return NULL; // Failed to read the score
        }
        // Add the score to the gradebook
        int result = add_score(new_book, name, score);
        if (result != 0) {
            fclose(file);
            free_gradebook(new_book);
            return NULL; // Failed to add a score to the gradebook
        }
    }
    fclose(file);
    return new_book; // Gradebook successfully loaded from binary file
}
