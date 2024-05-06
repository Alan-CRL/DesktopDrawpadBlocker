#include "DdbIO.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/locking.h>
#include <share.h>
#include <fcntl.h>

bool IsFileUsed(const char* filePath)
{
	bool ret = false;

	int fh = _sopen(filePath, _O_RDWR, _SH_DENYRW, _S_IREAD | _S_IWRITE);
	if (fh == -1) ret = true;
	else _close(fh);

	return ret;
}