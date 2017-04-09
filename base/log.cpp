#include "log.h"

#include <iostream>
#include <string>

//unsigned int Logger::count = 0;
Logger* Logger::m_instance = new Logger();


Logger::Logger():m_logEnable(false),m_pFileStream(NULL),m_is_async(false)
{
	pthread_mutex_init(&writeMutex, NULL);
}

/*Logger::Logger(const std::string &strLogPath, EnumLogLevel nLogLevel):m_logEnable(false),
																			m_pFileStream(NULL),
																			m_is_async(false)
{
	pthread_mutex_init(&writeMutex, NULL);
	init(strLogPath, nLogLevel);
}*/

Logger::~Logger(void)
{
	if(m_pFileStream){
		fclose(m_pFileStream);
		m_pFileStream = NULL;
	}	
	pthread_mutex_destroy(&writeMutex);	
}

void Logger::init(std::string strLogPath, EnumLogLevel nLogLevel)
{
	m_logEnable = true;
	m_strCurLogName = strLogPath;
	m_nLogLevel = nLogLevel;
}


//私有析构函数，类外程序无法直接delete
//delete 后，m_instance 指向无效地址
void Logger::Destroy(void)
{
	delete this;
}

Logger* Logger::GetInstance()
{
	return m_instance;
}

bool Logger::Open(void)
{
	if(m_pFileStream){
		Close();
		m_pFileStream = NULL;
	}
	m_pFileStream = fopen(m_strCurLogName.c_str(), "a");
	return m_pFileStream ==NULL? false : true;	
}


void Logger::Close(void)
{
	fclose(m_pFileStream);
	m_pFileStream = NULL;
}

void Logger::startBg()
{
	m_is_async = true;
	assert(pthread_create(&bgtid, NULL, Logger::flushLogThread, this) == 0);

}

void * Logger::flushLogThread(void * pParam)
{
	Logger *pThis = (Logger *)pParam;
	std::string logStr;
	while(true){
		logStr = pThis->m_logQueue.take();
		pThis->LogFlush(logStr);
	}
}

bool Logger::LogFlush(std::string& logs)
{
	if(!m_logEnable){
		std::cout << "log is disable" << std::endl;
		return false;
	}

	if(m_pFileStream == NULL){
		if(Open() == false){
			return false;
		}
	}

	/*if (++count >= MAX_LOG_LINE_NUM)
	{
		count = 0;
		Close();
		unlink(m_strCurLogName);
		if(Open() == false){
			return false;
		}
	}*/
	fprintf(m_pFileStream, "%s\n", logs.c_str());
	fflush(m_pFileStream);
	return true;
}

bool Logger::LogWrite(EnumLogLevel nLogLevel, unsigned char *buf, unsigned short int len, const char *pcFormat, ...)
{
	unsigned char logBuf[LOG_STR_BUF_LEN] = {0};
	va_list tArg;
	char str[LOG_STR_BUF_LEN] = {0};

	va_start (tArg, pcFormat);
	vsnprintf (str,sizeof(str),pcFormat, tArg);
	va_end (tArg);

	strcat(str, ": %s");

	unsigned char tmp[5];
	for(unsigned short int i=0; i<len; i++){
		sprintf((char *)tmp, "%x ", buf[i]);
		strcat((char *)logBuf, (const char *)tmp);
	}	
	return LogWrite(nLogLevel, str, logBuf);
}

bool Logger::LogWrite(EnumLogLevel nLogLevel, const char *pcFormat, ...)
{
	char log_str[LOG_STR_BUF_LEN];
	va_list tArg;
	//int iNum;
	char str[LOG_STR_BUF_LEN];
	int need_log_str = 0;

	LogCurrentTime();	
	va_start (tArg, pcFormat);
	//iNum = vsnprintf (str,sizeof(str),pcFormat, tArg);
	vsnprintf (str,sizeof(str),pcFormat, tArg);
	va_end (tArg);	
	switch (nLogLevel)
	{
		case LOG_LEVEL_DEBUG:
			if (m_nLogLevel <= LOG_LEVEL_DEBUG)
			{
				need_log_str = 1;
				snprintf(log_str, LOG_STR_BUF_LEN, "[DEBUG: %s] %s", cur_ts_str, str);
			}
			break;
		case LOG_LEVEL_INFO:
			if (m_nLogLevel <= LOG_LEVEL_INFO)
			{
				need_log_str = 1;
				snprintf(log_str, LOG_STR_BUF_LEN, "[INFO: %s] %s", cur_ts_str, str);
			}
			break;
		case LOG_LEVEL_WARNING:
			if (m_nLogLevel <= LOG_LEVEL_WARNING)
			{
				need_log_str = 1;
				snprintf(log_str, LOG_STR_BUF_LEN, "[WARNING: %s] %s", cur_ts_str, str);
			}
			break;
		case LOG_LEVEL_ERROR:
			if (m_nLogLevel <= LOG_LEVEL_ERROR)
			{
				need_log_str = 1;
				snprintf(log_str, LOG_STR_BUF_LEN, "[ERROR: %s] %s", cur_ts_str, str);
			}
			break;
		case LOG_LEVEL_FATAL:
			if (m_nLogLevel <= LOG_LEVEL_FATAL)
			{
				need_log_str = 1;
				snprintf(log_str, LOG_STR_BUF_LEN, "[FATAL: %s] %s", cur_ts_str, str);
			}
			break;
		case LOG_LEVEL_INDISPENSABLE:
			need_log_str = 1;
			snprintf(log_str, LOG_STR_BUF_LEN, "[%s] %s", cur_ts_str, str);
			break;
	}

	std::string logStr = log_str;
	if (need_log_str)
	{
		if(m_is_async){
			m_logQueue.put(logStr);
			
		}else{
			pthread_mutex_lock(&writeMutex);
			LogFlush(logStr);
			pthread_mutex_unlock(&writeMutex);
		}
	}	
	return true;
}

/*
bool Logger::LogWrite(EnumLogLevel nLogLevel, const char *pcFormat, ...)
{
	char log_str[LOG_STR_BUF_LEN];
	va_list tArg;
	//int iNum;
	char str[LOG_STR_BUF_LEN];
	int need_log_str = 0;

	pthread_mutex_lock(&writeMutex);
	
	if(m_pFileStream == NULL){
		if(Open() == false){
			return false;
		}
	}
	LogCurrentTime();	
	if (++count >= MAX_LOG_LINE_NUM)
	{
		count = 0;
		Close();
		unlink(m_strCurLogName);
		if(Open() == false){
			return false;
		}
	}

	va_start (tArg, pcFormat);
	//iNum = vsnprintf (str,sizeof(str),pcFormat, tArg);
	vsnprintf (str,sizeof(str),pcFormat, tArg);
	va_end (tArg);	

	switch (nLogLevel)
	{
		case LOG_LEVEL_INFO:
			if (nLogLevel <= LOG_LEVEL_INFO)
			{
				need_log_str = 1;
				snprintf(log_str, LOG_STR_BUF_LEN, "[INFO: %s] %s", cur_ts_str, str);
			}
			break;
		case LOG_LEVEL_WARNING:
			if (nLogLevel <= LOG_LEVEL_WARNING)
			{
				need_log_str = 1;
				snprintf(log_str, LOG_STR_BUF_LEN, "[WARNING: %s] %s", cur_ts_str, str);
			}
			break;
		case LOG_LEVEL_ERROR:
			if (nLogLevel <= LOG_LEVEL_ERROR)
			{
				need_log_str = 1;
				snprintf(log_str, LOG_STR_BUF_LEN, "[ERROR: %s] %s", cur_ts_str, str);
			}
			break;
		case LOG_LEVEL_FATAL:
			if (nLogLevel <= LOG_LEVEL_FATAL)
			{
				need_log_str = 1;
				snprintf(log_str, LOG_STR_BUF_LEN, "[FATAL: %s] %s", cur_ts_str, str);
			}
			break;
		case LOG_LEVEL_INDISPENSABLE:
			need_log_str = 1;
			snprintf(log_str, LOG_STR_BUF_LEN, "[%s] %s", cur_ts_str, str);
			break;
	}

	if (need_log_str)
	{
		if(AT_LOG){
			fprintf(m_pFileStream, "%s\n", log_str);
			fflush(m_pFileStream);
		}
		else{
			printf("%s\n", log_str);
			fflush(stdout);
		}		
	}	
	pthread_mutex_unlock(&writeMutex);

	return true;

}*/

void Logger::LogCurrentTime(void)
{
	struct tm *time_stamp;
	time_t cur_time;

	cur_time = time(NULL);
	time_stamp = localtime(&cur_time);

	snprintf(cur_ts_str, TIME_STAMP_BUF_LEN, "%02d/%02d/%02d %02d:%02d:%02d", time_stamp->tm_mday,
			time_stamp->tm_mon + 1, time_stamp->tm_year % 100, time_stamp->tm_hour, time_stamp->tm_min, time_stamp->tm_sec);
	return;

}

Logger* getlog()
{
	return Logger::GetInstance();
}

