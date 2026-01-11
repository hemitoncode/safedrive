import cv2 
import mediapipe as mp


# Capture the video. The 1 parameter is webcam
cap = cv2.VideoCapture(1)

while cap.isOpened():
    success, image = cap.read()

    if not success:
        break

    cv2.imshow("SafeDrive", cv2.flip(image, 1))

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()