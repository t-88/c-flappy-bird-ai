#include "neuralNetworkRender.h"
 



void NN_Render(SDL_Renderer* renderer,NN_Renderer nn_renderer,NN nn) {
    SDL_Rect rect = (SDL_Rect) {nn_renderer.x, nn_renderer.y,nn_renderer.w,nn_renderer.h};
    SDL_SetRenderDrawColor(renderer,255,255,0,255);
    SDL_RenderDrawRect(renderer,&rect);
    SDL_SetRenderDrawColor(renderer,0,0,0,255);



    SDL_SetRenderDrawColor(renderer,255,0,0,255);
    for (size_t input = 0; input < nn.inputs_count; input++) {
        SDL_Rect rect = (SDL_Rect) {nn_renderer.x + 10, nn_renderer.y + 20 + input * (nn_renderer.h / nn.inputs_count),10,10};
        SDL_RenderFillRect(renderer,&rect);
    }
    SDL_SetRenderDrawColor(renderer,0,0,0,255);

    SDL_SetRenderDrawColor(renderer,255,0,0,255);
    for (size_t hidden = 0; hidden < nn.hidden_count; hidden++) {
        rect = (SDL_Rect) {nn_renderer.x + 100, nn_renderer.y + 15 + hidden * (nn_renderer.h / nn.hidden_count),10,10};
        SDL_RenderFillRect(renderer,&rect);
    }
    SDL_SetRenderDrawColor(renderer,0,0,0,255);

    SDL_SetRenderDrawColor(renderer,255,0,0,255);
        rect = (SDL_Rect) {nn_renderer.x + 180, nn_renderer.y +  nn_renderer.h / 2,10,10};
        SDL_RenderFillRect(renderer,&rect);
    SDL_SetRenderDrawColor(renderer,0,0,0,255);    


    SDL_SetRenderDrawColor(renderer,255,255,255,255);
    for (size_t hidden = 0; hidden < nn.hidden_count; hidden++) {
        for (size_t input = 0; input < nn.inputs_count; input++) {
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
    for (size_t hidden = 0; hidden < nn.hidden_count; hidden++) {
        for (size_t input = 0; input < nn.inputs_count; input++) {
            SDL_RenderDrawLine(renderer,
                               nn_renderer.x + 10 + 5,
                               nn_renderer.y + 20 + input * (nn_renderer.h / nn.inputs_count) + 5,
                               nn_renderer.x + 100 + 5, 
                               nn_renderer.y + 15 + hidden * (nn_renderer.h / nn.hidden_count) + 5
                            );
        }
    }
    SDL_SetRenderDrawColor(renderer,0,0,0,255);

    
}