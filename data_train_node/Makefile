
CFLAGS ?= -g -Wall -Werror

all: data_train_node 

data_train_node: data_train_node.o
	gcc -o $@ $^ `mysql_config --cflags --libs`
	-@strip $@
	
%.o: %.c
	gcc $(CFLAGS) -o $@ -c $<

clean:
	rm -f data_train_node *.o

install:
	rm -vf /usr/local/sbin/data_train_node
	cp data_train_node /usr/local/sbin
