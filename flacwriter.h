#ifndef FLACWRITER_H
#define FLACWRITER_H

#include <vector>
#include <iostream>
#include <cstring>
#include <mutex>
#include <queue>
#include <thread>
#include <atomic>
#include <cassert>
#include <chrono>

#include <FLAC++/metadata.h>
#include <FLAC++/encoder.h>

#include "macros.h"



class FLACWriter
{
	std::atomic<bool> terminate;
	std::queue<std::vector<char>> queue;
	std::mutex queue_mutex;
	std::thread thread;
	FLAC::Encoder::File encoder;
public:
	FLACWriter(std::string filename, int sample_rate);
	~FLACWriter();
	void WriteMe(char* data, int length);
private:
	void ThreadMe();
};


#endif

