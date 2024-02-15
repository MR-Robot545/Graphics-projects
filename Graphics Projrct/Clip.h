#ifndef CLIP_H
#define CLIP_H

#include "structs.h"
#include "serilalizer.h"

// !Draw a rectangle
void DrawRectangle(HDC hdc, int left, int top, int right, int bottom)
{
    // Set the pen color and style
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    SelectObject(hdc, hPen);

    // Draw the outline of the rectangle
    MoveToEx(hdc, left, top, NULL);
    LineTo(hdc, right, top);
    LineTo(hdc, right, bottom);
    LineTo(hdc, left, bottom);
    LineTo(hdc, left, top);

    // Clean up the pen object
    DeleteObject(hPen);
}


//! Draw square
void Square(HDC hdc, int xleft, int yleft, int length, COLORREF c) {
    DrawRectangle(hdc, xleft, yleft, xleft + length, yleft + length);
}

/*! ============================ Point Clipping  ============================*/

//! Rectangle point clipping
void rectanglePointClipping(HDC hdc, int x, int y, int xleft, int ytop, int xright, int ybottom, COLORREF c, bool load = false) {
    
    if (!load)
    {
        std::string params = serializeParameters("x", x, "y", y, "xleft", xleft, "ytop", ytop, "xright", xright, "ybottom",
            ybottom, "Color", c);
        Drawing Clip("Clip", "Rectangle Point", params);
        operations.push_back(Clip);
    }

    if (x >= xleft && x <= xright && y >= ytop && y <= ybottom)
        SetPixel(hdc, x, y, c);

}

//! Square point clipping
void squarePointClipping(HDC hdc, int x, int y, int xleft, int yleft, int length, COLORREF color, bool load = false) {

    if (!load)
    {
        std::string params = serializeParameters("x", x, "y", y, "xleft", xleft, "yleft", yleft, "side", length, "Color", color);
        Drawing Clip("Clip", "Square Point", params);
        operations.push_back(Clip);
    }
    if (x >= xleft && x <= xleft + length && y >= yleft && y <= yleft + length)
        SetPixel(hdc, x, y, color);
}

/*! ============================ Line Clipping  ============================*/

OutCode GetOutCode(double x, double y, int xleft, int ytop, int xright, int ybottom) {
    OutCode out;
    out.All = 0;

    if (x < xleft)
        out.left = 1;
    else if (x > xright)
        out.right = 1;

    if (y < ytop)
        out.top = 1;
    else if (y > ybottom)
        out.bottom = 1;

    return out;
}

void VIntersect(double xs, double ys, double xe, double ye, int x, double* xi, double* yi) {
    *xi = x;
    *yi = ys + (x - xs) * (ye - ys) / (xe - xs);
}

void HIntersect(double xs, double ys, double xe, double ye, int y, double* xi, double* yi) {
    *yi = y;
    *xi = xs + (y - ys) * (xe - xs) / (ye - ys);
}

//! Rectangle cohen-southerland line clipping 
void rectangleLineClipping(HDC hdc, int xs, int ys, int xe, int ye, int xleft, int ytop, int xright, int ybottom, COLORREF c, bool load = false)
{
    if (!load)
    {
        std::string params = serializeParameters("x1", xs, "y1", ys, "x2", xe, "y2", ye, "xleft", xleft, "ytop", ytop,
            "xright", xright, "ybottom", ybottom);
        Drawing Clip("Clip", "Rectangle Line", params);
        operations.push_back(Clip);
    }

    double x1 = xs, y1 = ys, x2 = xe, y2 = ye;
    OutCode out1 = GetOutCode(x1, y1, xleft, ytop, xright, ybottom);
    OutCode out2 = GetOutCode(x2, y2, xleft, ytop, xright, ybottom);
    while ((out1.All || out2.All) && !(out1.All & out2.All)) {
        double xi, yi;
        if (out1.All) {
            if (out1.left)VIntersect(x1, y1, x2, y2, xleft, &xi, &yi);
            else if (out1.top)HIntersect(x1, y1, x2, y2, ytop, &xi, &yi);
            else if (out1.right)VIntersect(x1, y1, x2, y2, xright, &xi, &yi);
            else HIntersect(x1, y1, x2, y2, ybottom, &xi, &yi);
            x1 = xi;
            y1 = yi;
            out1 = GetOutCode(x1, y1, xleft, ytop, xright, ybottom);
        }
        else {
            if (out2.left)VIntersect(x1, y1, x2, y2, xleft, &xi, &yi);
            else if (out2.top)HIntersect(x1, y1, x2, y2, ytop, &xi, &yi);
            else if (out2.right)VIntersect(x1, y1, x2, y2, xright, &xi, &yi);
            else HIntersect(x1, y1, x2, y2, ybottom, &xi, &yi);
            x2 = xi;
            y2 = yi;
            out2 = GetOutCode(x2, y2, xleft, ytop, xright, ybottom);
        }
    }
    if (!out1.All && !out2.All) {
        MoveToEx(hdc, round(x1), round(y1), NULL);
        LineTo(hdc, round(x2), round(y2));
    }
}

//! Square line clipping
void squareLineClipping(HDC hdc, int xs, int ys, int xe, int ye, int xleft, int yleft, int length, COLORREF c, bool load = false) 
{
    if (!load)
    {
        std::string params = serializeParameters("x1", xs, "y1", ys, "x2", xe, "y2", ye, "xleft", xleft, "yleft", yleft, "side", length, "Color", c);
        Drawing Clip("Clip", "Square Line", params);
        operations.push_back(Clip);
    }

    int ytop = yleft;
    int xright = xleft + length;
    int ybottom = yleft + length;
    double x1 = xs, y1 = ys, x2 = xe, y2 = ye;
    OutCode out1 = GetOutCode(x1, y1, xleft, ytop, xright, ybottom);
    OutCode out2 = GetOutCode(x2, y2, xleft, ytop, xright, ybottom);
    while ((out1.All || out2.All) && !(out1.All & out2.All)) {
        double xi, yi;
        if (out1.All) {
            if (out1.left)VIntersect(x1, y1, x2, y2, xleft, &xi, &yi);
            else if (out1.top)HIntersect(x1, y1, x2, y2, ytop, &xi, &yi);
            else if (out1.right)VIntersect(x1, y1, x2, y2, xright, &xi, &yi);
            else HIntersect(x1, y1, x2, y2, ybottom, &xi, &yi);
            x1 = xi;
            y1 = yi;
            out1 = GetOutCode(x1, y1, xleft, ytop, xright, ybottom);
        }
        else {
            if (out2.left)VIntersect(x1, y1, x2, y2, xleft, &xi, &yi);
            else if (out2.top)HIntersect(x1, y1, x2, y2, ytop, &xi, &yi);
            else if (out2.right)VIntersect(x1, y1, x2, y2, xright, &xi, &yi);
            else HIntersect(x1, y1, x2, y2, ybottom, &xi, &yi);
            x2 = xi;
            y2 = yi;
            out2 = GetOutCode(x2, y2, xleft, ytop, xright, ybottom);
        }
    }
    if (!out1.All && !out2.All) {
        MoveToEx(hdc, round(x1), round(y1), NULL);
        LineTo(hdc, round(x2), round(y2));
    }
}

/*! ============================ Polygon Clipping  ============================*/

VertexList ClipWithEdge(VertexList p, int edge, IsInFunc In, IntersectFunc Intersect) {
    VertexList OutList;
    Vertex v1 = p[p.size() - 1];
    bool v1_in = In(v1, edge);
    for (int i = 0; i < (int) p.size(); i++) {
        Vertex v2 = p[i];
        bool v2_in = In(v2, edge);
        if (!v1_in && v2_in) {

            OutList.push_back(Intersect(v1, v2, edge));
            OutList.push_back(v2);
        } else if (v1_in && v2_in) OutList.push_back(v2);
        else if (v1_in) OutList.push_back(Intersect(v1, v2, edge));
        v1 = v2;
        v1_in = v2_in;
    }
    return OutList;
}

bool InLeft(Vertex &v, int edge) {
    return v.x >= edge;
}

bool InRight(Vertex &v, int edge) {
    return v.x <= edge;
}

bool InTop(Vertex &v, int edge) {
    return v.y >= edge;
}

bool InBottom(Vertex &v, int edge) {
    return v.y <= edge;
}

Vertex VIntersect(Vertex &v1, Vertex &v2, int xedge) {
    Vertex res;
    res.x = xedge;
    res.y = v1.y + (xedge - v1.x) * (v2.y - v1.y) / (v2.x - v1.x);
    return res;
}

Vertex HIntersect(Vertex &v1, Vertex &v2, int yedge) {
    Vertex res;
    res.y = yedge;
    res.x = v1.x + (yedge - v1.y) * (v2.x - v1.x) / (v2.y - v1.y);
    return res;
}

//! Rectangle Sutherland-Hodgman polygon clipping
void rectanglePolygonClip(HDC hdc, VertexList vlist, int n, int xleft, int ytop, int xright, int ybottom, bool load = false) 
{
    if (!load)
    {
        std::string params = serializeParameters("size", n, "vList", vlist, "xleft", xleft, "ytop", ytop, "xright", xright,
            "ybottom", ybottom);
        Drawing Clip("Clip", "Rectangle Polygon", params);
        operations.push_back(Clip);
    }

    vlist = ClipWithEdge(vlist, xleft, InLeft, VIntersect);
    vlist = ClipWithEdge(vlist, ytop, InTop, HIntersect);
    vlist = ClipWithEdge(vlist, xright, InRight, VIntersect);
    vlist = ClipWithEdge(vlist, ybottom, InBottom, HIntersect);
    Vertex v1 = vlist[vlist.size() - 1];
    for (int i = 0; i < (int)vlist.size(); i++) {
        Vertex v2 = vlist[i];
        MoveToEx(hdc, std::round(v1.x), std::round(v1.y), NULL);
        LineTo(hdc, std::round(v2.x), std::round(v2.y));
        v1 = v2;
    }
}

#endif