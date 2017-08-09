copy ..\target\debug\deps\RustyDynamics.dll.lib .\RustyDynamics.dll.lib
copy ..\target\debug\deps\RustyDynamics.dll .\RustyDynamics.dll
cl /W4 /EHsc ffi.cpp RustyDynamics.dll.lib /link /out:ffi.exe

ffi.exe
