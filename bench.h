/**
 * @file bench.h
 * @brief Benchmarking functions for the program
 * Calculates the time it takes to run a function
 * and the number of comparisons and rotations
 */
#ifndef __BECH_H__
#define __BECH_H__
#include "IO.h"
#include "avl.h"
#include "bst.h"
#include "collections.h"
#include "defs.h"
#include "rbt.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <time.h>

extern int CMPS;
extern int ROTATIONS;
extern int INSERTIONS;
// General purpose buffer to pass strings around
char       BUFFER[KB];

void PRINT_INFO(string _BENCH_NAME, double _KCALS, int _CMPS, int _H, int _ROTS, int _NODES, double _TIME, FILE *out);
void SEARCHES_TEST(Collection *const xs, FILE *const stream_reader, double *time, double *kCals, FILE *out);
void INSERT_TEST(Collection **const xs, FILE *const stream_reader, double *time, FILE *out);
void INSERT_SEARCH_BENCH(
    string             _BENCH_NAME,
    Collection **const xs,
    FILE *const        calory_file,
    FILE *const        food_file,
    FILE              *out
);
void    INSERT_N_TEST(string _BENCH_NAME, Collection **const xs, FILE *const stream_reader, int n, FILE *out);
void    SEARCHES_N_TEST(string _BENCH_NAME, Collection *const xs, FILE *const stream_reader, int n, FILE *out);
void    REINSERT(FILE *const stream_reader, int n);
void    TEST_BATERY(struct context ctx, int N, FILE *out);
clock_t _start, _end;
// Macro to calculate the time it takes to run a function
#define CLOCK_TIME(COUNTER, CODE)                                                                                      \
    _start = clock();                                                                                                  \
    CODE;                                                                                                              \
    _end = clock();                                                                                                    \
    COUNTER += (double)(_end - _start) / CLOCKS_PER_SEC;

#define pad 5
void PRINT_INFO(string _BENCH_NAME, double _KCALS, int _CMPS, int _H, int _ROTS, int _NODES, double _TIME, FILE *out)
{
    fprintf(out, "||================ %*s ================\n", pad, _BENCH_NAME);
    fprintf(out, "|| KiloCalories:                  %.2lf\n", _KCALS);
    fprintf(out, "|| Comparisons:                     %d\n", _CMPS);
    fprintf(out, "|| Height:                          %d\n", _H);
    fprintf(out, "|| Rotations:                       %d\n", _ROTS);
    fprintf(out, "|| Nodes:                           %d\n", _NODES);
    fprintf(out, "|| Time:                            %.2lf\n", _TIME);
    fprintf(out, "||================ %*s ================\n", pad, _BENCH_NAME);
}
/**
 * @brief Does a full run of the benchmarking prints the results to a file and then does N more runs of insertions and searches
 */
void TEST_BATERY(struct context ctx, int N, FILE *out)
{
    Collection *bst1 = bst_new();
    Collection *rbt1 = rbt_new();
    Collection *avl1 = avl_new();
    fprintf(out, "||============== ONE RUN BENCHMARK ========================||\n");
    INSERT_SEARCH_BENCH("Binary Search Tree", &bst1, ctx.calory_file.filestream, ctx.food_file.filestream, out);
    INSERT_SEARCH_BENCH("Red Black Tree", &rbt1, ctx.calory_file.filestream, ctx.food_file.filestream, out);
    INSERT_SEARCH_BENCH("AVL Tree", &avl1, ctx.calory_file.filestream, ctx.food_file.filestream, out);
    fprintf(out, "||============== ONE RUN BENCHMARK ========================||\n");
    fprintf(out, "\n");

    //FILE *bstShape = fopen("BST Shape.txt", "w");
    //FILE *rbtShape = fopen("RBT Shape.txt", "w");
    //FILE *avlShape = fopen("AVL Shape.txt", "w");

    // This bad boy crashes the program
    //pretty_print((BST *)bst1, (cstringfier)pair_tocstring, bstShape);
    //pretty_print((BST *)rbt1, (cstringfier)pair_tocstring, rbtShape);
    //pretty_print((BST *)avl1, (cstringfier)pair_tocstring, avlShape);

    Collection *bst = bst_new();
    Collection *rbt = rbt_new();
    Collection *avl = avl_new();

    fseek(ctx.calory_file.filestream, 0, SEEK_SET);
    fseek(ctx.food_file.filestream, 0, SEEK_SET);

    fprintf(out, "||============== INSERTION ONLY BENCHMARK ==============||\n");
    INSERT_N_TEST("Red Black Tree", &rbt, ctx.calory_file.filestream, N, out);
    INSERT_N_TEST("AVL Tree", &avl, ctx.calory_file.filestream, N, out);
    INSERT_N_TEST("Binary Search Tree", &bst, ctx.calory_file.filestream, N, out);
    fprintf(out, "||============== INSERTION ONLY BENCHMARK ==============||\n");
    fprintf(out, "\n");
    fprintf(out, "||============== SEARCH ONLY BENCHMARK =================||\n");
    SEARCHES_N_TEST("Red Black Tree", rbt, ctx.food_file.filestream, N, out);
    SEARCHES_N_TEST("AVL Tree", avl, ctx.food_file.filestream, N, out);
    SEARCHES_N_TEST("Binary Search Tree", bst, ctx.food_file.filestream, N, out);
    fprintf(out, "||============== SEARCH ONLY BENCHMARK =================||\n");

/**
 * Reource cleanup was causing the program to crash
 */
    // inorder((BST *)bst1, (action)kvp_free);
    // inorder((BST *)rbt1, (action)kvp_free);
    // inorder((BST *)avl1, (action)kvp_free);
    // delete (bst1);
    // delete (rbt1);
    // delete (avl1);
    // inorder((BST *)bst, (action)kvp_free);
    // inorder((BST *)rbt, (action)kvp_free);
    // inorder((BST *)avl, (action)kvp_free);
    // delete (bst);
    // delete (rbt);
    // delete (avl);
    // fclose(bstShape);
    //fclose(rbtShape);
    //fclose(avlShape);
}

void SEARCHES_TEST(Collection *const xs, FILE *const stream_reader, double *time, double *kCals, FILE *out)
{
    FILE_RESET(stream_reader);
    CLOCK_TIME(*time, *kCals = kcal_sum(stream_reader, xs));
}
void INSERT_TEST(Collection **const xs, FILE *const stream_reader, double *time, FILE *out)
{
    FILE_RESET(stream_reader);
    CLOCK_TIME(*time, deserialize_csv(xs, stream_reader));
}
void INSERT_SEARCH_BENCH(
    string             _BENCH_NAME,
    Collection **const xs,
    FILE *const        calory_file,
    FILE *const        food_file,
    FILE              *out
)
{
    CMPS         = 0;
    ROTATIONS    = 0;
    double _time = 0, _kCals = 0;
    INSERT_TEST(xs, calory_file, &_time, out);
    SEARCHES_TEST(*xs, food_file, &_time, &_kCals, out);

    int _H     = height(*(BST * *const)xs);
    int _Nodes = count(*xs);

    PRINT_INFO(_BENCH_NAME, _kCals, CMPS, _H, ROTATIONS, _Nodes, _time, out);
}
void INSERT_N_TEST(string _BENCH_NAME, Collection **const xs, FILE *const stream_reader, int n, FILE *out)
{
    ROTATIONS    = 0;
    double _time = 0;
    for (int i = 0; i < n; i++)
    {
        INSERT_TEST(xs, stream_reader, &_time, out);
    }
    fprintf(out, "||================ %*s ================\n", pad, _BENCH_NAME);
    fprintf(out, "|| Nodes:        %d\n", count(*xs));
    fprintf(out, "|| Height:       %d\n", height(*(BST *const *)xs));
    fprintf(out, "|| Rotations:    %d\n", ROTATIONS);
    fprintf(out, "|| Time taken:   %lf\n", _time);
    fprintf(out, "||================ %*s ================\n", pad, _BENCH_NAME);
}
void SEARCHES_N_TEST(string _BENCH_NAME, Collection *const xs, FILE *const stream_reader, int n, FILE *out)
{
    ROTATIONS = 0;
    CMPS      = 0;

    double time = 0;
    for (int i = 0; i < n; i++)
    {
        SEARCHES_TEST(xs, stream_reader, &time, (double *)BUFFER, out);
    }

    fprintf(out, "||================ %*s ================\n", pad, _BENCH_NAME);
    fprintf(out, "|| Comparisons: %d\n", CMPS);
    fprintf(out, "|| Time taken:  %.2lf\n", time);
    fprintf(out, "||================ %*s ================\n", pad, _BENCH_NAME);
}

#endif // __BECH_H__