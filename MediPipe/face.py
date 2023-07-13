#https://twgo.io/sintt	    
import cv2
import mediapipe as mp
mpDraw = mp.solutions.drawing_utils
mp_drawing_styles = mp.solutions.drawing_styles
mp_face_mesh = mp.solutions.face_mesh
face_mesh = mp.solutions.face_mesh.FaceMesh()

drawing_spec = mpDraw.DrawingSpec(thickness=1, circle_radius=1)
cap = cv2.VideoCapture(0)
while cap.isOpened():
  ret, frame = cap.read() #讀取鏡頭畫面    
  frame=cv2.flip(frame,1) #翻轉：-1上下、0上下左右、1左右
  imgRGB = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB) #轉換色彩頻道

  results = face_mesh.process(imgRGB)

  if results.multi_face_landmarks:    
    for face_landmarks in results.multi_face_landmarks:
      # 嘴巴開合
      #print(face_landmarks.landmark[14].y-face_landmarks.landmark[13].y)
      mpDraw.draw_landmarks( # 網狀
          image=frame,
          landmark_list=face_landmarks,
          connections=mp_face_mesh.FACEMESH_TESSELATION,
          landmark_drawing_spec=None,
          connection_drawing_spec=mp_drawing_styles
          .get_default_face_mesh_tesselation_style())
     
      mpDraw.draw_landmarks( # 輪廓
          image=frame,
          landmark_list=face_landmarks,
          connections=mp_face_mesh.FACEMESH_CONTOURS,
          landmark_drawing_spec=None,
          connection_drawing_spec=mp_drawing_styles
          .get_default_face_mesh_contours_style())
         
  cv2.imshow('Webcam',frame) #顯示畫面內容
  key=cv2.waitKey(1) #等候使用者按鍵盤指令
  if key==ord('a'):  #a拍照
      cv2.imwrite('webcam.jpg',frame) #拍照
  if key==ord('q'):  #q退出
      break
cap.release()
cv2.destroyAllWindows()

