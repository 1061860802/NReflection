#pragma once

#ifdef NReflectionCore

#define NREFLECTIONCORE_API __declspec(dllexport)

#else

#define NREFLECTIONCORE_API __declspec(dllimport)

#endif // NReflectionCore