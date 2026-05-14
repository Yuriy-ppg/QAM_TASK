#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "qam_point.h"
#include "qam_modulator.h"
#include "qam_demodulator.h"
#include "awgn_channel.h"

static const int NUM_BITS = 100000;

static int* generate_bits(int num_bits)
{
    int *bits = new int[num_bits];
    if (bits == nullptr)
    {
        return nullptr;
    }
    for (int i = 0; i < num_bits; i++)
    {
        bits[i] = rand() & 1;
    }
    return bits;
}

static double compute_ber(const int *tx_bits, const int *rx_bits, int num_bits)
{
    int errors = 0;
    for (int i = 0; i < num_bits; i++)
    {
        if (tx_bits[i] != rx_bits[i])
        {
            errors++;
        }
    }
    return (double)errors / (double)num_bits;
}

static int run_simulation(qam_order ord, double variance, double *ber_out, const char *label)
{
    qam_modulator mod;
    if(mod.init(ord) != 0)
    {
        fprintf(stderr, "Modulator init failed\n");
        return -1;
    }

    qam_demodulator demod;
    if(demod.init(mod) != 0)
    {
        fprintf(stderr, "Demodulator init failed\n");
        return -1;
    }

    awgn_channel channel;
    channel.set_variance(variance);

    int num_bits = (NUM_BITS / mod.get_bits_per_symbol()) * mod.get_bits_per_symbol();
    int *tx_bits = generate_bits(num_bits);
    if(tx_bits == nullptr)
    {
        fprintf(stderr, "Memory error\n");
        return -1;
    }

    int num_symbols = 0;
    qam_point *symbols = mod.modulate(tx_bits, num_bits, &num_symbols);
    if(symbols == nullptr)
    {
        delete[] tx_bits;
        fprintf(stderr, "Modulate failed\n");
        return -1;
    }

    qam_point *noisy = channel.add_noise_copy(symbols, num_symbols);
    if(noisy == nullptr)
    {
        delete[] symbols;
        delete[] tx_bits;
        fprintf(stderr, "Channel failed\n");
        return -1;
    }

    int num_rx_bits = 0;
    int *rx_bits = demod.demodulate(noisy, num_symbols, &num_rx_bits);
    if(rx_bits == nullptr)
    {
        delete[] noisy;
        delete[] symbols;
        delete[] tx_bits;
        fprintf(stderr, "Demodulate failed\n");
        return -1;
    }

    *ber_out = compute_ber(tx_bits, rx_bits, num_bits);
    printf("%-8s  variance=%.5f  BER=%.6f\n", label, variance, *ber_out);

    delete[] rx_bits;
    delete[] noisy;
    delete[] symbols;
    delete[] tx_bits;
    return 0;
}

static int save_csv(const char *filename, const double *variances, int nvar, const double *ber_qpsk, const double *ber_qam16, const double *ber_qam64)
{
    FILE *fp = fopen(filename, "w");
    if (fp == nullptr)
    {
        fprintf(stderr, "Cannot open %s\n", filename);
        return -1;
    }
    fprintf(fp, "variance,BER_QPSK,BER_QAM16,BER_QAM64\n");
    for (int i = 0; i < nvar; i++)
    {
        fprintf(fp, "%.6f,%.8f,%.8f,%.8f\n",
                variances[i], ber_qpsk[i], ber_qam16[i], ber_qam64[i]);
    }
    fclose(fp);
    return 0;
}