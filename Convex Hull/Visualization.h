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
	void Wait(bool wait);

	void SetPoints(std::vector<Point*>& other);
	void UpdatePoint(Point* newPoint);
	sf::CircleShape* ShapeForPoint(Point* newPoint) const;

	void DeleteLine(Point* A, Point* B);
	void AddLine(Point* A, Point* B);

private:
	sf::RenderWindow* window;

	std::vector<sf::CircleShape*> points;
	std::vector<Line*> lines;

};
