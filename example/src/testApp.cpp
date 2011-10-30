#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){

    cam.init(640, 360);
    cam.setHost("192.168.7.28");
    cam.setCredentials("root", "flab4711");
    cam.setVerbose(true);  
    cam.open();
    
    colorImg.allocate(640, 360);
	grayImage.allocate(640, 360);
	grayBg.allocate(640, 360);
	grayDiff.allocate(640, 360);

	bLearnBakground = true;
	threshold = 80;

	ofSetFrameRate(30);
}

//--------------------------------------------------------------
void testApp::update()
{
	ofBackground(100,100,100);
	cam.update();

    colorImg.setFromPixels(cam.getPixels(), 640, 360);
    grayImage = colorImg;
    if (bLearnBakground == true){
        grayBg = grayImage;		// the = sign copys the pixels from grayImage into grayBg (operator overloading)
        bLearnBakground = false;
    }

    // take the abs value of the difference between background and incoming and then threshold:
    grayDiff.absDiff(grayBg, grayImage);
    grayDiff.threshold(threshold);

    // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
    // also, find holes is set to true so we will get interior contours as well....
    contourFinder.findContours(grayDiff, 10, (640*360)/2, 20, false);	// find holes
}

//--------------------------------------------------------------
void testApp::draw(){
    
    // draw the incoming, the grayscale, the bg and the thresholded difference
	ofSetColor(0xffffff);
	colorImg.draw(20,20);
	//grayImage.draw(360,20, 320, 180);
	//grayBg.draw(20,280, 320, 180);
	grayDiff.draw(660, 20, 320, 180);

	// then draw the contours:

	//ofFill();
	//ofSetColor(0x333333);
	//ofRect(360,540,320,240);
	ofSetColor(0xffffff);

	// we could draw the whole contour finder
	contourFinder.draw(20, 20);//360,540);

	// or, instead we can draw each blob individually,
	// this is how to get access to them:
    /*
    for (int i = 0; i < contourFinder.nBlobs; i++){
        contourFinder.blobs[i].draw(360,540);
    }*/

	// finally, a report:

	ofSetColor(0xffffff);
	char reportStr[1024];
	sprintf(reportStr, "bg subtraction and blob detection\npress ' ' to capture bg\nthreshold %i (press: +/-)\nnum blobs found %i, fps: %f", threshold, contourFinder.nBlobs, ofGetFrameRate());
	ofDrawBitmapString(reportStr, 20, 600);
}

//--------------------------------------------------------------
void testApp::exit(){

	cam.close();
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
switch (key){
		case ' ':
			bLearnBakground = true;
			break;
		case '+':
			threshold ++;
			if (threshold > 255) threshold = 255;
			break;
		case '-':
			threshold --;
			if (threshold < 0) threshold = 0;
			break;
            
        case 'o':
        	cam.open();
            break;
            
        case 'c':
        	cam.close();
            break;
            
        case 'g':
        	cam.grabFrame();
            break;
	}
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

