#include <iostream>
#include <windows.h>
#include <fstream>
#include <vector>
#include <string.h>
#include <sys/stat.h>
#include <thread>
#include <process.h> 

using namespace std;

HANDLE hSemaphore = CreateSemaphore(nullptr, 2, 2, nullptr);

vector<string> result;
int cntr = 0;

DWORD WINAPI attr_search(LPVOID lpParam);
CRITICAL_SECTION section;

ofstream fin("D:\\OSLab6.txt");

struct lpParameters
{
	string path;
	int id;
};

int main()
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

		HANDLE th = CreateThread(NULL, 0, attr_search, LPVOID(param), CREATE_SUSPENDED, NULL);
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
	for (int i = 0; i < size; i++) 
	{
		ResumeThread(threadsHandles[i]);
	}
	CloseHandle(hSemaphore);

	system("pause");
	
	ifstream fin("D:\\OSLab6.txt", std::ios::app);
	setlocale(LC_CTYPE, "ukr");
	if (fin.is_open())
	{
		vector<string> x;
		string a;
		while (fin >> a)
		{
			x.push_back(a);
		}

		int N = x.size();
		
		system("pause");

		char* p;  //вказівник на результат ф-ї itoa()
		char fool[15];

		setlocale(LC_ALL, "ukr");

		int size = 0;
		size += N*16;

		HANDLE hFile, hMap;
		LPBYTE pData = NULL;

		hFile = CreateFile("D:\\OSLab6sys.txt", GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE) 
		{
 
			DWORD writtenBytes;
			long long bytes = 0;
			for (string item : x) {
				const char* buf = item.c_str();
				bytes += sizeof(buf);
				WriteFile(hFile, buf, strlen(buf), &writtenBytes, NULL);
				WriteFile(hFile, "\n", 1, &writtenBytes, NULL);
				bytes++;
			}

			HANDLE hMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, bytes * sizeof(BYTE), NULL);

			LPVOID lpMapAddress = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

			CloseHandle(hFile);
			UnmapViewOfFile(pData);
			CloseHandle(hMap);
		}
		
		printf("\nРезультуючий масив знаходиться у файлi: D:\\OSLab6sys.txt");
	}

	return 0;
}

DWORD WINAPI attr_search(LPVOID lpParam)
{
	const lpParameters* parameter = static_cast<const lpParameters*>(lpParam);
	int id = parameter->id;
	string path = parameter->path;

	WIN32_FIND_DATA FindFileData;
	HANDLE hf = FindFirstFile(path.c_str(), &FindFileData);
	if (hf != INVALID_HANDLE_VALUE) {
		do
		{
			HANDLE hThread[100];
			WaitForSingleObject(hSemaphore, INFINITE);
			EnterCriticalSection(&section);
			this_thread::sleep_for(0.5s);
			string s = FindFileData.cFileName;
			//result.push_back(s);
			cout << s << endl;
			ofstream fin("D:\\OSLab6.txt", std::ios::app);
			setlocale(LC_CTYPE, "ukr");
			if (fin.is_open())
			{
				fin << s << endl;
			}
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