
#include "stdafx.h"
#include "bdgps_api.h"
#pragma warning(disable:4996)
// 获得文件大小
size_t get_fileSize(const char* filename)
{
    FILE* pfile = fopen(filename, "rb");
    fseek(pfile, 0, SEEK_END);
    size_t size = ftell(pfile);
    fclose(pfile);
    return size;

}

// 获取gz文件的数据大小
size_t get_gzbinSize(const char* filename)
{
    const int BUFSIZE = 1024 * 1024;
    char* buf = new char[BUFSIZE];
    int data_size = 0;  int cnt = 0;
    gzFile gzf = gzopen(filename, "rb");
	while ((cnt = gzread(gzf, buf, BUFSIZE)) > 0)
	{
		data_size += cnt;
	}
    gzclose(gzf);
    delete[] buf;
    return data_size;

}

