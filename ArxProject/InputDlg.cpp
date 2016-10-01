// InputDlg.cpp : implementation file
//

#include "stdafx.h"
#include "InputDlg.h"
#include "afxdialogex.h"
#include "BlockCreater.h"

void MFCDialog()
{
	// When resource from this ARX app is needed, just
	// instantiate a local CAcModuleResourceOverride
	CAcModuleResourceOverride resOverride;

	CInputDlg dlg(CWnd::FromHandle(adsw_acadMainWnd()));
 	dlg.DoModal();
	if (dlg.strName.GetLength())
		AddBlock(dlg.strName);

}

// CInputDlg dialog
IMPLEMENT_DYNAMIC(CInputDlg, CDialogEx)

CInputDlg::CInputDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG, pParent)
{

}

CInputDlg::~CInputDlg()
{
}

void CInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT, m_edtName);
}

BEGIN_MESSAGE_MAP(CInputDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CInputDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// CInputDlg message handlers


void CInputDlg::OnBnClickedOk()
{
	int linecount = 0;
	int len = m_edtName.LineLength(m_edtName.LineIndex(linecount));
	m_edtName.GetLine(linecount, strName.GetBuffer(len), len);
	strName.ReleaseBuffer(len);
	CDialogEx::OnOK();
}

