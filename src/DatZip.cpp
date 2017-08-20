#include <zlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <utime.h>
#include <limits.h>

#include "DatZip.h"
#include "StrUtil.h"

#include "../zlib/unzip.h"
#include "../zlib/ioapi.h"

#define WRITEBUFFERSIZE (8192)
#define ERR_MSG_SIZE	300

/* change_file_date : change the date/time of a file
	filename : the filename of the file where date/time must be modified
	dosdate : the new date at the MSDos format (4 bytes)
	tmu_date : the SAME new date at the tm_unz format */
void change_file_date(const char *filename, uLong dosdate, tm_unz tmu_date)
{
#ifdef _WIN32
  HANDLE hFile;
  FILETIME ftm,ftLocal,ftCreate,ftLastAcc,ftLastWrite;

  hFile = CreateFileA(filename,GENERIC_READ | GENERIC_WRITE,
					  0,NULL,OPEN_EXISTING,0,NULL);
  GetFileTime(hFile,&ftCreate,&ftLastAcc,&ftLastWrite);
  DosDateTimeToFileTime((WORD)(dosdate>>16),(WORD)dosdate,&ftLocal);
  LocalFileTimeToFileTime(&ftLocal,&ftm);
  SetFileTime(hFile,&ftm,&ftLastAcc,&ftm);
  CloseHandle(hFile);
#else
  struct utimbuf ut;
  struct tm newdate;
  newdate.tm_sec = tmu_date.tm_sec;
  newdate.tm_min=tmu_date.tm_min;
  newdate.tm_hour=tmu_date.tm_hour;
  newdate.tm_mday=tmu_date.tm_mday;
  newdate.tm_mon=tmu_date.tm_mon;
  if (tmu_date.tm_year > 1900)
	  newdate.tm_year=tmu_date.tm_year - 1900;
  else
	  newdate.tm_year=tmu_date.tm_year ;
  newdate.tm_isdst=-1;

  ut.actime=ut.modtime=mktime(&newdate);
  utime(filename,&ut);
#endif
}

  int mymkdir(const char* dirname)
{
	int ret=0;
#ifdef _WIN32
	ret = _mkdir(dirname);
#else
	ret = mkdir (dirname,0775);
#endif
	return ret;
}

int makedir (const char *newdir)
{
  char *buffer ;
  char *p;
  int  len = (int)strlen(newdir);

  if (len <= 0)
	return 0;

  buffer = (char*)malloc(len+1);
		if (buffer==NULL)
		{
				printf("Error allocating memory\n");
				return UNZ_INTERNALERROR;
		}
  strcpy(buffer,newdir);

  if (buffer[len-1] == '/') {
	buffer[len-1] = '\0';
  }
  if (mymkdir(buffer) == 0)
	{
	  free(buffer);
	  return 1;
	}

  p = buffer+1;
  while (1)
	{
	  char hold;

	  while(*p && *p != '\\' && *p != '/')
		p++;
	  hold = *p;
	  *p = 0;
	  if ((mymkdir(buffer) == -1) && (errno == ENOENT))
		{
		  printf("couldn't create directory %s\n",buffer);
		  free(buffer);
		  return 0;
		}
	  if (hold == 0)
		break;
	  *p++ = hold;
	}
  free(buffer);
  return 1;
}

QString DatZip::uncompress(const QString& in_file, const QString& to_dir, QString& err)
{
	unzFile uf;
	char err_msg[ERR_MSG_SIZE];
	QString result_dir;

	*err_msg = 0;

	if ((uf = unzOpen64(in_file.toUtf8().constData()))) {
		result_dir = unpackArchive(uf, to_dir.toUtf8().constData(), err_msg);
		unzClose(uf);
	}
	else {
		yv_strcpy(ERR_MSG_SIZE, err_msg, "Cannot open file: ");
		yv_strcat(ERR_MSG_SIZE, err_msg, in_file.toUtf8().constData());
	}

	err = QString(err_msg);
	return result_dir;
}

QString DatZip::unpackArchive(unzFile uf, const char* to_dir, char* err_msg)
{
	uLong				i;
	unz_global_info64	gi;
	int					err;
	QString				result_dir;

	err = unzGetGlobalInfo64(uf, &gi);
	if (err != UNZ_OK) {
		snprintf(err_msg, ERR_MSG_SIZE, "An error occurred while reading .zip file structure: %d", err);
		return QString();
	}

	for (i=0;i<gi.number_entry;i++) {
		if (!extractCurrentFile(uf, to_dir, err_msg, result_dir))
			return QString();

		if ((i+1) < gi.number_entry) {
			err = unzGoToNextFile(uf);
			if (err != UNZ_OK) {
				snprintf(err_msg, ERR_MSG_SIZE, "An error occurred while accessing the .zip file directory: %d", err);
				return QString();
			}
		}
	}

	return result_dir;
}

bool DatZip::extractCurrentFile(unzFile uf, const char* out_dir, char* err_msg, QString& result_dir)
{
	char	zip_file[PATH_MAX];
	char	write_filename[PATH_MAX];
	int		err = UNZ_OK;
	FILE*	fout = NULL;
	void*	buf = NULL;
	uInt	size_buf;

	unz_file_info64 file_info;

	err = unzGetCurrentFileInfo64(uf, &file_info, zip_file, sizeof(zip_file), NULL, 0, NULL, 0);
	if (err!=UNZ_OK) {
		snprintf(err_msg, ERR_MSG_SIZE, "An error occurred while extracting a file from the .zip archive: %d", err);
		return false;
	}

	// Change to UNIX path:
	yv_replace_all(zip_file, '\\', '/');

	bool is_dir = yv_ends_with(zip_file, "/");
	bool is_absolute = yv_starts_with(zip_file, "/");

	size_buf = WRITEBUFFERSIZE;
	if (!(buf = (void*) malloc(size_buf))) {
		snprintf(err_msg, ERR_MSG_SIZE, "Out of memory");
		return false;
	}

	if (strchr(zip_file, '/') && result_dir.isEmpty()) {
		yv_nth_component_of_path(1, zip_file, (char*) buf, size_buf);
		result_dir = QString((char*) buf);
	}

	yv_strcpy(PATH_MAX, write_filename, out_dir);
	yv_add_dir_char(PATH_MAX, write_filename);
	yv_strcat(PATH_MAX, write_filename, is_absolute ? zip_file+1 : zip_file);

	if (is_dir)
		mymkdir(write_filename);
	else {
		err = unzOpenCurrentFilePassword(uf, NULL);
		if (err != UNZ_OK) {
			snprintf(err_msg, ERR_MSG_SIZE, "An error occurred while extracting a file from the .zip archive: %d", err);
			goto failed;
		}

		/* some zipfile don't contain directory alone before file */
		if (!(fout = fopen64(write_filename, "wb"))) {
			char new_dir[PATH_MAX];

			strcpy(new_dir, write_filename);
			yv_remove_last_name_of_path(new_dir);
			makedir(new_dir);
			fout = fopen64(write_filename, "wb");
		}

		if (fout == NULL) {
			snprintf(err_msg, ERR_MSG_SIZE, "An error occurred while opening: %s", zip_file);
			goto failed_2;
		}

		for (;;) {
			err = unzReadCurrentFile(uf, buf, size_buf);
			if (err < 0) {
				snprintf(err_msg, ERR_MSG_SIZE, "An error occurred while readind data from the .zip archive: %d", err);
				goto failed_3;
			}

			if (err == 0)
				break;

			if (fwrite(buf, err, 1, fout) != 1) {
				snprintf(err_msg, ERR_MSG_SIZE, "An error occurred while writing: %s", zip_file);
				goto failed_3;
			}
		}
		fclose(fout);

		change_file_date(write_filename, file_info.dosDate, file_info.tmu_date);

		err = unzCloseCurrentFile(uf);
		if (err != UNZ_OK) {
			snprintf(err_msg, ERR_MSG_SIZE, "An error occurred while closing a file from the .zip archive: %d", err);
			goto failed;
		}
	}

	free(buf);
	return true;

	failed_3:
	fclose(fout);

	failed_2:
	unzCloseCurrentFile(uf); /* don't lose the error */

	failed:
	if (buf)
		free(buf);
	return false;
}

