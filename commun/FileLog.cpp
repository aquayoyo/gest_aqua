// les variables statiques:
#include "FileLog.h"
#include <stdio.h>
#include <sys/timeb.h>
#include <time.h>
#include <io.h>

FileLog::FileLog() {
    if ( pthread_mutex_init(&mutexLockfile, NULL)==0)
        ;

    iLogfile = 0;
    m_LogCount = 0;
    m_FileRep[0]=0;
    m_FileName[0]=0;
    m_Niveau = 0;
    m_sFileFlush = 20;
    m_nbjour = 0;
    m_time_write = 0;
}

FileLog::~FileLog()
{
	if (iLogfile != 0)	{
        close(iLogfile);
        iLogfile=0;
	}

	pthread_mutex_destroy(&mutexLockfile);
}

void FileLog::Init(LPCSTR pFileRep, LPCSTR pFileName, int Niveau, short FlushCount)
{
	m_FileName[0]=0;
	strncat(m_FileName, pFileName, sizeof(m_FileName)-1);

	m_Niveau = Niveau;
	m_sFileFlush = FlushCount;
	m_LogCount = 0;

	SetRep(pFileRep);
}

void FileLog::SetRep(LPCSTR pFileRep) {
	m_FileRep[0]=0;
	strncat(m_FileRep, pFileRep, sizeof(m_FileRep)-1);


	if (iLogfile != 0) {
		close (iLogfile);
		iLogfile=0
	}

	// supprime les ancien fichiers log
	DelOldLog();

	char buf[_MAX_PATH];
	strcpy(buf, m_FileRep);
	strcat(buf, m_FileName);
	strcat(buf, ".txt");

	// ouvre le fichier du log pour le premiere fois
	iLogfile = open(buf,O_CREAT|O_APPEND|O_RDWR,
                 S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);

	if (iLogfile!=0) {
       tLastWrite=time(NULL);
	}
}

void FileLog::Log(short Niveau, LPCSTR lpszFormat, ...)
{
	// ecrite dans le fichier du log, le pStr formaté avec les params
	if (Niveau > m_Niveau || iLogfile == 0)
		return;

    pthread_mutex_lock (&mutexLockfile);
	va_list args;
	va_start(args, lpszFormat);

	int nBuf=0;
	int nBuf2;
	char szBuffer[300];

#if 0
	// Test si on doit changer de fichier
	struct tm *ptm_time;
	struct tm tm_time;
	struct tm tm_time_w;
    time_t long_time;

    time( &long_time );                   /* Get time as long integer. */
    ptm_time = localtime( &long_time );    /* Convert to local time. */
	if (ptm_time)
	{
		tm_time = *ptm_time;
		ptm_time = localtime( &m_time_write ); /* Convert to local time. */
		if (ptm_time)
		{
			tm_time_w = *ptm_time;
			if (tm_time.tm_year != tm_time_w.tm_year ||
				tm_time.tm_mon  != tm_time_w.tm_mon  ||
				tm_time.tm_mday != tm_time_w.tm_mday)
			{
				// Close le fichier
				CloseHandle(m_hLogfile);
				m_hLogfile = INVALID_HANDLE_VALUE;

				DelOldLog();

				_snprintf(szBuffer, sizeof(szBuffer),
					"%s%s-%04d%02d%02d.txt",
					m_FileRep,m_FileName,
					tm_time_w.tm_year+1900,tm_time_w.tm_mon+1,tm_time_w.tm_mday);

				char buf[_MAX_PATH];
				strcpy(buf, m_FileRep);
				strcat(buf, m_FileName);
				strcat(buf, ".txt");

				// essaye de renommer l'ancien fichier en XXX_aaaammjj.log
				MoveFile(buf, szBuffer);

				// ouvre le fichier du log pour le premiere fois
				m_hLogfile = CreateFile(buf, GENERIC_WRITE, FILE_SHARE_READ,
					NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
					NULL);
				// memorise la date du nouveau fichier log
				m_time_write = long_time;
			}
		}
	}
#endif
	// prend l'heure actuelle:
	struct _timeb timebuffer;
	char *timeline;

	ftime(&timebuffer);
	timeline = ctime(&(timebuffer.time));

	strdate( szBuffer );
	nBuf = strlen( szBuffer );
	szBuffer[nBuf++] = ' ';
	strtime( &szBuffer[nBuf] );
	nBuf = strlen( szBuffer );
	nBuf2 = snprintf(&szBuffer[nBuf], sizeof(szBuffer) - nBuf,".%03hu ",timebuffer.millitm );

	if (nBuf2 >= 0)
	{
		nBuf += nBuf2;
		nBuf2 = vsnprintf(&szBuffer[nBuf], sizeof(szBuffer) - nBuf, lpszFormat, args);

		// was there an error? was the expanded string too long?
		if (nBuf2 >= 0)
		{
			nBuf += nBuf2;
			szBuffer[nBuf++] = '\r';
			szBuffer[nBuf++] = '\n';
			szBuffer[nBuf] = 0;

			if (iLogfile != 0)
			{
				long lLgWritten=write (iLogfile, szBuffer, nBuf);
				if (lLgWritten>0)
				{
					if (--m_LogCount <0)
					{
						m_LogCount = m_sFileFlush;
						sync(iLogfile);
					}
				}
			}
		}
	}
	va_end(args);
	pthread_mutex_unlock(&mutexLockfile);
}

/*
	efface les anciens fichiers en fonction du nb_jours
*/
void FileLog::DelOldLog()
{
	if (!m_nbjour) return;	// off

	char buf[_MAX_PATH];
	strcpy(buf, m_FileRep);
	strcat(buf, m_FileName);
	strcat(buf, "-*.txt");

	struct tm *ptm_time;
    time_t long_time;
    time( &long_time );

	long_time -= (m_nbjour*24*60*60);

    ptm_time = localtime( &long_time );    /* Convert to local time. */
	if (!ptm_time) return;

	char bufbak[_MAX_PATH];

	_snprintf(bufbak, sizeof(bufbak),
		"%s-%04d%02d%02d.txt",m_FileName,
		ptm_time->tm_year+1900,ptm_time->tm_mon+1,ptm_time->tm_mday);


	struct _finddata_t fileinfo;
	long handle = _findfirst( buf, &fileinfo );
	if (handle >= 0)
	{
		do
		{
			if (strcmp(fileinfo.name,bufbak) <= 0)
			{
				char bufDel[_MAX_PATH];
				strcpy(bufDel, m_FileRep);
				strcat(bufDel, fileinfo.name);
				DeleteFile(bufDel);
			}

		}while(!_findnext( handle, &fileinfo ));
		_findclose(handle);
	}
}

