#https://twgo.io/mpttm	    
import cv2,numpy as np
import mediapipe as mp
mp_drawing = mp.solutions.drawing_utils
mp_drawing_styles = mp.solutions.drawing_styles
mp_face_mesh = mp.solutions.face_mesh
face_mesh = mp.solutions.face_mesh.FaceMesh()
OrgImg=cv2.imread('src/org.png')

# 擷取座標postion內圖形
def CropPic(img,postion):
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    mask=np.zeros(img.shape,dtype=np.uint8)
    #轉換座標格式
    roi_corners=np.array([postion],dtype=np.int32)
    ignore_mask_color = (255,255,255)
    #建立mask
    cv2.fillPoly(mask,roi_corners,ignore_mask_color)
    #cv2.imshow('mask',mask)
    #除了mask外，都為黑色
    #cv2.imshow('img',img)
    masked_image=cv2.bitwise_and(img,mask)
    #cv2.imshow('masked_image',masked_image)
    #計算矩形
    
    min_x=np.amin(roi_corners[0][:,0]) #取第x最小值
    max_x=np.amax(roi_corners[0][:,0]) #取第x最大值
    min_y=np.amin(roi_corners[0][:,1]) #取第y最小值
    max_y=np.amax(roi_corners[0][:,1]) #取第y最大值
    crop_img = masked_image[min_y:max_y, min_x:max_x]
    cv2.waitKey(1)
    return crop_img

#繪圖：將物件放置在背景的特定位置 
def addPng(ax,ay,bg,png): 
    global NewObj
    pngY,pngX,channels = png.shape        
    bgY,bgX,channels = bg.shape
    if (ax+pngX<=bgX and ax>=0) and (ay+pngY<=bgY and ay>=0):
        roi = bg[ay:ay+pngY, ax:ax+pngX ] #放置位置
        #1.建立png黑色部份作為遮罩
        pnggray = cv2.cvtColor(png,cv2.COLOR_BGR2GRAY)
        ret, mask = cv2.threshold(pnggray, 0, 255, cv2.THRESH_BINARY) #254以上都是黑色
        mask_inv = cv2.bitwise_not(mask)
        #cv2.imshow('mask',mask_inv)
        #cv2.waitKey(0)
        #2.找出未來貼上的背景部份
        bg_roi = cv2.bitwise_and(roi,roi,mask = mask_inv) #擷取出剩下的背景
        #cv2.imshow('mask',bg_roi)
        #cv2.waitKey(0)
        #3.取出Logo要顯示部份
        png_roi = cv2.bitwise_and(png,png,mask = mask) #取出真正要顯示的logo部份
        #cv2.imshow('mask',png_roi)
        #cv2.waitKey(0)
        #4.將以上部份組合
        dst = cv2.add(bg_roi,png_roi)
        bg[ay:ay+pngY, ax:ax+pngX ] = dst
        return bg
    else:
        NewObj=True
        return bg
    
drawing_spec = mp_drawing.DrawingSpec(thickness=1, circle_radius=1)
cap = cv2.VideoCapture(0)
while cap.isOpened():
  ret, frame = cap.read() #讀取鏡頭畫面    
  frame=cv2.flip(frame,1) #翻轉：-1上下、0上下左右、1左右
  imgRGB = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB) #轉換色彩頻道
  w,h = frame.shape[1],frame.shape[0]

  results = face_mesh.process(imgRGB)
  
  if results.multi_face_landmarks:
    
    for face_landmarks in results.multi_face_landmarks:
      #找嘴巴      
      mouthLandmarksIndex=[57,186,92,165,167,164,393,391,322,410,287,273,335,406,313,18,83,182,106,43]         
      mouthLandmarks=[]      
      for index in mouthLandmarksIndex:
         mouthLandmarks.append((face_landmarks.landmark[index].x * w,face_landmarks.landmark[index].y * h))
      MouthPic = CropPic(imgRGB,mouthLandmarks)
      #cv2.imshow('MouthPic',MouthPic)

      #左眼
      LefteyeLandmarksIndex=[226,113,225,224,223,222,221,189,244,233,232,231,230,229,228,31]
      LefteyeLandmarks=[]
      for index in LefteyeLandmarksIndex:
         LefteyeLandmarks.append((face_landmarks.landmark[index].x * w,face_landmarks.landmark[index].y * h))
      LefteyePic = CropPic(imgRGB,LefteyeLandmarks)
      #cv2.imshow('LefteyePic',LefteyePic)


      #右眼
      RighteyeLandmarksIndex=[413,464,453,452,451,450,449,448,261,446,342,445,444,443,442,441]
      RighteyeLandmarks=[]
      for index in RighteyeLandmarksIndex:
         RighteyeLandmarks.append((face_landmarks.landmark[index].x * w,face_landmarks.landmark[index].y * h))
      RighteyePic = CropPic(imgRGB,RighteyeLandmarks)
      #cv2.imshow('RighteyePic',RighteyePic)

      tempImg = OrgImg.copy()
      tempImg=addPng(80,120,tempImg,LefteyePic)
      tempImg=addPng(180,120,tempImg,RighteyePic)
      tempImg=addPng(120,180,tempImg,MouthPic)
      cv2.imshow('Org',tempImg)
      cv2.waitKey(1)
  # 印出臉部網路
  #     mp_drawing.draw_landmarks( # 網狀
  #         image=frame,
  #         landmark_list=face_landmarks,
  #         connections=mp_face_mesh.FACEMESH_TESSELATION,
  #         landmark_drawing_spec=None,
  #         connection_drawing_spec=mp_drawing_styles
  #         .get_default_face_mesh_tesselation_style())
  
  # 繪製臉部輪廓  
      mp_drawing.draw_landmarks( # 輪廓
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



