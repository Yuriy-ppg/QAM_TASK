#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "qam_point.h"
#include "qam_modulator.h"
#include "qam_demodulator.h"
#include "awgn_channel.h"
#include "help_func.h"

int main(void)
{
    srand((unsigned int)time(nullptr));

    static const double variances[] = {0.001, 0.005, 0.01, 0.02, 0.05, 0.1, 0.2, 0.3, 0.5, 0.8, 1.0, 1.5, 2.0, 3.0, 5.0};
    static const int NVAR = (int)(sizeof(variances) / sizeof(variances[0]));

    double ber_qpsk[NVAR];
    double ber_qam16[NVAR];
    double ber_qam64[NVAR];

    printf("=== QPSK ===\n");
    for(int i = 0; i < NVAR; i++)
    {
        if(run_simulation(qam_order::qpsk, variances[i], &ber_qpsk[i], "QPSK") != 0)
        {
            return 1;
        }
    }

    printf("\n=== QAM16 ===\n");
    for(int i = 0; i < NVAR; i++)
    {
        if(run_simulation(qam_order::qam16, variances[i], &ber_qam16[i], "QAM16") != 0)
        {
            return 1;
        }
    }

    printf("\n=== QAM64 ===\n");
    for(int i = 0; i < NVAR; i++)
    {
        if(run_simulation(qam_order::qam64, variances[i], &ber_qam64[i], "QAM64") != 0)
        {
            return 1;
        }
    }

    if(save_csv("ber_results.csv", variances, NVAR, ber_qpsk, ber_qam16, ber_qam64) != 0)
    {
        return 1;
    }
    printf("\nResults saved to ber_results.csv\n");
    printf("Run: python3 plot_ber.py\n");
    return 0;
}
