
CFLAGS ?= -g -Wall -Werror
LDFLAGS = `mysql_config --cflags --libs`

all: data_train_nic 

data_train_nic: data_train_nic.o
	cc -o $@ $^ $(LDFLAGS)
	-@strip $@

%.o: %.c
	cc $(CFLAGS) -o $@ -c $<

clean:
	rm -f data_train_nic *.o

install:
	rm -vf /usr/local/sbin/data_train_nic
	cp data_train_nic /usr/local/sbin
