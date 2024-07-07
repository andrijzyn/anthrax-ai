#include "anthraxAI/vkengine.h"

void Engine::run() {
#if defined(AAI_LINUX)
	runlinux();
#elif defined(AAI_WINDOWS)
	runwindows();
#endif
}

void Engine::init() {
#if defined(AAI_LINUX)
	linuxinitwindow();
#elif defined(AAI_WINDOWS)
	wininitwindow();
#endif

	initengine(Level);
	initvulkan();
	initscene();
	initimgui();

}

void Engine::initvulkan() {

	Builder.buildinstance();

#if defined(AAI_WINDOWS)
	Builder.buildwinsurface(hwnd, hinstance);
	Builder.initdevicebuilder(hwnd);
#endif
#ifdef AAI_LINUX
	Builder.buildlinuxsurface(connection, window);
	Builder.initdevicebuilder(WindowExtend);
#endif
	Builder.buildphysicaldevice();

	Builder.buildlogicaldevice();

	Builder.buildswapchain();
	Builder.buildimagesview();
	
	Builder.initrenderbuilder();
	Builder.buildcommandpool();

	Builder.builddepthbuffer();
	Builder.buildmainimage();
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
	EditorCamera.setposition({ 0.f, 0.f, 3.0f});
	EditorCamera.setdirections();

	Level.getbackground().setposition({0, 0});
	Level.getbackground().setpath("placeholder.jpg");
	Level.gettrigger().reserve(10);
	Level.getobject().reserve(10);

	resources[TYPE_BACKGROUND] = {Level.getbackground().getpath(), { 0, 0, 0 }, false};
}

void Engine::initscene() {

	int i = 0;
	int objcounter = 1;
	for (auto& list : resources) {
		if (list.second.texturepath == "") {
            continue;
        }

		RenderObject tri;

		if (list.first >= TYPE_MODEL) {
			tri.ID = objcounter;
			tri.type = TYPE_MODEL;
			objcounter++;
			if (list.first >= TYPE_GIZMO) {
				tri.type = TYPE_GIZMO;
				tri.ID = list.first;
			}
			Builder.descriptors.updatesamplerdescriptors(list.second.texturepath);
			if (animator.hasanimation(list.first)) {
				tri.material = Builder.getmaterial("animated");
				tri.animated = true;
				tri.ID = list.first;
			}
			else {
			tri.material = Builder.getmaterial("monkey");

			}
			tri.model = Builder.getmodel(list.first);
		}
		else {
			Builder.descriptors.updatesamplerdescriptors(list.second.texturepath);
			tri.material = Builder.getmaterial("defaultmesh");
			tri.mesh = Builder.getmesh(list.first);

		}

		tri.pos = list.second.pos;
		tri.textureset = &Builder.getsamplerset()[i];
		tri.debugcollision = list.second.debugcollision;
		Builder.pushrenderobject(tri);
		i++;
	}

	RenderObject debug;
	debug.material = Builder.getmaterial("debug");
	debug.debug = true;
	Builder.pushrenderobject(debug);
}

void Engine::initresources()
{
	int triggersize = Level.gettrigger().size();
	int objectsize = Level.getobject().size();
	int k = 0;
	for (int i = 0; i < objectsize; i++) {
		if (Level.getobject()[i]->getpath() != "") {
			resources[TYPE_OBJECT + k] = 	{Level.getobject()[i]->getpath(), 
											{Level.getobject()[i]->getposition().x, Level.getobject()[i]->getposition().y, 0}, 
											 Level.getobject()[i]->collision, Level.getobject()[i]->animation};
			Level.getobject()[i]->ID = k;
			k++;
		}
	}
	k = objectsize;
	// should be here ause i don't handle move/catch code for triggers yet
	for (int i = 0; i < triggersize; i++) {
		if (Level.gettrigger()[i].getpath() != "") {
			resources[TYPE_OBJECT + k] = 	{Level.gettrigger()[i].getpath(), 
											{Level.gettrigger()[i].getposition().x, Level.gettrigger()[i].getposition().y, 0},
											 Level.gettrigger()[i].collision, Level.gettrigger()[i].animation};
			k++;
		}
	}
	if (Level.getplayer()->getpath() != "") {
		resources[TYPE_PLAYER] = 	{Level.getplayer()->getpath(), 
									{Level.getplayer()->getposition().x, Level.getplayer()->getposition().y, 0},
									 Level.getplayer()->debugcollision, Level.getplayer()->animation}; // player has to be always second -- stupid
	}
	if (Level.getbackground().getpath() != "") {
		resources[TYPE_BACKGROUND] = {Level.getbackground().getpath(), {0, 0, 0}, false, false}; // background for some reason should be always top, looks kinda broken
	}

	resources[TYPE_MODEL] = {"zeroone.png", {0, 0, -10}, false, false}; 
	resources[TYPE_MODEL + 1] = {"bg2.png", {-2, 5, -10}, false, false}; 
	resources[TYPE_MODEL + 2] = {"floor.jpg", {2, 0, -10}, false, false};
	resources[TYPE_MODEL + 3] = {"human.png", {5, 0, -10}, false, false};
	// resources[TYPE_MODEL + 4] = {"human.png", {2, 0, -10}, false, false};
	//gizmo
	resources[TYPE_GIZMO + 0] = {"dummy.png", {0, 0, 0}, false, false};
	resources[TYPE_GIZMO + 1] = {"dummy.png", {0, 0, 0}, false, false};
	resources[TYPE_GIZMO + 2] = {"dummy.png", {0, 0, 0}, false, false};

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
	Builder.loadmeshes();

	Builder.loadmodel("models/monkeytextured.obj", deltatime.count(), TYPE_MODEL + 0);
	Builder.loadmodel("models/cube.obj",deltatime.count(),  TYPE_MODEL + 1);
	Builder.loadmodel("models/sphere.obj",deltatime.count(),  TYPE_MODEL + 2);

	Builder.loadmodel("models/gizmox.obj",deltatime.count(),  TYPE_GIZMO + 0);
	Builder.loadmodel("models/gizmoy.obj",deltatime.count(),  TYPE_GIZMO + 1);
	Builder.loadmodel("models/gizmoz.obj",deltatime.count(),  TYPE_GIZMO + 2);
	
	startms = getcurtime();
	// Builder.loadmodel("models/forgiving-mesh-anim.dae", deltatime.count(), TYPE_MODEL + 3);
	// animator.init({"models/forgiving-mesh-anim.dae", "models/body-jab-cross.dae", "models/double-kick-anim.dae"} , TYPE_MODEL + 3);
	
	Builder.loadmodel("models/Walking-mesh-anim.dae", deltatime.count(), TYPE_MODEL + 3);
	animator.init({"models/Walking-mesh-anim.dae", "models/Dying-anim.dae"} , TYPE_MODEL + 3);

	animprepared = true;
	
	for (int i = TYPE_MODEL; i < TYPE_MODEL + 4; i++) {
		Builder.updatemodel(Builder.getmodel(i));
	}
	for (int i = TYPE_GIZMO; i < TYPE_GIZMO + 3; i++) {
		Builder.updatemodel(Builder.getmodel(i));
	}

	initscene();

	for (int i = 0; i < Level.getobject().size(); i++) {
        if (Level.getobject()[i]->animation) {
   			DebugImGuiAnim.push_back({1, ImGui_ImplVulkan_AddTexture(Builder.gettexture(Level.getobject()[i]->getpath())->sampler, Builder.gettexture(Level.getobject()[i]->getpath())->imageview, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL), Level.getobject()[i]->getpath()});
		}
	}
	if (Level.getplayer()->animation) {
   		DebugImGuiAnim.push_back({1, ImGui_ImplVulkan_AddTexture(Builder.gettexture(Level.getplayer()->getpath())->sampler, Builder.gettexture(Level.getplayer()->getpath())->imageview, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL), Level.getplayer()->getpath() });
	}
}

void Engine::cleanup() {
	vkDeviceWaitIdle(Builder.getdevice());

	Builder.cleartextureset();
	Builder.clearimages();
	Builder.clearattachments();
	Builder.clearmeshes();
	resources.clear();
	Builder.renderqueue.clear();

	Builder.clearpipeline();
	Builder.cleanswapchain();
	Builder.clearframebuffers();
	Builder.cleanall();
	for (int i = 0; i < Level.getobject().size(); i++) {
		delete Level.getobject()[i];
	}

#if defined(AAI_LINUX)
    ImGui_ImplX11_Shutdown();
#elif defined(AAI_WINDOWS)
	ImGui_ImplWin32_Shutdown();
#endif
    ImGui::DestroyContext();
}
