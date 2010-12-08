ofxIpCamera: an ip camera grabber addon
=======================================

2010 Dan Wilcox danomatika.com

GPL v3

DESCRIPTION
-----------

ofxIpCamera is an Open Frameworks addon for Axis IP Cameras which *should* work with other, generic ip cameras

OpenFrameworks is a cross platform open source toolkit for creative coding in C++
http://www.openframeworks.cc/

BUILD REQUIREMENTS
------------------

Download OpenFrameworks 0.6+ from http://www.openframeworks.cc/ and unpack the ofxIpCamera source code into the apps folder to run the test app.

Furthermore, you can simply add the ofxIpCamera.h and ofxIpCamera.cpp to your own applciations. 

BUILD AND INSTALLATION
----------------------

Add your camera connection info into the testApp.cpp setup() function. Build and run the example app to test camera grabbing.

USAGE
-----

Make sure to init the grabber with your camera size, host uri, and authentication credentials:

    cam.init(640, 360);
    cam.setHost("192.168.7.28");
    cam.setCredentials("root", "flab4711"); 
    cam.open();

The grabber is threaded and will continually grab images after you call open(). You can also grab one image a time without running the thread by using grabFrame().

DEVELOPING
----------

You can help develop ofxIpCamera on GitHub: https://github.com/danomatika/ofxIpCamera

Create an account, clone or fork the repo, then request a push/merge.

If you find any bugs or suggestions please log them to GutHub as well.

FUTURE IDEAS/IMPROVEMENTS
-------------------------

* testing with more ip cameras
* implement isFrameNew()
* multiple platform build files (Linxus & Windows)

