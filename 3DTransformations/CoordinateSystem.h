#pragma once
#include <map>
#include <memory>
#include <vector>
#include <exception>
#include "PrimitiveTypes.h"
using namespace std;


namespace cs
{


	struct LogicPoint 
	{
	public:
		LogicPoint();
		LogicPoint(double x, double y);
		LogicPoint(double x, double y, double z);
		LogicPoint(const LogicPoint& other);

	public:
	 	LogicPoint& operator=(const LogicPoint& other);
		LogicPoint* operator+=(const LogicPoint& other);
		LogicPoint* operator-=(const LogicPoint& other);

	public:
		bool operator==(const LogicPoint& other);
		int operator*(LogicPoint other);

	public:
		double x, y, z;
	};


	class GraphicsObject;
	class CoordinateSystem
	{
	public:
		enum class Axis {
			X = 0,
			Y = 1,
			Z = 2 
		};
		//Type definitions
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
			ColorLogicPoint(const ColorLogicPoint& other)
			{
				this->logicPoint = other.logicPoint;
				this->color = other.color;
			}
			ColorLogicPoint* operator=(const ColorLogicPoint& other)
			{
				logicPoint = other.logicPoint;
				color = other.color;
				return this;
			}
			bool operator==(const ColorLogicPoint& other)
			{
				return logicPoint == other.logicPoint && color == other.color;
			}
		};

		typedef struct DetectLogicPoint
			: ColorLogicPoint
		{
		public:
			DetectLogicPoint(LogicPoint p, COLORREF cl, bool xDetection, bool yDetection, bool zDetection, bool xDetectionLine = true, bool yDetectionLine = true, bool zDetectionLine = true)
				: ColorLogicPoint(p, cl)
			{
				this->xDetection = xDetection;
				this->yDetection = yDetection;
				this->zDetection = zDetection;
				this->xDetectionLine = xDetectionLine;
				this->yDetectionLine = yDetectionLine;
				this->zDetectionLine = zDetectionLine;
			}

		public:
			bool operator==(const DetectLogicPoint& other)
			{
				return 
					xDetection == other.xDetection && yDetection == other.yDetection && zDetection == other.zDetection &&
					xDetectionLine == other.xDetectionLine && yDetectionLine == other.yDetectionLine && zDetectionLine == other.zDetectionLine &&
					ColorLogicPoint::operator==(other);
			}

		public:
			bool xDetection;
			bool yDetection;
			bool zDetection;
			bool xDetectionLine;
			bool yDetectionLine;
			bool zDetectionLine;
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
		enum Side { LEFT, RIGHT, TOP, BOTTOM };
		
		typedef struct AxisInfo
		{
			bool rendering;
			CString renderingName;

			double scale = 5;
			double divisionValue;

			HomogeneousPoint<double> minPoint;
			HomogeneousPoint<double> maxPoint;
		};

	public:
		static CoordinateSystem *GetInstance();

		//Main interface
	public:
		void Render(CPaintDC *dc);
		void Zoom(double val);
		void Move(double dx, double dy);
		void RotateAroundAxis(Axis axis, double deltaAngle);
		void Clear();

	public:
		inline void AddText(LogicPoint p, CString text);
		void AddLogicPoint(LogicPoint point, COLORREF cl = RGB(0, 0, 0), bool xDetection = true, bool yDetection = true, bool zDetection = true, bool xDetectionLine = true, bool yDetectionLine = true, bool zDetectionLine = true);
		void AddLogicPoint(double x, double y, COLORREF cl = RGB(0, 0, 0), bool xDetection = false, bool yDetection = false, bool xDetectionLine = true, bool yDetectionLine = true);
		void AddLogicPoint(double x, double y, double z, COLORREF cl = RGB(0, 0, 0), bool xDetection = false, bool yDetection = false, bool zDetection = true, bool xDetectLine = true, bool yDetectLine = true, bool zDetectionLine = true);
		void AddGraphicObject(GraphicsObject* obj, COLORREF color = RGB(0, 0, 0));
		inline GraphicsObject* LastGraphicObject();
		inline const vector<shared_ptr<GraphicsObject>>& GetGraphicObejctsList();

		//Properties
	public:
		inline void EnableGridRendering(bool enable = true);
		inline void EnableAxisRendering(Axis axis, bool enable = true);

		inline void SetAxisRenderingName(Axis axis, CString renderName);
		inline void SetAxisRenderingBorders(Axis axis, double minVal, double maxVal);
		inline void SetAxisRenderingDivisions(Axis axis, double value);

		inline void SetPhysOrigin(CPoint physOrigin);
		inline CPoint GetPhysOrigin() const;

		const HomogeneousPoint<double>& GetWatcherVector() const;

		//Convertations
	public:
		LogicPoint ConvertPhysPointToLogic(const CPoint& point);
		CPoint	   ConvertLogicPointToPhys(const LogicPoint& point) const;
		CPoint	   ConvertLogicPointToPhys(const HomogeneousPoint<double>& point) const;
		
	private:
		CoordinateSystem(
			CString xAxisName = "x",
			CString yAxisName = "y",
			COLORREF axisColor = RGB(0, 0, 0),
			COLORREF gridColor = RGB(0, 0, 0)
		);
		CoordinateSystem(
			CString xAxisName = "x",
			CString yAxisName = "y",
			CString zAxisName = "z",
			COLORREF axisColor = RGB(0, 0, 0),
			COLORREF gridColor = RGB(0, 0, 0)
		);
		CoordinateSystem::~CoordinateSystem();

		friend class GraphicsObject;
		friend class LinearGraphicsObject;
		friend class Polygon;
		friend class Polyhedron;

	private:
		void CheckAxisBounds(Axis axis, double v);

	private:
		void RenderAxes(CPaintDC *dc);
		void RenderGraphicObjects(CPaintDC *dc);
		void RenerColorPoints(CPaintDC *dc);
		void RenderDetectPoints(CPaintDC *dc);
		void RenderTexts(CPaintDC *dc);

	private:
		void RenderAxis(CPaintDC *dc, const AxisInfo&);

		void RenderArrow(CPaintDC *dc, Side type);

		void RenderColorPoint(CPaintDC *dc, const ColorLogicPoint&);
		void RenderDetectPoint(CPaintDC *dc, const DetectLogicPoint&);

		void RenderDivision(CPaintDC *dc, Axis axis, double value);

		void RenderText(CPaintDC *dc, const Text& text);
		void RenderText(CPaintDC *dc, const CPoint& physPoint, CString text);
		void RenderTextOnAxis(CPaintDC *dc, Axis axis, double value, CString text);

	private:
		void Init();
		Matrix<double>& GetXAxisRotationMatrix(double angle);
		Matrix<double>& GetYAxisRotationMatrix(double angle);
		Matrix<double>& GetZAxisRotationMatrix(double angle);

	private:
		vector<Text> _texts;
		vector<shared_ptr<GraphicsObject>> _objects;
		vector<ColorLogicPoint> _points;
		vector<DetectLogicPoint> _detectPoints;

		//System state
	private:
		double _scale = 1;
		CPoint _physOrigin;
		HomogeneousPoint<double> _watcherVector;

		bool _gridRender = false;
		map<Axis, AxisInfo> _axisInfoMap;
		Matrix<double> _projectionMatrix;

		//Constants
	private:
		#define DEFAULT_LLEFT_BOUND     -5
		#define DEFAULT_LRIGHT_BOUND     5
		#define DEFAULT_LTOP_BOUND       5
		#define DEFAULT_LBOTTOM_BOUND   -5
		#define DIVISION_HALF_LEN        5
		#define AXIS_INDENT              2
	};


	class GraphicsObject
	{
	public:
		typedef shared_ptr<GraphicsObject> Ptr;
		friend class CoordinateSystem;

		GraphicsObject(int penStyle = PS_SOLID, int penWidth = 1, COLORREF penColor = RGB(0, 0, 0));
		GraphicsObject(const GraphicsObject& other);
		~GraphicsObject();

	public:
		inline virtual GraphicsObject* operator=(const GraphicsObject& other);

	protected:
		virtual void Render(const CoordinateSystem*, CPaintDC *dc) = 0;

	protected:
		int _penStyle;
		int _penWidth;
		COLORREF _penColor;
	};


	class LinearGraphicsObject
		: public GraphicsObject
	{
	public:
		friend class CoordinateSystem;

		LinearGraphicsObject(const vector<LogicPoint>& points, int penStyle = PS_SOLID, int penWidth = 1, COLORREF penColor = RGB(0, 0, 0));
		LinearGraphicsObject(const LinearGraphicsObject& other);

	protected:
		virtual void Render(const CoordinateSystem*, CPaintDC *dc) override;

	protected:
		vector<LogicPoint> _points;
	};


	class Polygon
		: public LinearGraphicsObject
	{
	public:
		friend class CoordinateSystem;
		friend class Polyhedron;

		typedef shared_ptr<Polygon> Ptr;

	public:
		Polygon(const vector<LogicPoint>& points, bool rightHandNormalVector = true, int penStyle = PS_SOLID, int penWidth = 1, COLORREF penColor = RGB(0, 0, 0));
		Polygon(const Polygon& other);

	protected:
		virtual void Render(const CoordinateSystem*, CPaintDC *dc) override;

	private:
		void Init();

	private:
		bool _rightHandNormalVector;
		HomogeneousPoint<double> _nVector;
	};


	class Polyhedron
		: public GraphicsObject
	{
	public:
		friend class CoordinateSystem;

	public:
		Polyhedron(const vector<Polygon::Ptr>& facets, int penStyle = PS_SOLID, int penWidth = 1, COLORREF penColor = RGB(0, 0, 0));
		Polyhedron(const Polyhedron& other);

	protected:
		virtual void Render(const CoordinateSystem*, CPaintDC *dc) override;

	private:
		vector<Polygon::Ptr> _facets;
	};


}
