/*******************************************************************************
	File:		CStockRTInfo.h

	Contains:	the message manager class header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#ifndef __CStockRTInfo_H__
#define __CStockRTInfo_H__

#include "qcStock.h"
#include "CStockBase.h"

class CStockRTInfo : public CStockBase
{
public:
    CStockRTInfo(void);
    virtual ~CStockRTInfo(void);

	virtual int		Update (void);

protected:
	virtual int		FillPath (void);
	virtual void	ReleaseData (void);

public:
	qcStockRealTimeItem					m_rtItem;
	CObjectList<qcStockIndexInfoItem>	m_lstIndexItem;
};

#endif //__CStockRTInfo_H__

/*
            todayMax:18.02
            tradeNum:5241000
            circulation_value:276.91
            all_value:276.91
            time:15:00:00
            turnover:0.338%
            tradeAmount:93910000
            swing:1.06
            diff_rate:-0.28
            max52:
            diff_money:-0.05
            nowPrice:17.88
            min52:
            pe:61.84
            pb:3.55
*/

/*
      {
        "yestodayClosePrice": "3268.9354",
        "max52": "0",
        "diff_money": "-31.4883",
        "tradeNum": "200583223",
        "code": "sh000001",
        "maxPrice": "3274.1903",
        "nowPrice": "3237.4471",
        "min52": "0",
        "time": "2017-03-17 15:01:03",
        "name": "上证指数",
        "tradeAmount": "262184046475",
        "swing": "1.2821",
        "todayOpenPrice": "3271.8665",
        "diff_rate": "-0.9633",
        "minPrice": "3232.2806"
      }
*/