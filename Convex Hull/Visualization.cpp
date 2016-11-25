#include "Visualization.h"

Visualization::Visualization()
{
	window = new sf::RenderWindow(sf::VideoMode(1000, 1000), "Convex Hull");
}

Visualization::~Visualization()
{
	delete window;
}

void Visualization::Render()
{
	window->clear(sf::Color::White);
	
	float offsetx = window->getSize().x * 0.06f;
	float offsety = window->getSize().y * 0.04f;

	sf::CircleShape drawp;
	drawp.setRadius(2);
	drawp.setOrigin(drawp.getRadius(), drawp.getRadius());
	for (auto i = 0; i < structPoints->size(); ++i)
	{
		if (!(*structPoints)[i]->hull)
		{
			drawp.setFillColor(sf::Color::Black);
			drawp.setPosition((*structPoints)[i]->coords.x + offsetx, (*structPoints)[i]->coords.y + offsety);
			window->draw(drawp);
		}
		
	}
	for (auto i = 0; i < structPoints->size(); ++i)
	{
		if ((*structPoints)[i]->hull)
		{
			drawp.setRadius(5);
			drawp.setOrigin(drawp.getRadius(), drawp.getRadius());
			drawp.setFillColor(sf::Color::Magenta);
			drawp.setPosition((*structPoints)[i]->coords.x + offsetx, (*structPoints)[i]->coords.y + offsety);
			window->draw(drawp);
		}
	}


	sf::Vertex* drawHull = new sf::Vertex[linesHull.size()];

	for (auto i = 0; i < linesHull.size(); i++)
	{
		drawHull[i].position.x = linesHull[i]->coords.x + offsetx;
		drawHull[i].position.y = linesHull[i]->coords.y + offsety;
		drawHull[i].color = sf::Color::Red;
	}
	
	window->draw(drawHull, linesHull.size(), sf::LineStrip);

	window->display();
}

void Visualization::Wait()
{
	sf::Event event;
	bool waiting = true;

	Render();

	while(window->isOpen() && waiting)
	{
		while (window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window->close();
			}
			else if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Escape)
				{
					window->close();
				}
				if (event.key.code == sf::Keyboard::Space)
				{
					waiting = false;
				}
			}
		}
	}

}


void Visualization::SetPoints(std::vector<Point*>* other)
{
	structPoints = other;

}

void Visualization::AddLine(Point* A, Point* B)
{
	linesHull.push_back(B);
	linesHull.push_back(A);
	linesHull.push_back(A);
	linesHull.push_back(B);

}

void Visualization::AddLinePoint(const Point* p1, Point* newpoint, const Point* p2)
{
	auto it = linesHull.begin()+1;

	for(;it != linesHull.end();)
	{
		if ((*it)->coords == p2->coords && (*(it - 1))->coords == p1->coords)
		{
			linesHull.insert(it, newpoint);
			break;
		
		} else
		{
			++it;
		}
	}

}
