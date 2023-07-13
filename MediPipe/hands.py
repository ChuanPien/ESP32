#https://twgo.io/ftylw	    

import cv2
import mediapipe as mp
import time

mpDraw = mp.solutions.drawing_utils #呼叫繪圖工具
mpHands = mp.solutions.hands #呼叫手部工具
#呼叫手部工具內的手部辨識器
hands = mpHands.Hands(  
    static_image_mode=False, #單張或串流(True單張模式(慢)，False串流模式(快))
    #model_complexity=0,#0->精簡模型(快)，1->完整模型(慢)，rpi記得註解
    max_num_hands=2, #辨識最多手
    min_detection_confidence=0.5, #辨識信任度
    min_tracking_confidence=0.5 #追蹤信任度
    )

cap = cv2.VideoCapture(0)
while cap.isOpened(): #鏡頭是否能開啟
    stime=time.time()
    ret, frame = cap.read() #讀取鏡頭畫面
    h, w, c = frame.shape #取得螢幕長寬色彩
    frame=cv2.flip(frame,1) #翻轉：-1上下、0上下左右、1左右
    imgRGB = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB) #轉換色彩頻道
    results = hands.process(imgRGB) #手部辨識
       
    if results.multi_hand_landmarks: #如果有找到手部
        for i in range(len(results.multi_handedness)): #所有的手
            thisHandType=results.multi_handedness[i].classification[0].label #手的屬性          
            thisHand=results.multi_hand_landmarks[i] #取得這隻手
            mpDraw.draw_landmarks(frame, thisHand, mpHands.HAND_CONNECTIONS) #利用工具畫
            #學習自己畫關節(了解關節座標位置)
            for id, lm in enumerate(thisHand.landmark): #id=編號,lm=座標               
                hx, hy = int(lm.x * w), int(lm.y * h) #計算座標
                cv2.circle(frame, (hx, hy), 5, (255, 0, 0), cv2.FILLED) #在關節點上標藍色圓形
                cv2.putText(frame,str(id),(hx,hy), cv2.FONT_HERSHEY_PLAIN, 2, (255, 0, 255), 2)
                if id==0: #左右手
                    cv2.putText(frame,thisHandType,(hx,hy-30), cv2.FONT_HERSHEY_PLAIN, 2, (0, 255, 0), 2)            
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



