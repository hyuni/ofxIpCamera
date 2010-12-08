#ifndef _OFX_IP_CAMERA
#define _OFX_IP_CAMERA

#include "ofConstants.h"
#include "ofTexture.h"
#include "ofGraphics.h"
#include "ofTypes.h"

#include "ofxThread.h"

#include "Poco/URI.h"
#include "Poco/Net/HTTPBasicCredentials.h"
#include "Poco/Net/HTTPClientSession.h"

// Authorization credentials created with: 
/*
	camera: http://192.168.5.85/operator/basic.shtml
    
	References:
    http://pocoproject.org/docs/
    http://freeimage.sourceforge.net/fip/index.html
    http://www.cplusplus.com/reference/iostream/istream/get/
    
    
    http://www.openframeworks.cc/forum/viewtopic.php?f=8&t=3545
*/

class ofxIpCamera : public ofBaseVideo, protected ofxThread
{

	public :

		ofxIpCamera();
		virtual ~ofxIpCamera();

		/// is the current frame new?
		bool isFrameNew() {return true;}
        
        /// open the connection and start grabbing images
        void open();
        
        /// close the connection and stop grabbing images
		void close();
        
        /// initialize resources, must be called before open()
		bool init(int w, int h, bool bTexture=true);
        
        /// clear resources
        void clear();
        
        /// get the raw pixels of the most recent frame
		unsigned char 	* getPixels();
		
        ofTexture &		getTextureReference();
		
        void 			setVerbose(bool bTalkToMe);
        
        /**
        	\brief	set the camera host address
            \param	host	the address or ip of the camera
            
            builds the uri automatically using the given width and height,
            call after init
            
            http://192.168.5.85/demo/demo.shtml?type=jpg&size=320x180&videoFormat=mjpeg&color=0&clock=0
            
            the uri is of type:
            
            http://host/axis-cgi/jpg/image.cgi?resolution=320x180
        */
        void setHost(string host);
        
        /**
        	\brief	set the camera uri including query
            \param	uri	the camera uri
            
            overrides the prebuilt uri set by setHost
        */
		void setUri(string uri);
        
        /**
        	\brief	set the access credentials to the camera (if any)
            \param	user	username
            \param	password	password
            
            sets HTTP Authorization: Basic username:password
            from http://en.wikipedia.org/wiki/Basic_access_authentication
            
            see also
            http://www.motobit.com/util/base64-decoder-encoder.asp
        */
        void setCredentials(string user, string password);
        
		void 			setUseTexture(bool bUse);
		void 			draw(float x, float y, float w, float h);
		void 			draw(float x, float y);
		
        /**
        	\brief	updates the texture from the pixels
            
            call this function when using the the texture, otherwise the
            texture will not be updated from the grabbed pixels
        */
        void update();
        
        /**
        	\brief	grab a frame manually
            \return	true if frame grabbed successfully
        
        	non-threaded, do not use while thread is running
        */
        bool grabFrame();


		//the anchor is the point the image is drawn around.
		//this can be useful if you want to rotate an image around a particular point.
        void			setAnchorPercent(float xPct, float yPct);	//set the anchor as a percentage of the image width/height ( 0.0-1.0 range )
        void			setAnchorPoint(int x, int y);				//set the anchor point in pixels
        void			resetAnchor();								//resets the anchor to (0, 0)

		float 			getHeight();
		float 			getWidth();

		int			height;
		int			width;

	protected:

		Poco::URI 						uri;			// full path uri
    	Poco::Net::HTTPBasicCredentials credentials;	// encodes user name and pass

		bool					bUseTexture;
		ofTexture 				tex;				// the user tex
		bool 					bVerbose;
		bool 					bGrabberInited;
	    unsigned char * 		pixels;
        
    private:
    	
        // setup a session object with host, etc
        void setupSession(Poco::Net::HTTPClientSession& client);
        
        // grab one frame using the given session object
        bool grabOneFrame(Poco::Net::HTTPClientSession& client);
    
        // thread function
        void threadedFunction();
};

#endif
