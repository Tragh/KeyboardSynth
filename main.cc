#include <SDL2/SDL.h>
#include <fstream>

#include "sounder.h"
#include "synths.h"
#include "macros.h"


constexpr int SAMPLE_RATE = 44100;

//c wse swa asc esw se cs aa


//#include "midiparser.cc"
#include "sdldrawer.cc"

#include <boost/python.hpp>
namespace bpy=boost::python;



class PyPlayerInterface{
	std::map<int,float> keymap;
	std::shared_ptr<Sounder> sounder;
	public:
	std::vector<float> buffer;
	PyPlayerInterface(){}
	PyPlayerInterface(std::shared_ptr<Sounder> sounder_):
		sounder(sounder_)
	{}
	void SetKeymap(int k,float f){
		keymap[k]=f;
	};
	float GetKeymap(int k){
		return keymap[k];
	}
	void WriteBuffer(uint64_t location, float value){
		if(location>=buffer.size())
			buffer.resize(location*1.1+1);
		buffer[location]=value/8;		
	}
	float PushNote(const float frequency, const float user_volume, const std::string synth, const double duration, const double when){
		if(SYNTHS::Synth.count(synth)==0){
			std::cout << "ERROR: Synth " << synth << " does not exist";
			return when+duration;
		}
		sounder->AddTerminatingNote(frequency, user_volume, SYNTHS::Synth[synth],duration,when);
		return when+duration;
	}
};

class PythonReader {
	bpy::object main;
	bpy::object name_space;
	public:
	PyPlayerInterface Player;
	PythonReader(std::shared_ptr<Sounder> sounder_): Player(sounder_)
	{
		try{
			Py_Initialize();
			main=bpy::import("__main__");
			name_space=main.attr("__dict__");
			name_space["PyPlayerInterface"] = bpy::class_<PyPlayerInterface>("PyPlayerInterface")
				.def("SetKeymap",&PyPlayerInterface::SetKeymap)
				.def("GetKeymap",&PyPlayerInterface::GetKeymap)
				.def("WriteBuffer", &PyPlayerInterface::WriteBuffer)
				.def("PushNote",&PyPlayerInterface::PushNote);
			name_space["Player"] = bpy::ptr(&Player);
	
			//bpy::object result = exec("t=3", name_space, name_space);
					
			
		}catch(bpy::error_already_set){
			PyErr_Print();
		}
	}
	std::vector<float> ExtractBuffer()
	{
		return Player.buffer;
	}
	void ExecFile(std::string file)
	{
		try{
			bpy::object result = exec_file(file.c_str(),name_space,name_space);
		}catch(bpy::error_already_set){
			PyErr_Print();
		}
	}
};


int main(int argc, char* argv[])
{
	std::ofstream logfile;
	logfile.open("log.txt");


	std::shared_ptr<Sounder> sounder = std::make_shared<Sounder>("out.flac",SAMPLE_RATE);

	PythonReader pythonReader(sounder);
	pythonReader.ExecFile("data/notes.py");
	pythonReader.ExecFile("data/keycodes.py");
	pythonReader.ExecFile("data/set_keymap.py");
	//pythonReader.ExecFile("script.py");
	sounder->SetBacking(pythonReader.ExtractBuffer());

	std::shared_ptr<SDL_Window> window;
	SDL_Renderer *renderer;
	assertm(SDL_Init( SDL_INIT_VIDEO ) >= 0, "SDL2 failed to init video");
	{
		SDL_DisplayMode current;
		SDL_GetCurrentDisplayMode(0, &current);
		std::shared_ptr<SDL_Window> win(SDL_CreateWindow("Window Name",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,current.w, current.h, SDL_WINDOW_MAXIMIZED | SDL_WINDOW_FULLSCREEN),SDL_DestroyWindow);
		window=win;
	}
	SDLDrawer drawer(window);
	//SDrawer sdrawer(drawer);



	//std::function<float(float)> drawer_wave = [&sdrawer](float ticks) -> float { return sdrawer.Wave(ticks)*sdrawer.Wave2(ticks*0.8)*2+sdrawer.Wave2(ticks*2);};

	for(int i=0; i<100; ++i) {
		//sounder->AddTerminatingNote(440, SYNTHS::triangle,0.3,i);  //adding some notes!
		//sounder->AddTerminatingNote(220, SYNTHS::triangle,0.2,0.5+i);
	}


	SDL_PauseAudio(0); //starts playing
	int done = 0;
	SDL_Event event;
	while(!done) {
		SDL_Delay(20);
		drawer.Clear();
		//sdrawer.Draw();
		drawer.Colour(255,0,0);
		drawer.DrawRect(0,0,drawer.width,drawer.height);
		drawer.Render();
		drawer.Present();

		while(SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_QUIT:
				done = 1;
				break;
			case SDL_KEYDOWN:

				if(pythonReader.Player.GetKeymap(event.key.keysym.sym)!=0) {
					if(SDL_GetModState() & KMOD_CAPS){
						logfile << pythonReader.Player.GetKeymap(event.key.keysym.sym) << "(caps) ";
						sounder->AddTerminatingNote(pythonReader.Player.GetKeymap(event.key.keysym.sym),1,SYNTHS::test,2);
					}else{
						logfile << pythonReader.Player.GetKeymap(event.key.keysym.sym) << " ";
						sounder->AddNote(event.key.keysym.sym,pythonReader.Player.GetKeymap(event.key.keysym.sym),1,SYNTHS::test);
					}
				}

				break;
			case SDL_KEYUP:

				if(pythonReader.Player.GetKeymap(event.key.keysym.sym)!=0)
					sounder->RemoveNote(event.key.keysym.sym);
				if(event.key.keysym.sym == SDLK_ESCAPE) {
					done = 1;
				}
				break;


		/*	case SDL_MOUSEMOTION:
				sdrawer.MouseMove(event.motion.x, event.motion.y);
				break;
			case SDL_MOUSEBUTTONDOWN:
				if(event.button.button==SDL_BUTTON_LEFT) {
					sdrawer.MouseDown(event.button.x,event.button.y);
				}
				if(event.button.button==SDL_BUTTON_RIGHT) {
					sdrawer.MouseDownR(event.button.x,event.button.y);
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if(event.button.button==SDL_BUTTON_LEFT) {
					sdrawer.MouseUp();
				}
				if(event.button.button==SDL_BUTTON_RIGHT) {
					sdrawer.MouseUpR();
				}
				break;
		*/



			default:
				break;
			}
		}
	}


	SDL_CloseAudio();
	SDL_Quit();
	return 0;
}





