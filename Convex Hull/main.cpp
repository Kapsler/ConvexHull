#include <vector>
#include <random>
#include <iostream>
#include <algorithm>

#include "glm/glm.hpp"

const int numPoints = 1000;
const int maxCoord = 10000;

struct Point
{
	Point(){}
	Point(int px, int py)
		:coords(px, py){}
	glm::vec2 coords;
	bool hull = false;

	void print() const
	{
		std::cout << "(" << coords.x << " / " << coords.y << ")";
	}
};

void QuickHull(std::vector<Point*>& points);
void FindHull(std::vector<Point*>& points, Point P, Point Q);
int getAngle(Point P, Point Q, Point X);
void sortPoints(Point P, Point Q, std::vector<Point*>& points, std::vector<Point*>& S);
float distanceFromLine(Point P, Point Q, Point X);

int main()
{
	// Generate points
	std::vector<Point*> points(numPoints);

	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_int_distribution<> distr(0, maxCoord);
	
	for (int i = 0; i < numPoints; ++i)
	{
		points[i] = new Point(distr(eng), distr(eng));
		//points[i]->print();
		//std::cout << std::endl;
	}

	QuickHull(points);

	for (int i = 0; i < numPoints; ++i)
	{
		std::cout << "Point " << (i + 1) << " at ";
		points[i]->print();
		std::cout << " is " << (points[i]->hull ? "" : "not ") << "part of the hull." << std::endl;
	}

	for(auto obj : points)
	{
		delete obj;
	}

	return 0;
}

bool greaterX(Point *X, Point *Y)
{
	return X->coords.x < Y->coords.x;
}

void QuickHull(std::vector<Point*>& points)
{
	// sort all points by X
	//std::nth_element(points.begin(), points.begin(), points.end(), greaterX);
	std::sort(points.begin(), points.end(), greaterX);
	Point* P = points[0]; // smallest X
	Point* Q = points[points.size()-1]; // greatest X
	P->hull = true;
	Q->hull = true;

	std::vector<Point*> S1, S2; // Subgroups of points left and right from separating vector PQ
	sortPoints(*P, *Q, points, S1);
	sortPoints(*Q, *P, points, S2);

	if(!S1.empty())
	{
		FindHull(S1, *P, *Q);
	}
	if(!S2.empty())
	{
		FindHull(S2, *Q, *P);
	}
}

void FindHull(std::vector<Point*>& points, Point P, Point Q)
{
	// Find point with greatest distance from the line PQ
	float maxDistance = -std::numeric_limits<float>().infinity();
	float distance = maxDistance;
	int counter = 0;
	int farthestPointIdx = -1;
	for(auto obj : points)
	{
		if (obj->coords == P.coords || obj->coords == Q.coords) { continue; }
		distance = distanceFromLine(P, Q, *obj);
		//std::cout << "distance: " << distance << std::endl;
		if(distance > maxDistance)
		{
			maxDistance = distance;
			farthestPointIdx = counter;
		}
		++counter;
	}

	if(farthestPointIdx == -1)
	{
		std::cout << "farthestpoint not found :(" << std::endl;
		exit(0);
	}

	Point* farthestPoint = points[farthestPointIdx];
	farthestPoint->hull = true;

	//std::cout << "Farthest point was at ";
	//farthestPoint->print();
	//std::cout << " with a distance of " << maxDistance << std::endl;

	// Subgroups of points outside of triangle
	std::vector<Point*> S1, S2; 
	sortPoints(P, *farthestPoint, points, S1);
	sortPoints(*farthestPoint, Q, points, S2);

	if (!S1.empty())
	{
		FindHull(S1, P, *farthestPoint);
	}
	if (!S2.empty())
	{
		FindHull(S2, *farthestPoint, Q);
	}
}

int getAngle(Point P, Point Q, Point X)
{
	glm::vec2 PQ = Q.coords - P.coords;
	glm::vec2 PX = X.coords - P.coords;
	if(PQ.x * PX.y - PQ.y*PX.x < 0)
	{
		return -1;
	}
	return 1;
}

void sortPoints(Point P, Point Q, std::vector<Point*>& points, std::vector<Point*>& S)
{
	for (Point* obj : points)
	{
		//ignore P and Q
		if (obj->coords == P.coords || obj->coords == Q.coords) { continue; }
		float angle = getAngle(P, Q, *obj);
		if (angle > 0)
		{
			S.push_back(obj);
		}
	}
}

float distanceFromLine(Point P, Point Q, Point X)
{
	return std::abs((Q.coords.y - P.coords.y)*X.coords.x - (Q.coords.x - P.coords.x)*X.coords.y + Q.coords.x*P.coords.y - Q.coords.y*P.coords.x) / std::sqrt(std::pow((Q.coords.y - P.coords.y), 2) + std::pow((Q.coords.x - P.coords.x), 2));
}
