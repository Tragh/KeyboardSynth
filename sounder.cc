
#include "sounder.h"

namespace SOUNDER
{

class Note_Simple : public Note
{
public:
	float initial_volume;
	float final_volume;
	std::function<float(float,float)> generator;
	void UpdateVolume(float change_rate)
	{
		if(initial_volume < final_volume) //are we doing down or up?
			volume=std::min(final_volume, volume+(final_volume-initial_volume)/change_rate);
		else
			volume=std::max(final_volume, volume+(final_volume-initial_volume)/change_rate);
	}
	float GetSample(uint32_t sample_rate)
	{
		return volume*generator(float(ticker*frequency)/sample_rate,frequency);
	}
	void SetMaxVolume(float mv)
	{
		initial_volume=volume;
		final_volume=mv*user_volume;
	}
	void Deactivate()
	{
		alive=false;
		final_volume=0;
		initial_volume=volume;
	}
};

}


Sounder::Sounder(std::string outfile, int sample_rate_):
	sample_time(0),
	writer(outfile,sample_rate_),
	note_key((uint64_t)1<<32),
	sample_rate(sample_rate_),
	distribution(-0.25,0.25)
{
	assertm(SDL_Init(SDL_INIT_AUDIO) >= 0, "SDL2 failed to init audio");
	SDL_zero(wav_spec);
	wav_spec.freq = sample_rate;
	wav_spec.format = AUDIO_S32SYS;
	wav_spec.channels = 2;
	wav_spec.samples = 1024;
	wav_spec.callback = MyAudioCallback;
	wav_spec.userdata = this;
	assertm(SDL_OpenAudio(&wav_spec, NULL) >= 0, "SDL2 failed to open audio");
}

void Sounder::AddTerminatingNote(float frequency, float user_volume, const std::function<float(float,float)> &generator, double duration, double when)
{
	if(when==-1) when=(double)sample_time/sample_rate;
	AddNote_(++note_key,frequency,user_volume,generator,when);
	RemoveNote(note_key,std::max(when+0.01,when+duration-change_rate/sample_rate));
}

void Sounder::AddNote(uint32_t key, float frequency, float user_volume, const std::function<float(float,float)> &generator, double when)
{
	AddNote_(key, frequency,user_volume, generator, when); //only allow 32bit keys for AddNote, reserve the higher 32bits for internal use
}

void Sounder::RemoveNote(uint64_t key, double when)
{
	std::lock_guard<std::mutex> lock(callback_mutex);
	uint64_t time = (when==-1.0) ? (uint64_t)sample_time : (uint64_t)(when*sample_rate);
	if(future_notes.count(time)==0) future_notes[time]=std::vector<note_action_t>();
	future_notes[time].push_back({false, key, 0, 0, NULL});
}

void Sounder::SetBuffer(int32_t* stream32, int len)
{
	{
		std::lock_guard<std::mutex> lock(callback_mutex);

		float tmp;
		for(int i=0; i<len/2; i++) {
			sample_time++;
			ProcessNotes(); //activate notes that need activating
			tmp=0;
			for(auto &pair:active_notes) {
				pair.second->UpdateVolume(change_rate);
				pair.second->ticker++;
				tmp+=pair.second->GetSample(sample_rate);
			}
			if(sample_time>=backing.start_time && sample_time<backing.end_time)
				tmp+=backing.buffer[sample_time-backing.start_time];
			float norm=std::abs(tmp/max_volume);
			if(norm>0.5) tmp=std::copysign((std::tanh(2*norm-1)+1)/2,tmp)*max_volume; //to prevent clipping, not that sound should ever clip!
			if(tmp>max_volume || tmp<-max_volume) std::cout << "sound clipping" << std::endl;
			stream32[2*i]=tmp; //write to sound buffer RIGHT STREAM
			stream32[2*i+1]=tmp; //write to sound buffer LEFT STREAM
		}

		for(auto pair=active_notes.begin(); pair!=active_notes.end();){
			if(pair->second->alive!=1 && pair->second->volume==0){
				pair=active_notes.erase(pair);
			} else {
				++pair;
			}
		}

	} //ends the scope for the lockguard

	writer.WriteMe(reinterpret_cast<char*>(stream32), len*sizeof stream32[0]); //write to flac file, just a memcpy
}


void Sounder::AddNote_(uint64_t key, float frequency, float user_volume, const std::function<float(float,float)> &generator, double when=-1)
{
	std::lock_guard<std::mutex> lock(callback_mutex);
	uint64_t time = (when==-1.0) ? (uint64_t)sample_time : (uint64_t)(when*sample_rate);
	if(future_notes.count(time)==0) future_notes[time]=std::vector<note_action_t>();
	future_notes[time].push_back({true, key, frequency+distribution(random_generator) , user_volume, generator});
}

void Sounder::ProcessNotes()
{
	for(auto notes=future_notes.begin(); notes!=future_notes.end();notes=future_notes.erase(notes)) {
		if(notes->first > sample_time) break;
		for(auto &note : notes->second) {
			if(note.activate) ActivateNote(note.key,note.frequency,note.user_volume, note.generator);
			else DeactivateNote(note.key);
		}
	}
}

void Sounder::ActivateNote(int key, float frequency, float user_volume, const std::function<float(float,float)> &generator)
{
	if(active_notes.count(key)!=0) return; //note is already active!
	std::unique_ptr<SOUNDER::Note_Simple> N_S(new SOUNDER::Note_Simple());
	N_S->alive=1;
	N_S->frequency=frequency;
	N_S->user_volume=user_volume;
	N_S->generator=generator;
	N_S->initial_volume=0;
	N_S->time_activated=sample_time;
	N_S->SetMaxVolume(volume);
	active_notes[key]=std::move(N_S);

/*	int living=0;
	for(auto &pair : active_notes)
		if(pair.second->alive==true) living++;

	for(auto &pair : active_notes)
		if(pair.second->alive==true) {
			pair.second->SetMaxVolume(std::min(volume,(int)((float)max_volume/living)));
		}*/
}

void Sounder::DeactivateNote(int key)
{
	if(active_notes.count(key)==0) return; //no note to remove
	active_notes[key]->Deactivate();
}

void Sounder::SetBacking(std::vector<int32_t> buffer)
{
	//designed to keep the lock for as little time as possible.
	std::lock_guard<std::mutex> lock(callback_mutex);
	backing.buffer=std::move(buffer);
	backing.start_time=sample_time;
	backing.end_time=backing.start_time+backing.buffer.size();
}

void Sounder::SetBacking(const std::vector<float> &buffer_)
{
	//max_volume;
	std::vector<int32_t> buffer;
	for(auto &i : buffer_){
		const float norm=std::abs(i);
		if(norm>0.5){
			buffer.push_back(std::copysign((std::tanh(2*norm-1)+1)/2,i)*max_volume);
		}else{
			buffer.push_back(i*max_volume);
		}
	}
	SetBacking(buffer);
}

void Sounder::MyAudioCallback(void *userdata, Uint8 *stream, int len)
{
	int32_t* const &stream32 = reinterpret_cast<int32_t*>(stream); //using S32 audio.
	Sounder &sounder = *reinterpret_cast<Sounder*>(userdata); //cast sounder from userdata
	sounder.SetBuffer(stream32,len/4);
}
