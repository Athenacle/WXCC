#ifndef EXCEPTION_H
#define EXCEPTION_H

class Exception
{
protected:
	int exType;
	const char* ex;
	enum 
	{
		RIGHT = 0,
		WARNING = 1,
		ERROR = 2
	};

	
public:
	virtual const char* toString(void) const
	{
		return ex;
	}
	virtual operator bool() const
	{
		return exType == 0;
	}
	virtual bool no_exception() const
	{
		return this->operator bool();
	}
	Exception(int tp = 0, const char* ps = "no exception.")
		:exType(tp), ex(ps)
	{}

	virtual void setException(int tp, const char* st)
	{
		exType = tp;
		ex = st;
	}

	void setError(const char *errMsg)
	{
		this->ex = errMsg;
		this->exType = ERROR;
	}

};

class ExprExcetion : public Exception
{
public:
	static char *esprintf(const char *format, ...);
};

#endif