#include "Novara.h"
#include <Psio/Log.h>
#include <cstring>

int main(int argc, char* argv[])
{
	Novara* app = new Novara();
	app->setAppname("Novara");

	app->init(argc, argv);

	int argi = 1;
	bool scene_set = false;
	std::string home_scene;

	while (argi < argc) {
		if (!strcmp(argv[argi], "-d")) {
			argi++;
			app->setAppDir(argv[argi]);
			LOG(LOG_INFO) << "Set //local to directory: " << argv[argi];
		}
		else if (argv[argi][0] != '-') {
			home_scene = argv[argi];
			scene_set = true;
		}
		argi++;
	}

	app->setup();
	std::string world_path = scene_set ? home_scene : "//novara/world.scene";
	if (app->openWorld(world_path) != 0) {
		return -1;
	}

	app->loop();

	app->close();

	delete app;

	return 0;
}
