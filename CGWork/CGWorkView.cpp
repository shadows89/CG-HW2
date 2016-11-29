// CGWorkView.cpp : implementation of the CCGWorkView class
//
#include "stdafx.h"
#include "CGWork.h"

#include "CGWorkDoc.h"
#include "CGWorkView.h"

#include <iostream>
using std::cout;
using std::endl;
#include "MaterialDlg.h"
#include "LightDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "PngWrapper.h"
#include "iritSkel.h"


// For Status Bar access
#include "MainFrm.h"

#include "LinkedList.h"

extern CG_PolygonList polygons;
extern double max;
extern int firstDraw;

//Drawing Globals
std::vector<COLORREF>* vec_bitmap = NULL;
int global_h;
int global_w;

//Mouse Globals
int mousePressed = 0;
int mouseX;
int mouseY;


int cam = 0;
// Use this macro to display text messages in the status bar.
#define STATUS_BAR_TEXT(str) (((CMainFrame*)GetParentFrame())->getStatusBar().SetWindowText(str))


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView

IMPLEMENT_DYNCREATE(CCGWorkView, CView)

BEGIN_MESSAGE_MAP(CCGWorkView, CView)
	//{{AFX_MSG_MAP(CCGWorkView)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_LOAD, OnFileLoad)
	ON_COMMAND(ID_VIEW_ORTHOGRAPHIC, OnViewOrthographic)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ORTHOGRAPHIC, OnUpdateViewOrthographic)
	ON_COMMAND(ID_VIEW_PERSPECTIVE, OnViewPerspective)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PERSPECTIVE, OnUpdateViewPerspective)
	ON_COMMAND(ID_ACTION_ROTATE, OnActionRotate)
	ON_UPDATE_COMMAND_UI(ID_ACTION_ROTATE, OnUpdateActionRotate)
	ON_COMMAND(ID_ACTION_SCALE, OnActionScale)
	ON_UPDATE_COMMAND_UI(ID_ACTION_SCALE, OnUpdateActionScale)
	ON_COMMAND(ID_ACTION_TRANSLATE, OnActionTranslate)
	ON_UPDATE_COMMAND_UI(ID_ACTION_TRANSLATE, OnUpdateActionTranslate)
	ON_COMMAND(ID_AXIS_X, OnAxisX)
	ON_UPDATE_COMMAND_UI(ID_AXIS_X, OnUpdateAxisX)
	ON_COMMAND(ID_AXIS_Y, OnAxisY)
	ON_UPDATE_COMMAND_UI(ID_AXIS_Y, OnUpdateAxisY)
	ON_COMMAND(ID_AXIS_Z, OnAxisZ)
	ON_UPDATE_COMMAND_UI(ID_AXIS_Z, OnUpdateAxisZ)
	ON_COMMAND(ID_LIGHT_SHADING_FLAT, OnLightShadingFlat)
	ON_UPDATE_COMMAND_UI(ID_LIGHT_SHADING_FLAT, OnUpdateLightShadingFlat)
	ON_COMMAND(ID_LIGHT_SHADING_GOURAUD, OnLightShadingGouraud)
	ON_UPDATE_COMMAND_UI(ID_LIGHT_SHADING_GOURAUD, OnUpdateLightShadingGouraud)
	ON_COMMAND(ID_LIGHT_CONSTANTS, OnLightConstants)
	//ON_COMMAND(RI_MOUSE_LEFT_BUTTON_DOWN,OnLeftButtonDown)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_CAM_BUTTON,OnCamButton)
	ON_COMMAND(ID_OBJECT_BUTTON, OnObjectButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// A patch to fix GLaux disappearance from VS2005 to VS2008
void auxSolidCone(GLdouble radius, GLdouble height) {
	GLUquadric *quad = gluNewQuadric();
	gluQuadricDrawStyle(quad, GLU_FILL);
	gluCylinder(quad, radius, 0.0, height, 20, 20);
	gluDeleteQuadric(quad);
}

/////////////////////////////////////////////////////////////////////////////
// CCGWorkView construction/destruction

CCGWorkView::CCGWorkView()
{
	// Set default values
	m_nAxis = ID_AXIS_X;
	m_nAction = ID_ACTION_ROTATE;
	m_nView = ID_VIEW_ORTHOGRAPHIC;
	m_bIsPerspective = false;

	m_nLightShading = ID_LIGHT_SHADING_FLAT;

	m_lMaterialAmbient = 0.2;
	m_lMaterialDiffuse = 0.8;
	m_lMaterialSpecular = 1.0;
	m_nMaterialCosineFactor = 32;

	//init the first light to be enabled
	m_lights[LIGHT_ID_1].enabled = true;


}

CCGWorkView::~CCGWorkView()
{
}


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView diagnostics

#ifdef _DEBUG
void CCGWorkView::AssertValid() const
{
	CView::AssertValid();
}

void CCGWorkView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCGWorkDoc* CCGWorkView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCGWorkDoc)));
	return (CCGWorkDoc*)m_pDocument;
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView Window Creation - Linkage of windows to CGWork

BOOL CCGWorkView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	// An CGWork window must be created with the following
	// flags and must NOT include CS_PARENTDC for the
	// class style.

	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	return CView::PreCreateWindow(cs);
}



int CCGWorkView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	InitializeCGWork();

	return 0;
}


// This method initialized the CGWork system.
BOOL CCGWorkView::InitializeCGWork()
{
	m_pDC = new CClientDC(this);

	if (NULL == m_pDC) { // failure to get DC
		::AfxMessageBox(CString("Couldn't get a valid DC."));
		return FALSE;
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView message handlers

void CCGWorkView::resetTransformations(){
	CRect r;
	GetClientRect(&r);
	int w = r.Width();
	int h = r.Height();
	int mid_w = w / 2;
	int mid_h = h / 2;
	int scale = 0;
	if (w > h)
		scale = mid_h;
	else
		scale = mid_w;

	global_h = h;
	global_w = w;

	camera.lookAt(vec4(0, 2*h, 50), vec4(0, 0, 0), vec4(0, -1, 0));
	m_scale = mat4::scale(scale / 10);
	m_translate = mat4::translate(vec4(0,0,0));
}

void CCGWorkView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (0 >= cx || 0 >= cy) {
		return;
	}

	// save the width and height of the current window
	m_WindowWidth = cx;
	m_WindowHeight = cy;

	// compute the aspect ratio
	// this will keep all dimension scales equal
	m_AspectRatio = (GLdouble)m_WindowWidth / (GLdouble)m_WindowHeight;

	resetTransformations();
}


BOOL CCGWorkView::SetupViewingFrustum(void)
{
	return TRUE;
}


// This viewing projection gives us a constant aspect ration. This is done by
// increasing the corresponding size of the ortho cube.
BOOL CCGWorkView::SetupViewingOrthoConstAspect(void)
{
	return TRUE;
}





BOOL CCGWorkView::OnEraseBkgnd(CDC* pDC)
{
	// Windows will clear the window with the background color every time your window 
	// is redrawn, and then CGWork will clear the viewport with its own background color.

	//return CView::OnEraseBkgnd(pDC);
	return true;
}




void plotPixel(int x, int y){
	if (x >= 0 && x <= global_w && y >= 0 && y <= global_h)
		(*vec_bitmap)[x + y*global_w] = RGB(0, 0, 0);
}

void CCGWorkView::line(int x1, int y1, int x2, int y2){
	int dx, dy;
	dx = x2 - x1;
	dy = y2 - y1;

	//big or small slope
	if (abs(dy) > abs(dx)){
		if (x1 < x2){
			if (y1 < y2){
				big_slope_negative(x1, y1, x2, y2);
			}
			else{
				big_slope_positive(x1, y1, x2, y2);
			}
		}
		else{
			if (y1 < y2){
				big_slope_positive(x2, y2, x1, y1);
			}
			else{
				big_slope_negative(x2, y2, x1, y1);
			}
		}
	}
	else{
		if (x1 < x2){
			if (y1 < y2){
				small_slope_negative(x1, y1, x2, y2);
			}
			else{
				small_slope_positive(x1, y1, x2, y2);
			}
		}
		else{
			if (y1 < y2){
				small_slope_positive(x2, y2, x1, y1);
			}
			else{
				small_slope_negative(x2, y2, x1, y1);
			}
		}
	}
}

void CCGWorkView::small_slope_negative(int x1, int y1, int x2, int y2){
	int x, y, dx, dy, d, delta_e, delta_ne;
	x = x1;
	y = y1;
	dx = x2 - x1;
	dy = y2 - y1;
	d = 2 * dy - dx;
	delta_e = 2 * dy;
	delta_ne = 2 * (dy - dx);
	plotPixel(x, y);
	while (x < x2){
		if (d < 0){
			d = d + delta_e;
			x++;
		}
		else {
			d = d + delta_ne;
			x++;
			y++;
		}
		plotPixel(x, y);
	}
}

void CCGWorkView::small_slope_positive(int x1, int y1, int x2, int y2){
	int x, y, dx, dy, d, delta_e, delta_ne;
	x = x1;
	y = y1;
	dx = abs(x2 - x1);
	dy = abs(y2 - y1);
	d = 2 * dy - dx;
	delta_e = 2 * dy;
	delta_ne = 2 * (dy - dx);
	plotPixel(x, y);
	while (x < x2){
		if (d < 0){
			d = d + delta_e;
			x++;
		}
		else {
			d = d + delta_ne;
			x++;
			y--;
		}
		plotPixel(x, y);
	}
}


void CCGWorkView::big_slope_negative(int x1, int y1, int x2, int y2){
	int x, y, dx, dy, d, delta_e, delta_ne;
	x = x1;
	y = y1;
	dx = x2 - x1;
	dy = y2 - y1;
	d = 2 * dx - dy;
	delta_e = 2 * dx;
	delta_ne = 2 * (dx - dy);
	plotPixel(x, y);
	while (y < y2){
		if (d < 0){
			d = d + delta_e;
			y++;
		}
		else {
			d = d + delta_ne;
			x++;
			y++;
		}
		plotPixel(x, y);
	}
}

void CCGWorkView::big_slope_positive(int x1, int y1, int x2, int y2){
	int x, y, dx, dy, d, delta_e, delta_ne;
	x = x1;
	y = y1;
	dx = abs(x2 - x1);
	dy = abs(y2 - y1);
	d = 2 * dx - dy;
	delta_e = 2 * dx;
	delta_ne = 2 * (dx - dy);
	plotPixel(x, y);
	while (y > y2){
		if (d < 0){
			d = d + delta_e;
			y--;
		}
		else {
			d = d + delta_ne;
			x++;
			y--;
		}
		plotPixel(x, y);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCGWorkView drawing
/////////////////////////////////////////////////////////////////////////////

void CCGWorkView::updatePipeline(){
	//mat4 I = mat4::eye();
	//mat4 scaleMat = mat4::scale(scale);
	//mat4 rotate = mat4::rotate(xTheta,yTheta, zTheta);
	//mat4 translation = mat4::translate(vec4(mid_w, mid_h, 1));
	mat4 prespective = mat4::eye();
	prespective[3][3] = 0;
	prespective[2][2] = 15;
	prespective[2][3] = 80;
	prespective[3][2] = -1;
	//m_scale = m_scale * (1/max);
	m_pipeline = camera.transformation()*m_translate*m_scale*m_rotate;
}

void CCGWorkView::OnDraw(CDC* pDC)
{
	CCGWorkDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	//CG_PolygonList& polygons = MyPoly::GetInstance();
	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);

	if (firstDraw){
		resetTransformations();
		m_rotate = mat4::rotate(0, 0, 0);
		firstDraw = 0;
	}

	CCGWorkView::updatePipeline();

	CRect r;
	GetClientRect(&r);
	int w = r.Width();
	int h = r.Height();

	vec_bitmap = new std::vector<COLORREF>((w + 1)*(h + 1), RGB(255, 255, 255));

	if (polygons.getSize() != 0){
		CG_Point* p1;
		CG_Point* p2;
		CG_Point* tmp;
		for (CG_Polygon* polygon = polygons.first(); polygon != NULL; polygon = polygons.next()){
			p1 = polygon->first();
			p2 = polygon->next();
			while (true){
				/*vec4 p1Offset = *p1*(scale / max);
				vec4 p2Offset = *p2*(scale / max);*/
				vec4 p1Offset = m_pipeline*(*p1);
				vec4 p2Offset = m_pipeline*(*p2);
				line(p1Offset[0], p1Offset[1], p2Offset[0], p2Offset[1]);
				//line(mid_w + p1Offset[0], mid_h + p1Offset[1], mid_w + p2Offset[0], mid_h + p2Offset[1]);
				tmp = p2;
				p2 = polygon->next();
				p1 = tmp;
				if (p2 == NULL){
					p2 = polygon->first();
					break;
				}
			}

			vec4 p1Offset = m_pipeline*(*p1);
			vec4 p2Offset = m_pipeline*(*p2);
			line(p1Offset[0], p1Offset[1], p2Offset[0], p2Offset[1]);
			//line(mid_w + p1Offset[0], mid_h + p1Offset[1], mid_w + p2Offset[0], mid_h + p2Offset[1]);
		}
	}
	CBitmap bitmap;
	std::vector<COLORREF> tmp(*vec_bitmap);
	bitmap.CreateBitmap(w, h, 1, 32, &tmp[0]);
	dcMem.SelectObject(&bitmap);
	pDC->TransparentBlt(0, 0, w, h, &dcMem, 0, 0, w, h, RGB(100, 100, 100));
	bitmap.DeleteObject();
	dcMem.DeleteDC();
	delete vec_bitmap;

	//big_slope_negative(20, 200, 20, 500);
}


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView CGWork Finishing and clearing...

void CCGWorkView::OnDestroy()
{
	CView::OnDestroy();

	// delete the DC
	if (m_pDC) {
		delete m_pDC;
	}
}



/////////////////////////////////////////////////////////////////////////////
// User Defined Functions

void CCGWorkView::RenderScene() {
	// do nothing. This is supposed to be overriden...

	return;
}


void CCGWorkView::OnFileLoad()
{
	TCHAR szFilters[] = _T("IRIT Data Files (*.itd)|*.itd|All Files (*.*)|*.*||");

	CFileDialog dlg(TRUE, _T("itd"), _T("*.itd"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

	if (dlg.DoModal() == IDOK) {
		m_strItdFileName = dlg.GetPathName();		// Full path and filename
		PngWrapper p;
		CGSkelProcessIritDataFiles(m_strItdFileName, 1);
		// Open the file and read it.
		// Your code here...

		Invalidate();	// force a WM_PAINT for drawing.
	}

}





// VIEW HANDLERS ///////////////////////////////////////////

// Note: that all the following Message Handlers act in a similar way.
// Each control or command has two functions associated with it.

void CCGWorkView::OnViewOrthographic()
{
	m_nView = ID_VIEW_ORTHOGRAPHIC;
	m_bIsPerspective = false;
	Invalidate();		// redraw using the new view.
}

void CCGWorkView::OnUpdateViewOrthographic(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nView == ID_VIEW_ORTHOGRAPHIC);
}

void CCGWorkView::OnViewPerspective()
{
	m_nView = ID_VIEW_PERSPECTIVE;
	m_bIsPerspective = true;
	Invalidate();
}

void CCGWorkView::OnUpdateViewPerspective(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nView == ID_VIEW_PERSPECTIVE);
}




// ACTION HANDLERS ///////////////////////////////////////////

void CCGWorkView::OnActionRotate()
{
	m_nAction = ID_ACTION_ROTATE;
}

void CCGWorkView::OnUpdateActionRotate(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nAction == ID_ACTION_ROTATE);
}

void CCGWorkView::OnActionTranslate()
{
	m_nAction = ID_ACTION_TRANSLATE;
}

void CCGWorkView::OnUpdateActionTranslate(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nAction == ID_ACTION_TRANSLATE);
}

void CCGWorkView::OnActionScale()
{
	m_nAction = ID_ACTION_SCALE;
}

void CCGWorkView::OnUpdateActionScale(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nAction == ID_ACTION_SCALE);
}




// AXIS HANDLERS ///////////////////////////////////////////


// Gets calles when the X button is pressed or when the Axis->X menu is selected.
// The only thing we do here is set the ChildView member variable m_nAxis to the 
// selected axis.
void CCGWorkView::OnAxisX()
{
	m_nAxis = ID_AXIS_X;
	//m_rotate = mat4::rotateX(45);

}

// Gets called when windows has to repaint either the X button or the Axis pop up menu.
// The control is responsible for its redrawing.
// It sets itself disabled when the action is a Scale action.
// It sets itself Checked if the current axis is the X axis.
void CCGWorkView::OnUpdateAxisX(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_X);
}


void CCGWorkView::OnAxisY()
{
	m_nAxis = ID_AXIS_Y;
	//m_rotate = mat4::rotateY(45);

}

void CCGWorkView::OnUpdateAxisY(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_Y);
}


void CCGWorkView::OnAxisZ()
{
	m_nAxis = ID_AXIS_Z;
}

void CCGWorkView::OnUpdateAxisZ(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_Z);
	//m_rotate = mat4::rotateZ( 45);

}




// OPTIONS HANDLERS ///////////////////////////////////////////




// LIGHT SHADING HANDLERS ///////////////////////////////////////////

void CCGWorkView::OnLightShadingFlat()
{
	m_nLightShading = ID_LIGHT_SHADING_FLAT;
}

void CCGWorkView::OnUpdateLightShadingFlat(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nLightShading == ID_LIGHT_SHADING_FLAT);
}


void CCGWorkView::OnLightShadingGouraud()
{
	m_nLightShading = ID_LIGHT_SHADING_GOURAUD;
}

void CCGWorkView::OnUpdateLightShadingGouraud(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nLightShading == ID_LIGHT_SHADING_GOURAUD);
}

// LIGHT SETUP HANDLER ///////////////////////////////////////////

void CCGWorkView::OnLightConstants()
{
	CLightDialog dlg;

	for (int id = LIGHT_ID_1; id<MAX_LIGHT; id++)
	{
		dlg.SetDialogData((LightID)id, m_lights[id]);
	}
	dlg.SetDialogData(LIGHT_ID_AMBIENT, m_ambientLight);

	if (dlg.DoModal() == IDOK)
	{
		for (int id = LIGHT_ID_1; id<MAX_LIGHT; id++)
		{
			m_lights[id] = dlg.GetDialogData((LightID)id);
		}
		m_ambientLight = dlg.GetDialogData(LIGHT_ID_AMBIENT);
	}
	Invalidate();
}

void CCGWorkView::OnLButtonDown(UINT nFlags, CPoint point){
	mousePressed = 1;
}


void CCGWorkView::OnLButtonUp(UINT nFlags, CPoint point){
	mousePressed = 0;
}

void CCGWorkView::OnMouseMove(UINT nFlags, CPoint point){
	if (mousePressed){
		double delta = mouseX - point.x;
		if (cam){
			switch (m_nAction){
			case ID_ACTION_TRANSLATE:
				switch (m_nAxis){
				case ID_AXIS_X:
					camera.transformation().updateTranslate(vec4(-delta, 0, 0));
					break;
				case ID_AXIS_Y:
					camera.transformation().updateTranslate(vec4(0, -delta, 0));
					break;
				case ID_AXIS_Z:
					camera.transformation().updateTranslate(vec4(0, 0, -delta));
					break;
				}
				break;
			case ID_ACTION_ROTATE:
				switch (m_nAxis){
				case ID_AXIS_X:
					camera.setTransformation(camera.transformation() * mat4::rotateX(delta / 5));
					break;
				case ID_AXIS_Y:
					camera.setTransformation(camera.transformation() * mat4::rotateY(delta / 5));
					break;
				case ID_AXIS_Z:
					camera.setTransformation(camera.transformation() * mat4::rotateZ(delta / 5));
					break;
				}
				break;
			case ID_ACTION_SCALE:
				switch (m_nAxis){
				case ID_AXIS_X:
					camera.transformation().updateScale(vec4(delta, 0, 0));
					break;
				case ID_AXIS_Y:
					camera.transformation().updateScale(vec4(0, delta, 0));
					break;
				case ID_AXIS_Z:
					camera.transformation().updateScale(vec4(0, 0, delta));
					break;
				}
				break;
			}
		}
		else{
			switch (m_nAction){
			case ID_ACTION_TRANSLATE:
				switch (m_nAxis){
				case ID_AXIS_X:
					m_translate.updateTranslate(vec4(-delta, 0, 0));
					break;
				case ID_AXIS_Y:
					m_translate.updateTranslate(vec4(0, -delta, 0));
					break;
				case ID_AXIS_Z:
					m_translate.updateTranslate(vec4(0, 0, -delta));
					break;
				}
				break;
			case ID_ACTION_ROTATE:
				switch (m_nAxis){
				case ID_AXIS_X:
					m_rotate = m_rotate * mat4::rotateX(delta / 5);
					break;
				case ID_AXIS_Y:
					m_rotate = m_rotate * mat4::rotateY(delta / 5);
					break;
				case ID_AXIS_Z:
					m_rotate = m_rotate * mat4::rotateZ(delta / 5);
					break;
				}
				break;
			case ID_ACTION_SCALE:
				switch (m_nAxis){
				case ID_AXIS_X:
					m_scale.updateScale(vec4(delta, 0, 0));
					break;
				case ID_AXIS_Y:
					m_scale.updateScale(vec4(0, delta, 0));
					break;
				case ID_AXIS_Z:
					m_scale.updateScale(vec4(0, 0, delta));
					break;
				}
				break;
			}

		}
		Invalidate();
	}
	mouseX = point.x;
	mouseY = point.y;
}

void CCGWorkView::OnCamButton(){
	cam = 1;
}

void CCGWorkView::OnObjectButton(){
	cam = 0;
}