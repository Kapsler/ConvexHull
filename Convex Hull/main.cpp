#include <vector>
#include <random>
#include <iostream>
#include <algorithm>

#include "Visualization.h"
#include <chrono>
#include "TimerClass.h"
#include <ppl.h>

const int numPoints = 100000;
const int maxCoord = 1000000;
Visualization *vis = nullptr;

bool stepmode = false;
bool renderflag = true;

void QuickHull(std::vector<Point*>& points);
void FindHull(std::vector<Point*>& points, Point P, Point Q);
int getAngle(const Point& P, const Point& Q, const Point& X);
void sortPoints(Point P, Point Q, std::vector<Point*>& points, std::vector<Point*>& S);
float distanceFromLine(Point P, Point Q, Point X);
void DebugOutput(std::vector<Point*>& points);
void GeneratePoints(std::vector<Point*>& points);

bool greaterX(Point *X,Point *Y)
{
	return X->coords.x < Y->coords.x;
}

int main()
{
	TimerClass timer;
	double generationTime, hullTime, freeTime;
	// Generate points
	std::vector<Point*> points(numPoints);

	if (renderflag)
	{
		vis = new Visualization();
	}

	timer.StartTimer();
	GeneratePoints(points);
	generationTime = timer.GetTime();

	timer.StartTimer();
	QuickHull(points);
	hullTime = timer.GetTime();

	if (vis != nullptr)
	{
		vis->Render();
		vis->Wait(true);
	}

	//DebugOutput(points);

	//Freeing stuff

	timer.StartTimer();

	#pragma omp parallel
	{
		#pragma omp for
		for(auto i = 0; i < points.size(); ++i)
		{
			delete points[i];
		}
	}

	delete vis;
	freeTime = timer.GetTime();

	std::cout << "Generating " << numPoints << " took " << generationTime << " seconds." << std::endl;;
	std::cout << "Finding Hull took " << hullTime << " seconds." << std::endl;
	std::cout << "Freeing stuff took " << freeTime << " seconds." << std::endl;

	return 0;
}

void QuickHull(std::vector<Point*>& points)
{
	// sort all points by X
	//std::sort(points.begin(), points.end(), greaterX);
	//trying parallel - works
	concurrency::parallel_buffered_sort(points.begin(), points.end(), greaterX);

	//Generate Visuals
	if (vis != nullptr)
	{
		vis->SetPoints(points);
		vis->Wait(stepmode);
	}

	Point* P = points[0]; // smallest X
	Point* Q = points[points.size()-1]; // greatest X
	P->hull = true;
	Q->hull = true;
	if (vis != nullptr)
	{
		vis->UpdatePoint(P);
		vis->UpdatePoint(Q);
		vis->AddLine(P, Q);
		vis->Wait(stepmode);
	}

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
		distance = distanceFromLine(P, Q, *obj);
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

	if (vis != nullptr)
	{
		vis->UpdatePoint(farthestPoint);
		vis->AddLine(&P, farthestPoint);
		vis->AddLine(farthestPoint, &Q);
		vis->Wait(stepmode);
		vis->DeleteLine(&P, &Q);
		vis->Wait(stepmode);
	}

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

int getAngle(const Point& P, const Point& Q, const Point& X)
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
		float angle = static_cast<float>(getAngle(P, Q, *obj));
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

void DebugOutput(std::vector<Point*>& points)
{
	for (int i = 0; i < numPoints; ++i)
	{
	std::cout << "Point " << (i + 1) << " at ";
	points[i]->print();
	std::cout << " is " << (points[i]->hull ? "" : "not ") << "part of the hull." << std::endl;
	}
}

void GeneratePoints(std::vector<Point*>& points)
{
	#pragma omp parallel
	{	

		std::mt19937 eng(std::chrono::high_resolution_clock::now().time_since_epoch().count());
		std::uniform_int_distribution<> distr(0, maxCoord);

		#pragma omp for
		for (int i = 0; i < numPoints; ++i)
		{
			points[i] = new Point(distr(eng) / 1100.0f, distr(eng) / 1100.0f);
		}
	}

}
