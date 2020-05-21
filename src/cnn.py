import sys
from keras.models import load_model
import cv2
import keras
import numpy as np
from time import sleep

gesture_names = {0: 'Zero',
                 1: 'One',
                 2: 'Two',
                 3: 'Three',
                 4: 'Four',
                 5: 'Five'
                }

#loading trained model from file
model = load_model('/home/umi/rec/models/digits_recog_model3.h5')

#prediction function
def predict_rgb_image(image):
    image = np.array(image, dtype='float32')
    image /= 255
    pred_array = model.predict(image)
    result = gesture_names[np.argmax(pred_array)]
    score = float("%0.2f" % (max(pred_array[0]) * 100))
    return result, score

# parameters
threshold = 60  # binary threshold

#image processing and prediction
def image_proc(path):
    frame = cv2.imread(path)
    frame = cv2.flip(frame, 1)

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    ret, thresh = cv2.threshold(gray, threshold, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)

    target = np.stack((thresh,) * 3, axis=-1)
    target = cv2.resize(target, (224, 224))
    target = target.reshape(1, 224, 224, 3)

    if (target == np.zeros_like(target)).all():
        return 'Empty', None

    prediction, score = predict_rgb_image(target)
    action = 0

    for key, val in gesture_names.items():
        if val == prediction:
            action = key

    return prediction, action

if __name__ == '__main__':
    #reading images every 1 second 
    #and writing predicted action to file
    while True:
        prediction, action = image_proc('/home/umi/rec/cache/hand.jpg')
        f = open('prediction.txt', 'w')
        f.write(str(action))
        f.close()
        print(action)
        sleep(1)
