# import the standard library

from Tkinter import *
from time import sleep
import thread
from copy import copy

# import the communication node
from CommunicationNode import CommunicationNode


class Application(Frame):
    def __init__(self, master=None):
        Frame.__init__(self, master)
        self.pack(side = "left")
        self.createWidgets()

    def createWidgets(self):
        self.port_name_label = Label(self, text = "Port Name")
        self.port_name_label.pack(side = "top")

        self.port_name_txt = Text(self)
        self.port_name_txt.config(width = 15, height = 1)
        self.port_name_txt.pack(side = "top")
        self.port_name_txt.insert(END, "COM11")

        self.baudrate_label = Label(self, text = "Baudrate")
        self.baudrate_label.pack(side = "top")

        self.baudrate_txt = Text(self)
        self.baudrate_txt.config(width = 15, height = 1)
        self.baudrate_txt.pack(side = "top")
        self.baudrate_txt.insert(END, "115200")

        self.start_button = Button(self)
        self.start_button.config(text = "Start", fg = "red")
        self.start_button["command"] = self.start_update
        self.start_button.pack(side = "top")

        self.data_display = Data_display(master = self)
        self.mode_control = Mode_control(master = self, to_control = self.data_display)
        self.send_frame = Send_Frame(master = root)
        print ("Application set up ready!")

    def start_update(self):
		#Setting specified, setting up the communication node
		print ("Starting update board info,\n    input on the two text boxes later will be in-effective")
		self.com_node = CommunicationNode(
		        self.port_name_txt.get("1.0", "end-1c"),
				self.baudrate_txt.get("1.0", "end-1c"))
		#Let the send frame to send message independently
		self.send_frame.com_node = self.com_node
		
		# Let the display frame to read data independently
		self.data_display.com_node = self.com_node

    	# Then start a thread to update the information in a infinite loop
		self.updating_handle = thread.start_new_thread(self.keep_updating, ())

    	# Then start a thread to keep displaying the information in a
    	# infinite loop with given mode
		self.displaying_handle = thread.start_new_thread(self.keep_displaying, ())

    def keep_updating(self):
    	while(self.com_node.is_open()):
    		self.com_node.update_once()

    	print ("Communication Node is closed, stop updating and return")

    def keep_displaying(self):
    	while(self.com_node.is_open()):
    		self.data_display.update_display()
    		# set the update period
    		sleep(0.02)

class Send_Frame(Frame):
	# This is the frmae that contains tool box to sen message to the
	# development board
	def __init__(self, master = None):
		Frame.__init__(self, master)
		self.pack(side = "top")
		self.createWidgets()
		self.com_node = None

	def createWidgets(self):
		self.send = Button(self)
		self.send.config(text = "Send calibrate data")
		self.send.pack(side = "left")
		self.send["command"] = self.send_mesg

		self.data1 = Text(self)
		self.data1.config(width = 10, height = 1)
		self.data1.pack(side = "left")
		self.data1.insert(END, "0")

		self.data2 = Text(self)
		self.data2.config(width = 10, height = 1)
		self.data2.pack(side = "left")
		self.data2.insert(END, "0")

		self.data3 = Text(self)
		self.data3.config(width = 10, height = 1)
		self.data3.pack(side = "left")
		self.data3.insert(END, "0")

		self.musk = Text(self)
		self.musk.config(width = 10, height = 1)
		self.musk.pack(side = "left")
		self.musk.insert(END, "0")

	def send_mesg(self):
		# This method read the input in the text boxes and send the message
		# througth the communication node
		data1 = self.data1.get("1.0", "end-1c")
		data2 = self.data2.get("1.0", "end-1c")
		data3 = self.data3.get("1.0", "end-1c")
		musk = self.musk.get("1.0", "end-1c")

		# Try to send the data
		if self.com_node != None:
			self.com_node.send_data(float(data1), float(data2), float(data3), float(musk))
		else:
			raise RuntimeError("CommunicationNode not initialized")
		print ("Send data done")

class Data_display(Frame):
	# This is the frame that contains the information to display on the
	# screen, the content is decided by the buttons in Display_mode frame
	def __init__(self, master = None):
		Frame.__init__(self, master)
		self.com_node = None
		# set default mode
		self.mode = "gimbal_data"
		# To enable the control feature, you have to keep this dictionary
		# and the 'print_funct_dict' in the update_display() updated.
		# Also, you have to implement the printing function is the member
		# member class of the 'self.com_node....'
		self.mode_dict = {
				"gimbal_data": "Gimbal Data",\
				"chassis_data": "Chassis Data",\
				"shoot_task_data": "Shoot Task" ,\
                "cali_data"     : "Cali Data"
                                }
		self.pack()

		self.createWidgets()

	def createWidgets(self):
		self.txt_box = Text()
		self.txt_box.pack(side = "bottom")
		# self.txt_box.config(width = 480, height = 240)
		self.txt_box.insert(END, "hello world!!!\n")

	def update_display(self):
		# This function will go straight into the attributes of CommunicationNode
		# instead of consulting the CommunicationNode object
		if (self.com_node == None):
			raise ReferenceError("Didn't initialize the communication node befor print info")
		# Start acquiring the string the information
		print_funct_dict = {
			"chassis_data": self.com_node.board_rece_mesg.chassis_information.print_info,\
			"gimbal_data": self.com_node.board_rece_mesg.gimbal_information.print_info,\
			"shoot_task_data": self.com_node.board_rece_mesg.shoot_task_information.print_info, \
			"cali_data": self.com_node.board_rece_mesg.cali_information.print_info
			}
		to_print = print_funct_dict[self.mode]()

		# clear the content in textbox and refill information
		self.txt_box.delete("1.0", END)
		self.txt_box.insert(END, to_print)

		return

class Mode_control(Frame):
	# This frame contains all buttons that help to choose which set of
	# information to show on the Display frame.
	# Based on the class structure, the frame to control has to have an
	# attribute called 'mode', which specified by string.
	def __init__(self, master = None, to_control = None):
		# The 'to_control' attribute is added manually which specifies
		# where to change the display mode.
		Frame.__init__(self, master)
		if (to_control == None):
			raise ValueError('No target specified when creating Mode_control frame!')
		self.to_control = to_control

		# Specifies an attrubites to manage buttons in this frame
		# which has to be a dictionary
		self.buttons = {}
		self.pack(side = "left")

		self.createWidgets(to_control.mode_dict)

	def createWidgets(self, button_dict):
		# This function is implemented to create all buttons to choose
		# mode.
		# The 'button_dict' parameter is used to create buttons automatically
		# and set up the onclick response automatically

		# Iterate over the button_dict to create buttons under the control
		# of 'self.buttons'
		for key in button_dict.iterkeys():
			self.buttons[key] = Button(self)
			self.buttons[key]["text"] = button_dict[key]
			# The behavior of each button is to change the mood of the display.
			# But this might not behave as desgined. Be aware of that.
			# print "generateing button with key: %s" % key
			const_key = copy(key)
			self.buttons[key]["command"] = self.change_mode_lambda(key)
			self.buttons[key].pack()

	def change_mode_lambda(self, mode = None):
		return lambda:self.change_mode(copy(mode))

	# This method is desgined to be passed as parameters for the buttons
	# onclick. The 'mode' has to be a string.
	def change_mode(self, mode = None):
		self.to_control.mode = mode


if __name__ == "__main__":
	print("""
		Welcome use this software, you can find a tutorial at
		https://github.com/sujuanyan/ShanghaiTech_Robomasters2018/tree/basic/PC_Communication_PY
		""")
	root = Tk()
	root.geometry('640x420')
	app = Application(master=root)
	root.mainloop()
	print ("PC Communication GUI exited sucessfully!")
