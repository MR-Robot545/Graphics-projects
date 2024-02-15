#ifndef STATE_H
#define STATE_H

#include <fstream>
#include "json.hpp"
#include "serilalizer.h"
#include "structs.h"
#include "Draw.h"
#include "Clip.h"
#include "Fill.h"

using json = nlohmann::ordered_json;

void saveState(std::vector<Drawing> operations)
{
	json j;

	for (int i = 0; i < operations.size(); i++)
	{
		json operation;
		operation["type"] = operations[i].type;
		operation["algorithm"] = operations[i].algorithm;
		operation["parameters"] = operations[i].parameters;

		j[std::to_string(i+1)] = operation;
	}

	std::ofstream file("State.json");
	file << j.dump(4);
}

void loadState(HDC hdc)
{
	std::ifstream file("State.json");
	json j;
	file >> j;

	for (auto it = j.begin(); it != j.end(); ++it)
	{
		json operation = it.value();

		std::string params = operation["parameters"];
		std::stringstream ss(params);
		
		if (operation["type"] == "Line")
		{
			int x1, y1;
			int x2, y2;
			COLORREF c;

			ss.ignore(3);
			ss >> x1;
			ss.ignore(4);
			ss >> y1;
			ss.ignore(4);
			ss >> x2;
			ss.ignore(4);
			ss >> y2;
			ss.ignore(7);
			ss >> c;

			if (operation["algorithm"] == "DDA")
			{
				drawLineDDA(hdc, x1, y1, x2, y2, c, true);
			}
			else if (operation["algorithm"] == "Midpoint")
			{
				 drawLineMidpoint(hdc, x1, y1, x2, y2, c, true);
			}
			else if (operation["algorithm"] == "Parametric")
			{
				 drawLineParametric(hdc, x1, y1, x2, y2, c, true);
			}

		}
		//! done
		else if (operation["type"] == "Circle")
		{
			int x, y, r;
			COLORREF c;
			ss.ignore(3);
			ss >> x;
			ss.ignore(4);
			ss >> y;
			ss.ignore(3);
			ss >> r;
			ss.ignore(7);
			ss >> c;
			
			if (operation["algorithm"] == "Cartesian")
			{
				drawCircleCartesian(hdc, x, y, r, c, true);
			}
			else if (operation["algorithm"] == "Polar")
			{
				drawCirclePolar(hdc, x, y, r, c, true);
			}
			else if (operation["algorithm"] == "Iterative")
			{
				drawCircleItrPolar(hdc, x, y, r, c, true);
			}
			else if (operation["algorithm"] == "Midpoint")
			{
				drawCircleMidpoint(hdc, x, y, r, c, true);
			}
			else if (operation["algorithm"] == "Modified")
			{
				drawCircleModifiedMidpoint(hdc, x, y, r, c, true);
			}
		}
		//! done
		else if (operation["type"] == "Fill")
		{
			COLORREF c;
			if (operation["algorithm"] == "Convex" || operation["algorithm"] == "Non Convex")
			{
				int n;
				ss.ignore(5);
				ss >> n;

				VertexList vlist(n);
				ss.ignore(9);
				// (217,272) (355,115) (574,169) (551,339)
				char dummy; //! To capture the '(' , ' ' and  ',' characters
				int x, y;
				for (int i = 0; i < n; ++i) {
					ss >> dummy >> x >> dummy >> y >> dummy;
					vlist[i] = (Vertex(x, y));
				}

				ss.ignore(9);
				ss >> c;

				operation["algorithm"] == "Convex" ? convexFill(hdc, vlist, n, c, true) 
					: nonConvexFill(hdc, vlist, n, c, true);
			}
			else if (operation["algorithm"] == "Lines" || operation["algorithm"] == "Circles")
			{
				int xc, yc, quart, r;
				
				ss.ignore(3);
				ss >> xc;
				ss.ignore(4);
				ss >> yc;
				ss.ignore(3);
				ss >> r;
				ss.ignore(7);
				ss >> quart;
				ss.ignore(7);
				ss >> c;

				operation["algorithm"] == "Lines" ? fillCircleQuarter(hdc, xc, yc, r, quart, c, true)
					: FilledCircleQuarterWithCircles(hdc, xc, yc, r, quart, c, true);
			}
			else if (operation["algorithm"] == "Flood" || operation["algorithm"] == "NR Flood")
			{
				int x, y;
				COLORREF Cb, Cf;

				ss.ignore(2);
				ss >> x;
				ss.ignore(3);
				ss >> y;
				ss.ignore(4);
				ss >> Cb;
				ss.ignore(4);
				ss >> Cf;
			
				operation["algorithm"] == "Flood" ? FloodFill(hdc, x, y, Cb, Cf, true)
					: NRFloodFill(hdc, x, y, Cb, Cf, true);
			}
			else if (operation["algorithm"] == "Hermit" || operation["algorithm"] == "Bezier")
			{
				RECT rct;
				ss.ignore(6);
				ss >> rct.left;
				ss.ignore(6);
				ss >> rct.top;
				ss.ignore(8);
				ss >> rct.right;
				ss.ignore(9);
				ss >> rct.bottom;
				ss.ignore(7);
				ss >> c;

				operation["algorithm"] == "Flood" ? FillBezierRectangle(hdc, rct, c, true)
					: DrawHermitCurveSquare(hdc, rct, c, true);
			}

		}
		//! done
		else if (operation["type"] == "Spline")
		{
			int n, numpix;
			double c;
			ss.ignore(5);
			ss >> n;

			VertexList vlist(n);
			ss.ignore(9);

			// (217,272) (355,115) (574,169) (551,339)
			char dummy; //! To capture the '(' , ' ' and  ',' characters
			int x, y;
			for (int i = 0; i < n; ++i) {
				ss >> dummy >> x >> dummy >> y >> dummy;
				vlist[i] = (Vertex(x, y));
			}

			ss.ignore(5);
			ss >> c;
			ss.ignore(8);
			ss >> numpix;

			DrawCardinalSpline(hdc, vlist, n, c, numpix, true);
		}
		//! done
		else if (operation["type"] == "Ellipse")
		{
			int xc, yc;
			int A, B;
			COLORREF c;

			ss.ignore(3);
			ss >> xc;
			ss.ignore(4);
			ss >> yc;
			ss.ignore(3);
			ss >> A;
			ss.ignore(3);
			ss >> B;
			ss.ignore(7);
			ss >> c;


			if (operation["algorithm"] == "Cartessian")
			{
				drawEllipseCartesian(hdc, xc, yc, A, B, c, true);
			}
			else if (operation["algorithm"] == "Polar")
			{
				drawEllipsePolar(hdc, xc, yc, A, B, c, true);
			}
			else if (operation["algorithm"] == "Midpoint")
			{
				drawEllipseMidpoint(hdc, xc, yc, A, B, c, true);
			}

		}
		//! done
		else if (operation["type"] == "Clip")
		{
			int xleft, xright, ybottom, ytop, yleft, side;
			COLORREF c;

			if (operation["algorithm"] == "Rectangle Point")
			{
				int x, y;
				ss.ignore(2);
				ss >> x;
				ss.ignore(3);
				ss >> y;

				ss.ignore(7);
				ss >> xleft;
				ss.ignore(6);
				ss >> ytop;
				ss.ignore(8);
				ss >> xright;
				ss.ignore(9);
				ss >> ybottom;
				ss.ignore(7);
				ss >> c;
				DrawRectangle(hdc, xleft, ytop, xright, ybottom);
				rectanglePointClipping(hdc, x, y, xleft, ytop, xright, ybottom, c, true);
			}
			else if (operation["algorithm"] == "Rectangle Line")
			{
				int x1, y1, x2, y2;
				ss.ignore(3);
				ss >> x1;
				ss.ignore(4);
				ss >> y1;
				ss.ignore(4);
				ss >> x2;
				ss.ignore(4);
				ss >> y2;

				ss.ignore(7);
				ss >> xleft;
				ss.ignore(6);
				ss >> ytop;
				ss.ignore(8);
				ss >> xright;
				ss.ignore(9);
				ss >> ybottom;
				ss.ignore(7);
				ss >> c;

				DrawRectangle(hdc, xleft, ytop, xright, ybottom);
				rectangleLineClipping(hdc, x1, y1, x2, y2, xleft, ytop, xright, ybottom, c, true);
			}
			else if (operation["algorithm"] == "Rectangle Polygon")
			{
				int n;
				ss.ignore(5);
				ss >> n;

				VertexList vlist(n);
				ss.ignore(9);

				// (217,272) (355,115) (574,169) (551,339)
				char dummy; //! To capture the '(' , ' ' and  ',' characters
				int x, y;
				for (int i = 0; i < n; ++i) {
					ss >> dummy >> x >> dummy >> y >> dummy;
					vlist[i] = (Vertex(x, y));
				}

				ss.ignore(9);
				ss >> xleft;
				ss.ignore(6);
				ss >> ytop;
				ss.ignore(8);
				ss >> xright;
				ss.ignore(9);
				ss >> ybottom;

				DrawRectangle(hdc, xleft, ytop, xright, ybottom);
				rectanglePolygonClip(hdc, vlist, n, xleft, ytop, xright, ybottom, true);
			}
			else if (operation["algorithm"] == "Square Point")
			{
				int x, y;
				ss.ignore(2);
				ss >> x;
				ss.ignore(3);
				ss >> y;

				ss.ignore(7);
				ss >> xleft;
				ss.ignore(7);
				ss >> yleft;
				ss.ignore(6);
				ss >> side;
				ss.ignore(7);
				ss >> c;

				Square(hdc, xleft, yleft, side, c);
				squarePointClipping(hdc, x, y, xleft, yleft, side, c, true);
			}
			else if (operation["algorithm"] == "Square Line")
			{
				int x1, y1, x2, y2;
				ss.ignore(3);
				ss >> x1;
				ss.ignore(4);
				ss >> y1;
				ss.ignore(4);
				ss >> x2;
				ss.ignore(4);
				ss >> y2;

				ss.ignore(7);
				ss >> xleft;
				ss.ignore(7);
				ss >> yleft;
				ss.ignore(6);
				ss >> side;
				ss.ignore(7);
				ss >> c;

				Square(hdc, xleft, yleft, side, c);
				squareLineClipping(hdc, x1, y1, x2, y2, xleft, yleft,side, c, true);
			}
		}	
		//! done
	}
}

#endif