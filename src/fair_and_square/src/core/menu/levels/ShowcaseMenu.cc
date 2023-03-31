#include "ShowcaseMenu.hh"

// cuttlefish
#include "menu/Menu.hh"
#include "sprite/Picture.hh"
#include "text/Text.hh"
#include "texture/Texture.hh"
#include "window/Window.hh"

#include "core/game/showcase/RunShowcase.hh"

using namespace cuttlefish;

namespace
{

void quit(bool &quit_p)
{
	quit_p = true;
}

} // namespace

namespace fas
{


void showcaseMenu(cuttlefish::Window &window_p, bool &exit_p)
{
	auto last_l = std::chrono::steady_clock::now();
	double elapsed_l = 0.;
	bool quit_l = false;
	bool fullscreen_l = false;

	cuttlefish::Menu menu_l(window_p.getWidth()/2, 120, 5);

	Picture octopus_l(window_p.loadTexture("resources/octopus.png"), 64, 64, {2}, {1});
	octopus_l.setDestination(window_p.getWidth()-150, window_p.getHeight()-150, 128, 128);

{
	Picture * button_l = new Picture(window_p.loadTexture("resources/button.png"), 256, 64, {1, 1 ,1}, {1, 1, 1});
	Text * text_l = new Text(&window_p, {0, 0, 0}, 0, 0);
	text_l->setText("AoE Showcase");
	menu_l.addButton(button_l, text_l, std::bind(fas::runAoeShowcase, std::ref(window_p)));
}
{
	Picture * button_l = new Picture(window_p.loadTexture("resources/button.png"), 256, 64, {1, 1 ,1}, {1, 1, 1});
	Text * text_l = new Text(&window_p, {0, 0, 0}, 0, 0);
	text_l->setText("Chaining Showcase");
	menu_l.addButton(button_l, text_l, std::bind(fas::runChainingShowcase, std::ref(window_p)));
}
{
	Picture * button_l = new Picture(window_p.loadTexture("resources/button.png"), 256, 64, {1, 1 ,1}, {1, 1, 1});
	Text * text_l = new Text(&window_p, {0, 0, 0}, 0, 0);
	text_l->setText("Dot Showcase");
	menu_l.addButton(button_l, text_l, std::bind(fas::runDotShowcase, std::ref(window_p)));
}
{
	Picture * button_l = new Picture(window_p.loadTexture("resources/button.png"), 256, 64, {1, 1 ,1}, {1, 1, 1});
	Text * text_l = new Text(&window_p, {0, 0, 0}, 0, 0);
	text_l->setText("Lifesteal Showcase");
	menu_l.addButton(button_l, text_l, std::bind(fas::runLifestealShowcase, std::ref(window_p)));
}
{
	Picture * button_l = new Picture(window_p.loadTexture("resources/button.png"), 256, 64, {1, 1 ,1}, {1, 1, 1});
	Text * text_l = new Text(&window_p, {0, 0, 0}, 0, 0);
	text_l->setText("Main Menu");
	menu_l.addButton(button_l, text_l, std::bind(quit, std::ref(quit_l)));
}

	//Event handler
	SDL_Event e;

	while( !quit_l && !exit_p )
	{
		//Handle events on queue
		while( SDL_PollEvent( &e ) != 0 )
		{
			//User requests quit_l
			if( e.type == SDL_QUIT )
			{
				exit_p = true;
			}

			menu_l.handleEvent(window_p, e);
		}

		window_p.clear();

		menu_l.display(elapsed_l, window_p);

		octopus_l.update(elapsed_l);
		octopus_l.display(window_p);

		window_p.draw();

		auto cur_l = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds_l = cur_l-last_l;
		elapsed_l = elapsed_seconds_l.count();
		last_l = cur_l;
	}
}

} // namespace fas
