# DominoesDetection
This is a small C++ project in which uses the OpenCV Library 3.0. The purpose of this project is to detect and count the values on dominoes. Some simple processing is applied to the image in order to locate the dominoes within the video stream. Once found the dominoes are sliced out of the video steam into their own individual images and passed to another function in which attempts to read the values of the dominoes as two separate values (Top and bottom or left and right depending on the orientation of the domino). 

This is not a perfect process and however it can be accruate in the right environment. Ideally you would like to place your camera directly above the dominoes facing down at them. The camera should be approximate 1.5ft away from the scene. Lighting in the scene should be good (however, please note that if you dominoes are shiney then a bright light may cause the algorithm to fail due to the reflective surface). 

Here is an exammple of the system working.

![29996179_10214814356099790_1189389045_o](https://user-images.githubusercontent.com/18728027/39339996-8a33726c-4999-11e8-8518-2714a8ed5171.png)

Also note that very noisy backgrounds can cause the process to fail as it may be unable to detect the edges of the domino. 

![29995921_10214814356739806_891650355_o](https://user-images.githubusercontent.com/18728027/39340006-92c8ec5e-4999-11e8-9ac9-527f001f3ed3.png)


### Additional Resources

**OpenCV Library**
https://opencv.org/opencv-3-0.html 
