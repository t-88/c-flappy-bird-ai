#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <SDL.h>

#include "neuralNetworkRender.h"

#define WIDTH 600
#define HEIGHT 800
#define SPEED_UP 1

#define PIPE_GAP 250
#define PIPE_DISTANCE 300
#define PIPE_OFFSET_X WIDTH - 100
#define PIPE_SPEED 300 / SPEED_UP
#define PIPE_GAP_MARGIN 50

#define POP_COUNT 10
#define AGENT_START_X 50

#define DNA_INPUTS 4
#define DNA_HIDDEN 6
#define DNA_OUTPUTS 1
#define DNA_MUTATION_CHANCE 10
#define DNA_LR 0.1

#define GRAVITY 35 * SPEED_UP
#define AGENT_JUMP_FORCE 600 * SPEED_UP
#define AGENT_MAX_VELY 20 * SPEED_UP
#define AGENT_JUMP_DELAY 0.2 * SPEED_UP

#define FPS 60
#define TARGET_FPS 1
float dt;
int closest_pipe_index;
int global_score;
int holdingJump = 0;

typedef struct AABB
{
    float x, y, w, h;
} AABB;

typedef struct Pipe
{
    AABB aabb;
    float height;
    int tookScore;
} Pipe;

typedef struct DNA
{
    float w1[DNA_HIDDEN][DNA_INPUTS];
    float w2[DNA_OUTPUTS][DNA_HIDDEN];
    float b1[DNA_HIDDEN];
    float b2[DNA_OUTPUTS];
} DNA;

typedef struct Agent
{
    DNA dna;
    AABB aabb;
    float velY;
    int dead;
    float fitness;

    float jump_delay, jump_timer;
} Agent;

float randomFloat(float a)
{
    return (((float)rand() / RAND_MAX) - 0.5) * 2 * a;
}
int isColliding(AABB aabb1, AABB aabb2)
{
    return aabb1.x + aabb1.w >= aabb2.x && aabb2.x + aabb2.w >= aabb1.x && aabb1.y + aabb1.h >= aabb2.y && aabb2.y + aabb2.h >= aabb1.y;
}

Agent Agent_new();
void Agent_Update(Agent *agent, float dt, Pipe closest_pipe);
int Agent_FeedForward(Agent agent, Pipe pipe);
void Agent_Render(SDL_Renderer *renderer, Agent agent);

void Pipes_Init(Pipe *pipes, int size);
void Pipes_Update(Pipe *pipes, int size);
void Pipes_Render(SDL_Renderer *renderer, Pipe *pipes, int size);

void Pipes_Init(Pipe *pipes, int size)
{
    for (size_t i = 0; i < size; i++)
    {
        pipes[i].height = random() % (HEIGHT - PIPE_GAP_MARGIN - PIPE_GAP) + PIPE_GAP_MARGIN;
        pipes[i].aabb = (AABB){PIPE_OFFSET_X + (PIPE_DISTANCE + 60) * i, 0, 60, pipes[i].height};
        pipes[i].tookScore = 0;
        global_score = 0;
    }
    closest_pipe_index = 0;
}
void Pipes_Update(Pipe *pipes, int size)
{
    for (size_t i = 0; i < size; i++)
    {
        pipes[i].aabb.x -= PIPE_SPEED * dt * TARGET_FPS;
        if (pipes[i].aabb.x + pipes[i].aabb.w < 0)
        {
            pipes[i].aabb.x = PIPE_OFFSET_X + (PIPE_DISTANCE)*3 + pipes[i].aabb.w;
            if (closest_pipe_index == i)
            {
                closest_pipe_index = (closest_pipe_index + 1) % size;

                global_score++;
            }
        }
    }
}
void Pipes_Render(SDL_Renderer *renderer, Pipe *pipes, int size)
{
    SDL_Rect *rects;
    rects = calloc(size * 2, sizeof(SDL_Rect));

    for (size_t i = 0; i < size; i++)
    {
        rects[i] = (SDL_Rect){pipes[i].aabb.x, pipes[i].aabb.y, pipes[i].aabb.w, pipes[i].aabb.h};
        rects[4 + i] = (SDL_Rect){
            pipes[i].aabb.x,
            pipes[i].aabb.y + pipes[i].height + PIPE_GAP,
            pipes[i].aabb.w,
            HEIGHT - (pipes[i].aabb.h + PIPE_GAP)};
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRects(renderer, rects, size * 2);

    free(rects);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
}

DNA DNA_RandomDna()
{
    DNA dna;

    for (size_t hidden = 0; hidden < DNA_HIDDEN; hidden++)
        for (size_t input = 0; input < DNA_INPUTS; input++)
            dna.w1[hidden][input] = randomFloat(5);
    for (size_t output = 0; output < DNA_OUTPUTS; output++)
        for (size_t hidden = 0; hidden < DNA_HIDDEN; hidden++)
            dna.w2[output][hidden] = randomFloat(5);
    for (size_t hidden = 0; hidden < DNA_HIDDEN; hidden++)
        dna.b1[hidden] = randomFloat(5);
    for (size_t output = 0; output < DNA_OUTPUTS; output++)
        dna.b2[output] = randomFloat(5);

    return dna;
}
float DNA_MutateGen(float val)
{
    if ((rand() % 100) > DNA_MUTATION_CHANCE)
        return val;

    float offset = randomFloat(25);
    return val + offset * DNA_LR;
}
DNA DNA_Mutate(DNA dna)
{
    for (size_t hidden = 0; hidden < DNA_HIDDEN; hidden++)
        for (size_t input = 0; input < DNA_INPUTS; input++)
            dna.w1[hidden][input] = DNA_MutateGen(dna.w1[hidden][input]);

    for (size_t output = 0; output < DNA_OUTPUTS; output++)
        for (size_t hidden = 0; hidden < DNA_HIDDEN; hidden++)
            dna.w2[output][hidden] = DNA_MutateGen(dna.w2[output][hidden]);

    for (size_t hidden = 0; hidden < DNA_HIDDEN; hidden++)
        dna.b1[hidden] = DNA_MutateGen(dna.b1[hidden]);

    for (size_t output = 0; output < DNA_OUTPUTS; output++)
        dna.b2[output] = DNA_MutateGen(dna.b2[output]);
    return dna;
}
DNA DNA_Breed(DNA p1, DNA p2)
{
    DNA child;

    for (size_t hidden = 0; hidden < DNA_HIDDEN; hidden++)
        for (size_t input = 0; input < DNA_INPUTS; input++)
            child.w1[hidden][input] = (p1.w1[hidden][input] + p2.w1[hidden][input]) / 2;
    for (size_t output = 0; output < DNA_OUTPUTS; output++)
        for (size_t hidden = 0; hidden < DNA_HIDDEN; hidden++)
            child.w2[output][hidden] = (p1.w2[output][hidden] + p2.w2[output][hidden]) / 2;

    for (size_t hidden = 0; hidden < DNA_HIDDEN; hidden++)
        child.b1[hidden] = (p1.b1[hidden] + p2.b1[hidden]) / 2;
    for (size_t output = 0; output < DNA_OUTPUTS; output++)
        child.b2[output] = (p1.b2[output] + p2.b2[output]) / 2;

    return child;
}
void DNA_Print(DNA dna) {
    printf("{\n");
    for (size_t hidden = 0; hidden < DNA_HIDDEN; hidden++) {
        printf("w%li [",hidden);
        for (size_t input = 0; input < DNA_INPUTS; input++) {
            printf("%f ,",dna.w1[hidden][input]);
        }
        printf("]\n");
    }
    for (size_t output = 0; output < DNA_OUTPUTS; output++){
        printf("w%li[",output);
        for (size_t hidden = 0; hidden < DNA_HIDDEN; hidden++){
            printf("%f ,",dna.b1[output]);
        }
        printf("]\n");
    }

    printf("b1[");
    for (size_t hidden = 0; hidden < DNA_HIDDEN; hidden++)
        printf("%f ,",dna.b1[hidden]);
    printf("]\n");
    printf("b2[");
    for (size_t output = 0; output < DNA_OUTPUTS; output++)
        printf("%f ,",dna.b2[output]);
    printf("]\n");
    printf("}\n");

}

Agent Agent_new()
{
    Agent agent;
    agent.aabb = (AABB){AGENT_START_X, HEIGHT / 2 - 10, 30, 30};
    agent.dna = DNA_RandomDna();
    agent.velY = 0;
    agent.fitness = 0;
    agent.jump_delay = AGENT_JUMP_DELAY;
    agent.jump_timer = 0;
    agent.dead = 0;

    return agent;
}
Agent Agent_newFromDna(DNA dna)
{
    Agent agent = Agent_new();
    agent.dna = dna;
    return agent;
}
void Agent_Update(Agent *agent, float dt, Pipe closePipe)
{
    agent->velY += GRAVITY * dt * TARGET_FPS;
    agent->fitness += dt;

    if (agent->velY > AGENT_MAX_VELY)
        agent->velY = AGENT_MAX_VELY;

    int action = Agent_FeedForward(*agent, closePipe);
    if (action == 1 && agent->jump_timer <= 0)
    {
        agent->velY = -AGENT_JUMP_FORCE * dt * TARGET_FPS;
        agent->jump_timer = agent->jump_delay;
    }

    if (agent->jump_timer > 0)
    {
        agent->jump_timer -= dt;
    }

    AABB bottomPipe = (AABB){
        closePipe.aabb.x,
        closePipe.aabb.y + closePipe.height + PIPE_GAP,
        closePipe.aabb.w,
        HEIGHT - (closePipe.aabb.h + PIPE_GAP)};
    if (isColliding(closePipe.aabb, agent->aabb) || isColliding(bottomPipe, agent->aabb))
    {
        agent->fitness -= 5;
        agent->dead = 1;
        agent->fitness += global_score;
    }
    if (agent->aabb.y + agent->aabb.h > HEIGHT + 5 || agent->aabb.y < -5)
    {
        agent->fitness -= 10;
        agent->dead = 1;
        agent->fitness += global_score;
    }

    agent->aabb.y += agent->velY;
}
int Agent_FeedForward(Agent agent, Pipe closePipe)
{
    float inputs[DNA_INPUTS] = {
        agent.velY / HEIGHT,
        agent.aabb.y / HEIGHT,
        closePipe.aabb.x / WIDTH,
        (closePipe.aabb.y + closePipe.aabb.h + PIPE_GAP) / HEIGHT,
    };

    float hidden_layer[DNA_HIDDEN];
    for (size_t hidden = 0; hidden < DNA_HIDDEN; hidden++)
    {
        hidden_layer[hidden] = agent.dna.b1[hidden];
        for (size_t input = 0; input < DNA_INPUTS; input++)
        {
            hidden_layer[hidden] += inputs[input] * agent.dna.w1[hidden][input];
        }
    }

    for (size_t i = 0; i < DNA_HIDDEN; i++){
        hidden_layer[i] = 1 / (1 + SDL_powf(2.7, -hidden_layer[i]));
        // SDL_Log("%f + ",hidden_layer[i]);
    }

    float output_layer[DNA_OUTPUTS];
    for (size_t output = 0; output < DNA_OUTPUTS; output++)
    {
        output_layer[output] = agent.dna.b2[output];
        for (size_t hidden = 0; hidden < DNA_HIDDEN; hidden++)
        {
            output_layer[output] += hidden_layer[hidden] * agent.dna.w2[output][hidden];
        }
    }

    for (size_t i = 0; i < DNA_OUTPUTS; i++) {
        output_layer[i] = 1 / (1 + SDL_powf(2.7, -output_layer[i]));
    }
    if (output_layer[0] > 0.5)
        return 1;

    return 0;
}
void Agent_Render(SDL_Renderer *renderer, Agent agent)
{
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect rect = (SDL_Rect){agent.aabb.x, agent.aabb.y, agent.aabb.w, agent.aabb.h};
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}
void Player_Update(Agent *agent, float dt, Pipe closePipe)
{

    agent->velY += GRAVITY * dt * TARGET_FPS;
    agent->fitness += dt;

    if (agent->velY > AGENT_MAX_VELY)
        agent->velY = AGENT_MAX_VELY;

    const Uint8 *events = SDL_GetKeyboardState(NULL);

    if (events[SDL_SCANCODE_SPACE] && agent->jump_timer <= 0 && !holdingJump)
    {
        agent->velY = -AGENT_JUMP_FORCE * dt * TARGET_FPS;
        agent->jump_timer = agent->jump_delay;
        holdingJump = 1;
    }
    if (!events[SDL_SCANCODE_SPACE])
    {
        holdingJump = 0;
    }
    if (agent->jump_timer > 0)
    {
        agent->jump_timer -= dt;
    }

    AABB bottomPipe = (AABB){
        closePipe.aabb.x,
        closePipe.aabb.y + closePipe.height + PIPE_GAP,
        closePipe.aabb.w,
        HEIGHT - (closePipe.aabb.h + PIPE_GAP)};
    if (isColliding(closePipe.aabb, agent->aabb) || isColliding(bottomPipe, agent->aabb) || agent->aabb.y + agent->aabb.h > HEIGHT + 5 || agent->aabb.y < -5)
    {
        agent->fitness -= 5;
        agent->dead = 1;
        agent->fitness += global_score;
    }

    agent->aabb.y += agent->velY;
}

Agent* Pop_Reset(Agent *pop)
{
    DNA gen_bestDna = pop[0].dna;
    DNA gen_secondBestDna = pop[0].dna;

    float gen_bestFitness = SDL_MIN_SINT32;
    int tmp = 0;
    for (size_t i = 0; i < POP_COUNT; i++)
    {
        if (pop[i].fitness > gen_bestFitness)
        {
            gen_bestFitness = pop[i].fitness;
            gen_bestDna = pop[i].dna;
        }
        else if (pop[i].fitness > pop[tmp].fitness)
        {
            gen_secondBestDna = pop[i].dna;
            tmp = i;
        }
    }

    // SDL_Log("%f", gen_bestFitness);
    DNA_Print(gen_bestDna);
    Agent *new_pop = calloc(POP_COUNT, sizeof(Agent));

    // for (size_t i = 0; i < (int)(POP_COUNT / 3); i++) {
    for (size_t i = 0; i < (int)(POP_COUNT / 3); i++){
        new_pop[i] = Agent_newFromDna(DNA_Mutate(gen_bestDna));
    }
    for (size_t i = (int)(POP_COUNT / 3); i < (int)(2*POP_COUNT / 3); i++){
        new_pop[i] = Agent_newFromDna(DNA_Breed(DNA_Mutate(gen_bestDna), DNA_Mutate(gen_secondBestDna)));
    }
    for (size_t i = (int)(2 * POP_COUNT / 3); i < POP_COUNT; i++) {
    new_pop[i] = Agent_new();
    }

    free(pop);
    return new_pop;
}

int main(void)
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Event event;
    SDL_Window *window;
    SDL_Renderer *renderer;



    srand(time(NULL));

    Pipe pipes[4];
    closest_pipe_index = 0;
    Pipes_Init(pipes, 4);

    Agent *pop = calloc(POP_COUNT, sizeof(Agent));

    for (size_t i = 0; i < POP_COUNT; i++)
        pop[i] = Agent_new();

    // Agent player = Agent_new();

    NN_Renderer nn_renderer;
    nn_renderer.screen_width = WIDTH;
    nn_renderer.screen_height =  HEIGHT; 
    nn_renderer.w = 200;
    nn_renderer.h = 200;  
    nn_renderer.x = WIDTH - nn_renderer.w;
    nn_renderer.y = HEIGHT - nn_renderer.h;  


    NN nn;
    nn.inputs_count = DNA_INPUTS;
    nn.outputs_count = DNA_OUTPUTS;
    nn.hidden_count =  DNA_HIDDEN;




    SDL_Log("%f %f",nn_renderer.x , nn_renderer.y);


    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_SHOWN, &window, &renderer);
    int running = 1;

    SDL_Rect rect = {10, 10, 50, 50};
    while (running)
    {
        Uint64 start = SDL_GetPerformanceCounter();
        Uint32 start_frame = SDL_GetTicks();

        while (SDL_PollEvent(&event))
            if (event.type == SDL_QUIT)
                running = 0;

        Pipes_Update(pipes, 4);
        int pop_count = POP_COUNT;
        for (size_t i = 0; i < POP_COUNT; i++)
        {
            if (!pop[i].dead)
            {
                Agent_Update(&pop[i], dt, pipes[closest_pipe_index]);
            }
            else
            {
                pop_count--;
            }
        }
        // Player_Update(&player, dt, pipes[closest_pipe_index]);
        // if(player.dead) {
        //     player = Agent_new();
        //     Pipes_Init(pipes,4);
        // }
        if (pop_count <= 0)
        {
            pop = Pop_Reset(pop);
            Pipes_Init(pipes, 4);
        }

        Pipes_Render(renderer, pipes, 4);
        for (size_t i = 0; i < POP_COUNT; i++)
        {
            if (!pop[i].dead)
            {
                Agent_Render(renderer, pop[i]);
            }
        }
        // SDL_Rect rect = (SDL_Rect){pipes[closest_pipe_index].aabb.x, pipes[closest_pipe_index].aabb.y, pipes[closest_pipe_index].aabb.w, pipes[closest_pipe_index].aabb.h};
        // SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
        // SDL_RenderFillRect(renderer, &rect);
        // SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        // rect = (SDL_Rect){player.aabb.x, player.aabb.y, player.aabb.w, player.aabb.h};
        // SDL_RenderFillRect(renderer, &rect);
        // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        // SDL_SetRenderDrawColor(renderer,255,0,0,255);
        // SDL_RenderFillRect(renderer,&rect);
        // SDL_SetRenderDrawColor(renderer,0,0,0,255);



        NN_Render(renderer,nn_renderer,nn);

        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer);

        Uint64 elapsedCount = SDL_GetPerformanceCounter() - start;
        if (SDL_GetPerformanceFrequency() / FPS > elapsedCount)
        {
            Uint32 remainingMilliseconds = (SDL_GetPerformanceFrequency() / FPS - elapsedCount) * 1000 / SDL_GetPerformanceFrequency();
            SDL_Delay(remainingMilliseconds);
            dt = (SDL_GetTicks() - start_frame) / 1000.0f * SPEED_UP;
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}