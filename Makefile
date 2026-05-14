OPTS = -O3 -mfpmath=sse -fstack-protector-all -g -W -Wall -Wextra -Wunused \
       -Wcast-align -Werror -pedantic -pedantic-errors -Wfloat-equal \
       -Wpointer-arith -Wformat-security -Wmissing-format-attribute \
       -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long \
       -Woverloaded-virtual -Wnon-virtual-dtor -Wcast-qual \
       -Wno-suggest-attribute=format

all: a.out

a.out: main.o
	g++ $(OPTS) $^ -o $@ -lm

main.o: main.cpp qam_point.h qam_modulator.h awgn_channel.h qam_demodulator.h
	g++ -c $(OPTS) $< -o $@

run: a.out
	./a.out

plot: run
	python3 plot_ber.py

clean:
	rm -f *.o *.out *.csv *.png
