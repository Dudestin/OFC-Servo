#include "str_util.h"

char* strslice(char* src, size_t p0, size_t p1){
	char* dst;
	if (p1 - p0 < 0) return dst;
	dst = (char*)malloc((p1-p0)+1);
	size_t src_pos;
	size_t dst_pos = 0;
	for (src_pos = p0; src_pos <= p1; ++src_pos)
		dst[dst_pos++] = src[src_pos];
	return dst;
}

char* strtrim(const char* src, size_t strlen){
	char dst_tmp[strlen];
	size_t src_pos;
	size_t dst_pos = 0;
	for  (src_pos = 0; src_pos < strlen; src_pos++) {
		if (src[src_pos] == 0x20){
			NULL;
		}else if (src[src_pos] == 0x00) {
			dst_tmp[dst_pos++] = src[src_pos];
			break;
		}
		else  
			dst_tmp[dst_pos++] = src[src_pos];
	}
	char* dst = (char*)malloc(dst_pos);
	memcpy(dst, dst_tmp, dst_pos);
	return dst;
}
