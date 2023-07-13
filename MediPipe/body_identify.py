# https://twgo.io/uzpgf	    
import cv2
import mediapipe as mp
mp_drawing = mp.solutions.drawing_utils
mp_drawing_styles = mp.solutions.drawing_styles
mp_holistic = mp.solutions.holistic
holistic =  mp_holistic.Holistic()

cap = cv2.VideoCapture('src/sport.mp4')
while cap.isOpened():
  ret, frame = cap.read() #讀取鏡頭畫面    
  frame=cv2.flip(frame,1) #翻轉：-1上下、0上下左右、1左右
  imgRGB = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB) #轉換色彩頻道
  results = holistic.process(imgRGB)
 
  # 劃出臉部
  mp_drawing.draw_landmarks(
      frame,
      results.face_landmarks,
      mp_holistic.FACEMESH_CONTOURS,
      landmark_drawing_spec=None,
      connection_drawing_spec=mp_drawing_styles
      .get_default_face_mesh_contours_style())
  # 劃出身體
  mp_drawing.draw_landmarks(
      frame,
      results.pose_landmarks,
      mp_holistic.POSE_CONNECTIONS,
      landmark_drawing_spec=mp_drawing_styles
      .get_default_pose_landmarks_style())
  cv2.imshow('Webcam',frame) #顯示畫面內容
  key=cv2.waitKey(1) #等候使用者按鍵盤指令
  if key==ord('a'):  #a拍照
      cv2.imwrite('webcam.jpg',frame) #拍照
  if key==ord('q'):  #q退出
      break
cap.release()
cv2.destroyAllWindows()

