import sys
from keras.models import load_model
import cv2
import keras
import numpy as np

gesture_names = {0: 'Fist',
                 1: 'L',
                 2: 'Okay',
                 3: 'Palm',
                 4: 'C',
                 5: 'Thumb'
                }

model = load_model('./Models/gest_recog_model.h5')
#model = load_model('./Models/VGG_cross_validated.h5')

def predict_rgb_image_vgg(image):
    image = np.array(image, dtype='float32')
    image /= 255
    pred_array = model.predict(image)
    #print(f'pred_array: {pred_array}')
    result = gesture_names[np.argmax(pred_array)]
    #print(f'Result: {result}')
    #print(max(pred_array[0]))
    score = float("%0.2f" % (max(pred_array[0]) * 100))
    #print(result)
    return result, score

# parameters
cap_region_x_begin = 0.5  # start point/total width
cap_region_y_end = 0.8  # start point/total width
threshold = 60  # binary threshold
blurValue = 41  # GaussianBlur parameter
bgSubThreshold = 50
learningRate = 0

def image_proc(path):
    frame = cv2.imread(path)
    frame = cv2.flip(frame, 1)

    img = frame
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    ret, thresh = cv2.threshold(gray, threshold, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)

    target = np.stack((thresh,) * 3, axis=-1)
    target = cv2.resize(target, (224, 224))
    target = target.reshape(1, 224, 224, 3)
    prediction, score = predict_rgb_image_vgg(target)
    action = 0

    for key, val in gesture_names.items():
        if val == prediction:
            action = key

    return prediction, action

if __name__ == '__main__':
    if len(sys.argv) > 1:
        for i in range(len(sys.argv)):
            if i == 0:
                continue
            else:
                prediction, action = image_proc(sys.argv[i])
                print('Prediction:', prediction)
                print('Action:', action)
    else:
        print('Not enough args')