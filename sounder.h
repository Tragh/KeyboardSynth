#ifndef SOUNDER_H
#define SOUNDER_H

#include <SDL2/SDL.h>
#include <random>
#include "flacwriter.h"

#include "macros.h"


namespace SOUNDER
{

class Note
{
public:
	bool alive;
	float frequency;
	float user_volume;
	float volume;
	uint64_t ticker;
	uint64_t time_activated;
	Note():ticker(0){};
	virtual ~Note()=default;
	virtual void UpdateVolume(float change_rate)=0;
	virtual float GetSample(uint32_t sample_rate)=0;
	virtual void SetMaxVolume(float mv)=0;
	virtual void Deactivate()=0;
};


}


class Sounder
{
	std::atomic<uint64_t> sample_time; //how many samples have elapsed since the start of the thingy.
	const int volume=1<<26; //volume of each note
	const int max_volume=1<<31-2; //max volume to avoid clipping
	const float change_rate=200; //fade in / fade out rate, higher is slower

	std::default_random_engine random_generator;
	std::uniform_real_distribution<float> distribution;

	struct note_action_t {
		bool activate;
		uint64_t key;
		float frequency;
		float user_volume;
		std::function<float(float,float)> generator;			//the wave generator!
	};

	struct backing_t {
		std::vector<int32_t> buffer;
		std::int64_t start_time=0;
		std::int64_t end_time=0;
	};
	
	backing_t backing;
	std::map<uint64_t,std::unique_ptr<SOUNDER::Note>> active_notes;

	std::map<uint64_t,std::vector<note_action_t>> future_notes;
	std::mutex callback_mutex;

	uint64_t note_key;

	SDL_AudioSpec wav_spec; //specs for what's played
	FLACWriter writer;
	uint32_t sample_rate;

public:
	Sounder(std::string outfile, int sample_rate_);

	void AddTerminatingNote(float frequency, float user_volume, const std::function<float(float,float)> &generator, double duration, double when=-1);

	void AddNote(uint32_t key, float frequency, float user_volume, const std::function<float(float,float)> &generator, double when=-1);

	void RemoveNote(uint64_t key, double when=-1);
	
	void SetBacking(std::vector<int32_t> buffer);
	void SetBacking(const std::vector<float> &buffer_);

	void SetBuffer(int32_t* stream32, int len);

private:

	void AddNote_(uint64_t key, float frequency, float user_volume, const std::function<float(float,float)> &generator, double when);

	void ProcessNotes();

	void ActivateNote(int key, float frequency, float user_volume, const std::function<float(float,float)> &generator);

	void UpdateVolume(std::unique_ptr<SOUNDER::Note> & note);

	void DeactivateNote(int key);

	static void MyAudioCallback(void *userdata, Uint8 *stream, int len);
};

#endif


