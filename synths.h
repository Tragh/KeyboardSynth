#ifndef SYNTHS_H
#define SYNTHS_H

namespace SYNTHS
{
	
	constexpr float PI2 = 2*M_PI;
	
const std::function<float(float,float)> sine=[](float ticks, float freq)->float {
	return sin(ticks*2*M_PI);
};


const std::function<float(float,float)> square=[](float ticks, float freq)->float {
	int64_t n=(int64_t)(ticks*20000)%20000;
	if(n<10000)
		return -1.0;
	return 1.0;
};


const std::function<float(float,float)> organ=[](float ticks, float freq)->float {
	float sample = 0.5 * sin(2.0*M_PI*ticks);
	sample += 0.25 * sin(2.0*M_PI*ticks*2);
	sample += 0.125 * sin(2.0*M_PI*ticks*4);
	sample += 0.0625 * sin(2.0*M_PI*ticks*8);
	sample += 0.03125 * sin(2.0*M_PI*ticks*16);
	sample += 0.015625 * sin(2.0*M_PI*ticks*32);
	return sample;
};


const std::function<float(float,float)> odd=[](float ticks, float freq)->float {
	int64_t n=(int64_t)(ticks*4643)%4643;
	float ret=(n*2)%4643;
	ret=(ret*2)/4643 - 1;
	return ret;
};


const std::function<float(float,float)> triangle=[](float ticks, float freq)->float {
	int64_t n=(int64_t)(ticks*20000)%20000;
	if(n<10000)
		return (float)(-5000+n)/5000;
	return (float)(15000-n)/5000;
};


const std::function<float(float,float)> organ3=[](float ticks, float freq)->float {
	float sample = 0.5 * triangle(ticks,freq);
	sample += 0.25 * sin(2.0*M_PI*ticks*2);
	sample += 0.125 * square(ticks*4,freq);
	sample += 0.0625 * sin(2.0*M_PI*ticks*8);
	sample += 0.03125 * sin(2.0*M_PI*ticks*16);
	return sample;
};

const std::function<float(float,float)> strange=[](float ticks, float freq)->float {
	return 1.0*sin(sin(ticks*4*M_PI+triangle(ticks+ sin(ticks*M_PI)*2, freq)*2)+ticks*8*M_PI);//+0.7*sin(ticks*8*M_PI);
};


const std::function<float(float,float)> FM1=[](float x, float freq)->float {
	return (sin(x*PI2+2*sin(x*PI2*2))+sin(x*PI2+5*sin(x*PI2/4))/2)/1.5;
};

const std::function<float(float,float)> FM2=[](float x, float freq)->float {
	return (sin(x*PI2+5*sin(x*PI2)+2*sin(x*PI2/2))+sin(x*PI2+5*sin(x*PI2/4))/2)/1.5;
};

const std::function<float(float,float)> FMx=[](float x, float freq)->float {
	return sin(x*PI2+150*tanh(x/freq+0.1)*PI2*(0.95+0.05*sin(x*PI2)));
};

const std::function<float(float,float)> guitar=[](float x, float freq)->float {
	return exp(-x/freq*2)*sin(x*PI2+exp(-x/freq*0.2)*sin(x*PI2/2));
};

const std::function<float(float,float)> guitar2=[](float x, float freq)->float {
	return exp(-x/freq*2)*sin(x*PI2+exp(-x/freq*0.2)*sin(x*PI2/2)+0.5*exp(-x/freq*0.3)*sin(x*PI2/3));
};

constexpr float phi=(1+sqrt(5))/2;
const std::function<float(float,float)> bell=[](float x, float freq)->float {
	return exp(-x/freq*2)*sin(x*PI2+exp(-x/freq*0.2)*sin(x*PI2*phi));
};


const std::function<float(float,float)> test=[](float x, float freq)->float {
	return bell(x,freq);
};

std::map<std::string, std::function<float(float,float)> > Synth=
	{
		{"sine",sine},
		{"square",square},
		{"organ",organ},
		{"odd",odd},
		{"triangle",triangle},
		{"organ3",organ3},
		{"strange",strange},
		{"FM1",FM1},
		{"FM2",FM2},
		{"FMx",FMx},
		{"guitar",guitar},
		{"guitar2",guitar2},
		{"bell",bell},
		{"test",test}
	};

}
#endif

