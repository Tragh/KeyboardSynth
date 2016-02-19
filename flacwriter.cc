#include "flacwriter.h"




FLACWriter::FLACWriter(std::string filename, int sample_rate):
	terminate(false)
{
	bool ok=true;
	ok &= encoder.set_verify(true);
	ok &= encoder.set_compression_level(5);
	ok &= encoder.set_channels(2);
	ok &= encoder.set_bits_per_sample(24);
	ok &= encoder.set_sample_rate(sample_rate);
	assertm(ok, "failed to set encoder");
	ok &= encoder.init(filename);

	terminate=false;
	thread=std::thread(&FLACWriter::ThreadMe,this);
}
FLACWriter::~FLACWriter()
{
	terminate=true;		//tell the thread to die
	thread.join();		//wait for thread to die
	encoder.finish();	//finish writing file
}
void FLACWriter::WriteMe(char* data, int length)
{
	std::lock_guard<std::mutex> lock(queue_mutex);
	queue.push(std::vector<char>(length)); //create buffer at the back of the queue
	std::memcpy(queue.back().data(),data,length); //and copy into it
}


void FLACWriter::ThreadMe()
{
	for(;;) {
		queue_mutex.lock();
		if(queue.empty()) {
			queue_mutex.unlock();
			if(terminate) return;
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			continue;
		} else {
			std::vector<char> data;
			std::swap(data,queue.front()); //swap data out of the front of the queue to prevent memory copy
			queue.pop(); //pop first item off the queue
			queue_mutex.unlock();

			int32_t* buffer=reinterpret_cast<int32_t*>(data.data()); //let's make flac happy!
			
			for(int i=0; i<data.size()/4; i++)
				buffer[i]=buffer[i]/256;		 //put into flac's buffer, downsample from 32bits to 24bits
				
			encoder.process_interleaved(buffer, data.size()/8);
			continue;
		}
	}
}

