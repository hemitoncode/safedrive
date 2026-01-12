import cv2 
import mediapipe as mp
import numpy as np
from scipy.spatial import distance as dist
import serial
import time 

port = serial.Serial("/dev/cu.usbmodem1101", 9600)
time.sleep(2)

# Eye Aspect Ratio calculation
def calculate_ear(eye_landmarks):
    """Calculate the Eye Aspect Ratio (EAR)"""
    # Compute the euclidean distances between the vertical eye landmarks
    A = dist.euclidean(eye_landmarks[1], eye_landmarks[5])
    B = dist.euclidean(eye_landmarks[2], eye_landmarks[4])
    # Compute the euclidean distance between the horizontal eye landmarks
    C = dist.euclidean(eye_landmarks[0], eye_landmarks[3])
    # Calculate the EAR
    ear = (A + B) / (2.0 * C)
    return ear

# Constants for drowsiness detection
EAR_THRESHOLD = 0.25  # Eye aspect ratio below this indicates closed eyes
CONSEC_FRAMES = 20    # Number of consecutive frames eyes must be closed to trigger alert

# Counters
frame_counter = 0

# Boolean flags 
already_active = False

# MediaPipe Face Mesh landmark indices for eyes
LEFT_EYE = [33, 160, 158, 133, 153, 144]
RIGHT_EYE = [362, 385, 387, 263, 373, 380]

# Capture the video
cap = cv2.VideoCapture(1)

# Set up the face mesh
mp_face_mesh = mp.solutions.face_mesh
mp_drawing = mp.solutions.drawing_utils
mp_drawing_styles = mp.solutions.drawing_styles

# Use FaceMesh API 
with mp_face_mesh.FaceMesh(
    max_num_faces=1,
    refine_landmarks=True,
    min_detection_confidence=0.5,
    min_tracking_confidence=0.5
) as face_mesh:
    while cap.isOpened():
        success, image = cap.read()
        if not success:
            break
        
        # Get image dimensions
        h, w, _ = image.shape
        
        # Convert the BGR image to RGB for MediaPipe
        image_rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        
        # Process the image and find face mesh
        results = face_mesh.process(image_rgb)
        
        # Draw face mesh if detected
        if results.multi_face_landmarks:
            for face_landmarks in results.multi_face_landmarks:
                # Extract eye landmarks
                left_eye_coords = []
                right_eye_coords = []
                
                for idx in LEFT_EYE:
                    landmark = face_landmarks.landmark[idx]
                    left_eye_coords.append([landmark.x * w, landmark.y * h])
                
                for idx in RIGHT_EYE:
                    landmark = face_landmarks.landmark[idx]
                    right_eye_coords.append([landmark.x * w, landmark.y * h])
                
                # Convert to numpy arrays
                left_eye_coords = np.array(left_eye_coords)
                right_eye_coords = np.array(right_eye_coords)
                
                # Calculate EAR for both eyes
                left_ear = calculate_ear(left_eye_coords)
                right_ear = calculate_ear(right_eye_coords)
                
                # Average EAR
                avg_ear = (left_ear + right_ear) / 2.0
                
                # Check if eyes are closed
                eyes_closed = avg_ear < EAR_THRESHOLD

                # ON condition
                if eyes_closed and not already_active and frame_counter >= CONSEC_FRAMES:
                    port.write(b'1')        # Turn LED ON
                    already_active = True

                # OFF condition
                if not eyes_closed and already_active:
                    port.write(b'0')        # Turn LED OFF
                    already_active = False

                # Update frame counter
                frame_counter = frame_counter + 1 if eyes_closed else 0
                
                # Draw eye contours
                cv2.polylines(image, [left_eye_coords.astype(np.int32)], True, (0, 255, 0), 1)
                cv2.polylines(image, [right_eye_coords.astype(np.int32)], True, (0, 255, 0), 1)
                
                # Display EAR value
                cv2.putText(image, f"EAR: {avg_ear:.2f}", (10, 30),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)
                
                # Display frame counter when eyes are closing
                if frame_counter > 0:
                    cv2.putText(image, f"Closed: {frame_counter}/{CONSEC_FRAMES}", (10, 60),
                                cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 165, 255), 2)
        
        cv2.imshow("SafeDrive", cv2.flip(image, 1))
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

cap.release()
cv2.destroyAllWindows()
