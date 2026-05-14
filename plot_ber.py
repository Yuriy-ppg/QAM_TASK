import csv
import math
import sys

try:
    import matplotlib.pyplot as plt
    HAS_MATPLOTLIB = True
except ImportError:
    HAS_MATPLOTLIB = False


def read_csv(filename):
    variances = []
    ber_qpsk  = []
    ber_qam16 = []
    ber_qam64 = []
    with open(filename, newline='') as f:
        reader = csv.DictReader(f)
        for row in reader:
            variances.append(float(row['variance']))
            ber_qpsk.append(float(row['BER_QPSK']))
            ber_qam16.append(float(row['BER_QAM16']))
            ber_qam64.append(float(row['BER_QAM64']))
    return variances, ber_qpsk, ber_qam16, ber_qam64


def print_ascii(variances, ber_qpsk, ber_qam16, ber_qam64):
    print("\nBER vs Noise Variance (log scale)")
    print(f"{'Variance':>12}  {'QPSK':>10}  {'QAM16':>10}  {'QAM64':>10}")
    print("-" * 50)
    for v, q, q16, q64 in zip(variances, ber_qpsk, ber_qam16, ber_qam64):
        print(f"{v:12.4f}  {q:10.6f}  {q16:10.6f}  {q64:10.6f}")


def main():
    filename = "ber_results.csv"
    if len(sys.argv) > 1:
        filename = sys.argv[1]

    variances, ber_qpsk, ber_qam16, ber_qam64 = read_csv(filename)
    print_ascii(variances, ber_qpsk, ber_qam16, ber_qam64)

    if not HAS_MATPLOTLIB:
        print("\nmatplotlib not found — install with: pip3 install matplotlib")
        return

    fig, ax = plt.subplots(figsize=(9, 6))

    ax.semilogy(variances, ber_qpsk,  'b-o', label='QPSK',  linewidth=2, markersize=6)
    ax.semilogy(variances, ber_qam16, 'r-s', label='QAM16', linewidth=2, markersize=6)
    ax.semilogy(variances, ber_qam64, 'g-^', label='QAM64', linewidth=2, markersize=6)

    ax.set_xlabel('Дисперсия шума $\\sigma^2$', fontsize=13)
    ax.set_ylabel('BER (вероятность ошибки на бит)', fontsize=13)
    ax.set_title('BER vs дисперсия шума для QAM модуляций', fontsize=14)
    ax.legend(fontsize=12)
    ax.grid(True, which='both', linestyle='--', alpha=0.6)

    outfile = "ber_plot.png"
    plt.tight_layout()
    plt.savefig(outfile, dpi=150)
    print(f"\nPlot saved to {outfile}")
    plt.show()


if __name__ == '__main__':
    main()
