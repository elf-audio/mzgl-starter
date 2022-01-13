

#include "App.h"

class MyApp : public App {
public: 
	MyApp(Graphics &g) : App(g) {}

	void draw() {
		g.clear(0, 0, 0);
		g.setColor(1, 0, 0);
		g.drawRect(0, 0, g.width/2, g.height/2);
	}
};


App *instantiateApp(Graphics &g) {
	g.width = 512;
	g.height = 512;

	App *app = new MyApp(g);

	return app;
}

