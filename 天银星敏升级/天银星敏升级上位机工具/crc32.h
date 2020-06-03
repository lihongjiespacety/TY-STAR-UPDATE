
#ifdef __cplusplus
extern "C" {
#endif

extern UINT crc_tab[256];
UINT crc32(const BYTE * data, UINT length);
BYTE checksum8(const BYTE * data, UINT length);
#ifdef __cplusplus
}
#endif