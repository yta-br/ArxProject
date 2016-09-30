#include "StdAfx.h"
#include "BlockCreater.h"

//#include "CAcadMenuBar.h"
//#if defined(_DEBUG) && !defined(AC_FULL_DEBUG)
//#error _DEBUG should not be defined except in internal Adesk debug builds
//#endif
//
//#pragma warning( disable : 4278 )
//#import "acax21ENU.tlb" no_implementation raw_interfaces_only named_guids
//#pragma warning( default : 4278 )
//
//#include <rxregsvc.h>
//#include <aced.h>
//#include <adslib.h>
//#include "tchar.h"
//
/****************************************************************************
**
**	ArxDbgUtils::defineNewBlock
**		Use when you are trying to define or re-define a block.  If
**	re-defining, this function will delete the current contents of the block
**	for you first.  If defining for the first time, it will add a new
**	record to the blockTable for you and return an openedForWrite blockRec.
**
**	**jma
**
*************************************/
const TCHAR* nullStr = _T("");				// ""
void alertBox(LPCTSTR msg)
{
	AfxMessageBox(msg, MB_OK | MB_ICONEXCLAMATION);
}

void rxErrorAlert(Acad::ErrorStatus msgId)
{
	CString str;

	if (msgId != Acad::eOk) {
		str.Format(_T("ARX ERROR: %s"), acadErrorStatusText(msgId));
		alertBox(str);
	}
}

void rxErrorMsg(Acad::ErrorStatus msgId)
{
	if (msgId != Acad::eOk)
		acutPrintf(_T("\nARX ERROR: %s"), acadErrorStatusText(msgId));
}

Acad::ErrorStatus defineNewBlock(LPCTSTR blkName, AcDbBlockTableRecord*& newBlkRec, AcDbObjectId& newBlkRecId, AcDbDatabase* db)
{
	ASSERT(db != NULL);

	AcDbBlockTable* blkTbl;
	Acad::ErrorStatus es = db->getSymbolTable(blkTbl, AcDb::kForWrite);
	if (es != Acad::eOk)
		return es;

	// if this block already exists, erase its contents first
	if (blkTbl->getAt(blkName, newBlkRec, AcDb::kForWrite) == Acad::eOk) {
		newBlkRecId = newBlkRec->objectId();
		AcDbBlockTableRecordIterator* iter;
		es = newBlkRec->newIterator(iter);
		if (es != Acad::eOk) {
			rxErrorMsg(es);
			newBlkRec->close();
		}
		else {
			AcDbEntity* ent;
			for (; !iter->done(); iter->step()) {
				if (iter->getEntity(ent, AcDb::kForWrite) == Acad::eOk) {
					ent->erase();
					ent->close();
				}
			}
			delete iter;
		}
	}
	else {    // create a new block table record and add it to the block table
		newBlkRec = new AcDbBlockTableRecord;
		newBlkRec->setPathName(nullStr);    // constructor doesn't do it properly

		es = newBlkRec->setName(blkName);
		if (es == Acad::eOk)
			es = blkTbl->add(newBlkRecId, newBlkRec);

		if (es != Acad::eOk) {
			rxErrorMsg(es);
			delete newBlkRec;
		}
	}

	blkTbl->close();    // doesn't need to be open anymore
	return es;
}

void extMakeAFace(AcDbBlockTableRecord* blkRec,const AcGePoint3d& pt1, const AcGePoint3d& pt2,const AcGePoint3d& pt3, const AcGePoint3d& pt4)
{
	AcDbFace* face = new AcDbFace;
	face->setVertexAt(0, pt1);
	face->setVertexAt(1, pt2);
	face->setVertexAt(2, pt3);
	face->setVertexAt(3, pt4);

	Acad::ErrorStatus es = blkRec->appendAcDbEntity(face);
	if (es != Acad::eOk)
		rxErrorAlert(es);

	face->close();
}
void CreatePlane(AcDbBlockTableRecord* blkRec)
{
	AcGePoint3d& origin = AcGePoint3d(2.0, 3.0, 4.0);
	AcGeVector3d& x_axis = AcGeVector3d(0.7071, 0.7071, 0);
	AcGeVector3d& y_axis = AcGeVector3d(-0.7071, 0.7071, 0);
	int x_length = 1;
	int y_length = 2;
	AcDbPoint pt;

//	AcGePlane* plane = new AcGePlane(origin, x_axis, y_axis);

//	Acad::ErrorStatus es = blkRec->(plane);
	//if (es != Acad::eOk)
	//	rxErrorAlert(es);


}
void CreateCylinder()
{/*
	AcGePoint3d origin(2, 3, 4); // mid point of the cylinder
	int radius = 2;
	int length = 1;
	AcGeVector3d axis(0.7071, 0.7071, 0);	
	
	acdbModelerStart();
	//AcGeCylinder* pGeCylinder = new AcGeCylinder(radius, origin, axis);

	//acdbAcisDeleteModelerBulletins(); // Memory efficiency thing from autodesk
	AcDb3dSolid* pDbCylinder = new AcDb3dSolid();*/
/*
	pDbCylinder->createSphere(5000);
	pDbCylinder->setLayer(pszPerimLayerName);
	pDbCylinder->setColor(acColor);

	AcDbBlockTableRecord* pRecord;
	Acad::ErrorStatus eError = m_pBlockTable->getAt(ACDB_MODEL_SPACE, pRecord, m_eOpenMode);

	if (eError == Acad::eOk)
	{
		eError = pRecord->appendAcDbEntity(pDbCylinder);

		if (eError == Acad::eOk)
		{
			// The solid must be closed.
			pDbCylinder->close();
		}
		else
		{
			throw new CError(eError);
		}
	}
	acdbModelerEnd();

	delete pGeCylinder;
	*/
}
AcDbBlockTableRecord* openCurrentSpaceBlock(AcDb::OpenMode mode, AcDbDatabase* db)
{
	ASSERT(db != NULL);

	AcDbBlockTableRecord* blkRec;

	Acad::ErrorStatus es = acdbOpenObject(blkRec, db->currentSpaceId(), mode);
	if (es != Acad::eOk)
		return NULL;
	else
		return blkRec;
}

Acad::ErrorStatus addToCurrentSpace(AcDbEntity* newEnt, AcDbDatabase* db)
{
	ASSERT(newEnt != NULL);
	ASSERT(db != NULL);

	AcDbBlockTableRecord* blkRec = openCurrentSpaceBlock(AcDb::kForWrite, db);

	ASSERT(blkRec != NULL);

	if (blkRec == NULL)
		return Acad::eInvalidInput;

	// append new entity to current space block
	Acad::ErrorStatus es = blkRec->appendAcDbEntity(newEnt);
	if (es != Acad::eOk) {
		acutPrintf(_T("\nERROR: could not add entity to current space (%s)"), acadErrorStatusText(es));
	}

	blkRec->close();
	return es;
}


Acad::ErrorStatus addToCurrentSpaceAndClose(AcDbEntity* newEnt, AcDbDatabase* db)
{
	Acad::ErrorStatus es;

	es = addToCurrentSpace(newEnt, db);
	if (es == Acad::eOk)

		newEnt->close();
	else
		delete newEnt;

	return es;
}
Acad::ErrorStatus addToCurrentSpaceAndClose(AcDbEntity* newEnt)
{
	return addToCurrentSpaceAndClose(newEnt, acdbHostApplicationServices()->workingDatabase());
}


void AddBlock(CString Name)
{
	//AcDbEntity* obj;
	AcGePoint3d p1{ 1,1,1 }, p2{ 2,2,2 }, p3{ 3,3,3 }, p4{ 5,5,5 }, p5{ 6,6,6 }, p6{ 7,7,7 }, p7{ 8,8,8 }, p8{9,9,9};
	CString str;
	AcGePoint3d centerPt;
	AcDbBlockTableRecord* newBlkRec;
	AcDbBlockReference* blkRef;
	AcGeVector3d offset;
	AcDbObjectId newBlockId;

	{

		// make an anonymous block that envelops the entity
		if (defineNewBlock(Name, newBlkRec, newBlockId, acdbHostApplicationServices()->workingDatabase()) != Acad::eOk)
			return;

		extMakeAFace(newBlkRec, p1, p2, p3, p4);    // bottom face
		extMakeAFace(newBlkRec, p5, p6, p7, p8);    // top face
		extMakeAFace(newBlkRec, p1, p2, p6, p5);    // front face
		extMakeAFace(newBlkRec, p2, p3, p7, p6);    // right side face
		extMakeAFace(newBlkRec, p3, p4, p8, p7);    // back side face
		extMakeAFace(newBlkRec, p4, p1, p5, p8);    // left side face

		newBlkRec->close();

		// insert the block into the drawing
		blkRef = new AcDbBlockReference;
		if (blkRef->setBlockTableRecord(newBlockId) != Acad::eOk)
		{
			delete blkRef;
			return;
		}

		blkRef->setPosition(centerPt);
		blkRef->setDatabaseDefaults();
		addToCurrentSpaceAndClose(blkRef);
	}
}
/*
void AddMenu()
{
	AutoCAD::IAcadApplication *pAcad;
	AutoCAD::IAcadMenuBar *pMenuBar;
	AutoCAD::IAcadMenuGroups *pMenuGroups;
	AutoCAD::IAcadMenuGroup *pMenuGroup;
	AutoCAD::IAcadPopupMenus *pPopUpMenus;
	AutoCAD::IAcadPopupMenu *pPopUpMenu;
	AutoCAD::IAcadPopupMenuItem *pPopUpMenuItem;
	HRESULT hr = NOERROR;
	LPUNKNOWN pUnk = NULL;
	LPDISPATCH pAcadDisp = acedGetIDispatch(TRUE);
	if (pAcadDisp == NULL)
		return;
	hr = pAcadDisp->QueryInterface(AutoCAD::IID_IAcadApplication, (void**)&pAcad);
	pAcadDisp->Release();
	if (FAILED(hr))
		return;
	pAcad->put_Visible(true);
	pAcad->get_MenuBar(&pMenuBar);
	pAcad->get_MenuGroups(&pMenuGroups);
	
	pAcad->Release();
	long numberOfMenus;
	pMenuBar->get_Count(&numberOfMenus);
	pMenuBar->Release();
	VARIANT index;
	VariantInit(&index);
	V_VT(&index) = VT_I4;
	V_I4(&index) = 0;
	pMenuGroups->Item(index, &pMenuGroup);
	pMenuGroups->Release();
	pMenuGroup->get_Menus(&pPopUpMenus);
	pMenuGroup->Release();
	if (!bIsMenuLoaded) {
		// Добавляем три меню
		HRESULT hr = pPopUpMenus->Add(_T("myDemoMenu1"), &pPopUpMenu);
		hr = pPopUpMenus->Add(_T("myDemoMenu2"), &pPopUpMenu);
		hr = pPopUpMenus->Add(_T("myDemoMenu3"), &pPopUpMenu);
		if (hr == S_OK) {
			acutPrintf(_T("\nМеню создано."));
		}
		else {
			acutPrintf(_T("\nМеню не создано."));
		}
		bIsMenuLoaded = true;
	}else // Удаляем меню
	{
		long count = 0;
		pPopUpMenus->get_Count(&count);
		acutPrintf(_T("\n Счетчик до удаления: %d"), count);
		long indexOfMyMenu = -1;
		AutoCAD::IAcadPopupMenu* eachMenu = NULL;
		for (long i = 0; i< count; i++)
		{
			BSTR np;
			pPopUpMenus->Item(_variant_t(i), &eachMenu);
			eachMenu->get_Name(&np);
			// Удаляем первое меню в группе
			if (_tcscmp(np, _T("myDemoMenu1")) == 0)
			{
				indexOfMyMenu = i;
				break;
			}
		}
		// Удаляем
		if (indexOfMyMenu > -1)
		{
			HRESULT hr = pPopUpMenus->RemoveMenuFromMenuBar(_variant_t(indexOfMyMenu));
			assert(hr == S_OK);
		}
		pPopUpMenus->get_Count(&count);
        // Снова пересчитываем после удаления.
		acutPrintf(_T("\n Счетчик после удаления: %d"), count);
		bIsMenuLoaded = false;
	}
	pPopUpMenus->Release();
}
*/