#Make file for usbshare-server

SHELL=/bin/bash

TARGET = ./build/usbshare-server
CFILE1 = ./src/usbshare_server.c 
CFILE2 = ./src/usbshare_server_func.c 
CFILE3 = ./src/server_net_func.c
OBJECT1 = ./build/usbshare_server.o 
OBJECT2 = ./build/usbshare_server_func.o
OBJECT3 = ./build/server_net_func.o

#header file
HFILE = ./inc/usbshare_server.h

#compiler
CC = gcc

#installation directories
DESTDIR = /usr/bin

#dependencies & rules

all: $(TARGET)

$(TARGET): $(HFILE) $(OBJECT1) $(OBJECT2) $(OBJECT3)
	$(CC) -o $(TARGET) $(OBJECT1) $(OBJECT2) $(OBJECT3)

$(OBJECT1): $(HFILE) $(CFILE1)
	$(CC) -o $(OBJECT1) -c $(CFILE1)

$(OBJECT2): $(HFILE) $(CFILE2)
	$(CC) -o $(OBJECT2) -c $(CFILE2)

$(OBJECT3): $(HFILE) $(CFILE3)
	$(CC) -o $(OBJECT3) -c $(CFILE3)

clean:
	rm -f $(TARGET) $(OBJECT1) $(OBJECT2) $(OBJECT3)

install:

	install -m 755 $(TARGET) $(DESTDIR)
