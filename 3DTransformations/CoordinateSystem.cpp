#include "stdafx.h"
#include "CViewDlg.h"
#include "CPerspectiveViewDlg.h"
#include "CoordinateSystem.h"
#include "ChildView.h"
using namespace cs;

extern CChildView* cwnd;

CoordinateSystem *CoordinateSystem::GetInstance()
{
	static CoordinateSystem coordSystem(
		"x", "y", "z", RGB(0, 0, 0), RGB(0, 255, 0)
	);
	return &coordSystem;
}


CoordinateSystem::CoordinateSystem(
	CString xAxisName/* = "x"*/,
	CString yAxisName/* = "y"*/,
	COLORREF axisColor/* = RGB(0, 0, 0)*/,
	COLORREF gridColor/* = RGB(0, 0, 0)*/
)
{
	Init();
}


CoordinateSystem::CoordinateSystem(
	CString xAxisName/* = "x"*/,
	CString yAxisName/* = "y"*/,
	CString zAxisName/* = "z"*/,
	COLORREF axisColor/* = RGB(0, 0, 0)*/,
	COLORREF gridColor/* = RGB(0, 0, 0)*/
)
{
	Init();
}

void CoordinateSystem::Init()
{
  positionMatrix_ = Matrix<double>(4, 4);
  for (int i = 0; i < 4; ++i)
  {
    positionMatrix_(i, i) = 1;
  }
  switchToOrthogonalProjectionMode();
  moveViewToStart();
	_origin = LogicPoint(0, 0);
	_physOrigin = CPoint(200, 200);

	for (int axisIndex = (int)CoordinateAxisName::X; (int)axisIndex < (int)CoordinateAxisName::Z + 1; axisIndex++)
	{
		CoordinateAxisName axis = CoordinateAxisName(axisIndex);
		_axisInfoMap[axis] = AxisInfo();

		AxisInfo& axisInfo = _axisInfoMap[axis];
		axisInfo.rendering = true;
		axisInfo.minPoint[(int)axis] = -100;
		axisInfo.maxPoint[(int)axis] = 100;

		switch (axis)
		{
		case CoordinateAxisName::X:
			axisInfo.renderingName = "x";
			break;
		case CoordinateAxisName::Y:
			axisInfo.renderingName = "y";
			break;
		case CoordinateAxisName::Z:
			axisInfo.renderingName = "z";
			break;
		}
	}
}


CoordinateSystem::~CoordinateSystem()
{

}


void CoordinateSystem::Render(CPaintDC *dc, CWnd *wnd)
{
	RECT rect;
	wnd->GetClientRect(&rect);

	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	
	//Creating buffer
	CDC memDc;
	CBitmap memBitmap;
	memDc.CreateCompatibleDC(dc);
	memBitmap.CreateCompatibleBitmap(dc, width, height);
	memDc.SelectObject(memBitmap);
	memDc.FillSolidRect(&rect, GetSysColor(COLOR_WINDOW));

	switch (_renderingAlg)
	{
	case RenderingAlgorithm::STANDARD:
		StandardRenderingAlgorithm(&memDc);
		break;
	case RenderingAlgorithm::ZBUFFER:
		ZBufferRenderingAlgorithm(&memDc);
		break;
	default:
		break;
	}
	
	//Flushing
	dc->BitBlt(0, 0, width, height, &memDc, 0, 0, SRCCOPY);
}


void CoordinateSystem::Zoom(double val)
{
	_scale *= val;
}


void CoordinateSystem::Move(double dx, double dy, double dz)
{
  positionMatrix_ = GetTransferenceMatrix(dx, dy, dz) * positionMatrix_;
}


void CoordinateSystem::MoveOriginPhysPoint(double dx, double dy)
{
	_physOrigin.x += dx;
	_physOrigin.y += dy;
}


void CoordinateSystem::SetOriginTo(const LogicPoint& p)
{
	auto physPointNewOrigin = ConvertLogicPointToPhys(p);
	_physOrigin = physPointNewOrigin;

	Move(-p.x, -p.y, -p.z);
}


void CoordinateSystem::SetRenderingAlgorithm(RenderingAlgorithm alg)
{
	_renderingAlg = alg;
}


void CoordinateSystem::RotateAroundAxis(CoordinateAxisName axis, double deltaAngle)
{
	switch (axis)
	{
	case CoordinateAxisName::X:
		{
      positionMatrix_ = GetXAxisRotationMatrix(deltaAngle) * positionMatrix_;
		}
		break;
	case CoordinateAxisName::Y:
		{
      positionMatrix_ = GetYAxisRotationMatrix(deltaAngle) * positionMatrix_;
		}
		break;
	case CoordinateAxisName::Z:
		{
      positionMatrix_ = GetZAxisRotationMatrix(deltaAngle) * positionMatrix_;
		}
		break;
	}
}


void CoordinateSystem::RotateAroundAxis(const std::pair<LogicPoint, LogicPoint>& axisPoints, double deltaAngle)
{
	double cx = axisPoints.second.x - axisPoints.first.x;
	double cy = axisPoints.second.y - axisPoints.first.y;
	double cz = axisPoints.second.z - axisPoints.first.z;

	double dzy = sqrt(cz * cz + cy * cy);
	double dzx = sqrt(cz * cz + cx * cx);

	double xRotAngle = asin(cy / dzy);
	double yRotAngle = asin(cx / dzx);

	Move(axisPoints.first.x, axisPoints.first.y, axisPoints.first.z);

	RotateAroundAxis(CoordinateAxisName::X, -xRotAngle);
	RotateAroundAxis(CoordinateAxisName::Y, -yRotAngle);
	
	RotateAroundAxis(CoordinateAxisName::Z, deltaAngle);

	RotateAroundAxis(CoordinateAxisName::Y, yRotAngle);
	RotateAroundAxis(CoordinateAxisName::X, xRotAngle);

	Move(-axisPoints.first.x, -axisPoints.first.y, -axisPoints.first.z);
}


CPoint CoordinateSystem::GetPhysOrigin() const
{
	return _physOrigin;
}


HomogeneousPoint<double> CoordinateSystem::GetWatcherVector() const
{
  return fabs(viewMatrix_(3, 3)) <= 0.000001 
    ? HomogeneousPoint<double>() 
    : HomogeneousPoint<double>(viewMatrix_(0,3)/ viewMatrix_(3, 3), viewMatrix_(1, 3)/ viewMatrix_(3, 3), viewMatrix_(2, 3)/ viewMatrix_(3, 3));
}


template<class T>
constexpr Matrix<T> ProjectionOrthogonalOffCenterLH(T left, T right, T bottom, T top, T zNear, T zFar)
{
  Matrix<T> orthogonalMatrix {4, 4};
  orthogonalMatrix(0, 0) = static_cast<T>(2) / (right - left);
  orthogonalMatrix(1, 1) = static_cast<T>(2) / (top - bottom);
  orthogonalMatrix(2, 2) = static_cast<T>(-2) / (zFar - zNear);
  orthogonalMatrix(3, 0) = -(right + left) / (right - left);
  orthogonalMatrix(3, 1) = -(top + bottom) / (top - bottom);
  orthogonalMatrix(3, 2) = -(zFar + zNear) / (zFar - zNear);
  orthogonalMatrix(3, 3) = static_cast<T>(1);
  return orthogonalMatrix;
}


void cs::CoordinateSystem::switchToOrthogonalProjectionMode()
{
  projectionMatrix_ = ProjectionOrthogonalOffCenterLH<double>(1, 3, 1, 3, 1, 3);
}

//fovy in degrees
template<class T>
Matrix<T> ProjectionPerspectiveOffCenterLH(T fovy, T aspect, T zNear, T zFar)
{
  T f = 1.0 / std::tan(((fovy * M_PI) / 180.0) / 2);
  Matrix<T> perspectiveMatrix{ 4, 4 };
  perspectiveMatrix(0, 0) = f / aspect;
  perspectiveMatrix(1, 1) = f;
  perspectiveMatrix(2, 2) = (zFar + zNear) / (zNear -zFar);
  perspectiveMatrix(2, 3) = -1;
  perspectiveMatrix(3, 2) = (2 * zFar * zNear) / zNear - zFar;
  return perspectiveMatrix;
}

void cs::CoordinateSystem::switchToPerspectiveProjectionMode(CWnd *wnd)
{
  RECT rect;
  wnd->GetClientRect(&rect);

  int width = rect.right - rect.left;
  int height = rect.bottom - rect.top;

  changePerspectiveProjection(60, static_cast<double>(width) / height, 1, 2);
}

template<class T, size_t N>
void normalize(T (&vec)[N])
{
  T length = 0;
  for (size_t i = 0; i < N; ++i)
    length += vec[i] * vec[i];

  if (fabs(length) > 0.000001)
  {
    length = sqrt(length);

    for (size_t i = 0; i < N; ++i)
      vec[i] /= length;
  }
}

template<class T, size_t N>
void crossProduct(T (&a)[N], T (&b)[N], T (&result)[N]) 
{
  static_assert(N == 3);
  result[0] = a[1] * b[2] - a[2] * b[1];
  result[1] = -a[0] * b[2] + a[2] * b[0];
  result[2] = a[0] * b[1] - a[1] * b[0];
}

template<class T>
Matrix<T> ViewLookAt(T eyeX, T eyeY, T eyeZ, T centerX, T centerY, T centerZ, T upX, T upY, T upZ)
{
  T F[] = { centerX - eyeX, centerY - eyeY, centerZ - eyeZ }, UP[] = { upX, upY, upZ };

  normalize(F);
  normalize(UP);

  T s[3];
  crossProduct(F, UP, s);

  T s_norm[] = { s[0], s[1], s[2] };
  normalize(s_norm);

  T u[3];
  crossProduct(s_norm, F, u);

  Matrix<T> viewMatrix{ 4, 4 };
  // update matrix values
  for (int i = 0; i < 3; ++i)
  {
    viewMatrix(i, 0) = s[i];
    viewMatrix(i, 1) = u[i];
    viewMatrix(i, 2) = -F[i];
  }
  viewMatrix(3, 3) = 1;
  return viewMatrix;
}

void cs::CoordinateSystem::moveViewToStart()
{
  changeView(0, 0, 1, -1, 0, 0);
}

void cs::CoordinateSystem::changeView(double eyeX, double eyeY, double eyeZ, double upX, double upY, double upZ)
{
  //save values to set in dialog as default
  auto &dlgViewData = CViewDlg::data;
  dlgViewData.eye[0] = eyeX;
  dlgViewData.eye[1] = eyeY;
  dlgViewData.eye[2] = eyeZ;
  dlgViewData.up[0] = upX;
  dlgViewData.up[1] = upY;
  dlgViewData.up[2] = upZ;

  viewMatrix_ = ViewLookAt<double>(eyeX, eyeY, eyeZ, positionMatrix_(0, 3), positionMatrix_(1, 3), positionMatrix_(2, 3), upX, upY, upZ);
}

void cs::CoordinateSystem::changePerspectiveView(double fovy, double zNear, double zFar, double sigma, double fi)
{
  double x = zFar * cos((sigma * M_PI) / 180.0) * sin((fi * M_PI) / 180.0),
    y = zFar * sin((sigma * M_PI) / 180.0) * sin((fi* M_PI) / 180.0),
    z = zFar * cos((fi * M_PI) / 180.0);
  auto &dlgViewData = CViewDlg::data;
  changeView(x, y, z, dlgViewData.up[0], dlgViewData.up[1], dlgViewData.up[2]);


  RECT rect;
  cwnd->GetClientRect(&rect);

  int width = rect.right - rect.left;
  int height = rect.bottom - rect.top;

  cwnd->setProjectionMode(CChildView::ProjectionMode::Perspective);
  changePerspectiveProjection(fovy, static_cast<double>(width) / height, zNear, zFar);
}


void CoordinateSystem::Clear()
{
	_texts.clear();
	_objects.clear();
	_points.clear();
	_detectPoints.clear();
}


double CoordinateSystem::GetPointDistanceToProjectionPlane(const LogicPoint& point) const
{
	Matrix<double> res = HomogeneousPoint<double>(point.x, point.y, point.z) * getMVP();
	return res(0, 2);
}


void CoordinateSystem::AddText(LogicPoint p, CString text)
{
	_texts.push_back(Text(p, text));
}


void CoordinateSystem::AddLogicPoint(const LogicPoint& point, COLORREF color/* = RGB(0, 0, 0)*/, bool xDetection/* = true*/, bool yDetection/* = true*/, bool zDetection/* = true*/, bool xDetectionLine/* = true*/, bool yDetectionLine/* = true*/, bool zDetectionLine/* = true*/)
{
	bool exist = false;
	if (xDetection || yDetection || zDetection)
	{
		DetectLogicPoint newDetectPoint(point, color, xDetection, yDetection, xDetectionLine, yDetectionLine, zDetectionLine);
		if (find(_detectPoints.begin(), _detectPoints.end(), newDetectPoint) == _detectPoints.end())
		{
			_detectPoints.push_back(newDetectPoint);
		}
		else
		{
			exist = true;
		}
	}
	else
	{
		ColorLogicPoint newPoint(point, color);
		if (find(_points.begin(), _points.end(), newPoint) == _points.end())
		{
			_points.push_back(newPoint);
		}
		else
		{
			exist = true;
		}
	}

	if (!exist)
	{
		CheckAxisBounds(CoordinateAxisName::X, point.x);
		CheckAxisBounds(CoordinateAxisName::Y, point.y);
		CheckAxisBounds(CoordinateAxisName::Z, point.z);
	}
}


void CoordinateSystem::AddLogicPoint(double x, double y, COLORREF cl/* = RGB(0, 0, 0)*/, bool xDetection/* = false*/, bool yDetection/* = false*/, bool xDetectionLine/* = true*/, bool yDetectionLine/* = true*/)
{
	AddLogicPoint(LogicPoint(x, y), cl, xDetection, yDetection, false, xDetectionLine, yDetectionLine, false);
}


void CoordinateSystem::AddLogicPoint(double x, double y, double z, COLORREF cl/* = RGB(0, 0, 0)*/, bool xDetection/* = true*/, bool yDetection/* = true*/, bool zDetection/* = true*/, bool xDetectionLine/* = true*/, bool yDetectionLine/* = true*/, bool zDetectionLine/* = true*/)
{
	AddLogicPoint(LogicPoint(x, y, z), cl, xDetection, yDetection, zDetection, xDetectionLine, yDetectionLine, zDetectionLine);
}


void CoordinateSystem::AddGraphicObject(GraphicObject* obj, bool deleteOnDestroy/* = true*/)
{
	if (deleteOnDestroy)
	{
		_objects.push_back(shared_ptr<GraphicObject>(obj));
	}
	else
	{
		_objects.push_back(shared_ptr<GraphicObject>(obj, [](GraphicObject*) {}));
	}
}


GraphicObject* CoordinateSystem::LastGraphicObject()
{
	if (_objects.size())
	{
		return  _objects[_objects.size() - 1].get();
	}

	return nullptr;
}


const vector<GraphicObject::Ptr>& CoordinateSystem::GetGraphicObejctsList() const
{
	return _objects;
}


void CoordinateSystem::EnableInvisibleLinesAsDash(bool enable/* = true*/)
{
	_invisibleLinesAsDash = enable;
}


void CoordinateSystem::EnableGridRendering(bool enable/* = true*/)
{
	_gridRender = enable;
}


void CoordinateSystem::EnableAxisRendering(CoordinateAxisName axis, bool enable/* = true*/)
{
	_axisInfoMap[axis].rendering = enable;
}


void CoordinateSystem::SetAxisRenderingName(CoordinateAxisName axis, CString renderingName)
{
	_axisInfoMap[axis].renderingName = renderingName;
}


void CoordinateSystem::SetAxisRenderingBorders(CoordinateAxisName axis, double minVal, double maxVal)
{
	_axisInfoMap[axis].minPoint[0] = 1;// minPoint[(int)axis] = minVal;
	_axisInfoMap[axis].maxPoint[(int)axis] = maxVal;
}


void CoordinateSystem::SetAxisRenderingDivisions(CoordinateAxisName axis, double value)
{
	_axisInfoMap[axis].divisionValue = value;
}


void CoordinateSystem::SetPhysOrigin(CPoint physOrigin)
{
	_physOrigin = physOrigin;
}


LogicPoint CoordinateSystem::ConvertPhysPointToLogic(const CPoint& point)
{
	LogicPoint lp;

	return lp;
}


LogicPoint CoordinateSystem::ConvertToProjectionSytemPoint(const LogicPoint& lp) const
{
	static HomogeneousPoint<double> point;
	point[0] = lp.x;
	point[1] = lp.y;
	point[2] = lp.z;

	Matrix<double> res = point * getMVP();
	return LogicPoint(res(0, 0), res(0, 1), res(0, 2));
}


CPoint CoordinateSystem::ConvertLogicPointToPhys(const LogicPoint& lp) const
{
	static HomogeneousPoint<double> point;
	point[0] = lp.x;
	point[1] = lp.y;
	point[2] = lp.z;

	return ConvertLogicPointToPhys(point);
}


CPoint CoordinateSystem::ConvertLogicPointToPhys(const HomogeneousPoint<double>& point) const
{
	Matrix<double> res = point * getMVP();
	return CPoint( _scale * res(0, 0) + _physOrigin.x, _scale * res(0, 1) + _physOrigin.y);
}


void CoordinateSystem::CheckAxisBounds(CoordinateAxisName ax, double v)
{
	/*if (v > _axisInfoMap[ax].maxRenderingValue)
	{
		_axisInfoMap[ax].maxRenderingValue = v + 1;
	}
	else if (v < _axisInfoMap[ax].minRenderingValue)
	{
		_axisInfoMap[ax].maxRenderingValue = v - 1;
	}*/
}

Matrix<double> cs::CoordinateSystem::getMVP() const
{
  return projectionMatrix_ * viewMatrix_ * positionMatrix_;
}

void cs::CoordinateSystem::changePerspectiveProjection(double fovy, double aspect, double zNear, double zFar)
{
  auto &dlgPerspectiveViewData = CPerspectiveViewDlg::data;
  dlgPerspectiveViewData.fovy = fovy;
  dlgPerspectiveViewData.p = zFar;
  dlgPerspectiveViewData.d = zNear;
  auto &dlgViewData = CViewDlg::data;
  auto r = sqrt(dlgViewData.eye[0] * dlgViewData.eye[0] + dlgViewData.eye[1] * dlgViewData.eye[1] + dlgViewData.eye[2] * dlgViewData.eye[2]);
  dlgPerspectiveViewData.sigma = (atan(dlgViewData.eye[1] / dlgViewData.eye[0]) * 180.0) / M_PI;
  dlgPerspectiveViewData.fi = (acos(dlgViewData.eye[2] / r) * 180.0) / M_PI;

  projectionMatrix_ = ProjectionPerspectiveOffCenterLH<double>(fovy, aspect, zNear, zFar);
}


void CoordinateSystem::StandardRenderingAlgorithm(CDC *dc)
{
	RenderAxes(dc);
	RenderGraphicObjects(dc);

	RenerColorPoints(dc);
	RenderDetectPoints(dc);

	RenderTexts(dc);
}


void CoordinateSystem::ZBufferRenderingAlgorithm(CDC *dc)
{
	CRect rect;
	dc->GetClipBox(&rect);

	_buff.Resize(rect.bottom - rect.top, rect.right - rect.left);

    _buff.Reset();
	_buff.Init(this, _objects, _invisibleLinesAsDash);
	_buff.Render(dc);
}


void CoordinateSystem::RenderAxes(CDC *dc)
{
	CPoint tmp = dc->GetCurrentPosition();

	bool renderSystemOrigin = false;
	for each(auto& axisInfo in _axisInfoMap)
	{
		if (axisInfo.second.rendering)
		{
			if (!renderSystemOrigin)
			{
				renderSystemOrigin = true;
			}

			RenderAxis(dc, axisInfo.second);
		}
	}

	dc->MoveTo(tmp);
}


void CoordinateSystem::RenderGraphicObjects(CDC *dc)
{
	for each(GraphicObject::Ptr pObj in _objects)
	{
		pObj->Render(this, dc);
	}
}


void CoordinateSystem::RenerColorPoints(CDC *dc)
{
	for each(const ColorLogicPoint& p in _points)
	{
		RenderColorPoint(dc, p);
	}
}


void CoordinateSystem::RenderDetectPoints(CDC *dc)
{
	for each(const DetectLogicPoint& p in _detectPoints)
	{
		RenderDetectPoint(dc, p);
	}
}


void CoordinateSystem::RenderTexts(CDC *dc)
{
	for each(const Text& text in _texts)
	{
		RenderText(dc, text);
	}
}


void CoordinateSystem::RenderAxis(CDC *dc, const AxisInfo& axisInfo)
{
	CPoint minPointPhysic = ConvertLogicPointToPhys(axisInfo.minPoint);
	CPoint maxPointPhysic = ConvertLogicPointToPhys(axisInfo.maxPoint);

	CPoint prevPos = dc->MoveTo(minPointPhysic);
	dc->LineTo(maxPointPhysic);
	dc->TextOut(maxPointPhysic.x, maxPointPhysic.y, axisInfo.renderingName);
	dc->MoveTo(prevPos);
}


void CoordinateSystem::RenderArrow(CDC *dc, Side type)
{
#define ARROW_LEN 50
#define ARROW_HEIGHT 10
	CPoint tmp = dc->GetCurrentPosition();
	CPoint p1, p2;
	switch (type)
	{
	case RIGHT:case LEFT:
		p1.y = tmp.y - ARROW_HEIGHT;
		p2.y = tmp.y + ARROW_HEIGHT;
		if (RIGHT == type)
		{
			p2.x = p1.x = tmp.x - ARROW_LEN;
		}
		else
		{
			p2.x = p1.x = ARROW_LEN;
		}
		break;
	case TOP: case BOTTOM:
		p1.x = tmp.x - ARROW_HEIGHT;
		p2.x = tmp.x + ARROW_HEIGHT;
		if (TOP == type)
		{
			p2.y = p1.y = ARROW_LEN;
		}
		else
		{
			p2.y = p1.y = tmp.y - ARROW_LEN;
		}
		break;
	}
	dc->LineTo(p1);
	dc->MoveTo(tmp);
	dc->LineTo(p2);
	dc->MoveTo(tmp);
}


void CoordinateSystem::RenderColorPoint(CDC *dc, const ColorLogicPoint &colorPoint)
{
#define POINT_RECT_SZ 5
	CPoint pp = ConvertLogicPointToPhys(colorPoint.logicPoint);
	CBrush pointBrush(colorPoint.color);
	CBrush *oldBrush = (CBrush*)dc->SelectObject(&pointBrush);
	dc->Ellipse(pp.x - POINT_RECT_SZ, pp.y - POINT_RECT_SZ, pp.x + POINT_RECT_SZ, pp.y + POINT_RECT_SZ);
	dc->SelectObject(oldBrush);
}


void CoordinateSystem::RenderDetectPoint(CDC *dc, const DetectLogicPoint &detectPoint)
{
	//CPoint pp = ConvertLogicPointToPhys(detectPoint.logicPoint);
	//CPoint tmp = dc->GetCurrentPosition();
	//CPen detectPen(PS_DASH, 1, RGB(0, 0, 0));
	//CPen *oldPen = (CPen*)dc->SelectObject(&detectPen);
	////horizontal detect line
	//if (detectPoint.horzDetect)
	//{
	//	if (detectPoint.horzDetectLine)
	//	{
	//		dc->MoveTo(pp.x, _physOrigin.y);
	//		dc->LineTo(pp.x, pp.y);
	//	}
	//	RenderDivision(dc, Axis::X, detectPoint.logicPoint.x);
	//}
	////vert detect line
	//if (detectPoint.vertDetect)
	//{
	//	if (detectPoint.vertDetectLine)
	//	{
	//		dc->MoveTo(_physOrigin.x, pp.y);
	//		dc->LineTo(pp.x, pp.y);
	//	}
	//	RenderDivision(dc, Axis::Y, detectPoint.logicPoint.y);
	//}
	//dc->MoveTo(tmp);
	//dc->SelectObject(oldPen);
	//RenderColorPoint(dc, detectPoint);
}


void CoordinateSystem::RenderDivision(CDC *dc, CoordinateAxisName axis, double value)
{
	CString str;
	str.Format("%.1f", value);
	//RenderTextOnAxis(dc, axis, value, str);
}


void CoordinateSystem::RenderText(CDC *dc, const Text& text)
{
	/*int tmp = dc->SetBkMode(TRANSPARENT);
	dc->TextOut(p.x, p.y, text);
	dc->SetBkMode(tmp);*/
}


void CoordinateSystem::RenderText(CDC *dc, const CPoint& physPoint, CString text)
{

}


void CoordinateSystem::RenderTextOnAxis(CDC *dc, CoordinateAxisName axis, double value, CString text)
{
	/*CPoint p;
	if (Axis::Y == axis)
	{
		p.x = ConvertLogicXToPhys(0) + AXIS_INDENT;
		p.y = ConvertLogicYToPhys(value);
	}
	else
	{
		CSize size = dc->GetTextExtent(text);
		p.x = ConvertLogicXToPhys(value);
		p.y = ConvertLogicYToPhys(0) - size.cy - AXIS_INDENT;
	}*/
	//RenderText(dc, p, text);
}


Matrix<double>& CoordinateSystem::GetXAxisRotationMatrix(double angle)
{
	static Matrix<double> matr(4, 4);
	matr(0, 0) = matr(3, 3) = 1;
	matr(1, 1) = matr(2, 2) = cos(angle);
	matr(2, 1) = -(matr(1, 2) = sin(angle));

	return matr;
}


Matrix<double>& CoordinateSystem::GetYAxisRotationMatrix(double angle)
{
	static Matrix<double> matr(4, 4);
	matr(1, 1) = matr(3, 3) = 1;
	matr(0, 0) = matr(2, 2) = cos(angle);
	matr(2, 0) = -(matr(0, 2) = sin(angle));

	return matr;
}


Matrix<double>& CoordinateSystem::GetZAxisRotationMatrix(double angle)
{
	static Matrix<double> matr(4, 4);
	matr(2, 2) = matr(3, 3) = 1;
	matr(0, 0) = matr(1, 1) = cos(angle);
	matr(1, 0) = -(matr(0, 1) = sin(angle));

	return matr;
}


Matrix<double>& CoordinateSystem::GetTransferenceMatrix(double dx, double dy, double dz)
{
	static Matrix<double> matr(4, 4);
	matr(0, 0) = matr(1, 1) = matr(2, 2) = matr(3, 3) = 1;
	matr(3, 0) = dx;
	matr(3, 1) = dy;
	matr(3, 2) = dz;

	return matr;
}
