

#include "App.h"

class MyApp : public App {
public: 
	MyApp(Graphics &g) : App(g) {}

	void draw() {
		g.clear(0.5, 1, 0);
		g.setColor(1, 0, 0);
		g.drawRect(100, 100, 100, 100);
	}
};


App *instantiateApp(Graphics &g) {
	g.width = 828;
	g.height = 1792;

	App *app = new MyApp(g);

	return app;
}

