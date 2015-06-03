#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"

#include "CinderOpenCv.h"

using namespace ci;
using namespace ci::app;
using namespace cv;

class testOpenCVApp : public AppNative {
  public:
	void setup();
	void draw();
	void sharpen(const Mat& myImage, Mat& Result);
	
	gl::Texture	mTexture;
};

void testOpenCVApp::setup()
{
	// The included image is copyright Trey Ratcliff
	// http://www.flickr.com/photos/stuckincustoms/4045813826/
	
	ci::Surface8u surface( loadImage( loadAsset( "dfw.jpg" ) ) );
	cv::Mat input( toOcv( surface ) );
	cv::Mat output;

//	cv::medianBlur( input, output, 11 );
//	cv::Sobel( input, output, CV_8U, 0, 1 ); 
//	cv::threshold( input, output, 128, 255, CV_8U );
	sharpen(input, output);
	mTexture = gl::Texture( fromOcv( output ) );
}   

void testOpenCVApp::draw()
{
	gl::clear();
	gl::draw( mTexture );
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
