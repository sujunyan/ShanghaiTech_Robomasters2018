import struct

# enum for judge_data_id_e
GAME_INFO_ID       = 0x0001 # 10Hz
REAL_BLOOD_DATA_ID = 0x0002 # Damage Taken
REAL_SHOOT_DATA_ID = 0x0003 # Bullet Shooting
POWER_HEAT_DATA_ID = 0x0004 # 50Hz
REAL_FIELD_DATA_ID = 0x0005 # 10Hz if RFID exists
GAME_RESULT_ID     = 0x0006 # End of the game
GAIN_BUFF_ID       = 0x0007 # Buff gained
ROBOT_POSITION_ID  = 0x0008 # 50Hz

STU_CUSTOM_DATA_ID = 0x0100
UPLOAD_DATA_ID     = 0x0101
DOWNLOAD_DATA_ID   = 0x0102


class gimbal_ctrl_t:
	# uint8_t ctrl_mode: gimbal control mode;
	# float pit_ref: gimbal pitch reference angle(degree);
	# float yaw_ref: gimbal yaw reference angle(degree);
	# uint8_t visual_valid: visual information valid or not;
	self_size = 1 + 4 + 4 + 1
	def __len__(self):
		return self.self_size
	def __init__(self, ctrl_mode, pit_ref, yaw_ref, visual_valid):
		# Set all the attributes in the object
		self.ctrl_mode = ctrl_mode
		self.pit_ref = pit_ref
		self.yaw_ref = yaw_ref
		self.visual_valid = visual_valid
		return

	# This function use struct to pack all data into a consecutive binary data
	# 	and it also returns the length of the data (index 0 in the tuple)
	def pack(self):
		# reference: https://docs.python.org/2/library/struct.html
		packed = struct.pack(">bffb", self.ctrl_mode, self.pit_ref, self.yaw_ref, self.visual_valid)

		return packed

class extGameRobotState_t:
	# ----brief:  game information structures definition(0x0001)
	# 		this package send frequency is 10Hz
	# uint16_t stageRemainTime;
	# uint8_t gameProgress;
	# uint8_t  robotLevel;
	# uint16_t remainHP;
	# uint16_t maxHP;
	self_size = 2 + 1 + 1 + 2 + 2
	def __init__(self, stageRemainTime = 0,
						gameProgress = 0,
						robotLevel = 0,
						remainHP = 0,
						maxHP = 0):
		self.stageRemainTime = stageRemainTime
		self.gameProgress = gameProgress
		self.robotLevel = robotLevel
		self.remainHP = remainHP
		self.maxHP = maxHP

	def pack(self):
		# reference: https://docs.python.org/2/library/struct.html
		packed = struct.pack(">hbbhh", self.stageRemainTime,
									self.gameProgress,
									self.robotLevel,
									self.remainHP,
									self.maxHP)
		return packed

class extRobotHurt_t:
	# ----brief:  real time blood volume change data(0x0002)

	# uint8_t armorType:4;
	# 	0-3bits: the attacked armor id:
	# 		0x00: 0 front
	# 		0x01: 1 left
	# 		0x02: 2 behind
	# 		0x03: 3 right
	# 		0x04: 4 above1
	# 		0x05: 5 above2
	# 	others reserved
	# uint8_t hurtType:4;
	# 	4-7bits: blood volume change type
	# 		0x00: armor attacked
	# 		0x01: module offline
	self_sizes = 1+1
	def __init__(self, armorType = 0, hurtType = 0):
		self.armorType = armorType
		self.hurtType = hurtType

	def pack(self):
		# reference: https://docs.python.org/2/library/struct.html
		packed = struct.pack(">bb", self.armorType,
									self.hurtTypes,)
		return packed

class extShootData_t:
	# ----brief:  real time shooting data(0x0003)

	# uint8_t bulletType;
	# uint8_t bulletFreq;
	# float   bulletSpeed;
	self_size = 1+1+4
	def __init__ (self, bulletType = 0, bulletFreq = 0, bulletSpeed = 0):
		self.bulletType = bulletType
		self.bulletFreq = bulletFreq
		self.bulletSpeed = bulletSpeed

	def pack(self):
		# reference: https://docs.python.org/2/library/struct.html
		packed = struct.pack(">bbf", self.bulletType,
									self.bulletFreq,
									self.bulletSpeed)
		return packed

class extPowerHeatData_t:
	# ----brief:  real time power heat data(0x0004)

	# float chassisVolt;
	# float chassisCurrent;
	# float chassisPower;
	# float chassisPowerBuffer;
	# uint16_t shooterHeat0;
	# uint16_t shooterHeat1;
	self_size = 4+4+4+4+2+2
	def __init__(self, chassisVolt = 0,
						chassisCurrent = 0,
						chassisPower = 0,
						chassisPowerBuffer = 0,
						shooterHeat0 = 0,
						shooterHeat1 = 0):
		self.chassisVolt = chassisVolt
		self.chassisCurrent = chassisCurrent
		self.chassisPower = chassisPower
		self.chassisPowerBuffer = chassisPowerBuffer
		self.shooterHeat0 = shooterHeat0
		self.shooterHeat1 = shooterHeat1

	def pack(self):
		# reference: https://docs.python.org/2/library/struct.html
		packed = struct.pack(">ffffII", self.chassisVolt,
									self.chassisCurrent,
									self.chassisPower,
									self.chassisPowerBuffer,
									self.shooterHeat0,
									self.shooterHeat1)

		return packed

class extRfidDetect_t:
	# ----brief:  rfid detect data(0x0005)
	 
	# uint8_t cardType;
	# 	1bit: card type:
	# 	0: ATK card
	# 	1: DEF card
	# 	2: red HEAL card
	# 	3: blue HEAL card
	# 	4: red ENERGY card
	# 	5: blue ENERGY card
	# others reserved */
	# uint8_t cardIdx;
	self_size = 1+1
	def __init__(self, cardType = 0, cardIdx = 0):
		self.cardType = cardType
		self.cardIdx = cardIdx

	def pack(self):
		# reference: https://docs.python.org/2/library/struct.html
		packed = struct.pack(">bb", self.cardType, self.cardIdx)
		return packed

class extGameResult_t:
	# /**
	#  * @brief  game result data(0x0006)
	#  */
	#  uint8_t winner;
	#  /* 1bit: winner:
	#     0: draw-game
	#     1: red win
	#     2: blue win
	#     others reserved */
	def __init__(self, winner = 0):
		self.winner = winner

class extGetBuff_t:
	# /**
	#  * @brief  the data of get field buff(0x0007)
	#  */
	#  uint8_t buffType;
	#  /* 1bit: buff type:
	#     0: ATK buff
	#     1: DEF buff
	#     2: ENERGY buff
	#     others reserved */
	#  uint8_t buffAddition;
	#  /* 1bit: percentage:
	#     10 == 10% */
	def __init__(self, buffType = 0, buffAddition = 0):
		self.buffType = buffType
		self.buffAddition = buffAddition

class extGameRobotPos_t:
	# /**
	#   * @brief  GPS state structures definition(0x0008)
	#   *         this package send frequency is 50Hz
	#   */
	#   float x;
	#   float y;
	#   float z;
	#   float yaw;
	def __init__(self, x = 0, y = 0, z = 0):
		self.x = x
		self.y = y
		self.z = z

class extShowData_t:
	# /**
	#  * @brief  student custom data(0x0100)
	#  */
	#  float data1;
	#  float data2;
	#  float data3;
	# uint8_t mask;
	def __init__(self, data1 = 0, data2 = 0, data3 = 0, mask = 0):
		self.data1 = data1
		self.data2 = data2
		self.data3 = data3
		self.mask = mask

class extUpStreamData_t:
	# /**
	#  * @brief  UPLOAD (0x0101)
	#  */
	#  uint8_t data[64];
	def __init__(self, data = None):
		self.data = data

class extDownStreamData_t:
	# /**
	#  * @brief  DOWNLOAD (0x0101)
	#  */
	#   uint8_t data[32];
	def __init__(self, data = None):
		self.data = data

class receive_judge_t:
	# /**
	#   * @brief  the data structure receive from judgement
	#   */
	# extGameRobotState_t     game_information;
	# extRobotHurt_t          blood_changed_data;
	# extShootData_t          real_shoot_data;
	# extPowerHeatData_t      power_heat_data;
	# extRfidDetect_t         rfid_data;
	# extGameResult_t         game_result_data;
	# extGetBuff_t            get_buff_data;
	# extGameRobotPos_t       robot_position_data;
	# extShowData_t           clinet_show_data;
	# extDownStreamData_t     clinet_download_data;
	# Be noticed that there seems a spell typo of 'clinet', which could be 'client'
	def __init__(self, \
				game_information = extGameRobotState_t(),\
				blood_changed_data = extRobotHurt_t(),\
				real_shoot_data = extShootData_t(),\
				power_heat_data = extPowerHeatData_t(),\
				rfid_data = extRfidDetect_t(),\
				game_result_data = extGameResult_t(),\
				get_buff_data = extGetBuff_t(),\
				robot_position_data = extGameRobotPos_t(),\
				clinet_show_data = extShowData_t(),\
				clinet_download_data = extDownStreamData_t()):
		self.game_information = game_information;
		self.blood_changed_data = blood_changed_data;
		self.real_shoot_data = real_shoot_data;
		self.power_heat_data = power_heat_data;
		self.rfid_data = rfid_data;
		self.game_result_data = game_result_data;
		self.get_buff_data = get_buff_data;
		self.robot_position_data = robot_position_data;
		self.clinet_show_data = clinet_show_data;
		self.clinet_download_data = clinet_download_data;