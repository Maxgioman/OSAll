#include <iostream> 
#include <windows.h> 
#include <stdio.h> 
#include <conio.h> 
#include <tchar.h> 
#include <string> 

using namespace std;

#define BUFFER_SIZE 256
//Задаємо максимальний розмір об'єкта для запису 

int main()
{
	string mapping_object_name;
	cout << "Enter the name of maping object" << endl;
	cin >> mapping_object_name; //назва області пам'яті ( рядок )
	
	boolean was_created = false; //датчик який перевіряє чи область пам'яті була створена  

	HANDLE hMapFile; //абстрактне посилання на память 
	LPCTSTR  pBuf; //макрос для декларування рядка 

	//перетворення string у wstring, а потім в LPCWSTR, щоби задати його як параметр функції OpenFileMapping або CreateFileMappping
	//LPCWSTR mapping_object = transformation(mapping_object_name);
	wstring stemp = wstring(mapping_object_name.begin(), mapping_object_name.end());
	LPCWSTR mapping_object = stemp.c_str();

	cout << "Searching for |" << mapping_object_name <<"|"<< endl;
	hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, mapping_object); //отримує повний доступ до рядка у спільній пам'яті

	if (hMapFile == NULL) //перевірка чи існує дана область пам'яті
	//якщо не існує, тоді створюємо дану область пам'яті, і змінюємо датчик
	{
		cout << "Object |" << mapping_object_name << "| wasn't found" << endl;
		cout << "Mapping object |" << mapping_object_name << "| will be created" << endl;

		was_created = true; //позначає що область пам'яті була створена 

		hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, BUFFER_SIZE, mapping_object); //створює вказану в параметрах область пам'яті             

		if (hMapFile == NULL) //перевірка чи створилась область пам'яті 
		{
			cout << "Creating of file mapping object failed(%d).\n" << GetLastError(); //при невдачі повертає помилку
			return 1;
		}
	}
	else //якщо існує, тоді показуємо це, і не змінюємо датчик
	{
		cout << "Mmaping object" << mapping_object_name << " has been found" << endl;
		was_created = false; //позначає що область пам'яті вже була створена раніше
	}

	pBuf = (LPCTSTR)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BUFFER_SIZE); //отримуємо лінк на введений рядок у пам'яті 

	if (pBuf == NULL) // якщо область пуста, повертає помилку
	{
		cout << "Mapping view of file failed(%d).\n" << GetLastError();

		CloseHandle(hMapFile); //очищуємо посилання на спільну область пам'яті 
		return 1;
	}

	int command;
	string input;

	do //починається робота програми до моменту виходу певною командою
	{
		cout << "\nAvailable actions with mapping object: " << endl;
		cout << "to write enter '1'" << endl;
		cout << "to read enter '2'" << endl;
		cout << "to exit enter '3'" << endl;
		cin >> command; //введення команди 

		switch (command)
		{
		case 1:
		{
			cout << "Enter the string you want to store in the memory" << endl;
			cin >> input; //введений рядок для запису

			//перехід string в LPCWSTR 
			//LPCWSTR input_text = transformation(input);
			wstring stemp = wstring(input.begin(), input.end());
			LPCWSTR input_text = stemp.c_str();

			CopyMemory((PVOID)pBuf, input_text, (_tcslen(input_text) * sizeof(CHAR*))); //записує рядок в спільну область пам'яті, який можуть бачити інші користувачі/процеси 

			cout << "Your text was stored in memory" << endl;

			was_created = false; //позначає що область пам'яті вже була створена

			break;
		}
		case 2:
		{
			if (was_created == true) //перевіряє чи область пам'яті існує за допомогою датчика 
			{
				cout << "This mmaping object was created recently, so its empty" << endl;
				// область пуста, бо у неї ніхто ще не записував дані
				break;
			}

			MessageBox(NULL, pBuf, TEXT("The content of mapping object is: "), MB_OK); //виводить рядок, який знаходиться в спільній області пам'яті 
			
			break;
		}
		case 3:
		{
			cout << "Work was ended. Goodbye" << endl;
			UnmapViewOfFile(pBuf); //звільняє спільну область пам'яті 
			CloseHandle(hMapFile); //видаляємо наше посилання на спільну область пам'яті
			exit(0); //вихід з програми 
			break;
		}
		}
	} while (true);

	return 0;
}