/**
 * @file try.cpp
 * @author Pulkit Mehta (pmehta09@umd.edu)
 * @brief Node for RWA1
 * @version 0.1
 * @date 2022-02-28
 * 
 * @copyright Copyright (c) 2022
 * 
 */

// Copyright 2016 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include <algorithm>
#include <vector>
#include <string>
#include <ros/ros.h>

#include <nist_gear/LogicalCameraImage.h>
#include <nist_gear/Order.h>
#include <nist_gear/Proximity.h>
#include <sensor_msgs/JointState.h>
#include <sensor_msgs/LaserScan.h>
#include <sensor_msgs/PointCloud.h>
#include <sensor_msgs/Range.h>
#include <std_msgs/Float32.h>
#include <std_msgs/String.h>
#include <std_srvs/Trigger.h>
#include <trajectory_msgs/JointTrajectory.h>
#include <nist_gear/DetectedProduct.h>
#include <nist_gear/SubmitShipment.h>
#include <nist_gear/AGVToAssemblyStation.h>
#include <nist_gear/AssemblyStationSubmitShipment.h>


/**
 * @brief Structure of a product
 * 
 */
typedef struct Product
{
    std::string type;
    geometry_msgs::Pose frame_pose;
}
product;



/**
 * @brief Structure of a kitting shipment
 * 
 */
typedef struct Kitting
{
    std::string shipment_type;
    std::string agv_id;
    std::string station_id;
    std::vector<Product> products;
}
kitting;

/**
 * @brief Structure of an assembly shipment
 * 
 */
typedef struct Assembly
{
    std::string shipment_type;
    std::string stations;
    std::vector<Product> products;
}
assembly;

/**
 * @brief Structure of an order
 * 
 */
typedef struct Order
{   std::string order_id;
    unsigned short int priority;
    unsigned short int kitting_robot_health;
    unsigned short int assembly_robot_health;
    std::string announcement_condition;
    double announcement_condition_value;
    std::vector<Kitting> kitting;
    std::vector<Assembly> assembly;
}
order;

class Agv 
{
  public:
    explicit Agv(ros::NodeHandle & node){
        client1 = node.serviceClient<nist_gear::AGVToAssemblyStation>("/ariac/agv1/submit_shipment");
        client2 = node.serviceClient<nist_gear::AGVToAssemblyStation>("/ariac/agv2/submit_shipment");
        client3 = node.serviceClient<nist_gear::AGVToAssemblyStation>("/ariac/agv3/submit_shipment");
        client4 = node.serviceClient<nist_gear::AGVToAssemblyStation>("/ariac/agv4/submit_shipment");
        

        // Subscribe to the '/ariac/agv1/state' topic.
        agv1_state_subscriber = node.subscribe("/ariac/agv1/state", 10, &Agv::agv1_state_callback, this);

        // Subscribe to the '/ariac/agv2/state' topic.
        agv2_state_subscriber = node.subscribe("/ariac/agv2/state", 10,&Agv::agv2_state_callback, this);

        // Subscribe to the '/ariac/agv3/state' topic.
        agv3_state_subscriber = node.subscribe("/ariac/agv3/state", 10, &Agv::agv3_state_callback, this);

        // Subscribe to the '/ariac/agv4/state' topic.
        agv4_state_subscriber = node.subscribe("/ariac/agv4/state", 10, &Agv::agv4_state_callback, this);    
    }

    /// Called when a new LogicalCameraImage message is received.
    void agv1_state_callback(const std_msgs::String::ConstPtr & msg)
    {
        msg->data;
        // ROS_INFO_STREAM("Callback triggered for Topic /ariac/agv1/state");
    }

    void agv2_state_callback(const std_msgs::String::ConstPtr & msg)
    {
        msg->data;
        // ROS_INFO_STREAM("Callback triggered for Topic /ariac/agv2/state");
    }

    void agv3_state_callback(const std_msgs::String::ConstPtr & msg)
    {
        msg->data;
        // ROS_INFO_STREAM("Callback triggered for Topic /ariac/agv3/state");
    }

    void agv4_state_callback(const std_msgs::String::ConstPtr & msg)
    {
        msg->data;
        // ROS_INFO_STREAM("Callback triggered for Topic /ariac/agv4/state");
    }

    void submit_shipment(std::string agv_id_){
        nist_gear::AGVToAssemblyStation srv;

        if (agv_id_ == "agv1"){
            client1.call(srv);
        }
        if (agv_id_ == "agv2"){
            client2.call(srv);
        }
        if (agv_id_ == "agv3"){
            client3.call(srv);
        }
        if (agv_id_ == "agv4"){
            client4.call(srv);
        }

        
    }

    std::string get_agv_id(){
        return agv_id_;
    }
 
  private:
    std::string station;
    std::string shipment_id;
    ros::Subscriber agv1_state_subscriber;
    ros::Subscriber agv2_state_subscriber;
    ros::Subscriber agv3_state_subscriber;
    ros::Subscriber agv4_state_subscriber;
    ros::ServiceClient client1;
    ros::ServiceClient client2;
    ros::ServiceClient client3;
    ros::ServiceClient client4;
    std::string agv_id_;
};


/**
 * @brief Start the competition by waiting for and then calling the start ROS Service.
 * 
 * @param node Nodehandle
 */
void start_competition(ros::NodeHandle & node)
{
  // create a Service client for the correct service, i.e. '/ariac/start_competition'.
  ros::ServiceClient start_client =
    node.serviceClient<std_srvs::Trigger>("/ariac/start_competition");
  // if it's not already ready, wait for it to be ready.
  // calling the Service using the client before the server is ready would fail.
  if (!start_client.exists())
  {
    ROS_INFO("Waiting for the competition to be ready...");
    start_client.waitForExistence();
    ROS_INFO("Competition is now ready.");
  }
  ROS_INFO("Requesting competition start...");
  std_srvs::Trigger srv;  // combination of the "request" and the "response".
  start_client.call(srv);  // call the start Service.
  // if not successful, print out why.
  if (!srv.response.success)
  {
    ROS_ERROR_STREAM("Failed to start the competition: " << srv.response.message);
  }
  else
  {
    ROS_INFO("Competition started!");
  }
}



void as_submit_assembly(ros::NodeHandle & node, std::string as)
{
  ros::ServiceClient client1 = node.serviceClient<nist_gear::AssemblyStationSubmitShipment>("/ariac/as1/submit_shipment");
  ros::ServiceClient client2 = node.serviceClient<nist_gear::AssemblyStationSubmitShipment>("/ariac/as2/submit_shipment");
  ros::ServiceClient client3 = node.serviceClient<nist_gear::AssemblyStationSubmitShipment>("/ariac/as3/submit_shipment");
  ros::ServiceClient client4 = node.serviceClient<nist_gear::AssemblyStationSubmitShipment>("/ariac/as4/submit_shipment");
   
  nist_gear::AssemblyStationSubmitShipment srv;
  
  if(as == "as1"){
    client1.call(srv);
  }
  if(as == "as2"){
    client2.call(srv);
  }
  if(as == "as3"){
    client3.call(srv);
  }
  if(as == "as4"){
    client4.call(srv);
}
  
}

void end_competition(ros::NodeHandle & node)
{
  ros::ServiceClient end_client = node.serviceClient<std_srvs::Trigger>("/ariac/end_competition");

  std_srvs::Trigger srv;
  end_client.call(srv);
  if (!srv.response.success)
  {
    ROS_ERROR_STREAM("Failed to end the competition: " << srv.response.message);
  }
  else
  {
    ROS_INFO("Competition ended!");
  }
  ros::shutdown();
}

/**
 * @brief Example class that can hold state and provide methods that handle incoming data.
 * 
 */
class MyCompetitionClass
{
public:
  explicit MyCompetitionClass(ros::NodeHandle & node)
  : current_score_(0)
  {
    gantry_arm_joint_trajectory_publisher_ = node.advertise<trajectory_msgs::JointTrajectory>(
      "/ariac/arm1/arm/command", 10);

    kitting_arm_joint_trajectory_publisher_ = node.advertise<trajectory_msgs::JointTrajectory>(
      "/ariac/arm2/arm/command", 10);
  }

  /// Called when a new message is received.
  void current_score_callback(const std_msgs::Float32::ConstPtr & msg)
  {
    if (msg->data != current_score_)
    {
      ROS_INFO_STREAM("Score: " << msg->data);
    }
    current_score_ = msg->data;
  }

  /// Called when a new message is received.
  void competition_state_callback(const std_msgs::String::ConstPtr & msg)
  {
    if (msg->data == "done" && competition_state_ != "done")
    {
      ROS_INFO("Competition ended.");
    }
    competition_state_ = msg->data;

  }

  std::string getCompetitionState(){
      return competition_state_;
  }  

  /// Called when a new Order message is received.
  void order_callback(const nist_gear::Order::ConstPtr & order_msg)
  {
       
    ROS_INFO_STREAM("Received order:\n" << *order_msg);
    
    received_orders_.push_back(*order_msg);
    
    ROS_INFO_STREAM("" << received_orders_.at(0).order_id);
    Order new_order;
    new_order.order_id = order_msg->order_id;

    for (const auto &kit: order_msg->kitting_shipments){
        Kitting new_kitting;
        new_kitting.agv_id = kit.agv_id;
        new_kitting.shipment_type = kit.shipment_type;
        new_kitting.station_id = kit.station_id;
        for (const auto &Prod: kit.products){
            Product new_kproduct;
            new_kproduct.type = Prod.type;
            new_kproduct.frame_pose = Prod.pose;
            new_kitting.products.push_back(new_kproduct);
        }
        new_order.kitting.push_back(new_kitting);
    }

    for (const auto &asmb: order_msg->assembly_shipments){
        Assembly new_assembly;
        new_assembly.shipment_type = asmb.shipment_type;
        new_assembly.stations = asmb.station_id;
        for (const auto &Prod: asmb.products){
            Product new_aproduct;
            new_aproduct.type = Prod.type;
            new_aproduct.frame_pose = Prod.pose;
            new_assembly.products.push_back(new_aproduct);
        }
        new_order.assembly.push_back(new_assembly);
    }
   
    order_list_.push_back(new_order);
    
  }

  void process_order(){
      auto current_order = order_list_.front();
      auto current_kitting = current_order.kitting.front();
      auto kproduct_list = current_kitting.products;
      auto current_assembly = current_order.assembly.front();
      auto aproduct_list = current_assembly.products;

      for (const auto &kp: kproduct_list){
          kproduct_list.push_back(kp);
      }
      kproduct_list_ = kproduct_list;

      for (const auto &ap: aproduct_list){
          aproduct_list.push_back(ap);
      }
      aproduct_list_ = aproduct_list;
  }

  std::vector<Order> get_order_list(){
      return order_list_;
  }

  std::vector<Product> get_product_list(){
      return kproduct_list_, aproduct_list_;
  }


  /// Called when a new LogicalCameraImage message from /ariac/logical_camera_station1 is received.
  void logical_camera_station1_callback(
    const nist_gear::LogicalCameraImage::ConstPtr & image_msg)
  {
    ROS_INFO_STREAM_THROTTLE(10,
      "Logical camera station 1: '" << image_msg->models.size() << "' objects.");
  }

  /// Called when a new LogicalCameraImage message from /ariac/logical_camera_station2 is received.
  void logical_camera_station2_callback(
    const nist_gear::LogicalCameraImage::ConstPtr & image_msg)
  {
    ROS_INFO_STREAM_THROTTLE(10,
      "Logical camera station 2: '" << image_msg->models.size() << "' objects.");
  }

  /// Called when a new LogicalCameraImage message from /ariac/logical_camera_station3 is received.
  void logical_camera_station3_callback(
    const nist_gear::LogicalCameraImage::ConstPtr & image_msg)
  {
    ROS_INFO_STREAM_THROTTLE(10,
      "Logical camera station 3: '" << image_msg->models.size() << "' objects.");
  }

  /// Called when a new LogicalCameraImage message from /ariac/logical_camera_station4 is received.
  void logical_camera_station4_callback(
    const nist_gear::LogicalCameraImage::ConstPtr & image_msg)
  {
    ROS_INFO_STREAM_THROTTLE(10,
      "Logical camera station 4: '" << image_msg->models.size() << "' objects.");
  }

  /// Called when a new LogicalCameraImage message from /ariac/logical_camera_bins8 is received.
  void logical_camera_bins8_callback(
    const nist_gear::LogicalCameraImage::ConstPtr & image_msg)
  {
    ROS_INFO_STREAM_THROTTLE(10,
      "Logical camera bins8: '" << image_msg->models.size() << "' objects.");
  }

  /// Called when a new Proximity message is received.
  void breakbeam0_callback(const nist_gear::Proximity::ConstPtr & msg) {
    if (msg->object_detected) {  // If there is an object in proximity.
      ROS_INFO("Break beam triggered.");
    }
  }

  /// Called when a new LogicalCameraImage message from /ariac/quality_control_sensor1 is received.
  void quality_control_sensor1_callback(
    const nist_gear::LogicalCameraImage::ConstPtr & image_msg)
  {
    if (!image_msg->models.empty()){
      ROS_INFO_STREAM_THROTTLE(10,"Faulty part detected by Quality sensor 1");
    }
    // ROS_INFO_STREAM("Callback triggered for Topic /ariac/quality_control_sensor_1");
    
  }

  /// Called when a new LogicalCameraImage message from /ariac/quality_control_sensor2 is received.
  void quality_control_sensor2_callback(
    const nist_gear::LogicalCameraImage::ConstPtr & image_msg)
  {
    if (!image_msg->models.empty()){
      ROS_INFO_STREAM_THROTTLE(10,"Faulty part detected by Quality sensor 2");
    }
    // ROS_INFO_STREAM("Callback triggered for Topic /ariac/quality_control_sensor_2");
    
  }

  /// Called when a new LogicalCameraImage message from /ariac/quality_control_sensor3 is received.
  void quality_control_sensor3_callback(
    const nist_gear::LogicalCameraImage::ConstPtr & image_msg)
  {
    if (!image_msg->models.empty()){
      ROS_INFO_STREAM_THROTTLE(10,"Faulty part detected by Quality sensor 3");
    }
    // ROS_INFO_STREAM("Callback triggered for Topic /ariac/quality_control_sensor_3");

  }

  /// Called when a new LogicalCameraImage message from /ariac/quality_control_sensor4 is received.
  void quality_control_sensor4_callback(
    const nist_gear::LogicalCameraImage::ConstPtr & image_msg)
  {
    if (!image_msg->models.empty()){
      ROS_INFO_STREAM_THROTTLE(10,"Faulty part detected by Quality sensor 4");
    }
    // ROS_INFO_STREAM("Callback triggered for Topic /ariac/quality_control_sensor_4");
  }

  void depth_camera_bins1_callback(
    const sensor_msgs::PointCloud::ConstPtr & pc_msg)
    {

    }

    void agv1_station_callback(const std_msgs::String::ConstPtr & msg)
  {
    msg->data;
    // ROS_INFO_STREAM("Callback triggered for Topic /ariac/agv1/station");
  }

  void agv2_station_callback(const std_msgs::String::ConstPtr & msg)
  {
    msg->data;
    // ROS_INFO_STREAM("Callback triggered for Topic /ariac/agv2/station");
  }

  void agv3_station_callback(const std_msgs::String::ConstPtr & msg)
  {
    msg->data;
    // ROS_INFO_STREAM("Callback triggered for Topic /ariac/agv3/station");
  }

  void agv4_station_callback(const std_msgs::String::ConstPtr & msg)
  {
    msg->data;
    // ROS_INFO_STREAM("Callback triggered for Topic /ariac/agv4/station");
  }

  kitting kitt;
  assembly asmb;

private:
  std::string competition_state_;
  double current_score_;
  ros::Publisher gantry_arm_joint_trajectory_publisher_;
  ros::Publisher kitting_arm_joint_trajectory_publisher_;
  std::vector<nist_gear::Order> received_orders_;
  sensor_msgs::JointState gantry_arm_current_joint_states_;
  sensor_msgs::JointState kitting_arm_current_joint_states_;
  std::vector<Order> order_list_;
  std::vector<Product> kproduct_list_;
  std::vector<Product> aproduct_list_;

};

void proximity_sensor_callback(const sensor_msgs::Range::ConstPtr & msg)
{
  if ((msg->max_range - msg->range) > 0.01)
  {  // If there is an object in proximity.
    ROS_INFO_THROTTLE(1, "Proximity sensor sees something.");
  }
}

void laser_profiler_callback(const sensor_msgs::LaserScan::ConstPtr & msg)
{
  size_t number_of_valid_ranges = std::count_if(
    msg->ranges.begin(), msg->ranges.end(), [](const float f)
    {
      return std::isfinite(f);
      });
  if (number_of_valid_ranges > 0)
  {
    ROS_INFO_THROTTLE(1, "Laser profiler sees something.");
  }
}


int main(int argc, char ** argv)
{
  // Last argument is the default name of the node.
  ros::init(argc, argv, "ariac_example_node");

  ros::NodeHandle node;

  // Instance of custom class from above.
  MyCompetitionClass comp_class(node);


  // Subscribe to the '/ariac/current_score' topic.
  ros::Subscriber current_score_subscriber = node.subscribe(
    "/ariac/current_score", 10,
    &MyCompetitionClass::current_score_callback, &comp_class);

  // Subscribe to the '/ariac/competition_state' topic.
  ros::Subscriber competition_state_subscriber = node.subscribe(
    "/ariac/competition_state", 10,
    &MyCompetitionClass::competition_state_callback, &comp_class);

  // %Tag(SUB_CLASS)%
  // Subscribe to the '/ariac/orders' topic.
  ros::Subscriber orders_subscriber = node.subscribe(
    "/ariac/orders", 10,
    &MyCompetitionClass::order_callback, &comp_class);

  // Subscribe to the '/ariac/proximity_sensor_1' topic.
  ros::Subscriber proximity_sensor_subscriber = node.subscribe(
    "/ariac/proximity_sensor_1", 10, proximity_sensor_callback);
  // %EndTag(SUB_FUNC)%

  // Subscribe to the '/ariac/break_beam0' topic.
  ros::Subscriber break_beam0_subscriber = node.subscribe(
    "/ariac/breakbeam_0", 10,
    &MyCompetitionClass::breakbeam0_callback, &comp_class);
  
  // Subscribe to the '/ariac/logical_camera_bins8' topic.
  ros::Subscriber logical_camera_bins8_subscriber = node.subscribe(
    "/ariac/logical_camera_station1", 10,
    &MyCompetitionClass::logical_camera_bins8_callback, &comp_class);

  // Subscribe to the '/ariac/logical_camera_station1' topic.
  ros::Subscriber logical_camera_station1_subscriber = node.subscribe(
    "/ariac/logical_camera_station1", 10,
    &MyCompetitionClass::logical_camera_station1_callback, &comp_class);

  // Subscribe to the '/ariac/logical_camera_station2' topic.
  ros::Subscriber logical_camera_station2_subscriber = node.subscribe(
    "/ariac/logical_camera_station2", 10,
    &MyCompetitionClass::logical_camera_station2_callback, &comp_class);

  // Subscribe to the '/ariac/logical_camera_station3' topic.
  ros::Subscriber logical_camera_station3_subscriber = node.subscribe(
    "/ariac/logical_camera_station3", 10,
    &MyCompetitionClass::logical_camera_station3_callback, &comp_class);

  // Subscribe to the '/ariac/logical_camera_station4' topic.
  ros::Subscriber logical_camera_station4_subscriber = node.subscribe(
    "/ariac/logical_camera_station4", 10,
    &MyCompetitionClass::logical_camera_station4_callback, &comp_class);

  // Subscribe to the '/ariac/laser_profiler_1' topic.
  ros::Subscriber laser_profiler_subscriber = node.subscribe(
    "/ariac/laser_profiler_1", 10, laser_profiler_callback);

  // Subscribe to the '/ariac/quality_control_sensor_1' topic.
  ros::Subscriber quality_control_sensor1_subscriber = node.subscribe(
    "/ariac/quality_control_sensor_1", 10,
    &MyCompetitionClass::quality_control_sensor1_callback, &comp_class);

  // Subscribe to the '/ariac/quality_control_sensor_2' topic.
  ros::Subscriber quality_control_sensor2_subscriber = node.subscribe(
    "/ariac/quality_control_sensor_2", 10,
    &MyCompetitionClass::quality_control_sensor2_callback, &comp_class);

  // Subscribe to the '/ariac/quality_control_sensor_3' topic.
  ros::Subscriber quality_control_sensor3_subscriber = node.subscribe(
    "/ariac/quality_control_sensor_3", 10,
    &MyCompetitionClass::quality_control_sensor3_callback, &comp_class);

  // Subscribe to the '/ariac/quality_control_sensor_4' topic.
  ros::Subscriber quality_control_sensor4_subscriber = node.subscribe(
    "/ariac/quality_control_sensor_4", 10,
    &MyCompetitionClass::quality_control_sensor4_callback, &comp_class);   

  // Subscribe to the '/ariac/agv1/station' topic.
  ros::Subscriber agv1_station_subscriber = node.subscribe(
    "/ariac/agv1/station", 10,
    &MyCompetitionClass::agv1_station_callback, &comp_class);

  // Subscribe to the '/ariac/agv2/station' topic.
  ros::Subscriber agv2_station_subscriber = node.subscribe(
    "/ariac/agv2/station", 10,
    &MyCompetitionClass::agv2_station_callback, &comp_class);

  // Subscribe to the '/ariac/agv3/station' topic.
  ros::Subscriber agv3_station_subscriber = node.subscribe(
    "/ariac/agv3/station", 10,
    &MyCompetitionClass::agv3_station_callback, &comp_class);

  // Subscribe to the '/ariac/agv4/station' topic.
  ros::Subscriber agv4_station_subscriber = node.subscribe(
    "/ariac/agv4/station", 10,
    &MyCompetitionClass::agv4_station_callback, &comp_class);

  ROS_INFO("Setup complete.");
  start_competition(node);
  Agv agv(node);
  std::string state = comp_class.getCompetitionState();


  // ros::Duration(10);
  std::vector<Order> orders;
  std::vector<Kitting> kittings;
  std::string agv_id;
  
  do{
  orders = comp_class.get_order_list();
  }while(orders.size() == 0);

  kittings = orders.at(0).kitting;
  agv_id = kittings.at(0).agv_id;

  ROS_INFO_STREAM("AGV ID : " << agv_id);

  // ROS_INFO_STREAM("Current order is: " << orders.at(0).order_id);
  
  // Order cur_order = orders.at(0);
  // Kitting cur_kit = cur_order.kitting.at(0);
  // agv.submit_shipment(cur_kit.agv_id);
  // std::cout << cur_kit.agv_id << '\n';  

  ros::spinOnce();  // This executes callbacks on new data until ctrl-c.

  return 0;
}