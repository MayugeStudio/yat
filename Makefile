all: yat da_test

yat: yat.c
	gcc -o yat yat.c -I./thirdparty -ggdb

da_test: thirdparty/da.h thirdparty/da_test.c
	gcc -o da_test thirdparty/da_test.c -I./thirdparty -ggdb

