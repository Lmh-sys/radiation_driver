#include "radiation.hpp"

using namespace std;
using namespace boost::asio;
//串口相关对象
boost::asio::io_service iosev;
boost::asio::serial_port sp(iosev, "/dev/ttyUSB0");
boost::system::error_code err;
/********************************************************
            串口发送控制命令
********************************************************/
const unsigned char header[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0X02, 0xC4, 0x0B};

bool radiation::radiation_init()//初始化函数
{
    //串口参数初始化
    sp.set_option(serial_port::baud_rate(19200));
    sp.set_option(serial_port::flow_control(serial_port::flow_control::none));
    sp.set_option(serial_port::parity(serial_port::parity::none));
    sp.set_option(serial_port::stop_bits(serial_port::stop_bits::one));
    sp.set_option(serial_port::character_size(8));

    pub_ = nh.advertise<geometry_msgs::PointStamped>("radiation/data", 50);
    return true;
}
/********************************************************
函数功能：获得8位循环冗余校验值
入口参数：数组地址、长度
出口参数：校验值
********************************************************/
uint16_t radiation::getCrc16(unsigned char *ptr)
{
    uint16_t CRCV = 0xFFFF;
    unsigned char iB, i, j;
    for(i = 0;i < 7; i++)
    {
        CRCV ^= *ptr++;
        for(j = 0; j < 8 ;j++)
        {
            iB = (unsigned char)(CRCV & 0X0001);
            CRCV = (uint16_t)(CRCV >> 1);
            if(iB == 1)
            {
                CRCV = (uint16_t)(CRCV ^ 0XA001);
            }
        }
    }
    return CRCV;
}
/********************************************************
函数功能：下发控制命令
入口参数：
出口参数：
********************************************************/
void radiation::writeContral()
{
    static unsigned char buf[8] = {0}; //

    // 控制消息
    for (int i = 0; i < 8; i++)
    {
        buf[i] = header[i];
        //ROS_INFO("%02x",buf[i]); 
    }

    // 通过串口下发数据
    boost::asio::write(sp, boost::asio::buffer(buf));
}

/********************************************************
函数功能：读取数据，解析出辐射值
入口参数：
出口参数：bool
********************************************************/
bool radiation::readradiation()
{
    int receive_data;//接受数据

    uint16_t check;//CRC检验值
    unsigned char buf[150] = {0};//接受的缓存区
    
    unsigned char low_byte;
    unsigned char high_byte;
    //=========================================================
    //此段代码可以读数据的结尾，进而来进行读取数据的头部
    try
    {
        boost::asio::streambuf response;
        // ROS_INFO("ready to read");
        // boost::asio::read_until(sp, response, "\r\n", err);//\r\n
        boost::asio::read( sp, boost::asio::buffer( buf , 9 ) );
        // ROS_INFO("read successful");
        
        // copy(istream_iterator<unsigned char>(istream(&response) >> noskipws),
        //      istream_iterator<unsigned char>(),
        //      buf);
        
        // for(int i = 0; i < 11 ; i++)
        //     ROS_INFO("%02x",buf[i]);
    }
    catch (boost::system::system_error &err)
    {
        ROS_INFO("read_until error");
    }
    //=========================================================
    ROS_INFO("check!");
    // 检查信息头
    if (buf[0] != header[0] || buf[1] != header[1]) //buf[0] buf[1]
    {
        ROS_ERROR("Received message header error!");
        return false;
    }
    // ROS_INFO("CRC校验");
    // 检查信息校验值
    check = getCrc16(buf); //getCrc16计算得出
    // ROS_INFO("%02X",check);
    low_byte = (check >> 0)& 0x00ff ;
    //ROS_INFO("%02X",low_byte);
    high_byte = (check >> 8)& 0x00ff ;
    //ROS_INFO("%02X",high_byte);
    if (low_byte != buf[7] || high_byte != buf[8])     //接受的低字节放在高位，高字节放在底位
    {
        ROS_ERROR("Received data check sum error!");
        return false;
    }
    // ROS_INFO("CRC校验成功！");

    //ROS_INFO("开始计算辐射值！");
    // 读取辐射值   -----------
    btf.bytes[0] = buf[6]; // 这些值只是示例
    btf.bytes[1] = buf[5];
    btf.bytes[2] = buf[4];
    btf.bytes[3] = buf[3];
    // btf.bytes[0] = 0xCD; // 0.20uS/v
    // btf.bytes[1] = 0xCC;
    // btf.bytes[2] = 0x4C;
    // btf.bytes[3] = 0x3E;
    // receive_data = (buf[5] << 24) + (buf[6] << 16) + (buf[7] << 8) + (buf[8]);
    receive_data = btf.f;
    //ROS_INFO("%d",receive_data);
    final_data.header.stamp = ros::Time::now();
    final_data.point.x = btf.f;
    ROS_INFO("辐射值%f",btf.f);
    
    // 辐射数据发布
    pub_.publish(final_data);
    return true;
}