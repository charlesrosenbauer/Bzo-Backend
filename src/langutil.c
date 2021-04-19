#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "program.h"




int isTypeBID(BuiltinId bid){
	return (bid >= BID_I8) && (bid < BID_MAX_TYPE);
}
