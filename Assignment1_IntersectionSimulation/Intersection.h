
#ifndef INTERSECTION_H
#define INTERSECTION_H

#endif // INTERSECTION_H

#include <algorithm>
#include <chrono>
#include <deque>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>


#include <QMetaObject>
#include <QObject>
#include <QString>


enum is_emergency { NOT_E = 0, EMERGENCY }; //是否紧急

enum Road { //路中交叉部分标识(四个象限)
  NORTH_WEST = 0,
  SOUTH_WEST,
  SOUTH_EAST,
  NORTH_EAST
};
enum Direction { NORTH = 0, EAST, SOUTH, WEST }; //四个方向(顺时针枚举)

enum LightsDirection { NORTH_SOUTH = 0, EAST_WEST }; //信号灯方向

enum Color { GREEN = 0, YELLOW, RED }; //信号灯颜色

using namespace std;
using namespace std::chrono;

class TrafficLight { //信号灯类
public:
  TrafficLight() { color_ = RED; }
  ~TrafficLight() {}

  void SetColor(int color) { color_ = color; }
  //设置信号灯颜色

  bool IsColor(int color) const { return color_ == color; }
  //获取是否为指定颜色

private:
  int color_; //颜色
};

class Vehicle { //车辆类
public:
  Vehicle(int id, int direction, bool is_emergency = false) {
    id_ = id, direction_from_ = direction, is_emergency_ = is_emergency;
  }
  ~Vehicle() {}

  bool IsEmergency() const { return is_emergency_; }
  //获取是否为紧急车辆(救护车、消防车等)

  int GetID() const { return id_; }
  //获取车辆ID

  int GetDirectionFrom() const { return direction_from_; }
  //获取车辆进入路口方向

  int GetDirectionTo() const { return (direction_from_ + 2) % 4; }
  //获取车辆出去的方向(此处假设车辆均为直行)

private:
  int id_;
  int direction_from_;
  bool is_emergency_;
};

class Intersection { //十字路口类
  //为避免死锁，车辆行进出现冲突时采用南北优先的策略
public:
  mutex road_[4]; //路中交叉部分四个象限是否占用

  Intersection(int pass_time = 2, int timer = 8) {
    vehicle_id_cnt_ = 0;
    pass_time_ = pass_time;
    timer_ = timer;
    for (int i = NORTH; i <= WEST; ++i) {
      emergency_num_on_road[i] = 0;
    }
  }
  ~Intersection() {}

  void SetLightTimer(int seconds) { timer_ = seconds; }
  //设置信号灯时间(单位:秒)

  int GetLightTimer() const { return timer_; }
  //获取时间(单位:秒)

  int GetpassTime() const { return pass_time_; }
  //获取车穿过路口的时间(单位:秒)

  void AddVehicle(const int direction, const bool is_emergency = false);
  //增加车辆的函数

  void GoVehicle_later(const int ID, const int from, const bool is_emergency);
  //创建行车后半段进程的函数(每个车都创建一次)

  void GoVehicle(const int from);
  //用于创建四个方向普通车辆管理进程的函数

  void GoEmergency(const int from);
  //用于创建四个方向紧急车辆管理进程的函数

  void SetNorthSouth(const int color);
  void SetEastWest(const int color);
  //设置某方向车道的红绿灯

  void Tick();
  //创建切换路灯颜色的进程所用函数.此处时间绿灯：黄灯：红灯=3:1:4(总时长为16s)

  int GetVehicleNum(const int direction) const {
    return vehicle_queue_[direction].size();
  }
  int GetEmergencyNum(const int direction) const {
    return emergency_queue_[direction].size();
  }
  //获取正在等待的普通车辆数量或紧急车辆数量

  int GetEmergencyNumOnRoad(const int direction) const {
    return emergency_num_on_road[direction];
  }
  //获取正在路上指定方向行驶的紧急车辆数量

  bool EmergencyEmpty() const {
    return (emergency_queue_[NORTH].empty() &&
            emergency_queue_[SOUTH].empty() && emergency_queue_[EAST].empty() &&
            emergency_queue_[WEST].empty());
  }
  //返回是否有紧急车辆

  void GoThread();
  //创建进程

  using SignalType_GoVehicle = function<void(int, int, bool, bool)>;
  using SignalType_ChangeLight = function<void(int, int)>;

  void GoVehicleSignal(SignalType_GoVehicle signal);
  //向主进程发送更改ui界面的信号(车辆行驶)

  void LightSignal(SignalType_ChangeLight signal);
  //向主进程发送更改ui界面的信号(更改信号灯)

private:
  int timer_; //红灯与(绿灯加黄灯)的时间(单位：秒)
  int vehicle_id_cnt_;
  int pass_time_; //每辆车通过路口的时间
  int emergency_num_on_road
      [4]; //四个方向在路上直行紧急车辆(分别为N->S,E->W,S->N,W->E)
  TrafficLight lights_[2];
  queue<Vehicle> vehicle_queue_[4];
  queue<Vehicle> emergency_queue_[4];

  SignalType_GoVehicle
      go_signal_; //传输GoVehicle车辆方向与是否紧急的信号量.四个变量分别为:车辆id、方向、是否紧急、是前段还是后段(0为前端)
  SignalType_ChangeLight
      light_signal_; //传输Tick中更改信号灯的信号量.两个变量分别为:方向(0为南北,1为东西)、颜色
};
