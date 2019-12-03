#include <iostream>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <sys/resource.h>

using namespace std;

void func() 
{
	for (double x = -1; x <= 1; x += 0.000001)
	{
		long double res = 1;
		long long denom = 1, num = 1;
		for (int i = 1; i < 10; i++)
		{
			denom *= (i * 3);
			if (i == 1)
				num = 1;
			else if (i == 2)
				num = 2;
			else
			{
				num *= pow(-1, i) * (i * 3);
				num -= 2;
			}
			res -= (pow(x, i) * num) / denom;
		}
		cout << res << endl;
	}
}

int main(void) {
	cout << "Id " << getpid() << endl;

	int n;
	cout << "enter amount of processes:";
	cin >> n;

	pid_t* handles = new pid_t[n];
	int* id = new int[n];
	for (int i = 0; i < n; i++) {
		pid_t pid = fork();
		if (pid < 0) {
			cout << "failed to create process" << endl;
		}
		else if (pid == 0) {
			handles[i] = getpid();
			setpriority(0, getpid(), 1);
			cout << "process created " << getpid() << endl;
			id[i] = pid;
			func();

		}
	}

	return 0;
}