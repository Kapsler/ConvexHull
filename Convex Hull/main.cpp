#include <vector>
#include <random>
#include <iostream>
#include <algorithm>

#include "Visualization.h"
#include <chrono>
#include "TimerClass.h"
#include "CommandLineParser.h"
#include "ReaderWriter.h" 
#include <omp.h>

int numPoints = 1000000;
int maxCoord = 1000000;
Visualization* vis = nullptr;
string inputFilename = "";

bool stepmode = false;
bool renderflag = false;

void QuickHull(std::vector<Point*>& points);
void FindHull(std::vector<Point*>& points, Point P, Point Q);
int getAngle(const Point& P, const Point& Q, const Point& X);
void sortPoints(Point P, Point Q, std::vector<Point*>& points, std::vector<Point*>& S);
float distanceFromLine(Point P, Point Q, Point X);
void DebugOutput(std::vector<Point*>& points);
void GeneratePoints(std::vector<Point*>& points);
bool ParseParameters(int& argc, char** argv);

bool greaterX(Point* X, Point* Y)
{
	return X->coords.x < Y->coords.x;
}

int main(int argc, char** argv)
{
	TimerClass timer;
	double generationTime, hullTime, freeTime, singleRender = 0;
	std::vector<Point*> points;

	if (!ParseParameters(argc, argv))
	{
		return -1;
	}


	if (inputFilename.empty())
	{
		// Generate points
		if (numPoints > 0)
		{
			timer.StartTimer();
			GeneratePoints(points);
			generationTime = timer.GetTime();
			std::cout << "Generating " << numPoints << " took " << generationTime << " seconds." << std::endl;;
		}
		else
		{
			cerr << "Not enough points!" << endl;
			return -1;
		}
	}
	else
	{
		//Read from file
		std::vector<float> numbers;
		ReaderWriter(inputFilename, numbers);

		points.resize(numbers.size() / 2.0f);
		for (auto i = 0; i < points.size(); ++i)
		{
			points[i] = new Point(numbers[i], numbers[i + 1]);
		}
	}


	if (vis != nullptr)
	{
		vis->SetPoints(&points);
		if(stepmode) vis->Wait();
	}

	//Make Hull
	timer.StartTimer();
	QuickHull(points);
	hullTime = timer.GetTime();
	std::cout << "Finding Hull took " << hullTime << " seconds." << std::endl;

	if (vis != nullptr)
	{
		vis->Render();
		vis->Wait();
	}

	//Freeing stuff
	timer.StartTimer();
#pragma omp parallel
	{
#pragma omp for
		for (auto i = 0; i < points.size(); ++i)
		{
			delete points[i];
		}
	}
	delete vis;
	freeTime = timer.GetTime();
	std::cout << "Freeing stuff took " << freeTime << " seconds." << std::endl;

	return 0;
}

void QuickHull(std::vector<Point*>& points)
{
	std::nth_element(points.begin(), points.begin(), points.end(), greaterX);
	std::nth_element(points.begin(), points.begin() + points.size() - 1, points.end(), greaterX);

	Point* P = points[0]; // smallest X
	Point* Q = points[points.size() - 1]; // greatest X
	P->hull = true;
	Q->hull = true;
	if (vis != nullptr)
	{
		vis->AddLine(P, Q);
		if(stepmode)vis->Wait();
	}

	#pragma omp parallel num_threads(8)
	{
		#pragma omp sections
		{
			#pragma omp section
			{
				std::vector<Point*> S1; // Subgroups of points left and right from separating vector PQ
				sortPoints(*P, *Q, points, S1);
				if (!S1.empty())
				{
					FindHull(S1, *P, *Q);
				}
			}

			#pragma omp section
			{
				std::vector<Point*> S2; // Subgroups of points left and right from separating vector PQ
				sortPoints(*Q, *P, points, S2);
				if (!S2.empty())
				{
					FindHull(S2, *Q, *P);
				}
			}
		}
	}
}

void FindHull(std::vector<Point*>& points, Point P, Point Q)
{
	// Find point with greatest distance from the line PQ
	float maxDistance = -std::numeric_limits<float>().infinity();
	int counter = 0;
	int farthestPointIdx = -1;

	#pragma omp parallel shared(maxDistance)
	{
		float distance = maxDistance;
		#pragma omp for
		for (auto i = 0; i < points.size(); ++i)
		{
			distance = distanceFromLine(P, Q, *points[i]);
			if (distance > maxDistance)
			{
				maxDistance = distance;
				farthestPointIdx = counter;
			}
			++counter;
		}
	}
	

	if (farthestPointIdx == -1)
	{
		std::cout << "farthestpoint not found :(" << std::endl;
		exit(0);
	}

	Point* farthestPoint = points[farthestPointIdx];
	farthestPoint->hull = true;

	if (vis != nullptr)
	{
		vis->AddLine(&P, farthestPoint);
		vis->AddLine(farthestPoint, &Q);
		if(stepmode) vis->Wait();
		vis->DeleteLine(&P, &Q);
		if(stepmode) vis->Wait();
	}

	#pragma omp parallel num_threads(8)
	{
		#pragma omp sections
		{
			#pragma omp section
			{

				std::vector<Point*> S1;
				sortPoints(P, *farthestPoint, points, S1);
				if (!S1.empty())
				{
					FindHull(S1, P, *farthestPoint);
				}
			}

			#pragma omp section
			{
				std::vector<Point*> S2;
				sortPoints(*farthestPoint, Q, points, S2);

				if (!S2.empty())
				{
					FindHull(S2, *farthestPoint, Q);
				}
			}
		}
	}	
}

int getAngle(const Point& P, const Point& Q, const Point& X)
{
	glm::vec2 PQ = Q.coords - P.coords;
	glm::vec2 PX = X.coords - P.coords;
	if (PQ.x * PX.y - PQ.y * PX.x < 0)
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
	return std::abs((Q.coords.y - P.coords.y) * X.coords.x - (Q.coords.x - P.coords.x) * X.coords.y + Q.coords.x * P.coords.y - Q.coords.y * P.coords.x) / std::sqrt(std::pow((Q.coords.y - P.coords.y), 2) + std::pow((Q.coords.x - P.coords.x), 2));
}

void DebugOutput(std::vector<Point*>& points)
{
	vis = new Visualization();
	vis->SetPoints(&points);
	vis->Wait();
}

void GeneratePoints(std::vector<Point*>& points)
{
	points.resize(numPoints);

#pragma omp parallel
	{
		std::mt19937 eng(std::chrono::high_resolution_clock::now().time_since_epoch().count());
		std::uniform_int_distribution<> distr(0, maxCoord);

#pragma omp for
		for (auto i = 0; i < numPoints; ++i)
		{
			points[i] = new Point(distr(eng) / 1100.0f, distr(eng) / 1100.0f);
		}
	}
}

bool ParseParameters(int& argc, char** argv)
{
	CommandLineParser cmdline(argc, argv);

	if (cmdline.cmdOptionExists("--in"))
	{
		inputFilename = cmdline.getCmdOption("--in");
	}
	else
	{
		cerr << "No inputfile, generating numbers!" << endl;

		//If no inputfile --count is needed
		if (cmdline.cmdOptionExists("--count"))
		{
			numPoints = stoi(cmdline.getCmdOption("--count"));
		}
		else
		{
			cerr << "--count not found, standard Numbercount is 1000000" << endl;
			numPoints = 1000000;
		}
	}

	if (cmdline.cmdOptionExists("--step"))
	{
		stepmode = true;
	}
	else
	{
		stepmode = false;
	}

	if (cmdline.cmdOptionExists("--render"))
	{
		vis = new Visualization();
	}

	return true;
}
