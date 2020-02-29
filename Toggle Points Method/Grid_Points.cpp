/**
* @file Grid_Points.cpp
* @brief Source file for Grid_Points to store properties such as location and toggle state of each grid point
*
* @author: Praneeth Eddu
* Contact: praneetheddu@gatech.edu
*/

#include "Grid_Points.h"

/**
* Initializes the variables for a given grid point. Color for the grid point is chosen by its unique state
*
* @param point Location of the grid point
* @param is_selected checks to see if a grid point is selected by the user
*/
void Grid_Points::set_params(cv::Point point, bool is_selected) {

	this->point = point;
	this->is_selected = is_selected;
	(!is_selected) ? this->color = cv::Scalar(128, 128, 128) : this->color = cv::Scalar(255, 0, 0); // if selected point, set to blue else gray
	this->grid_offset.x = point.x + 5;
	this->grid_offset.y = point.y + 5;

}
/**
* Toggles the state of the grid point when the user clicks on the point
*
*/
void Grid_Points::toggle() {
	if (is_selected == false) {
		is_selected = true;
		this->color = cv::Scalar(255, 0, 0);
	}
	else {
		is_selected = false;
		this->color = cv::Scalar(128, 128, 128);
	}
}

/**
* returns the state of the grid point
*
* @return is_selected state of the grid point
*/
bool Grid_Points::get_is_selected() {
	return is_selected;
}