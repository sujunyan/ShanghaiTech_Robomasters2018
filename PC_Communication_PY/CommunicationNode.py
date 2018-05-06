# include library
from serial import *
import struct
from copy import copy

# link local files
from judgement_info import *
from board_info import *
from protocol import *

toHex = lambda x: "".join("{:02X}".format(ord(c)) for c in x)

def protocol_packet_pack(cmd_id, p_data, sof):
	# This function performs as packing the input data and returns a tuple comtaining
	# 	the data length and the packed binary data
	# To perform the function of packing, the following arguments are treated
	#	as type listed below:
	#		sof: uint8_t

	p_data += '\0' # in python there are no '\0' at the end of the string
	frame_header_length = HEADER_LEN + CMD_LEN
	frame_length = HEADER_LEN + CMD_LEN + len(p_data) + CRC_LEN

	# Pack the header into the tx_buf
	# Write bytes in big-endian
	# Due to the restriction of the struct.pack_into() method,
	#	 it is hard to set the packed bytes dynamically (with reference to the variable size)
	# reference: https://docs.python.org/2/library/struct.html
	tx_buf = struct.pack("<BHb", sof, len(p_data), 0)
	# Pack the cmd_id into the tx_buf
	
	tx_buf = append_crc8_check_sum(tx_buf,HEADER_LEN)
	tx_buf = tx_buf + struct.pack("<H", cmd_id)
	
	# connect the packed data package into the tx_buf
	tx_buf = tx_buf + p_data
	
	tx_buf = append_ccr16_check_sum(tx_buf,frame_length)

	return (frame_header_length + len(p_data), tx_buf)

class CommunicationNode:
	def __init__(self, portname, baudrate):
		# configure and open the port
		self.port = Serial(port = portname, baudrate = baudrate,
			parity = PARITY_NONE,
			stopbits = STOPBITS_ONE,
			bytesize = 	EIGHTBITS)
		# Didn't set the 'timeout' field, which means the port.read
		# will be blocked if the number of bytes read is not reached.
		if not (self.is_open()):
			self.port.open()
		self.data = unpack_data_t(p_header = frame_header_t())
		self.judge_rece_mesg = receive_judge_t()
		self.board_rece_mesg = receive_board_t()

	def is_open(self):
		# using the port field to check if the prot is still open
		return self.port.port != None

	def update_once(self):
		# Based on the description about the protocol, the C implementation
		# will not be used which is 'unpack_data' method.
		# This function will process the serial data and call the data handle
		# independently.

		# print "update once start:.............."

		# Keep reading untill reached the matched 'sof' value
		sof_value = DN_REG_ID
		sof_s = self.port.read(size = 1)
		sof_read = struct.unpack("<B", sof_s)[0]
		while (sof_read != sof_value):
			# print "Read sof start"
			sof_s = self.port.read(size = 1)
			sof_read = struct.unpack("<B", sof_s)[0]
			# print "Read sof done"
		self.data.sof = sof_value
		data_packet = sof_s
		# print "finishing reading sof"

		# Get the correct 'sof', then read the length
		length_s = self.port.read(size = 2)
		self.data.data_len = struct.unpack("<H", length_s)[0]
		# print "Read serial data length: %d" % self.data.data_len
		if (self.data.data_len > PROTOCAL_FRAME_MAX_SIZE - HEADER_LEN - CRC_LEN):
			return
		data_packet += length_s

		# Get the 'seq'
		seq_s = self.port.read(size = 1)
		data_packet += seq_s
		# print "finishing reading seq"

		# Get the 'crc8' for check reading
		crc8_s = self.port.read(size = 1)
		data_packet += crc8_s
		# print "finishing reading crc8"
		# 'self.port.timeout' is not set, so the readings before will all
		# be the enough amount of data

		# Try get the 'cmd_id'
		cmd_id_s = self.port.read(size = 2)
		data_packet += cmd_id_s
		# print "finishing reading cmd_id"

		# print "cmd id: %d" % struct.unpack("<H", cmd_id_s)[0]

		# Acquire all the data in terms of the data length
		data_packet += self.port.read(size = self.data.data_len)
		self.data.protocol_packet = data_packet
		# print "finishing reading packed data"

		# Get the 'crc16' for check reading
		crc16 = struct.unpack("<H", self.port.read(size = 2))[0]
		# print "finishing reading crc16"
		# 'self.port.timeout' is not set, so the readings before will all
		# be the enough amount of data

		with open('serial_read.log', 'a') as read_log:
			read_log.write(data_packet);
			read_log.write("|||||||")

		self.board_data_handle(data_packet)

		# print "Update once end.............."
		MAX_SERIAL_BUFFER = 10000
		if (self.port.inWaiting() > MAX_SERIAL_BUFFER):
			self.port.reset_input_buffer()
		return

	def send_data(self, data1, data2, data3, mask):
		# the three data should be float

		package = gimbal_ctrl_t(ctrl_mode = 0,
			pit_ref = 1,
			yaw_ref = 2,
			visual_valid = 3)

		# acquire the data length and the package to sent
		
		data = package.pack()
		packed_package = protocol_packet_pack(CALI_GIMBAL_ID, data, DN_REG_ID)

		# write to the serial port
		self.port.write(packed_package[1])

	# according to the C code, 'p_obj' has to be a unpack_data_t object
	# according to the C code, 'sof' has to be a number, better to be uint8_t
	# This function is not in use

	# Refering to C code (uint8_t *p_frame)
	def board_data_handle(self, p_frame):
		# unpack memory to p_header
		frame_header_unpacked = struct.unpack("<BHBB", p_frame[0:HEADER_LEN])
		p_header = frame_header_t(\
			frame_header_unpacked[0],\
			frame_header_unpacked[1],\
			frame_header_unpacked[2],\
			frame_header_unpacked[3])

		data_length = p_header.data_length

		#print ("handle data: data length: %d" % data_length)
		cmd_id = struct.unpack("<H", p_frame[HEADER_LEN:(HEADER_LEN+CMD_LEN)])[0]
		data_start_index = HEADER_LEN + CMD_LEN
		#print ("CMD_ID: %x" % cmd_id)

		# print type(cmd_id)

		# print "Get the compare parameter"
		# print CHASSIS_DATA_ID, '\n', REMOTE_CTRL_INFO_ID
		if (cmd_id == CHASSIS_DATA_ID):
			print ("----read chassis data")
			self.board_rece_mesg.chassis_information.unpack(p_frame[data_start_index:(data_start_index + data_length)])
		elif (cmd_id == GIMBAL_DATA_ID):
			print ("----read gimbal data")
			self.board_rece_mesg.gimbal_information.unpack(p_frame[data_start_index:(data_start_index + data_length)])
		elif (cmd_id == CALI_DATA_ID):
			#print ("----read cali data id")
			self.board_rece_mesg.cali_information.unpack(p_frame[data_start_index:(data_start_index + data_length)])
		elif (cmd_id == SHOOT_TASK_DATA_ID):
			print ("----read shoot task data")
			# self.board_rece_mesg.shoot_task_information.unpack(p_frame[data_start_index:(data_start_index + data_length)])
		elif (cmd_id == INFANTRY_ERR_ID):
			print ("----read infantry error")
			# self.board_rece_mesg.bottom_error_data.unpack(p_frame[data_start_index:(data_start_index + data_length)])
		elif (cmd_id == CONFIG_RESPONSE_ID):
			print ("----read config response")
			# self.board_rece_mesg.structure_config_data.unpack(p_frame[data_start_index:(data_start_index + data_length)])
		elif (cmd_id == CALI_RESPONSE_ID):
			print ("----read cali response")
			# self.board_rece_mesg.cali_response_data.unpack(p_frame[data_start_index:(data_start_index + data_length)])
		elif (cmd_id == REMOTE_CTRL_INFO_ID):
			print ("Remote control data recieved, but not updated. (Not implemented yet)")
		elif (cmd_id == BOTTOM_VERSION_ID):
			print ("----read bottom version")
			# self.board_rece_mesg.version_info_data.unpack(p_frame[data_start_index:(data_start_index + data_length)])
		else:
			print ("----read data un-recognized")
		return


	# Referring to C code (uint8_t *p_frame)
	# Not Finished yet
	def judgement_data_handle(self, p_frame):
		# copy memory to p_header
		p_header = frame_header_t(sof = p_frame[0],\
			data_len = (p_frame[1] | (p_frame[2] << 8)),\
			seq = p_frame[3],\
			crc8 = p_frame[4])

		data_len = p_frame[1] | (p_frame[2] << 8)
		cmd_id = p_frame[HEADER_LEN] | (p_frame[HEADER_LEN + 1] << 8)
		data_start_index = HEADER_LEN + CMD_LEN

		# switch (cmd_id)
		# if (cmd_id == GAME_INFO_ID):

		# elif (cmd_id == REAL_BLOOD_DATA_ID):
