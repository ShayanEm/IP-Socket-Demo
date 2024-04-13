/**
 * common.cpp
 *
 *  Created on: May 6, 2016
 *	@author Kevin Leong
 */

#include "common.h"

using namespace std;

long long int getFileSize(char* filename) {
	ifstream ifs(filename, ios::in | ios::ate | ios::binary);
	long long int filesize = (long long int) ifs.tellg();
	ifs.close();
	return filesize;
}


