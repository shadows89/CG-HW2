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

extern CG_ModelList models;
extern double max;
extern int firstDraw;
extern double maxX, minX, maxY, minY, maxZ, minZ;
//Drawing Globals
std::vector<COLORREF>* vec_bitmap = NULL;
int global_h;
int global_w;

//Mouse Globals
int mousePressed = 0;
int mouseX;
int mouseY;


int scale;

int cam = 0;
int colorNotChange = 1;
COLORREF modelColor;
COLORREF backgroundColor = RGB(0, 0, 0);
COLORREF normalColor = RGB(0, 0, 255);

bool polygonNormals = FALSE;
bool vertexNormals = FALSE;
bool polyGiven = FALSE;
bool vertexGiven = FALSE;
bool drawNormals = FALSE;
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
	ON_COMMAND(ID_CAM_BUTTON, OnCamButton)
	ON_UPDATE_COMMAND_UI(ID_CAM_BUTTON, OnUpdateCamButton)
	ON_COMMAND(ID_OBJECT_BUTTON, OnObjectButton)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_BUTTON, OnUpdateObjectButton)
	ON_COMMAND(ID_COLOR_MODEL, OnModelColorUpdate)
	ON_COMMAND(ID_COLOR_BACKGROUND, OnBackgroundColorUpdate)
	ON_COMMAND(ID_POLYGON_GIVEN, OnNormalPolygonGiven)
	ON_COMMAND(ID_POLYGON_CALCULATED, OnNormalPolygonCalculated)
	ON_UPDATE_COMMAND_UI(ID_POLYGON_GIVEN, OnNormalPolygonGivenCheck)
	ON_UPDATE_COMMAND_UI(ID_POLYGON_CALCULATED, OnNormalPolygonCalculatedCheck)
	ON_COMMAND(ID_VERTEX_GIVEN, OnNormalVertexGiven)
	ON_COMMAND(ID_VERTEX_CALCULATED, NULL)
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


	firstDraw = 1;


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
	if (w > h)
		scale = mid_h;
	else
		scale = mid_w;

	global_h = h;
	global_w = w;

	/*if (firstDraw){
		camera.lookAt(vec4(0,0, 0), vec4(0,0, -1), vec4(0, -1, 0));
		m_scale = mat4::scale(1/max);
		m_translate = mat4::translate(vec4(0, 0, 0));
		//m_rotate = mat4::rotate(0, 0, 0);
	}*/
	double R = maxX, L = minX, T = maxY, B = minY;
	double F = minZ, N = maxZ;

	/*camera.lookAt(vec4((R + L) / 2, (T + B) / 2, N + 1, 1), vec4((R + L) / 2, (T + B) / 2, N,1), vec4(0, -1, 0));*/
	//camera.lookAt(vec4(0.5, 0.5, 50, 1), vec4(0, 0, 0, 1), vec4(0, -1, 0, 1));
	/*camera.lookAt(vec4(minX, maxY, 3, 1), vec4(minX,maxY, 0, 0), vec4(0, -1, 0, 1));
	*/
	camera.lookAt(vec4(0.5, 0.5, 0, 1.0), vec4(0.5, 0.5, -0.1, 1.0), vec4(0, -1, 0, 1.0));

	m_scale = mat4::scale(1.0);

	m_translate = mat4::translate(vec4(0, 0, 0));
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

	global_w = cx;
	global_h = cy;
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
		if (drawNormals)
			(*vec_bitmap)[x + y*global_w] = normalColor;
		else
			(*vec_bitmap)[x + y*global_w] = modelColor;
}

void CCGWorkView::line(CG_Point p1, CG_Point p2){
	line(p1[0], p1[1], p2[0], p2[1]);
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

	//m_scale = m_scale * (1/max);
	//double R = maxs[0] + 2 * (abs(maxs[0])), L = mins[0] - 2 * (abs(mins[0])), T = maxs[1] + 2 * (abs(maxs[1])), B = mins[1] - 2 * (abs(mins[1]));
	//double F = mins[2] - 2 * (abs(mins[2])), N = maxs[2];
	//camera.lookAt(vec4(global_w / 2, global_h / 2, 50, 1), vec4(global_w / 2, global_h / 2, 0, 1), vec4(0, -1, 0, 1));


	mat4 invCamera = camera.transformation().inverse();
	//mat4 invCamera = mat4::eye();
	/*double R = mm[0], L =mi[0], T = mm[1], B = mi[1];*/
	double R = 10, L = -R, T = 1.0 / m_AspectRatio*R, B = -T;
	//double R = global_w, L = 0, T = global_h, B = 0;
	double F = -50.0, N = -0.01;
	double normalizedCubeFactor = 2.0;
	mat4 s1 = mat4::scale(vec4(normalizedCubeFactor / (R - L), normalizedCubeFactor / (T - B), normalizedCubeFactor / (N - F), 1));
	mat4 t1 = mat4::translate(vec4(-(L + R) / normalizedCubeFactor, -(T + B) / normalizedCubeFactor, -(F + N) / normalizedCubeFactor, 1));

	mat4 s2 = mat4::scale(vec4(global_w / normalizedCubeFactor, global_h / normalizedCubeFactor, 1, 1));
	mat4 t2 = mat4::translate(vec4((global_w - 1) / normalizedCubeFactor, (global_h - 1) / normalizedCubeFactor, 0, 1));
	mat4 l = t2*s2;
	mat4 r = s1*t1;
	mat4 projection = l*r;


	camera.setProjection(projection);



	if (m_bIsPerspective){

		//mat4 p = mat4::eye();

		//mat4 p = mat4::eye();
		//p[0][0] = N;
		//p[1][1] = (N);
		//p[2][2] = (N) + (F);
		//p[2][3] = -(N*F);
		//p[3][2] = 1;
		//p[3][3] = 0;
		mat4 p = mat4::eye();
		//p[3][3] = 0;
		//p[2][2] = (abs(F))/(abs(F)-abs(N));
		//p[2][3] = - (abs(N*F)) / (abs(F) - abs(N));
		//p[3][2] = 1/(abs(F));

		p[3][2] = 1 / (abs(F));
		//p[3][3]=0;
		m_pipeline = invCamera *m_translate*m_scale*m_rotate;
		m_pipeline = p*m_pipeline;
		m_pipeline = projection*m_pipeline;
		//m_pipeline =projection*p *invCamera *m_translate*m_scale*m_rotate;//*camera.transformation().inverse()*m_translate*m_scale*m_rotate;
	}
	else{

		m_pipeline = projection *invCamera*m_translate*m_scale*m_rotate;
	}
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

	vec_bitmap = new std::vector<COLORREF>((w + 1)*(h + 1), backgroundColor);

	if (models.getSize() != 0){
		CG_Point* p1;
		CG_Point* p2;
		CG_Point* tmp;
		for (Model* model = models.first(); model != NULL; model = models.next()){
			if(colorNotChange)
				modelColor = model->color;
			for (CG_Polygon* polygon = model->polygons->first(); polygon != NULL; polygon = model->polygons->next()){
				p1 = polygon->first();
				p2 = polygon->next();
				while (true){
					/*vec4 p1Offset = *p1*(scale / max);
					vec4 p2Offset = *p2*(scale / max);*/
					vec4 p1Offset = m_pipeline*model->position*(*p1);
					vec4 p2Offset = m_pipeline*model->position*(*p2);
					if (m_bIsPerspective)
						line(p1Offset[0] / p1Offset[3], p1Offset[1] / p1Offset[3], p2Offset[0] / p2Offset[3], p2Offset[1] / p2Offset[3]);
					else
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

				vec4 p1Offset = m_pipeline*model->position*(*p1);
				vec4 p2Offset = m_pipeline*model->position*(*p2);
				if (m_bIsPerspective)
					line(p1Offset[0] / p1Offset[3], p1Offset[1] / p1Offset[3], p2Offset[0] / p2Offset[3], p2Offset[1] / p2Offset[3]);
				else
					line(p1Offset[0], p1Offset[1], p2Offset[0], p2Offset[1]);
				//line(mid_w + p1Offset[0], mid_h + p1Offset[1], mid_w + p2Offset[0], mid_h + p2Offset[1]);
			}
			CG_Point p1(minX, minY, minZ,1.0);
			CG_Point p2(minX, minY, maxZ,1.0);
			CG_Point p3(minX, maxY, minZ,1.0);
			CG_Point p4(minX, maxY, maxZ,1.0);
			CG_Point p5(maxX, minY, minZ,1.0);
			CG_Point p6(maxX, minY, maxZ,1.0);
			CG_Point p7(maxX, maxY, minZ,1.0);
			CG_Point p8(maxX, maxY, maxZ,1.0);

			p1 = m_pipeline*(p1);
			p2 = m_pipeline*(p2);
			p3 = m_pipeline*(p3);
			p4 = m_pipeline*(p4);
			p5 = m_pipeline*(p5);
			p6 = m_pipeline*(p6);
			p7 = m_pipeline*(p7);
			p8 = m_pipeline*(p8);
			drawNormals = true;
			line(p1, p5);
			line(p1, p3);
			line(p1, p2);
			line(p3, p7);
			line(p3, p4);
			line(p4, p2);
			line(p4, p8);
			line(p2, p6);
			line(p6, p8);
			line(p6, p5);
			line(p8, p7);
			line(p5, p7);
			drawNormals = false;
			if (polygonNormals){
				drawNormals = TRUE;
				CG_NormalList* tmp = NULL;
				if (polyGiven)
					tmp = model->polygonNormals;
				else
					tmp = model->calculatedPolygonNormals;
				for (CG_Point* dir = tmp->first(),*mid = model->polygonMids->first(); dir != NULL && mid != NULL;
					dir = model->polygonNormals->next(), mid = model->polygonMids->next()){
					vec4 normal = (*mid - *dir);
					normal = m_pipeline*normal;
					normal = vec4::normalize(normal) * 15;
					vec4 screenPoint = m_pipeline*(*mid);
					line(screenPoint[0], screenPoint[1], screenPoint[0] + normal[0], screenPoint[1] + normal[1]);
				}
				drawNormals = FALSE;
			}
			if (vertexNormals){
				if (vertexGiven){
					drawNormals = TRUE;
					if (model->vertices->getSize() == model->vertexNormals->getSize())
					for (CG_Point* point = model->vertices->first(),
						*dir = model->vertexNormals->first(); point != NULL && dir != NULL;
						point = model->vertices->next(), dir = model->vertexNormals->next()){
						vec4 normal = (*point+*dir);
						normal = m_pipeline*normal;
						normal = vec4::normalize(normal)*15;
						vec4 screenPoint = m_pipeline*(*point);
						line(screenPoint[0], screenPoint[1], screenPoint[0] - normal[0], screenPoint[1]- normal[1]);
					}
					drawNormals = FALSE;
				}
				else{

				}
			}
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
					//maxX -= delta;
					//minX -= delta;
					//mins[0] -= delta;
					//maxs[0] -= delta;
					m_translate.updateTranslate(vec4(-delta, 0, 0));
					break;
				case ID_AXIS_Y:
					//mins[1] -= delta;
					//maxs[1] -= delta;
					m_translate.updateTranslate(vec4(0, -delta, 0));
					break;
				case ID_AXIS_Z:
					//maxs[2] -= delta;
					//mins[2] -= delta;
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
					maxX *= delta;
					minY *= delta;
					m_scale.updateScale(vec4(delta / 5.0, 0, 0));
					break;
				case ID_AXIS_Y:
					maxY *= delta;
					minY *= delta;
					m_scale.updateScale(vec4(0 / 5.0, delta, 0));
					break;
				case ID_AXIS_Z:
					maxZ *= delta;
					minZ *= delta;
					m_scale.updateScale(vec4(0 / 5.0, 0, delta));
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

void CCGWorkView::OnUpdateCamButton(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cam == 1);
}

void CCGWorkView::OnUpdateObjectButton(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cam == 0);
}

void CCGWorkView::OnModelColorUpdate(){
	CColorDialog color;
	color.DoModal();
	modelColor = color.GetColor();
	colorNotChange = 0;
	Invalidate();
}

void CCGWorkView::OnBackgroundColorUpdate(){
	CColorDialog color;
	color.DoModal();
	backgroundColor = color.GetColor();
	Invalidate();
}

void CCGWorkView::OnNormalPolygonGiven(){
	polygonNormals = !polygonNormals;
	polyGiven = !polyGiven;
	Invalidate();
}

void CCGWorkView::OnNormalPolygonCalculated(){
	if (!polygonNormals)
		polygonNormals = !polygonNormals;
	if (polyGiven){
		polyGiven = !polyGiven;
	}
	Invalidate();
}

void CCGWorkView::OnNormalPolygonCalculatedCheck(CCmdUI* pCmdUI){
	pCmdUI->SetCheck(polygonNormals);
}


void CCGWorkView::OnNormalPolygonGivenCheck(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(polygonNormals);
}

void CCGWorkView::OnNormalVertexGiven(){
	vertexNormals = !vertexNormals;
	vertexGiven = !vertexGiven;
	Invalidate();
}

void CCGWorkView::OnNormalVertexGivenCheck(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(vertexNormals);
}