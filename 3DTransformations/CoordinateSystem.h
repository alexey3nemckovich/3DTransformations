#pragma once
#include <map>
#include <memory>
#include <vector>
#include <exception>
#include "PrimitiveTypes.h"
#include "PrimitiveTypesTemplates.h"
//
#include "ZBuffer.h"
#include "GraphicObject.h"
using namespace std;


namespace cs
{


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
			ColorLogicPoint(const LogicPoint& p, COLORREF cl)
			{
				this->logicPoint = p;
				this->color = cl;
			}
      ColorLogicPoint(const ColorLogicPoint& other) = default;
			ColorLogicPoint* operator=(const ColorLogicPoint& other)
			{
				logicPoint = other.logicPoint;
				color = other.color;
				return this;
			}
      ColorLogicPoint& operator=(ColorLogicPoint&& o) = default;
      ColorLogicPoint(ColorLogicPoint&& o) = default;
			bool operator==(const ColorLogicPoint& other)
			{
				return logicPoint == other.logicPoint && color == other.color;
			}
		};

		typedef struct DetectLogicPoint
			: ColorLogicPoint
		{
		public:
			DetectLogicPoint(const LogicPoint& p, COLORREF cl, bool xDetection, bool yDetection, bool zDetection, bool xDetectionLine = true, bool yDetectionLine = true, bool zDetectionLine = true)
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
			Text(const LogicPoint& p,const CString& text)
			{
				this->point = p;
				this->text = text;
			}
		};
		
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
		enum class RenderingAlgorithm
		{
			STANDARD,
			ZBUFFER,
		};
		static CoordinateSystem *GetInstance();

		//Main interface
	public:
		void Render(CPaintDC *dc, CWnd *wnd);
		void Zoom(double val);
		void Move(double dx, double dy, double dz);
		void MoveOriginPhysPoint(double dx, double dy);
		void SetOriginTo(const LogicPoint& p);
		void SetRenderingAlgorithm(RenderingAlgorithm);
		void RotateAroundAxis(CoordinateAxisName axis, double deltaAngle);
		void RotateAroundAxis(const std::pair<LogicPoint, LogicPoint>& axisPoints, double deltaAngle);
		void Clear();

		//Vision interface
	public:
		double GetPointDistanceToProjectionPlane(const LogicPoint&) const;

	public:
		void AddText(LogicPoint p, CString text);
		void AddLogicPoint(const LogicPoint& point, COLORREF cl = RGB(0, 0, 0), bool xDetection = true, bool yDetection = true, bool zDetection = true, bool xDetectionLine = true, bool yDetectionLine = true, bool zDetectionLine = true);
		void AddLogicPoint(double x, double y, COLORREF cl = RGB(0, 0, 0), bool xDetection = false, bool yDetection = false, bool xDetectionLine = true, bool yDetectionLine = true);
		void AddLogicPoint(double x, double y, double z, COLORREF cl = RGB(0, 0, 0), bool xDetection = false, bool yDetection = false, bool zDetection = true, bool xDetectLine = true, bool yDetectLine = true, bool zDetectionLine = true);
		void AddGraphicObject(GraphicObject* obj, bool deleteOnDestroy = true);
		GraphicObject* LastGraphicObject();
		const vector<shared_ptr<GraphicObject>>& GetGraphicObejctsList() const;

		//Properties
	public:
		void EnableInvisibleLinesAsDash(bool enable = true);
		inline void EnableGridRendering(bool enable = true);
		inline void EnableAxisRendering(CoordinateAxisName axis, bool enable = true);

		inline void SetAxisRenderingName(CoordinateAxisName axis, CString renderName);
		inline void SetAxisRenderingBorders(CoordinateAxisName axis, double minVal, double maxVal);
		inline void SetAxisRenderingDivisions(CoordinateAxisName axis, double value);

		inline void SetPhysOrigin(CPoint physOrigin);
		CPoint GetPhysOrigin() const;

		const HomogeneousPoint<double>& GetWatcherVector() const;

		//Convertations
	public:
		LogicPoint ConvertPhysPointToLogic(const CPoint& point);
		LogicPoint ConvertToProjectionSytemPoint(const LogicPoint& point) const;
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

	private:
		void CheckAxisBounds(CoordinateAxisName axis, double v);

	private:
		void StandardRenderingAlgorithm(CDC *dc);
		void ZBufferRenderingAlgorithm(CDC *dc);

	private:
		void RenderAxes(CDC *dc);
		void RenderGraphicObjects(CDC *dc);
		void RenerColorPoints(CDC *dc);
		void RenderDetectPoints(CDC *dc);
		void RenderTexts(CDC *dc);

	private:
		void RenderAxis(CDC *dc, const AxisInfo&);

		void RenderArrow(CDC *dc, Side type);

		void RenderColorPoint(CDC *dc, const ColorLogicPoint&);
		void RenderDetectPoint(CDC *dc, const DetectLogicPoint&);

		void RenderDivision(CDC *dc, CoordinateAxisName axis, double value);

		void RenderText(CDC *dc, const Text& text);
		void RenderText(CDC *dc, const CPoint& physPoint, CString text);
		void RenderTextOnAxis(CDC *dc, CoordinateAxisName axis, double value, CString text);

	private:
		void Init();
		Matrix<double>& GetXAxisRotationMatrix(double angle);
		Matrix<double>& GetYAxisRotationMatrix(double angle);
		Matrix<double>& GetZAxisRotationMatrix(double angle);
		Matrix<double>& GetTransferenceMatrix(double dx, double dy, double dz);

	private:
		vector<Text>				      _texts;
		vector<GraphicObject::Ptr>	      _objects;
		vector<ColorLogicPoint>			  _points;
		vector<DetectLogicPoint>		  _detectPoints;

		//System state
	private:
		double							  _scale = 1;
		CPoint							  _physOrigin;
		LogicPoint						  _origin;
		HomogeneousPoint<double>		  _watcherVector;
		RenderingAlgorithm				  _renderingAlg = RenderingAlgorithm::STANDARD;

		bool						      _invisibleLinesAsDash = false;
		bool							  _gridRender = false;
		map<CoordinateAxisName, AxisInfo> _axisInfoMap;
		Matrix<double>					  _projectionMatrix;
        ZBuffer                           _buff;

		//Constants
	private:
		enum class Defaults
		{
			DEFAULT_LLEFT_BOUND		 = -5,
			DEFAULT_LRIGHT_BOUND	 =	5,
			DEFAULT_LTOP_BOUND		 =  5,
			DEFAULT_LBOTTOM_BOUND	 = -5,
			DIVISION_HALF_LEN		 =  5,
			AXIS_INDENT				 =  2,
		};
	};


}
