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

CString GetWorkPath()
{
	TCHAR szFileName[MAX_PATH];
	GetModuleFileName(_hdllInstance, szFileName, MAX_PATH);
	CString csPath(szFileName);
	csPath = csPath.Left(csPath.ReverseFind(_T('\\')));

	return csPath;
}
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
	bool found = false;

/*	
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

    acadApp->Release();*/
    return found;
}
static void _LoadMyCui(void)
{
	//if (!acedIsMenuGroupLoaded(_T("MyMenuGroups")))
	//	acedCommand(RTSTR, _T("_.cuiload"), RTSTR, _T("my.cuix"), 0);
}

void CreateToolbars()
{
	CString csPAth = GetWorkPath();
	acedLoadPartialMenu(csPAth + _T("\\SomeBlock.cuix"));
}

void CleanUpToolbars()
{
	CString csPAth = GetWorkPath();
	acedUnloadPartialMenu(csPAth + _T("\\SomeBlock.cuix"));
}

