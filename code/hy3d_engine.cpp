#include "hy3d_engine.h"
#include <intrin.h>

static void InitializeMemoryArena(memory_arena *arena, u8 *base, size_t size)
{
    arena->base = base;
    arena->size = size;
    arena->used = 0;
}

#define ReserveStructMemory(arena, type) (type *)ReserveMemory(arena, sizeof(type))
#define ReserveArrayMemory(arena, count, type) (type *)ReserveMemory(arena, (count) * sizeof(type))
static void *ReserveMemory(memory_arena *arena, size_t size)
{
    Assert(arena->used + size <= arena->size);
    void *result = arena->base + arena->used;
    arena->used += size;
    return result;
}

static void Initialize(hy3d_engine *e, engine_state *state, engine_memory *memory)
{
    e->input = {};
    e->onResize = false;
    InitializeMemoryArena(&state->memoryArena,
                          (u8 *)memory->permanentMemory + sizeof(engine_state),
                          memory->permanentMemorySize - sizeof(engine_state));

    // NOTE: Everything is initialized here
    state->color[0] = 0.1f;
    state->color[1] = 0.3f;
    state->color[2] = 0.6f;
    state->change[0] = 1.0f;
    state->change[1] = 1.5f;
    state->change[2] = 2.0f;

    //--------------------------------------

    memory->isInitialized = true;
    e->frameStart = std::chrono::steady_clock::now();
}

extern "C" UPDATE_AND_RENDER(UpdateAndRender)
{
    engine_state *state = (engine_state *)memory->permanentMemory;
    if (!memory->isInitialized)
        Initialize(&e, state, memory);

    std::chrono::steady_clock::time_point frameEnd = std::chrono::steady_clock::now();
    std::chrono::duration<f32> frameTime = frameEnd - e.frameStart;
    f32 dt = frameTime.count();
    e.frameStart = frameEnd;

    // NOTE: UPDATE
    float min = 0.2f;
    float max = 0.5f;
    for (int i = 0; i < ArrayCount(state->color); i++)
    {
        if (state->color[i] >= max)
        {
            state->change[i] *= -1.0f;
            state->color[i] = max;
        }
        if (state->color[i] <= min)
        {
            state->change[i] *= -1.0f;
            state->color[i] = min;
        }
        state->color[i] += state->change[i] * dt * 0.2;
    }

    //state->color[0] = 0.8;
    //state->color[1] = 0.55;
    //state->color[2] = 0.35;

    memory->platformAPI.Update(state->color);

    // NOTE: RENDER
    memory->platformAPI.Draw();

    //if we cant render
    //Sleep(100);
}