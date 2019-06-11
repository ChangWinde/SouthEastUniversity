#include<iostream>  
using namespace std;
int q1 = 0;//比较次数
int q2 = 0;//移动次数

void quickSort(int a[], int, int);
int main()
{
	int array[11] = { 23,232,1,1245,44,32,3,35,46,26,91 }, k;
	int len = sizeof(array) / sizeof(int);
	cout << "Before Sort: " << endl;
	for (k = 0; k<len; k++)
		cout << array[k] << ",";
	cout << endl;
	quickSort(array, 0, len - 1);
	cout << "After Sort: " << endl;
	for (k = 0; k<len; k++)
		cout << array[k] << ",";
	cout << endl;
	cout << q1 << ":" << q2 << endl;
	system("pause");
	return 0;
}

void quickSort(int s[], int l, int r)
{
	if (l< r)
	{
		int i = l, j = r, x = s[l];
		while (i < j)
		{
			while (i < j && s[j] >= x) // 从右向左找第一个小于x的数  
			{
				q1 += 2;
				j--;
			}
			if (i < j) {
				s[i++] = s[j];
				q1++;
				q2++;
			}
			while (i < j && s[i] < x) {
				i++;
				q1 += 2;
				q2++;
			} // 从左向右找第一个大于等于x的数  
				
			if (i < j)
				s[j--] = s[i];
			q1++;
			q2 += 2;
		}
		s[i] = x;
		quickSort(s, l, i - 1); // 递归调用  
		quickSort(s, i + 1, r);
	}
}
