#pragma once
#include <time.h>

class Test
{
public:
	Test(std::string strTestName) : m_tStartTime(0), m_bRunning(false)
	{
		m_strTestName = strTestName;
	};

	//virtual methods
	virtual void OnInitTest() {};
	virtual void Process() {};

	void RunTest()
	{
		m_tStartTime = time(0);
		m_bRunning = true;
	
		while (1)
		{
 			Process();
			Sleep(1);
		}
	}

	void RunTestOnce()
	{
		m_tStartTime = time(0);
		m_bRunning = true;

		Process();
	}

	size_t GetElapsedTime()
	{
		return time(0) - m_tStartTime;
	}

	std::string GetTestName() { return m_strTestName; }

protected:
	time_t m_tStartTime;
	std::string m_strTestName;
	bool m_bRunning;
};