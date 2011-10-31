/*! \file cuda_runtime.h
	\author Andrew Kerr <arkerr@gatech.edu>
	\brief implements an up-to-date CUDA Runtime API
	\date 11 Dec 2009
*/

#ifndef CUDA_RUNTIME_H_INCLUDED
#define CUDA_RUNTIME_H_INCLUDED

// C includes
#include <cstring>
#include <limits.h>
#include <cstdint>

// Ocelot includes
#include <ocelot/cuda/interface/cudaFatBinary.h>

#ifdef __cplusplus
extern "C" {
#endif

struct cudaArray;
struct cudaMemcpy3DParms;
struct textureReference;

typedef int cudaEvent_t;
typedef int cudaStream_t;
typedef unsigned int GLuint;

#define cudaHostAllocDefault        0   ///< Default page-locked allocation flag
#define cudaHostAllocPortable       1   ///< Pinned memory accessible by all CUDA contexts
#define cudaHostAllocMapped         2   ///< Map allocation into device space
#define cudaHostAllocWriteCombined  4   ///< Write-combined memory

#define cudaEventDefault            0   ///< Default event flag
#define cudaEventBlockingSync       1   ///< Event uses blocking synchronization

#define cudaDeviceScheduleAuto      0   ///< Device flag - Automatic scheduling
#define cudaDeviceScheduleSpin      1   ///< Device flag - Spin default scheduling
#define cudaDeviceScheduleYield     2   ///< Device flag - Yield default scheduling
#define cudaDeviceBlockingSync      4   ///< Device flag - Use blocking synchronization
#define cudaDeviceMapHost           8   ///< Device flag - Support mapped pinned allocations
#define cudaDeviceLmemResizeToMax   16  ///< Device flag - Keep local memory allocation after launch
#define cudaDeviceMask              0x1f ///< Device flags mask

enum cudaMemcpyKind {
	cudaMemcpyHostToHost = 0,
	cudaMemcpyHostToDevice = 1,
	cudaMemcpyDeviceToHost = 2,
	cudaMemcpyDeviceToDevice = 3
};

enum cudaChannelFormatKind {
	cudaChannelFormatKindSigned = 0,
	cudaChannelFormatKindUnsigned = 1,
	cudaChannelFormatKindFloat = 2,
	cudaChannelFormatKindNone = 3
};

enum cudaComputeMode {
	cudaComputeModeDefault,
	cudaComputeModeExclusive,
	cudaComputeModeProhibited
};

enum cudaError
{
  cudaSuccess                           =      0,   ///< No errors
  cudaErrorMissingConfiguration         =      1,   ///< Missing configuration error
  cudaErrorMemoryAllocation             =      2,   ///< Memory allocation error
  cudaErrorInitializationError          =      3,   ///< Initialization error
  cudaErrorLaunchFailure                =      4,   ///< Launch failure
  cudaErrorPriorLaunchFailure           =      5,   ///< Prior launch failure
  cudaErrorLaunchTimeout                =      6,   ///< Launch timeout error
  cudaErrorLaunchOutOfResources         =      7,   ///< Launch out of resources error
  cudaErrorInvalidDeviceFunction        =      8,   ///< Invalid device function
  cudaErrorInvalidConfiguration         =      9,   ///< Invalid configuration
  cudaErrorInvalidDevice                =     10,   ///< Invalid device
  cudaErrorInvalidValue                 =     11,   ///< Invalid value
  cudaErrorInvalidPitchValue            =     12,   ///< Invalid pitch value
  cudaErrorInvalidSymbol                =     13,   ///< Invalid symbol
  cudaErrorMapBufferObjectFailed        =     14,   ///< Map buffer object failed
  cudaErrorUnmapBufferObjectFailed      =     15,   ///< Unmap buffer object failed
  cudaErrorInvalidHostPointer           =     16,   ///< Invalid host pointer
  cudaErrorInvalidDevicePointer         =     17,   ///< Invalid device pointer
  cudaErrorInvalidTexture               =     18,   ///< Invalid texture
  cudaErrorInvalidTextureBinding        =     19,   ///< Invalid texture binding
  cudaErrorInvalidChannelDescriptor     =     20,   ///< Invalid channel descriptor
  cudaErrorInvalidMemcpyDirection       =     21,   ///< Invalid memcpy direction
  cudaErrorAddressOfConstant            =     22,   ///< Address of constant error
  cudaErrorTextureFetchFailed           =     23,   ///< Texture fetch failed
  cudaErrorTextureNotBound              =     24,   ///< Texture not bound error
  cudaErrorSynchronizationError         =     25,   ///< Synchronization error
  cudaErrorInvalidFilterSetting         =     26,   ///< Invalid filter setting
  cudaErrorInvalidNormSetting           =     27,   ///< Invalid norm setting
  cudaErrorMixedDeviceExecution         =     28,   ///< Mixed device execution
  cudaErrorCudartUnloading              =     29,   ///< CUDA runtime unloading
  cudaErrorUnknown                      =     30,   ///< Unknown error condition
  cudaErrorNotYetImplemented            =     31,   ///< Function not yet implemented
  cudaErrorMemoryValueTooLarge          =     32,   ///< Memory value too large
  cudaErrorInvalidResourceHandle        =     33,   ///< Invalid resource handle
  cudaErrorNotReady                     =     34,   ///< Not ready error
  cudaErrorInsufficientDriver           =     35,   ///< CUDA runtime is newer than driver
  cudaErrorSetOnActiveProcess           =     36,   ///< Set on active process error
  cudaErrorNoDevice                     =     38,   ///< No available CUDA device
  cudaErrorECCUncorrectable             =     39,   ///< Uncorrectable ECC error detected
  cudaErrorStartupFailure               =   0x7f,   ///< Startup failure
  cudaErrorApiFailureBase               =  10000    ///< API failure base
};

typedef cudaError cudaError_t;

enum cudaFuncCache
{
  cudaFuncCachePreferNone   = 0,    ///< Default function cache configuration, no preference
  cudaFuncCachePreferShared = 1,    ///< Prefer larger shared memory and smaller L1 cache 
  cudaFuncCachePreferL1     = 2     ///< Prefer larger L1 cache and smaller shared memory
};

enum cudaLimit
{
    cudaLimitStackSize      = 0x00, //< GPU thread stack size
    cudaLimitPrintfFifoSize = 0x01, //< GPU printf FIFO size
    cudaLimitMallocHeapSize = 0x02  //< GPU malloc heap size
};

struct uint3 {
	unsigned int x, y, z;
};

struct dim3
{
    unsigned int x, y, z;
#if defined(__cplusplus) && !defined(__CUDABE__)
    dim3(unsigned int x = 1, unsigned int y = 1, unsigned int z = 1) : x(x), y(y), z(z) {}
    dim3(uint3 v) : x(v.x), y(v.y), z(v.z) {}
    operator uint3(void) { uint3 t; t.x = x; t.y = y; t.z = z; return t; }
#endif
};

/*DEVICE_BUILTIN*/
typedef struct dim3 dim3;

struct cudaExtent {
	size_t width;
	size_t height;
	size_t depth;
};

struct cudaDeviceProp
{
  char   name[256];                  /**< ASCII string identifying device */
  size_t totalGlobalMem;             /**< Global memory available on device in bytes */
  size_t sharedMemPerBlock;          /**< Shared memory available per block in bytes */
  int    regsPerBlock;               /**< 32-bit registers available per block */
  int    warpSize;                   /**< Warp size in threads */
  size_t memPitch;                   /**< Maximum pitch in bytes allowed by memory copies */
  int    maxThreadsPerBlock;         /**< Maximum number of threads per block */
  int    maxThreadsDim[3];           /**< Maximum size of each dimension of a block */
  int    maxGridSize[3];             /**< Maximum size of each dimension of a grid */
  int    clockRate;                  /**< Clock frequency in kilohertz */
  size_t totalConstMem;              /**< Constant memory available on device in bytes */
  int    major;                      /**< Major compute capability */
  int    minor;                      /**< Minor compute capability */
  size_t textureAlignment;           /**< Alignment requirement for textures */
  int    deviceOverlap;              /**< Device can concurrently copy memory and execute a kernel. Deprecated. Use instead asyncEngineCount. */
  int    multiProcessorCount;        /**< Number of multiprocessors on device */
  int    kernelExecTimeoutEnabled;   /**< Specified whether there is a run time limit on kernels */
  int    integrated;                 /**< Device is integrated as opposed to discrete */
  int    canMapHostMemory;           /**< Device can map host memory with cudaHostAlloc/cudaHostGetDevicePointer */
  int    computeMode;                /**< Compute mode (See ::cudaComputeMode) */
  int    maxTexture1D;               /**< Maximum 1D texture size */
  int    maxTexture2D[2];            /**< Maximum 2D texture dimensions */
  int    maxTexture3D[3];            /**< Maximum 3D texture dimensions */
  int    maxTexture1DLayered[2];     /**< Maximum 1D layered texture dimensions */
  int    maxTexture2DLayered[3];     /**< Maximum 2D layered texture dimensions */
  size_t surfaceAlignment;           /**< Alignment requirements for surfaces */
  int    concurrentKernels;          /**< Device can possibly execute multiple kernels concurrently */
  int    ECCEnabled;                 /**< Device has ECC support enabled */
  int    pciBusID;                   /**< PCI bus ID of the device */
  int    pciDeviceID;                /**< PCI device ID of the device */
  int    pciDomainID;                /**< PCI domain ID of the device */
  int    tccDriver;                  /**< 1 if device is a Tesla device using TCC driver, 0 otherwise */
  int    asyncEngineCount;           /**< Number of asynchronous engines */
  int    unifiedAddressing;          /**< Device shares a unified address space with the host */
  int    memoryClockRate;            /**< Peak memory clock frequency in kilohertz */
  int    memoryBusWidth;             /**< Global memory bus width in bits */
  int    l2CacheSize;                /**< Size of L2 cache in bytes */
  int    maxThreadsPerMultiProcessor;/**< Maximum resident threads per multiprocessor */
};

struct cudaChannelFormatDesc {
	int x;
	int y;
	int z;
	int w;
	enum cudaChannelFormatKind f;
};

struct cudaFuncAttributes {
   size_t sharedSizeBytes;  ///< Size of shared memory in bytes
   size_t constSizeBytes;   ///< Size of constant memory in bytes
   size_t localSizeBytes;   ///< Size of local memory in bytes
   int maxThreadsPerBlock;  ///< Maximum number of threads per block
   int numRegs;             ///< Number of registers used
   int ptxVersion;          ///< PTX version number eq 21
   int binaryVersion;       ///< binary version 
};

struct cudaPitchedPtr {
	void *ptr;
	size_t pitch;
	size_t xsize;
	size_t ysize;
};

struct cudaPos {
	size_t x;
	size_t y;
	size_t z;
};

struct cudaMemcpy3DParms {
	struct cudaArray *srcArray;
	struct cudaPos srcPos;
	struct cudaPitchedPtr srcPtr;
	struct cudaArray *dstArray;
	struct cudaPos dstPos;
	struct cudaPitchedPtr dstPtr;
	struct cudaExtent extent;
	enum cudaMemcpyKind kind;
};

enum cudaTextureAddressMode
{
  cudaAddressModeWrap,
  cudaAddressModeClamp
};

enum cudaTextureFilterMode
{
  cudaFilterModePoint,
  cudaFilterModeLinear
};

enum cudaTextureReadMode
{
  cudaReadModeElementType,
  cudaReadModeNormalizedFloat
};

struct textureReference {
  int normalized;
  cudaTextureFilterMode filterMode;
  cudaTextureAddressMode addressMode[3];
  cudaChannelFormatDesc channelDesc;
  int __cudaReserved[16];
};

typedef struct CUuuid_st cudaUUID_t;

/*
 * Function        : Select a load image from the __cudaFat binary
 *                   that will run on the specified GPU.
 * Parameters      : binary  (I) Fat binary
 *                   policy  (I) Parameter influencing the selection process in case no
 *                               fully matching cubin can be found, but instead a choice can
 *                               be made between ptx compilation or selection of a
 *                               cubin for a less capable GPU.
 *                   gpuName (I) Name of target GPU
 *                   cubin   (O) Returned cubin text string, or NULL when 
 *                               no matching cubin for the specified gpu
 *                               could be found.
 *                   dbgInfo (O) If this parameter is not NULL upon entry, then
 *                               the name of a file containing debug information
 *                               on the returned cubin will be returned, or NULL 
 *                               will be returned when cubin or such debug info 
 *                               cannot be found.
 */
void fatGetCubinForGpuWithPolicy( __cudaFatCudaBinary *binary, __cudaFatCompilationPolicy policy, char* gpuName, char* *cubin, char* *dbgInfoFile );

#define fatGetCubinForGpu(binary,gpuName,cubin,dbgInfoFile) \
          fatGetCubinForGpuWithPolicy(binary,__cudaFatAvoidPTX,gpuName,cubin,dbgInfoFile)

/*
 * Function        : Check if a binary will be JITed for the specified target architecture
 * Parameters      : binary  (I) Fat binary
 *                   policy  (I) Compilation policy, as described by fatGetCubinForGpuWithPolicy
 *                   gpuName (I) Name of target GPU
 *                   ptx     (O) PTX string to be JITed
 * Function Result : True if the given binary will be JITed; otherwise, False
 */
unsigned char fatCheckJitForGpuWithPolicy( __cudaFatCudaBinary *binary, __cudaFatCompilationPolicy policy, char* gpuName, char* *ptx );

#define fatCheckJitForGpu(binary,gpuName,ptx) \
          fatCheckJitForGpuWithPolicy(binary,__cudaFatAvoidPTX,gpuName,ptx)

/*
 * Function        : Free information previously obtained via function fatGetCubinForGpu.
 * Parameters      : cubin   (I) Cubin text string to free
 *                   dbgInfo (I) Debug info filename to free, or NULL
 */
void fatFreeCubin( char* cubin, char* dbgInfoFile );


/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern void** __cudaRegisterFatBinary(void *fatCubin);

extern void __cudaUnregisterFatBinary(void **fatCubinHandle);

extern void __cudaRegisterVar(void **fatCubinHandle, char *hostVar, 
	char *deviceAddress, const char *deviceName, int ext, int size, 
	int constant, int global);

extern void __cudaRegisterTexture(
        void **fatCubinHandle,
  const struct textureReference *hostVar,
  const void **deviceAddress,
  const char *deviceName,
        int dim,
        int norm,
        int ext
);

extern void __cudaRegisterShared(
  void **fatCubinHandle,
  void **devicePtr
);

extern void __cudaRegisterSharedVar(
  void **fatCubinHandle,
  void **devicePtr,
  size_t size,
  size_t alignment,
  int storage
);

extern void __cudaRegisterFunction(
        void **fatCubinHandle,
  const char *hostFun,
        char *deviceFun,
  const char *deviceName,
        int thread_limit,
        uint3 *tid,
        uint3 *bid,
        dim3 *bDim,
        dim3 *gDim,
        int *wSize
);

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern cudaError_t cudaMalloc3D(struct cudaPitchedPtr* pitchedDevPtr, 
	struct cudaExtent extent);
extern cudaError_t cudaMalloc3DArray(struct cudaArray** arrayPtr, 
	const struct cudaChannelFormatDesc* desc, struct cudaExtent extent);
extern cudaError_t cudaMemset3D(struct cudaPitchedPtr pitchedDevPtr, 
	int value, struct cudaExtent extent);
extern cudaError_t cudaMemcpy3D(const struct cudaMemcpy3DParms *p);
extern cudaError_t cudaMemcpy3DAsync(const struct cudaMemcpy3DParms *p, 
	cudaStream_t stream);


/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern cudaError_t cudaMalloc(void **devPtr, size_t size);
extern cudaError_t cudaMallocHost(void **ptr, size_t size);
extern cudaError_t cudaMallocPitch(void **devPtr, size_t *pitch, 
	size_t width, size_t height);
extern cudaError_t cudaMallocArray(struct cudaArray **array, 
	const struct cudaChannelFormatDesc *desc, size_t width, size_t height = 1);
extern cudaError_t cudaFree(void *devPtr);
extern cudaError_t cudaFreeHost(void *ptr);
extern cudaError_t cudaFreeArray(struct cudaArray *array);

extern cudaError_t cudaHostAlloc(void **pHost, size_t bytes, unsigned int flags);
extern cudaError_t cudaHostGetDevicePointer(void **pDevice, void *pHost, 
	unsigned int flags);
extern cudaError_t cudaHostGetFlags(unsigned int *pFlags, void *pHost);

extern cudaError_t cudaHostRegister(void *pHost, size_t bytes, unsigned int flags);
extern cudaError_t cudaHostUnregister(void *pHost);

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern cudaError_t cudaMemcpy(void *dst, const void *src, size_t count, 
	enum cudaMemcpyKind kind);
extern cudaError_t cudaMemcpyToArray(struct cudaArray *dst, size_t wOffset, 
	size_t hOffset, const void *src, size_t count, enum cudaMemcpyKind kind);
extern cudaError_t cudaMemcpyFromArray(void *dst, const struct cudaArray *src, 
	size_t wOffset, size_t hOffset, size_t count, enum cudaMemcpyKind kind);
extern cudaError_t cudaMemcpyArrayToArray(struct cudaArray *dst, size_t wOffsetDst, 
	size_t hOffsetDst, const struct cudaArray *src, size_t wOffsetSrc, 
	size_t hOffsetSrc, size_t count, enum cudaMemcpyKind kind = cudaMemcpyDeviceToDevice);
extern cudaError_t cudaMemcpy2D(void *dst, size_t dpitch, const void *src, 
	size_t spitch, size_t width, size_t height, enum cudaMemcpyKind kind);
extern cudaError_t cudaMemcpy2DToArray(struct cudaArray *dst, size_t wOffset, 
	size_t hOffset, const void *src, size_t spitch, size_t width, 
	size_t height, enum cudaMemcpyKind kind);
extern cudaError_t cudaMemcpy2DFromArray(void *dst, size_t dpitch, 
	const struct cudaArray *src, size_t wOffset, size_t hOffset, size_t width, 
	size_t height, enum cudaMemcpyKind kind);
extern cudaError_t cudaMemcpy2DArrayToArray(struct cudaArray *dst, 
	size_t wOffsetDst, size_t hOffsetDst, const struct cudaArray *src, 
	size_t wOffsetSrc, size_t hOffsetSrc, size_t width, size_t height, 
	enum cudaMemcpyKind kind = cudaMemcpyDeviceToDevice);
extern cudaError_t cudaMemcpyToSymbol(const char *symbol, const void *src, 
	size_t count, size_t offset = 0, 
	enum cudaMemcpyKind kind = cudaMemcpyHostToDevice);
extern cudaError_t cudaMemcpyFromSymbol(void *dst, const char *symbol, 
	size_t count, size_t offset = 0, 
	enum cudaMemcpyKind kind = cudaMemcpyDeviceToHost);

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern cudaError_t cudaMemcpyAsync(void *dst, const void *src, size_t count, 
	enum cudaMemcpyKind kind, cudaStream_t stream);
extern cudaError_t cudaMemcpyToArrayAsync(struct cudaArray *dst, size_t wOffset,
	size_t hOffset, const void *src, size_t count, enum cudaMemcpyKind kind, 
	cudaStream_t stream);
extern cudaError_t cudaMemcpyFromArrayAsync(void *dst, 
	const struct cudaArray *src, size_t wOffset, size_t hOffset, size_t count, 
	enum cudaMemcpyKind kind, cudaStream_t stream);
extern cudaError_t cudaMemcpy2DAsync(void *dst, size_t dpitch, const void *src, 
	size_t spitch, size_t width, size_t height, enum cudaMemcpyKind kind, 
	cudaStream_t stream);
extern cudaError_t cudaMemcpy2DToArrayAsync(struct cudaArray *dst, 
	size_t wOffset, size_t hOffset, const void *src, size_t spitch, 
	size_t width, size_t height, enum cudaMemcpyKind kind, cudaStream_t stream);
extern cudaError_t cudaMemcpy2DFromArrayAsync(void *dst, size_t dpitch, 
	const struct cudaArray *src, size_t wOffset, size_t hOffset, size_t width, 
	size_t height, enum cudaMemcpyKind kind, cudaStream_t stream);
extern cudaError_t cudaMemcpyToSymbolAsync(const char *symbol, const void *src, 
	size_t count, size_t offset, enum cudaMemcpyKind kind, cudaStream_t stream);
extern cudaError_t cudaMemcpyFromSymbolAsync(void *dst, const char *symbol, 
	size_t count, size_t offset, enum cudaMemcpyKind kind, cudaStream_t stream);

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern cudaError_t cudaMemset(void *devPtr, int value, size_t count);
extern cudaError_t cudaMemset2D(void *devPtr, size_t pitch, int value, 
	size_t width, size_t height);

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern cudaError_t cudaGetSymbolAddress(void **devPtr, const char *symbol);
extern cudaError_t cudaGetSymbolSize(size_t *size, const char *symbol);

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern cudaError_t cudaGetDeviceCount(int *count);
extern cudaError_t cudaGetDeviceProperties(struct cudaDeviceProp *prop, int device);
extern cudaError_t cudaChooseDevice(int *device, const struct cudaDeviceProp *prop);
extern cudaError_t cudaSetDevice(int device);
extern cudaError_t cudaGetDevice(int *device);
extern cudaError_t cudaSetValidDevices(int *device_arr, int len);
extern cudaError_t cudaSetDeviceFlags( int flags );

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern cudaError_t cudaBindTexture(size_t *offset, 
	const struct textureReference *texref, const void *devPtr, 
	const struct cudaChannelFormatDesc *desc, size_t size = UINT_MAX);
extern cudaError_t cudaBindTexture2D(size_t *offset,
	const struct textureReference *texref,const void *devPtr, 
	const struct cudaChannelFormatDesc *desc,size_t width, size_t height, 
	size_t pitch);
extern cudaError_t cudaBindTextureToArray(const struct textureReference *texref, 
	const struct cudaArray *array, const struct cudaChannelFormatDesc *desc);
extern cudaError_t cudaUnbindTexture(const struct textureReference *texref);
extern cudaError_t cudaGetTextureAlignmentOffset(size_t *offset, 
	const struct textureReference *texref);
extern cudaError_t cudaGetTextureReference(const struct textureReference **texref, 
	const char *symbol);

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern cudaError_t cudaGetChannelDesc(struct cudaChannelFormatDesc *desc, 
	const struct cudaArray *array);
extern struct cudaChannelFormatDesc cudaCreateChannelDesc(int x, int y, int z, 
	int w, enum cudaChannelFormatKind f);

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern cudaError_t cudaGetLastError(void);
extern const char* cudaGetErrorString(cudaError_t error);

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern cudaError_t cudaConfigureCall(dim3 gridDim, dim3 blockDim, 
	size_t sharedMem = 0, cudaStream_t stream = 0);
extern cudaError_t cudaSetupArgument(const void *arg, size_t size, 
	size_t offset);
extern cudaError_t cudaLaunch(const char *entry);
extern cudaError_t cudaFuncGetAttributes(struct cudaFuncAttributes *attr, 
	const char *func);
extern cudaError_t cudaFuncSetCacheConfig(const char *func, 
	enum cudaFuncCache cacheConfig);

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern cudaError_t cudaStreamCreate(cudaStream_t *pStream);
extern cudaError_t cudaStreamDestroy(cudaStream_t stream);
extern cudaError_t cudaStreamSynchronize(cudaStream_t stream);
extern cudaError_t cudaStreamQuery(cudaStream_t stream);

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern cudaError_t cudaEventCreate(cudaEvent_t *event);
extern cudaError_t cudaEventCreateWithFlags(cudaEvent_t *event, int flags);
extern cudaError_t cudaEventRecord(cudaEvent_t event, cudaStream_t stream);
extern cudaError_t cudaEventQuery(cudaEvent_t event);
extern cudaError_t cudaEventSynchronize(cudaEvent_t event);
extern cudaError_t cudaEventDestroy(cudaEvent_t event);
extern cudaError_t cudaEventElapsedTime(float *ms, cudaEvent_t start, cudaEvent_t end);


/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern cudaError_t cudaGraphicsGLRegisterBuffer(
	struct cudaGraphicsResource **resource, GLuint buffer, unsigned int flags);
extern cudaError_t cudaGraphicsGLRegisterImage(
	struct cudaGraphicsResource **resource, GLuint image, int target, 
	unsigned int flags);

extern cudaError_t cudaGraphicsUnregisterResource(
	struct cudaGraphicsResource *resource);
extern cudaError_t cudaGraphicsResourceSetMapFlags(
	struct cudaGraphicsResource *resource, unsigned int flags); 
extern cudaError_t cudaGraphicsMapResources(int count, 
	struct cudaGraphicsResource **resources, cudaStream_t stream = 0);
extern cudaError_t cudaGraphicsUnmapResources(int count, 
	struct cudaGraphicsResource **resources, cudaStream_t stream = 0);
extern cudaError_t cudaGraphicsResourceGetMappedPointer(void **devPtr, 
	size_t *size, struct cudaGraphicsResource *resource);
extern cudaError_t cudaGraphicsSubResourceGetMappedArray(
	struct cudaArray **arrayPtr, struct cudaGraphicsResource *resource, 
	unsigned int arrayIndex, unsigned int mipLevel);

extern cudaError_t cudaGLMapBufferObject(void **devPtr, GLuint bufObj);
extern cudaError_t cudaGLMapBufferObjectAsync(void **devPtr, 
	GLuint bufObj, cudaStream_t stream);
extern cudaError_t cudaGLRegisterBufferObject(GLuint bufObj);
extern cudaError_t cudaGLSetBufferObjectMapFlags(GLuint bufObj, 
	unsigned int flags);
extern cudaError_t cudaGLSetGLDevice(int device);
extern cudaError_t cudaGLUnmapBufferObject(GLuint bufObj);
extern cudaError_t cudaGLUnmapBufferObjectAsync(GLuint bufObj, cudaStream_t stream);
extern cudaError_t cudaGLUnregisterBufferObject(GLuint bufObj);

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern cudaError_t cudaSetDoubleForDevice(double *d);
extern cudaError_t cudaSetDoubleForHost(double *d);

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern cudaError_t cudaThreadExit(void);
extern cudaError_t cudaThreadSynchronize(void);

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern cudaError_t cudaDriverGetVersion(int *driverVersion);
extern cudaError_t cudaRuntimeGetVersion(int *runtimeVersion);
extern cudaError_t cudaGetExportTable(const void **ppExportTable,
	const cudaUUID_t *pExportTableId);

#ifdef __cplusplus
}
#endif

#endif

