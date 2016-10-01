//
#include "StdAfx.h"

#if defined(_DEBUG) && !defined(AC_FULL_DEBUG)
#error _DEBUG should not be defined except in internal Adesk debug builds
#endif


#include <afxdisp.h>        // MFC OLE automation classes
#include "adslib.h"
#include "acadi.h"
#include "rxmfcapi.h"
#include "acadi_i.c"


bool getApplication(LPDISPATCH * pVal)
{
    LPDISPATCH pDispatch = acedGetAcadWinApp()->GetIDispatch(TRUE);
    if(pDispatch == NULL)
		return false;
    *pVal = pDispatch;
    return true;
}


bool getAcadMenuGroup(IAcadMenuGroup  **pVal)
{
	IAcadApplication *acadApp = NULL;
    LPDISPATCH  pDisp = NULL;
    if(!getApplication(&pDisp))
        return false;
    HRESULT hr = S_OK;
    hr = pDisp->QueryInterface(IID_IAcadApplication, (LPVOID*)&acadApp);
    if(FAILED(hr))
        return false;
    LPDISPATCH  pTempDisp = NULL;
    IAcadMenuGroups *mnuGrps = NULL;
    long cnt = 0;
    //get the menu groups
    hr = acadApp->get_MenuGroups(&mnuGrps);
    if(FAILED(hr))
    {
        acadApp->Release();
        return false;
    }
    mnuGrps->get_Count(&cnt);
    //get AutoCAD menu group. say it is index 0.
    IAcadMenuGroup *mnuGrp = NULL;
    VARIANT  vtName;
    vtName.vt = VT_I4;
    BSTR  grpName;
    bool found = false ;
    for(long i=0; i < cnt; i++)
    {
        vtName.lVal = i;
        hr = mnuGrps->Item(vtName, &mnuGrp);
        if(FAILED(hr))
            return false;
		
        hr  = mnuGrp->get_Name(&grpName);
        CString cgrpName(grpName);
        if(cgrpName.CompareNoCase(_T("Acad"))==0) 
        {
            found = true;
            *pVal = mnuGrp;
            break;
        }
    }

    acadApp->Release();
    return found;
}

void CreateToolbars()
{
    IAcadMenuGroup *mnuGrp = NULL;
    if(!getAcadMenuGroup(&mnuGrp))
        return ;
    //now get all the popup menus 
    IAcadToolbars  *tlbrs = NULL;
    HRESULT hr = S_OK;
    hr = mnuGrp->get_Toolbars(&tlbrs);
    mnuGrp->Release();
    //let us create toolbars for polysamp
    IAcadToolbar  *tlbr = NULL;
    hr = tlbrs->Add(L"SOMEBLOCK APPLICATION", &tlbr);
    if FAILED(hr)
		return;
    tlbrs->Release();
    //now add toolbar buttons
    IAcadToolbarItem *button=NULL;
    VARIANT index;
    index.vt = VT_I4;
    index.lVal = 100l;

    VARIANT vtFalse;
    vtFalse.vt = VT_BOOL;
    vtFalse.boolVal = VARIANT_FALSE;
	TCHAR szFileName[MAX_PATH];
	GetModuleFileName(_hdllInstance, szFileName, MAX_PATH);
	CString csPath(szFileName);
	csPath = csPath.Left(csPath.ReverseFind(_T('\\')));

	CString icoPath = csPath + _T("\\block.ico");
    hr = tlbr->AddToolbarButton(index, L"SomeBlock", L"Creates some block entity", L"MyCommandLocal ", vtFalse, &button);
    hr = button->SetBitmaps(CComBSTR(icoPath),CComBSTR(icoPath));
    button->Release();

    tlbr->Dock(acToolbarDockLeft);
    tlbr->Release();
    return;


}

void CleanUpToolbars()
{
    IAcadMenuGroup *mnuGrp = NULL;
    if(!getAcadMenuGroup(&mnuGrp))
        return ;
    IAcadToolbars  *tlbrs = NULL;
    HRESULT hr = S_OK;

    hr = mnuGrp->get_Toolbars(&tlbrs);
    mnuGrp->Release();

    long cnt = 0;
    hr = tlbrs->get_Count(&cnt);

    IAcadToolbar *myTlbr = NULL;
    BSTR  tempName;

    VARIANT vtName;
    for(long i=0; i < cnt; i++)
    {
        vtName.vt = VT_I4;
        vtName.lVal = i;
        hr = tlbrs->Item(vtName, &myTlbr);
        hr = myTlbr->get_Name(&tempName);
        CString tlbrName(tempName);
        SysFreeString(tempName);
        if(tlbrName.CompareNoCase(_T("SOMEBLOCK APPLICATION"))==0)
        {
			myTlbr->Delete();
            break;
        }
        else
			myTlbr->Release();

    }
    tlbrs->Release();
    return;
}

void UpdateUserInterfaceForPolySamp()
{
    CreateToolbars();
}

void CleanUpUserInterfaceForPolySamp()
{
    CleanUpToolbars();
}
