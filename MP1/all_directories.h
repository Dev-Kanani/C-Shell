#ifndef ALL_DIRECTORIES_H
#define ALL_DIRECTORIES_H

#define _POSIX_C_SOURCE 199309L
#include <signal.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <grp.h>
#include <string.h>
#include "log.h"
#include "queue.h"
#include "reveal.h"
#include "foreground.h"
#include "parse_command.h"
#include "proclore.h"
#include "seek.h"

extern int job_number;
extern char LOG_FILE_PATH[4096];

#define MAX_SIZE_OF_NAMES 1000
#define MAX_SIZE 1000

#endif