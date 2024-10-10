#ifndef RADIATION_DRIVER_H
#define RADIATION_DRIVER_H

#include <ros/ros.h>
#include <ros/time.h>

#include <boost/asio.hpp>
#include <geometry_msgs/PointStamped.h>
#include <std_msgs/Float64.h>
union BytesToFloat {
    float f;
    unsigned char bytes[4];
};
class radiation
{
    private:
        ros::NodeHandle nh;
        ros::Publisher pub_;
        //std_msgs::Float64 final_data;
        geometry_msgs::PointStamped final_data;
        union BytesToFloat btf;
    public:
        radiation(){}//构造函数
        virtual ~radiation(){}//虚析构函数

        bool radiation_init();//初始化函数
        void writeContral();//发送控制命令
        bool readradiation();//读取辐射数据
        uint16_t getCrc16(unsigned char *ptr);//CRC校验
};







#endif