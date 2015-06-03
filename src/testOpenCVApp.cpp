#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/Capture.h"

#include "CinderOpenCv.h"

using namespace ci;
using namespace ci::app;
using namespace cv;

static const int WIDTH = 640, HEIGHT = 480;

class testOpenCVApp : public AppNative {
  public:
	void setup();
	void update();
	void draw();
	void removeGreen(const Mat& myImage, Mat& Result);
	void sharpen(const Mat& myImage, Mat& Result);
	
	CaptureRef		mCapture;
	gl::Texture		mTexture;
};

void testOpenCVApp::setup()
{
	// The included image is copyright Trey Ratcliff
	// http://www.flickr.com/photos/stuckincustoms/4045813826/
	
	ci::Surface8u surface( loadImage( loadAsset( "dfw.jpg" ) ) );
	cv::Mat input( toOcv( surface ) );
	cv::Mat output;

	//init camera
	try {
		mCapture = Capture::create(640, 480, Capture::findDeviceByNameContains("Front"));
		mCapture->start();
	}
	catch (...) {
		console() << "Failed to initialize capture" << std::endl;
	}

}   

void testOpenCVApp::update()
{
	//mTexture = gl::Texture::create(mCapture->getSurface());
	Mat input = toOcv(mCapture->getSurface()), output;
	removeGreen(input, output);
	mTexture = gl::Texture( fromOcv( output ) );
}

void testOpenCVApp::draw()
{
	gl::clear(Color(0.0f, 0.0f, 0.0f));
	gl::setMatricesWindow(getWindowWidth(), getWindowHeight());
	gl::draw(mTexture, Rectf(0, 0, WIDTH, HEIGHT));

	/*
	if( mTexture ) {
		glPushMatrix();
		#if defined( CINDER_COCOA_TOUCH )
		//change iphone to landscape orientation
		gl::rotate( 90.0f );
		gl::translate( 0.0f, -getWindowWidth() );

		Rectf flippedBounds( 0.0f, 0.0f, getWindowHeight(), getWindowWidth() );
		gl::draw( mTexture, flippedBounds );
		#else
		gl::draw( mTexture );
		#endif
		glPopMatrix();
	}
*/
}

void testOpenCVApp::removeGreen(const Mat& myImage, Mat& Result)
{
	Result.create(myImage.size(), myImage.type());
	const int nChannels = myImage.channels();

	for (int j = 0; j < myImage.rows; ++j)
	{
		for (int i = 0; i < myImage.cols; ++i)
		{
			cv::Vec3b color = myImage.at<cv::Vec3b>(j, i);
			int b = color[0], g = color[1], r = color[2];
			if (g - r > 20 && g - b >20){
				Result.at<cv::Vec3b>(j, i) = { 0, 0, 0 };
				//console() << "yes" << std::endl;
			}
			else {
				Result.at<cv::Vec3b>(j, i) = myImage.at<cv::Vec3b>(j, i);
			}
		}
	}
}

void testOpenCVApp::sharpen(const Mat& myImage, Mat& Result)
{
	CV_Assert(myImage.depth() == CV_8U);

	Result.create(myImage.size(), myImage.type());
	const int nChannels = myImage.channels();

	for (int j = 1; j < myImage.rows - 1; ++j)
	{
		const uchar* previous = myImage.ptr<uchar>(j - 1);
		const uchar* current = myImage.ptr<uchar>(j);
		const uchar* next = myImage.ptr<uchar>(j + 1);

		uchar* output = Result.ptr(j);

		for (int i = nChannels; i < nChannels * (myImage.cols - 1); ++i)
		{
			*output++ = saturate_cast<uchar>(5 * current[i]
				- current[i - nChannels] - current[i + nChannels] - previous[i] - next[i]);
		}
	}

	Result.row(0).setTo(Scalar(0));
	Result.row(Result.rows - 1).setTo(Scalar(0));
	Result.col(0).setTo(Scalar(0));
	Result.col(Result.cols - 1).setTo(Scalar(0)); 

}
CINDER_APP_NATIVE( testOpenCVApp, RendererGl )
