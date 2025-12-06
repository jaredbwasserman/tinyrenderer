//
// Created by Jared Wasserman on 11/22/25.
//

#include <fstream>
#include <iostream>
#include <ranges>
#include <string>

#include "bresenham.h"
#include "geometry.h"
#include "tgaimage.h"

static constexpr TGAColor white = {255, 255, 255, 255}; // attention, BGRA order
static constexpr TGAColor green = {0, 255, 0, 255};
static constexpr TGAColor red = {0, 0, 255, 255};
static constexpr TGAColor blue = {255, 128, 64, 255};
static constexpr TGAColor yellow = {0, 200, 255, 255};

static constexpr std::string LINE_FACE = "f";
static constexpr std::string LINE_VERTEX = "v";
static constexpr char DELIMITER_SPACE = ' ';
static constexpr char DELIMITER_SLASH = '/';

static void Line(int ax, int ay, int bx, int by, TGAImage& framebuffer, TGAColor color)
{
	bool steep = std::abs(ax - bx) < std::abs(ay - by);
	if (steep)
	{
		// if the line is steep, we transpose the image
		std::swap(ax, ay);
		std::swap(bx, by);
	}
	if (ax > bx)
	{
		// make it left−to−right
		std::swap(ax, bx);
		std::swap(ay, by);
	}
	int y = ay;
	int ierror = 0;
	for (int x = ax; x <= bx; x++)
	{
		if (steep) // if transposed, de−transpose
			framebuffer.set(y, x, color);
		else
			framebuffer.set(x, y, color);
		ierror += 2 * std::abs(by - ay);
		y += (by > ay ? 1 : -1) * (ierror > bx - ax);
		ierror -= 2 * (bx - ax) * (ierror > bx - ax);
	}
}

static int GetVertexIndex(const std::string& face)
{
	const std::string& vertexNumber = face.substr(0, face.find_first_of(DELIMITER_SLASH));
	return std::stoi(vertexNumber) - 1;
}

static void ProcessFace(const std::string& line, const std::vector<vec3>& vertices, TGAImage& image)
{
	int vertexIndex1 = -1, vertexIndex2 = -1, vertexIndex3 = -1;

	uint pos = 0;
	for (const auto& subrange : std::views::split(line, DELIMITER_SPACE))
	{
		const auto& substring = std::string(subrange.begin(), subrange.end());

		switch (pos)
		{
		case 1:
			{
				vertexIndex1 = GetVertexIndex(substring);
				break;
			}
		case 2:
			{
				vertexIndex2 = GetVertexIndex(substring);
				break;
			}
		case 3:
			{
				vertexIndex3 = GetVertexIndex(substring);
				break;
			}
		default:
			{
				break;
			}
		}

		++pos;
	}

	assert(vertexIndex1 >= 0);
	assert(vertexIndex2 >= 0);
	assert(vertexIndex3 >= 0);

	const vec3& vertex1 = vertices.at(vertexIndex1);
	const vec3& vertex2 = vertices.at(vertexIndex2);
	const vec3& vertex3 = vertices.at(vertexIndex3);

	Line(vertex1.x, vertex1.y, vertex2.x, vertex2.y, image, red);
	Line(vertex2.x, vertex2.y, vertex3.x, vertex3.y, image, red);
	Line(vertex3.x, vertex3.y, vertex1.x, vertex1.y, image, red);
}

static double GetTransformedVertex(const auto& rawVertex, const int size)
{
	return (std::stod(rawVertex) + 1) * size / 2;
}

static void ProcessVertex(const std::string& line, const int width, const int height, std::vector<vec3>& vertices)
{
	auto& [x, y, z] = vertices.emplace_back();
	z = 0;

	uint pos = 0;
	for (const auto& subrange : std::views::split(line, DELIMITER_SPACE))
	{
		const auto& substring = std::string(subrange.begin(), subrange.end());

		switch (pos)
		{
		case 1:
			{
				x = GetTransformedVertex(substring, width);
				break;
			}
		case 2:
			{
				y = GetTransformedVertex(substring, height);
				break;
			}
		default:
			{
				break;
			}
		}

		++pos;
	}
}

int Bresenham(const std::vector<std::string>&)
{
	constexpr int width = 64;
	constexpr int height = 64;
	TGAImage framebuffer(width, height, TGAImage::RGB);

	int ax = 7, ay = 3;
	int bx = 12, by = 37;
	int cx = 62, cy = 53;

	Line(ax, ay, bx, by, framebuffer, blue);
	Line(cx, cy, bx, by, framebuffer, green);
	Line(cx, cy, ax, ay, framebuffer, yellow);
	Line(ax, ay, cx, cy, framebuffer, red);

	framebuffer.set(ax, ay, white);
	framebuffer.set(bx, by, white);
	framebuffer.set(cx, cy, white);

	if (!framebuffer.write_tga_file("framebuffer.tga"))
	{
		std::cerr << "Error writing output file." << std::endl;
		return 1;
	}

	return 0;
}

int BresenhamHW(const std::vector<std::string>& arguments)
{
	if (arguments.empty())
	{
		std::cerr << "Must provide an input file path." << std::endl;
		return 1;
	}
	const std::string& filePath = arguments.at(0);

	std::ifstream inputFile(filePath);
	if (!inputFile.is_open())
	{
		std::cerr << "Error opening file \"" << filePath << "\"." << std::endl;
		return 1;
	}

	std::vector<vec3> vertices;

	static constexpr int width = 800;
	static constexpr int height = 800;
	TGAImage image(width, height, TGAImage::RGB);

	std::string line;
	while (std::getline(inputFile, line))
	{
		const std::string& lineType = line.substr(0, line.find_first_of(DELIMITER_SPACE));
		if (lineType == LINE_FACE)
		{
			ProcessFace(line, vertices, image);
		}
		else if (lineType == LINE_VERTEX)
		{
			ProcessVertex(line, width, height, vertices);
		}
	}

	inputFile.close();

	// Color the vertices white
	for (const auto& vertex : vertices)
	{
		image.set(vertex.x, vertex.y, white);
	}

	if (!image.write_tga_file("bresenhamhw.tga"))
	{
		std::cerr << "Error writing output file." << std::endl;
		return 1;
	}

	return 0;
}
