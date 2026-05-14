#ifndef QAM_MODULATOR_H
#define QAM_MODULATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "qam_point.h"

enum class qam_order
{
    qpsk = 4,
    qam16 = 16,
    qam64 = 64,
};

class qam_modulator
{
    private:
        qam_order order = qam_order::qpsk;
        int bits_per_symbol = 2;
        double *constellation_i = nullptr;
        double *constellation_q = nullptr;
        int constellation_size = 0;

    public:
        qam_modulator() = default;
        qam_modulator(const qam_modulator &x) = delete;
        qam_modulator(qam_modulator &&x)
        {
            order = x.order;
            bits_per_symbol = x.bits_per_symbol;
            constellation_i = x.constellation_i;
            constellation_q = x.constellation_q;
            constellation_size = x.constellation_size;
            x.erase_links();
        }
        ~qam_modulator()
        {
            free_constellation();
        }

        qam_modulator& operator= (const qam_modulator &x) = delete;
        qam_modulator& operator= (qam_modulator &&x)
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



        int init(qam_order ord)
        {
            free_constellation();
            order = ord;
            constellation_size = (int)ord;
            bits_per_symbol = (int)round(log2((double)constellation_size));

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
            build_gray_constellation();
            normalize();
            return 0;
        }

        int get_bits_per_symbol() const
        {
            return bits_per_symbol;
        }

        int get_constellation_size() const
        {
            return constellation_size;
        }

        qam_point* modulate(const int *bits, int num_bits, int *num_symbols_out) const
        {
            if(bits == nullptr || num_bits <= 0 || constellation_i == nullptr)
            {
                return nullptr;
            }
            int num_symbols = num_bits / bits_per_symbol;
            *num_symbols_out = num_symbols;
            qam_point *symbols = new qam_point[num_symbols];
            if(symbols == nullptr)
            {
                return nullptr;
            }
            for(int s = 0; s < num_symbols; s++)
            {
                int idx = 0;
                for(int b = 0; b < bits_per_symbol; b++)
                {
                    idx = (idx << 1) | (bits[s * bits_per_symbol + b] & 1);
                }
                symbols[s].i = constellation_i[idx];
                symbols[s].q = constellation_q[idx];
            }
            return symbols;
        }

        void print_constellation(FILE *fp = stdout) const
        {
            fprintf(fp, "Constellation (M=%d):\n", constellation_size);
            for (int k = 0; k < constellation_size; k++)
            {
                fprintf(fp, "  [%2d] I=%.4f Q=%.4f\n", k, constellation_i[k], constellation_q[k]);
            }
        }

    private:
        void build_gray_constellation()
        {
            int side = (int)round(sqrt((double)constellation_size));
            for(int k = 0; k < constellation_size; k++)
            {
                int gray = gray_decode(k, bits_per_symbol);
                int xi = gray % side;
                int xq = gray / side;
                constellation_i[k] = (double)(2 * xi - (side - 1));
                constellation_q[k] = (double)(2 * xq - (side - 1));
            }
        }

        void normalize()
        {
            double avg_power = 0.0;
            for(int k = 0; k < constellation_size; k++)
            {
                avg_power += constellation_i[k] * constellation_i[k] + constellation_q[k] * constellation_q[k];
            }
            avg_power /= (double)constellation_size;
            double scale = sqrt(avg_power);
            if(scale < 1e-12)
            {
                return;
            }
            for(int k = 0; k < constellation_size; k++)
            {
                constellation_i[k] /= scale;
                constellation_q[k] /= scale;
            }
        }

        static int gray_decode(int n, int nbits)
        {
            int side = (int)round(sqrt(pow(2.0, (double)nbits)));
            int xi = n % side;
            int xq = n / side;
            xi = gray_to_bin(xi);
            xq = gray_to_bin(xq);
            return xq * side + xi;
        }

        static int gray_to_bin(int g)
        {
            int p = g;
            int res = g;
            while((p >>= 1))
            {
                res ^= p;
            }
            return res;
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
