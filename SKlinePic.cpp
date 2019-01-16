#include "stdafx.h"
#include "SKlinePic.h"


SKlinePic::SKlinePic()
{
	m_bFocusable = 1; //可以获取焦点
	m_nKWidth = K_WIDTH_TOTAL;

	m_nMouseX = m_nMouseY = 0;

	ZeroMemory(m_ztLine, sizeof(float) * MAX_DATA_COUNT * MAX_FLINE_COUNT);
	ZeroMemory(m_bShowZtLine, sizeof(bool)* MAX_FLINE_COUNT);

	ZeroMemory(m_Klines, sizeof(KLINE_INFO) * MAX_KLINE_COUNT);
	/*SYSTEMTIME st,sn;
	GetLocalTime(&st);
	LONGLONG llms = FormatSYSTEMTIME2Msecond(&st);
	srand((int)llms);		//随机数种子
	for(int nk = 0; nk < MAX_KLINE_COUNT; nk++)
	{
		m_Klines[nk].bShow = true;
		DATA_FOR_SHOW *p = m_Klines[nk].pd;
		for (int i = 0; i < 1000; i++, p++)
		{
			FormatMsecond2SYSTEMTIME(llms, &sn);
			llms += 1000;
			p->date = sn.wYear * 10000 + sn.wMonth * 100 + sn.wDay;
			p->time = (sn.wHour * 10000 + sn.wMinute * 100 + sn.wSecond) * 1000 + rand() % 1000;
			if (i == 0)
			{
				p->open = (float)(rand() % 10 + 1000);
				p->close = (float)(rand() % 10 + 1000);
				p->high = (float)((p->open > p->close ? p->open + rand() % 10 : p->close + rand() % 10));
				p->low = (float)((p->open < p->close ? p->open - rand() % 10 : p->close - rand() % 10));
			}
			else
			{
				DATA_FOR_SHOW *pm = p - 1;
				p->open = (float)(rand() % 11 + pm->close - 5);
				p->close = (float)(rand() % 21 + pm->close - 10);
				p->high = (float)((p->open > p->close ? p->open + rand() % 8 : p->close + rand() % 8));
				p->low = (float)((p->open < p->close ? p->open - rand() % 8 : p->close - rand() % 8));
			}	
		}
		m_nTotal = 1000;
		wcscpy_s(m_Klines[nk].sDecimal, 9, L"%.0f");
		m_Klines[nk].nDecimalXi = 1;
	}

	//生成线条
	m_bShowZtLine[0] = true;
	m_bShowZtLine[1] = true;
	for (int i=20;i<1000;i++)
	{
		double f1 = 0;
		for (int j=0;j<20;j++)
		{
			f1 += m_Klines[0].pd[i - j].close;
			if(j == 9)
				m_ztLine[1][i] = (float)(f1 / 10);
		}
		m_ztLine[0][i] = (float)(f1 / 20);
	}

	//生成附图
	ZeroMemory(&m_Futu, sizeof(FUTU_INFO));
	//生成线条
	m_Futu.bft[0] = 2;
	wcscpy_s(m_Futu.sDecimal, 9, L"%.0f");
	m_Futu.nDecimalXi = 1;
	for (int i = 0; i < 1000; i++)
	{
		m_Futu.ftl[0][i] = (float)(rand() % 50 * 2 + 100);
	}*/
}

SKlinePic::~SKlinePic()
{
}

void SKlinePic::OnPaint(IRenderTarget * pRT)
{
	SPainter pa;
	SWindow::BeforePaint(pRT,pa);
	SWindow::GetClientRect(&m_rcAll);
	m_rcAll.DeflateRect(RC_LEFT, RC_TOP, RC_RIGHT, RC_BOTTOM);
	m_rcUpper.SetRect(m_rcAll.left, m_rcAll.top, m_rcAll.right, m_rcAll.bottom - (m_rcAll.bottom - m_rcAll.top) /5);
	m_rcLower.SetRect(m_rcAll.left, m_rcAll.top + (m_rcAll.bottom - m_rcAll.top) * 4 / 5, m_rcAll.right, m_rcAll.bottom);
	if (SWindow::IsFocused())
	{
		CRect rcFocus = GetClientRect();
		rcFocus.DeflateRect(2, 2);
		DrawDefFocusRect(pRT, rcFocus);
	}
	for (int dk = 0; dk < MAX_KLINE_COUNT; dk++)
	{
		if (!m_Klines[dk].bShow)
			continue;
		GetMaxDiff(dk);
	}
	GetFuTuMaxDiff();
	DrawArrow(pRT);
	for (int dk = 0; dk < MAX_KLINE_COUNT; dk++)
	{
		if (!m_Klines[dk].bShow)
			continue;
		DrawData(pRT, dk);
	}
	AfterPaint(pRT,pa);
}

void SKlinePic::DrawArrow(IRenderTarget * pRT)
{
	//画k线区
	int nLen = m_rcUpper.bottom - m_rcUpper.top;
	int nYoNum = 3;		//y轴标示数量 3 代表画两根线
	if (nLen > 500)
		nYoNum = 4;
	CPoint pts[5];
	{
		CAutoRefPtr<IPen> pen, oldPen;
		pRT->CreatePen(PS_SOLID, RGBA(255, 0, 0, 0xFF), 1, &pen);
		pRT->SelectObject(pen, (IRenderObj**)&oldPen);
		//y轴	//x轴
		pts[0].SetPoint(m_rcUpper.left, m_rcUpper.top );
		pts[1].SetPoint(m_rcUpper.left, m_rcUpper.bottom );
		pts[2].SetPoint(m_rcUpper.right , m_rcUpper.bottom );
		pts[3].SetPoint(m_rcUpper.right , m_rcUpper.top );
		pts[4] = pts[0];
		pRT->DrawLines(pts, 5);
		pRT->SelectObject(oldPen);
	}
	pRT->SetTextColor(RGBA(255, 0, 0, 255));
	//	pRT->DrawText(L"你好dfsdfsdf", -1, m_rcFit, DT_SINGLELINE | DT_LEFT);
	//	pRT->TextOut(m_rcFit.left + 10, m_rcFit.top + 10, L"你好", 4);

	//k线区横向虚线
	COLORREF clRed = RGB(255, 0, 0);
	HDC pdc = pRT->GetDC();
	for (int i = 1; i < nYoNum; i++)
	{
		int nY = m_rcUpper.bottom - ((m_rcUpper.bottom - m_rcUpper.top) / nYoNum * i);
		for (int j = m_rcUpper.left+1; j < m_rcUpper.right; j += 3)
			::SetPixelV(pdc, j, nY, clRed);		//	划虚线
		//k线区y轴加轴标
		SStringW s1 = GetYPrice(nY,0);
		pRT->TextOut(m_rcUpper.left - RC_LEFT + 8, nY-6, s1, -1);
		if (m_Klines[1].bShow)
		{
			s1 = GetYPrice(nY, 1);
			pRT->TextOut(m_rcUpper.right + 8, nY - 6, s1, -1);
		}
	}
	pRT->ReleaseDC(pdc);

/*
	//k线区横轴位置所在y坐标
	int nlo = m_rcUpper.bottom / nYoNum * (nYoNum - 1) - 5 - m_rcUpper.bottom;
	if (nlo > 15 || nlo < -15)
	{
		SStringW strf1 = GetYPrice(m_rcUpper.bottom);
		pRT->TextOut(m_rcUpper.left -RC_LEFT + 8 , m_rcUpper.bottom - 5, strf1, -1);
	}*/

	//画指标区
	{
		CAutoRefPtr<IPen> pen, oldPen;
		pRT->CreatePen(PS_SOLID, RGBA(255, 0, 0, 0xFF), 1, &pen);
		pRT->SelectObject(pen, (IRenderObj**)&oldPen);
		//y轴	//x轴
		pts[0].SetPoint(m_rcLower.left, m_rcLower.top);
		pts[1].SetPoint(m_rcLower.left, m_rcLower.bottom);
		pts[2].SetPoint(m_rcLower.right, m_rcLower.bottom);
		pts[3].SetPoint(m_rcLower.right, m_rcLower.top);
		pts[4] = pts[0];
		pRT->DrawLines(pts, 4);
		pRT->SelectObject(oldPen);
	}

	//副图区横向虚线
	pdc = pRT->GetDC();
	for (int i = 1; i < 2; i++)
	{
		int nY = m_rcLower.bottom - ((m_rcLower.bottom - m_rcLower.top)  / 2 * i);
		for (int j = m_rcLower.left+1; j < m_rcLower.right; j += 3)
			::SetPixelV(pdc, j, nY, clRed);		//	划虚线

		//标注
		
		SStringW s1 = GetFuTuYPrice(nY);
		pRT->TextOut(m_rcLower.left - RC_LEFT + 8, nY -6, s1, -1);
	}
	pRT->ReleaseDC(pdc);
}

void SKlinePic::GetMaxDiff(DWORD dk)		//判断坐标最大最小值和k线条数
{
	if (dk >= MAX_KLINE_COUNT)
	{
		m_nFirst = m_nEnd = 0;
		m_Klines[dk].fMax = 1;
		m_Klines[dk].fMin = 0;
		return;
	}
	if(dk == 0)
	{
		//判断k线条数
		int nLen = m_rcUpper.right - m_rcUpper.left - 20;	//判断是否超出范围
		m_nMaxKNum = nLen / m_nKWidth;
		m_nFirst = 0;
		m_nEnd = m_nTotal;
		if (m_nTotal > m_nMaxKNum)
			m_nFirst = m_nTotal - m_nMaxKNum;

		//开始计算左右偏移(鼠标控制)
		if (m_nMove > 0)
		{
			if (m_nFirst <= 0 && m_nEnd == m_nTotal)
				m_nMove = 0;
			if (m_nTotal < m_nMove)
				m_nMove = m_nTotal;
			else if (m_nFirst > m_nMove)
			{
				m_nFirst -= m_nMove;
				m_nEnd = m_nTotal - m_nMove;
			}
			else
			{
				m_nEnd -= m_nFirst;
				m_nFirst = 0;
			}
		}
	}
	//判断最大最小值
	//	OutputDebugString("判断最大值\n");
	double fMax = -100000000000000;
	double fMin = 100000000000000;

	DATA_FOR_SHOW *p = m_Klines[dk].pd;
	for (int j = m_nFirst; j < m_nEnd; j++)
	{
		if (p[j].low < fMin)
			fMin = p[j].low;
		if (p[j].high > fMax)
			fMax = p[j].high;
	}
	//看指标中的数值大小
	if(dk == 0)
	{
		for (int nt=0;nt<MAX_FLINE_COUNT;nt++)
		{
			if(!m_bShowZtLine[nt])
				continue;
			float* pf = m_ztLine[nt];
			for (int j = m_nFirst; j < m_nEnd; j++)
			{
				if (pf[j] < fMin)
					fMin = pf[j];
				if (pf[j] > fMax)
					fMax = pf[j];
			}
		}
	}

	m_Klines[dk].fMax = fMax;
	m_Klines[dk].fMin = fMin;
	if (m_Klines[dk].fMax == fMin)
		m_Klines[dk].fMax = m_Klines[dk].fMax * 1.1;
	if (m_Klines[dk].fMax == 0)
		m_Klines[dk].fMax = 1;


	int nLen = m_rcUpper.bottom - m_rcUpper.top - RC_MAX - RC_MIN;
	double fDiff = 0;
	fDiff = m_Klines[dk].fMax - m_Klines[dk].fMin;
	fDiff *= m_Klines[dk].nDecimalXi;
	int nDiff = (int)fDiff;
	if (nDiff == 0)
	{
		nDiff = (int)(fDiff * m_Klines[dk].nDecimalXi);
		if (nDiff == 0)
		{
			nDiff = 1;
			OutputDebugString(L"nDiff异常!");
		}
	}
	m_Klines[dk].nDivY = 0;
	while (nDiff > nLen)
	{
		nDiff /= 10;
		m_Klines[dk].nDivY++;
	}
	m_Klines[dk].nMulY = nLen / nDiff;
}

void SKlinePic::GetFuTuMaxDiff()		//判断副图坐标最大最小值和k线条数
{
	//判断最大最小值
	//	OutputDebugString("判断最大值\n");
	double fMax = -100000000000000;
	double fMin = 100000000000000;

	for (int i=0;i<MAX_FLINE_COUNT;i++)
	{
		if(!m_Futu.bft)
			continue;
		float *p = m_Futu.ftl[i];
		for (int j = m_nFirst; j < m_nEnd; j++)
		{
			if (p[j] < fMin)
				fMin = p[j];
			if (p[j] > fMax)
				fMax = p[j];
		}
	}

	m_Futu.fMax = fMax;
	m_Futu.fMin = fMin;
	if (m_Futu.fMax == fMin)
		m_Futu.fMax = m_Futu.fMax * 1.1;
	if (m_Futu.fMax == 0)
		m_Futu.fMax = 1;


	int nLen = m_rcLower.bottom - m_rcLower.top - RC_MAX - RC_MIN;
	double fDiff = 0;
	fDiff = m_Futu.fMax - m_Futu.fMin;
	fDiff *= m_Futu.nDecimalXi;
	int nDiff = (int)fDiff;
	if (nDiff == 0)
	{
		nDiff = (int)(fDiff * m_Futu.nDecimalXi);
		if (nDiff == 0)
		{
			nDiff = 1;
			OutputDebugString(L"nDiff异常!");
		}
	}
	m_Futu.nDivY = 0;
	while (nDiff > nLen)
	{
		nDiff /= 10;
		m_Futu.nDivY++;
	}
	m_Futu.nMulY = nLen / nDiff;
}

BOOL SKlinePic::IsInRect(int x, int y,int nMode)
{
	CRect *prc;
	switch (nMode)
	{
	case 0:
		prc = &m_rcAll;
		break;
	case 1:
		prc = &m_rcUpper;
		break;
	case 2:
		prc = &m_rcLower;
		break;
	default:
		return FALSE;
	}
	if (x >= prc->left && x <= prc->right &&
		y >= prc->top  && y <= prc->bottom )
		return TRUE;
	return FALSE;
}

int SKlinePic::GetFuTuYPos(double fDiff)	//获得附图y位置
{
	double fPos = fDiff - m_Futu.fMin;
	fPos *= m_Futu.nDecimalXi;
	int nPos = (int)fPos;
	for (int i = 0; i < m_Futu.nDivY; i++)
	{
		nPos /= 10;
	}
	nPos *= m_Futu.nMulY;
	nPos = m_rcLower.bottom - nPos - RC_MIN;
	return nPos;
}

SStringW SKlinePic::GetFuTuYPrice(int nY)
{
	SStringW strRet; strRet.Empty();
	if (nY > m_rcLower.bottom || nY < m_rcLower.top)
		return strRet;
	int nDiff = m_rcLower.bottom - RC_MIN - nY;
	double fDiff = nDiff;
	if (m_Futu.nMulY > 0)
		fDiff /= m_Futu.nMulY;
	for (int i = 0; i < m_Futu.nDivY; i++)
		fDiff *= 10;
	fDiff /= m_Futu.nDecimalXi;
	fDiff = fDiff + m_Futu.fMin;
	strRet.Format(m_Futu.sDecimal, fDiff);
	return strRet;
}

int SKlinePic::GetYPos(double fDiff,DWORD dk)
{
	double fPos = fDiff - m_Klines[dk].fMin;
	fPos *= m_Klines[dk].nDecimalXi;
	int nPos = (int)fPos;
	for (int i = 0; i < m_Klines[dk].nDivY; i++)
	{
		nPos /= 10;
	}
	nPos *= m_Klines[dk].nMulY;
	nPos = m_rcUpper.bottom - nPos - RC_MIN;
	return nPos;
}

SStringW SKlinePic::GetYPrice(int nY, DWORD dk)
{
	SStringW strRet; strRet.Empty();
	if (dk >= MAX_KLINE_COUNT)
		return strRet;
	int nDiff = m_rcUpper.bottom - RC_MIN - nY;
	double fDiff = nDiff;
	if (m_Klines[dk].nMulY > 0)
		fDiff /= m_Klines[dk].nMulY;
	for (int i = 0; i < m_Klines[dk].nDivY; i++)
		fDiff *= 10;
	fDiff /= m_Klines[dk].nDecimalXi;
	fDiff = fDiff + m_Klines[dk].fMin;
	strRet.Format(m_Klines[dk].sDecimal, fDiff);
	return strRet;
}

void SOUI::SKlinePic::OnLButtonDown(UINT nFlags, CPoint point)
{
//	OutputDebugString(L"1");
//	SWindow::SetTimer(1, 16);
}

int SOUI::SKlinePic::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SWindow::SetTimer(1, 16);
	return 0;
}

int SKlinePic::GetXData(int nx) {	//获取鼠标下的数据id
	int n = (nx - m_rcUpper.left) / m_nKWidth;
	if (n < 0)
		n = 0;
	n += m_nFirst;
	return n;
}

void SKlinePic::DrawData(IRenderTarget * pRT, DWORD dk)
{
	if (dk >= MAX_KLINE_COUNT)
		return;
	CPoint pts[5];
	int x = 0, yopen = 0, yclose = 0, yhigh = 0, ylow = 0;
	CAutoRefPtr<IPen> penRed,penGreen,penWhite, oldPen;
	CAutoRefPtr<IBrush> bBrush, bOldBrush;
	if(dk == 0)
	{
		pRT->CreatePen(PS_SOLID, RGBA(255, 0, 0, 200), 1, &penRed);
		pRT->CreatePen(PS_SOLID, RGBA(0, 255, 255, 200), 1, &penGreen);
		pRT->CreatePen(PS_SOLID, RGBA(255, 255, 255, 200), 1, &penWhite);
		pRT->CreateSolidColorBrush(RGBA(0, 255, 255, 200), &bBrush);
	}
	else
	{
		//176, 196, 222
		COLORREF cl = RGBA(176, 196, 222, 100);
		pRT->CreatePen(PS_SOLID, cl, 1, &penRed);
		pRT->CreatePen(PS_SOLID, cl, 1, &penGreen);
		pRT->CreatePen(PS_SOLID, cl, 1, &penWhite);
		pRT->CreateSolidColorBrush(cl, &bBrush);
	}
	pRT->SelectObject(penRed, (IRenderObj**)&oldPen);
	pRT->SelectObject(bBrush, (IRenderObj**)&bOldBrush);

	DATA_FOR_SHOW *p = m_Klines[dk].pd + m_nFirst;
	for (int i = 0; i < m_nEnd - m_nFirst; i++)
	{
		x = i * m_nKWidth + 1 + m_rcUpper.left;
		int nTmpWidth = m_nKWidth / 4;
		yopen	= GetYPos(p[i].open,dk);
		yhigh	= GetYPos(p[i].high, dk);
		ylow	= GetYPos(p[i].low, dk);
		yclose	= GetYPos(p[i].close, dk);

		//加数值
		int nTimeTmp = 10 + 10 * m_nCyc;
		if (((i + 1) % nTimeTmp == 0 && m_nEnd - m_nFirst - i > 10 && x < m_rcUpper.right - 50) || i == 0 || i == m_nEnd - m_nFirst - 1)
		{

			//加最后的数值
			if (i == m_nEnd - m_nFirst - 1)
			{
				SStringW strTemp;
				strTemp.Format(m_Klines[dk].sDecimal, p[i].close);
				pRT->TextOut(x + 20, yclose - 5, strTemp,-1);
			//	if(dk == 0)
			//		pRT->TextOut(m_rcUpper.left - RC_LEFT + 8, m_rcUpper.top - RC_TOP + 10, strTemp, -1);
			}
		}

		if (p[i].close > p[i].open)
		{
			pRT->SelectObject(penRed);
			pts[0].SetPoint(x + m_nKWidth / 2, yclose);
			pts[1].SetPoint(x + m_nKWidth / 2, yhigh);
			pts[2].SetPoint(x + m_nKWidth / 2, yopen);
			pts[3].SetPoint(x + m_nKWidth / 2, ylow);
		}

		else if (p[i].close < p[i].open)
		{
			pRT->SelectObject(penGreen);
			pts[0].SetPoint(x + m_nKWidth / 2, yopen);
			pts[1].SetPoint(x + m_nKWidth / 2, yhigh);
			pts[2].SetPoint(x + m_nKWidth / 2, yclose);
			pts[3].SetPoint(x + m_nKWidth / 2, ylow);
		}
		else
		{
			pRT->SelectObject(penWhite);
			pts[0].SetPoint(x + m_nKWidth / 2, yopen);
			pts[1].SetPoint(x + m_nKWidth / 2, yhigh);
			pts[2].SetPoint(x + m_nKWidth / 2, yclose);
			pts[3].SetPoint(x + m_nKWidth / 2, ylow);
		}

		pRT->DrawLines(pts, 2);
		pRT->DrawLines(pts+2, 2);

		if (p[i].close == p[i].open)
		{
			pts[0].SetPoint(x + 2, yopen);
			pts[1].SetPoint(x + m_nKWidth -1, yopen);
			pRT->DrawLines(pts, 2);
		}
		else
		{
			if (p[i].close > p[i].open)
				pRT->DrawRectangle(CRect(x + 2, yopen, x + m_nKWidth - 2, yclose));
			else
				pRT->FillRectangle(CRect(x + 2, yopen, x + m_nKWidth , yclose));
		}

		//画附图
		if (dk == 0)
		{
			for (int nt = 0; nt < MAX_FLINE_COUNT; nt++)
			{
				if (!m_Futu.bft[nt])
					continue;
				if (m_Futu.bft[nt] == 1 && i > 0)
				{
					pRT->SelectObject(penWhite);
					pts[0].SetPoint(x + m_nKWidth / 2 - m_nKWidth, GetFuTuYPos(m_Futu.ftl[nt][m_nFirst + i - 1]));
					pts[1].SetPoint(x + m_nKWidth / 2, GetFuTuYPos(m_Futu.ftl[nt][m_nFirst + i]));
					pRT->DrawLines(pts, 2);
				}
				else if (m_Futu.bft[nt] == 2)
				{
					if (p[i].close > p[i].open)
						pRT->DrawRectangle(CRect(x + 2, GetFuTuYPos(m_Futu.ftl[nt][m_nFirst + i]), x + m_nKWidth - 2, m_rcLower.bottom));
					else
						pRT->FillRectangle(CRect(x + 2, GetFuTuYPos(m_Futu.ftl[nt][m_nFirst + i]), x + m_nKWidth, m_rcLower.bottom));
				}
				else if (m_Futu.bft[nt] == 3)
				{

				}
			}
		}

		//画指标
		if (dk == 0 && i > 0)
		{
			for (int nt = 0;nt < MAX_FLINE_COUNT; nt++)
			{
				if(!m_bShowZtLine[nt])
					continue;
				pRT->SelectObject(penWhite);
				pts[0].SetPoint(x + m_nKWidth / 2 - m_nKWidth, GetYPos(m_ztLine[nt][m_nFirst + i-1],dk));
				pts[1].SetPoint(x + m_nKWidth / 2, GetYPos( m_ztLine[nt][m_nFirst + i], dk));
				pRT->DrawLines(pts, 2);
			}
		}
	}
	pRT->SelectObject(oldPen);
	pRT->SelectObject(bOldBrush);
}