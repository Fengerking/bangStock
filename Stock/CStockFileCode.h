/*******************************************************************************
	File:		CStockFileCode.h

	Contains:	the message manager class header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#ifndef __CStockFileCode_H__
#define __CStockFileCode_H__

#include "CStockFileBase.h"

class CStockFileCode : public CStockFileBase
{
public:
    CStockFileCode(void);
    virtual ~CStockFileCode(void);

	virtual int		Open (char * pFile);
	virtual int		GetCodeCount (void);
	virtual char *	GetCodeNum (int nIndex);
	virtual char *	GetCodeName (int nIndex);
	virtual char *	GetURLFormat (void);
	virtual int		SetCurSel (int nSel);
	virtual int		GetCurSel (void);
	virtual char *	GetSelCode (void);

protected:
	virtual void	ReleaseCode (void);

protected:
	char **			m_ppCodeNum;
	int				m_nCodeCount;
	char *			m_pURLFormat;
	int				m_nCurSel;

};

#endif //__CStockFileCode_H__
