#ifndef _LOG_H_
#define _LOG_H_

#include "Head.h"

class Log {
public:
	FILE* file;

	Log() {
		file = NULL;
	}

	/*
	* open the logging file
	*/
	void OpenFile() {
		file = fopen(LOG_FILE.c_str(), "w");
	}

	~Log() {
		if (!file) {
			fclose(file);
		}
	}

	void WriteString(const char content[]) {
		fprintf(file,"%s",content);
	}

	void WriteInt(const int content) {
		fprintf(file, "%d", content);
	}

	void WriteDouble(const double content) {
		fprintf(file, "%f", content);
	}

	void WriteChar(const char content) {
		fprintf(file, "%c", content);
	}
};

Log *gLog = new Log();

#endif