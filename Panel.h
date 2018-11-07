#ifndef __Panel_h__
#define __Panel_h__

class Panel
{
public:
	Panel();
	~Panel();

	virtual void Draw() {}

	bool IsEnabled() const
	{
		return enabled;
	}

	void SetEnabled(bool state)
	{
		enabled = state;
	}
protected:
	bool enabled = true;
};

#endif __Panel_h__