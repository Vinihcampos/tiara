#include "Canvas.h"
#include <string>
#include <cstring>
#include <iostream>

tiara::Canvas::Canvas(size_t width_, size_t height_, float norm) : width{width_}, height{height_} {
	canvas = std::make_unique<component_t[]>(width * height * 3);
	std::memset(canvas.get(), 0, width * height * 3 * sizeof(component_t));

	for(int i = 0; i < 3; ++i){
		for(int j = 0; j < 3; ++j){
			kernel[i][j] *= norm; 
		}
	}
}

void tiara::Canvas::fill(const Color & color, const Point2d p, const Color & borderColor){

	//std::cout << "(x,y): " << p.x << "," << p.y << std::endl;

	if(p.x >= 0 && p.x < width && p.y >= 0 && p.y < height && !(get(p) == borderColor) && !(get(p) == color)){
		pixel(p, color);
		fill(color, Point2d(p.x, p.y+1), borderColor);
		fill(color, Point2d(p.x+1, p.y), borderColor);
		fill(color, Point2d(p.x, p.y-1), borderColor);
		fill(color, Point2d(p.x-1, p.y), borderColor);
	}

	//std::memset(canvas.get(),                       color.r, width * height * sizeof(component_t));
	//std::memset(canvas.get() + (width * height),    color.g, width * height * sizeof(component_t));
	//std::memset(canvas.get() + (width * height * 2),color.b, width * height * sizeof(component_t));
}

tiara::Color tiara::Canvas::get(const tiara::Point2d & p) const{
	return tiara::Color( canvas[p.y * width + p.x],
	                     canvas[p.y * width + p.x + width * height],
	                     canvas[p.y * width + p.x + width * height * 2] );
}

tiara::Color tiara::Canvas::get(const size_t & idx) const{
	return tiara::Color( canvas[idx],
	                     canvas[idx + width * height] ,
	                     canvas[idx + width * height * 2] );
}

void tiara::Canvas::pixel(const Point2d & p, const Color & color){
	int real_p = p.y * width + p.x;
	for(int i = 0; i < 3; ++i) 
		canvas[real_p + i * width * height] = color[i];
}

void tiara::Canvas::pixels(const Point2d ps[], const size_t & p_size, const Color & color){
	for(size_t i = 0; i < p_size; ++i) 
		pixel(ps[i], color);
}

void tiara::Canvas::rectangle(const Point2d & tl, const Point2d & br, const Color & color, const bool filled){
	
	// Draw horizontal lines
	for(size_t c = tl.y; c <= br.y; ++c){
		pixel(Point2d(tl.x, c), color);
		pixel(Point2d(br.x, c), color);
	}

	// Draw vertical lines
	for(size_t r = tl.y; r <= br.y; ++r){
		pixel(Point2d(r, tl.y), color);
		pixel(Point2d(r, br.y), color);
	}

	if(filled){
		for(size_t r = tl.x + 1; r < br.x; ++r){
			for(size_t c = tl.y + 1; c < br.y; ++c){
				pixel(Point2d(r,c), color);
			}
		}
	}	
}

void tiara::Canvas::antialiasing(){
	std::unique_ptr<component_t[]> copy = std::make_unique<component_t[]>(width * height * 3);
	float * kernel_results = new float[3];
	for(int i = 0; i < height; ++i){
		for(int j = 0; j < width; ++j){
			kernel_results = multiply(i-1, j-1, i+1, j+1);
			copy[i * width + j] = kernel_results[0];
			copy[i * width + j + width * height] = kernel_results[1];
			copy[i * width + j + width * height * 2] = kernel_results[2];
		}
	}

	canvas = std::move(copy);
	delete kernel_results;
}

float * tiara::Canvas::multiply(int sh, int sw, int fh, int fw){
	float * sum = new float[3];
	sum[0] = 0;
	sum[1] = 0;
	sum[2] = 0;

	int kernel_index = 0;

	for(int i = sh; i <= fh; ++i){
		for(int j = sw; j <= fw; ++j){
			if(i < 0 || j < 0 || i >= height || j >= width){
				continue;
			}else{
				sum[0] += canvas[i * width + j] * kernel[kernel_index / 3][kernel_index % 3];
				sum[1] += canvas[i * width + j + width * height] * kernel[kernel_index / 3][kernel_index % 3];
				sum[2] += canvas[i * width + j + width * height * 2] * kernel[kernel_index / 3][kernel_index % 3];
			}
			kernel_index++;	
		}
	}

	return sum;
}