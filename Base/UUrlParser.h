/*******************************************************************************
	File:		UUrlParser.h

	Contains:	URL parser header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2017-01-06		Bangfei			Create file

*******************************************************************************/

/**
* \fn                       void ParseProtocal(const char* aUrl, char* aProtocal);
* \brief                    根据Url前缀，解析协议名。
* \param[in]	aUrl		Url
* \param[out]	aProtocal	协议名(没有协议名时，返回空字符)
*/
void					qcUrlParseProtocal(const char* aUrl, char* aProtocal);

/**
* \fn                       void ParseExtension(const char* aUrl, char* aExtension);
* \brief                    解析Url后缀名。
* \param[in]	aUrl		Url
* \param[out]	aExtension	后缀名(没有后缀时，返回空字符)
*/
void					qcUrlParseExtension(const char* aUrl, char* aExtension);

/**
* \fn                       void ParseShortName(const char* aUrl, char* aShortName);
* \brief                    解析Url路径中的short name。例如: 输入http://www.google.com/download/1.mp3 --> 输出1.mp3
* \param[in]	aUrl		Url
* \param[out]	aShortName	Short name.
*/
void					qcUrlParseShortName(const char* aUrl, char* aShortName);

/**
* \fn                       void ParseUrl(const char* aUrl, char* aHost, char* aPath, int& aPort);
* \brief                    从Url中解析出主机名，路径和端口号。
* \param[in]	aUrl		Url
* \param[out]	aHost		主机名
* \param[out]	aPath		路径
* \param[out]	aPort		端口号
*/
void					qcUrlParseUrl(const char* aUrl, char* aHost, char* aPath, int& aPort);

