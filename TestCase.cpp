//Import Table Hook (Also Called "Function Redirection")
extern "C" void PTS_ImportTableHook(
	void const * const pLibraryImportBaseAddress
	, void const * const pLibraryImportSlideAddress //Only Used By Mach To Support Shared Cache
	, char const * const //pLibraryExportName //Only Used By Win32 To Distinguish Different (Export)Library
	, char const * const * const pFuntionToHookNameVector //SOA(Structure Of Array)
	, void(*const * const pFuntionToHookNewAddressVector)(void) //SOA(Structure Of Array)
	, size_t const FuntionToHookVectorCount
);

//To Allow Custom Allocator (For Example The TBB Malloc By Intel To Avoid Lock In Multi-Thread Envrionment)
//We Try To Hook The "malloc" And "free" In "libobjc.dylib"

#include <stdlib.h>

static void *Hooked_Malloc(size_t size)
{
	return malloc(size); //For Example, We Can Replace It By Intel Tbb Malloc
}

static void Hooked_Free(void *ptr)
{
	free(ptr); //For Example, We Can Replace It By Intel Tbb Malloc
}

#include <mach-o/dyld.h>

void Hook_Allocation()
{
	void const *pLibObjCBaseAddress = NULL;
	void const *pLibObjCSlideAddress = NULL;
	{
		uint32_t Imagecount = ::_dyld_image_count();
		for (uint32_t iImage = 0U; iImage < Imagecount; ++iImage)
		{
			char const *dylibname = ::_dyld_get_image_name(iImage);
			if (::strstr(dylibname, "/libobjc"))
			{
				pLibObjCBaseAddress = static_cast<void const *>(::_dyld_get_image_header(iImage));
				pLibObjCSlideAddress = reinterpret_cast<void const *>(_dyld_get_image_vmaddr_slide(iImage));
				break;
			}
		}
	}

	//Because Of The Name Decoration Of The Compiler, The Actual Name Of The Function In The Mach-O File Has "_" Prefix.
	char const *FuntionToHookNameVector[] = {
		"_malloc"
		,"_free"
	};

	uintptr_t FuntionToHookAddressVector[] = {
		reinterpret_cast<uintptr_t>(&Hooked_Malloc)
		,reinterpret_cast<uintptr_t>(&Hooked_Free)
	};

	assert((sizeof(FuntionToHookNameVector) / sizeof(FuntionToHookNameVector[0])) == (sizeof(FuntionToHookAddressVector) / sizeof(FuntionToHookAddressVector[0])));

	PTS_ImportTableHook(
		pLibObjCBaseAddress,
		pLibObjCSlideAddress,
		NULL,
		FuntionToHookNameVector,
		FuntionToHookAddressVector,
		sizeof(FuntionToHookNameVector) / sizeof(FuntionToHookNameVector[0])
	);
}
