/*
* @file Best_Fitting_Circle.h
* @brief Header file for Best_Fitting_Circle which is used to generate best fit circle given at least three
* sets of points by computing an initial guess of the circle using the points and hten
* reducing the distances between the circle center and set of points.
*
*
* Code was adapted from research paper written by Maisonobe L. titled "Finding the circle that
* best fits a set of points."
* @source: http://www.spaceroots.org/documents/circle/circle-fitting.pdf
*
*
* @author: Praneeth Eddu
* Contact: praneetheddu@gatech.edu

*/

#include <opencv2/opencv.hpp>
#include <vector>

#pragma once
#ifndef BEST_FITTING_CIRCLE
#define BEST_FITTING_CIRCLE

struct Gradient {
	double x;
	double y;
};
struct Circle_Center {
	double x;
	double y;
};

class Best_Fitting_Circle
{
private:
	double radius_estimate;
	Circle_Center circle_center_est;
	double cost;
	std::vector<cv::Point> selected_points;
	double delta;
public:

	Best_Fitting_Circle(std::vector<cv::Point>);
	bool compute_best_fit_circle();
	bool converge(Gradient);
	double compute_lambda(std::vector<cv::Point>, Gradient);
	Gradient get_gradient_for_conjugate_gradient(std::vector<cv::Point>);
	double cost_function(std::vector<cv::Point>);
	double compute_radius_estimate(std::vector<cv::Point>);
	Circle_Center initial_estimate(std::vector<cv::Point> points);
	Circle_Center calculate_circumcenter(cv::Point, cv::Point, cv::Point, double);
	double get_distance(const int x1, const int x2, const int y1, const int y2);
	double get_radius();
	Circle_Center get_center_coordinate();
};
#endif
