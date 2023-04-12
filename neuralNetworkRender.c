#include "neuralNetworkRender.h"
 

float map(float value,float start1,float end1,float start2,float end2) {
    float maped1 = end1 - start1;
    float maped2 = end2 - start2;

    return (maped2 / maped1) * (value - start1) + start2;  
}

void NN_Render(SDL_Renderer* renderer,NN_Renderer nn_renderer,NN nn) {
    SDL_Rect rect = (SDL_Rect) {nn_renderer.x, nn_renderer.y,nn_renderer.w,nn_renderer.h};
    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    SDL_RenderFillRect(renderer,&rect);
    SDL_SetRenderDrawColor(renderer,0,0,0,255);

    float maxW1 = SDL_MIN_SINT32;
    float minW1 = SDL_MAX_SINT32;
    float maxW2 = SDL_MIN_SINT32;
    float minW2 = SDL_MAX_SINT32;

    for (size_t hidden = 0; hidden < nn.hidden_count; hidden++)
        for(size_t input = 0; input < nn.inputs_count; input++) {
            if(maxW1 < nn.dna.w1[hidden][input])
                maxW1 = nn.dna.w1[hidden][input];
            else if(minW1 > nn.dna.w1[hidden][input] )
                minW1 = nn.dna.w1[hidden][input];
        }
    for (size_t output = 0; output < nn.outputs_count; output++)
        for (size_t hidden = 0; hidden < nn.hidden_count; hidden++) {
            if(maxW2 < nn.dna.w2[output][hidden])
                maxW2 = nn.dna.w2[output][hidden];
            else if(minW2 > nn.dna.w2[output][hidden] )
                minW2 = nn.dna.w2[output][hidden];
        }    

    NN_Output output = DNA_FeedForward(nn.dna,nn.inputs);
    // float maxB1 = 0;
    // float maxB2 = 0;
    // for (size_t hidden = 0; hidden < nn.hidden_count; hidden++)
    //     maxB1 += nn.dna.b1[hidden];
    // for (size_t output = 0; output < nn.outputs_count; output++)
    //     maxB2 += nn.dna.b2[output];

    for (size_t input = 0; input < nn.inputs_count; input++) {
        SDL_SetRenderDrawColor(renderer,map(nn.inputs[input],0,1,0,255),0,0,255);
        SDL_Rect rect = (SDL_Rect) {nn_renderer.x + 10, nn_renderer.y + 20 + input * (nn_renderer.h / nn.inputs_count),10,10};
        SDL_RenderFillRect(renderer,&rect);

        SDL_SetRenderDrawColor(renderer,255,0,0,255);
        SDL_RenderDrawRect(renderer,&rect);
    }
    SDL_SetRenderDrawColor(renderer,0,0,0,255);

    for (size_t hidden = 0; hidden < nn.hidden_count; hidden++) {
        SDL_SetRenderDrawColor(renderer,map(output.l1[hidden],0,1,0,255),0,0,255);
        rect = (SDL_Rect) {nn_renderer.x + 100, nn_renderer.y + 15 + hidden * (nn_renderer.h / nn.hidden_count),10,10};
        SDL_RenderFillRect(renderer,&rect);

        SDL_SetRenderDrawColor(renderer,255,0,0,255);
        SDL_RenderDrawRect(renderer,&rect);
    }
    SDL_SetRenderDrawColor(renderer,0,0,0,255);

    SDL_SetRenderDrawColor(renderer,map(output.l2[0],0,1,0,255),0,0,255);
        rect = (SDL_Rect) {nn_renderer.x + 180, nn_renderer.y +  nn_renderer.h / 2,10,10};
        SDL_RenderFillRect(renderer,&rect);
        SDL_SetRenderDrawColor(renderer,255,0,0,255);
        SDL_RenderDrawRect(renderer,&rect);
    SDL_SetRenderDrawColor(renderer,0,0,0,255);    







    for (size_t hidden = 0; hidden < nn.hidden_count; hidden++) {
        for (size_t input = 0; input < nn.inputs_count; input++) {
            SDL_SetRenderDrawColor(renderer,0,map(nn.dna.w1[hidden][input],minW1,maxW1,0,255),0,255);
            SDL_RenderDrawLine(renderer,
                               nn_renderer.x + 10 + 5,
                               nn_renderer.y + 20 + input * (nn_renderer.h / nn.inputs_count) + 5,
                               nn_renderer.x + 100 + 5, 
                               nn_renderer.y + 15 + hidden * (nn_renderer.h / nn.hidden_count) + 5
                            );
        }
    }

    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    SDL_SetRenderDrawColor(renderer,255,255,255,255);
    for (size_t output = 0; output < nn.outputs_count; output++) {
        for (size_t hidden = 0; hidden < nn.hidden_count; hidden++) {
            SDL_SetRenderDrawColor(renderer,0,map(nn.dna.w1[output][hidden],minW2,maxW2,0,255),0,255);

            SDL_RenderDrawLine(renderer,
                               nn_renderer.x + 100 + 5,
                               nn_renderer.y + 20 + hidden * (nn_renderer.h / nn.hidden_count),
                               nn_renderer.x + 180 + 5,
                               nn_renderer.y +  nn_renderer.h / 2 + 5
                            );
        }
    }
    SDL_SetRenderDrawColor(renderer,0,0,0,255);

    
}