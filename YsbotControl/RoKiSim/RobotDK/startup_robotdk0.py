# Type help("robolink") or help("robodk") for more information
# Press F5 to run the script
# Note: you do not need to keep a copy of this file, your python script is saved with the station
from robolink import *    # API to communicate with RoboDK
from robodk import *      # basic matrix operations

import os
import sys

sys.path.append("C:\\Python34\\lib\\site-packages\\win32")
import win32api, win32pdhutil, win32con
import win32com.client
import win32pdh, string

import ctypes  
whnd = ctypes.windll.kernel32.GetConsoleWindow()  
if whnd != 0:  
    ctypes.windll.user32.ShowWindow(whnd, 0)  #hide zhe console
    ctypes.windll.kernel32.CloseHandle(whnd)

# ***********************************************************************    
# CheckProcExistByPN start
# ***********************************************************************
def CheckProcExistByPN(process_name):
    try:
        WMI = win32com.client.GetObject('winmgmts:') 
        processCodeCov = WMI.ExecQuery('select * from Win32_Process where Name="%s"' % process_name)
    except Exception as e:
        print (process_name + "error : ", e)
    if len(processCodeCov) > 0:
        print (process_name + " exist")
        return 1
    else:
        print (process_name + " is not exist")
        return 0
# CheckProcExistByPN end
# ***********************************************************************


# ***********************************************************************
# ***********************************************************************
def GetAllProcesses():
    object = "Process"
    items, instances = win32pdh.EnumObjectItems(None,None,object, win32pdh.PERF_DETAIL_WIZARD)
    return instances
# ***********************************************************************
 
 
# ***********************************************************************
# ***********************************************************************
def GetProcessID( name ):
    object = "Process"
    items, instances = win32pdh.EnumObjectItems(None,None,object, win32pdh.PERF_DETAIL_WIZARD)
    val = None
    if name in instances :
        hq = win32pdh.OpenQuery()
        hcs = []
        item = "ID Process"
        path = win32pdh.MakeCounterPath( (None,object,name, None, 0, item) )
        hcs.append(win32pdh.AddCounter(hq, path))
        win32pdh.CollectQueryData(hq)
        time.sleep(0.01)
        win32pdh.CollectQueryData(hq)
        for hc in hcs:
            type, val = win32pdh.GetFormattedCounterValue(hc, win32pdh.PDH_FMT_LONG)
            win32pdh.RemoveCounter(hc)
            win32pdh.CloseQuery(hq)
            return val
# ***********************************************************************
 
 
'''
#THIS IS SLOW !!
def Kill_Process ( process ) :
    #get process id's for the given process name
    pids = win32pdhutil.FindPerformanceAttributesByName ( process )
    for p in pids:
        handle = win32api.OpenProcess(win32con.PROCESS_TERMINATE, 0, p) #get process handle
    win32api.TerminateProcess(handle,0) #kill by handle
    win32api.CloseHandle(handle) #close api
'''
 
# ***********************************************************************
# ***********************************************************************
def Kill_Process_pid(pid) :
    handle = win32api.OpenProcess(win32con.PROCESS_TERMINATE, 0, pid) #get process handle
    win32api.TerminateProcess(handle,0) #kill by handle
    win32api.CloseHandle(handle) #close api
# ***********************************************************************
 
 
# ***********************************************************************
# ***********************************************************************
def Kill_Process ( name ) :
    pid = GetProcessID (name)
    print (pid)
    if pid:
        print ("exist")
        Kill_Process_pid(pid)
    else:
        print ("not this proccess")
# ***********************************************************************

isNewRobot=False
#if not exit process of robotdk,creat it
try:
    if CheckProcExistByPN('RoboDK.exe')==0:
        win32api.ShellExecute (0, 'open', 'DR605L.robot', '','',0)
        isNewRobot=True
        print('robotdk is called ')
    else:
        print('robotdk is running ')
except OSError as e:
    print (e)
    input('input enter to exit <enter>')
    exit(0)

time.sleep(2)

# ***********************************************************************
# the function of check if the string is float
# ***********************************************************************
def check_float(string):
    str1 = str(string)
    if str1.count('.')>1:#判断小数点是不是大于1
        return False
    elif str1.isdigit():
        return True#判断是不是整数
    else:
        new_str  = str1.split('.')#按小数点分割字符
        frist_num = new_str[0] #取分割完之后这个list的第一个元素
        if frist_num.count('-')>1:#判断负号的格数，如果大于1就是非法的
            return False
        else:
            frist_num = frist_num.replace('-','')#把负号替换成空
        if frist_num.isdigit() and new_str[1].isdigit():
        #如果小数点两边都是整数的话，那么就是一个小数
            return True
        else:
            return False
# check_float end
# ***********************************************************************


RL = Robolink()

# get a robot
robot = RL.Item('', ITEM_TYPE_ROBOT)
if not robot.Valid():
    print("No robot in the station. Load a robot first, then run this program.")
    pause(5)
    #raise Exception("No robot in the station!")

print('Using robot: %s' % robot.Name())
#print('Use the arrows (left, right, up, down), Q and A keys to move the robot')
print('Note: This works on console mode only, you must run the PY file separately')

# define the move increment
move_speed = 10

from msvcrt import getch
import socket
  
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # s = socket.socket()
hostname = socket.gethostname()
ip = socket.gethostbyname(hostname)
address = (ip, 2001)
address = ('127.0.0.1', 2001)
s.bind(address)  

s.listen(3)                                            #监听，等待连接的最大数目为1
funcode=''
print('Server is running...')                          

# ***********************************************************************
# ***********************************************************************
def TCP(sock, addr):                                   #TCP服务器端处理逻辑   
    print('--Accept new connection from %s:%s.' %address)   #接受新的连接请求

    while True:
        try:
            ts=time.clock()
            
            data = sock.recv(128)                         #接受其数据
            
            print('recv data:%s' %data)
            #'axis = 6 : 1.0 2.0 3.0 4.0 5.0 6.0'
            
            r_joint=[]
            strdata=[]
            for i in data:
                strdata.append(chr(i))
            string=''.join(strdata)
            if string.isalpha:
                code=string.lower()
                print ('function code: ',code)
                if (code == 'quit'):        #如果数据为空或者'quit'，则退出
                    sock.close()
                    funcode='quit'
                    if isNewRobot:
                        Kill_Process ('RoboDK.exe')
                        print('kill robodk.exe')
                    time.sleep(10)
                    quit()
                    break
                
            strlist=string.split()
            if (len(strlist)<10):
                print('data length error')
                continue
            if ('axis'==strlist[0]):
                joints= strlist[2]
                if joints.isdigit()==False:
                    print('joints error')
                    continue
                if int(joints)<6:
                    print('joints error')
                    continue
                for i in range(0,6):
                    if check_float(strlist[i+4]):
                        r_joint.append(float(strlist[i+4]))                  
                        print(r_joint[i])
                    else:
                        print('joint data is not a float')
            else:
                print('the first string is not "axis"')
            
            if len(r_joint)>=6 :               
                    
                # get the robot joints
                robot_joints = robot.Joints()
                #time.sleep()
                for i in range(0,6):
                    robot_joints[i]=r_joint[i]  
                    #print (robot_joints[i])
                '''               
                # get the robot position from the joints (calculate forward kinematics)
                robot_position = robot.SolveFK(robot_joints)

                # get the robot configuration (robot joint state)
                robot_config = robot.JointsConfig(robot_joints)

                # calculate the new robot joints
                new_robot_joints = robot.SolveIK(robot_position)
                if len(new_robot_joints.tolist()) < 6:
                    print("No robot solution!! The new position is too far, out of reach or close to a singularity")
                    continue

                # calculate the robot configuration for the new joints
                new_robot_config = robot.JointsConfig(new_robot_joints)

                if robot_config[0] != new_robot_config[0] or robot_config[1] != new_robot_config[1] or robot_config[2] != new_robot_config[2]:
                    print("Warning!! Robot configuration changed!! This will lead to unextected movements!")
                    print(robot_config)
                    print(new_robot_config)
                '''
                # move the robot joints to the new position
                robot.MoveJ(robot_joints)
                #robot.MoveL(new_robot_joints)
				
                sock.close()
                time.sleep(0.001)                                  #延迟
                te = time.clock()
                print(" timelsp = %f" %(te-ts))
                #break
                
            if not data:
                break
            
        except Exception as e:
            print ('something error:',e)
            break
            
        #sock.send(data.decode('utf-8').upper().encode())  #发送变成大写后的数据,需先解码,再按utf-8编码,  encode()其实就是encode('utf-8')

    #sock.close()                                       #关闭连接
    print('Connection from %s:%s closed.' %addr)       
    #return True
# ***********************************************************************

while True:  
    sock, addr = s.accept()                            #接收一个新连接
    TCP(sock, addr)
    if (funcode=='quit'):
        print(funcode)
        break
quit()
    

