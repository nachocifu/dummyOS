#ifndef __NET_H
#define __NET_H

#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <interruptions.h>
#include <drivers.h>
#include <ethernet.h>


void net_start();
void net_send(char *msg);
int net_read(char *b);

#endif