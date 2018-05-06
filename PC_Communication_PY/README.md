# 校准程序教程
## 环境配置：
当前只能在python2 环境下运行，需要安装的第三方库为pyserial
可以执行以下命令安装: 

	pip install pyserial 

## 校准步骤:
#### 1. 打开软件,进入界面。 插入USB TTL串口线。 

	1.1 TTL 绿灯亮说明正在接收信号 

	1.2 一般来说TTL上RX 接串口的蓝线

#### 2.  输入需要打开的串口名，点击“start”

	2.1 Windows 下串口名 一般为“COMx”(如COM11), 
	linux 下为“/dev/ttyUSBx”(如/dev/ttyUSB0).

	2.2 Windows 下串口名可用设备管理器查看

#### 3. 点击 "Cali Data" 会出现显示的数据。

#### 4. 将炮管移动到中心位置，点击“Send calibrate data” 即可完成校准

## 视频
[可以在这里下载视频](media/tutorial_origin_0.mp4)
    

