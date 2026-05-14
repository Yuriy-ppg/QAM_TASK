#ifndef QAM_DEMODULATOR_H
#define QAM_DEMODULATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "qam_point.h"
#include "qam_modulator.h"

class qam_demodulator
{
    private:
        qam_order order = qam_order::qpsk;
        int bits_per_symbol = 2;
        double *constellation_i = nullptr;
        double *constellation_q = nullptr;
        int constellation_size = 0;

    public:
        qam_demodulator() = default;
        qam_demodulator(const qam_demodulator &x) = delete;
        qam_demodulator(qam_demodulator &&x)
        {
            order = x.order;
            bits_per_symbol = x.bits_per_symbol;
            constellation_i = x.constellation_i;
            constellation_q = x.constellation_q;
            constellation_size = x.constellation_size;
            x.erase_links();
        }
        ~qam_demodulator()
        {
            free_constellation();
        }

        qam_demodulator& operator= (const qam_demodulator &x) = delete;
        qam_demodulator& operator= (qam_demodulator &&x)
        {
            if (this == &x)
            {
                return *this;
            }
            free_constellation();
            order = x.order;
            bits_per_symbol = x.bits_per_symbol;
            constellation_i = x.constellation_i;
            constellation_q = x.constellation_q;
            constellation_size = x.constellation_size;
            x.erase_links();
            return *this;
        }

        int init(const qam_modulator &mod)
        {
            free_constellation();
            order = (qam_order)mod.get_constellation_size();
            constellation_size = mod.get_constellation_size();
            bits_per_symbol = mod.get_bits_per_symbol();

            constellation_i = new double[constellation_size];
            if (constellation_i == nullptr)
            {
                return -1;
            }
            constellation_q = new double[constellation_size];
            if (constellation_q == nullptr)
            {
                delete[] constellation_i;
                constellation_i = nullptr;
                return -1;
            }

            int *dummy_bits = new int[bits_per_symbol];
            if (dummy_bits == nullptr)
            {
                free_constellation();
                return -1;
            }
            for (int k = 0; k < constellation_size; k++)
            {
                for (int b = bits_per_symbol - 1; b >= 0; b--)
                {
                    dummy_bits[b] = (k >> (bits_per_symbol - 1 - b)) & 1;
                }
                int nsym = 0;
                qam_point *pt = mod.modulate(dummy_bits, bits_per_symbol, &nsym);
                if (pt == nullptr)
                {
                    delete[] dummy_bits;
                    free_constellation();
                    return -1;
                }
                constellation_i[k] = pt[0].i;
                constellation_q[k] = pt[0].q;
                delete[] pt;
            }
            delete[] dummy_bits;
            return 0;
        }

        int get_bits_per_symbol() const
        {
            return bits_per_symbol;
        }

        int* demodulate(const qam_point *symbols, int num_symbols, int *num_bits_out) const
        {
            if (symbols == nullptr || num_symbols <= 0 || constellation_i == nullptr)
            {
                return nullptr;
            }
            int num_bits = num_symbols * bits_per_symbol;
            *num_bits_out = num_bits;
            int *bits = new int[num_bits];
            if (bits == nullptr)
            {
                return nullptr;
            }
            for (int s = 0; s < num_symbols; s++)
            {
                int best = find_nearest(symbols[s]);
                for (int b = bits_per_symbol - 1; b >= 0; b--)
                {
                    bits[s * bits_per_symbol + (bits_per_symbol - 1 - b)] = (best >> b) & 1;
                }
            }
            return bits;
        }

    private:
        int find_nearest(const qam_point &pt) const
        {
            int best_idx = 0;
            double best_dist = 1e300;
            for (int k = 0; k < constellation_size; k++)
            {
                double di = pt.i - constellation_i[k];
                double dq = pt.q - constellation_q[k];
                double dist = di * di + dq * dq;
                if(dist < best_dist)
                {
                    best_dist = dist;
                    best_idx = k;
                }
            }
            return best_idx;
        }

        void free_constellation()
        {
            if (constellation_i != nullptr)
            {
                delete[] constellation_i;
                constellation_i = nullptr;
            }
            if (constellation_q != nullptr)
            {
                delete[] constellation_q;
                constellation_q = nullptr;
            }
            constellation_size = 0;
        }

        void erase_links()
        {
            constellation_i = nullptr;
            constellation_q = nullptr;
            constellation_size = 0;
        }
};

#endif
