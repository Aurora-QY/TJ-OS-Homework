
#include "Intersection.h"

//增加车辆的函数
void Intersection::AddVehicle(const int direction, const bool is_emergency) {
  Vehicle vehicle(++vehicle_id_cnt_, direction, is_emergency);
  if (vehicle.IsEmergency()) {
    emergency_queue_[vehicle.GetDirectionFrom()].push(vehicle);
  } else {
    vehicle_queue_[vehicle.GetDirectionFrom()].push(vehicle);
  }
}

//创建行车后半段进程的函数(每个车都创建一次)
void Intersection::GoVehicle_later(const int ID, const int from,
                                   const bool is_emergency) {
  while (1) { //一直尝试
    if (road_[(from + 1) % 4].try_lock()) {
      road_[from].unlock();
      //解锁上一个象限的资源后再锁上下一个象限的资源，例如从北边来经过西北象限的车此处抵达西南象限
      go_signal_(ID, from, NOT_E, 0);
      this_thread::sleep_for(milliseconds(pass_time_ * 500));
      road_[(from + 1) % 4].unlock();
      if (is_emergency) {
        emergency_num_on_road[from]--;
      }
      return; //最后结束线程
    }
  }
}

//用于创建四个方向普通车辆管理进程的函数
void Intersection::GoVehicle(const int from) {
  int dir = from % 2; // dir是车辆所在路的朝向(南北,东西)
  while (1) {         //一直尝试
    if (!vehicle_queue_[from].empty() && lights_[dir].IsColor(GREEN) &&
        emergency_queue_[from].empty() &&
        emergency_queue_[(from + 1) % 4].empty() &&
        emergency_queue_[(from + 3) % 4].empty() &&
        emergency_num_on_road[(from + 1) % 4] == 0 &&
        emergency_num_on_road[(from + 3) % 4] == 0 && road_[from].try_lock()) {
      //队列不空、绿灯、除对面方向外紧急车辆等待队列为空、左右方向无紧急车辆在路口行驶、路口可用
      //占用对应的十字路口交叉部分的资源，例如北边来的车就先占用西北象限

      //特别注意：占用互斥量应该放在最后一个判断！！！

      Vehicle vehicle = vehicle_queue_[from].front();
      vehicle_queue_[from].pop();

      go_signal_(vehicle.GetID(), from, NOT_E, 1);
      //发送信号

      //此处还不能解锁，要到GoVehicle_later的线程中要到下一个象限的占用权才能解锁这个象限
      this_thread::sleep_for(milliseconds(pass_time_ * 500));
      new thread(&Intersection::GoVehicle_later, this, vehicle.GetID(), from,
                 vehicle.IsEmergency());
    }
  }
}

//用于创建四个方向紧急车辆管理进程的函数
void Intersection::GoEmergency(const int from) {
  int dir = from % 2; // dir是车辆所在路的朝向(南北,东西)
  while (1) {         //为了避免死锁，设置为南北优先
    if (!emergency_queue_[from].empty() &&
        (dir == NORTH_SOUTH ||
         (dir == EAST_WEST && emergency_queue_[NORTH].empty() &&
          emergency_queue_[SOUTH].empty() &&
          emergency_num_on_road[NORTH] == 0 &&
          emergency_num_on_road[SOUTH] == 0)) &&
        road_[from].try_lock()) {
      //队列不空且路口可用
      Vehicle vehicle = emergency_queue_[from].front();
      emergency_queue_[from].pop();

      go_signal_(vehicle.GetID(), from, EMERGENCY, 1);
      //发送信号

      emergency_num_on_road[from]++;
      this_thread::sleep_for(milliseconds(pass_time_ * 500));
      new thread(&Intersection::GoVehicle_later, this, vehicle.GetID(), from,
                 vehicle.IsEmergency());
    }
  }
}

//设置某方向车道的红绿灯
void Intersection::SetNorthSouth(const int color) {
  lights_[NORTH_SOUTH].SetColor(color);
}
void Intersection::SetEastWest(const int color) {
  lights_[EAST_WEST].SetColor(color);
}

//创建切换路灯颜色的进程所用函数.此处时间绿灯：黄灯：红灯=3:1:4(总时长为16s)
void Intersection::Tick() {
  while (1) {
    SetEastWest(RED);
    light_signal_(1, RED);
    SetNorthSouth(GREEN);
    light_signal_(0, GREEN);
    this_thread::sleep_for(milliseconds(timer_ * 750));
    SetNorthSouth(YELLOW);
    light_signal_(0, YELLOW);
    this_thread::sleep_for(milliseconds(timer_ * 250));
    SetNorthSouth(RED);
    light_signal_(0, RED);
    SetEastWest(GREEN);
    light_signal_(1, GREEN);
    this_thread::sleep_for(milliseconds(timer_ * 750));
    SetEastWest(YELLOW);
    light_signal_(1, YELLOW);
    this_thread::sleep_for(milliseconds(timer_ * 250));
  }
}

//创建进程
void Intersection::GoThread() {
  thread Tick(&Intersection::Tick, this);
  thread Road_N(&Intersection::GoVehicle, this, NORTH);
  thread Road_E(&Intersection::GoVehicle, this, EAST);
  thread Road_S(&Intersection::GoVehicle, this, SOUTH);
  thread Road_W(&Intersection::GoVehicle, this, WEST);
  thread Road_N_Em(&Intersection::GoEmergency, this, NORTH);
  thread Road_E_Em(&Intersection::GoEmergency, this, EAST);
  thread Road_S_Em(&Intersection::GoEmergency, this, SOUTH);
  thread Road_W_Em(&Intersection::GoEmergency, this, WEST);
  Tick.detach();
  Road_N.detach();
  Road_E.detach();
  Road_S.detach();
  Road_W.detach();
  Road_N_Em.detach();
  Road_E_Em.detach();
  Road_S_Em.detach();
  Road_W_Em.detach();
}

//向主进程发送更改ui界面的信号(车辆行驶)
void Intersection::GoVehicleSignal(SignalType_GoVehicle signal) {
  go_signal_ = signal;
}

//向主进程发送更改ui界面的信号(更改信号灯)
void Intersection::LightSignal(SignalType_ChangeLight signal) {
  light_signal_ = signal;
}
