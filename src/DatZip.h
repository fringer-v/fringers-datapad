#ifndef __DatZip_h__
#define __DatZip_h__

#include "QString"
#include "../zlib/unzip.h"

class DatZip
{
public:
	static QString uncompress(const QString& in_file, const QString& to_dir, QString& err_msg);

private:
	static QString unpackArchive(unzFile uf, const char* to_dir, char* err_msg);
	static bool extractCurrentFile(unzFile uf, const char* out_dir, char* err_msg, QString& result_dir);
};

#endif // __DatZip_h__
