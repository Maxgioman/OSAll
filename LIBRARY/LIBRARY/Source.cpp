#include <Windows.h>
#include "pch.h"
#include <iostream>
#include <time.h> 
#include <mutex>
#include <chrono>
#include <thread>
#include <cstdlib>
#include "Library.h"

using namespace std;

HANDLE hSemaphore = CreateSemaphore(nullptr, 2, 2, nullptr);

CRITICAL_SECTION section;

struct lpParameters
{
	string path;
	int id;
	HANDLE hSemaphore;
	CRITICAL_SECTION section;
};

DWORD WINAPI attr_search(LPVOID lpParam) {
	struct lpParameters
	{
		string path;
		int id;
		HANDLE hSemaphore;
		CRITICAL_SECTION section;
	};

	const lpParameters* parameter = static_cast<const lpParameters*>(lpParam);
	int id = parameter->id;
	string path = parameter->path;
	HANDLE hSemaphore = parameter->hSemaphore;
	CRITICAL_SECTION section = parameter->section;

	WIN32_FIND_DATA FindFileData;
	HANDLE hf = FindFirstFile(path.c_str(), &FindFileData);
	if (hf != INVALID_HANDLE_VALUE) {
		do
		{
			HANDLE hThread[100];
			WaitForSingleObject(hSemaphore, INFINITE);
			EnterCriticalSection(&section);
			this_thread::sleep_for(0.5s);
			cout << FindFileData.cFileName << endl;
			LeaveCriticalSection(&section);
			ReleaseSemaphore(hSemaphore, 1, NULL);
		} while (FindNextFile(hf, &FindFileData) != 0);
		FindClose(hf);
	}
	else
	{
		printf("Invalid File Handle. GetLastError reports %d\n", GetLastError());
		return (0);
	}
	return 0;
}

int search(void) 
{

	InitializeCriticalSection(&section);

	cout << "Enter how many threads(atributes) you want" << endl;
	int size;
	cin >> size;
	string* attr = new string[size];
	string* attrpath = new string[size];
	cout << "Enter atributes you want to check" << endl;
	for (int i = 0; i < size; i++)
	{
		cin >> attr[i];
		if (attr[i].size() < 0 && attr[i].size() > 4)
		{
			cout << "Try other format" << endl;
			i--;
		}
	}
	for (int i = 0; i < size; i++)
	{
		string currentattr = attr[i];
		string attrpathi({ 'D', ':', '\\', '*', currentattr.at(0), currentattr.at(1), currentattr.at(2), currentattr.at(3) });
		attrpath[i] = attrpathi;
	}

	HANDLE* threadsHandles = new HANDLE[size];

	for (int i = 0; i < size; i++) {
		lpParameters* param = new lpParameters();
		param->id = i;
		param->path = attrpath[i];
		param->hSemaphore = hSemaphore;
		param->section = section;
		/*HINSTANCE hDll = LoadLibrary("C:\\Users\\ACER\\source\\repos\\MathLibrary\\Debug\\MathLibrary.dll");
		if (hDll == NULL)
			cout << "dll failed" << endl;

		attrsearch search = (attrsearch)GetProcAddress(hDll, "attr_search");
		if (search == NULL) {
			cout << "func adress failed" << GetLastError() << endl;
			return 0;
		}*/
		//DWORD search = attr_search(LPVOID(param));
		HANDLE th = CreateThread(NULL, 0, attr_search, LPVOID(param), CREATE_SUSPENDED, NULL);
		//HANDLE th = create(size, i, attrpath[i], hSemaphore, section);
		threadsHandles[i] = th;
	}
	//WaitForMultipleObjects(size, threadsHandles, true, INFINITY);

	int priority = -1;
	int number = -1;
	string success = "priority changed";
	string fail = "failed to change priority";

	while (number != 0) {
		cout << "If you want change priority enter number of thread " << endl << "If not, enter '0' " << endl;
		cin >> number;
		if (number != 0) {
			cout << "1 - above normal" << endl;
			cout << "2 - below normal" << endl;
			cout << "3 - lowest" << endl;
			cout << "4 - normal" << endl;
			cout << "5 - highest" << endl;
			cout << "6 - idle" << endl;
			cout << "7 - critical time" << endl;
		}
		else {
			break;
		}
		cin >> priority;
		switch (priority) {
		case 1:
			if (SetThreadPriority(threadsHandles[number - 1], THREAD_PRIORITY_ABOVE_NORMAL))
				cout << success << endl;
			else
				cout << fail << endl;
			break;
		case 2:
			if (SetThreadPriority(threadsHandles[number - 1], THREAD_PRIORITY_BELOW_NORMAL))
				cout << success << endl;
			else
				cout << fail << endl;
			break;
		case 3:
			if (SetThreadPriority(threadsHandles[number - 1], THREAD_PRIORITY_LOWEST))
				cout << success << endl;
			else
				cout << fail << endl;
		case 4:
			if (SetThreadPriority(threadsHandles[number - 1], THREAD_PRIORITY_NORMAL))
				cout << success << endl;
			else
				cout << fail << endl;
			break;
		case 5:
			if (SetThreadPriority(threadsHandles[number - 1], THREAD_PRIORITY_HIGHEST))
				cout << success << endl;
			else
				cout << fail << endl;
			break;
		case 6:
			if (SetThreadPriority(threadsHandles[number - 1], THREAD_PRIORITY_IDLE))
				cout << success << endl;
			else
				cout << fail << endl;
			break;
		case 7:
			if (SetThreadPriority(threadsHandles[number - 1], THREAD_PRIORITY_TIME_CRITICAL))
				cout << success << endl;
			else
				cout << fail << endl;
			break;
		}
	}
	for (int i = 0; i < size; i++) {
		ResumeThread(threadsHandles[i]);
	}
	CloseHandle(hSemaphore);
	system("pause");
	cout << endl;

	return 0;
}


