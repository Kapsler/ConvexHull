#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "glm/glm.hpp"

struct Point
{
	Point() {}
	Point(float px, float py)
		:coords(px, py) {}
	glm::vec2 coords;
	bool hull = false;

	void print() const
	{
		std::cout << "(" << coords.x << " / " << coords.y << ")";
	}
};

struct Line
{
	Line() {}
	Line(glm::vec2 p1, glm::vec2 p2) : start(p1), end(p2) {}
	
	glm::vec2 start, end;
};

class Visualization
{
public:
	Visualization();
	~Visualization();

	void Render();
	void Wait();

	void SetPoints(std::vector<Point*>* other);

	void AddLine(Point* A, Point* B);
	void AddLinePoint(const Point* p1, Point* newpoint, const Point* p2);

private:
	sf::RenderWindow* window;
	std::vector<Point*>* structPoints;
	std::vector<Point*> linesHull;

};
