//
//	GradientImage.hpp
//
//		勾配画像を作成
//
//	Created by W.Ohyama   Feb. 2007
//

//
//	compile : g++ -o GradientImage GradientImage.cpp `Magick++ --cppflags --libs` `gsl-config --cflags --libs`
//

#include<iostream>
#include<algorithm>
#include<string>
#include<cmath>
#include<Magick++.h>
#include<gsl/gsl_statistics.h>

#include"GradientImage.hpp"

using namespace Magick;
using namespace std;

//##########################################################

GradientImage::GradientImage ( void ) : Image( )
{
  pixel_d = NULL;
}

GradientImage::GradientImage( std::string &imgSpec_ )
  : Image(imgSpec_)
{
  pixel_d = NULL;
  mag = NULL;
  dir = NULL;  
}


Geometry
GradientImage::get_bounding_box ( void )
{
  Geometry geom;
  Color white ( "white" );

  size_t l=columns(), r=0, t=rows(), b=0;
  for( size_t y = 0; y < rows(); ++y )
    for( size_t x = 0; x < columns(); ++x )
      {
	if( pixelColor( x, y ) != white )
	  {
	    l = min ( l, x );
	    r = max ( r, x );

	    t = min ( t, y );
	    b = max ( b, y );
	  }
	
      }

  geom.xOff( l ); geom.yOff( t );
  geom.width( r - l ); geom.height( b - t );

  return geom;
}


Geometry
GradientImage::get_center_point ( void )
{
  Geometry geom;
  Color white ( "white" );

  double cx, cy;
  double num;
  cx = 0;
  cy = 0;
  num = 0;
  for( size_t y = 0; y < rows(); ++y )
    for( size_t x = 0; x < columns(); ++x )
      {
	if( pixelColor( x, y ) != white )
	  {
	    cx += x;
	    cy += y;
	    num += 1;
	  }
      }

  cx /= num;
  cy /= num;

  geom.xOff( size_t(cx) ); geom.yOff( size_t(cy) );
  geom.width( 0 ); geom.height( 0 );

  cout << string(geom) << endl;
  return geom;
}



//
//	画像のサイズ正規化
//
//
void
GradientImage::size_normalization( Geometry geom )
{
  Geometry text_bb = get_bounding_box ( );

  crop(text_bb);
  size(text_bb);

  cout << "sizeof image = " << string( size()) << endl;
  
  Geometry centerPoint = get_center_point ();

  //display();

  GradientImage tmpImgLU(*this);

  tmpImgLU.crop ( Geometry( centerPoint.xOff(), centerPoint.yOff(), text_bb.xOff(), text_bb.xOff() ) );
  //tmpImgLU.display();
  
  GradientImage tmpImgRU(*this);
  tmpImgRU.crop ( Geometry( tmpImgRU.columns()-centerPoint.xOff(),
			    centerPoint.yOff(),
			    centerPoint.xOff() + text_bb.xOff(),
			    text_bb.yOff() ) );
  //tmpImgRU.display();
  GradientImage tmpImgLD(*this);
  tmpImgLD.crop ( Geometry( centerPoint.xOff(),
			    tmpImgLD.rows() - centerPoint.yOff(),
			    text_bb.xOff(),
			    centerPoint.yOff() + text_bb.yOff() ) );
  //tmpImgLD.display();
  GradientImage tmpImgRD(*this);
  tmpImgRD.crop ( Geometry( tmpImgRD.columns() - centerPoint.xOff(),
			    tmpImgRD.rows() - centerPoint.yOff(),
			    centerPoint.xOff() + text_bb.xOff(),
			    centerPoint.yOff() + text_bb.yOff() ) );
  //tmpImgRD.display();

  scale(geom);
  
  draw( DrawableCompositeImage( 0, 0,
				geom.width()/2, geom.height()/2,
				tmpImgLU ) );
  draw( DrawableCompositeImage( geom.width()/2, 0,
				geom.width()/2, geom.height()/2,
				tmpImgRU ) );
  draw( DrawableCompositeImage( 0, geom.height()/2,
				geom.width()/2, geom.height()/2,
				tmpImgLD ) );
  draw( DrawableCompositeImage( geom.width()/2, geom.height()/2,
				geom.width()/2, geom.height()/2,
				tmpImgRD ) );
  
  //scale( geom );
}


//
//	画像の正準化
//
void 
GradientImage::canonicalization( void )
{
  int size = columns() * rows();
  if ( this -> pixel_d == NULL )
    {
      pixel_d = new double [ size ];
    }
  PixelPacket *org_pixel = this->getPixels( 0, 0, columns(), rows() );

  for( int i = 0; i < size; ++i )
    {
      pixel_d[i] = org_pixel[i].red;
    }
  
  double mean = gsl_stats_mean ( pixel_d, 1, size );
  double variance  = gsl_stats_sd ( pixel_d, 1, size );

  cout << "mean = " << mean << ": variance = " << variance << endl;
  
  for( int i = 0; i < size; ++i )
    {
      pixel_d[i] = (pixel_d[i] - mean) / variance;
    }

  ColorGray gray;
  for( int i = 0; i < size; ++i )
    {
      double shade = max( 0.0, min(1.0, ((pixel_d[i] + 1)/2) ) );
      gray.shade(shade);
      org_pixel[i] = gray;
    }
  
  syncPixels();
}


//
//	勾配画像生成
//
void
GradientImage::gradient( void )
{
	
  int size = columns() * rows();
  
  if( this->pixel_d == NULL )
    {
      this -> canonicalization ();
    }
	
  if( this->mag == NULL )
    {
      mag = new double [size];
    }
	
  if( this->dir == NULL )
    {
      dir = new double [size];
    }
  
  double str_x[] = {-1,0,1,
										-2,0,2,
										-1,0,1};
  double str_y[] = {-1,-2,-1,
										0, 0, 0,
										1, 2, 1};
	
  double *gx = new double [size];
  apply_filt( pixel_d, columns(), rows(), str_x, 3, 3, 1, gx );
	
  double *gy = new double [size];
  apply_filt( pixel_d, columns(), rows(), str_y, 3, 3, 1, gy );
	
  for( int i = 0; i < size; ++i )
    {
      mag[i] = hypot( gx[i], gy[i] );
      dir[i] = atan2 ( gy[i], gx[i] );
    }
	
#if 0
  double  max_mag, min_mag;
  gsl_stats_minmax( &min_mag, &max_mag, mag, 1, size );
  double c = 100;
  for( size_t y = 0; y < rows(); ++y )
    for( size_t x = 0; x < columns(); ++x )
      {
				double r = hypot( x-c, y-c );
				int p = y * columns() + x;
				if( r < c )
					{
						mag[p] = (r/c)*(max_mag-min_mag) + min_mag;
						dir[p] = atan2( y-c, x-c );
					}
				else
					{
						mag[p] = 0;
						dir[p] = 0;
					}
      }
#endif  

  this -> sync_mag ();
  delete[] gx;
  delete[] gy;
}


void 
GradientImage::sync_mag ( void )
{
  this->type(TrueColorType);

  int size = columns() * rows();

  PixelPacket *org_pixel = this->getPixels( 0, 0, columns(), rows() );

  double  max_mag, min_mag;
  gsl_stats_minmax( &min_mag, &max_mag, mag, 1, size );

  ColorHSL hsl_c;

  for( int i = 0; i < size; ++i )
    {
      hsl_c.hue( (dir[i]+M_PI) / (2*M_PI) );
      hsl_c.saturation( 1.0 );
      hsl_c.luminosity( (mag[i]-min_mag)/(max_mag-min_mag) * .8 );

      org_pixel[i] = hsl_c;
    }

  syncPixels();
}



template <class T>
double * GradientImage::apply_filt ( T*  img,
				     int   width,
				     int   height,
				     T*  str,
				     int   str_w,
				     int   str_h,
				     double denominator,
				     T*    ret_pixel)
{

  int x, y, p;
  int i, j;
  
  double ret;

  for( i = 0; i < width * height ; i++ )
    ret_pixel[i] = img[i];	
  
  for( y = 0; y < height-str_h; y++ )
    for( x = 0; x < width-str_w; x++ )
      {
	p = (y+str_h/2) * width + (x+str_w/2);
	ret = 0;
	
	for( j = 0; j < str_h; j++ )
	  for( i = 0; i < str_w; i++ ) 
	    {
	      ret += str[j*str_w+i] * img[ (y+j) * width + (x+i) ];
	    }

	ret_pixel[p] = ret / denominator;
      }
  
  return ret_pixel;
}



//********************************************************************************************
//
//	main
//
//********************************************************************************************
int main ( int argc, char *argv[] )
{
  std::string  fname = argv[1];
  GradientImage image ( fname );
	
  std::string  fname_png ( fname );
  fname_png.erase(0, fname_png.find_last_of("/")+1);
  fname_png.replace(fname_png.find_last_of("."),4, ".png");
	
	
  // original Image 
  //image.display();
  std::string  fname_org = string("01org_")+fname_png;
	try {
		image.write ( fname_org );
	}catch ( Magick::WarningCoder &e )
		{
			;
		}

	
  Geometry geom ( argv[2] );
	
  // Image after Cropping and size normalization
  // image.size_normalization ( geom );

	image.scale(geom);
	
  //image.display();
  std::string  fname_norm = string("02norm_")+fname_png;

	try {
		image.write ( fname_norm );
	}catch ( Magick::WarningCoder &e )
		{
			;
		}
	
  
  image.gaussianBlur( 3.0, 3.0);
  image.normalize();
	
  // Image after Smthing  
	//  image.display();
  std::string  fname_smth = string("03smth_")+fname_png;
	try {
		image.write ( fname_smth );
	}catch ( Magick::WarningCoder &e )
		{
			;
		}

	
  // 正準化
  image.canonicalization( );
  // image.display();
  std::string  fname_cano = string("04cano_")+fname_png;
	try {
		image.write ( fname_cano );
	}catch ( Magick::WarningCoder &e )
		{
			;
		}

  
  // 勾配画像
  image.gradient( );
  std::string  fname_grad = string("05grad_")+fname_png;
	try {
		image.write ( fname_grad );
	}catch ( Magick::WarningCoder &e )
		{
			;
		}

  // image.display();
  
  //image.write ( fname_smth );
  
  // Image after Gradient
  //image.get_gradient();
  
  
  return 0;
}
