#include<iostream>  
using namespace std;
int q1 = 0;//比较次数
int q2 = 0;//移动次数
int a[] = { 23,232,1,1245,44,32,3,35,46,26,91 };

void shellSort(int a[], int n);
int main()
{
	cout << "Before Sort: ";
	for (int i = 0; i<11; i++)
		cout << a[i] << " ";
	cout << endl;
	shellSort(a, 11);
	cout << "After Sort: ";
	for (int i = 0; i<11; i++)
		cout << a[i] << " ";
	cout << endl;
	cout << q1 << ":" << q2 << endl;
	system("pause");
}

void shellSort(int a[], int n)
{
	int gap;
	for (gap = 3; gap >0; gap--)
	{
		for (int i = 0; i<gap; i++)
		{
			for (int j = i + gap; j<n; j = j + gap)
			{
				if (a[j]<a[j - gap])
				{
					q1++;
					int temp = a[j];
					int k = j - gap;
					while (k >= 0 && a[k]>temp)
					{
						q1 += 2;
						a[k + gap] = a[k];
						q2++;
						k = k - gap;
					}
					a[k + gap] = temp;
					q2++;
				}
			}
		}
	}
}
