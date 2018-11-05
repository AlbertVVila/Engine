#ifndef __Timer_h__
#define __Timer_h__
class Timer
{
public:
	Timer();
	~Timer();

	void Start();
	int Read();
	int Stop();

	int InitTime;
	int TotalTime;
	bool timerOn;

	void StartPrecise();
	double ReadPrecise();
	double StopPrecise();

	double startPrecise;
	bool timerPreciseOn;
	double TotalPrecise;

};

#endif	__Timer_h__

