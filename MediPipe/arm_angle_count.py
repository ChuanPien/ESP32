
import cv2
import mediapipe as mp
import numpy as np
import math

mp_drawing = mp.solutions.drawing_utils
mp_drawing_styles = mp.solutions.drawing_styles
mp_selfie_segmentation = mp.solutions.selfie_segmentation
mp_pose = mp.solutions.pose
BG_COLOR = (192, 192, 192) # gray
Angle=100
ExAngle=50
ExStatus=False
countEx=0
pose = mp_pose.Pose()

def FindAngleF(a,b,c):    
    ang = math.degrees(math.atan2(c[1]-b[1], c[0]-b[0]) - math.atan2(a[1]-b[1], a[0]-b[0]))
    if ang<0 :
      ang=ang+360
    if ang >= 360- ang:
        ang=360-ang
    return ang

def countExF(HandAngel):
  global ExStatus
  if HandAngel<Angle and ExStatus==False:
    countEx=1
    ExStatus=True
  elif HandAngel>Angle :
    countEx=0
    ExStatus=False
  else:
    countEx=0
  return countEx


# For webcam input:
cap = cv2.VideoCapture('src/sport.mp4')
with mp_selfie_segmentation.SelfieSegmentation(model_selection=0) as selfie_segmentation:
  with mp_pose.Pose(
      min_detection_confidence=0.5,
      min_tracking_confidence=0.5) as pose:
    while cap.isOpened():
      success, image = cap.read()
      image=cv2.flip(image,1) #翻轉：-1上下、0上下左右、1左右
      if not success:
        print("Ignoring empty camera frame.")
        # If loading a video, use 'break' instead of 'continue'.
        continue
      h,w=image.shape[0],image.shape[1]
      #image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
      results = pose.process(image) #偵測身體
      #右手軸3點->11,13,15
      #右腳軸3點->24,26,28
      if (not results.pose_landmarks==None): #至少有一個身體
        a=np.array([results.pose_landmarks.landmark[24].x*w,results.pose_landmarks.landmark[24].y*h])
        b=np.array([results.pose_landmarks.landmark[26].x*w,results.pose_landmarks.landmark[26].y*h])
        c=np.array([results.pose_landmarks.landmark[28].x*w,results.pose_landmarks.landmark[28].y*h])
        #算出角度
        HandAngle=FindAngleF(a,b,c)
        #print(HandAngle)
        #算出次數

        countEx=countEx+countExF(HandAngle)
        print("countEx=",countEx)
        #畫出點位
        mp_drawing.draw_landmarks(
            image,
            results.pose_landmarks, #點
            mp_pose.POSE_CONNECTIONS, #連線
            landmark_drawing_spec=mp_drawing_styles.get_default_pose_landmarks_style()
          )
        #image=cv2.flip(image, 1)
        x13,y13=round((results.pose_landmarks.landmark[26].x)*w),int(results.pose_landmarks.landmark[26].y*h)
        if (x13<w and x13>0) and (y13<h and y13>0):
            cv2.putText(image, str(round(HandAngle,2)) , (x13,y13), cv2.FONT_HERSHEY_PLAIN, 2, (255, 0, 255), 3)
        cv2.putText(image,  str(countEx) , (30,40), cv2.FONT_HERSHEY_PLAIN, 3, (255, 0, 255), 3)
        # 畫面切割
        # image.flags.writeable = True
        # image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
        # results = selfie_segmentation.process(cv2.cvtColor(image, cv2.COLOR_BGR2RGB))
        # condition = np.stack((results.segmentation_mask,) * 3, axis=-1) > 0.1
        # bg_image = np.zeros(image.shape, dtype=np.uint8)
        # bg_image[:] = BG_COLOR
        # image = np.where(condition, image, bg_image)
        # image=cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        #顯示結果      
      cv2.imshow('MediaPipe Pose',image)
      key=cv2.waitKey(1) #等候使用者按鍵盤指令
      if key==ord('a'):  #a拍照
          cv2.imwrite('webcam.jpg',image) #拍照
      if key==ord('q'):  #q退出
          break
cap.release()





