#pragma once
#include "usami/math/math.h"
#include "usami/math/random.h"

namespace usami
{
    class Sampler
    {
    public:
        Sampler(uint64_t seed)
        {
            engine_.Seed(seed);
        }
        // virtual ~Sampler() = default;

        // get a sample of x where x is in [0, 1)
        float Get1D()
        {
            return SampleUniformFloat(engine_);
        }
        // get a sample of (x, y) where both x, y are in [0, 1)
        Point2f Get2D()
        {
            return SampleUniformFloat2D(engine_);
        }

    private:
        RandomEngine engine_;
    };
} // namespace usami
