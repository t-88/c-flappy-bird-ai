#include "dna.h"





DNA DNA_RandomDna(){
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
float DNA_MutateGen(float val){
    if ((rand() % 100) > DNA_MUTATION_CHANCE)
        return val;

    float offset = randomFloat(25);
    return val + offset * DNA_LR;
}
DNA DNA_Mutate(DNA dna){
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
DNA DNA_Breed(DNA p1, DNA p2){
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


NN_Output DNA_FeedForward(DNA dna, float inputs[DNA_INPUTS]){

    NN_Output feedForwardOutput;
    for (size_t hidden = 0; hidden < DNA_HIDDEN; hidden++) {
        feedForwardOutput.l1[hidden] = dna.b1[hidden];
        for (size_t input = 0; input < DNA_INPUTS; input++) {
            feedForwardOutput.l1[hidden] += inputs[input] * dna.w1[hidden][input];
        }
    }
    for (size_t i = 0; i < DNA_HIDDEN; i++) {
        feedForwardOutput.l1[i] = 1 / (1 + SDL_powf(2.71, -feedForwardOutput.l1[i]));
    }

    for (size_t output = 0; output < DNA_OUTPUTS; output++) {
        feedForwardOutput.l2[output] = dna.b2[output];
        for (size_t hidden = 0; hidden < DNA_HIDDEN; hidden++) {
            feedForwardOutput.l2[output] += feedForwardOutput.l1[hidden] * dna.w2[output][hidden];
        }
    }
    for (size_t i = 0; i < DNA_OUTPUTS; i++) {
        feedForwardOutput.l2[i] = 1 / (1 + SDL_powf(2.71, -feedForwardOutput.l2[i]));
    } 


    return feedForwardOutput;
}