/*******************************************************************************
	File:		UMsgMng.cpp

	Contains:	The message manager implement file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#include "qcErr.h"
#include "UMsgMng.h"

#include "CMsgMng.h"

CMsgMng * qc_pMsgMng = NULL;

int	QCMSG_Init (void)
{
	if (qc_pMsgMng != NULL)
		return QC_ERR_NONE;
	qc_pMsgMng = new CMsgMng ();
	return QC_ERR_NONE;
}

int	QCMSG_Close (void)
{
	QC_DEL_P (qc_pMsgMng);
	return QC_ERR_NONE;
}

int	QCMSG_RegNotify (CMsgReceiver * pReceiver)
{
	if (qc_pMsgMng == NULL)
		return QC_ERR_IMPLEMENT;
	return qc_pMsgMng->RegNotify (pReceiver);
}

int	QCMSG_RemNotify (CMsgReceiver * pReceiver)
{
	if (qc_pMsgMng == NULL)
		return QC_ERR_IMPLEMENT;
	return qc_pMsgMng->RemNotify (pReceiver);
}

int	QCMSG_Notify (int nMsg, int nValue, long long llValue)
{
	if (qc_pMsgMng == NULL)
		return QC_ERR_IMPLEMENT;
	return qc_pMsgMng->Notify (nMsg, nValue, llValue);
}

int	QCMSG_Notify (int nMsg, int nValue, long long llValue, const char * pValue)
{
	if (qc_pMsgMng == NULL)
		return QC_ERR_IMPLEMENT;
	return qc_pMsgMng->Notify (nMsg, nValue, llValue, pValue);
}

int	QCMSG_Notify (int nMsg, int nValue, long long llValue, const char * pValue, void * pInfo)
{
	if (qc_pMsgMng == NULL)
		return QC_ERR_IMPLEMENT;
	return qc_pMsgMng->Notify (nMsg, nValue, llValue, pValue, pInfo);
}

int	QCMSG_Send (int nMsg, int nValue, long long llValue)
{
	if (qc_pMsgMng == NULL)
		return QC_ERR_IMPLEMENT;
	return qc_pMsgMng->Send (nMsg, nValue, llValue);
}

int	QCMSG_Send (int nMsg, int nValue, long long llValue, const char * pValue)
{
	if (qc_pMsgMng == NULL)
		return QC_ERR_IMPLEMENT;
	return qc_pMsgMng->Send (nMsg, nValue, llValue, pValue);
}

int	QCMSG_Send (int nMsg, int nValue, long long llValue, const char * pValue, void * pInfo)
{
	if (qc_pMsgMng == NULL)
		return QC_ERR_IMPLEMENT;
	return qc_pMsgMng->Send (nMsg, nValue, llValue, pValue, pInfo);
}

int	QCMSG_ConvertName (int nMsg, char * pName, int nSize)
{
	switch (nMsg)
	{
	case QC_MSG_HTTP_CONNECT_START:
		strcpy (pName, "QC_MSG_HTTP_CONNECT_START");
		break;
	case QC_MSG_HTTP_CONNECT_FAILED:
		strcpy (pName, "QC_MSG_HTTP_CONNECT_FAILED");
		break;
	case QC_MSG_HTTP_CONNECT_SUCESS:
		strcpy (pName, "QC_MSG_HTTP_CONNECT_SUCESS");
		break;
	case QC_MSG_HTTP_DNS_START:	
		strcpy (pName, "QC_MSG_HTTP_DNS_START");
		break;
	case QC_MSG_HTTP_DNS_GET_CACHE:
		strcpy (pName, "QC_MSG_HTTP_DNS_GET_CACHE");
		break;
	case QC_MSG_HTTP_DNS_GET_IPADDR:
		strcpy (pName, "QC_MSG_HTTP_DNS_GET_IPADDR");
		break;
	case QC_MSG_HTTP_GET_HEADDATA:
		strcpy (pName, "QC_MSG_HTTP_GET_HEADDATA");
		break;
	case QC_MSG_HTTP_CONTENT_LEN:
		strcpy (pName, "QC_MSG_HTTP_CONTENT_LEN");
		break;
	case QC_MSG_HTTP_REDIRECT:
		strcpy (pName, "QC_MSG_HTTP_REDIRECT");
		break;
	case QC_MSG_HTTP_DISCONNECT_START:
		strcpy (pName, "QC_MSG_HTTP_DISCONNECT_START");
		break;
	case QC_MSG_HTTP_DISCONNECT_DONE:
		strcpy (pName, "QC_MSG_HTTP_DISCONNECT_DONE");
		break;
	case QC_MSG_HTTP_DOWNLOAD_SPEED:
		strcpy (pName, "QC_MSG_HTTP_DOWNLOAD_SPEED");
		break;

	case QC_MSG_PARSER_NEW_STREAM:
		strcpy (pName, "QC_MSG_PARSER_NEW_STREAM");
		break;

	case QC_MSG_SNKA_FIRST_FRAME:
		strcpy (pName, "QC_MSG_SNKA_FIRST_FRAME");
		break;
	case QC_MSG_SNKA_EOS:
		strcpy (pName, "QC_MSG_SNK_AUDIO_EOS");
		break;
	case QC_MSG_SNKV_FIRST_FRAME:
		strcpy (pName, "QC_MSG_SNKV_FIRST_FRAME");
		break;
	case QC_MSG_SNKV_EOS:
		strcpy (pName, "QC_MSG_SNKV_EOS");
		break;
	case QC_MSG_SNKV_NEW_FORMAT:
		strcpy (pName, "QC_MSG_SNKV_NEW_FORMAT");
		break;
	case QC_MSG_PLAY_OPEN_DONE:
		strcpy (pName, "QC_MSG_PLAY_OPEN_DONE");
		break;
	case QC_MSG_PLAY_OPEN_FAILED:
		strcpy (pName, "QC_MSG_PLAY_OPEN_FAILED");
		break;
	case QC_MSG_PLAY_CLOSE_DONE:
		strcpy (pName, "QC_MSG_PLAY_CLOSE_DONE");
		break;
	case QC_MSG_PLAY_CLOSE_FAILED:
		strcpy (pName, "QC_MSG_PLAY_CLOSE_FAILED");
		break;
	case QC_MSG_PLAY_SEEK_DONE:
		strcpy (pName, "QC_MSG_PLAY_SEEK_DONE");
		break;
	case QC_MSG_PLAY_SEEK_FAILED:
		strcpy (pName, "QC_MSG_PLAY_SEEK_FAILED");
		break;
	case QC_MSG_PLAY_COMPLETE:
		strcpy (pName, "QC_MSG_PLAY_COMPLETE");
		break;
	case QC_MSG_PLAY_STATUS:
		strcpy (pName, "QC_MSG_PLAY_STATUS");
		break;

	case QC_MSG_BUFF_VBUFFTIME:
		strcpy (pName, "QC_MSG_BUFF_VIDEO_TIME");
		break;
	case QC_MSG_BUFF_ABUFFTIME:
		strcpy (pName, "QC_MSG_BUFF_AUDIO_TIME");
		break;
	case QC_MSG_BUFF_BUFFERING:
		strcpy (pName, "QC_MSG_BUFF_BUFFERING");
		break;
	case QC_MSG_BUFF_NEWSTREAM:
		strcpy (pName, "QC_MSG_BUFF_NEWSTREAM");
		break;

	case QC_MSG_LOG_TEXT:
		strcpy (pName, "QC_MSG_LOG_TEXT");
		break;

	default:
		sprintf (pName, "Unknow id % 8X", nMsg);
		break;
	}
	return 0;
}


void * QCMSG_InfoClone (int nMsg, void * pInfo)
{
	void * pNewInfo = NULL;
	switch (nMsg)
	{
	case QC_MOD_IO_HTTP:
		break;

	case QC_MOD_IO_RTMP:
		break;

	case QC_MOD_PARSER_MP4:
		break;

	case QC_MOD_PARSER_FLV:
		break;

	case QC_MOD_PARSER_TS:
		break;

	case QC_MOD_PARSER_M3U8:
		break;

	case QC_MOD_AUDIO_DEC_AAC:
		break;

	case QC_MOD_VIDEO_DEC_H264:
		break;

	case QC_MOD_SINK_AUDIO:
		break;

	case QC_MOD_SINK_VIDEO:
		break;

	case QC_MOD_SINK_DATA:
		break;

	case QC_MOD_MFW_PLAY:
		break;

	default:
		break;
	}

	return pNewInfo;
}	

int	QCMSG_InfoRelase (int nMsg, void * pInfo)
{
	switch (nMsg)
	{
	case QC_MOD_IO_HTTP:
		break;

	case QC_MOD_IO_RTMP:
		break;

	case QC_MOD_PARSER_MP4:
		break;

	case QC_MOD_PARSER_FLV:
		break;

	case QC_MOD_PARSER_TS:
		break;

	case QC_MOD_PARSER_M3U8:
		break;

	case QC_MOD_AUDIO_DEC_AAC:
		break;

	case QC_MOD_VIDEO_DEC_H264:
		break;

	case QC_MOD_SINK_AUDIO:
		break;

	case QC_MOD_SINK_VIDEO:
		break;

	case QC_MOD_SINK_DATA:
		break;

	case QC_MOD_MFW_PLAY:
		break;

	default:
		break;
	}
	return QC_ERR_NONE;
}
