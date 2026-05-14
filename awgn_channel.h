#ifndef AWGN_CHANNEL_H
#define AWGN_CHANNEL_H

#include <stdlib.h>
#include <math.h>
#include "qam_point.h"

class awgn_channel
{
    private:
        double variance = 0.0;

    public:
        awgn_channel() = default;
        awgn_channel(const awgn_channel &x) = default;
        awgn_channel(awgn_channel &&x) = default;
        ~awgn_channel() = default;

        awgn_channel& operator= (const awgn_channel &x) = default;
        awgn_channel& operator= (awgn_channel &&x) = default;

        void set_variance(double var)
        {
            variance = var;
        }

        double get_variance() const
        {
            return variance;
        }

        void add_noise(qam_point *symbols, int num_symbols) const
        {
            if (symbols == nullptr || num_symbols <= 0)
            {
                return;
            }
            double sigma = sqrt(variance);
            for(int s = 0; s < num_symbols; s++)
            {
                symbols[s].i += sigma * randn();
                symbols[s].q += sigma * randn();
            }
        }

        qam_point* add_noise_copy(const qam_point *symbols, int num_symbols) const
        {
            if (symbols == nullptr || num_symbols <= 0)
            {
                return nullptr;
            }
            qam_point *noisy = new qam_point[num_symbols];
            if (noisy == nullptr)
            {
                return nullptr;
            }
            double sigma = sqrt(variance);
            for (int s = 0; s < num_symbols; s++)
            {
                noisy[s].i = symbols[s].i + sigma * randn();
                noisy[s].q = symbols[s].q + sigma * randn();
            }
            return noisy;
        }

    private:
        static double randn()
        {
            double u1, u2;
            do
            {
                u1 = (double)rand() / ((double)RAND_MAX + 1.0);
            }
            while (u1 < 1e-12);
            u2 = (double)rand() / ((double)RAND_MAX + 1.0);
            return sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
        }
};

#endif
