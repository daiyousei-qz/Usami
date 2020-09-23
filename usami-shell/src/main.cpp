#include <cstdio>
#include "usami/common.h"
#include "usami/memory/arena.h"
#include "usami/math/math.h"
#include "usami/model.h"

int main()
{
    using namespace usami;

    auto model = LoadModel("d:/demo/scene/bunny2/bunny2.obj");

    printf("hello world!");
}