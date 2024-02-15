#ifndef DRAW_H
#define DRAW_H

#include "structs.h"
#include "serilalizer.h"
using namespace std;

//! utility to draw 4 points based on symetry in ellipse
void draw4Points(HDC hdc, int xc, int yc, int x, int y, COLORREF C)
{
	SetPixel(hdc, xc + x, yc + y, C);
	SetPixel(hdc, xc + x, yc - y, C);
	SetPixel(hdc, xc - x, yc - y, C);
	SetPixel(hdc, xc - x, yc + y, C);
}

//! utility to draw 4 points based on symetry in circle
void draw8points(HDC hdc, int xc, int yc, int x, int y, COLORREF c) {
	SetPixel(hdc, xc + x, yc + y, c);
	SetPixel(hdc, xc + x, yc - y, c);
	SetPixel(hdc, xc - x, yc + y, c);
	SetPixel(hdc, xc - x, yc - y, c);
	SetPixel(hdc, xc + y, yc + x, c);
	SetPixel(hdc, xc + y, yc - x, c);
	SetPixel(hdc, xc - y, yc + x, c);
	SetPixel(hdc, xc - y, yc - x, c);
}

/*! ============================ Ellipse ============================*/

//! Cartesian elipse algorithm
void drawEllipseCartesian(HDC hdc, int xc, int yc, int A, int B, COLORREF C, bool load = false)
{
	if (!load)
	{
		std::string params = serializeParameters("xc", xc, "yc", yc, "A", A, "B", B, "Color", C);
		Drawing Ellipse("Ellipse", "Cartessian", params);
		operations.push_back(Ellipse);
	}

	int x = 0;
	double y = B;
	draw4Points(hdc, xc, yc, 0, B, C);
	while ((double)x * B * B < y * A * A)
	{
		x++;
		y = B * sqrt(1.0 - (double)x * x / ((double)A * A));
		draw4Points(hdc, xc, yc, x, std::round(y), C);
	}
	int y1 = 0;
	double x1 = A;
	draw4Points(hdc, xc, yc, A, 0, C);
	while (x1 * B * B > (double)y1 * A * A)
	{
		y1++;
		x1 = A * sqrt(1.0 - (double)y1 * y1 / ((double)B * B));
		draw4Points(hdc, xc, yc, std::round(x1), y1, C);
	}
}

//! Bresnham's Ellipse midpoint algorithm
void drawEllipseMidpoint(HDC hdc, int xc, int yc, int A, int B, COLORREF C, bool load = false)
{
	if (!load)
	{
		std::string params = serializeParameters("xc", xc, "yc", yc, "A", A, "B", B, "Color", C);
		Drawing Ellipse("Ellipse", "Midpoint", params);
		operations.push_back(Ellipse);
	}
	float dx, dy, d1, d2, x, y;
	x = 0;
	y = B;

	//! decision parameter of region 1
	d1 = (B * B) - (A * A * B) + (0.25 * A * A);
	dx = 2 * B * B * x;
	dy = 2 * A * A * y;

	//! region 1
	while (dx < dy)
	{
		draw4Points(hdc, xc, yc, x, y, C);

		if (d1 < 0)
		{
			x++;
			dx = dx + (2 * B * B);
			d1 = d1 + dx + (B * B);
		}
		else
		{
			x++;
			y--;
			dx = dx + (2 * B * B);
			dy = dy - (2 * A * A);
			d1 = d1 + dx - dy + (B * B);
		}
	}

	//! decision parameter of region 2
	d2 = ((B * B) * ((x + 0.5) * (x + 0.5))) + (((double)A * A) * (((double)y - 1) * ((double)y - 1))) - ((double)A * A * B * B);

	//! region 2
	while (y >= 0)
	{

		draw4Points(hdc, xc, yc, x, y, C);

		if (d2 > 0)
		{
			y--;
			dy = dy - (2 * A * A);
			d2 = d2 + (A * A) - dy;
		}
		else
		{
			y--;
			x++;
			dx = dx + (2 * B * B);
			dy = dy - (2 * A * A);
			d2 = d2 + dx - dy + (A * A);
		}
	}
}

//! polar ellipse algorithm
void drawEllipsePolar(HDC hdc, int xc, int yc, int A, int B, COLORREF C, bool load = false)
{
	if (!load)
	{
		std::string params = serializeParameters("xc", xc, "yc", yc, "A", A, "B", B, "Color", C);
		Drawing Ellipse("Ellipse", "Polar", params);
		operations.push_back(Ellipse);
	}
	double theta = 0, pi = 3.1416;;
	double dtheta = (1.0f / ((A + B) / 2.0f));
	int x, y;

	while (theta < 2 * pi)
	{
		x = A * cos(theta);
		y = B * sin(theta);
		draw4Points(hdc, xc, yc, x, y, C);
		theta += dtheta;
	}
}

/*! ============================ Line ============================*/

void drawLineParametric(HDC hdc, int x1, int y1, int x2, int y2, COLORREF c, bool load = false) {

	if (!load)
	{
		std::string params = serializeParameters("x1", x1, "y1", y1, "x2", x2, "y2", y2, "Color", c);
		Drawing Line("Line", "Parametric", params);
		operations.push_back(Line);
	}
	int dx = x2 - x1;
	int dy = y2 - y1;
	int steps = max(abs(dx), abs(dy));

	float increment = 1.0 / steps;

	float x = x1;
	float y = y1;


	for (int i = 0; i <= steps; i++) {
		SetPixel(hdc, x, y, c);
		x += dx * increment;
		y += dy * increment;
	}
}

void drawLineDirect(HDC hdc, int x1, int y1, int x2, int y2, COLORREF c, bool load = false) {
	
	if (!load)
	{
		std::string params = serializeParameters("x1", x1, "y1", y1, "x2", x2, "y2", y2, "Color", c);
		Drawing Line("Line", "Direct", params);
		operations.push_back(Line);
	}
	int dx = x2 - x1, dy = y2 - y1;
	if (abs(dx) > abs(dy)) {
		if (x1 > x2)swap(x1, x2), swap(y1, y2);
		int x = x1;
		SetPixel(hdc, x1, y1, c);
		while (x < x2) {
			double y = y1 + (x - x1) * ((double)(y2 - y1) / (x2 - x1));
			SetPixel(hdc, x++, (int)round(y), c);
		}
	}
	else {
		if (y1 > y2)swap(x1, x2), swap(y1, y2);
		int y = y1;
		SetPixel(hdc, x1, y1, c);
		while (y < y2) {
			double x = x1 + (y - y1) * ((double)(x2 - x1) / (y2 - y1));
			SetPixel(hdc, (int)round(x), y++, c);
		}
	}
}

void drawLineDDA(HDC hdc, int x1, int y1, int x2, int y2, COLORREF c, bool load = false) {

	if (!load)
	{
		std::string params = serializeParameters("x1", x1, "y1", y1, "x2", x2, "y2", y2, "Color", c);
		Drawing Line("Line", "DDA", params);
		operations.push_back(Line);
	}
	int dx = x2 - x1, dy = y2 - y1;
	if (abs(dx) > abs(dy)) {
		if (x1 > x2)swap(x1, x2), swap(y1, y2);
		int x = x1;
		double y = y1;
		double m = (double)dy / dx;
		while (x < x2)x++, y += m, SetPixel(hdc, x, (int)round(y), c);
	}
	else {
		if (y1 > y2)swap(x1, x2), swap(y1, y2);
		int y = y1;
		double x = x1;
		double mi = (double)dx / dy;
		while (y < y2)y++, x += mi, SetPixel(hdc, (int)round(x), y, c);
	}
}

void drawLineMidpoint(HDC hdc, int x1, int y1, int x2, int y2, COLORREF c, bool load = false) {

	if (!load)
	{
		std::string params = serializeParameters("x1", x1, "y1", y1, "x2", x2, "y2", y2, "Color", c);
		Drawing Line("Line", "Midpoint", params);
		operations.push_back(Line);
	}
	int dx = x2 - x1, dy = y2 - y1;
	if (abs(dx) >= abs(dy)) {
		if (x1 > x2)swap(x1, x2), swap(y1, y2);
		int x = x1, y = y1;
		int incre = y1 > y2 ? -1 : 1;
		dx = x2 - x1, dy = (y2 - y1) * incre;
		int d = dx - 2 * dy;
		int change1 = 2 * (dx - dy);
		int change2 = -2 * dy;
		SetPixel(hdc, x, y, c);
		//dx>dy
			// dx>0  dy>0
			// dx>0  dy<0
			// dx<0  dy>0
			// dx<0  dy<0
			// dx>0 && dy>0
		while (x <= x2) {
			if (d <= 0) {
				y += incre;
				d += change1;
			}
			else {
				d += change2;
			}
			x++;
			SetPixel(hdc, x, y, c);
		}
	}
	else {
		if (y1 > y2)swap(y1, y2), swap(x1, x2);
		int incre = x1 > x2 ? -1 : 1;
		int x = x1, y = y1;
		dx = (x2 - x1) * incre, dy = y2 - y1;
		int d = 2 * dx - dy;
		int change1 = 2 * (dx - dy);
		int change2 = 2 * dx;
		SetPixel(hdc, x, y, c);
		while (y <= y2) {
			if (d >= 0) {
				x += incre;
				d += change1;
			}
			else {
				d += change2;
			}
			y++;
			SetPixel(hdc, x, y, c);
		}
	}
}

/*! ============================ Circle ============================*/

void drawCircleCartesian(HDC hdc, int xc, int yc, int R, COLORREF c, bool load = false) {

	if (!load)
	{
		std::string params = serializeParameters("xc", xc, "yc", yc, "r", R, "Color", c);
		Drawing Circle("Circle", "Cartesian", params);
		operations.push_back(Circle);
	}
	int x = 0;
	double y = R * 1.0;
	draw8points(hdc, xc, yc, 0, R, c);
	R *= R;
	while (x <= y) {
		x++; y = sqrt(R - x * x);
		draw8points(hdc, xc, yc, x, (int)round(y), c);
	}

}

void drawCirclePolar(HDC hdc, int xc, int yc, int R, COLORREF color, bool load = false)
{
	if (!load)
	{
		std::string params = serializeParameters("xc", xc, "yc", yc, "r", R, "Color", color);
		Drawing Circle("Circle", "Polar", params);
		operations.push_back(Circle);
	}
	int x = R, y = 0;
	double theta = 0, dtheta = 1.0 / R;
	draw8points(hdc, xc, yc, x, y, color);
	while (x > y)
	{
		theta += dtheta;
		x = (int)round(R * cos(theta));
		y = (int)round(R * sin(theta));
		draw8points(hdc, xc, yc, x, y, color);
	}
}

void drawCircleItrPolar(HDC hdc, int xc, int yc, int R, COLORREF c, bool load = false) {
	
	if (!load)
	{
		std::string params = serializeParameters("xc", xc, "yc", yc, "r", R, "Color", c);
		Drawing Circle("Circle", "Iterative", params);
		operations.push_back(Circle);
	}

	double x = 0, y = R;
	double dtheta = 1.0 / R;
	double cdtheta = cos(dtheta), sdtheta = sin(dtheta);
	draw8points(hdc, xc, yc, 0, R, c);
	while (x < y) {
		double x1 = x * cdtheta - y * sdtheta;
		double y2 = y * cdtheta + x * sdtheta;
		x = x1;
		y = y2;
		draw8points(hdc, xc, yc, (int)round(x), (int)round(y), c);
	}
}

void drawCircleMidpoint(HDC hdc, int xc, int yc, int R, COLORREF c, bool load = false) {

	if (!load)
	{
		std::string params = serializeParameters("xc", xc, "yc", yc, "r", R, "Color", c);
		Drawing Circle("Circle", "Midpoint", params);
		operations.push_back(Circle);
	}

	int x = 0, y = R;
	int d = 1 - R;
	draw8points(hdc, xc, yc, 0, R, c);
	while (x <= y) {
		if (d >= 0) {
			d += 2 * (x - y) + 5;
			y--;
		}
		else {
			d += 2 * x + 3;
		}
		x++;
		draw8points(hdc, xc, yc, x, y, c);
	}
}

void drawCircleModifiedMidpoint(HDC hdc, int xc, int yc, int R, COLORREF c, bool load = false) {

	if (!load)
	{
		std::string params = serializeParameters("xc", xc, "yc", yc, "r", R, "Color", c);
		Drawing Circle("Circle", "Modified", params);
		operations.push_back(Circle);
	}
	int x = 0, y = R;
	int d = 1 - R;
	int c1 = 3, c2 = 5 - 2 * R;
	draw8points(hdc, xc, yc, 0, R, c);
	while (x <= y) {
		if (d >= 0) {
			d += c2;
			c2 += 4;
			y--;
		}
		else {
			d += c1;
			c2 += 2;
		}
		c1 += 2;
		x++;
		draw8points(hdc, xc, yc, x, y, c);
	}
}

vector<POINT> points;

void CircleFasterBresenham(HDC hdc, int xc, int yc, int R, COLORREF color) {
	int x = 0, y = R;
	int d = 1 - R;
	int c1 = 3, c2 = 5 - 2 * R;
	draw8points(hdc, xc, yc, x, y, color);
	while (x < y) {
		if (d < 0) {
			d += c1;
			c2 += 2;
		}
		else {

			d += c2;
			c2 += 4;
			y--;
		}
		c1 += 2;
		x++;
		draw8points(hdc, xc, yc, x, y, color);
		points.push_back({ x, y });
	}
}

/*! ============================ Spline ============================*/

Vector4 GetHermiteCoeff(double x0, double s0, double x1, double s1) {
	static double H[16] = { 2, 1, -2, 1, -3, -2, 3, -1, 0, 1, 0, 0, 1, 0, 0, 0 };
	static Matrix4 basis(H);
	Vector4 v(x0, s0, x1, s1);
	return basis * v;
}

void DrawHermiteCurve(HDC hdc, Vertex& P0, Vertex& T0, Vertex& P1, Vertex& T1, int numpoints) {
	Vector4 xcoeff = GetHermiteCoeff(P0.x, T0.x, P1.x, T1.x);
	Vector4 ycoeff = GetHermiteCoeff(P0.y, T0.y, P1.y, T1.y);
	if (numpoints < 2)return;
	double dt = 1.0 / (numpoints - 1);
	for (double t = 0; t <= 1; t += dt) {
		Vector4 vt;
		vt[3] = 1;
		for (int i = 2; i >= 0; i--)vt[i] = vt[i + 1] * t;
		int x = round(DotProduct(xcoeff, vt));
		int y = round(DotProduct(ycoeff, vt));
		if (t == 0)MoveToEx(hdc, x, y, NULL); else LineTo(hdc, x, y);
	}
}

void DrawCardinalSpline(HDC hdc, vector<Vertex> P, int n, double c, int numpix, bool load = false) {

	if (!load)
	{
		std::string params = serializeParameters("size", n, "vlist", P, "c", c, "numpix", numpix);
		Drawing Spline("Spline", "Cardinal", params);
		operations.push_back(Spline);
	}
	double c1 = 1 - c;
	Vertex T0(c1 * (P[2].x - P[0].x), c1 * (P[2].y - P[0].y));
	for (int i = 2; i < n - 1; i++) {
		Vertex T1(c1 * (P[i + 1].x - P[i - 1].x), c1 * (P[i + 1].y - P[i - 1].y));
		DrawHermiteCurve(hdc, P[i - 1], T0, P[i], T1, numpix);
		T0 = T1;
	}
}

#endif