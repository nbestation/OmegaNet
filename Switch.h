#include "Config.h"

class Switch
{
public:
	Switch()
	{
		blocked_ = false;
		upper_ = NO_CONNECTION;
		lower_ = NO_CONNECTION;
	}
	Switch(int upper, int lower)
	{
		blocked_ = (upper != lower);
		upper_ = upper;
		lower_ = lower;
	}
	Switch(int upper, int lower, bool blocked)
	{
		blocked_ = blocked;
		upper_ = upper;
		lower_ = lower;
	}
	Switch(const Switch& s)
	{
		blocked_ = s.blocked_;
		upper_ = s.upper_;
		lower_ = s.lower_;
	}


	bool blocked_;
	int upper_;
	int lower_;
};