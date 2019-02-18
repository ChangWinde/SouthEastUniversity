//main.cpp
#include<iostream>
using namespace std;
int q1 = 0;//比较次数
int q2 = 0;//移动次数

void maxheapDown(int a[], int start, int end)
{
	int c = start;            // 当前(current)节点的位置
	int l = 2 * c + 1;        // 左(left)孩子的位置
	int tmp = a[c];            // 当前(current)节点的大小
	for (; l <= end; c = l, l = 2 * l + 1)
	{
		// "l"是左孩子，"l+1"是右孩子
		if (l < end && a[l] < a[l + 1]) {
			l++;        // 左右两孩子中选择较大者，即m_heap[l+1]
			q1 += 2;
		}
		if (tmp >= a[l]) {
			break;        // 调整结束
			q1++;
		}
		else            // 交换值
		{
			a[c] = a[l];
			a[l] = tmp;
			q2++;
		}
	}
}

void heapSortAsc(int a[], int n)
{
	int i;

	// 从(n/2-1) --> 0逐次遍历。遍历之后，得到的数组实际上是一个(最大)二叉堆。
	for (i = n / 2 - 1; i >= 0; i--)
		maxheapDown(a, i, n - 1);

	// 从最后一个元素开始对序列进行调整，不断的缩小调整的范围直到第一个元素
	for (i = n - 1; i > 0; i--)
	{
		// 交换a[0]和a[i]。交换后，a[i]是a[0...i]中最大的。
		swap(a[0], a[i]);
		q2++;
		// 调整a[0...i-1]，使得a[0...i-1]仍然是一个最大堆。
		// 即，保证a[i-1]是a[0...i-1]中的最大值。
		maxheapDown(a, 0, i - 1);
	}
}

int main() {
	int a[11] = { 23,232,1,1245,44,32,3,35,46,26,91 };
	cout << "Before Sort: " << endl;
	for (int k = 0; k<11; k++)
		cout << a[k] << ",";
	cout << endl;
	heapSortAsc(a, 11);

	cout << "After Sort: " << endl;
	for (int k = 0; k<11; k++)
		cout << a[k] << ",";
	cout << endl;
	cout << q1 << ":" << q2 << endl;
	system("pause");
	return 0;
}
