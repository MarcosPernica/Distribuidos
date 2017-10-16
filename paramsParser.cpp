/*
 * paramsParser.cpp
 *
 *  Created on: Oct 16, 2017
 *      Author: tobias
 */
#include "paramsParser.h"
#include <stdio.h>
#include <regex.h>


bool parseIp(char* argv, char* ip)
{
	//2[0-4][0-9]|25[0-5]|[0-1][0-9][0-9]
	char* textRegex = "^([0-9]{1,3}\.){3}[0-9]{1,3}$";
	regex_t regex;
	int result;
	bool boolResult = false;
	char msgbuf[100];

	/* Compile regular expression */
	result = regcomp(&regex,textRegex, 0);
	if ( result ) {
		printf("error compilando regex fpara ip ip\n");
	    return boolResult;
	}

	/* Execute regular expression */
	result = regexec(&regex, argv, 0, NULL, 0);
	if ( !result ) {
	    boolResult = true;
	} else if (result != REG_NOMATCH) {
	    regerror(result, &regex, msgbuf, sizeof(msgbuf));
	    fprintf(stderr, "Regex match failed: %s\n", msgbuf);
	}

	regfree(&regex);
	return boolResult;
}

bool parsePort(char* argv,int* port)
{
	int myPort;
	if( sscanf(argv,"%i",&myPort) == EOF )
	{
		return false;
	}
	*port = myPort;
	return true;
}
