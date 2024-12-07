#ifndef __AVASPECX64__H__
#define __AVASPECX64__H__

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

#include <stdint.h>
#include <windows.h>
/**********************************************/
/*******************_MARCO_********************/
/**********************************************/
#define AVS_ENABLE 1
#define AVS_DISABLE 0

#define ERR_SUCCESS 0
#define ERR_INVALID_PARAMETER -1
#define ERR_OPERATION_NOT_SUPPORTED -2
#define ERR_DEVICE_NOT_FOUND -3
#define ERR_OPERATION_PENDING -5
#define ERR_TIMEOUT -6
#define ERR_INVALID_MEAS_DATA -8
#define ERR_INVALID_SIZE -9
#define ERR_INVALID_PIXEL_RANGE -10
#define ERR_INVALID_INT_TIME -11
#define ERR_INVALID_COMBINATION -12
#define ERR_NO_MEAS_BUFFER_AVAIL -14
#define ERR_UNKNOWN -15
#define ERR_COMMUNICATION -16
#define ERR_NO_SPECTRA_IN_RAM -17
#define ERR_INVALID_DLL_VERSION -18
#define ERR_NO_MEMORY -19
#define ERR_DLL_INITIALISATION -20
#define ERR_INVALID_STATE -21
#define ERR_INVALID_REPLY -22
#define ERR_ACCESS -24
#define ERR_INTERNAL_READ -25
#define ERR_INTERNAL_WRITE -26
#define ERR_ETHCONN_REUSE -27
#define ERR_INVALID_DEVICE_TYPE -28
#define ERR_SECURE_CFG_NOT_READ -29
#define ERR_UNEXPECTED_MEAS_RESPONSE -30
#define ERR_INVALID_PARAMETER_NR_PIXEL -100
#define ERR_INVALID_PARAMETER_ADC_GAIN -101
#define ERR_INVALID_PARAMETER_ADC_OFFSET -102
#define ERR_INVALID_MEASPARAM_AVG_SAT2 -110
#define ERR_INVALID_MEASPARAM_AVG_RAM -111
#define ERR_INVALID_MEASPARAM_SYNC_RAM -112
#define ERR_INVALID_MEASPARAM_LEVEL_RAM -113
#define ERR_INVALID_MEASPARAM_SAT2_RAM -114
#define ERR_INVALID_MEASPARAM_FWVER_RAM -115
#define ERR_INVALID_MEASPARAM_DYNDARK -116
#define ERR_NOT_SUPPORTED_BY_SENSOR_TYPE -120
#define ERR_NOT_SUPPORTED_BY_FW_VER -121
#define ERR_NOT_SUPPORTED_BY_FPGA_VER -122
#define ERR_SL_CALIBRATION_NOT_AVAILABLE -140
#define ERR_SL_STARTPIXEL_NOT_IN_RANGE -141
#define ERR_SL_ENDPIXEL_NOT_IN_RANGE -142
#define ERR_SL_STARTPIX_GT_ENDPIX -143
#define ERR_SL_MFACTOR_OUT_OF_RANGE -144    

/**********************************************/
/********************Typing********************/
/**********************************************/
typedef int AvsHandle;
typedef unsigned char SensorType;
typedef uint32_t uint32;
typedef int32_t int32;
typedef uint8_t uint8;
typedef uint16_t uint16;

/**********************************************/
/********************Typing********************/
/**********************************************/
typedef enum {
    UNKNOWN,
    USB_AVAILABLE,
    USB_IN_USE_BY_APPLICATION,
    USB_IN_USE_BY_OTHER,
    ETH_AVAILABLE,
    ETH_IN_USE_BY_APPLICATION,
    ETH_IN_USE_BY_OTHER,
    ETH_ALREADY_IN_USE_USB
} DeviceStatus;

typedef enum {
    TYPE_UNKNOWN,
    TYPE_AS5216,
    TYPE_ASMINI,
    TYPE_AS7010,
    TYPE_AS7007
} AvsDeviceType;

typedef enum {
    RS232,
    USB5216,
    USBMINI,
    USB7010,
    ETH7010,
    USB7007
} InterfaceType;

typedef struct {
    char SerialNumber[10];
    char UserFriendlyName[64];
    DeviceStatus Status;
} AvsIdentityType;

typedef struct {
    unsigned char InterfaceType;
    unsigned char serial[10];
    unsigned short port;
    unsigned char status;
    unsigned int RemoteHostIp;
    unsigned int LocalIp;
    unsigned char reserved[4];
} BroadcastAnswerType;

typedef struct {
    unsigned short m_StrobeControl;
    unsigned int m_LaserDelay;
    unsigned int m_LaserWidth;
    float m_LaserWaveLength;
    unsigned short m_StoreToRam;
} ControlSettingsType;

typedef struct {
    unsigned char m_Enable;
    unsigned char m_ForgetPercentage;
} DarkCorrectionType;

typedef struct {
    int32 m_Nmsr;
    uint8 m_Reserved[8];
} DynamicStorageType;

typedef struct {
    uint32 m_TotalScans;
    uint32 m_UsedScans;
    uint32 m_Flags;
    unsigned char m_IsStopEvent;
    unsigned char m_IsOverflowEvent;
    unsigned char m_IsInternalErrorEvent;
    unsigned char m_Reserved;
} DstrStatusType;

typedef struct {
    unsigned int m_IpAddr;
    unsigned int m_NetMask;
    unsigned int m_Gateway;
    unsigned char m_DhcpEnabled;
    unsigned short m_TcpPort;
    unsigned char m_LinkStatus;
    unsigned char m_ClientIdType;
    char m_ClientIdCustom[32];
    unsigned short m_MeasurementDataPortKey;
    unsigned short m_MeasurementDataPort;
    unsigned char m_Reserved[75];
} EthernetSettingsType;

typedef struct {
    unsigned char data[4096];
} OemDataType;

typedef struct {
    float m_AnalogLow[2];
    float m_AnalogHigh[2];
    float m_DigitalLow[10];
    float m_DigitalHigh[10];
} ProcessControlType;

typedef struct {
    unsigned short m_SmoothPix;
    unsigned char m_SmoothModel;
} SmoothingType;

typedef struct {
    SmoothingType m_Smoothing;
    float m_CalInttime;
    float m_aCalibConvers[4096];
} SpectrumCalibrationType;

typedef struct {
    float m_aSpectrumCorrect[4096];
} SpectrumCorrectionType;

typedef struct {
    bool m_Enable;
    float m_Setpoint;
    float m_aFit[2];
} TecControlType;

typedef struct {
    float m_aFit[5];
} TempSensorType;

typedef struct {
    unsigned short m_Date;
    unsigned short m_Time;
} TimeStampType;

typedef struct {
    unsigned char m_Mode;
    unsigned char m_Source;
    unsigned char m_SourceType;
} TriggerType;

typedef struct {
    SpectrumCalibrationType m_IntensityCalib;
    unsigned char m_CalibrationType;
    unsigned int m_FiberDiameter;
} IrradianceType;

typedef struct {
    SensorType m_SensorType;
    unsigned short m_NrPixels;
    float m_aFit[5];
    bool m_NLEnable;
    double m_aNLCorrect[8];
    double m_aLowNLCounts;
    double m_aHighNLCounts;
    float m_Gain[2];
    float m_Reserved;
    float m_Offset[2];
    float m_ExtOffset;
    unsigned short m_DefectivePixels[30];
} DetectorType;

typedef struct {
    unsigned short m_StartPixel;
    unsigned short m_StopPixel;
    float m_IntegrationTime;
    unsigned int m_IntegrationDelay;
    unsigned int m_NrAverages;
    DarkCorrectionType m_CorDynDark;
    SmoothingType m_Smoothing;
    unsigned char m_SaturationDetection;
    TriggerType m_Trigger;
    ControlSettingsType m_Control;
} MeasConfigType;

typedef struct {
    bool m_Enable;
    MeasConfigType m_Meas;
    signed short m_Nmsr;
} StandaloneType;

typedef struct {
    unsigned short m_Len;
    unsigned short m_ConfigVersion;
    char m_aUserFriendlyId[64];
    DetectorType m_Detector;
    IrradianceType m_Irradiance;
    SpectrumCalibrationType m_Reflectance;
    SpectrumCorrectionType m_SpectrumCorrect;
    StandaloneType m_StandAlone;
    DynamicStorageType m_DynamicStorage;
    TempSensorType m_Temperature[3];
    TecControlType m_TecControl;
    ProcessControlType m_ProcessControl;
    EthernetSettingsType m_EthernetSettings;
    bool m_MessageAckDisable;
    bool m_IncludeCRC;
    unsigned char m_aReserved[9606];
    OemDataType m_OemData;
} DeviceConfigType;

/**********************************************/
/*******************Function*******************/
/**********************************************/

#ifdef _MSC_VER
    #ifndef EXPORTS
        #define AVS_SPECX64_API __declspec(dllexport)
    #else
        #define AVS_SPECX64_API __declspec(dllimport)
    #endif
#else
    #define AVS_SPECX64_API
#endif

AVS_SPECX64_API int __stdcall AVS_Init(short a_Port);
AVS_SPECX64_API int __stdcall AVS_Done(void);
AVS_SPECX64_API int __stdcall AVS_UpdateUSBDevices(void);
AVS_SPECX64_API int __stdcall AVS_UpdateETHDevices(unsigned int a_ListSize, unsigned int* a_pRequiredSize,BroadcastAnswerType* a_pList);
AVS_SPECX64_API int __stdcall AVS_GetList(unsigned int a_ListSize, unsigned int* a_pRequiredSize,AvsIdentityType* a_pList);
AVS_SPECX64_API AvsHandle __stdcall AVS_GetHandleFromSerial(char* a_pSerial);
AVS_SPECX64_API AvsHandle __stdcall AVS_Activate(AvsIdentityType* a_pDeviceId);
AVS_SPECX64_API AvsHandle __stdcall AVS_ActivateConnCb(AvsIdentityType* a_pDeviceIdvoid,void (*__Conn)(AvsHandle*, int));
AVS_SPECX64_API AvsHandle __stdcall AVS_ActivateConn(AvsIdentityType* a_pDeviceId, void* a_hWnd);
AVS_SPECX64_API bool __stdcall AVS_Deactivate(AvsHandle a_hDeviceId);
AVS_SPECX64_API bool __stdcall AVS_Register(HWND a_hWnd);
AVS_SPECX64_API int __stdcall AVS_PrepareMeasure(AvsHandle a_hDevice, MeasConfigType* a_pMeasConfig);
AVS_SPECX64_API int __stdcall AVS_Measure(AvsHandle a_hDevice, HWND a_hWnd, short a_Nmsr);
AVS_SPECX64_API int __stdcall AVS_MeasureCallback(AvsHandle a_hDevice, void (*__Done)(AvsHandle*, int*),short a_Nmsr);
AVS_SPECX64_API int __stdcall AVS_GetLambda(AvsHandle a_hDevice, double* a_pWavelength);
AVS_SPECX64_API int __stdcall AVS_GetNumPixels(AvsHandle a_hDevice, unsigned short* a_pNumPixels);
AVS_SPECX64_API int __stdcall AVS_GetParameter(AvsHandle a_hDevice, unsigned int a_Size,unsigned int* a_pRequiredSize, DeviceConfigType* a_pData);
AVS_SPECX64_API int __stdcall AVS_GetOemParameter(AvsHandle a_hDevice, OemDataType* a_pOemData);
AVS_SPECX64_API int __stdcall AVS_GetIPAddress(AvsIdentityType* a_pDeviceId, char* a_pIp, int* a_size);
AVS_SPECX64_API int __stdcall AVS_GetComType(AvsIdentityType* a_pDeviceId, int* a_type);
AVS_SPECX64_API int __stdcall AVS_PollScan(AvsHandle a_hDevice);
AVS_SPECX64_API int __stdcall AVS_GetScopeData(AvsHandle a_hDevice, unsigned int* a_pTimeLabel,double* a_pSpectrum);
AVS_SPECX64_API int __stdcall AVS_EnableCRC(AvsHandle a_hDevice, bool a_enableCRC);
AVS_SPECX64_API int __stdcall AVS_GetRawScopeDataCRC(AvsHandle a_hDevice, unsigned int* a_pTimeLabel,unsigned short* a_pAvg, unsigned int* a_pCRC32,uint16* a_nrPixels, uint32* a_pSpectrum);
AVS_SPECX64_API bool __stdcall AVS_CheckCRC(unsigned int a_CRC32, unsigned short a_NrPixels,bool a_Use16Bit, uint32* a_pSpectrum);
AVS_SPECX64_API int __stdcall AVS_GetSaturatedPixels(AvsHandle a_hDevice, unsigned char* a_pSaturated);
AVS_SPECX64_API int __stdcall AVS_GetAnalogIn(AvsHandle a_hDevice, unsigned char a_AnalogInId,float* a_pAnalogIn);
AVS_SPECX64_API int __stdcall AVS_GetDigIn(AvsHandle a_hDevice, unsigned char a_DigInId,unsigned char* a_pDigIn);
AVS_SPECX64_API int __stdcall AVS_GetVersionInfo(AvsHandle a_hDevice, unsigned char* a_pFPGAVersion,unsigned char* a_pFirmwareVersion,unsigned char* a_pDLLVersion);
AVS_SPECX64_API int __stdcall AVS_GetDLLVersion(char* a_pVersionString);
AVS_SPECX64_API int __stdcall AVS_SetParameter(AvsHandle a_hDevice, DeviceConfigType* a_pData);
AVS_SPECX64_API int __stdcall AVS_ResetParameter(AvsHandle a_hDevice);
AVS_SPECX64_API int __stdcall AVS_SetOemParameter(AvsHandle a_hDevice, OemDataType* a_pOemData);
AVS_SPECX64_API int __stdcall AVS_SetAnalogOut(AvsHandle a_hDevice, unsigned char a_PortId,float a_Value);
AVS_SPECX64_API int __stdcall AVS_SetDigOut(AvsHandle a_hDevice, unsigned char a_PortId,unsigned char a_Value);
AVS_SPECX64_API int __stdcall AVS_SetPwmOut(AvsHandle a_hDevice, unsigned char a_PortId,unsigned int a_Frequency, unsigned char a_DutyCycle);
AVS_SPECX64_API int __stdcall AVS_SetSyncMode(AvsHandle a_hDevice, unsigned char a_Enable);
AVS_SPECX64_API int __stdcall AVS_StopMeasure(AvsHandle a_hDevice);
AVS_SPECX64_API int __stdcall AVS_SetPrescanMode(AvsHandle a_hDevice, bool a_Prescan);
AVS_SPECX64_API int __stdcall AVS_UseHighResAdc(AvsHandle a_hDevice, bool a_Enable);
AVS_SPECX64_API int __stdcall AVS_SetSensitivityMode(AvsHandle a_hDevice, unsigned int a_SensitivityMode);
AVS_SPECX64_API int __stdcall AVS_GetIpConfig(AvsHandle a_hDevice, EthernetSettingsType* a_Data);
AVS_SPECX64_API int __stdcall AVS_SuppressStrayLight(AvsHandle a_hDevice, float a_MultiFactor,double* a_pSrcSpectrum, double* a_pDestSpectrum);
AVS_SPECX64_API int __stdcall AVS_ResetDevice(AvsHandle a_hDevice);
// int AVS_Heartbeat(AvsHandle a_hDevice, HeartbeatReqType *a_pHbReq,
//                   HeartbeatRespType *a_pHbResp);

#ifdef __cplusplus
}
#endif

#endif //!__AVASPECX64__H__
