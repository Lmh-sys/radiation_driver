# radiation_driver
Driving package of WNB-8301G nuclear radiation detector based on ROS 
* 遇到的问题：
    - 硬件安装，需要小心安装
    - UT-890A驱动未安装
    ```
    # 驱动文件地址
    https://www.szutek.com/pro_view-79.html
    ```
    ![UT-890A驱动](pic/UT-890A驱动.png)
    - 驱动设置开机自启
    ```
    cd /etc/modules-load.d/
    sudo gedet modules.conf
    # 在文件最后加上：
    pl2303
    ```
    - 参考博客
    ```shell
    sudo chmod 777 /dev/ttyUSB0
    rosrun radiation_driver radaation_driver
    https://blog.csdn.net/xx970829/article/details/115678890
    https://blog.csdn.net/m0_38144614/article/details/116614056


* 成功读取辐射数据
```
[ INFO] [1679732570.480520406]: radiation_driver initialized successful.
[ INFO] [1679732570.702055392]: read successful
[ INFO] [1679732570.702096880]: check!
[ INFO] [1679732570.702123530]: CRC校验成功！
[ INFO] [1679732570.702131370]: 辐射值0.170000
[ INFO] [1679732570.921066355]: read successful
[ INFO] [1679732570.921115577]: check!
[ INFO] [1679732570.921123800]: CRC校验成功！
[ INFO] [1679732570.921131107]: 辐射值0.170000
[ INFO] [1679732571.138903773]: read successful
[ INFO] [1679732571.138956830]: check!
[ INFO] [1679732571.138965178]: CRC校验成功！
[ INFO] [1679732571.138972729]: 辐射值0.150000
