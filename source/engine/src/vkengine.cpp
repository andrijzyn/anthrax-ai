#include "../includes/vkengine.h"

void Engine::init() {
	initengine(Level);
	initvulkan();
	initscene();
	initimgui();
}

void Engine::initvulkan() {

	Builder.buildinstance();

#ifdef OS_WINDOWS
	Builder.buildwinsurface(hwnd, hinstance);
	Builder.initdevicebuilder(hwnd);
#endif
#ifdef OS_LINUX
	Builder.buildlinuxsurface(connection, window);
	Builder.initdevicebuilder(WindowExtend);
#endif
	Builder.buildphysicaldevice();

	Builder.buildlogicaldevice();

	Builder.buildswapchain();
	Builder.buildimagesview();
	
	Builder.initrenderbuilder();
	Builder.buildcommandpool();

	Builder.buildrenderpass();
	Builder.builframebuffers();
	
	Builder.startsync();

	Builder.inittexture(resources);
	Builder.loadimages();

	Builder.initdescriptors();
	Builder.builddescriptors();

	Builder.initpipelinebuilder();
	Builder.buildpipeline(Level.check);

	Builder.initmeshbuilder();
	Builder.loadmeshes();

}

void Engine::initengine(LevelManager &levels) {

	Level.getbackground().setposition({0, 0});
	Level.getbackground().setpath("placeholder.jpg");
	Level.gettrigger().reserve(10);
	Level.getobject().reserve(10);

	resources[TYPE_BACKGROUND] = {Level.getbackground().getpath(), Level.getbackground().getposition(), false};
}

void Engine::initscene() {

	int i = 0;
	for (auto& list : resources) {
		if (list.second.texturepath == "") {
            continue;
        }
		Builder.descriptors.updatesamplerdescriptors(list.second.texturepath);

		RenderObject tri;
		tri.mesh = Builder.getmesh(list.first);
		tri.material = Builder.getmaterial("defaultmesh");

		tri.textureset = &Builder.getsamplerset()[i];
		tri.debugcollision = list.second.debugcollision;
		Builder.pushrenderobject(tri);

		i++;
	}
}

void Engine::initresources()
{
	int triggersize = Level.gettrigger().size();
	int objectsize = Level.getobject().size();
	int k = 0;
	for (int i = 0; i < objectsize; i++) {
		if (Level.getobject()[i].getpath() != "") {
			resources[TYPE_OBJECT + k] = {Level.getobject()[i].getpath(), Level.getobject()[i].getposition(), Level.getobject()[i].collision, Level.getobject()[i].animation};
			k++;
		}
	}
	k = objectsize;
	// should be here ause i don't handle move/catch code for triggers yet
	for (int i = 0; i < triggersize; i++) {
		if (Level.gettrigger()[i].getpath() != "") {
			resources[TYPE_OBJECT + k] = { Level.gettrigger()[i].getpath(), Level.gettrigger()[i].getposition(), Level.gettrigger()[i].collision, Level.getobject()[i].animation};
			k++;
		}
	}
	if (Level.getplayer()->getpath() != "") {
		resources[TYPE_PLAYER] = {Level.getplayer()->getpath(), Level.getplayer()->getposition(), Level.getplayer()->debugcollision, Level.getplayer()->animation}; // player has to be always second -- stupid
	}
	if (Level.getbackground().getpath() != "") {
		resources[TYPE_BACKGROUND] = {Level.getbackground().getpath(), Level.getbackground().getposition(), false, false}; // background for some reason should be always top, looks kinda broken
	}
}

void Engine::reloadresources() {
	vkDeviceWaitIdle(Builder.getdevice()); // test it

	Builder.clearimages();
	Builder.cleartextureset();
	Builder.clearmeshes();

	resources.clear();

	Builder.renderqueue.clear();

	initresources();

	Builder.inittexture(resources);
	Builder.loadimages();

	Builder.initdescriptors();
	Builder.builddescriptors();

	Builder.initmeshbuilder();
	Builder.loadmeshes(); // check why here are weird x,y pos for resource

	initscene();
}

void Engine::cleanup() {
	vkDeviceWaitIdle(Builder.getdevice());

	Builder.cleartextureset();
	Builder.clearimages();
	Builder.clearmeshes();
	resources.clear();
	Builder.renderqueue.clear();

	Builder.clearpipeline();
	Builder.cleanswapchain();
	Builder.clearframebuffers();
	Builder.cleanall();

    ImGui_ImplX11_Shutdown();
    ImGui::DestroyContext();
}
