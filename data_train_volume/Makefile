
CFLAGS ?= -g -Wall -Werror

all: data_train_volume 

data_train_volume: data_train_volume.o
	cc -o $@ $^ `mysql_config --cflags --libs`
	-@strip $@

%.o: %.c
	cc $(CFLAGS) -o $@ -c $<

clean:
	rm -f data_train_volume *.o

install:
	rm -vf /usr/local/sbin/data_train_volume
	cp data_train_volume /usr/local/sbin
