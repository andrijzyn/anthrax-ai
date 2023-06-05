#pragma once

#include "vkdefines.h"
#include "vkbuilder.h"
#include "vkdevices.h"
#include "vkrenderer.h"
#include "vkpipeline.h"

#include "levelmanager.h"

#include "vkdebug.h"

#ifdef OS_WINDOWS
	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif

class Engine {

public:

#ifdef OS_WINDOWS
	HWND						hwnd;
	HINSTANCE					hinstance;
#endif
#ifdef OS_LINUX
	xcb_connection_t* 			connection;
	xcb_window_t 				window;
	xcb_screen_t* 				screen;
	xcb_atom_t 					wmProtocols;
	xcb_atom_t 					wmDeleteWin;
#endif

	VkBuilder 					Builder;
	LevelManager				Levels;

#ifdef OS_WINDOWS
	void 						wininitwindow();
	void 						runwindows();
#endif
#ifdef OS_LINUX
	bool 						running = true;
	int 						frameCounter;
	int 						fpsTimer;
	int 						lastFPS;

	void 						linuxinitwindow();
	void  						runlinux();
	bool 						handleEvent(const xcb_generic_event_t *event);
#endif
	void 						init();
	void 						cleanup();

private:

	DeletionQueue 				Deletor;
	int 						FrameNumber = 0;
	int 						BgNumber = 0;

	void 						draw();
	void 						drawobjects(VkCommandBuffer cmd,RenderObject* first, int count);
	
	void  						ui();

	void 						initscene();
	void 						initvulkan();
	void 						initimgui();

};
