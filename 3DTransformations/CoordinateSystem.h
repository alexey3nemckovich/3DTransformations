#pragma once
#include <vector>
#include <map>
#include <exception>
#include "Plot.h"
using namespace std;


struct LogicPoint{
	int x;
	int y;
	int z;

	LogicPoint()
	{
		x = y = z = 0;
	}

	LogicPoint(int x, int y)
	{
		this->x = x;
		this->y = y;
		this->z = 0;
	}

	LogicPoint(int x, int y, int z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	bool operator==(const LogicPoint& other)
	{
		if (
			this->x == other.x &&
			this->y == other.y &&
			this->z == other.z
			)
		{
			return true;
		}

		return false;
	}

	LogicPoint* operator+=(const LogicPoint& other)
	{
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
		return this;
	}

	LogicPoint* operator-=(const LogicPoint& other)
	{
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;
		return this;
	}

	int operator*(LogicPoint other)
	{
		return this->x * other.x + this->y * other.y + this->z * other.z;
	}
};


class StandardCoordinateSystem
{
//type definitions
private:
	class Exception
		: public exception
	{
		explicit Exception(char const* const message)
			: exception(message)
		{

		}
	};

    typedef struct ColorLogicPoint
    {
		LogicPoint logicPoint;
        COLORREF color;
        ColorLogicPoint(LogicPoint p, COLORREF cl)
        {
            this->logicPoint = p;
            this->color = cl;
        }
        bool operator==(const ColorLogicPoint& other)
        {
            return logicPoint == other.logicPoint && color == other.color;
        }
    };

    typedef struct DetectLogicPoint
        : ColorLogicPoint
    {
        bool horzDetect;
        bool vertDetect;
        bool horzDetectLine;
        bool vertDetectLine;
        DetectLogicPoint(LogicPoint p, COLORREF cl, bool horzDetect, bool vertDetect, bool horzDetectLine = true, bool vertDetectLine = true)
            : ColorLogicPoint(p, cl)
        {
            this->vertDetect = vertDetect;
            this->horzDetect = horzDetect;
            this->horzDetectLine = horzDetectLine;
            this->vertDetectLine = vertDetectLine;
        }
        bool operator==(const DetectLogicPoint& other)
        {
            return horzDetect == other.horzDetect && vertDetect == other.vertDetect && ColorLogicPoint::operator==(other);
        }
    };

    typedef struct Text
    {
        LogicPoint point;
        CString text;
        Text(LogicPoint p, CString text)
        {
            this->point = p;
            this->text = text;
        }
    };
    enum Side{LEFT, RIGHT, TOP, BOTTOM};
    enum class Axis {X, Y, Z};

	typedef struct AxisInfo
	{
		bool rendering;
		CString renderingName;

		double renderingValues;
		double minRenderingValue;
		double maxRenderingValue;

		double rotationAngle;
	};

public:
    static StandardCoordinateSystem *GetInstance();

	//Main interface
public:
    void Render(CPaintDC *dc);
    void Clear();

    void AddText(LogicPoint p, CString text);
    void AddLogicPoint(LogicPoint point, COLORREF cl = RGB(0, 0, 0), bool xDetection = false, bool yDetection = false, bool xDetectionLine = true, bool yDetectionLine = true);
    void AddLogicPoint(double x, double y, COLORREF cl = RGB(0, 0, 0), bool xDetection = false, bool yDetection = false, bool xDetectionLine = true, bool yDetectionLine = true);
	void AddLogicPoint(double x, double y, double z, COLORREF cl = RGB(0, 0, 0), bool xDetection = false, bool yDetection = false, bool xDetectLine = true, bool yDetectLine = true);

    inline void AddPlot(Plot &plot, COLORREF color = RGB(0,0,0))
    {
        _plots.push_back(std::move(plot));
    }

    inline Plot* LastPlot()
    {
        if (_plots.size())
        {
            return  &_plots[_plots.size() -1];
        }
    }

	inline vector<Plot> GetPlots()
	{
		return _plots;
	}

	//Properties
public:
	inline void EnableGridRendering(bool enable = true)
	{
		_gridRender = enable;
	}

	inline void EnableAxisRendering(Axis axis, bool enable = true)
	{
		_axisInfoMap[axis].rendering = enable;
	}

	inline void SetAxisRenderingName(Axis axis, CString renderName)
    {
		_axisInfoMap[axis].renderingName = renderName;
    }

	inline void SetAxisLogicBounds(Axis axis, double minVal, double maxVal)
	{
		_axisInfoMap[axis].minRenderingValue = minVal;
		_axisInfoMap[axis].maxRenderingValue = minVal;
	}

	inline void SetAxisRenderingValues(Axis axis, double value)
	{
		_axisInfoMap[axis].renderingValues = value;
	}

	//Convertations
public:
    int		   ConvertLogicXToPhys(double x) const;
    int		   ConvertLogicYToPhys(double x) const;
	double	   ConvertPhysXToLogic(double x) const;
    double     ConvertPhysYToLogic(double y) const;
	LogicPoint ConvertPhysPointToLogic(const CPoint& point) const;
	CPoint	   ConvertLogicPointToPhys(const LogicPoint& point) const;

private:
	StandardCoordinateSystem(
		CString xAxisName = "x",
		CString yAxisName = "y",
		COLORREF axisColor = RGB(0, 0, 0),
		COLORREF gridColor = RGB(0, 0, 0)
	);

	StandardCoordinateSystem(
		CString xAxisName = "x",
		CString yAxisName = "y",
		CString zAxisName = "z",
		COLORREF axisColor = RGB(0, 0, 0),
		COLORREF gridColor = RGB(0, 0, 0)
	);

private:
	void RenderArrow(CPaintDC *dc, Side type);
	void CheckHorzBounds(int x);
	void CheckVertBounds(int y);
	void InitSystemParameters(CPaintDC *dc);
	void RenderAxis(CPaintDC *dc);
	virtual void RenderVertAxis(CPaintDC *dc);
	virtual void RenderHorzAxis(CPaintDC *dc);
	void RenderColorPoint(CPaintDC *dc, const ColorLogicPoint &logiLogicPoint);
	void RenderDetectPoint(CPaintDC *dc, const DetectLogicPoint &logiLogicPoint);
	void RenderDivision(CPaintDC *dc, Axis axis, int value);
	void RenderDivision(CPaintDC *dc, Axis axis, double value);
	void RenderTextOnAxis(CPaintDC *dc, Axis axis, double value, CString text);
	void RenderText(CPaintDC *dc, CPoint p, CString text);
	
	//System state
private:
	CPen _axisPen;
	CPen _gridPen;
	vector<Plot> _plots;
	vector<ColorLogicPoint> _points;
	vector<DetectLogicPoint> _detectPoints;
	
	bool _gridRender = false;
	map<Axis, AxisInfo> _axisInfoMap;

	//Constants
private:
	#define DEFAULT_LLEFT_BOUND     -5
	#define DEFAULT_LRIGHT_BOUND     5
	#define DEFAULT_LTOP_BOUND       5
	#define DEFAULT_LBOTTOM_BOUND   -5
	#define DIVISION_HALF_LEN        5
	#define AXIS_INDENT              2
};
