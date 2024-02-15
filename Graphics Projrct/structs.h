#ifndef STRUCTS_H
#define STRUCTS_H

#include <vector>
#include <Windows.h>
#include <iostream>
#include <list>

/*! ============================ General Structs ============================*/

//! Struct to hold all drawn shapes, clippings, fills, etc...
struct Drawing
{
	std::string type;
	std::string algorithm;
	std::string parameters;
	Drawing(const std::string type, const std::string algorithm, const std::string params)
		: type(type), algorithm(algorithm), parameters(params)
	{}
};

struct Vertex
{
	double x, y;
	Vertex(int x1 = 0, int y1 = 0)
	{
		x = x1;
		y = y1;
	}
};
using VertexList = std::vector<Vertex>;
typedef bool (*IsInFunc)(Vertex& v, int edge);
typedef Vertex(*IntersectFunc)(Vertex& v1, Vertex& v2, int edge);

typedef struct tagPOINTF {
    float x;
    float y;
} POINTF, * PPOINTF;

/*! ============================ Clipping Structs ============================*/

union OutCode
{
	unsigned All : 4;
	struct { unsigned left : 1, top : 1, right : 1, bottom : 1; };
};


/*! ============================ Filling Structs ============================*/

#define MAXENTRIES 600
struct Entry
{
	int xmin, xmax;
};

struct EdgeRec
{
	double x;
	double minv;
	int ymax;
	bool operator<(EdgeRec r)
	{
		return x < r.x;
	}
};
using EdgeList = std::list<EdgeRec>;


/*! ============================ Curves Structs ============================*/

class Vector4 {
    double v[4];
public:
    Vector4(double a = 0, double b = 0, double c = 0, double d = 0) {
        v[0] = a;
        v[1] = b;
        v[2] = c;
        v[3] = d;
    }

    Vector4(double a[]) {
        memcpy(v, a, 4 * sizeof(double));
    }

    double& operator[](int i) {
        return v[i];
    }
};

class Matrix4 {
    Vector4 M[4];
public:
    Matrix4(double A[]) {
        memcpy(M, A, 16 * sizeof(double));
    }

    Vector4& operator[](int i) {
        return M[i];
    }
};

Vector4 operator*(Matrix4 M, Vector4& b) {
    Vector4 res;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            res[i] += M[i][j] * b[j];

    return res;
}

double DotProduct(Vector4& a, Vector4& b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
}

#endif