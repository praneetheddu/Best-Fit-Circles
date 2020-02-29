/**
* @file Grid_Points.h
* @brief Header file for Grid_Points to store properties such as location and toggle state of each grid point
*
* @author: Praneeth Eddu
* Contact: praneetheddu@gatech.edu
*/
#include <opencv2/opencv.hpp>

#pragma once
#ifndef GRID_POINTS
#define GRID_POINTS

class Grid_Points
{
public:
	cv::Point point;
	cv::Point grid_offset;
	bool is_selected = false;
	cv::Scalar color;
	void set_params(cv::Point, bool);
	void toggle();
	bool get_is_selected();
};

#endif

