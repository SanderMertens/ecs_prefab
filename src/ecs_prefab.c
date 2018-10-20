#include <reflecs/reflecs.h>
#include <stdio.h>
#include <unistd.h>

typedef struct Player {
    uint32_t generated;
} Player;

typedef struct Generator {
    uint32_t output;
} Generator;

void InitPlayer(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Player *p = ecs_column(rows, row, 0);
        p->generated = 0;
    }
}

void Collect(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Player *p = ecs_column(rows, row, 0);
        Generator *g = ecs_column(rows, row, 1);
        p->generated += g->output;
    }
}

int main(int argc, char *argv[]) {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Player);
    ECS_COMPONENT(world, Generator);
    ECS_SYSTEM(world, InitPlayer, EcsOnInit, Player);
    ECS_SYSTEM(world, Collect, EcsPeriodic, COMPONENT.Player, Generator);
    ECS_PREFAB(world, GeneratorPrefab, Generator);

    /* Initialize prefab */
    Generator *g = ecs_get(world, GeneratorPrefab_h, Generator_h);
    g->output = 10;

    /* Create and initialize player */
    EcsHandle me = ecs_new(world, 0);
    ecs_add(world, me, Player_h);
    ecs_add(world, me, EcsComponent_h); /* Lets entity be used as component */
    ecs_commit(world, me);

    /* Create 10 generators with GeneratorPrefab for player 'me' */
    int i;
    for (i = 0; i < 10; i ++) {
        EcsHandle generator = ecs_new(world, 0);
        ecs_add(world, generator, GeneratorPrefab_h);
        ecs_add(world, generator, me);
        ecs_commit(world, generator);
    }

    ecs_progress(world);

    /* Print state of Player to console */
    Player *p = ecs_get(world, me, Player_h);
    printf("Generated: %u\n", p->generated);

    return ecs_fini(world);
}
