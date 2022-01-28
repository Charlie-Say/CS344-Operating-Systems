/*
 * Assignment 3: smallsh (Portfolio Assignment)
 * By: Charlie Say
 * sayc@oregonstate.edu
 * CS 344 Operating Systems
 * 01/28/2022
 * smallsh.c
 *
 * Requirements:
 * - Provide a prompt for running commands
 * - Handle blank lines and comments, which are lines beginning with the # character
 * - Provide expansion for the variable $$
 * - Execute 3 commands exit, cd, and status via code built into the shell
 * - Execute other commands by creating new processes using a function from the exec family of functions
 * - Support input and output redirection
 * - Support running commands in foreground and background processes
 * - Implement custom handlers for 2 signals, SIGINT and SIGTSTP
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_CMD 2048
#define MAX_ARGS 512


