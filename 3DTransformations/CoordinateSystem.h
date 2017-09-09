#pragma once
#include <vector>
#include <map>
#include <exception>
#include <memory>
using namespace std;


namespace cs
{


	struct LogicPoint 
	{
	public:
		LogicPoint();
		LogicPoint(double x, double y);
		LogicPoint(double x, double y, double z);

	public:
		inline bool operator==(const LogicPoint& other);
		inline LogicPoint* operator+=(const LogicPoint& other);
		inline LogicPoint* operator-=(const LogicPoint& other);
		inline int operator*(LogicPoint other);

	public:
		double x, y, z;
	};


	class GraphicsObject;
	class CoordinateSystem
	{
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
		enum class Axis { X, Y, Z };

		typedef struct AxisInfo
		{
			bool rendering;
			CString renderingName;

			double divisionValue;
			double minRenderingValue;
			double maxRenderingValue;

			double rotationAngle;
		};

		typedef struct RenderingContext
		{

		};

	public:
		static CoordinateSystem *GetInstance();

		//Main interface
	public:
		void Render(CPaintDC *dc);
		void Clear();

	public:
		inline void AddText(LogicPoint p, CString text);
		inline void AddLogicPoint(LogicPoint point, COLORREF cl = RGB(0, 0, 0), bool xDetection = true, bool yDetection = true, bool zDetection = true, bool xDetectionLine = true, bool yDetectionLine = true, bool zDetectionLine = true);
		inline void AddLogicPoint(double x, double y, COLORREF cl = RGB(0, 0, 0), bool xDetection = false, bool yDetection = false, bool xDetectionLine = true, bool yDetectionLine = true);
		inline void AddLogicPoint(double x, double y, double z, COLORREF cl = RGB(0, 0, 0), bool xDetection = false, bool yDetection = false, bool zDetection = true, bool xDetectLine = true, bool yDetectLine = true, bool zDetectionLine = true);
		inline void AddGraphicObject(GraphicsObject* obj, COLORREF color = RGB(0, 0, 0));
		inline GraphicsObject* LastGraphicObject();
		inline vector<shared_ptr<GraphicsObject>> GetGraphicObejctsList();

		//Properties
	public:
		inline void EnableGridRendering(bool enable = true);
		inline void EnableAxisRendering(Axis axis, bool enable = true);
		inline void SetAxisRenderingName(Axis axis, CString renderName);
		inline void SetAxisLogicBounds(Axis axis, double minVal, double maxVal);
		inline void SetAxisRenderingValues(Axis axis, double value);

		//Convertations
	public:
		int		   ConvertLogicXToPhys(double x) const;
		int		   ConvertLogicYToPhys(double x) const;
		double	   ConvertPhysXToLogic(double x) const;
		double     ConvertPhysYToLogic(double y) const;
		LogicPoint ConvertPhysPointToLogic(const CPoint& point) const;
		CPoint	   ConvertLogicPointToPhys(const LogicPoint& point) const;

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

	private:
		void InitRenderingContext(CPaintDC *dc, RenderingContext&);

	private:
		void CheckAxisBounds(Axis axis, double v);

	private:
		void RenderAxes(CPaintDC *dc, const RenderingContext&);
		void RenderGraphicObjects(CPaintDC *dc, const RenderingContext&);
		void RenerColorPoints(CPaintDC *dc, const RenderingContext&);
		void RenderDetectPoints(CPaintDC *dc, const RenderingContext&);
		void RenderTexts(CPaintDC *dc, const RenderingContext&);

	private:
		void RenderAxis(CPaintDC *dc, const RenderingContext&, const AxisInfo&);

		void RenderArrow(CPaintDC *dc, const RenderingContext&, Side type);

		void RenderColorPoint(CPaintDC *dc, const RenderingContext&, const ColorLogicPoint&);
		void RenderDetectPoint(CPaintDC *dc, const RenderingContext&, const DetectLogicPoint&);

		void RenderDivision(CPaintDC *dc, const RenderingContext&, Axis axis, double value);

		void RenderText(CPaintDC *dc, const RenderingContext&, const Text& text);
		void RenderTextOnAxis(CPaintDC *dc, const RenderingContext&, Axis axis, double value, CString text);

		//System state
	private:
		CPen _axisPen;
		CPen _gridPen;
		vector<Text> _texts;
		vector<shared_ptr<GraphicsObject>> _objects;
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


	class GraphicsObject
	{
	public:
		typedef shared_ptr<GraphicsObject> Ptr;

		GraphicsObject() = default;
		GraphicsObject(vector<LogicPoint> &points, int penStyle = PS_SOLID, int penWidth = 1, COLORREF penColor = RGB(0, 0, 0));
		GraphicsObject(const GraphicsObject& other);
		GraphicsObject(const GraphicsObject&& other);
		~GraphicsObject();

		friend class CoordinateSystem;

	public:
		inline GraphicsObject* operator=(const GraphicsObject& other);
		inline LogicPoint& operator[](int index);

	protected:
		virtual void Render(const CoordinateSystem*, const CoordinateSystem::RenderingContext&, CPaintDC *dc) = 0;

	protected:
		vector<LogicPoint> _points;

	protected:
		int _penStyle;
		int _penWidth;
		COLORREF _penColor;
	};


}
