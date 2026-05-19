#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;

class Television {
private:
    int id;       // 电视机编号
    int volume;   // 音量
    int mode;     // 1表示TV，2表示DVD
    int channel;  // 频道

    static int tvCount;
    static int dvdCount;

public:
    Television() {
        id = 0;
        volume = 50;
        mode = 1;
        channel = 0;
    }

    void init(int i) {
        id = i;
        volume = 50;
        mode = 1;
        channel = i;
        tvCount++;
    }

    void print() {
        cout << "第" << id << "号电视机--";

        if (mode == 1) {
            cout << "TV模式";
        }
        else {
            cout << "DVD模式";
        }

        cout << "--频道" << channel
            << "--音量" << volume << endl;
    }

    static void printCount() {
        cout << "播放电视的电视机数量为" << tvCount << endl;
        cout << "播放DVD的电视机数量为" << dvdCount << endl;
    }

    friend void remoteControl(Television& tv, int newMode, int newChannel, int changeVolume);
};

int Television::tvCount = 0;
int Television::dvdCount = 0;

void remoteControl(Television& tv, int newMode, int newChannel, int changeVolume) {
    // 更新模式数量
    if (tv.mode != newMode) {
        if (tv.mode == 1) {
            Television::tvCount--;
        }
        else {
            Television::dvdCount--;
        }

        if (newMode == 1) {
            Television::tvCount++;
        }
        else {
            Television::dvdCount++;
        }
    }

    // 设置模式
    tv.mode = newMode;

    // 设置频道
    if (newMode == 1) {
        tv.channel = newChannel;
    }
    else {
        tv.channel = 99;
    }

    // 调整音量
    int newVolume = tv.volume + changeVolume;

    if (newVolume > 100) {
        tv.volume = 100;
    }
    else if (newVolume < 0) {
        tv.volume = 0;
    }
    else {
        tv.volume = newVolume;
    }

    tv.print();
}

int main() {
    int n;
    cin >> n;

    Television* tv = new Television[n + 1];

    for (int i = 1; i <= n; i++) {
        tv[i].init(i);
    }

    int t;
    cin >> t;

    while (t--) {
        int i, k, x, v;
        cin >> i >> k >> x >> v;

        remoteControl(tv[i], k, x, v);
    }

    Television::printCount();

    delete[] tv;

    return 0;
}