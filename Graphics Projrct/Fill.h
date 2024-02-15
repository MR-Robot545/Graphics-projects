#ifndef FILL_H
#define FILL_H

#include "structs.h"
#include "serilalizer.h"
#include "Draw.h"
#include <queue>

/*! ============================ Circle Quarter Fill ============================*/

void fillCircleQuarter(HDC hdc, int xc, int yc, int radius, int quarter, COLORREF c, bool load = false) {

    if (!load)
    {
        std::string params = serializeParameters("xc", xc, "yc", yc, "r", radius, "quart", quarter, "Color", c);
        Drawing Fill("Fill", "Lines", params);
        operations.push_back(Fill);
    }

    CircleFasterBresenham(hdc, xc, yc, radius, c);
    switch (quarter) {
    case 1: {
        for (const POINT& p : points) {
            drawLineDDA(hdc, xc, yc, xc + p.x, yc - p.y, c, true);
            drawLineDDA(hdc, xc, yc, xc + p.y, yc - p.x, c, true);
        }
        break;
    }
    case 2: {
        for (const POINT& p : points) {
            drawLineDDA(hdc, xc, yc, xc - p.x, yc - p.y, c, true);
            drawLineDDA(hdc, xc, yc, xc - p.y, yc - p.x, c, true);
        }
        break;
    }
    case 3: {
        for (const POINT& p : points) {
            drawLineDDA(hdc, xc, yc, xc - p.x, yc + p.y, c, true);
            drawLineDDA(hdc, xc, yc, xc - p.y, yc + p.x, c, true);
        }
        break;
    }
    case 4: {
        for (const POINT& p : points) {
            drawLineDDA(hdc, xc, yc, xc + p.x, yc + p.y, c, true);
            drawLineDDA(hdc, xc, yc, xc + p.y, yc + p.x, c, true);
        }
        break;
    }
    default:
        cout << "Invalid quarter specified!" << endl;
        break;
    }
    points.clear();
}

void FilledCircleQuarterWithCircles(HDC hdc, int xc, int yc, int radius, int quarter, COLORREF c, bool load = false) {
   
    if (!load)
    {
        std::string params = serializeParameters("xc", xc, "yc", yc, "r", radius, "quart", quarter, "Color", c);
        Drawing Fill("Fill", "Circles", params);
        operations.push_back(Fill);
    }
    // Calculate the squared radius
    int radiusSquared = radius * radius;
    CircleFasterBresenham(hdc, xc, yc, radius, c);

    // Vector to store the center points of the smaller circles
    std::vector<POINT> centerPoints;

    int startX, startY, endX, endY;
    switch (quarter) {
    case 1:  // Upper-right quarter
        startX = 0;
        startY = -radius;
        endX = radius;
        endY = 0;
        break;
    case 2:  // Upper-left quarter
        startX = -radius;
        startY = -radius;
        endX = 0;
        endY = 0;
        break;
    case 3:  // Lower-left quarter
        startX = -radius;
        startY = 0;
        endX = 0;
        endY = radius;
        break;
    case 4:  // Lower-right quarter
        startX = 0;
        startY = 0;
        endX = radius;
        endY = radius;
        break;
    default:
        std::cout << "Invalid quarter specified!" << std::endl;
        return;
    }

    // Iterate over the bounding box of the filling quarter
    for (int y = startY; y <= endY; y++) {
        for (int x = startX; x <= endX; x++) {
            if (x * x + y * y <= radiusSquared) {
                centerPoints.push_back({ xc + x, yc + y });
            }
        }
    }

    // Draw smaller circles inside the filled region
    int smallerRadius = radius / 30;
    for (const POINT& center : centerPoints) {
        CircleFasterBresenham(hdc, center.x, center.y, smallerRadius, c);
    }
}

/*! ============================ Flood Fill ============================*/
int dx[]={1,-1,0,0,1,-1,-1,1},dy[]={0,0,1,-1,1,-1,1,-1};
void FloodFill(HDC hdc, int x, int y, COLORREF Cb, COLORREF Cf, bool load = false) {
    if (!load)
    {
        std::string params = serializeParameters("x", x, "y", y, "Cb", Cb, "Cf", Cf);
        Drawing Fill("Fill", "Flood", params);
        operations.push_back(Fill);
    }
    for (int i = 0; i <4 ; ++i) {
        COLORREF c = GetPixel(hdc, x+dx[i], y+dy[i]);
        if (c == Cf||c==Cb) {
            continue;
        }
        SetPixel(hdc, x+dx[i], y+dy[i], Cf);
        FloodFill(hdc,x+dx[i],y+dy[i],Cb,Cf,load);
    }
}

void NRFloodFill(HDC hdc, int x, int y, COLORREF Cb, COLORREF Cf, bool load = false) {

    if (!load)
    {
        std::string params = serializeParameters("x", x, "y", y, "CB", Cb, "Cf", Cf);
        Drawing Fill("Fill", "NR Flood", params);
        operations.push_back(Fill);
    }
    queue<Vertex> S;
    S.push(Vertex(x, y));

    while (!S.empty()) {
        Vertex v = S.front();
        S.pop();
        for (int i = 0; i <4 ; ++i) {
            COLORREF c = GetPixel(hdc, v.x+dx[i], v.y+dy[i]);
            if (c == Cf||c==Cb)
                continue;
            S.push(Vertex(v.x + dx[i], v.y+dy[i]));
            SetPixel(hdc, v.x+dx[i], v.y+dy[i], Cf);
        }
    }
}

/*! ============================ Convex Fill ============================*/

void InitEntries(Entry table[]) {
    for (int i = 0; i < MAXENTRIES; i++) {
        table[i].xmin = INT_MAX;
        table[i].xmax = INT_MIN;
    }
}

void ScanEdge(Vertex v1, Vertex v2, Entry table[]) {
    if (v1.y == v2.y)
        return;
    if (v1.y > v2.y)
        std::swap(v1, v2);

    double minv = ((double) v2.x - v1.x) / ((double) v2.y - v1.y);
    double x = v1.x;
    int y = v1.y;

    while (y < v2.y) {
        if (x < table[y].xmin)
            table[y].xmin = (int) ceil(x);
        if (x > table[y].xmax)
            table[y].xmax = (int) floor(x);

        y++;
        x += minv;
    }
}

void DrawSanLines(HDC hdc, Entry table[], COLORREF color) {
    for (int y = 0; y < MAXENTRIES; y++)
        if (table[y].xmin < table[y].xmax)
            for (int x = table[y].xmin; x <= table[y].xmax; x++)
                SetPixel(hdc, x, y, color);
}

void convexFill(HDC hdc, std::vector<Vertex> p, int n, COLORREF c, bool load = false) {

    if (!load)
    {
        std::string params = serializeParameters("size", n, "vList", p, "Color", c);
        Drawing Fill("Fill", "Convex", params);
        operations.push_back(Fill);
    }
    Entry* table = new Entry[MAXENTRIES];
    InitEntries(table);
    Vertex v1 = p[n - 1];
    for (int i = 0; i < n; i++) {
        Vertex v2 = p[i];
        ScanEdge(v1, v2, table);
        v1 = p[i];
    }
    DrawSanLines(hdc, table, c);
    delete[] table;
}

/*! ============================ Non Convex Fill ============================*/

EdgeRec InitEdgeRec(Vertex &v1, Vertex &v2) {
    if (v1.y > v2.y)
        std::swap(v1, v2);

    EdgeRec rec;
    rec.x = v1.x;
    rec.ymax = v2.y;
    rec.minv = (double) (v2.x - v1.x) / (v2.y - v1.y);
    return rec;
}

void InitEdgeTable(std::vector<Vertex> &polygon, int n, EdgeList table[]) {
    Vertex v1 = polygon[n - 1];
    for (int i = 0; i < n; i++) {
        Vertex v2 = polygon[i];
        if (v1.y == v2.y) {
            v1 = v2;
            continue;
        }
        EdgeRec rec = InitEdgeRec(v1, v2);
        table[(int) v1.y].push_back(rec);
        v1 = polygon[i];
    }
}

void nonConvexFill(HDC hdc, std::vector<Vertex> &p, int n, COLORREF c, bool load = false) {

    if (!load)
    {
        std::string params = serializeParameters("size", n, "vList", p, "Color", c);
        Drawing Fill("Fill", "Non Convex", params);
        operations.push_back(Fill);
    }
    EdgeList *table = new EdgeList[MAXENTRIES];
    InitEdgeTable(p, n, table);

    int y = 0;
    while (y < MAXENTRIES && table[y].size() == 0)
        y++;

    if (y == MAXENTRIES)
        return;

    EdgeList ActiveList = table[y];
    while (ActiveList.size() > 0) {
        ActiveList.sort();
        for (EdgeList::iterator it = ActiveList.begin(); it != ActiveList.end(); it++) {
            int x1 = (int) ceil(it->x);
            it++;
            int x2 = (int) floor(it->x);
            for (int x = x1; x <= x2; x++)
                SetPixel(hdc, x, y, c);
        }
        y++;

        EdgeList::iterator it = ActiveList.begin();
        while (it != ActiveList.end())
            if (y == it->ymax)
                it = ActiveList.erase(it);
            else it++;

        for (EdgeList::iterator it = ActiveList.begin(); it != ActiveList.end(); it++)
            it->x += it->minv;

        ActiveList.insert(ActiveList.end(), table[y].begin(), table[y].end());
    }
    delete[] table;
}

/*! ============================ Bezier/Hermite fills ============================*/

void DrawBezierRectangle(HDC hdc, RECT rect) {
    POINT points[4] = {
            {rect.left,  rect.top},
            {rect.right, rect.top},
            {rect.right, rect.bottom},
            {rect.left,  rect.bottom}
    };

    Polygon(hdc, points, 4);

    POINT bezierPoints[4] = {
            {rect.left,                                    rect.top + (rect.bottom - rect.top) / 2},
            {rect.right,                                   rect.top + (rect.bottom - rect.top) / 2},
            {rect.left + (rect.right - rect.left) / 4,     rect.top},
            {rect.left + (rect.right - rect.left) * 3 / 4, rect.top}
    };

    PolyBezier(hdc, bezierPoints, 4);
}

void FillBezierRectangle(HDC hdc, RECT rect, COLORREF c, bool load = false) {

    if (!load)
    {
        std::string params = serializeParameters("xleft", rect.left, "ytop", rect.top, "xright", rect.right,
            "ybottom", rect.bottom, "Color", c);
        Drawing Fill("Fill", "Bezier", params);
        operations.push_back(Fill);
    }
    HBRUSH hBrush = CreateSolidBrush(c);

    HBRUSH hOldBrush = (HBRUSH) SelectObject(hdc, hBrush);

    DrawBezierRectangle(hdc, rect);

    PatBlt(hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, PATCOPY);

    SelectObject(hdc, hOldBrush);
    DeleteObject(hBrush);
}

void DrawHermitCurveSquare(HDC hdc, RECT rect, COLORREF c, bool load = false) {

    if (!load)
    {
        std::string params = serializeParameters("xleft", rect.left, "ytop", rect.top, "xright", rect.right,
            "ybottom", rect.bottom, "Color", c);
        Drawing Fill("Fill", "Hermit", params);
        operations.push_back(Fill);
    }
    POINT corners[4] = {rect.left, rect.top, rect.right, rect.top, rect.right, rect.bottom, rect.left, rect.bottom};
    POINT midpoints[4] = {(corners[0].x + corners[1].x) / 2, corners[0].y, corners[1].x,
                          (corners[1].y + corners[2].y) / 2, corners[2].x, corners[2].y,
                          (corners[3].x + corners[0].x) / 2, corners[3].y};

    HBRUSH hBrush = CreateSolidBrush(c);

    HBRUSH hOldBrush = (HBRUSH) SelectObject(hdc, hBrush);

    POINT curvePoints[17];
    curvePoints[0] = corners[0];
    curvePoints[16] = corners[0];
    for (int i = 0; i < 4; i++) {
        curvePoints[i * 4 + 1] = midpoints[i];
        curvePoints[i * 4 + 2] = midpoints[i];
        curvePoints[i * 4 + 3] = corners[(i + 1) % 4];
        curvePoints[i * 4 + 4] = corners[(i + 1) % 4];
    }
    Polygon(hdc, curvePoints, 17);

    SelectObject(hdc, hOldBrush);
    DeleteObject(hBrush);
}

#endif