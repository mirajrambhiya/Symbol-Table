#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 101

typedef struct Entry {
    char *variable;
    int value;
    struct Entry *next;
} Entry;

typedef struct {
    Entry *entries[TABLE_SIZE];
} HashTable;

typedef struct ScopeNode {
    HashTable *table;
    struct ScopeNode *prev;
    struct ScopeNode *next;
} ScopeNode;

HashTable *createHashTable() {
    HashTable *table = malloc(sizeof(HashTable));
    for (int i = 0; i < TABLE_SIZE; i++) {
        table->entries[i] = NULL;
    }
    return table;
}

unsigned int hash(const char *key) {
    unsigned long int value = 0;
    unsigned int i = 0;
    unsigned int key_len = strlen(key);

    for (; i < key_len; ++i) {
        value = value * 37 + key[i];
    }
    value = value % TABLE_SIZE;

    return value;
}

void insert(HashTable *table, const char *variable, int value) {
    unsigned int slot = hash(variable);
    Entry *entry = table->entries[slot];
    while (entry != NULL) {
        if (strcmp(entry->variable, variable) == 0) {
            entry->value = value;
            return;
        }
        entry = entry->next;
    }

    Entry *newEntry = malloc(sizeof(Entry));
    newEntry->variable = strdup(variable); 
    newEntry->value = value;
    newEntry->next = table->entries[slot];
    table->entries[slot] = newEntry;
}

int lookup(HashTable *table, const char *variable, int *found) {
    unsigned int slot = hash(variable);
    Entry *entry = table->entries[slot];

    while (entry != NULL) {
        if (strcmp(entry->variable, variable) == 0) {
            *found = 1;
            return entry->value;
        }
        entry = entry->next;
    }

    *found = 0; 
    return 0; 
}

ScopeNode *initializeScope(ScopeNode *currentScope) {
    ScopeNode *newScope = malloc(sizeof(ScopeNode));
    newScope->table = createHashTable();
    newScope->prev = currentScope;
    newScope->next = NULL;

    if (currentScope != NULL) {
        currentScope->next = newScope;
    }

    return newScope;
}

ScopeNode *finalizeScope(ScopeNode *currentScope) {
    ScopeNode *prevScope = NULL;

    if (currentScope != NULL) {
        prevScope = currentScope->prev;

        for (int i = 0; i < TABLE_SIZE; ++i) {
            Entry *entry = currentScope->table->entries[i];
            while (entry != NULL) {
                Entry *temp = entry;
                entry = entry->next;
                free(temp->variable); 
                free(temp);
            }
        }
        free(currentScope->table);
        free(currentScope);

        if (prevScope != NULL) {
            prevScope->next = NULL;
        }
    }

    return prevScope;
}

void assign(ScopeNode *currentScope, const char *variable, int value) {
    if (currentScope != NULL && currentScope->table != NULL) {
        insert(currentScope->table, variable, value);
    }
}

void print(ScopeNode *currentScope, const char *variable) {
    int found = 0;
    int value = 0;
    ScopeNode *scope = currentScope;

    while (!found && scope != NULL) {
        value = lookup(scope->table, variable, &found);
        scope = scope->prev;
    }

    if (found) {
        printf("%s = %d\n", variable, value);
    } else {
        printf("%s is undefined\n", variable);
    }
}

int main() {
    char command[100];
    char var[50]; 
    int value; 
    ScopeNode *currentScope = NULL;

    FILE *file = fopen("input.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    while (fgets(command, sizeof(command), file)) {
        if (strncmp(command, "begin", 5) == 0) {
            currentScope = initializeScope(currentScope);
        }
        else if (strncmp(command, "end", 3) == 0) {
            currentScope = finalizeScope(currentScope);
        }
        else if (sscanf(command, "assign %s %d", var, &value) == 2) {
            assign(currentScope, var, value);
        }
        else if (sscanf(command, "print %s", var) == 1) {
            print(currentScope, var);
        }
    }

    fclose(file);
    while (currentScope != NULL) {
        currentScope = finalizeScope(currentScope);
    }

    return 0;
}