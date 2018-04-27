#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/imgproc.hpp"

void processFrame(cv::Mat frame);
void detectCircles(cv::Mat src, cv::Mat);
void drawText(std::string text, cv::Point loc, cv::Mat dst);
int countPips(cv::Mat);

int lowerT, upperT, cdIndex;

/* Main entry-point to application */
int main(int argc, char** argv)
{
    cv::VideoCapture captureDevice;
    cv::Mat frame;
    bool findDomino = true;
    
    // Select a video source
    if(argc == 1)
    {
        // Use default camera
        captureDevice.open(0);
        cdIndex = 0;
        lowerT = 8000;
        upperT = 30000;
    }
    else
    {
        // use selected capture device
        cdIndex = atoi(argv[1]);
        captureDevice.open(cdIndex);
        
        if(argc == 4) {
            lowerT = (atoi)(argv[2]);
            upperT = atoi(argv[3]);
        } else {
            lowerT = 8000;
            upperT = 30000;
        }
    }
    
    // Output settings
    std::cout << "Opening capture device at index " << cdIndex << std::endl;
    std::cout << "Lower threshold " << lowerT << std::endl;
    std::cout << "Upper threshold " << upperT << std::endl;
    
    // Ensure the video source was opened
    if(!captureDevice.isOpened())
    {
        std::cerr << "Failed to open capture device at index : " << argv[1] << std::endl;
        return -1;
    }
    
    while(findDomino)
    {
        // Capture the next frame
        captureDevice >> frame;
        
        // Ensure data was captured
        if(frame.empty())
        {
            findDomino = false;
        }
        else
        {
            processFrame(frame);
        }
        
        // Wait 33 MS to capture next frame...
        switch(cv::waitKey(33))
        {
            // ESC --- Stop finding dominoes and exit program
            case 27:
                findDomino = false;
                break;
        }
    }
    
    // EOP
    return 0;
}

void processFrame(cv::Mat frame)
{
    cv::Mat dst = frame.clone();
    
    // Brighten the image
    //frame.convertTo(frame, -1, 2.2, 1.1);
    
    
    // Convert to grey
    cv::cvtColor(frame, dst, CV_BGR2GRAY);
    
    // Reduce noise
    // cv::GaussianBlur(dst, dst, cv::Size(3, 3), 2, 2);
    
    // Find edges
    // cv::Canny(dst, dst, 25, 75, 3);
    
    // cv::imshow("Frame", frame);
    try {
        detectCircles(dst, frame);
    } catch(cv::Exception e) {
        
    }
}

// ===
// Code below im not sure about i was just playing with it
// ===

void detectCircles(cv::Mat src, cv::Mat orig) {
    cv::Mat circles = src.clone();
    
    // Store the found contour points here
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy; /// not sure what this is for...
    
        // not sure about this... cv::absdiff(src, backgroundFrame, frame);
    
    // Binary image it
    cv::threshold(circles, circles, 150,255, cv::THRESH_BINARY );
    
    // cv::GaussianBlur(circles, circles, cv::Size(3, 3), 2, 2);
    cv::Canny(circles, circles, 2, 2*2, 3, false );
    
    cv::findContours(circles, contours, hierarchy, CV_RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE); // , cv::Point(0, 0));
    
    // circles = src.clone();
    cv::cvtColor(circles, circles, CV_GRAY2BGR);
    
    for(int i = 0; i < contours.size(); i++)
    {
        // Get the area of the dominoe?
        double domContourArea = cv::contourArea(contours[i]);
        
        // std::cout << "Contour " << i << " area is " << domContourArea << std::endl;
        
        // filter to make sure we found the actual dominoes
        // *** NOTE --- THIS MAY NEED TO CHANGE DEPENDING ON LOCATION OF CAMERA AND ROOM LIGHTING ***
        if(domContourArea > lowerT && domContourArea < upperT)
        {
            // Get rectangle of the dominoes
            cv::Rect domRectangle = cv::boundingRect(cv::Mat(contours[i]));
            cv::Rect part1, part2;
            int nOfPips;
            cv::Mat domROI;
            
            // *** TODO ***
            // Divide rect by 2 to determine top value and bottom value of dominoes
            // ***
            if(domRectangle.width > domRectangle.height) {
                part1 = cv::Rect(domRectangle.x, domRectangle.y, domRectangle.width / 2, domRectangle.height);
                
                part2 = cv::Rect(domRectangle.x + domRectangle.width / 2, domRectangle.y, domRectangle.width / 2, domRectangle.height);
            } else {
                // Dominoe is up right (value on top and bottom)
                part1 = cv::Rect(domRectangle.x, domRectangle.y, domRectangle.width, domRectangle.height / 2);
                part2 = cv::Rect(domRectangle.x, domRectangle.y + domRectangle.height / 2, domRectangle.width, domRectangle.height);
                
                
            }
            
            // Process part 1
            domROI = src(part1);
            nOfPips = countPips(domROI);
            drawText(std::to_string(nOfPips), cv::Point(part1.x, part1.y) , orig);
            
            // Process part 2
            domROI = src(part2);
            nOfPips = countPips(domROI);
            drawText(std::to_string(nOfPips), cv::Point(part2.x, part2.y) , orig);
            
            cv::rectangle(orig, domRectangle, cv::Scalar(0, 255, 0));
        }
    }
    
    cv::imshow("Contours", circles);
    cv::imshow("Live Feed", orig);
}

int countPips(cv::Mat dom)
{
    
    // show
    cv::namedWindow("processed", true);
    cv::imshow("processed", dom);
    
    // search for blobs
    cv::SimpleBlobDetector::Params params;
    
    // filter by interia defines how elongated a shape is.
    // params.filterByInertia = true;
    // params.minInertiaRatio = 0.5;
    params.filterByCircularity = true;
    params.minCircularity = 0.1;
    params.filterByColor = 1;
    params.blobColor = 255;
    
    // will hold our keyponts
    std::vector<cv::KeyPoint> keypoints;
    
    // create new blob detector with our parameters
    cv::Ptr<cv::SimpleBlobDetector> blobDetector = cv::SimpleBlobDetector::create(params);
    
    // detect blobs
    blobDetector->detect(dom, keypoints);
    
    
    // return number of pips
    return keypoints.size();
}

void drawText(std::string text, cv::Point loc, cv::Mat dst)
{
    int fontFace = cv::FONT_HERSHEY_SCRIPT_SIMPLEX;
    double fontScale = 1;
    int thickness = 3;
    
    cv::putText(dst, text, loc, fontFace, fontScale, cv::Scalar(0, 0, 255), thickness, 8);
}

