ofxIpCamera
===========

an ip camera grabber addon

[Dan Wilcox](danomatika.com) 2010

GPL v3

DESCRIPTION
-----------

ofxIpCamera is an Open Frameworks addon for Axis IP Cameras which *should* work with other, generic ip cameras

[OpenFrameworks](http://www.openframeworks.cc/) is a cross platform open source toolkit for creative coding in C++

See the [OF forum post](http://www.openframeworks.cc/forum/viewtopic.php?f=9&t=4087&hilit=ip+camera+axis) as well.

BUILD REQUIREMENTS & INSTALLATION
---------------------------------

Download [OpenFrameworks](http://www.openframeworks.cc/) and unpack the ofxIpCamera source code into the apps folder to run the test app.

Furthermore, you can simply add the ofxIpCamera.h and ofxIpCamera.cpp to your own applciations. 

To use ofxIpCamera, first you need to download and install [Open Frameworks](https://github.com/openframeworks/openFrameworks).

To get a copy of the repository you can download the source from [http://github.com/danomatika/ofxIpCamera](http://github.com/danomatika/ofxIpCamera/) or, alternatively, you can use git clone:
<pre>
git clone git://github.com/danomatika/ofxIpCamera.git
</pre>

The addon should sit in `openFrameworks/addons/ofxIpCamera/`.

#### Which version to use?

If you are using a stable version (0062, 007, ...) of OpenFrameworks then you want to use a git tag of ofxIpCamera for that version. You can select the tag in the Github menu or clone and check it out using git.

For example, the following commands will clone ofxKinect and switch to the OF 0062 tagged version:
<pre>
git clone git://github.com/danomatika/ofxIpCamera.git
cd ofxIpCamera
git checkout 0062
</pre>

#### Using the latest ofxIpCamera

The master branch of ofxIpCamera will work with the current master of OpenFrameworks and can be considered *relatively* stable.

BUILDING
----------------------

Add your camera connection info into the testApp.cpp setup() function. Build and run the example app to test camera grabbing.

USAGE
-----

Make sure to init the grabber with your camera size, host uri, and authentication credentials:

    cam.init(640, 360);
    cam.setHost("192.168.7.28");
    cam.setCredentials("user", "password"); 
    cam.open();

The grabber is threaded and will continually grab images after you call open(). You can also grab one image a time without running the thread by using grabFrame().

DEVELOPING
----------

You can help develop ofxIpCamera on GitHub: [https://github.com/danomatika/ofxIpCamera](https://github.com/danomatika/ofxIpCamera)

Create an account, clone or fork the repo, then request a push/merge.

If you find any bugs or suggestions please log them to GutHub as well.

FUTURE IDEAS/IMPROVEMENTS
-------------------------

* testing with more ip cameras
* multiple platform build files (Linux & Windows)

