#pragma once
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "consts.h"


float randomFloat(float a);
int isColliding(AABB aabb1, AABB aabb2);
float map(float value,float start1,float end1,float start2,float end2);