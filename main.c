#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <SDL.h>

#include "neuralNetworkRender.h"
#include "consts.h"
#include "dna.h"
#include "utils.h"


float dt;
int closest_pipe_index;
int global_score;
int holdingJump = 0;


typedef struct Pipe
{
    AABB aabb;
    float height;
    int tookScore;
} Pipe;



typedef struct Agent
{
    DNA dna;
    AABB aabb;
    float velY;
    int dead;
    float fitness;

    float jump_delay, jump_timer;
} Agent;




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

int Agent_getBest(Agent* pop) {
    int index = 0;
    for (size_t i = 0; i < POP_COUNT; i++) 
        if (pop[i].fitness > pop[index].fitness) {
            index = i;
        }
    return index;
}
Agent Agent_new(){
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
        agent.velY / AGENT_MAX_VELY,
        agent.aabb.y / HEIGHT,
        closePipe.aabb.x / WIDTH,
        (closePipe.aabb.y + closePipe.aabb.h + PIPE_GAP) / HEIGHT,
    };

    NN_Output output = DNA_FeedForward(agent.dna,inputs);

    if (output.l2[0] > 0.5)
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
            if (!pop[i].dead) {
                Agent_Update(&pop[i], dt, pipes[closest_pipe_index]);
            }
            else {
                pop_count--;
            }
        }




        if (pop_count <= 0) {
            pop = Pop_Reset(pop);
            Pipes_Init(pipes, 4);
            continue;
        }

        int index = Agent_getBest(pop);
        nn.dna = pop[index].dna;
        nn.inputs[0] = pop[index].velY / AGENT_MAX_VELY;
        nn.inputs[1] = pop[index].aabb.y / HEIGHT;
        nn.inputs[2] = pipes[closest_pipe_index].aabb.x / WIDTH;
        nn.inputs[3] = (pipes[closest_pipe_index].aabb.y + pipes[closest_pipe_index].aabb.h + PIPE_GAP) / HEIGHT;


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