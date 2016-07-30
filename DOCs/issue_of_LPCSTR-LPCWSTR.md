# Convert from LPWSTR to LPTSTR

* Im trying to convert arguments LPWSTR to LPTSTR, it there any library I can user for easy string convertion?

LPTSTR is defined as follows:

```c
#ifdef UNICODE
 typedef LPWSTR LPTSTR;
#else
 typedef LPSTR LPTSTR;
#endif

```

* So all you need to do is switch your character set to Unicode. For example, in Visual Studio you go to Configuration Properties -> General -> Character Set -> Use Unicode Character Set.
