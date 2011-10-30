/*==============================================================================

	ofxIpCamera.cpp

	ofxIpCamera: an ip camera grabber addon
  
	Copyright (C) 2010, 2011  Dan Wilcox <danomatika@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

==============================================================================*/
#include "ofxIpCamera.h"

#include "ofUtils.h"

#include "FreeImage.h"

#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/NetException.h"
#include "Poco/Buffer.h"

//--------------------------------------------------------------------
ofxIpCamera::ofxIpCamera() {
	// common
	bVerbose 				= false;
	bGrabberInited 			= false;
	bUseTexture				= true;
	width 					= 320;	// default setting
	height 					= 180;	// default setting
    bIsFrameNew             = false;
    bNeedsUpdate            = false;
}


//--------------------------------------------------------------------
ofxIpCamera::~ofxIpCamera() {
	close();
    clear();
}

//--------------------------------------------------------------------
bool ofxIpCamera::isFrameNew() {
    if(isThreadRunning()) {
        bool curIsFrameNew = bIsFrameNew;
        bIsFrameNew = false;
        return curIsFrameNew;
    }
    return false;
}

//--------------------------------------------------------------------
void ofxIpCamera::setVerbose(bool bTalkToMe) {
	bVerbose = bTalkToMe;
}

//--------------------------------------------------------------------
void ofxIpCamera::setHost(string host) {
	try {
    	uri = "http://"+host;
        uri.setPath("/axis-cgi/jpg/image.cgi");
        uri.setQuery("resolution="+ofToString(width)+"x"+ofToString(height));
    }
    catch(Poco::SyntaxException& e) {
    	ofLog(OF_LOG_ERROR, "ofxIpCamera: "+e.displayText());
        return;
    }
}

//--------------------------------------------------------------------
void ofxIpCamera::setUri(string uri) {
	try {
    	this->uri = uri;
    }
    catch(Poco::SyntaxException& e) {
    	ofLog(OF_LOG_ERROR, "ofxIpCamera: "+e.displayText());
        return;
    }
}

//--------------------------------------------------------------------
void ofxIpCamera::setCredentials(string username, string password) {
	credentials.setUsername(username);
    credentials.setPassword(password);
}

//---------------------------------------------------------------------------
unsigned char * ofxIpCamera::getPixels() {
	return pixels.getPixels();
}

//---------------------------------------------------------------------------
ofPixels& ofxIpCamera::getPixelsRef() {
	return pixels;
}

//------------------------------------
//for getting a reference to the texture
ofTexture & ofxIpCamera::getTextureReference() {
	if(!tex.bAllocated()) {
		ofLog(OF_LOG_WARNING, "ofxIpCamera: getTextureReference - texture is not allocated");
	}
	return tex;
}

//--------------------------------------------------------------------
void ofxIpCamera::open() {

	if(!bGrabberInited) {
    	ofLog(OF_LOG_WARNING, "ofxIpCamera: Cannot open, init not called");
        return;
    }
    
	if(uri.empty()) {
    	ofLog(OF_LOG_WARNING, "ofxIpCamera: Cannot open, uri not set");
        return;
    }
	
    startThread(false, false);	// non blocking, not verbose
}

//--------------------------------------------------------------------
void ofxIpCamera::close() {
	if(isThreadRunning())
		stopThread();
    bIsFrameNew = false;
    bNeedsUpdate = false;
}

//--------------------------------------------------------------------
bool ofxIpCamera::init(int w, int h, bool setUseTexture) {
	
    close();
    clear();
    
    bUseTexture = setUseTexture;
    
    width = w;
    height = h;
    
    pixels.allocate(width, height, OF_PIXELS_RGB);
    pixelsTemp.allocate(width, height, OF_PIXELS_RGB);
    pixels.set(0);
    pixelsTemp.set(0);
    if(bUseTexture) {
        tex.allocate(width, height, GL_RGB);
    }
    
    bGrabberInited = true;
    ofLog(OF_LOG_VERBOSE, "ofxIpCamera: Inited");

    return bGrabberInited;
}

//----------------------------------------------------------
void ofxIpCamera::clear() {
    pixels.clear();
    pixelsTemp.clear();
	tex.clear();
    bGrabberInited = false;
}

//----------------------------------------------------------
void ofxIpCamera::update() {
	// you need to call init() before running!
	assert(bGrabberInited);

	this->lock();
    
    if(bNeedsUpdate) {
    
        // copy pixels from temp pixels
        pixels = pixelsTemp;
    
        // update texture
        if(bUseTexture) {
            tex.loadData(pixels.getPixels(), width, height, GL_RGB);
        }
        
        bNeedsUpdate = false;
        bIsFrameNew = true;
    }
    this->unlock();
}

//----------------------------------------------------------
bool ofxIpCamera::grabFrame() {
	// you need to call init() before running!
	assert(bGrabberInited);
    
    Poco::Net::HTTPClientSession client;
    setupSession(client);
    
	return grabOneFrame(client);
}

//------------------------------------
void ofxIpCamera::setUseTexture(bool bUse) {
	bUseTexture = bUse;
}

//we could cap these values - but it might be more useful
//to be able to set anchor points outside the image

//----------------------------------------------------------
void ofxIpCamera::setAnchorPercent(float xPct, float yPct) {
    if(bUseTexture) tex.setAnchorPercent(xPct, yPct);
}

//----------------------------------------------------------
void ofxIpCamera::setAnchorPoint(int x, int y) {
    if(bUseTexture) tex.setAnchorPoint(x, y);
}

//----------------------------------------------------------
void ofxIpCamera::resetAnchor() {
   	if(bUseTexture) tex.resetAnchor();
}

//----------------------------------------------------------
void ofxIpCamera::draw(float _x, float _y, float _w, float _h) {
    if(bUseTexture) tex.draw(_x, _y, _w, _h);    
}

//----------------------------------------------------------
void ofxIpCamera::draw(float _x, float _y) {
	draw(_x, _y, (float)width, (float)height);
}

//----------------------------------------------------------
float ofxIpCamera::getHeight() {
	return (float)height;
}

//----------------------------------------------------------
float ofxIpCamera::getWidth() {
	return (float)width;
}

/* ***** PRIVATE ***** */

void ofxIpCamera::setupSession(Poco::Net::HTTPClientSession& client) {
    client.setHost(uri.getHost());
    if(uri.getPort() != 0)
    	client.setPort(uri.getPort());
        
    client.setKeepAlive(true); // use a persistent connection while running
    client.setTimeout(Poco::Timespan(2, 0)); // s, ms; this is live, dont wait too long for a response
}

bool ofxIpCamera::grabOneFrame(Poco::Net::HTTPClientSession& client) {
	// send request
    Poco::Net::HTTPRequest request;
    request.setVersion(Poco::Net::HTTPMessage::HTTP_1_1);
    request.setURI(uri.getPathAndQuery());
    credentials.authenticate(request);
    try {
        //request.write(cout);	// print header
        client.sendRequest(request);
    }
    catch(Poco::Exception& e) {
        ofLog(OF_LOG_ERROR, "ofxIpCamera: "+e.displayText());
        return false;
    }
    
    // receive response
    Poco::Net::HTTPResponse response;
    istream* responseBody;
    try {
        responseBody = (istream*) &client.receiveResponse(response);
    }
    catch(Poco::Net::NoMessageException& e) {
        // catch no message exception when shutting down thread
        return false;
    }
    //response.write(cout);	// print header
    
    // check response
    if(response.getStatus() != Poco::Net::HTTPResponse::HTTP_OK) {
        ofLog(OF_LOG_ERROR, "ofxIpCamera: Got response: "+
              ofToString(response.getStatus())+" "+response.getReason());
        return false;
    }
    if(response.getContentLength() == 0) {
        ofLog(OF_LOG_ERROR, "ofxIpCamera: Received content body is empty");
        return false;
    }
    
    // grab image from response into memory
    if(response.getContentType() != "image/jpeg") {
        ofLog(OF_LOG_ERROR, "ofxIpCamera: Received content type is not \"image/jpeg\"");
        return false;
    }
        
    // write raw response into byte buffer
    Poco::Buffer<uint8_t> buffer(response.getContentLength());
    responseBody->read((char*) buffer.begin(), response.getContentLength());
    
    // decode and load image from raw memory
    FIMEMORY* imageMem = FreeImage_OpenMemory(buffer.begin(), response.getContentLength());
    if(imageMem == NULL) {
        ofLog(OF_LOG_ERROR, "ofxIpCamera: FreeImage_OpenMemory failed");
        return false;
    }
    
    FREE_IMAGE_FORMAT format = FreeImage_GetFileTypeFromMemory(imageMem, 0);
    if(format != FIF_JPEG) {
        ofLog(OF_LOG_ERROR, "ofxIpCamera: Image is not jpeg format: "+ofToString(format));
        FreeImage_CloseMemory(imageMem);
        return false;
    }
    
    FIBITMAP* image = FreeImage_LoadFromMemory(format, imageMem, 0);
    if(image == NULL) {
        ofLog(OF_LOG_ERROR, "ofxIpCamera: FreeImage_LoadFromMemoryFailed");
        FreeImage_CloseMemory(imageMem);
        return false;
    }
    
    // resize if needed
    if(width != FreeImage_GetWidth(image) || height != FreeImage_GetHeight(image)) {
        ofLog(OF_LOG_WARNING, "ofxIpCamera: Resizing image from "+
              ofToString((int) FreeImage_GetWidth(image))+"x"+
              ofToString((int)FreeImage_GetHeight(image))+
              " to "+ofToString(width)+"x"+ofToString(height));
        FreeImage_Rescale(image, width, height, FILTER_BOX);	// fastest rescale filter
    }
    
    // load image into pixels temp
    this->lock();
    int bpp = FreeImage_GetBPP(image);
    int scanWidth = FreeImage_GetPitch(image);
    FreeImage_ConvertToRawBits(pixelsTemp.getPixels(), image, scanWidth, bpp,
        FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, true);
    
    // swap RGB
    pixelsTemp.swapRgb();
    
    bNeedsUpdate = true;
    
    this->unlock();

    // cleanup
    FreeImage_CloseMemory(imageMem);
    FreeImage_Unload(image);
    
    return true;
}

void ofxIpCamera::threadedFunction() {    	
    // setup http session
    Poco::Net::HTTPClientSession client;
	setupSession(client);

	if(bVerbose) {
    	cout << "ofxIpCamera: Thread started" << endl;
    	cout << "ofxIpCamera: Uri \"" << uri.toString() << "\"" << endl;
    	cout << "ofxIpCamera: Host " << client.getHost() << " Port " << client.getPort() << endl;
    }

	while(isThreadRunning()) {
    	if(!grabOneFrame(client))
        	break;
    }
    
    if(bVerbose)
    	cout << "ofxIpCamera: Thread stopped" << endl;
}
