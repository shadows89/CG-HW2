/*****************************************************************************
* Skeleton for an interface to a parser to read IRIT data files.	     *
******************************************************************************
* Written by Amit Mano						November 2008*
******************************************************************************/

#ifndef	IRIT_SKEL_H
#define	IRIT_SKEL_H

#include <stdlib.h>
#include "irit_sm.h"
#include "iritprsr.h"
#include "attribut.h"
#include "allocate.h"
#include "ip_cnvrt.h"
#include "symb_lib.h"
#include <vector>
#include "LinkedList.h"


typedef double CG_Point;
typedef LinkedList<CG_Point*> CG_Polygon;
typedef LinkedList<CG_Polygon*> CG_PolygonList;


/*class MyPoly{
public:
	//static CG_PolygonList polygons;
	static CG_PolygonList& GetInstance(){
		static CG_PolygonList instance;
		return instance;
	}
private:
	MyPoly(MyPoly const&);
	void operator=(MyPoly const&);
	MyPoly();
};*/


bool CGSkelProcessIritDataFiles(CString &FileNames, int NumFiles);
void CGSkelDumpOneTraversedObject(IPObjectStruct *PObj, IrtHmgnMatType Mat);
int CGSkelGetObjectColor(IPObjectStruct *PObj, double RGB[3]);
const char *CGSkelGetObjectTexture(IPObjectStruct *PObj);
const char *CGSkelGetObjectPTexture(IPObjectStruct *PObj);
int CGSkelGetObjectTransp(IPObjectStruct *PObj, double *Transp);

bool CGSkelStoreData(IPObjectStruct *PObj);

#endif // IRIT_SKEL_H
