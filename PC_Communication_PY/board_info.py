import struct

# enums for: infantry_data_id_e
CHASSIS_DATA_ID     = 0x0010
GIMBAL_DATA_ID      = 0x0011
SHOOT_TASK_DATA_ID  = 0x0012
INFANTRY_ERR_ID     = 0x0013
CONFIG_RESPONSE_ID  = 0x0014
CALI_RESPONSE_ID    = 0x0015
REMOTE_CTRL_INFO_ID = 0x0016
BOTTOM_VERSION_ID   = 0x0017
CALI_DATA_ID		= 0x0018

CHASSIS_CTRL_ID     = 0x00A0
GIMBAL_CTRL_ID      = 0x00A1
SHOOT_CTRL_ID       = 0x00A2
ERROR_LEVEL_ID      = 0x00A3
INFANTRY_STRUCT_ID  = 0x00A4
CALI_GIMBAL_ID      = 0x00A5

# enum for err_id_e
BOTTOM_DEVICE        = 0
GIMBAL_GYRO_OFFLINE  = 1
CHASSIS_GYRO_OFFLINE = 2
CHASSIS_M1_OFFLINE   = 3
CHASSIS_M2_OFFLINE   = 4
CHASSIS_M3_OFFLINE   = 5
CHASSIS_M4_OFFLINE   = 6
REMOTE_CTRL_OFFLINE  = 7
JUDGE_SYS_OFFLINE    = 8
PC_SYS_OFFLINE       = 9
GIMBAL_YAW_OFFLINE   = 10
GIMBAL_PIT_OFFLINE   = 11
TRIGGER_MOTO_OFFLINE = 12
BULLET_JAM           = 13
CHASSIS_CONFIG_ERR   = 14
GIMBAL_CONFIG_ERR    = 15
ERROR_LIST_LENGTH    = 16

# enum for buttom_err_e
DEVICE_NORMAL = 0
ERROR_EXIST   = 1
UNKNOWN_STATE = 2

# enum for err_level_e
GLOBAL_NORMAL        = 0
SOFTWARE_WARNING     = 1
SOFTWARE_ERROR       = 2
SOFTWARE_FATAL_ERROR = 3
GIMBAL_ERROR         = 4
CHASSIS_ERROR        = 5
HARAWARE_ERROR       = 6

# enum for struct_config_e
NO_CONFIG      = 0
DEFAULT_CONFIG = 1
CUSTOM_CONFIG  = 3



# /********** the information get from the board ***********/


class chassis_info_t:
	# * @brief  chassis information
	# uint8_t ctrl_mode;      /* chassis control mode */
	# float   gyro_palstance; /* chassis palstance(degree/s) from gyroscope */
	# float   gyro_angle;     /* chassis angle(degree) relative to ground from gyroscope */
	# float   ecd_palstance;  /* chassis palstance(degree/s) from chassis motor encoder calculated */
	# float   ecd_calc_angle; /* chassis angle(degree) relative to ground from chassis motor encoder calculated */
	# int16_t x_speed;        /* chassis x-axis move speed(mm/s) from chassis motor encoder calculated */
	# int16_t y_speed;        /* chassis y-axis move speed(mm/s) from chassis motor encoder calculated */
	# int32_t x_position;     /* chassis x-axis position(mm) relative to the starting point */
	# int32_t y_position;     /* chassis y-axis position(mm) relative to the starting point */

	# Add 'self_size' attribute for packing and unpacking
	self_size = 1+4+4+4+4+2+2+4+4
	def __init__(self,\
			ctrl_mode = 0,\
			gyro_palstance = 0,\
			gyro_angle = 0,\
			ecd_palstance =0,\
			ecd_calc_angle = 0,\
			x_speed = 0,\
			y_speed = 0,\
			x_position = 0,\
			y_position = 0):
		self.ctrl_mode = ctrl_mode
		self.gyro_palstance = gyro_palstance
		self.gyro_angle = gyro_angle
		self.ecd_palstance = ecd_palstance
		self.ecd_calc_angle = ecd_calc_angle
		self.x_speed = x_speed
		self.y_speed = y_speed
		self.x_position = x_position
		self.y_position = y_position

	# This function takes in the str object and unpack into the following
	# attributes.
	# Unpack method is refered to
	#		https://docs.python.org/2/library/struct.html
	def unpack(self, data):
		unpacked = struct.unpack("<Bffffhhii", data)
		# print "data updated: ", unpacked
		self.ctrl_mode      = unpacked[0]
		self.gyro_palstance = unpacked[1]
		self.gyro_angle     = unpacked[2]
		self.ecd_palstance  = unpacked[3]
		self.ecd_calc_angle = unpacked[4]
		self.x_speed        = unpacked[5]
		self.y_speed        = unpacked[6]
		self.x_position     = unpacked[7]
		self.y_position     = unpacked[8]

	def print_info(self):
		# This method returns string to print on the display
		to_return = "Chassis information:\n"
		to_return += "ctrl_mode: %d\n" % self.ctrl_mode
		to_return += "gyro_palstance: %d\n" % self.gyro_palstance
		to_return += "gyro_angle: %d\n" % self.gyro_angle
		to_return += "ecd_palstance: %d\n" % self.ecd_palstance
		to_return += "ecd_calc_angle: %d\n" % self.ecd_calc_angle
		to_return += "x_speed: %d\n" % self.x_speed
		to_return += "y_speed: %d\n" % self.y_speed
		to_return += "x_position: %d\n" % self.x_position
		to_return += "y_position: %d\n" % self.y_position

		return to_return

class cali_info_t:
	"""
	* @brief  gimbal information
	int pit_offset;
	int yaw_offset;
	int pit_ecd;
	int yaw_ecd;
	 Add 'self_size' attribute for packing and unpacking
	"""

	self_size = 4
	def __len__(self):
		return self.self_size
	def __init__(self,\
				pit_offset = 0,yaw_offset =0, \
				pit_ecd = 0, yaw_ecd = 0 \
				):
				self.pit_offset = pit_offset
				self.yaw_offset = yaw_offset
				self.pit_ecd = pit_ecd
				self.yaw_ecd = yaw_ecd
				self.debug_info = 0

	def pack(self):
		return "cali"

	def unpack(self, data):
		# print "unpack gimbal info with data len: %d" % len(data)
		unpacked = struct.unpack("<iiiii", data)
		self.pit_offset = unpacked[0]
		self.yaw_offset = unpacked[1]
		self.pit_ecd = unpacked[2]
		self.yaw_ecd = unpacked[3]
		self.debug_info = unpacked[4]

	def print_info(self):
		# This method returns string to print on the display
		to_return = "Cali Information:\n"
		to_return += "pit_offset in flash : %d\n" % self.pit_offset
		to_return += "yaw_offset in flash : %d\n" % self.yaw_offset
		to_return += "pit_ecd : %d\n" % self.pit_ecd
		to_return += "yaw_ecd : %d\n" % self.yaw_ecd
		to_return += "debug_info : %x \n" % self.debug_info

		return to_return
	
class gimbal_info_t:
	# * @brief  gimbal information
	# uint8_t ctrl_mode;          /* gimbal control mode */
	# float   pit_relative_angle; /* pitch angle(degree) relative to the gimbal center */
	# float   yaw_relative_angle; /* yaw angle(degree) relative to the gimbal center */
	# float   pit_absolute_angle; /* pitch angle(degree) relative to ground */
	# float   yaw_absolute_angle; /* yaw angle(degree) relative to ground */
	# float   pit_palstance;      /* pitch axis palstance(degree/s) */
	# float   yaw_palstance;      /* yaw axis palstance(degree/s) */

	# Add 'self_size' attribute for packing and unpacking
	self_size = 1+4+4+4+4+4+4
	def __init__(self,\
			ctrl_mode = 0,\
			pit_relative_angle = 0,\
			yaw_relative_angle = 0,\
			pit_absolute_angle = 0,\
			yaw_absolute_angle = 0,\
			pit_palstance      = 0,\
			yaw_palstance      = 0):
		self.ctrl_mode          = ctrl_mode
		self.pit_relative_angle = pit_relative_angle
		self.yaw_relative_angle = yaw_relative_angle
		self.pit_absolute_angle = pit_absolute_angle
		self.yaw_absolute_angle = yaw_absolute_angle
		self.pit_palstance      = pit_palstance
		self.yaw_palstance      = yaw_palstance

	def unpack(self, data):
		# print "unpack gimbal info with data len: %d" % len(data)
		unpacked = struct.unpack("<Bffffff", data)
		self.ctrl_mode          = unpacked[0]
		self.pit_relative_angle = unpacked[1]
		self.yaw_relative_angle = unpacked[2]
		self.pit_absolute_angle = unpacked[3]
		self.yaw_absolute_angle = unpacked[4]
		self.pit_palstance      = unpacked[5]
		self.yaw_palstance      = unpacked[6]

	def print_info(self):
		# This method returns string to print on the display
		to_return = "Gimbal information:\n"
		to_return += "ctrl_mode: %d\n" % self.ctrl_mode
		to_return += "pit_relative_angle: %f\n" % self.pit_relative_angle
		to_return += "yaw_relative_angle: %f\n" % self.yaw_relative_angle
		to_return += "pit_absolute_angle: %f\n" % self.pit_absolute_angle
		to_return += "yaw_absolute_angle: %f\n" % self.yaw_absolute_angle
		to_return += "pit_palstance: %f\n" % self.pit_palstance
		to_return += "yaw_palstance: %f\n" % self.yaw_palstance

		return to_return

class shoot_info_t:
	# * @brief  shoot information
	# int16_t remain_bullets;  /* the member of remain bullets */
	# int16_t shot_bullets;    /* the member of bullets that have been shot */
	# uint8_t fric_wheel_run;  /* friction run or not */

	# Add 'self_size' attribute for packing and unpacking
	self_size = 2+1+1
	def __init__(self,\
			remain_bullets = 0,\
			shot_bullets = 0,\
			fric_wheel_run = 0):
		self.remain_bullets = remain_bullets
		self.shot_bullets = shot_bullets
		self.fric_wheel_run = fric_wheel_run

	# This function takes in the str object and unpack into the following
	# attributes.
	# Unpack method is refered to
	#		https://docs.python.org/2/library/struct.html
	def unpack(self, data):
		unpacked = struct.unpack("<hhB", data)
		self.remain_bullets = unpacked[0]
		self.shot_bullets   = unpacked[1]
		self.fric_wheel_run = unpacked[2]

	def print_info(self):
		# This method returns string to print on the display
		to_return = "Shoot Information:\n"
		to_return += ("remain_bullets: %d\n" % self.remain_bullets)
		to_return += ("shot_bullets: %d\n" % self.shot_bullets)

		return to_return

class infantry_err_t:
	# * @brief  infantry error information
	# bottom_err_e err_sta;                 /* bottom error state */
	# bottom_err_e err[ERROR_LIST_LENGTH];  /* device error list */

	# Add 'self_size' attribute for packing and unpacking
	self_size = 4 + 4*ERROR_LIST_LENGTH
	def __init__(self, err_sta = 0, err = None):
		self.err_sta = err_sta
		self.err = err

	# This function takes in the str object and unpack into the following
	# attributes.
	# Unpack method is refered to
	#		https://docs.python.org/2/library/struct.html
	def unpack(self, data):
		unpacked = struct.unpack("<ii", data)
		self.err_sta = unpacked[0]
		self.err     = unpacked[1]

	def print_info(self):
		# This method returns string to print on the display
		to_return = ""

		return to_return

class version_info_t:
	# * @brief  bottom software version information
	# uint8_t num[4];

	# Add 'self_size' attribute for packing and unpacking
	self_size = 1*4
	def __init__(self, num = None):
		self.num = num

	# This function takes in the str object and unpack into the following
	# attributes.
	# Unpack method is refered to
	#		https://docs.python.org/2/library/struct.html
	def unpack(self, data):
		for i in range(0, 4):
			self.num[i] = struct.unpack("<B", data[i])[0]

class cali_cmd_t:
	# * @brief  gimbal calibrate command
	# uint8_t type;        /* 0x01 calibrate gimbal center, 0x02 calibrate camera */

	# Add 'self_size' attribute for packing and unpacking
	self_size = 1
	def __init__(self, type_a = 0):
		self.type = type_a

# /********* variables **********/


class receive_board_t:
	def __init__(self,\
			chassis_information = chassis_info_t(),\
			gimbal_information = gimbal_info_t(),\
			shoot_task_information = shoot_info_t(),\
			cali_information = cali_info_t(), \
			):
		self.chassis_information = chassis_information
		self.gimbal_information = gimbal_information
		self.shoot_task_information = shoot_task_information
		self.cali_information = cali_information
