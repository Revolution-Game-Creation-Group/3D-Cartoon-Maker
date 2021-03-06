// LmUITalkBoxForm.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUITalkBoxForm.h"
#include "LmUITimeLineView.h"
#include "MainFrm.h"

// LmUITalkBoxForm 대화 상자입니다.

IMPLEMENT_DYNAMIC(LmUITalkBoxForm, CDialog)

LmUITalkBoxForm::LmUITalkBoxForm(CWnd* pParent /*=NULL*/)
	: CDialog(LmUITalkBoxForm::IDD, pParent)
{
	m_TalkBoxColor = RGB(255,255,255);
	m_brush.CreateSolidBrush(RGB(255,255,255));
	m_brushColor.CreateSolidBrush(m_TalkBoxColor);
	

	m_nStartFrm = 1;
	m_nTalkFrmLength = 1;
	m_nOldStartFrm = 1;
	m_isInsert = FALSE;
	m_nTalkBoxStyle = 0;
}

LmUITalkBoxForm::~LmUITalkBoxForm()
{
}

void LmUITalkBoxForm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TALKBOX, m_listTalkBox);

	DDX_Text(pDX, IDC_EDIT1, m_strTalkContent);
	DDX_Text(pDX, IDC_EDIT3, m_nStartFrm);
	DDX_Text(pDX, IDC_EDIT2, m_nTalkFrmLength);
	DDX_Control(pDX, IDC_EDIT1, m_ctrlTalkContent);
	DDX_Control(pDX, IDC_EDIT3, m_ctrlStartFrm);
	DDX_Control(pDX, IDC_EDIT2, m_ctrlTalkFrmLength);
	//DDX_Radio(pDX, IDC_TALKBOX_BOX1, m_nTalkBoxStyle );
	DDX_Control(pDX, IDC_TALK_INSERT, m_ctrlInsertTalkBox);
}


BEGIN_MESSAGE_MAP(LmUITalkBoxForm, CDialog)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_TALK_INSERT, &LmUITalkBoxForm::OnBnClickedTalkInsert)
	ON_BN_CLICKED(IDC_TALK_MODIFY, &LmUITalkBoxForm::OnBnClickedTalkModify)
	ON_BN_CLICKED(IDC_TALK_DELETE, &LmUITalkBoxForm::OnBnClickedTalkDelete)
	ON_BN_CLICKED(IDC_CHANGE_COLOR, &LmUITalkBoxForm::OnBnClickedChangeColor)
	ON_NOTIFY(NM_DBLCLK, IDC_TALKBOX, &LmUITalkBoxForm::OnNMDblclkTalkbox)
	ON_BN_CLICKED(IDC_TALK_NOW_FRAME, &LmUITalkBoxForm::OnBnClickedTalkNowFrame)
	ON_BN_CLICKED(IDC_TALK_NEW_INPUT, &LmUITalkBoxForm::OnBnClickedTalkNewInput)
END_MESSAGE_MAP()


// LmUITalkBoxForm 메시지 처리기입니다.


void LmUITalkBoxForm::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rect;
	GetClientRect(&rect);
	dc.SelectStockObject(WHITE_PEN);
	dc.SelectStockObject(WHITE_BRUSH);
	dc.Rectangle(&rect);

	CRect rectColor(128, 369, 158, 389);
	CPen pen(PS_SOLID, 1, RGB(0, 0, 0));
	CPen * pOldPen = dc.SelectObject(&pen);
	CBrush * pOldBrush = dc.SelectObject(&m_brushColor);
	dc.Rectangle(&rectColor);
	dc.SelectObject(pOldBrush);
	dc.SelectObject(pOldPen);

}



HBRUSH LmUITalkBoxForm::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkColor(RGB(255,255,255));
	return m_brush;
}

BOOL LmUITalkBoxForm::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_listTalkBox.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	m_listTalkBox.InsertColumn(0,L"캐릭터 이름",LVCFMT_CENTER, 80);
	//m_listTalkBox.InsertColumn(1,L"말풍선 종류",LVCFMT_CENTER, 80);
	m_listTalkBox.InsertColumn(1,L"시작",LVCFMT_CENTER, 60);
	m_listTalkBox.InsertColumn(2,L"끝",LVCFMT_CENTER, 60);
	m_listTalkBox.InsertColumn(3,L"문장",LVCFMT_CENTER,100);

	//초기 프로그램이 실행되었을때 씬이 없으므로, 말풍선 삽입 버튼도 Disable한다.
	EnableInsertBtn(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void LmUITalkBoxForm::OnBnClickedTalkInsert()
{

	// 현재 타임라인의 Caret 위치를 읽어온다.
	LmUITimeLineView* pTimeLineView = GetTimeLineView();

	SetInputMode(TRUE);
//	m_nStartFrm = pTimeLineView->GetCurrFrame();
	INT iListCount = m_listTalkBox.GetItemCount();

	//말풍선 리스트에 추가 한다.

	if(!isOKTalkBoxInfo())
		return;

	// [말풍선] 캐릭터 이름이 필요하다.

	// [동인] 말풍선 삽입
	InsertTalkBoxContent(iListCount);

	TheCartoonMaker->InsertTalkBox(m_strTalkContent.GetBuffer(0), m_nStartFrm, m_nTalkFrmLength, m_TalkBoxColor);
}

//타임라인의 주소를 얻어온다.
LmUITimeLineView* LmUITalkBoxForm::GetTimeLineView()
{
	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
	LmUITimeLineView* pTimeLineView = (LmUITimeLineView*) ( pMainFrame->m_wndSplitRow.GetPane(1, 0) );
	return pTimeLineView;
}

void LmUITalkBoxForm::OnBnClickedTalkModify()
{
	// 몇 개가 선택되었는가?
	UINT uSelectedCount = m_listTalkBox.GetSelectedCount();
	int  nItem = -1;

	// 1개가 선택되었다면 수정 창이 뜬다.
	if (uSelectedCount > 0)
	{
		nItem = m_listTalkBox.GetNextItem(nItem, LVNI_SELECTED);
		ASSERT(nItem != -1);

		CString strCharacterName	= m_listTalkBox.GetItemText(nItem,0);

		if(!isOKTalkBoxInfo())
			return;

		SetTalkBoxContent( nItem );	

		// [동인] 말풍선 수정
		TheCartoonMaker->ModifyTalkBox(strCharacterName.GetBuffer(0) , m_strTalkContent.GetBuffer(0), 
			m_nOldStartFrm, m_nStartFrm, m_nTalkFrmLength, m_TalkBoxColor);
	}
}

//TalkBoxDlg Class의 값을 세팅해준다.
void LmUITalkBoxForm::SetTalkBoxContent( INT iListCount )
{
	//////////////////////////////////////////////////////////////////////////
	// 캐릭터 이름은 어떻게 하지?
	// 우선 캐릭터 이름을 임의로 정해 둔다.

	CString strCharacterName=	L"dingding#1";
	m_listTalkBox.SetItemText(iListCount, 0, strCharacterName);

	//같은 Row에서 나머지 SubItems 세팅
	SetTalkBoxListSubItems(iListCount);

}

void LmUITalkBoxForm::InsertTalkBoxContent( INT iListCount )
{
	//////////////////////////////////////////////////////////////////////////
	// 캐릭터 이름은 어떻게 하지?
	// 우선 캐릭터 이름을 임의로 정해 둔다.

	// [말풍선] 캐릭터 이름을 얻어와서 입력한다. 
	CString strCharacterName=	L"dingding#1";
	m_listTalkBox.InsertItem(iListCount, strCharacterName);
	
	//같은 Row에서 나머지 SubItems 세팅
	SetTalkBoxListSubItems(iListCount);

}

void LmUITalkBoxForm::SetTalkBoxListSubItems( INT iListCount )
{
	CString strTalkBoxSytle = GetTalkBoxStyleStr(m_nTalkBoxStyle);
	CString strFrameStart;
	CString strFrameEnd;
	CString strTalkContent	= m_strTalkContent;

	strFrameStart.Format(L"%d", m_nStartFrm);
	strFrameEnd.Format(L"%d", m_nStartFrm + m_nTalkFrmLength);

	//m_listTalkBox.SetItemText(iListCount, 1, strTalkBoxSytle);
	m_listTalkBox.SetItemText(iListCount, 1, strFrameStart);
	m_listTalkBox.SetItemText(iListCount, 2, strFrameEnd);
	m_listTalkBox.SetItemText(iListCount, 3, strTalkContent);
}

void LmUITalkBoxForm::InsertTalkBoxContent( LPCWSTR pcwsCharacterName, UINT uFrameStart, UINT uFrameEnd, LPCWSTR pcwsTalkContent , LPCWSTR pcwsTalkBoxStyle )
{
	INT iListCount = m_listTalkBox.GetItemCount();

	CString strFrameStart;
	CString strFrameEnd;

	strFrameStart.Format(L"%d", uFrameStart);
	strFrameEnd.Format(L"%d", uFrameEnd);

	m_listTalkBox.InsertItem(iListCount, pcwsCharacterName);
	//m_listTalkBox.SetItemText(iListCount, 1, pcwsTalkBoxStyle);
	m_listTalkBox.SetItemText(iListCount, 1, strFrameStart);
	m_listTalkBox.SetItemText(iListCount, 2, strFrameEnd);
	m_listTalkBox.SetItemText(iListCount, 3, pcwsTalkContent);
}


void LmUITalkBoxForm::OnBnClickedTalkDelete()
{
	UINT uSelectedCount = m_listTalkBox.GetSelectedCount();
	int  nItem = -1;

	//1개가 선택되었다면 그 Row(행)을 지운다.
	if (uSelectedCount > 0)
	{
		//선택된 아이템의 번호를 얻어온다.
		nItem = m_listTalkBox.GetNextItem(nItem, LVNI_SELECTED);
		ASSERT(nItem != -1);

		CString strCharacterName	= m_listTalkBox.GetItemText(nItem,0);
		CString strTalkBoxSytle		= m_listTalkBox.GetItemText(nItem,1);
		CString strFrameStart		= m_listTalkBox.GetItemText(nItem,2);

		INT nFrmaeStart = _wtoi(strFrameStart);
		m_listTalkBox.DeleteItem(nItem);

		// [동인] 말풍선 삭제
		TheCartoonMaker->RemoveTalkBox(strCharacterName.GetBuffer(0), nFrmaeStart);
	}
}
BOOL LmUITalkBoxForm::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_RETURN)
		return TRUE;

	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void LmUITalkBoxForm::InitTalkBoxList()
{
	//기존에 있던 모든 내용을 지운다. 
	m_listTalkBox.DeleteAllItems();
}

CString LmUITalkBoxForm::GetTalkBoxStyleStr( INT nStyle )
{
	CString str = L"말풍선5";
	switch(nStyle)
	{
	case 0:
		str = L"말풍선1";
		return str;
	case 1:
		str = L"말풍선2";
		return str;
	case 2:
		str = L"말풍선3";
		return str;
	case 3:
		str = L"말풍선4";
		return str;
	}
	return str;
}

INT LmUITalkBoxForm::GetTalkBoxStyleNum( LPCWSTR pcwsTalkBoxStr )
{
	CString str = pcwsTalkBoxStr;
	CString strTalkBox1 = L"말풍선1";
	CString strTalkBox2 = L"말풍선2";
	CString strTalkBox3 = L"말풍선3";
	CString strTalkBox4 = L"말풍선4";

	if(str == strTalkBox1)
		return 0;

	if(str == strTalkBox2)
		return 1;

	if(str == strTalkBox3)
		return 2;

	if(str == strTalkBox4)
		return 3;

	return -1;
}

BOOL LmUITalkBoxForm::isOKTalkBoxInfo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strTalkContent;
	CString strStartFrm;
	CString strTalkFrmLength;

	m_ctrlTalkContent.GetWindowText(strTalkContent);
	m_ctrlStartFrm.GetWindowText(strStartFrm);
	m_ctrlTalkFrmLength.GetWindowText(strTalkFrmLength);

	INT nStartFrm = _wtoi(strStartFrm.GetBuffer(0));
	INT nTalkFrmLength = _wtoi(strTalkFrmLength.GetBuffer(0));
	INT nEndFrm = nStartFrm + nTalkFrmLength - 1;


	if(strTalkContent.IsEmpty() ||
		strStartFrm.IsEmpty() ||
		strTalkFrmLength.IsEmpty())
	{
		AfxMessageBox(L"말풍선 정보를 모두 입력해 주세요!");
		return FALSE;
	}

	// [말풍선] 캐릭터 이름을 얻어온다.
	if(strTalkContent.GetLength()>11)
	{
		AfxMessageBox(L"글자수는 10자로 제한되어 있습니다.");
		return FALSE;
	}

	//Insert 모드 일 경우, 말풍선을 넣을 Frame이 부족하면, 다시 입력 받아라!
	CString strError = L"이미 프레임에 말풍선을 넣을 수 있는 Frame이 부족합니다. 다시 입력해 주세요!";
	if( m_isInsert && (TheCartoonMaker->CheckTalkBoxSectionArea(L"dingding#1", nStartFrm, nEndFrm) <= 0))
	{	
		AfxMessageBox(strError);
		return FALSE;
	}

	if( !m_isInsert && (TheCartoonMaker->CheckTalkBoxSectionArea(L"dingding#1", m_nOldStartFrm, nStartFrm, nEndFrm) <= 0) )
	{	
		AfxMessageBox(strError);
		return FALSE;
	}

	if(nStartFrm<=0)
	{
		AfxMessageBox(L"시작 프레임은 1부터 시작합니다. 다시 입력 해주세요!");
		return FALSE;
	}

	if(nTalkFrmLength<=0)
	{
		AfxMessageBox(L"지속 프레임 수는 최소 1 프레임 이상입니다. 다시 입력 해주세요!");
		return FALSE;
	}

	UpdateData(TRUE);
	return TRUE;
//	LmUIMainToolFormView * pParentWnd = (LmUIMainToolFormView*)GetParent();
//	pParentWnd->ChangeCurDlg(L"MainTool");
	//OnOK();
}


void LmUITalkBoxForm::OnBnClickedChangeColor()
{
	CColorDialog dlg(m_TalkBoxColor);
	if(dlg.DoModal() == IDOK)
	{
		m_TalkBoxColor = dlg.GetColor();
		m_brushColor.DeleteObject();
		m_brushColor.CreateSolidBrush(m_TalkBoxColor);
		Invalidate();
	}
}

void LmUITalkBoxForm::SetInputMode(BOOL isInsert)
{
	m_isInsert = isInsert;
}
void LmUITalkBoxForm::OnNMDblclkTalkbox(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	// 몇 개가 선택되었는가?
	UINT uSelectedCount = m_listTalkBox.GetSelectedCount();
	int  nItem = -1;

	// 1개가 선택되었다면 수정 창이 뜬다.
	if (uSelectedCount > 0)
	{
		//선택된 아이템의 번호를 얻어온다.
		nItem = m_listTalkBox.GetNextItem(nItem, LVNI_SELECTED);
		ASSERT(nItem != -1);

		CString strCharacterName	= m_listTalkBox.GetItemText(nItem,0);
		//CString strTalkBoxSytle		= m_listTalkBox.GetItemText(nItem,1);
		CString strFrameStart		= m_listTalkBox.GetItemText(nItem,1);
		CString strFrameEnd			= m_listTalkBox.GetItemText(nItem,2);
		CString strTalkContent		= m_listTalkBox.GetItemText(nItem,3);

		//INT nTalkBoxStyle = GetTalkBoxStyleNum(strTalkBoxSytle.GetBuffer(0));

		m_nStartFrm = _wtoi(strFrameStart);
		m_nTalkFrmLength = _wtoi(strFrameEnd) - m_nStartFrm + 1;
		m_strTalkContent = strTalkContent;
		SetInputMode(FALSE);
		m_nOldStartFrm = _wtoi(strFrameStart);
		//m_nTalkBoxStyle  = nTalkBoxStyle;

		// [동인] 캐릭터의 색깔을 돌려준다.
		 TheCartoonMaker->GetTalkBoxColor(strCharacterName.GetBuffer(0), m_nStartFrm, m_TalkBoxColor);

		Invalidate();
		UpdateData(FALSE);
	}

}


void LmUITalkBoxForm::OnBnClickedTalkNowFrame()
{
	UpdateData(TRUE);

	LmUITimeLineView* pTimeLineView = GetTimeLineView();

	m_nStartFrm = pTimeLineView->GetCurrFrame();

	UpdateData(FALSE);
	
}

void LmUITalkBoxForm::OnBnClickedTalkNewInput()
{
	m_strTalkContent = L"";
	m_nStartFrm  = 1;
	m_nTalkFrmLength = 1;
	
	m_TalkBoxColor = RGB(255,255,255);
	m_nTalkBoxStyle = 0;

	m_brushColor.DeleteObject();
	m_brushColor.CreateSolidBrush(m_TalkBoxColor);

	UpdateData(FALSE);

}
