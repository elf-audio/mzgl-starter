
#include "App.h"
#include "FliteLite.h"
#include "FloatBuffer.h"

class Slider : public Layer {
public:
	float &value;
	float minVal;
	float maxVal;
	Slider(Graphics &g, std::string name, float &val, float minVal, float maxVal)
		: Layer(g, name)
		, minVal(minVal)
		, maxVal(maxVal)
		, value(val) {
		interactive = true;
	}
	void draw() override {
		g.setColor(0.2);
		g.drawRect(*this);

		Rectf r = *this;
		r.width = mapf(value, minVal, maxVal, 0, r.width);
		g.setColor(0.5, 0, 0);
		g.drawRect(r);
		g.setColor(1);
		g.drawText(name, x + 5, y + height - 5);

		g.noFill();
		g.drawRect(*this);
		g.fill();
	}

	bool touchDown(float x, float y, int id) override {
		touchMoved(x, y, id);
		return true;
	}

	void touchMoved(float x, float y, int id) override {
		value = mapf(x, this->x, this->right(), minVal, maxVal, true);
	}
};

class Toggle : public Layer {
public:
	bool &value;

	Toggle(Graphics &g, std::string name, bool &val)
		: Layer(g, name)
		, value(val) {
		interactive = true;
	}
	void draw() override {
		if (value) {
			g.setColor(0.5, 0, 0);
		} else {
			g.setColor(0.2);
		}
		g.drawRect(*this);

		g.setColor(1);
		g.drawText(name, x + 5, y + height - 5);

		g.noFill();
		g.drawRect(*this);
		g.fill();
	}

	bool touchDown(float x, float y, int id) override {
		value = !value;
		return true;
	}
};
class MyApp : public App {
public:
	

	Slider *pitchSlider;
	Slider *formantSlider;
	Slider *speedSlider;
	Toggle *reverseToggle;
	
	
	
	std::shared_ptr<FliteLite> liveSynth;
	std::shared_ptr<FliteLite> nextSynth;
	std::shared_ptr<FliteLite> deadSynth;
	float pitch	  = 256;
	float formant = 0;
	float speed	  = 1;
	bool reverse = false;
	std::vector<double> mceps;
	
	
	MyApp(Graphics &g)
		: App(g) {
			mceps.resize(FliteLite::getNumMceps());
			say("Cleo, a spark of dawn, a name like gold, a story begun, tiny hands clutch the world with wonder, her gaze a universe untold, laughter like bells, a heart so bold, a future bright, a legacy spun, in her smile, the sun’s warmth won");
			
		pitchSlider	  = new Slider(g, "pitch", pitch, 20, 500);
		formantSlider = new Slider(g, "formant", formant, -1, 1);
		speedSlider	  = new Slider(g, "speed", speed, 0.1, 5);
		reverseToggle = new Toggle(g, "reverse", reverse);

		root->addChildren({pitchSlider, formantSlider, speedSlider, reverseToggle});
	}
	void say(std::string s) {
		std::thread([s, this]() {
			auto synth = std::make_shared<FliteLite>();
			synth->setLooping(true);
			synth->createParams(s);
			nextSynth = synth;
		}).detach();
	}
	void setup() override { doLayout(); }
	void resized() override { doLayout(); }
	void doLayout() {
		vec2 sliderDims(450, 50);

		pitchSlider->size(sliderDims);
		formantSlider->size(sliderDims);
		speedSlider->size(sliderDims);

		reverseToggle->size(sliderDims.x / 2, sliderDims.y);
		pitchSlider->position(30, 30);
		formantSlider->positionUnder(pitchSlider, 20);
		speedSlider->positionUnder(formantSlider, 20);
		reverseToggle->positionUnder(speedSlider, 20);
	}

	

	void draw() override {
		if (deadSynth != nullptr) {
			deadSynth = nullptr;
		}
		g.clear(0, 0, 0);
		g.setColor(0, 1, 0);

		auto spec = FliteLite::convertMcepsToSpectrum(mceps);

		int sub = 4;
		std::vector<float> sup(spec.size() / 4, 0);
		for (int i = 0; i < sup.size(); i++) {
			for (int j = 0; j < sub; j++) {
				sup[i] += spec[i * sub + j];
			}
			sup[i] /= sub;
		}
		float ww = g.width / (float) sup.size();
		for (int i = 0; i < sup.size(); i++) {
			Rectf r;
			g.setColor(0, 0.4, i / (float) sup.size());
			float v = sup[i] * 0.15;
			r.set(ww * i, g.height / 2, ww, g.height * v);
			g.drawRect(r);
		}

		float asp = sup[2] / sup[3];
		// bigger than 1 is o, smaller than 1 is eee
		vec2 v(2 / asp, 1);
		float vol = std::min(runningAve * 10.f, 1.f);
		v *= vec2(0.4 + 0.3 * vol, vol);

		g.noFill();
		g.setColor(1, 0, 1);
		std::vector<vec2> pts;
		float rad = 50;
		for (int i = 0; i < 100; i++) {
			float angle = mapf(i, 0, 100, 0, M_PI * 2);
			float xx	= g.width / 2 + cos(angle) * rad * v.x;
			float yy	= g.height / 2 + sin(angle) * rad * v.y;
			pts.push_back({xx, yy});
		}
		g.drawLineStrip(pts);
		g.fill();
	}
	FloatBuffer mono;
	float runningAve = 0.f;
	void keyDown(int k) {
		if (k == ' ') {
			for (int i = 0; i < root->getNumChildren(); i++) {
				root->getChild(i)->visible = !root->getChild(i)->visible;
			}
		} else if (k == 13) {
			dialogs.textbox("Enter text", "Enter text", "", [&](std::string s, bool) { say(s); });
		}
	}
	void audioOut(float *buff, int frames, int nchans) override {
		if (nextSynth != nullptr) {
			deadSynth = liveSynth;
			liveSynth = nextSynth;
			nextSynth = nullptr;
		}
		if (liveSynth == nullptr) {
			memset(buff, 0, sizeof(float) * frames * nchans);
			return;
		}
		mono.resize(frames);
		liveSynth->getMceps(mceps.data());
		liveSynth->setPitch(pitch);
		liveSynth->setFormantShift(formant);
		liveSynth->setSpeed(speed * (reverse ? -1 : 1));
		liveSynth->audioOut(mono);

		for (int i = 0; i < frames; i++) {
			buff[i * 2] = buff[i * 2 + 1] = mono[i];
			runningAve = 0.9995f * runningAve + 0.0005f * std::abs(mono[i]);
		}
	}
};

std::shared_ptr<AudioSystem> audioSystem;

std::shared_ptr<App> instantiateApp(Graphics &g) {
	g.width	 = 512;
	g.height = 512;

	audioSystem = std::make_shared<AudioSystem>();
	audioSystem->setBufferSize(64);
	audioSystem->setup(2, 2);

	auto app = std::make_shared<MyApp>(g);
	audioSystem->bindToApp(app.get());
	audioSystem->start();
	return app;
}
