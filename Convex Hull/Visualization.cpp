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
		if ((*structPoints)[i]->hull)
		{
			drawp.setFillColor(sf::Color::Red);
		}
		else
		{
			drawp.setFillColor(sf::Color::Black);
		}
		drawp.setPosition((*structPoints)[i]->coords.x + offsetx, (*structPoints)[i]->coords.y + offsety);
		window->draw(drawp);
	}


	sf::Vertex line[2];
	for (auto i = 0; i < lines.size(); ++i)
	{
		line[0].position.x = lines[i]->start.x + offsetx;
		line[0].position.y = lines[i]->start.y + offsety;
		line[0].color = sf::Color::Red;

		line[1].position.x = lines[i]->end.x + offsetx;
		line[1].position.y = lines[i]->end.y + offsety;
		line[1].color = sf::Color::Red;

		window->draw(line, 2, sf::Lines);
	}

	

	window->display();
}

void Visualization::Wait(bool wait)
{
	sf::Event event;
	bool waiting = wait;

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
