#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>
#include "consts.h"
#include "utils.h"

typedef struct DNA {
    float w1[DNA_HIDDEN][DNA_INPUTS];
    float w2[DNA_OUTPUTS][DNA_HIDDEN];
    float b1[DNA_HIDDEN];
    float b2[DNA_OUTPUTS];
} DNA;
typedef struct NN_Output {
    float l1[DNA_HIDDEN] ,  l2[DNA_OUTPUTS];
} NN_Output;

DNA DNA_RandomDna();
float DNA_MutateGen(float val);
DNA DNA_Mutate(DNA dna);
DNA DNA_Breed(DNA p1, DNA p2);
void DNA_Print(DNA dna);
void DNA_Print(DNA dna);

NN_Output DNA_FeedForward(DNA dna, float* inputs);