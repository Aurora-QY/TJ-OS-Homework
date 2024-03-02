#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

enum Road { //路中交叉部分标识(四个象限)
  NORTH_EAST = 0,
  NORTH_WEST,
  SOUTH_WEST,
  SOUTH_EAST
};
enum Direction { NORTH = 0, EAST, SOUTH, WEST }; //四个方向

enum LightsDirection { NORTH_SOUTH = 0, EAST_WEST }; //信号灯方向

enum Color { GREEN = 0, YELLOW, RED }; //信号灯颜色

using namespace std;
using namespace std::chrono;

class TrafficLight { //信号灯类
public:
  TrafficLight(int timer = 8) {
    color_ = RED;
    timer_ = timer;
  }
  ~TrafficLight() {}

  void SetTimer(int seconds) { timer_ = seconds; }
  //设置信号灯时间

  void SetColor(int color) { color_ = color; }
  //设置信号灯颜色

  int GetTimer() { return timer_; }
  //获取时间

  bool IsColor(int color) const { return color_ == color; }
  //获取是否为指定颜色

private:
  int color_; //颜色
  int timer_; //红灯/绿灯加黄灯的时间(单位：秒)
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

  Intersection(int pass_time = 2) {
    vehicle_id_cnt_ = 0;
    pass_time_ = pass_time;
    for (int i = 0; i < 4; ++i) {
      vehicle_num_[i] = emergency_num_[i] = 0;
    }
  }
  ~Intersection() {}

  void AddVehicle(int direction, bool is_emergency = false) { //增加车辆的函数
    Vehicle vehicle(++vehicle_id_cnt_, direction, is_emergency);
    if (vehicle.IsEmergency()) {
      emergency_queue_[vehicle.GetDirectionFrom()].push(vehicle);
    } else {
      vehicle_queue_[vehicle.GetDirectionFrom()].push(vehicle);
    }
  }

  void GoVehicle_later(int from) {
    road_[(from + 1) % 4].lock();
    printf("%d is lock\n", (from + 1) % 4);
    //解锁上一个象限的资源后再锁上下一个象限的资源，例如从北边来经过西北象限的车此处抵达西南象限
    this_thread::sleep_for(milliseconds(pass_time_ * 500));
    road_[(from + 1) % 4].unlock();
    printf("%d is unlock\n", (from + 1) % 4);
    return;
  }

  void GoVehicle(int from) { //用于创建四个方向普通车辆管理进程的函数
    int dir = from % 2; // dir是车辆所在路的朝向(东西，南北)
    while (1) {
      if (!vehicle_queue_[from].empty() && lights_[dir].IsColor(GREEN)) {
        //判断条件:如果本方向队列非空，且左右方向的紧急队列不全非空(避免紧急车辆闯红灯导致死锁)，则可以占用路口并出发
        road_[from].lock();
        printf("%d is lock\n", from);
        //占用对应的十字路口交叉部分的资源，例如北边来的车就先占用西北象限
        Vehicle vehicle = vehicle_queue_[from].front();
        vehicle_queue_[from].pop();
        printf("%d is gone from %d to %d\n", vehicle.GetID(),
               vehicle.GetDirectionFrom(), vehicle.GetDirectionTo());
        this_thread::sleep_for(milliseconds(pass_time_ * 500));
        road_[from].unlock();
        printf("%d is unlock\n", from);
        new thread(&Intersection::GoVehicle_later, this, from);
      }
    }
  }

  void GoVehicle_Emergency(int from) { //用于创建四个方向紧急车辆管理进程的函数
    int to = (from + 2) % 4;
    while (1) {
      if (!emergency_queue_[from].empty()) {
        road_[from].lock();
        Vehicle vehicle = vehicle_queue_[from].front();
        vehicle_queue_[from].pop();
        printf("%d is gone from %d to %d\n", vehicle.GetID(),
               vehicle.GetDirectionFrom(), vehicle.GetDirectionTo());
        // go
        this_thread::sleep_for(milliseconds(pass_time_ * 500));
        road_[from].unlock();
        this_thread::sleep_for(milliseconds(pass_time_ * 500));
      }
    }
  }

  void SetNorthSouth(int color) { lights_[NORTH_SOUTH].SetColor(color); }
  void SetEastWest(int color) { lights_[EAST_WEST].SetColor(color); }

  void Tick() {
    //创建切换路灯颜色的进程所用函数此处时间绿灯：黄灯：红灯=3:1:4(总时长为16s)
    while (1) {
      SetEastWest(RED);
      SetNorthSouth(GREEN);
      printf("NS_lights is Green now\n");
      this_thread::sleep_for(milliseconds(lights_[NORTH].GetTimer() * 750));
      SetNorthSouth(YELLOW);
      printf("NS_lights is Yellow now\n");
      this_thread::sleep_for(milliseconds(lights_[NORTH].GetTimer() * 250));
      SetNorthSouth(RED);
      SetEastWest(GREEN);
      printf("EW_lights is Green now\n");
      this_thread::sleep_for(milliseconds(lights_[NORTH].GetTimer() * 750));
      SetEastWest(YELLOW);
      printf("EW_lights is Yellow now\n");
      this_thread::sleep_for(milliseconds(lights_[NORTH].GetTimer() * 250));
    }
  }

private:
  int vehicle_id_cnt_;
  int pass_time_;
  int vehicle_num_[4];
  int emergency_num_[4];
  TrafficLight lights_[2];
  queue<Vehicle> vehicle_queue_[4];
  queue<Vehicle> emergency_queue_[4];
};

int main() {
  Intersection IT;
  IT.AddVehicle(NORTH);
  IT.AddVehicle(NORTH);
  IT.AddVehicle(NORTH);
  IT.AddVehicle(NORTH);
  IT.AddVehicle(NORTH);
  IT.AddVehicle(SOUTH);
  IT.AddVehicle(EAST);
  IT.AddVehicle(WEST);
  thread Tick(&Intersection::Tick, &IT);
  thread Road_N(&Intersection::GoVehicle, &IT, NORTH);
  thread Road_E(&Intersection::GoVehicle, &IT, EAST);
  thread Road_S(&Intersection::GoVehicle, &IT, SOUTH);
  thread Road_W(&Intersection::GoVehicle, &IT, WEST);
  Tick.join();
  Road_N.join();
  Road_E.join();
  Road_S.join();
  Road_W.join();
  return 0;
}
