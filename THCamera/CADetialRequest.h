#ifndef   _CADETIALREQUEST
#define   _CADETIALREQUEST

#include "..\..\inc\THFrameDef.h"
//#include "..\..\CommonAPI\inc\THFrameDef.h"
#include "CaInteral.h"



extern
KSTATUS
CaCreateRequest(
PROGRAM_OBJECT                PrgObj,
PCA_EXTENSION                 pExtension,
PINIT_CAMERA_PARAMETERS       pPar
);

extern
KSTATUS
CaSetPrviewRequest(
PCA_EXTENSION                 pExtension,
PCAMERA_PRVIEW_CONFIG         PrviewConfig
);

extern
KSTATUS
CaQueryCameraDevCapRequest(
PCA_EXTENSION                 pExtension,
PDEVICE_CAP_DES               pDevCapDes
);

extern
KSTATUS
CaSetCameraDevCapRequest(
PCA_EXTENSION                 pExtension,
PDEVICE_CAP_DES               pDevCapDes
);


extern
KSTATUS
CaSaveFrameFile(
PCA_EXTENSION                 pExtension, 
PFRAME_FILE_INFO              pFrameFileInfo
);

extern
KSTATUS
CaSaveFrameBitmap(
PCA_EXTENSION                 pExtension, 
PFRAME_FILE_INFO              pFrameFileInfo,
HANDLE						  *pHbitmap
);

extern
KSTATUS
CaReadCameraGrapRawBuffer(
PCA_EXTENSION      pExtension, 
PUSHORT            pDHIndex,
HANDLE            *handle
);

extern
KSTATUS
CaQueryCameraSNInfo(
PCA_EXTENSION  pExtension,
UCHAR          nCaDevIndex,
PCHAR          pSnBuffer,
ULONG          nSnBufferLength
);

extern
KSTATUS
CaCloseCameraRequest(
PCA_EXTENSION      pExtension
);


//老版本接口
extern
KSTATUS
CaSetCameraConfigRequest(
PCA_EXTENSION                 pExtension,
PCHAR                         pCfgContent,
ULONG                         nCfgLength
);

extern
KSTATUS
CaQueryCameraConfigRequest(
PCA_EXTENSION                 pExtension,
PCHAR                         pCfgContent,
ULONG                         nCfgLength
);

extern
KSTATUS
CaQueryCameraDevAblityRequest(
PCA_EXTENSION                 pExtension,
PQUERY_CADEV_ABLITY           pCablity,
PCHAR                         pOutputBuffer,
ULONG                         nLength							  
);

extern
KSTATUS
CaQueryCameraSNInfoOld(
PCA_EXTENSION  pExtension,
UCHAR          nCaDevIndex,
PCHAR          pSnBuffer,
ULONG          nSnBufferLength
);

#endif