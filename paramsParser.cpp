/*
 * paramsParser.cpp
 *
 *  Created on: Oct 16, 2017
 *      Author: tobias
 */
#include "paramsParser.h"
#include <stdio.h>
#include <regex.h>
#include <string>
#include <string.h>

bool parseIp(char* argv, char* ip)
{
	//2[0-4][0-9]|25[0-5]|[0-1][0-9][0-9]
	const char* textRegex = std::string("^([0-9]{1,3}\\.){3}[0-9]{1,3}$").c_str();
	regex_t regex;
	int result;
	bool boolResult = false;
	char msgbuf[100];

	/* Compile regular expression */
	result = regcomp(&regex,textRegex, 0);
	if ( result != 0 ) {
		printf("error compilando regex fpara ip ip\n");
	    return boolResult;
	}

	/* Execute regular expression */
	result = regexec(&regex, argv, 0, NULL, 0);
	if ( result == 0 ) {
	    boolResult = true;
	} else if (result != 0) {
	    regerror(result, &regex, msgbuf, sizeof(msgbuf));
	    fprintf(stderr, "Regex match failed: %s\n", msgbuf);
	}

	regfree(&regex);
	memcpy(ip,argv,strlen(argv));
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
