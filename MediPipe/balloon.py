#https://twgo.io/xprlz
import cv2,numpy as np
import mediapipe as mp
import random as rd
import time as t
import pygame
pygame.init()
pygame.mixer.init()

mp_drawing = mp.solutions.drawing_utils
mp_drawing_styles = mp.solutions.drawing_styles
mp_holistic = mp.solutions.holistic
holistic =  mp_holistic.Holistic()
w,h,c=0,0,0
GetScore=True
Score=0
objSize=120,120
objx,objy=0,0
HFSize=30
obj=cv2.resize(cv2.imread('src/bollon.png'),(objSize))
obj_b=cv2.resize(cv2.imread('src/bollon_break.png'),(objSize))
i=0
#繪圖：將物件放置在背景的特定位置，物件為白背景
def addPng(bg,png,ax,ay):
  pngY,pngX,channels = png.shape        
  bgY,bgX,channels = bg.shape
  if (ax+pngX<=bgX and ax>=0) and (ay+pngY<=bgY and ay>=0):
    roi = bg[ay:ay+pngY, ax:ax+pngX ] #放置位置
    #1.建立png黑色部份作為遮罩
    pnggray = cv2.cvtColor(png,cv2.COLOR_BGR2GRAY)
    ret, mask = cv2.threshold(pnggray, 250, 255, cv2.THRESH_BINARY) #254以上都是黑色
    mask_inv = cv2.bitwise_not(mask)
    #cv2.imshow('mask',mask_inv)
    #cv2.waitKey(0)
    #2.找出未來貼上的背景部份
    bg_roi = cv2.bitwise_and(roi,roi,mask = mask) #擷取出剩下的背景
    #cv2.imshow('mask',bg_roi)
    #cv2.waitKey(0)
    #3.取出Logo要顯示部份
    png_roi = cv2.bitwise_and(png,png,mask = mask_inv) #取出真正要顯示的logo部份
    #cv2.imshow('mask',png_roi)
    #cv2.waitKey(0)
    #4.將以上部份組合
    dst = cv2.add(bg_roi,png_roi)
    bg[ay:ay+pngY, ax:ax+pngX ] = dst
    return bg
  else:
    return bg

def Scored(frame,target,x,y):  
  print(target,t.time())  
  #氣球消失
  addPng(frame,obj_b,x,y)
  #播放音效
  pygame.mixer.music.load('src/break.mp3')
  pygame.mixer.music.play()

#判斷手腳的位置是否碰觸氣球
def isGetScore(RH,LH,RF,LF,objx,objy):
  #判斷位置
  #右手
  target=""
  if RH!=(-1,-1):
    if (((objx+(objSize[0]) >= (RH[0]-HFSize)) and (objx <= (RH[0]+HFSize)))
        and
        ((objy+(objSize[0]) >= (RH[1]-HFSize)) and (objy <= (RH[1]+HFSize)))
       ):      
      target="RH"
  #左手
  if LH!=(-1,-1):
    if (((objx+(objSize[0]) >= (LH[0]-HFSize)) and (objx <= (LH[0]+HFSize)))
        and
        ((objy+(objSize[0]) >= (LH[1]-HFSize)) and (objy <= (LH[1]+HFSize)))
       ):      
      target="LH"
  #左手
  if RF!=(-1,-1):
    if (((objx+(objSize[0]) >= (RF[0]-HFSize)) and (objx <= (RF[0]+HFSize)))
        and
        ((objy+(objSize[0]) >= (RF[1]-HFSize)) and (objy <= (RF[1]+HFSize)))
       ):      
      target="RF"

  if LF!=(-1,-1):
    if (((objx+(objSize[0]) >= (LF[0]-HFSize)) and (objx <= (LF[0]+HFSize)))
        and
        ((objy+(objSize[0]) >= (LF[1]-HFSize)) and (objy <= (LF[1]+HFSize)))
       ):      
      target="LF"
  if target!="":
    return True,target
  else:
    return False,target
 
#新增物件
def NewObj(frame,obj):
  #play Sound
  #New rand position  
  #Show obj
  x,y=rd.randint(20,w-120-20) ,rd.randint(20,h-120-20)
  addPng(frame,obj,x,y)
  return x,y
  pass
 
def DrawHandFoot(frame,results):
  #leftHand Postion
  if results.pose_landmarks.landmark[19].visibility>0.5: #實際是右手
    RH=int(np.array(results.pose_landmarks.landmark)[19].x*w),int(np.array(results.pose_landmarks.landmark)[19].y*h)
    cv2.circle(frame,(RH), 30, (255, 0, 0), 3)
  else:
    RH=-1,-1
  #rightHand Postion
  if results.pose_landmarks.landmark[20].visibility>0.5: #實際是左手
    LH=int(np.array(results.pose_landmarks.landmark)[20].x*w),int(np.array(results.pose_landmarks.landmark)[20].y*h)
    cv2.circle(frame,(LH), 30, (0, 255, 0), 3)
  else:
    LH=-1,-1
  #RightFoot Postion
  if results.pose_landmarks.landmark[31].visibility>0.5: #右腳
    RF=int(np.array(results.pose_landmarks.landmark)[31].x*w),int(np.array(results.pose_landmarks.landmark)[31].y*h)
    cv2.circle(frame,(RF), 30, (255, 0, 0), 3)
  else:
    RF=-1,-1
  #LeftFoot Postion
  if results.pose_landmarks.landmark[32].visibility>0.5: #左腳
    LF=int(np.array(results.pose_landmarks.landmark)[32].x*w),int(np.array(results.pose_landmarks.landmark)[32].y*h)
    cv2.circle(frame,(LF), 30, (0, 255, 0), 3)
  else:
    LF=-1,-1

  return(RH,LH,RF,LF)


cap = cv2.VideoCapture(0)
while cap.isOpened():
  ret, frame = cap.read() #讀取鏡頭畫面  
  (h,w,c)=frame.shape
  frame=cv2.flip(frame,1) #翻轉：-1上下、0上下左右、1左右
  frameRGB = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB) #轉換色彩頻道
  #身體偵測  
  results = holistic.process(frameRGB)
  if results.pose_landmarks!=None:
    #找到四肢位置
    RH,LH,RF,LF=DrawHandFoot(frame,results)
  else:
    RH,LH,RF,LF=(-1,-1),(-1,-1),(-1,-1),(-1,-1)

  GetScore,target=isGetScore(RH,LH,RF,LF,objx,objy)
  #是否得分
  if GetScore == True:
    #得分動作
    Scored(frame,target,objx,objy)
    Score+=1
    #產生新物件
    objx,objy=NewObj(frame,obj)
    GetScore=False
  else:
    addPng(frame,obj,objx,objy)
 
 
  cv2.putText(frame, "Get " + str(Score) , (10, 70), cv2.FONT_HERSHEY_PLAIN, 3, (255, 0, 255), 3)    
  frame = cv2.resize(frame,(1920,1080))
  cv2.imshow('Webcam',frame) #顯示畫面內容
  key=cv2.waitKey(1) #等候使用者按鍵盤指令
  if key==ord('a'):  #a拍照
    cv2.imwrite('webcam.jpg',frame) #拍照
  if key==ord('q'):  #q退出
    break
cap.release()
cv2.destroyAllWindows()
