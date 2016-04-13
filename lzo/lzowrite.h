#include <stdio.h>
#include <stdint.h>
#include "minilzo.h"

#define LZOWRITE_BUFFER_SIZE 32 * 1024
#define LZOWRITE_OUT_BUFFER_SIZE (LZOWRITE_BUFFER_SIZE + LZOWRITE_BUFFER_SIZE / 16 + 64 + 3)
#define LZOWRITE_LZO_MAGIC {0x89,0x4c,0x5a,0x4f,0x00,0x0d,0x0a,0x1a,0x0a}
#define LZOWRITE_LZO_MAGIC_LEN 9
#define LZOWRITE_LZO_VERSION 0x3010 // as in LZOP 1.03
#define LZOWRITE_LZO_LIB_VERSION (lzo_version() & 0xffff)
#define LZOWRITE_LZO_VERSION_NEEDED_TO_EXTRACT 0x4009 // not using filters, otherwise 0x0950
#define LZOWRITE_LZO_METHOD 1 // LZO1X
#define LZOWRITE_LZO_COMPRESSION_LEVEL 1 // with lzo, we have compression level = 1.
#define LZOWRITE_LZO_FLAGS 0 // no checksums on data!!
#define LZOWRITE_LZO_MODE 0xa481 // 100644 oct

void fwrite_int32_be(void* ptr, FILE* out);


struct lzowrite_buffer {
	unsigned char buffer[LZOWRITE_BUFFER_SIZE];
	unsigned char out_buffer[LZOWRITE_OUT_BUFFER_SIZE];
	uint32_t length;
	uint32_t out_length;
	FILE* output;
	lzo_align_t* workmemory;
};

struct lzowrite_file_header {
	uint16_t version;
	uint16_t library_version;
	uint16_t needed_version;
	uint8_t compression_method;
	uint8_t compression_level;
	uint32_t compression_flags;
	uint32_t mode;
	uint32_t file_mtime_low;
	uint32_t file_mtime_high;
	uint8_t file_name_length;
	uint32_t file_header_checksum;
};

struct lzowrite_block_header {
	uint32_t uncompressed_size;
	uint32_t compressed_size;
	uint32_t uncompressed_adler32;
	uint32_t uncompressed_crc32;
	uint32_t compressed_adler32;
	uint32_t compressed_crc32;
};
void lzowrite32(struct lzowrite_buffer* lzowrite_buffer, uint32_t data);
void lzowrite16(struct lzowrite_buffer* lzowrite_buffer, uint16_t data);
void lzowrite_wbuf(struct lzowrite_buffer* lzowrite_buffer);
int lzowrite_init(struct lzowrite_buffer * buffer, const char* filename);
void lzowrite(struct lzowrite_buffer* lzowrite_buffer, void* src, size_t len);
void lzowrite_free(struct lzowrite_buffer* lzowrite_buffer);
