﻿#include "il2cpp.h"
#include "il2cppGC.h"
#include "il2cppBridge.h"
#include <math.h>
#include <algorithm>

#if defined(_WIN32)
#include <windows.h>
#else
#include <sys/types.h>
#include <unistd.h>
#include <sched.h>
#endif

void il2cpp_InitVariables();
void il2cpp_Init()
{
	il2cpp_GC_Init();
	il2cpp_InitVariables();
}

void* il2cpp_New(uint32_t sz, uint32_t typeID, uint8_t isNoRef)
{
	if (sz < 4)
		sz = 4;

	cls_Object* obj;
#if defined(IL2CPP_PATCH_LLVM)
	if (isNoRef)
		obj = (cls_Object*)calloc(sz, 1);
	else
		obj = (cls_Object*)calloc(1, sz);
#else
	if (isNoRef)
		obj = (cls_Object*)il2cpp_GC_AllocAtomic(sz);
	else
		obj = (cls_Object*)il2cpp_GC_Alloc(sz);
#endif
	obj->TypeID = typeID;
	return obj;
}

void* il2cpp_New(uint32_t sz, uint32_t typeID, uint8_t isNoRef, IL2CPP_FINALIZER_FUNC finalizer)
{
	if (sz < 4)
		sz = 4;

	cls_Object* obj;

	if (isNoRef)
		obj = (cls_Object*)il2cpp_GC_AllocAtomic(sz);
	else
		obj = (cls_Object*)il2cpp_GC_Alloc(sz);

	il2cpp_GC_RegisterFinalizer(obj, finalizer);

	obj->TypeID = typeID;
	return obj;
}

void il2cpp_CommitRoots(il2cppRootItem* roots, uint32_t num)
{
	std::sort(roots, roots + num,
		[](const il2cppRootItem& lhs, const il2cppRootItem& rhs)
	{
		return lhs.Ptr < rhs.Ptr;
	});

	uint8_t* start = nullptr;
	uint8_t* end = nullptr;
	for (const il2cppRootItem* itEnd = roots + num;
		roots < itEnd;
		++roots)
	{
		const il2cppRootItem& item = *roots;
		if (start)
		{
			if (item.Ptr <= end)
			{
				end += item.Length;
				continue;
			}
			il2cpp_GC_AddRoots(start, end);
		}

		start = item.Ptr;
		end = item.Ptr + item.Length;
	}
	if (start)
		il2cpp_GC_AddRoots(start, end);
}

void il2cpp_Yield()
{
#if defined(_WIN32)
	Sleep(0);
#else
	sched_yield();
#endif
}

void il2cpp_SleepMS(uint32_t ms)
{
#if defined(_WIN32)
	Sleep(ms);
#else
	usleep(ms * 1000);
#endif
}

uintptr_t il2cpp_ThreadID()
{
#if defined(_WIN32)
	return (uintptr_t)GetCurrentThreadId();
#else
	return (uintptr_t)gettid();
#endif
}

void il2cpp_CallOnce(uint8_t &onceFlag, uintptr_t &lockTid, void(*invokeFunc)())
{
	if (IL2CPP_UNLIKELY(onceFlag != 2))
	{
		if (IL2CPP_ATOMIC_CAS_8(&onceFlag, 0, 1) == 0)
		{
			lockTid = il2cpp_ThreadID();
			invokeFunc();
			IL2CPP_ATOMIC_CAS_8(&onceFlag, 1, 2);
		}
		else if (lockTid != il2cpp_ThreadID())
		{
			while (onceFlag != 2)
				il2cpp_Yield();
		}
		else if (onceFlag != 1)
			IL2CPP_UNREACHABLE;
	}
}

void il2cpp_SpinLock(uint8_t &flag)
{
	uint32_t count = 10;
	while (IL2CPP_ATOMIC_CAS_8(&flag, 0, 1) != 0)
	{
		if (count < 200)
		{
			for (uint32_t i = 0; i < count; ++i)
				il2cpp_Yield();
			++count;
		}
		else
			il2cpp_SleepMS(1);
	}
}

void il2cpp_SpinUnlock(uint8_t &flag)
{
	IL2CPP_ATOMIC_CAS_8(&flag, 1, 0);
}

int32_t il2cpp_HashString(const uint16_t* str, int32_t len)
{
	const size_t szLen = static_cast<size_t>(len);
	const size_t unitSize = sizeof(uintptr_t) / sizeof(uint16_t);
	const size_t step = szLen / unitSize;
	const size_t remain = szLen % unitSize;

	uintptr_t result = 0x14AE055C;
	for (size_t i = 0; i < step; ++i)
	{
		result += result << 5;
		result ^= reinterpret_cast<const uintptr_t*>(str)[i];
	}

	for (size_t i = szLen - remain; i < szLen; ++i)
	{
		result += result << 5;
		result ^= str[i];
	}

	return static_cast<int32_t>(result & 0xFFFFFFFF) ^ ((result >> 32) & 0xFFFFFFFF);
}

double il2cpp_Abs(double n)
{
	return fabs(n);
}

double il2cpp_Sqrt(double n)
{
	return sqrt(n);
}

double il2cpp_Sin(double n)
{
	return sin(n);
}

double il2cpp_Cos(double n)
{
	return cos(n);
}

double il2cpp_Tan(double n)
{
	return tan(n);
}

double il2cpp_Exp(double n)
{
	return exp(n);
}

double il2cpp_Pow(double n, double m)
{
	return pow(n, m);
}

#if defined(IL2CPP_DISABLE_CHECK_RANGE)
void il2cpp_CheckRange(int64_t lowerBound, int64_t length, int64_t index)
{
}
void il2cpp_CheckRange(int64_t lowerBound, int64_t length, int64_t index, int64_t rangeLen)
{
}
#else
void il2cpp_CheckRange(int64_t lowerBound, int64_t length, int64_t index)
{
	if (index < lowerBound || index >= IL2CPP_ADD(lowerBound, length))
		IL2CPP_TRAP;
}

void il2cpp_CheckRange(int64_t lowerBound, int64_t length, int64_t index, int64_t rangeLen)
{
	il2cpp_CheckRange(lowerBound, length, index);

	index = IL2CPP_ADD(index, rangeLen);
	if (index < lowerBound || index > IL2CPP_ADD(lowerBound, length))
		IL2CPP_TRAP;
}
#endif

float il2cpp_Remainder(float numer, float denom)
{
	return remainderf(numer, denom);
}

double il2cpp_Remainder(double numer, double denom)
{
	return remainder(numer, denom);
}

inline int16_t il2cpp_FDTest(float *ptr)
{
	if ((*((uint16_t*)ptr + 1) & 0x7F80) == 0x7F80)
	{
		if (*((uint16_t*)ptr + 1) & 0x7F || *(uint16_t*)ptr)
			return 2;
		else
			return 1;
	}
	else if (*((uint16_t*)ptr + 1) & 0x7FFF || *(uint16_t*)ptr)
	{
		if (*((uint16_t*)ptr + 1) & 0x7F80)
			return -1;
		else
			return -2;
	}
	return 0;
}

inline int16_t il2cpp_DTest(double *ptr)
{
	if ((*((uint16_t*)ptr + 3) & 0x7FF0) == 0x7FF0)
	{
		if (*((uint16_t*)ptr + 3) & 0xF || *(uint32_t*)((char*)ptr + 2) || *(uint16_t*)ptr)
			return 2;
		else
			return 1;
	}
	else if (*((uint16_t*)ptr + 3) & 0x7FFF || *(uint32_t*)((char*)ptr + 2) || *(uint16_t*)ptr)
	{
		if (*((uint16_t*)ptr + 3) & 0x7FF0)
			return -1;
		else
			return -2;
	}
	return 0;
}

inline bool il2cpp_IsFinite(float num)
{
	return il2cpp_FDTest(&num) <= 0;
}

inline bool il2cpp_IsFinite(double num)
{
	return il2cpp_DTest(&num) <= 0;
}

#if defined(IL2CPP_BRIDGE_HAS_4ObKN3_ThrowHelper__Throw_ArithmeticException)
float il2cpp_Ckfinite(float num)
{
	if (IL2CPP_UNLIKELY(!il2cpp_IsFinite(num)))
		met_4ObKN3_ThrowHelper__Throw_ArithmeticException();
	return num;
}

double il2cpp_Ckfinite(double num)
{
	if (IL2CPP_UNLIKELY(!il2cpp_IsFinite(num)))
		met_4ObKN3_ThrowHelper__Throw_ArithmeticException();
	return num;
}
#endif

#if defined(IL2CPP_BRIDGE_HAS_GJNRe4_ThrowHelper__Throw_InvalidCastException)
void il2cpp_ThrowInvalidCast()
{
	met_GJNRe4_ThrowHelper__Throw_InvalidCastException();
}
#endif

#if defined(IL2CPP_BRIDGE_HAS_aBjyT3_ThrowHelper__Throw_OverflowException)
void il2cpp_ThrowOverflow()
{
	met_aBjyT3_ThrowHelper__Throw_OverflowException();
}
#endif

#if defined(IL2CPP_BRIDGE_HAS_cls_System_Array)
uint32_t il2cpp_SZArray__LoadLength(cls_System_Array* ary)
{
	IL2CPP_ASSERT(ary->Rank == 0);
	return (uint32_t)ary->Length;
}

uint32_t il2cpp_Array__GetLength(cls_System_Array* ary)
{
	return (uint32_t)ary->Length;
}

uint64_t il2cpp_Array__GetLongLength(cls_System_Array* ary)
{
	return (uint64_t)ary->Length;
}

uint32_t il2cpp_Array__GetLength(cls_System_Array* ary, uint32_t dim)
{
	if (IL2CPP_LIKELY(ary->Rank == 0))
	{
		IL2CPP_CHECK_RANGE(0, 1, dim);
		return (uint32_t)ary->Length;
	}
	else
	{
		IL2CPP_CHECK_RANGE(0, ary->Rank, dim);
		return ((uint32_t*)&ary[1])[dim * 2 + 1];
	}
}

int32_t il2cpp_Array__GetLowerBound(cls_System_Array* ary, uint32_t dim)
{
	if (IL2CPP_LIKELY(ary->Rank == 0))
	{
		IL2CPP_CHECK_RANGE(0, 1, dim);
		return 0;
	}
	else
	{
		IL2CPP_CHECK_RANGE(0, ary->Rank, dim);
		return ((int32_t*)&ary[1])[dim * 2];
	}
}

int32_t il2cpp_Array__GetUpperBound(cls_System_Array* ary, uint32_t dim)
{
	if (IL2CPP_LIKELY(ary->Rank == 0))
	{
		IL2CPP_CHECK_RANGE(0, 1, dim);
		return (int32_t)(ary->Length - 1);
	}
	else
	{
		IL2CPP_CHECK_RANGE(0, ary->Rank, dim);
		int32_t* pBound = (int32_t*)&ary[1];
		return pBound[dim * 2] + (uint32_t)pBound[dim * 2 + 1] - 1;
	}
}

void il2cpp_Array__Copy(cls_System_Array* srcAry, uint32_t srcIdx, cls_System_Array* dstAry, uint32_t dstIdx, uint32_t copyLen)
{
	auto elemSize = dstAry->ElemSize;
	auto rank = dstAry->Rank;
	IL2CPP_ASSERT(elemSize == srcAry->ElemSize);
	IL2CPP_ASSERT(rank == srcAry->Rank);

	auto srcLen = srcAry->Length;
	auto dstLen = dstAry->Length;
	il2cpp_CheckRange(0, srcLen, srcIdx, copyLen);
	il2cpp_CheckRange(0, dstLen, dstIdx, copyLen);

	uint32_t dataOffset = rank == 0 ? 0 : rank * sizeof(uint32_t) * 2;
	uint8_t* srcPtr = (uint8_t*)&srcAry[1] + dataOffset + elemSize * srcIdx;
	uint8_t* dstPtr = (uint8_t*)&dstAry[1] + dataOffset + elemSize * dstIdx;

	IL2CPP_MEMCPY(dstPtr, srcPtr, elemSize * copyLen);
}

void il2cpp_Array__Clear(cls_System_Array* ary, uint32_t idx, uint32_t clearLen)
{
	auto elemSize = ary->ElemSize;
	auto rank = ary->Rank;
	auto aryLen = ary->Length;
	il2cpp_CheckRange(0, aryLen, idx, clearLen);

	uint32_t dataOffset = rank == 0 ? 0 : rank * sizeof(uint32_t) * 2;
	uint8_t* ptr = (uint8_t*)&ary[1] + dataOffset + elemSize * idx;

	IL2CPP_MEMSET(ptr, 0, elemSize * clearLen);
}

void il2cpp_Array__Init(cls_System_Array* ary, il2cppFieldInfo* fldInfo)
{
	auto elemSize = ary->ElemSize;
	auto rank = ary->Rank;
	auto aryLen = ary->Length;

	uint32_t dataOffset = rank == 0 ? 0 : rank * sizeof(uint32_t) * 2;
	uint8_t* ptr = (uint8_t*)&ary[1] + dataOffset;

	IL2CPP_MEMCPY(ptr, fldInfo->FieldInit.Data, il2cpp_Min(elemSize * aryLen, fldInfo->FieldInit.Length));
}
#endif
