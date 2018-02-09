TARGET=GradientImage

$(TARGET): GradientImage.cpp GradientImage.hpp
	g++ -g -o $@ GradientImage.cpp `Magick++-config --cppflags --cxxflags --libs` `gsl-config --cflags --libs`

.PHONY: clean
clean:
	rm -rf $(TARGET) *.dSYM
