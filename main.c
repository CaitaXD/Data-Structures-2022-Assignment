#include "IO.h"
#include "avl.h"
#include "bench.h"
#include "bst.h"
#include "collections.h"
#include "rbt.h"
#include <stdio.h>
#include <stdlib.h>

int CMPS;
int ROTATIONS;

int main(int argc, char const *argv[])
{
    if (argc < 4)
    {
        printf("Usage: <calory table> <food consumed> <output> <log> <times to run: default 1>");
    }
    int N = max(atoi(argv[5]), 1);

    struct context ctx = ctx_init_default(argv);
    *ctx.output_stream = ctx.log_file.filestream;
    TEST_BATERY(ctx, N, *ctx.output_stream);

    Collection *xs = rbt_new();
    deserialize_csv(&xs, ctx.calory_file.filestream);

    *ctx.output_stream = ctx.output_file.filestream;
    write_output_header(xs, ctx);
    write_output_food_stats(xs, ctx);

    fclose(ctx.output_file.filestream);
    fclose(ctx.food_file.filestream);
    fclose(ctx.calory_file.filestream);
    fclose(ctx.log_file.filestream);
    delete (xs);
    return EXIT_SUCCESS;
}
void write_output_food_stats(Collection *const xs, struct context ctx)
{
    fseek(ctx.food_file.filestream, 0, SEEK_SET);
    double sum = 0;
    for (int i = 0; fgets(BUFFER, KB, ctx.food_file.filestream); i++)
    {
        char *food         = strtok(BUFFER, ";");
        int   calory_eaten = atoi(strtok(NULL, ";"));
        if (food[0] != '\n')
        {
            Collection *node            = find(xs, &(kvp){.key = food}, (comparer)keycmp);
            kvp        *table_result    = (kvp *)node->data;
            float       calory_per_100g = ((float)calory_eaten * table_result->value) / 100;
            sum += calory_per_100g;
            fprintf(
                *ctx.output_stream,
                "%dg de %s (%d a cada 100g) = %.2f\n",
                calory_eaten,
                food,
                calory_eaten,
                calory_per_100g
            );
        }
    }
    fprintf(*ctx.output_stream, "\nTotal: %.2f\n", sum);
}
void write_output_header(Collection *const xs, struct context ctx)
{
    string foodfilename   = ctx.food_file.filename;
    string caloryfilename = ctx.calory_file.filename;
    fprintf(*ctx.output_stream, "Calorias Calculadas para %s segundo a tabela %s.\n\n", foodfilename, caloryfilename);
}
