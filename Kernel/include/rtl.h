#ifndef __RTL_H
#define __RTL_H

#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <interruptions.h>
#include <drivers.h>
#include <ethernet.h>



void net_send(char *msg, char *mac);
void net_start();

#endif