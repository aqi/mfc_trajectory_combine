# mfc_trajectory_combine
###**百度导航轨迹文件分析与合并工具**
***
####功能说明:
1.百度导航轨迹文件GPS点导出:

    可以将手机百度地图导航生成的.bin.gz轨迹文件中的GPS点导出
    成文本,也可以导出成地图文件,用googl浏览器打开生成的地图文件可以
    看到之前走过的路径.

2.百度地图轨迹文件合并:

    可以将手机开启百度导航生成的多个.bin.gz轨迹文件合并成一
    个轨迹文件,方便GPS点的导出,也方便地图文件的生成.
***
***
####使用图文教程:

![image](https://github.com/qianghaohao/mfc_trajectory_combine/raw/master/img/export_demo.png)
![image](https://github.com/qianghaohao/mfc_trajectory_combine/raw/master/img/trajectory_combine_demo.png)
![image](https://github.com/qianghaohao/mfc_trajectory_combine/raw/master/img/trajectory_demo.png)
![image](https://github.com/qianghaohao/mfc_trajectory_combine/raw/master/img/about_tool.png)
***
***
####工具实现思路:
    由于百度导航轨迹文件是gz压缩格式的,因此在解析前先用zlib开源库将gz文件解压
    到内存缓冲,然后根据轨迹文件头信息来解析解压出来的轨迹文件.将其中的GPS点的经度,
    纬度,时速,时间戳提取出来,可以将这些信息保存成文本文件,也可以将这些GPS点导出
    成地图文件(html文件).然后用Google浏览器打开生成的地图文件即可查看以前走过的路
    径.Google地图会自动连接这些GPS点成轨迹,但是导出的地图文件中的GPS不能过多,一般
    不要多于14个点左右,否则生成的地图文件无法打开.
    ------>手机百度地图轨迹文件在:SD卡/BaiduMap/bnav/trajectory

***
***
####工具源码目录说明:
    mfc_trajectory_combine\mfc_trajectory_combine\bin   可执行文件目录
    mfc_trajectory_combine\mfc_trajectory_combine\doc   软件说明帮助文档
    mfc_trajectory_combine\mfc_trajectory_combine\res  软件资源文件,里面包含图标,背景图片等
    mfc_trajectory_combine\mfc_trajectory_combine\zlib-1.2.8   zlib库头文件
    mfc_trajectory_combine\mfc_trajectory_combine\zlib-dll  zlib动态链接库及其导入库
***
***
####软件源码:

#####百度导航轨迹文件分析与合并工具界面版:
[https://github.com/qianghaohao/mfc_trajectory_combine](https://github.com/qianghaohao/mfc_trajectory_combine)      *By Qiang Haohao*
#####百度导航轨迹文件分析与合并工具控制台版:
[https://github.com/hongwenjun/TrajectoryCombine](https://github.com/hongwenjun/TrajectoryCombine)      *By Hong Wenjun*
                          
####本工具参考代码:
[https://github.com/hongwenjun/TrajectoryCombine](https://github.com/hongwenjun/TrajectoryCombine)      **ThanksTo:hongwenjun**
***
