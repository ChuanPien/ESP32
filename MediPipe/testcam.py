#twgo.io/uqqyj
import cv2
cap=cv2.VideoCapture(0) #抓取第0號攝影鏡頭
while cap.isOpened(): #鏡頭是否能開啟
    ret,frame=cap.read() #讀取鏡頭內容
    frame=cv2.flip(frame,1) #翻轉：-1上下、0上下左右、1左右
    cv2.imshow('Webcam',frame) #顯示畫面內容
    key=cv2.waitKey(1) #等候使用者按鍵盤指令
    if key==ord('a'):  #a拍照
        cv2.imwrite('webcam.jpg',frame) #拍照
    if key==ord('q'):  #q退出
        break
cap.release()
cv2.destroyAllWindows()

