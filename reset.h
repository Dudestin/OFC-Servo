#pragma once
#include "mem.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>
#include <errno.h>

int reset_setup();

void reset_exit();

void reset_sig_handler();

void reset_fclk0();

void reset_fclk1();
