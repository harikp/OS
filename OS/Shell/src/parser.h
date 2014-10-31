/*
 * parser.h
 *
 *  Created on: 31-Oct-2014
 *      Author: Harikrishnan.P
 */

#ifndef PARSER_H_
#define PARSER_H_

typedef struct command_t
{
	char **argv;
	int argc;
	char *infile;
	char *outfile;
	int internal;
} command;

#define MAXWORDS	(1024)
#define MAXCOMMANDS	(MAXWORDS / 2 + 1)
#define MAXLINELEN	MAXWORDS

int parse_commands(const char *line, command comLine[]);

#endif /* PARSER_H_ */
