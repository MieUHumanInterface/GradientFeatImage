#ifndef GRADIENT_IMAGE_HPP_INCLUDED__
#define GRADIENT_IMAGE_HPP_INCLUDED__
//
//	GradientImage.hpp
//
//		勾配画像を作成
//
//	Created by W.Ohyama   Feb. 2007
//

#include<Magick++.h>
#include<string>

//
//	勾配画像を作成するクラス
//	（ベースクラス：Magick++::Image）
//


class GradientImage : public Magick::Image
{
private:
  double *pixel_d;
  double *mag;
  double *dir;
  
  // private function
  template <class T>
  double * apply_filt ( T*  img,
			int   width,
			int   height,
			T*  str,
			int   str_w,
			int   str_h,
			double denominator,
			T*    ret_pixel);

  void sync_mag ( void );

public:
  GradientImage(); // Default Constructor
  GradientImage(std::string &imgSpec_);

  void
  size_normalization( Magick::Geometry geom );

  void 
  canonicalization( void );

  void
  gradient( void );
    
  Magick::Geometry
  get_bounding_box ( void );
  
  Magick::Geometry
  get_center_point ( void );

};


#endif

