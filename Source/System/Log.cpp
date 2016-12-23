#include "Log.h"
#include <time.h>
#include "System.h"
#include "../Utility/Mutex.h"

static const char* LOG_FILENAME = "log.txt";
static bool LOG_ERROR = false;

void Log::NewLog()
{
	remove(LOG_FILENAME);
}
void Log::Shutdown()
{
	if (LOG_ERROR)
	{
		// save log on error
		FILE* pFileSrc = fopen(LOG_FILENAME, "r");
		if (pFileSrc)
		{
			time_t rawtime;
			struct tm* timeinfo;

			time(&rawtime);
			timeinfo = localtime(&rawtime);
			if (timeinfo)
			{
				char buffer[256];
				sprintf(buffer, "%04d_%02d_%02d__%02d_%02d_%02d_%s", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
					timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, LOG_FILENAME);

				FILE* pFileDst = fopen(buffer, "w");
				if (pFileDst)
				{
					char c;
					while ((c = getc(pFileSrc)) != EOF)
					{
						putc(c, pFileDst);
					}
					fclose(pFileDst);
				}
			}
			fclose(pFileSrc);
		}
	}
}
void Log::Write(const std::string& txt)
{
	static Mutex muLog;

	time_t rawtime;
	struct tm* timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	if (timeinfo == nullptr)
		return;

	muLog.Lock();
	FILE* pFile = fopen(LOG_FILENAME, "a");
	if (pFile)
	{
		fprintf(pFile, "%02d:%02d:%02d: %s\n",
			timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, txt.c_str());
		fclose(pFile);
	}
	muLog.Unlock();
}
void Log::Info(const std::string& i)
{
	FILE* pFile = fopen(LOG_FILENAME, "a");
	if (pFile)
	{
		fprintf(pFile, "\t%s\n", i.c_str());
		fclose(pFile);
	}

}

void Log::Error(const std::exception& e)
{
	LOG_ERROR = true;
	Log::Write(std::string("ERROR: ") + e.what());

	System::MessageBox("Error", e.what(), System::IconError);
}
void Log::Error(const std::string& s)
{
	LOG_ERROR = true;
	Log::Write(std::string("ERROR: ") + s);
	System::MessageBox("Error", s, System::IconError);
}
void Log::Warning(const std::string& txt)
{
	Log::Write("WARNING: " + txt);
}