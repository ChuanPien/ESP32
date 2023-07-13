#https://twgo.io/phjwd
import cv2
import mediapipe as mp
import numpy as np
import math

mp_drawing = mp.solutions.drawing_utils
mp_drawing_styles = mp.solutions.drawing_styles
mp_selfie_segmentation = mp.solutions.selfie_segmentation
mp_pose = mp.solutions.pose
pose = mp_pose.Pose()


def FindAngleF(a,b,c):    
    ang = math.degrees(math.atan2(c[1]-b[1], c[0]-b[0]) - math.atan2(a[1]-b[1], a[0]-b[0]))
    if ang<0 :
      ang=ang+360
    if ang >= 360- ang:
        ang=360-ang
    return ang

# For webcam input:
cap = cv2.VideoCapture(0)
while cap.isOpened():
  ret, frame = cap.read() #讀取鏡頭畫面    
  frame=cv2.flip(frame,1) #翻轉：-1上下、0上下左右、1左右
  imgRGB = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB) #轉換色彩頻道    
  h, w, c = frame.shape #取得螢幕長寬色彩
  results = pose.process(imgRGB) #偵測身體
  #左手軸3點->11,13,15
  if (not results.pose_landmarks==None): #至少有一個身體
    a=(results.pose_landmarks.landmark[11].x,results.pose_landmarks.landmark[11].y)
    b=(results.pose_landmarks.landmark[13].x,results.pose_landmarks.landmark[13].y)
    c=(results.pose_landmarks.landmark[15].x,results.pose_landmarks.landmark[15].y)
    #算出角度
    HandAngle=FindAngleF(a,b,c)
    print(HandAngle)

    #畫出點位
    mp_drawing.draw_landmarks(
        frame,
        results.pose_landmarks, #點
        mp_pose.POSE_CONNECTIONS, #連線
        landmark_drawing_spec=mp_drawing_styles.get_default_pose_landmarks_style()
      )    
   
    x13,y13=round((results.pose_landmarks.landmark[13].x)*w),int(results.pose_landmarks.landmark[13].y*h)
    if (x13<w and x13>0) and (y13<h and y13>0):
      cv2.putText(frame, str(round(HandAngle,2)) , (x13,y13), cv2.FONT_HERSHEY_PLAIN, 2, (255, 0, 255), 3)
  cv2.imshow('Webcam',frame) #顯示畫面內容
  key=cv2.waitKey(1) #等候使用者按鍵盤指令
  if key==ord('a'):  #a拍照
      cv2.imwrite('webcam.jpg',frame) #拍照
  if key==ord('q'):  #q退出
      break
cap.release()
cv2.destroyAllWindows()

