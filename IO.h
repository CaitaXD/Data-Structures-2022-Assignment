/**
 * @file IO.h
 * IO functions for the program
 * Handles serialization and deserialization of csv
 * and bookkeeping file handles
 */
#ifndef DATA_PAIR_H
#define DATA_PAIR_H
#include "defs.h"
#include <stdio.h>
#include <stdlib.h>
#include "collections.h"

#define EMPTY (void *)INT_MIN
char BUFFER[KB];

// Key value pair of string and a int
typedef struct str_int kvp;
struct str_int
{
    char *key;
    int   value;
};
// struct conaining file information and it's handle
struct fileInfo
{
    string filepath;
    string filename;
    string extension;
    FILE *filestream;
};
// struct containing the current state of the program
// information about the file being written to and read from
// and the pointer to the current output stream
// if the output stream is set to stdout it will be written to the terminal
struct context
{
    struct fileInfo food_file;   
    struct fileInfo calory_file;
    struct fileInfo output_file;
    struct fileInfo log_file;
    FILE          **output_stream;
};
// writes the title of the output file
void            write_output_header(Collection *const xs, struct context ctx);
// Write the amount of calories consumed what as consumed and whhat table was consulted
void            write_output_food_stats(Collection *const xs, struct context ctx);
// Opens a file and returns a struct containing the file information
struct fileInfo fi_open(string filepath, string mode);
// Initializes a context struct
struct context  ctx_init(string foodfilepath, string caloryfilepath, string outputfilepath, string logfilepath);
// Initializes the context struct with command line arguments
struct context  ctx_init_default(char const**arg) { return ctx_init(arg[1], arg[2], arg[3], arg[4]); }
// Consults the calory table and sums the amount of calories consumed
double          kcal_sum(FILE *const food_file, Collection *calory_table);
// Deserializes a csv file into a Collection
void            deserialize_csv(Collection **const xs, FILE *const data_file);
// Allocates memory for a kvp and returns a pointer to it
kvp *kvp_new(char *key, int value)
{
    kvp *new_kvp   = (kvp *)malloc(sizeof(kvp));
    new_kvp->key   = key;
    new_kvp->value = value;
    return new_kvp;
}
// Deserializes a csv line into a kvp
kvp *kvp_parse_new(char *line, string delimiter)
{
    char *aux = strtok(line, delimiter);
    if (!aux) return NULL;

    char *key   = strcpy((char *)calloc(1, strlen(aux)), aux);
    int   value = atoi(strtok(NULL, delimiter));
    return kvp_new(key, value);
}
// Frees the memory allocated for a kvp
void kvp_free(kvp *kvp)
{
    if (kvp)
    {
        free(kvp);
        if (kvp->key)
        {
            free(kvp->key);
            kvp->key = NULL;
        }
        kvp = EMPTY;
    }
}
// Compares two kvp's by their key with strcmpi wittch is case insensitive
int keycmp(kvp *a, kvp *b)
{
    if (!b) return 1;
    if (!a) return -1;
    if (!a && !b) return 0;
    return strcmpi(a->key, b->key);
}
// Takes a pointer to a kvp and returns a string representation of it
char *pair_tocstring(kvp *a)
{
    if (a == NULL) return NULL;

    char *str = (char *)malloc(sizeof(char) * 100);
    sprintf(str, "%s: %d", a->key, a->value);
    return str;
}
// Takes 2 pointers to kvp's adds their values and returns a new kvp Keyed as Sum
kvp *v_add(kvp *a, kvp *b)
{
    kvp c = {"Sum", a->value + b->value};
    *a    = c;
    return a;
}
// Takes a pointer to a kvp and prints it to the terminal
void print_kvp(kvp *data) { printf("%s %d\n", data->key, data->value); }
// keycmp casted to a function pointer
comparer kcmp = (comparer)keycmp;

void deserialize_csv(Collection **const xs, FILE *const stream_reader)
{
    fseek(stream_reader, 0, SEEK_SET);
    GUARD_AGAINST_NULL(xs, "Collection must not be null");
    GUARD_AGAINST_NULL(stream_reader, "Stream reader must not be null");

    for (// clang-format off
        kvp *pair = NULL; 
        fgets(BUFFER, KB, stream_reader); 
        add(xs, pair, kcmp)
        //*xs = insert(*xs, pair, kcmp)
    ) pair = kvp_parse_new(BUFFER, ";");
    // clang-format on
}
double kcal_sum(FILE *const stream_reader, Collection *src)
{
    double sum = 0;
    for (int i = 0; fgets(BUFFER, KB, stream_reader); i++)
    {
        char       *tmp = strtok(BUFFER, ";"), *food = calloc(sizeof(char), strlen(tmp));
        int         calory = atoi(strtok(NULL, ";"));
        kvp         pair   = {.key = strcpy(food, tmp), .value = calory};
        Collection *node   = find(src, &pair, kcmp);
        if (node)
        {
            kvp *calories = node->data;
            sum += pair.value * calories->value;
        }
        else if (pair.key[0] != '\n')
        {
            printf("Food not found: %s %d \n", pair.key, pair.value);
        }
    }
    return sum / 100;
}
struct context ctx_init(string caloryfilepath, string foodfilepath, string outputfilepath, string  loffilepath)
{
    FILE **out         = malloc(sizeof(intptr_t));
    *out               = stdout;
    struct context ctx = {
        .calory_file   = fi_open(caloryfilepath, "r"),
        .food_file     = fi_open(foodfilepath, "r"),
        .output_file   = fi_open(outputfilepath, "w"),
        .log_file      = fi_open(loffilepath, "w"),
        .output_stream = out,
    };
    GUARD_AGAINST_NULL(ctx.calory_file.filestream, "Could not open calory file");
    GUARD_AGAINST_NULL(ctx.food_file.filestream, "Could not open food file");
    GUARD_AGAINST_NULL(ctx.output_file.filestream, "Could not open output file");
    GUARD_AGAINST_NULL(ctx.log_file.filestream, "Could not open lof file");
    return ctx;
}
struct fileInfo fi_open(string filepath, string mode)
{
    struct fileInfo info = {
        .filepath   = filepath,
        .filename   = strrchr(filepath, '\\') + 1,
        .extension  = strrchr(filepath, '.') + 1,
        .filestream = fopen(filepath, mode),
    };
    return info;
}

#endif // DATA_PAIR_H