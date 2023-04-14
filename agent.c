#include "agent.h"

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
    agent.aabb = (AABB){AGENT_START_X, HEIGHT / 2 - 10, 26 * 2, 32};
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
void Agent_Update(Agent *agent, float dt, Pipe closePipe,int global_score)
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
    if (isColliding(closePipe.aabb, agent->aabb) || isColliding(bottomPipe, agent->aabb)) {
        agent->dead = 1;
    }
    if (agent->aabb.y + agent->aabb.h > HEIGHT + 5 || agent->aabb.y < -5) {
        agent->dead = 1;
    }

    if(agent->dead) {
        agent->fitness += global_score;
        agent->fitness -= 10;
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

    NN_Output output = DNA_FeedForward(agent.dna,inputs);

    if (output.l2[0] > 0.5)
        return 1;

    return 0;
}
void Agent_Render(SDL_Renderer *renderer, Agent agent,SDL_Texture* bird)
{
    // SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect rect = (SDL_Rect){agent.aabb.x, agent.aabb.y, 26 * 2, 32};
    // SDL_RenderFillRect(renderer, &rect);


    SDL_RenderCopy(renderer,bird,NULL,&rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}
