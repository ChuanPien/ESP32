#https://twgo.io/dqzfa	    
import cv2
import math
import time
import mediapipe as mp

mpDraw = mp.solutions.drawing_utils #呼叫繪圖工具
mpHands = mp.solutions.hands #呼叫手部工具
#呼叫手部工具內的手部辨識器
hands = mpHands.Hands(  
    static_image_mode=False, #單張或串流(True單張模式，False串流模式)
    model_complexity=0,#0->精簡模型(快)，1->完整模型(慢)，rpi記得註解
    max_num_hands=1, #辨識最多手
    min_detection_confidence=0.7, #辨識信任度
    min_tracking_confidence=0.5 #追蹤信任度
    )
AngleTH=130 #判斷張開角度
def findAngleF(a,b,c):    
    ang = math.degrees(math.atan2(c[2]-b[2], c[1]-b[1]) - math.atan2(a[2]-b[2], a[1]-b[1]))
    if ang<0 :
      ang=ang+360
    if ang >= 360- ang:
        ang=360-ang
    return round(ang,2)

cap = cv2.VideoCapture(0)
while cap.isOpened(): #鏡頭是否能開啟
    stime=time.time()
    ret, frame = cap.read() #讀取鏡頭畫面
    frame=cv2.flip(frame,1) #翻轉：-1上下、0上下左右、1左右
    imgRGB = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB) #轉換色彩頻道
    results = hands.process(imgRGB) #手部辨識
       
    if results.multi_hand_landmarks: #如果有找到手部
        for i in range(len(results.multi_handedness)): #所有的手
            thisHandType=results.multi_handedness[i].classification[0].label #手的屬性          
            thisHand=results.multi_hand_landmarks[i] #取得這隻手
            mpDraw.draw_landmarks(frame, thisHand, mpHands.HAND_CONNECTIONS) #利用工具畫
            #學習自己畫關節(了解關節座標位置)
            thisHandLMList = []
            for id, lm in enumerate(thisHand.landmark): #id=編號,lm=座標                
                h, w, c = frame.shape #取得螢幕長寬色彩
                thisHandLMList.append([id, lm.x, lm.y,lm.z])
                hx, hy = int(lm.x * w), int(lm.y * h) #計算座標
                cv2.circle(frame, (hx, hy), 5, (255, 0, 0), cv2.FILLED) #在關節點上標藍色圓形
                cv2.putText(frame,str(id),(hx,hy), cv2.FONT_HERSHEY_PLAIN, 1, (255, 0, 255), 1)
                if id==0:
                    cv2.putText(frame,thisHandType,(hx,hy-30), cv2.FONT_HERSHEY_PLAIN, 2, (0, 255, 0), 2)            
           
           
            #print(findAngleF(thisHandLMList[0],thisHandLMList[6],thisHandLMList[8]))
            finger=[0,0,0,0,0]
            if (findAngleF(thisHandLMList[0],thisHandLMList[3],thisHandLMList[4])>AngleTH):
                finger[0]=1 #拇指
            if (findAngleF(thisHandLMList[0],thisHandLMList[6],thisHandLMList[8])>AngleTH):
                finger[1]=1 #食指
            if (findAngleF(thisHandLMList[0],thisHandLMList[10],thisHandLMList[12])>AngleTH):
                finger[2]=1 #中指
            if (findAngleF(thisHandLMList[0],thisHandLMList[14],thisHandLMList[16])>AngleTH):
                finger[3]=1 #無名
            if (findAngleF(thisHandLMList[0],thisHandLMList[18],thisHandLMList[20])>AngleTH):
                finger[4]=1 #小指
            print(finger)

    etime=time.time()
    fps=round(1/(etime-stime),2)
    cv2.putText(frame,"FPS:" + str(fps),(10,50), cv2.FONT_HERSHEY_PLAIN, 3, (0, 0, 255), 3)
    cv2.imshow('Webcam',frame) #顯示畫面內容
    key=cv2.waitKey(1) #等候使用者按鍵盤指令
    if key==ord('a'):  #a拍照
        cv2.imwrite('webcam.jpg',frame) #拍照
    if key==ord('q'):  #q退出
        break
cap.release()
cv2.destroyAllWindows()

