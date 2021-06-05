///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  Copyright (c) 2021 ____   Michael  Julian  Moran    All rights reserved  //
//                                                                           //
//                        Not intended for distribution.                     //
//                                                                           //
//                            Use at your own risk.                          //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

//#include<iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <random>
#include <limits>
#include <type_traits>
#include <SFML/Graphics.hpp>

constexpr char TITLE[] = "Hackz for days"; // TITLE

struct Model_delta
{
// TODO
	std::chrono::nanoseconds time;
	
};
template <typename T>struct Stat
{
	T min = std::numeric_limits<T>::max();
	T max = 0;
	T ave = 0;
	unsigned long long count = 0;
	void add(T n)
	{
		if(n < min)min = n;
		if(max < n)max = n;
		ave = (static_cast<T>(count) * ave + n) / static_cast<T>(1+count);
		count += 1;
	}
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
 public:
	using type = unsigned long long;
	using vtype = float;
	std::vector<sf::Vertex> value;
	sf::PrimitiveType primitive;
	sf::RenderStates states;
	Graph(bool line=false):value{}, primitive{line?sf::PrimitiveType::LineStrip:sf::PrimitiveType::Points}, states{}
	{}
	void clip(vtype x_min, vtype x_max, vtype y_min, vtype y_max)
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
			else
			{
				// TODO save *w to clip buffer
			}
		}
		// TODO add values from clip buffer within the frame
		// TODO save w through value.end() to clip buffer
		while(w!=value.end())value.pop_back();
	}
	void plot(sf::Vector2f p, sf::Color k = sf::Color::White)
	{
		value.push_back({p,k});
	}

	virtual void draw(sf::RenderTarget& target,sf::RenderStates states)const
	{
		auto t = value.size();
		states.transform *= this->states.transform;
		if (t) target.draw(&value[0], t, primitive, states);
	}
};
class View : public sf::Drawable
{
 private:
	using clock = std::chrono::steady_clock;
	sf::RectangleShape tl;
	sf::Font freefont;
	sf::Text stats[8];
	Graph graph;
	unsigned width,height;
	Stat<double> fps;
	Stat<double> full_fps;
 public:
	View():tl({5,5}),freefont(),stats(),graph(true),width{},height{},fps{}
	{
		tl.setFillColor(sf::Color::Red);
		freefont.loadFromFile("FreeMono.ttf");
		stats[0] = sf::Text(sf::String("Test"),freefont);
		stats[1] = sf::Text(sf::String("Test"),freefont);
		stats[1].setPosition({230,0});
		stats[2] = sf::Text(sf::String("Test"),freefont);
		stats[2].setPosition({530,0});
		stats[3] = sf::Text(sf::String("Test"),freefont);
		stats[3].setPosition({760,0});
		stats[4] = sf::Text(sf::String("Test"),freefont);
		stats[4].setPosition({990,0});
		stats[5] = sf::Text(sf::String("Test"),freefont);
		stats[5].setPosition({1200,0});
		stats[6] = sf::Text(sf::String("Test"),freefont);
		stats[6].setPosition({1450,0});
		stats[7] = sf::Text(sf::String("Test"),freefont);
		stats[7].setPosition({1700,0});
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
	template<typename T> inline const T bXdigits(T num)
	{
		using namespace std;
		static_assert(is_integral<decltype(num)>::value,"Assumes integral type.");
		return num < 1000000000?
		(
			num < 10000
			?	num < 100
				?	num < 10
					?	1
					:	2
				:	num < 1000
					?	3
					:	4
			:	num < 1000000
				?	num < 100000
					?	5
					:	6
				:	num < 10000000
					?	7
					:	num < 100000000
						?	8
						:	9
		):(
			throw "Couldn't be bothered to support 10+digits"
		);
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
			+ &nano[bXdigits(partial)]
			+ std::to_string(partial)
		;
		auto cur = 1000000000.0/t;
		auto out1
			= std::to_string(cur)
			+ "fps "
		;
		fps.add(cur);
		full_fps.add(cur);
		stats[0].setString(out0);
		stats[1].setString(out1);
		stats[2].setString(std::to_string(full_fps.min));
		stats[3].setString(std::to_string(fps.min));
		stats[4].setString(std::to_string(fps.ave));
		stats[5].setString(std::to_string(full_fps.ave));
		stats[6].setString(std::to_string(fps.max));
		stats[7].setString(std::to_string(full_fps.max));
		if(fps.count > fps.ave*10)
		{
			fps.max = (fps.ave + fps.max)/2;
			fps.min = (fps.ave + fps.min)/2;
			fps.count /= 10;
		}
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
