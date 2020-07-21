#pragma once

#include <vector>

struct Vertex
{
	Vertex() {}
	Vertex(float x, float y, float z, float nx, float ny, float nz, float tx, float ty, float tz, float u, float v) :
		x(x), y(y), z(z),
		nx(nx), ny(ny), nz(nz),
		tx(tx), ty(ty), tz(tz),
		u(u), v(v) {}

	float x;
	float y;
	float z;

	float u;
	float v;

	float nx;
	float ny;
	float nz;

	float tx = 0;
	float ty = 0;
	float tz = 0;

	float bx = 0;
	float by = 0;
	float bz = 0;
};

struct MeshData
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};