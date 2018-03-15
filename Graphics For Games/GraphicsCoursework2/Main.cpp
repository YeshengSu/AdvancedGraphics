#include "../../nclgl/window.h"
#include "Renderer2.h"

#pragma comment(lib, "nclgl.lib")

int main() {	
	Window w("Coursework Scene 2!", 1024,768,false);
	if(!w.HasInitialised()) {
		return -1;
	}

	Renderer2 renderer(w);
	if(!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while(w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		renderer.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer.RenderScene();
	}

	return 0;
}