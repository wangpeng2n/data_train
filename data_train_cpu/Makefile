
CFLAGS ?= -g -Wall -Werror

all: data_train_cpu 

data_train_cpu: data_train_cpu.o
	cc -o $@ $^ `mysql_config --cflags --libs`
	-@strip $@

%.o: %.c
	cc $(CFLAGS) -o $@ -c $<

clean:
	rm -f data_train_cpu *.o

install:

	rm -vf /usr/local/sbin/data_train_cpu
	cp data_train_cpu /usr/local/sbin
