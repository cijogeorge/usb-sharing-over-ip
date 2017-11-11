#Make file for usbshare-client

SHELL=/bin/bash

TARGET = ./build/usbshare-client
CFILE1 = ./src/usbshare_client.c 
CFILE2 = ./src/usbshare_client_func.c 
CFILE3 = ./src/client_net_func.c
OBJECT1 = ./build/usbshare_client.o 
OBJECT2 = ./build/usbshare_client_func.o
OBJECT3 = ./build/client_net_func.o

#header file
HFILE = ./inc/usbshare_client.h

#compiler
CC = gcc

#installation directories
DESTDIR = /usr/bin

#dependencies & rules

all: $(TARGET)

$(TARGET): $(HFILE) $(OBJECT1) $(OBJECT2) $(OBJECT3)
	$(CC) -o $(TARGET) $(OBJECT1) $(OBJECT2) $(OBJECT3)

$(OBJECT1): $(CFILE1)
	$(CC) -o $(OBJECT1) -c $(CFILE1)

$(OBJECT2): $(HFILE) $(CFILE2)
	$(CC) -o $(OBJECT2) -c $(CFILE2)

$(OBJECT3): $(HFILE) $(CFILE3)
	$(CC) -o $(OBJECT3) -c $(CFILE3)

clean:
	rm -f $(TARGET) $(OBJECT1) $(OBJECT2) $(OBJECT3)

install:

	install -m 755 $(TARGET) $(DESTDIR)
