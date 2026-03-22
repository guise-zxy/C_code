//#define _CRT_SECURE_NO_WARNINGS
//
//using namespace std;
//#include <iostream>
//#include <climits>
//void find(int* num, int n, int& minIndex, int& maxIndex)
//{
//	int min_val = INT_MAX;
//	int max_val = INT_MIN;
//	for (int i = 0; i < n; i++)
//	{
//		if (num[i] > max_val)
//		{
//			max_val = num[i];
//			maxIndex = i;
//		}
//		if (num[i] < min_val) 
//		{ 
//			min_val = num[i];
//			minIndex= i;
//		}
//	}
//}
//
//
//int main()
//{
//	int t = 0;
//	cin >> t;
//	while (t--)
//	{
//		int n = 0;
//		cin >> n;
//		int* arr = new int [n];
//		for (int i = 0; i < n; i++)
//		{
//			cin >> arr[i];
//		}
//		int minn = -1, maxx = -1;
//		find(arr, n, minn, maxx);
//
//		cout << "min=" << arr[minn] << " " << "minindex=" << minn<<endl;
//		cout << "max=" << arr[maxx] << " " << "maxindex=" << maxx<<endl;
//		//cout << "\n";
//
//	}
//
//
//	return 0;
//}