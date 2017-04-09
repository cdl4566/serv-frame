#ifndef __BL_LOG_H__
#define __BL_LOG_H__

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <pthread.h>
#include <signal.h>
#include <sys/param.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>	
#include <stdarg.h>
#include <pthread.h>
#include <string>

#include "BlockingQueue.h"

//#define __DEBUG

 
#define MAX_LOG_NAME_LEN  128
#define LOG_STR_BUF_LEN   1024
#define TIME_STAMP_BUF_LEN  128
#define MAX_LOG_LINE_NUM 2000

#define DEFAULT_LOG_PATH    "/var/log/log.log"

/*#ifdef __DEBUG
	#define  AT_LOG     0
#else
	#define  AT_LOG     1 
#endif
*/

typedef enum 
{
	LOG_LEVEL_DEBUG	= 0,
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_FATAL,
	LOG_LEVEL_INDISPENSABLE
	
}EnumLogLevel;

#define DEFAULT_LOG_LEVEL  LOG_LEVEL_INFO 

class Logger{

	private:
		Logger();
		//Logger(const std::string &strLogPath, EnumLogLevel nLogLevel);
		virtual ~Logger();

	public:
		bool LogWrite(EnumLogLevel nLogLevel, const char *pcFormat, ...);
		bool LogWrite(EnumLogLevel nLogLevel, unsigned char *buf, unsigned short int len, const char *pcFormat, ...);
		//void SetLogLevel(EnumLogLevel nLogLevel);		
		void Destroy();
		void startBg();
		static Logger* GetInstance();
		//std::string &strLogPath=DEFAULT_LOG_PATH ??
		//in definition, do not set default value again
		void init(std::string strLogPath=DEFAULT_LOG_PATH, EnumLogLevel nLogLevel=DEFAULT_LOG_LEVEL);

	private:
		bool Open(void);
		void Close(void);
		void LogCurrentTime(void);
		static void *flushLogThread(void * pParam);
		bool LogFlush(std::string& logs);

	private:
		bool m_logEnable;
		FILE * m_pFileStream;
		EnumLogLevel m_nLogLevel;	
		//char m_strCurLogName[MAX_LOG_NAME_LEN];
		std::string m_strCurLogName;
		pthread_mutex_t writeMutex;
		//static unsigned int count;
		char cur_ts_str[TIME_STAMP_BUF_LEN];
		static Logger* m_instance;
		bool m_is_async;
		BlockingQueue<std::string> m_logQueue;
		pthread_t bgtid;

};

//format: "time object,event,fd:desc:data"

Logger* getlog();

#endif