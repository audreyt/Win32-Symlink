/*
 * Source: http://www.mail-archive.com/pgsql-committers@postgresql.org/msg03912.html
 */

#include <windows.h>
#include <winioctl.h>
#include <stdio.h>
 
/*
 *	pgsymlink support:
 *
 *	This struct is a replacement for REPARSE_DATA_BUFFER which is defined in VC6 winnt.h
 *	but omitted in later SDK functions.
 *	We only need the SymbolicLinkReparseBuffer part of the original struct's union.
 */
typedef struct
{
    DWORD  ReparseTag;
    WORD   ReparseDataLength;
    WORD   Reserved;
    /* SymbolicLinkReparseBuffer */
        WORD   SubstituteNameOffset;
        WORD   SubstituteNameLength;
        WORD   PrintNameOffset;
        WORD   PrintNameLength;
        WCHAR PathBuffer[1];
}
REPARSE_JUNCTION_DATA_BUFFER;

#define REPARSE_JUNCTION_DATA_BUFFER_HEADER_SIZE   \
		FIELD_OFFSET(REPARSE_JUNCTION_DATA_BUFFER, SubstituteNameOffset)

/*
 *	pgsymlink - uses Win32 junction points
 *
 *	For reference:	http://www.codeproject.com/w2k/junctionpoints.asp
 */
int
pgsymlink(const char *oldpath, const char *newpath)
{
	HANDLE dirhandle;
	DWORD len;
	char nativeTarget[MAX_PATH];
	char *p = nativeTarget;
	char buffer[MAX_PATH*sizeof(WCHAR) + sizeof(REPARSE_JUNCTION_DATA_BUFFER)];
	REPARSE_JUNCTION_DATA_BUFFER *reparseBuf = (REPARSE_JUNCTION_DATA_BUFFER*)buffer;
    
	CreateDirectory(newpath, 0);
	dirhandle = CreateFile(newpath, GENERIC_READ | GENERIC_WRITE, 
			0, 0, OPEN_EXISTING, 
			FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS, 0);
    
	if (dirhandle == INVALID_HANDLE_VALUE)
		return -1;
    
	/* make sure we have an unparsed native win32 path */
	if (memcmp("\\??\\", oldpath, 4))
		sprintf(nativeTarget, "\\??\\%s", oldpath);
	else
		strcpy(nativeTarget, oldpath);
    
	while ((p = strchr(p, '/')) != 0)
		*p++ = '\\';

	len = strlen(nativeTarget) * sizeof(WCHAR);
	reparseBuf->ReparseTag = IO_REPARSE_TAG_MOUNT_POINT;
	reparseBuf->ReparseDataLength = (unsigned short)(len + 12);
	reparseBuf->Reserved = 0;
	reparseBuf->SubstituteNameOffset = 0;
	reparseBuf->SubstituteNameLength = (unsigned short)(len);
	reparseBuf->PrintNameOffset = (unsigned short)(len+sizeof(WCHAR));
	reparseBuf->PrintNameLength = 0;
	MultiByteToWideChar(CP_ACP, 0, nativeTarget, -1,
						reparseBuf->PathBuffer, MAX_PATH);
    
	/*
	 * FSCTL_SET_REPARSE_POINT is coded differently depending on SDK version;
	 * we use our own definition
	 */
	if (!DeviceIoControl(dirhandle, 
		CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 41, METHOD_BUFFERED, FILE_ANY_ACCESS),
		reparseBuf, 
		reparseBuf->ReparseDataLength + REPARSE_JUNCTION_DATA_BUFFER_HEADER_SIZE,
		0, 0, &len, 0))
	{
		LPSTR msg;

		errno=0;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
					  NULL, GetLastError(), 
					  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					  (LPSTR)&msg, 0, NULL );
		Perl_warn(aTHX_ "Error setting junction for %s: %s", nativeTarget, msg);
	    
		LocalFree(msg);
	    
		CloseHandle(dirhandle);
		RemoveDirectory(newpath);
		return -1;
	}

	CloseHandle(dirhandle);

	return 0;
}
