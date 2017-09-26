#include "stdafx.h"
#include "CoordinateSystem.h"
using namespace cs;


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
	_projectionMatrix = Matrix<double>(4, 4);
	_projectionMatrix[0][0] = _projectionMatrix[1][1] = _projectionMatrix[3][3] = 1;

	_origin = LogicPoint(0, 0);
	_physOrigin = CPoint(0, 0);

	_watcherVector[0] = _watcherVector[1] = 0;
	_watcherVector[2] = 100;

	for (int axisIndex = (int)Axis::X; (int)axisIndex < (int)Axis::Z + 1; axisIndex++)
	{
		Axis axis = Axis(axisIndex);
		_axisInfoMap[axis] = AxisInfo();

		AxisInfo& axisInfo = _axisInfoMap[axis];
		axisInfo.rendering = true;
		axisInfo.minPoint[(int)axis] = -100;
		axisInfo.maxPoint[(int)axis] = 100;

		switch (axis)
		{
		case Axis::X:
			axisInfo.renderingName = "x";
			break;
		case Axis::Y:
			axisInfo.renderingName = "y";
			break;
		case Axis::Z:
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
	dc->GetClipBox(&rect);

	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	
	//Creating buffer
	CDC memDc;
	CBitmap memBitmap;
	memDc.CreateCompatibleDC(dc);
	memBitmap.CreateCompatibleBitmap(dc, width, height);
	memDc.SelectObject(memBitmap);
	memDc.FillSolidRect(&rect, GetSysColor(COLOR_WINDOW));

	//Rendering algorithm

	StandardRenderingAlgorithm(&memDc);
	//ZBufferRenderingAlgorithm(&memDc);

	//

	//Flushing
	dc->BitBlt(0, 0, width, height, &memDc, 0, 0, SRCCOPY);
}


void CoordinateSystem::Zoom(double val)
{
	_scale *= val;
}


void CoordinateSystem::Move(double dx, double dy, double dz)
{
	_projectionMatrix = GetTransferenceMatrix(dx, dy, dz) * _projectionMatrix;
}


void CoordinateSystem::MoveOriginPhysPoint(double dx, double dy)
{
	_physOrigin.x += dx;
	_physOrigin.y += dy;
}


void CoordinateSystem::SetOriginTo(LogicPoint p)
{
	auto physPointNewOrigin = ConvertLogicPointToPhys(p);
	_physOrigin = physPointNewOrigin;

	Move(-p.x, -p.y, -p.z);
}


void CoordinateSystem::RotateAroundAxis(Axis axis, double deltaAngle)
{
	Matrix<double> newWatchVectorMatrix(1, 4);

	switch (axis)
	{
	case Axis::X:
		{
			newWatchVectorMatrix = _watcherVector * GetXAxisRotationMatrix(-deltaAngle);
			_projectionMatrix = GetXAxisRotationMatrix(deltaAngle) * _projectionMatrix;
		}
		break;
	case Axis::Y:
		{
			newWatchVectorMatrix = _watcherVector * GetYAxisRotationMatrix(-deltaAngle);
			_projectionMatrix = GetYAxisRotationMatrix(deltaAngle) * _projectionMatrix;
		}
		break;
	case Axis::Z:
		{
			newWatchVectorMatrix = _watcherVector * GetZAxisRotationMatrix(-deltaAngle);
			_projectionMatrix = GetZAxisRotationMatrix(deltaAngle) * _projectionMatrix;
		}
		break;
	}

	_watcherVector[0] = newWatchVectorMatrix[0][0];
	_watcherVector[1] = newWatchVectorMatrix[0][1];
	_watcherVector[2] = newWatchVectorMatrix[0][2];
}


void CoordinateSystem::RotateAroundAxis(std::pair<LogicPoint, LogicPoint> axisPoints, double deltaAngle)
{
	double cx = axisPoints.second.x - axisPoints.first.x;
	double cy = axisPoints.second.y - axisPoints.first.y;
	double cz = axisPoints.second.z - axisPoints.first.z;

	double dzy = sqrt(cz * cz + cy * cy);
	double dzx = sqrt(cz * cz + cx * cx);

	double xRotAngle = asin(cy / dzy);
	double yRotAngle = asin(cx / dzx);

	Move(axisPoints.first.x, axisPoints.first.y, axisPoints.first.z);

	RotateAroundAxis(Axis::X, -xRotAngle);
	RotateAroundAxis(Axis::Y, -yRotAngle);
	
	RotateAroundAxis(Axis::Z, deltaAngle);

	RotateAroundAxis(Axis::Y, yRotAngle);
	RotateAroundAxis(Axis::X, xRotAngle);

	Move(-axisPoints.first.x, -axisPoints.first.y, -axisPoints.first.z);
}


CPoint CoordinateSystem::GetPhysOrigin() const
{
	return _physOrigin;
}


const HomogeneousPoint<double>& CoordinateSystem::GetWatcherVector() const
{
	return _watcherVector;
}


void CoordinateSystem::Clear()
{
	_texts.clear();
	_objects.clear();
	_points.clear();
	_detectPoints.clear();
}


void CoordinateSystem::AddText(LogicPoint p, CString text)
{
	_texts.push_back(Text(p, text));
}


void CoordinateSystem::AddLogicPoint(LogicPoint point, COLORREF color/* = RGB(0, 0, 0)*/, bool xDetection/* = true*/, bool yDetection/* = true*/, bool zDetection/* = true*/, bool xDetectionLine/* = true*/, bool yDetectionLine/* = true*/, bool zDetectionLine/* = true*/)
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
		CheckAxisBounds(Axis::X, point.x);
		CheckAxisBounds(Axis::Y, point.y);
		CheckAxisBounds(Axis::Z, point.z);
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


void CoordinateSystem::AddGraphicObject(GraphicObject* obj, COLORREF color /*= RGB(0, 0, 0)*/)
{
	_objects.push_back(shared_ptr<GraphicObject>(obj));
}


GraphicObject* CoordinateSystem::LastGraphicObject()
{
	if (_objects.size())
	{
		return  _objects[_objects.size() - 1].get();
	}

	return nullptr;
}


const vector<GraphicObject::Ptr>& CoordinateSystem::GetGraphicObejctsList()
{
	return _objects;
}


void CoordinateSystem::EnableGridRendering(bool enable/* = true*/)
{
	_gridRender = enable;
}


void CoordinateSystem::EnableAxisRendering(Axis axis, bool enable/* = true*/)
{
	_axisInfoMap[axis].rendering = enable;
}


void CoordinateSystem::SetAxisRenderingName(Axis axis, CString renderingName)
{
	_axisInfoMap[axis].renderingName = renderingName;
}


void CoordinateSystem::SetAxisRenderingBorders(Axis axis, double minVal, double maxVal)
{
	_axisInfoMap[axis].minPoint[0] = 1;// minPoint[(int)axis] = minVal;
	_axisInfoMap[axis].maxPoint[(int)axis] = maxVal;
}


void CoordinateSystem::SetAxisRenderingDivisions(Axis axis, double value)
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
	Matrix<double> res = point * _projectionMatrix;
	return CPoint( _scale * res[0][0] + _physOrigin.x, _scale * res[0][1] + _physOrigin.y);
}


void CoordinateSystem::CheckAxisBounds(Axis ax, double v)
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


	for each(GraphicObject::Ptr pObj in _objects)
	{
		pObj->Render(this, dc);
	}
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


void CoordinateSystem::RenderDivision(CDC *dc, Axis axis, double value)
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


void CoordinateSystem::RenderTextOnAxis(CDC *dc, Axis axis, double value, CString text)
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
	matr[0][0] = matr[3][3] = 1;
	matr[1][1] = matr[2][2] = cos(angle);
	matr[2][1] = -(matr[1][2] = sin(angle));

	return matr;
}


Matrix<double>& CoordinateSystem::GetYAxisRotationMatrix(double angle)
{
	static Matrix<double> matr(4, 4);
	matr[1][1] = matr[3][3] = 1;
	matr[0][0] = matr[2][2] = cos(angle);
	matr[2][0] = -(matr[0][2] = sin(angle));

	return matr;
}


Matrix<double>& CoordinateSystem::GetZAxisRotationMatrix(double angle)
{
	static Matrix<double> matr(4, 4);
	matr[2][2] = matr[3][3] = 1;
	matr[0][0] = matr[1][1] = cos(angle);
	matr[1][0] = -(matr[0][1] = sin(angle));

	return matr;
}


Matrix<double>& CoordinateSystem::GetTransferenceMatrix(double dx, double dy, double dz)
{
	static Matrix<double> matr(4, 4);
	matr[0][0] = matr[1][1] = matr[2][2] = matr[3][3] = 1;
	matr[3][0] = dx;
	matr[3][1] = dy;
	matr[3][2] = dz;

	return matr;
}
