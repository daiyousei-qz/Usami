#pragma once
#include "usami/math/math.h"
#include <vector>

namespace usami
{
    class DiscrateDistribution
    {
    public:
        DiscrateDistribution() = default;
        DiscrateDistribution(const float* weight_begin, const float* weight_end) : thresholds_{}
        {
            Reset(weight_begin, weight_end);
        }

        int Sample(float u, float& pdf_out) const
        {
            if (u < thresholds_.front())
            {
                pdf_out = thresholds_.front();
                return 0;
            }

            for (int i = 1; i < thresholds_.size(); ++i)
            {
                if (u < thresholds_[i])
                {
                    pdf_out = thresholds_[i] - thresholds_[i - 1];
                    return i;
                }
            }

            // error
            pdf_out = 0;
            return -1;
        }

        void Reset(const float* weight_begin, const float* weight_end)
        {
            thresholds_.clear();

            int num_output = std::distance(weight_begin, weight_end);
            if (num_output <= 0)
            {
                thresholds_ = {1.f};
                return;
            }

            thresholds_.reserve(num_output);

            float acc = 0;
            for (const float* p = weight_begin; p != weight_end; ++p)
            {
                USAMI_ASSERT(*p > 0);

                acc += *p;
                thresholds_.push_back(acc);
            }

            for (float& x : thresholds_)
            {
                x /= acc;
            }
        }

    private:
        std::vector<float> thresholds_{1.f};
    };

} // namespace usami