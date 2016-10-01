#include "StdAfx.h"
#include "BlockCreater.h"

const TCHAR* nullStr = _T("");				// ""
void alertBox(LPCTSTR msg)
{
	AfxMessageBox(msg, MB_OK | MB_ICONEXCLAMATION);
}

void rxErrorAlert(Acad::ErrorStatus msgId)
{
	CString str;
	if(msgId != Acad::eOk)
	{
		str.Format(_T("ARX ERROR: %s"), acadErrorStatusText(msgId));
		alertBox(str);
	}
}

void rxErrorMsg(Acad::ErrorStatus msgId)
{
	if(msgId != Acad::eOk)
		acutPrintf(_T("\nARX ERROR: %s"), acadErrorStatusText(msgId));
}

Acad::ErrorStatus defineNewBlock(LPCTSTR blkName, AcDbBlockTableRecord*& newBlkRec, AcDbObjectId& newBlkRecId, AcDbDatabase* db)
{
	ASSERT(db != NULL);
	AcDbBlockTable* blkTbl;
	Acad::ErrorStatus es = db->getSymbolTable(blkTbl, AcDb::kForWrite);
	if(es != Acad::eOk)
		return es;
	// if this block already exists, erase its contents first
	if(blkTbl->getAt(blkName, newBlkRec, AcDb::kForWrite) == Acad::eOk) 
	{
		newBlkRecId = newBlkRec->objectId();
		AcDbBlockTableRecordIterator* iter;
		es = newBlkRec->newIterator(iter);
		if (es != Acad::eOk)
		{
			rxErrorMsg(es);
			newBlkRec->close();
		}
		else
		{
			AcDbEntity* ent;
			for (; !iter->done(); iter->step())
			{
				if (iter->getEntity(ent, AcDb::kForWrite) == Acad::eOk)
				{
					ent->erase();
					ent->close();
				}
			}
			delete iter;
		}
	}
	else 
	{    // create a new block table record and add it to the block table
		newBlkRec = new AcDbBlockTableRecord;
		newBlkRec->setPathName(nullStr);    // constructor doesn't do it properly
		es = newBlkRec->setName(blkName);
		if(es == Acad::eOk)
			es = blkTbl->add(newBlkRecId, newBlkRec);
		if(es != Acad::eOk)
		{
			rxErrorMsg(es);
			delete newBlkRec;
		}
	}
	blkTbl->close();    // doesn't need to be open anymore
	return es;
}
static void AddXdataToEntity(AcDbObject* pObj, CString strData)
{
	AcDbDatabase* pDb = acdbCurDwg();
	Acad::ErrorStatus es;
	TCHAR szFileName[MAX_PATH];
	GetModuleFileName(_hdllInstance, szFileName, MAX_PATH);
	CString AppName(szFileName);
	AppName = AppName.Right(AppName.GetLength()- AppName.ReverseFind(_T('\\'))-1);
	acdbRegApp(AppName);
	resbuf *rb = acutBuildList(AcDb::kDxfRegAppName, AppName, AcDb::kDxfXdAsciiString, strData, RTNONE);
	es = pObj->setXData(rb); acutRelRb(rb);
	if (es != Acad::eOk)
	{
		acutPrintf(_T("\nError adding XDATA: %s"), acadErrorStatusText(es));
	}
}
void CreatePlane(AcDbBlockTableRecord* blkRec)
{
	AcGePoint3d& origin = AcGePoint3d(2.0, 3.0, 4.0);
	AcGePoint3d& x_axis = AcGePoint3d(0.7071, 0.7071, 0);
	AcGePoint3d& y_axis = AcGePoint3d(-0.7071, 0.7071, 0);
	int x_length = 1;
	int y_length = 2;
	AcDbFace* pDbPlane = new AcDbFace(origin, x_axis, y_axis);
	AddXdataToEntity(pDbPlane, "foo");

	Acad::ErrorStatus es = blkRec->appendAcDbEntity(pDbPlane);
	if (es != Acad::eOk)
		rxErrorAlert(es);
	pDbPlane->close();
}
void CreateCylinder(AcDbBlockTableRecord* blkRec)
{
	AcGeVector3d origin(2, 3, 4); // mid point of the cylinder
	double radius = 2;
	int length = 1;
	AcGeVector3d axis(0.7071, 0.7071, 0);	
	AcDb3dSolid* pDbCylinder = new AcDb3dSolid;
	assert(pDbCylinder != NULL);
	pDbCylinder->createFrustum(length, radius, radius, radius);
	AcGeMatrix3d mat;
	mat.setTranslation(origin);
	pDbCylinder->transformBy(mat);
	AddXdataToEntity(pDbCylinder, "bar");

	Acad::ErrorStatus es = blkRec->appendAcDbEntity(pDbCylinder);
	if(es != Acad::eOk)
		rxErrorAlert(es);
	pDbCylinder->close();
}
AcDbBlockTableRecord* openCurrentSpaceBlock(AcDb::OpenMode mode, AcDbDatabase* db)
{
	ASSERT(db != NULL);
	AcDbBlockTableRecord* blkRec;
	Acad::ErrorStatus es = acdbOpenObject(blkRec, db->currentSpaceId(), mode);
	if(es != Acad::eOk)
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
	if(blkRec == NULL)
		return Acad::eInvalidInput;
	// append new entity to current space block
	Acad::ErrorStatus es = blkRec->appendAcDbEntity(newEnt);
	if(es != Acad::eOk)
		acutPrintf(_T("\nERROR: could not add entity to current space (%s)"), acadErrorStatusText(es));
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

void CreateNewLayer(CString Name)
{
	AcDbLayerTable* pLayerTable;
	acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pLayerTable, AcDb::kForWrite);

	//create a new layout.
	if (!pLayerTable->has(Name))
	{
		pLayerTable->upgradeOpen();
		AcDbLayerTableRecord *pLayerTableRecord = new AcDbLayerTableRecord;
		pLayerTableRecord->setName(Name);
		pLayerTable->add(pLayerTableRecord);
		pLayerTableRecord->close();
	}
	pLayerTable->close();
}

void AddBlock(CString Name)
{
	AcGePoint3d centerPt;
	AcDbBlockTableRecord* newBlkRec;
	AcDbBlockReference* blkRef;
	AcGeVector3d offset;
	AcDbObjectId newBlockId;
	AcDbDatabase* pDbase = acdbHostApplicationServices()->workingDatabase();
	// make a block that envelops the entity
	if(defineNewBlock(Name, newBlkRec, newBlockId, pDbase) != Acad::eOk)
		return;
	CreatePlane(newBlkRec);
	CreateCylinder(newBlkRec);
	newBlkRec->close();
	CreateNewLayer(Name);

	// insert the block into the drawing
	blkRef = new AcDbBlockReference;
	if(blkRef->setBlockTableRecord(newBlockId) != Acad::eOk)
	{
		delete blkRef;
		return;
	}
	blkRef->setLayer(Name);
	blkRef->setDatabaseDefaults();
	addToCurrentSpaceAndClose(blkRef);
	pDbase->saveAs(Name);
}