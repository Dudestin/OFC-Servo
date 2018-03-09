#pragma once
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

extern int mem_fd;

int mem_setup();
int mem_exit();
void mem_sig_handler();
