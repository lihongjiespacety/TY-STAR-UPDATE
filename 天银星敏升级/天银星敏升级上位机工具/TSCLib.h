/*----------------------
   TSCLIB.H header file
  ----------------------*/

#ifdef __cplusplus
#define EXPORT_TSC extern "C" __declspec (dllexport)
#else
#define EXPORT_TSC __declspec (dllexport)
#endif

EXPORT_TSC int CALLBACK openport(char*);

EXPORT_TSC int CALLBACK closeport();

EXPORT_TSC int CALLBACK sendcommand (char*);

EXPORT_TSC int CALLBACK setup (char *,char *,char *,char *,char *,char *,char *);

EXPORT_TSC int CALLBACK downloadpcx (char *filename,char *image_name);

EXPORT_TSC int CALLBACK barcode (char *,char *,char *,char *,char *,char *,char *,char *,char *);

EXPORT_TSC int CALLBACK printerfont (char *,char *,char *,char *,char *,char *,char *);

EXPORT_TSC int CALLBACK clearbuffer();

EXPORT_TSC int CALLBACK printlabel(char *, char *);

EXPORT_TSC int CALLBACK formfeed();

EXPORT_TSC int CALLBACK nobackfeed();

EXPORT_TSC BOOL CALLBACK isready();

EXPORT_TSC int CALLBACK windowsfont(int x, int y, int fontheight, int rotation, int fontstyle, int fontunderline, char *szFaceName, char *content);

EXPORT_TSC int CALLBACK about();

int OutBuffer(char *);
int CreatePort(char *);

