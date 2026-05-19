//#define _CRT_SECURE_NO_WARNINGS
//
//
//#include <iostream>
//#include <string>
//using namespace std;
//
//class Robot {
//private:
//    string name;
//    char type;
//    int level;
//    int hp;
//    int attack;
//    int defense;
//
//    void updateAttr() {
//        hp = level * 5;
//        attack = level * 5;
//        defense = level * 5;
//
//        if (type == 'A') {
//            attack = level * 10;
//        }
//        else if (type == 'D') {
//            defense = level * 10;
//        }
//        else if (type == 'H') {
//            hp = level * 50;
//        }
//    }
//
//public:
//    Robot(string n, char t, int lv) {
//        name = n;
//        type = t;
//        level = lv;
//        updateAttr();
//    }
//
//    string getName() {
//        return name;
//    }
//
//    char getType() {
//        return type;
//    }
//
//    int getLevel() {
//        return level;
//    }
//
//    int getHp() {
//        return hp;
//    }
//
//    int getAttack() {
//        return attack;
//    }
//
//    int getDefense() {
//        return defense;
//    }
//
//    void setType(char t) {
//        type = t;
//        updateAttr();
//    }
//
//    void show() {
//        cout << name << "--" << type << "--" << level << "--"
//            << hp << "--" << attack << "--" << defense << endl;
//    }
//};
//
//bool transform(Robot* r, char newType) {
//    if (r->getType() == newType) {
//        return false;
//    }
//
//    r->setType(newType);
//    return true;
//}
//
//int main() {
//    int t;
//    cin >> t;
//
//    int transformCount = 0;
//
//    while (t--) {
//        string name;
//        char type, newType;
//        int level;
//
//        cin >> name >> type >> level;
//        cin >> newType;
//
//        Robot robot(name, type, level);
//
//        if (transform(&robot, newType)) {
//            transformCount++;
//        }
//
//        robot.show();
//    }
//
//    cout << "The number of robot transform is " << transformCount << endl;
//
//    return 0;
//}