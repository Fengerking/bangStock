/*******************************************************************************
	File:		UUrlParser.cpp

	Contains:	url parser implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2017-01-06		Bangfei			Create file

*******************************************************************************/
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "UUrlParser.h"

#define HTTP_PREFIX_LENGTH	7
#define HTTPS_PREFIX_LENGTH	8

static bool isSplashCharacter(char character) 
{
	return character == '/' || character == '\\';
}

static bool isDotCharacter(char character) 
{
	return character == '.';
}

static void upperCaseString(const char* head, const char* tail, char* upperCasedString) 
{
	if (head != NULL && tail != NULL)
	{
		do
		{
			*upperCasedString++ = toupper(*head++);
		}while (head < tail);
	}

	*upperCasedString = '\0';
}

static void copyString(const char* head, const char* tail, char* copiedString) 
{
	if (head != NULL && tail != NULL)
	{
		do
		{
			*copiedString++ = *head++;
		}while (head < tail);
	}

	*copiedString = '\0';
}

static const char* ptrToNearestDotFromStringTail(const char* head, const char* tail) 
{
	const char* ptr = tail;
	char character;

	do 
	{
		character = *(--ptr);
	} while (ptr >= head && !isSplashCharacter(character) && !isDotCharacter(character));
	ptr++;

	return character == '.' ? ptr : NULL;
}

static const char* ptrToNearestSplashFromStringTail(const char* head, const char* tail) 
{
	const char* ptr = tail;
	char character;

	do 
	{
		character = *(--ptr);
	} while (ptr >= head && !isSplashCharacter(character));
	ptr++;

	return ptr;
}

static const char* ptrToFirstQuestionMarkOrStringTail(const char* string) 
{
	const char* ptr = strchr(string, '?');
	if (ptr == NULL)
	{
		ptr = string + strlen(string);
	}

	return ptr;
}

static const char* ptrToExtHeadOrStringTail(const char* url) 
{
	const char* extTail = ptrToFirstQuestionMarkOrStringTail(url);

	const char* extHead = ptrToNearestDotFromStringTail(url, extTail);

	return (extHead != NULL) ? extHead - 1 : url + strlen(url);
}


void qcUrlParseProtocal(const char* aUrl, char* aProtocal)
{
	const char* protocalTail = strstr(aUrl, "://");
	int protocalLength = 0;
	if (protocalTail != NULL)
	{
		protocalLength = protocalTail - aUrl;
		memcpy(aProtocal, aUrl, protocalLength);
	}
	aProtocal[protocalLength] = '\0';
}

void qcUrlParseExtension(const char* aUrl, char* aExtension)
{
	const char* tail = ptrToFirstQuestionMarkOrStringTail(aUrl);

	const char* head = ptrToNearestDotFromStringTail(aUrl, tail);

	upperCaseString(head, tail, aExtension);

	// work around to remove "," in extension. For example,  MP3,1 -> MP3
	char* ptr = strchr(aExtension, ',');
	if (ptr != NULL) 
	{
		*ptr = '\0';
	}
}

void qcUrlParseShortName(const char* aUrl, char* aShortName)
{
	const char* shortNameTail = ptrToExtHeadOrStringTail(aUrl);

	const char* shortNameHead = ptrToNearestSplashFromStringTail(aUrl, shortNameTail);

	copyString(shortNameHead, shortNameTail, aShortName);
}

void qcUrlParseUrl(const char* aUrl, char* aHost, char* aPath, int& aPort) 
{
	// parse host
	char* hostHead = const_cast<char*> (aUrl);
	if (!strncmp(hostHead, "http://", HTTP_PREFIX_LENGTH))
	{
		hostHead += HTTP_PREFIX_LENGTH;
	}
	else if (!strncmp(hostHead, "https://", HTTPS_PREFIX_LENGTH))
	{
		hostHead += HTTPS_PREFIX_LENGTH;
	}

	char* urlTail = hostHead + strlen(hostHead);
	char* hostTail = strchr(hostHead, '/');
	if (hostTail == NULL)
	{
		hostTail = urlTail;
	}

	int hostLength = hostTail - hostHead;
	memcpy(aHost, hostHead, hostLength);
	aHost[hostLength] = '\0';
    
    char* charColon = strchr(aHost, ':');
    if (charColon) {
        *charColon++ = '\0';
        aPort = atoi(charColon);
    } else {
	      aPort = 80;
	}

	// parse path and port
	aPath[0] = '\0';
	if (hostTail < urlTail) 
	{
		char* pathHead = hostTail + 1;
		int pathLength = urlTail - pathHead;
		memcpy(aPath, pathHead, pathLength);
		aPath[pathLength] = '\0';
	}
}
