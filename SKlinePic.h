#ifndef _SKLINE_PIC
#define _SKLINE_PIC

#pragma once
#include <core/swnd.h>

#define MAX_KLINE_COUNT		2
#define MAX_FLINE_COUNT		5
#define MAX_DATA_COUNT		5000

#define MOVE_ONESTEP		10		//每次平移的数据量
#define K_WIDTH_TOTAL		16		//k线占用总宽度(在x轴上)
#define UM_QUERY	(WM_USER + 321)
//一些基本的界面框架信息
#define RC_LEFT		40
#define RC_RIGHT	40
#define RC_TOP		25
#define RC_BOTTOM	25

#define RC_MAX		5
#define RC_MIN		5

typedef struct {
	DWORD	date;
	DWORD	time;
	float	open;
	float	high;
	float	low;
	float	close;
}DATA_FOR_SHOW;

typedef struct _KLINE_INFO {
	DATA_FOR_SHOW	pd[MAX_DATA_COUNT];	//data信息
	bool			bShow;				//是否显示

	double			fMax;
	double			fMin;
	int				nDivY;				//y轴显示系数1
	int				nMulY;				//y轴显示系数2
	int				nDecimal;			//小数位数
	TCHAR			sDecimal[10];		//用来格式化
	int				nDecimalXi;			//10的m_nDecimal次方
}KLINE_INFO;

typedef struct _FUTU_INFO {
	float			ftl[MAX_FLINE_COUNT][MAX_DATA_COUNT];	//副图线
	BYTE			bft[MAX_FLINE_COUNT];					//显示方式 0为不显示,1为折线图,2为柱状图,3竖线图
	COLORREF		clft[MAX_FLINE_COUNT];					//

	double			fMax;
	double			fMin;
	int				nDivY;				//y轴显示系数1
	int				nMulY;				//y轴显示系数2
	int				nDecimal;			//小数位数
	TCHAR			sDecimal[10];		//用来格式化
	int				nDecimalXi;			//10的m_nDecimal次方
}FUTU_INFO;

namespace SOUI
{

	class SKlinePic : public SWindow
	{
		SOUI_CLASS_NAME(SKlinePic, L"klinePic")	//定义xml标签

	public:
		SKlinePic();
		~SKlinePic();

		CRect       m_rcAll;		//上下框相加
		CRect		m_rcUpper;		//上框坐标,K线
		CRect		m_rcLower;		//下框坐标,指标

		KLINE_INFO  m_Klines[MAX_KLINE_COUNT];	//两个k线的数据和参数 第二个全程为灰色,与第一个形成对比
		FUTU_INFO	m_Futu;			//附图相关数据

		int		m_nTotal;		//总长度
		int		m_nCyc;			//按键缩放
		int		m_nKWidth;		//两线之间的宽度
		int		m_nMouseX;
		int		m_nMouseY;
		int		m_nMaxKNum;		//能显示的k线最大数量
		int		m_nMove;		//正数为向右平移,0为现在
		bool	m_bHover;
		int		m_nTickPre;		//鼠标上一次刷新时间
		int		m_nFirst;		//开始k线
		int		m_nEnd;			//结束k线

		//主图指标
		float	m_ztLine[MAX_FLINE_COUNT][MAX_DATA_COUNT];	//主图线
		bool	m_bShowZtLine[MAX_FLINE_COUNT];

	protected:
		void		OnPaint(IRenderTarget *pRT);
		void		DrawArrow(IRenderTarget * pRT);
		void		GetMaxDiff(DWORD dk);		//判断坐标最大最小值和k线条数
		BOOL		IsInRect(int x, int y, int nMode = 0);	//是否在坐标中,0为全部,1为上方,2为下方
		SStringW	GetYPrice(int nY, DWORD nk);
		void		DrawData(IRenderTarget * pRT, DWORD dk);
		int			GetXData(int nx); 	//获取鼠标下的数据id
		int			GetYPos(double fDiff, DWORD dk);
		void		OnLButtonDown(UINT nFlags, CPoint point);
		int			OnCreate(LPCREATESTRUCT lpCreateStruct);

		void		GetFuTuMaxDiff();		//判断副图坐标最大最小值和k线条数
		int			GetFuTuYPos(double fDiff);	//获得附图y位置
		SStringW	GetFuTuYPrice(int nY);		//获得附图y位置价格

		SOUI_MSG_MAP_BEGIN()
			MSG_WM_PAINT_EX(OnPaint)
			MSG_WM_LBUTTONDOWN(OnLButtonDown)
			MSG_WM_MOUSEMOVE(OnMouseMove)
			MSG_WM_MOUSELEAVE(OnMouseLeave)
			MSG_WM_CREATE(OnCreate)
		SOUI_MSG_MAP_END()
	};
}
#endif // !_SKLINE_PIC