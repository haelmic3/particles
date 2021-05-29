///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  Copyright (c) 2021        Michael  Julian  Moran    All rights reserved  //
//                                                                           //
//                                                                           //
//               The author is not responsible for any damage.               //
//   This includes but not limited to the injury, damage or destruction of   //
//           any person, piece or pieces of hardware, software, data         //
//                or anything in the known or unknown universe.              //
//                                                                           //
// Having this on you're computer at all, ever is an agreement to the terms. //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
//#include<iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <random>
#include <SFML/Graphics.hpp>

constexpr char TITLE[] = "Hackz for days"; // TITLE

struct Model_delta
{
// TODO
	std::chrono::nanoseconds time;
	
};
template<typename T,unsigned long long Size>
struct Array
{
	T value[Size];

	using type = T;
	static constexpr unsigned long long size = Size;
	static constexpr char index_error[] = "TMP Array Index error";
	
	T&operator*(){return *value;}
	T*operator->(){return value;}
	T operator[](size_t n)const{if(n<size)return value[n];throw index_error;}
	T&operator[](size_t n)     {if(n<size)return value[n];throw index_error;}
	operator T*(){return value;}
};
class Graph : public sf::Drawable
{
	using type = unsigned long long;
	using vtype = float;
 public:
	std::vector<sf::Vertex> value;
	sf::PrimitiveType primitive;
	sf::RenderStates states;
	Graph():value{}, primitive{sf::PrimitiveType::Points}, states{}
	{}
	void clip(float x_min, float x_max, float y_min, float y_max)
	{
		auto e = value.begin();
		auto n = value.end();
		auto w = e;
		while(e!=n)
		{
			*w = *e++;
			auto p = w->position;
			if(
				x_min < p.x && p.x < x_max
			&&	y_min < p.y && p.y < y_max
			)
				++w;
		}
		while(w!=value.end())value.pop_back();
	}
	void plot(sf::Vector2f p, sf::Color k = sf::Color::White)
	{
		value.push_back({p,k});
	}

	virtual void draw(sf::RenderTarget& target,sf::RenderStates states)const
	{
		states.transform *= this->states.transform;
		auto t = value.size();
		if (t) target.draw(&value[0], t, primitive, states);
	}
};
class View : public sf::Drawable
{
 private:
	using clock = std::chrono::steady_clock;
	sf::RectangleShape tl;
	sf::Font freefont;
	sf::Text stats[2];
	Graph graph;
	unsigned width,height;
 public:
	View():tl({5,5}),freefont(),stats(),graph()
	{
		tl.setFillColor(sf::Color::Red);
		freefont.loadFromFile("FreeMono.ttf");
		stats[0] = sf::Text(sf::String("Test"),freefont);
		stats[1] = sf::Text(sf::String("Test"),freefont);
		stats[1].setPosition({220,0});
		for(int i = -1000; i <= 1000; i++)
		{
			float k = static_cast<float>(i)/5.f;
			graph.plot({27.f*k,k*k*k/27.f});
		}
	}
	void dimention(sf::VideoMode&v)
	{
		width = v.width;
		height = v.height;
		auto hw = width/2.f;
		auto hh = height/2.f;
		graph.states.transform.translate(hw,hh).scale(1.f,-1.f);
		graph.clip(-hw,hw,-hh,hh);
	}
	void update(Model_delta model)
	{
		auto t = model.time.count();
		constexpr auto precision = 1000000000;
		constexpr auto nano = "000000000";
		auto seconds = t/precision;
		auto partial = t%precision;
		auto out0
			= std::to_string(seconds)
			+ "."
			+ &nano
				[
					partial < 10? 1:
					partial < 100? 2:
					partial < 1000? 3:
					partial < 10000? 4:
					partial < 100000? 5:
					partial < 1000000? 6:
					partial < 10000000? 7:
					partial < 100000000? 8:
					9
				]
			+ std::to_string(partial)
		;
		auto out1
			= std::to_string(1000000000.0/t)
			+ "fps "
		;
		stats[0].setString(out0);
		stats[1].setString(out1);
	}
	virtual void draw(sf::RenderTarget& target,sf::RenderStates states)const
	{
		target.draw(tl, states);
		for(auto& e : stats)target.draw(e, states);
		target.draw(graph,states);
	}
};
class Input
{
 private:
	using clock = std::chrono::steady_clock;
	std::chrono::time_point<clock> t_point;
	bool fullscreen;
 	
	void pause(sf::RenderWindow& window)
	{
		// TODO
		window.close();
	}
 public:
 	Input():t_point{clock::now()}, fullscreen{false}{}
	Model_delta operator()(sf::RenderWindow& window)
	{
		static constexpr auto isKey = sf::Keyboard::isKeyPressed;
		Model_delta model{};
		sf::Event event;
		while ( window.pollEvent (event) )
		{
			if ( event.type == sf::Event::Closed ) window.close();
		}

		if(isKey(sf::Keyboard::Key::Escape)) pause(window);
		if (!fullscreen && isKey(sf::Keyboard::Key::LControl)
		  && isKey(sf::Keyboard::Key::F)
		  )
		{
			window.close();
			window.create(sf::VideoMode::getDesktopMode(), TITLE, sf::Style::Fullscreen);
			fullscreen = true;
		}
		auto t_now = clock::now();
		model.time = t_now - t_point;
		t_point = t_now;
		return model;
	}
};
int main(int argc, char**argv)
{
	auto screen_options = sf::VideoMode::getDesktopMode();
	View view;
	Input input;
	sf::RenderWindow window;

	window.setPosition({0,0});
//	window.setFramerateLimit(0);
//	window.setVerticalSyncEnabled(0);
	window.create
	(	screen_options
	,	TITLE
	,	sf::Style::None
	);
	window.requestFocus();
	view.dimention(screen_options);
/*/	for(auto&e : full_screen_options)
	{
		std::cout << e << std::endl;
	}// */

	while (window.isOpen())
	{
		view.update(input(window));
		window.clear();
		window.draw(view);
		window.display();
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(5));
	//
	return 0;
}
