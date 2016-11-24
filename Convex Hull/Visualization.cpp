#include "Visualization.h"

Visualization::Visualization()
{
	window = new sf::RenderWindow(sf::VideoMode(1000, 1000), "Convex Hull");
}

Visualization::~Visualization()
{
	for(auto* p : points)
	{
		delete p;
	}

	delete window;
}

void Visualization::Render()
{
	window->clear(sf::Color::White);

	float offsetx = window->getSize().x * 0.06f;
	float offsety = window->getSize().y * 0.04f;


	sf::Vertex line[2];
	for(const auto* l : lines)
	{
		line[0].position.x = l->start.x + offsetx;
		line[0].position.y = l->start.y + offsety;
		line[0].color = sf::Color::Red;

		line[1].position.x = l->end.x + offsetx;
		line[1].position.y = l->end.y + offsety;
		line[1].color = sf::Color::Red;

		window->draw(line, 2, sf::Lines);
	}

	for(const auto* p : points)
	{
		sf::CircleShape drawp(*p);
		drawp.setPosition(drawp.getPosition().x + offsetx, drawp.getPosition().y + offsety);
		window->draw(drawp);
	}

	window->display();
}

void Visualization::Wait(bool wait)
{
	sf::Event event;
	bool waiting = wait;

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
	
		Render();
	
	}

}


void Visualization::SetPoints(std::vector<Point*>& other)
{
	for(auto i = 0; i < points.size(); i++)
	{
		delete points[i];
	}

	points.clear();

	for(auto* o : other)
	{
		points.push_back(ShapeForPoint(o));
	}
}

void Visualization::UpdatePoint(Point* newPoint)
{
	int index = -1;

	for(int i = 0; i < points.size(); ++i)
	{
		if(points[i]->getPosition().x == newPoint->coords.x && points[i]->getPosition().y == newPoint->coords.y)
		{
			index = i;
			break;
		}
	}

	if(index == -1)
	{
		std::cerr << "SHOULD NOT HAPPEN" << std::endl;
		exit(0);
	}

	sf::CircleShape* oldPoint = points[index];

	sf::CircleShape* p = ShapeForPoint(newPoint);
	points[index] = p;

	delete oldPoint;
}

sf::CircleShape* Visualization::ShapeForPoint(Point* newPoint) const
{
	sf::CircleShape* p;
	p = new sf::CircleShape(2.0f);
	p->setPosition(newPoint->coords.x, newPoint->coords.y);
	if (newPoint->hull)
	{
		p->setFillColor(sf::Color::Red);
	}
	else
	{
		p->setFillColor(sf::Color::Black);
	}
	p->setOrigin(p->getRadius(), p->getRadius());

	return p;
}

void Visualization::DeleteLine(Point* A, Point* B)
{
	auto i = lines.begin();
	for(; i != lines.end();)
	{
		if ((A->coords == (*i)->start && B->coords == (*i)->end) || (B->coords == (*i)->start && A->coords == (*i)->end))
		{
			i = lines.erase(i);
		} else
		{
			++i;
		}
	}
}

void Visualization::AddLine(Point* A, Point* B)
{
	Line* l = new Line();
	l->start = A->coords;
	l->end = B->coords;
	lines.push_back(l);
}
