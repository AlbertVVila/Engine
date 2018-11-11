#ifndef __Timer_h__
#define __Timer_h__
class Timer
{
public:
	Timer();
	~Timer();

	void Start();
	int Read() const;
	int Stop();

	int InitTime = 0;
	int TotalTime = 0;
	bool timerOn = false;
};

#endif	//__Timer_h__

