#include <node.h>
#include <v8.h>
#include <ctime>
#include <fstream>
#include "mtrand.h"

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	#include <Wincrypt.h>
	#pragma lib("Advapi32.lib")
#endif

using namespace v8;

MTRand_int32 irand;

#ifdef _WIN32

uint32_t random_seed()
{
	uint32_t seedValue = 0;
	
	HCRYPTPROV hCryptProv = NULL;
	LPCSTR UserName = "MyKeyContainer";
	
	if (!CryptAcquireContext(&hCryptProv, UserName, NULL, PROV_RSA_FULL, 0))
	{
		if (GetLastError() == NTE_BAD_KEYSET)
		{
			if (!CryptAcquireContext(&hCryptProv, UserName, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET))
			{
				exit(1);
			}
		}
	}
	
	if (!CryptGenRandom(hCryptProv, sizeof(seedValue), (BYTE *)&seedValue))
	{
		exit(3);
	}
	
	if (!CryptReleaseContext(hCryptProv, 0))
	{
		exit(2);
	}
	
	return seedValue;
}

#else

uint32_t random_seed()
{
	uint32_t a = 0;
	uint32_t b = 0;

	std::ifstream file("/dev/urandom", std::ios::binary);
	if (file.is_open())
	{
		file.read((char *)&a, sizeof(a));
		file.close();
	}

	b = std::time(0);
	return a ^ b;
}

#endif

Handle<Value> Mtrand(const Arguments& args)
{
	HandleScope scope;

	if (args.Length() > 2)
	{
		return ThrowException(Exception::TypeError(String::New("Random cannot support more than 2 arguments")));
	}

	uint32_t randValue = irand();

	if (args.Length() == 1)
	{
		if (!args[0]->IsUint32())
		{
			return ThrowException(Exception::TypeError(String::New("Range must be a 32 bit unsigned integer")));
		}

		randValue = randValue % args[0]->Uint32Value();
	}

	if (args.Length() == 2)
	{
		if (!args[0]->IsUint32() || !args[1]->IsUint32())
		{
			return ThrowException(Exception::TypeError(String::New("Max value and range must be 32 bit unsigned intergers")));
		}

		if (args[1]->Uint32Value() < args[0]->Uint32Value())
		{
			return ThrowException(Exception::TypeError(String::New("The Max value cannot be less than the Min Value")));
		}

		randValue = (randValue % (args[1]->Uint32Value() - args[0]->Uint32Value())) + args[0]->Uint32Value();
	}

	return scope.Close(Uint32::New(randValue));
}

Handle<Value> MtrandArray(const Arguments& args)
{
	HandleScope scope;
	
	if (args.Length() != 3)
	{
		return ThrowException(Exception::TypeError(String::New("Must provide NumberOfElements, MinValue, MaxValue")));
	}

	if (!args[0]->IsUint32() ||
		!args[1]->IsUint32() ||
		!args[2]->IsUint32())
	{
		return ThrowException(Exception::TypeError(String::New("NumberOfElements, MinValue and MaxValue must be 32 bit unsigned integers.")));
	}
	
	uint32_t size = args[0]->Uint32Value();
	Handle<Array> array = Array::New(size);
	uint32_t min = args[1]->Uint32Value();
	uint32_t max = args[2]->Uint32Value();

	if (max < min)
	{
		return ThrowException(Exception::TypeError(String::New("Max Value must be greater than Min value")));
	}

	for (uint32_t index = 0; index < size; ++index)
	{
		uint32_t rand = (irand() % (max - min)) + min;
		array->Set(index, Uint32::New(rand));
	}
	
	return scope.Close(array);
}

void init(Handle<Object> exports)
{
	irand.seed(random_seed());

	exports->Set(String::NewSymbol("irand"),
			FunctionTemplate::New(Mtrand)->GetFunction());
	exports->Set(String::NewSymbol("irand_array"),
			FunctionTemplate::New(MtrandArray)->GetFunction());
}

NODE_MODULE(mtrand, init)
