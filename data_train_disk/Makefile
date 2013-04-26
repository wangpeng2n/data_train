

CFLAGS ?= -g -Wall -Werror
LDFLAGS = `mysql_config --cflags --libs`

all: data_train_disk 

data_train_disk: data_train_disk.o
	cc -o $@ $^ $(LDFLAGS)
	-@strip $@
	
%.o: %.c
	cc $(CFLAGS) -o $@ -c $<

clean:
	rm -f data_train_disk *.o

install:
	rm -vf /usr/local/sbin/data_train_disk
	cp data_train_disk /usr/local/sbin
