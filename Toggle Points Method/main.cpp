/*
* @file Digitizing_Circle_part2.cpp
* @brief Calculates a best fit circle and displays the plot on the grid when user toggles points on the grid
*
* @author: Praneeth Eddu
* Contact: praneetheddu@gatech.edu
*/

#include <iostream>
#include <vector>
#include <stdlib.h>
#include "Best_Fitting_Circle.h"
#include "Grid_Points.h"


// Instantiate global variables
Grid_Points grid_points[20][20]; //2D array of Grid point class
const unsigned int grid_spacing = 40; //Grid Spacing

// Parameters to check for mouse activity
bool left_button_clicked = false;
bool left_button_released = true;
bool clicked_flag = true;
bool released_flag = true;
bool draw_circ = false;

std::vector<cv::Point> selected_points; //Vector for point selection
cv::Mat background_with_grid; // Original grid image with no plots

bool circle_generated = false; //Check to see if a circle aldready exists

// Button Dimensions
cv::Point generate_button_top_left = cv::Point(700, 820);
cv::Point generate_button_bottom_right = cv::Point(790, 845);
cv::Point reset_top_left = cv::Point(580, 820);
cv::Point reset_bottom_right = cv::Point(670, 845);


/**
 * Overlays grid points on white background
 *
 *
 * @param background image of white background
 * @param grid_spacing space between the grid points
 */
void overlay_grid_points(cv::Mat& background, unsigned int grid_spacing) {
	std::cout << "Creating Grid ...\n";

	// Layout the points on the white background
	for (unsigned int i = 1; i < 21; ++i) {
		for (unsigned int j = 1; j < 21; ++j) {
			grid_points[i - 1][j - 1].set_params(cv::Point(grid_spacing * i, grid_spacing * j), false);
			cv::rectangle(background, grid_points[i - 1][j - 1].point, grid_points[i - 1][j - 1].grid_offset, grid_points[i - 1][j - 1].color, -1, 8, 0);
		}
	}

	//Add a button for Generate
	cv::rectangle(background, generate_button_top_left, generate_button_bottom_right, cv::Scalar(199, 207, 196), -1, 8, 0);
	cv::putText(background, "Generate", cv::Point(705, 840), cv::FONT_HERSHEY_DUPLEX, 0.6, cv::Scalar(0, 0, 0), 1, 8, false);


	//Add a button for Reset
	cv::rectangle(background, reset_top_left, reset_bottom_right, cv::Scalar(199, 207, 196), -1, 8, 0);
	cv::putText(background, "Reset", cv::Point(600, 840), cv::FONT_HERSHEY_DUPLEX, 0.6, cv::Scalar(0, 0, 0), 1, 8, false);

	std::cout << "Grid completed ...\n";
}

/**
 * Check to see if the mouse click overlaps generate button
 *
 * @param x x coordinate of the mouse click
 * @param y y coordinate of the mouse click
 * @return true if generate button overlaps mouse click coordinates
 *
 */
bool click_contains_generate_box(int x, int y) {
	if (x > generate_button_top_left.x&& x < generate_button_bottom_right.x)
	{
		if (y > generate_button_top_left.y&& y < generate_button_bottom_right.y)
		{
			// User mouse click overlaps generate button
			std::cout << "Generate clicked" << std::endl;
			return true;
		}
	}
	return false;
}
/**
 * Check to see if the mouse click overlaps reset button
 *
 * @param x x coordinate of the mouse click
 * @param y y coordinate of the mouse click
 * @return true if reset button overlaps mouse click coordinates
 *
 */
bool click_contains_reset(int x, int y) {
	if (x > reset_top_left.x&& x < reset_bottom_right.x)
	{
		if (y > reset_top_left.y&& y < reset_bottom_right.y)
		{
			// User mouse click overlaps reset button
			std::cout << "Reset clicked" << std::endl;
			return true;
		}
	}
	return false;
}

/**
* Clears any objects drawn on the grid and displays the original grid
*
* @param populated_image Image that needs to be cleared
*/
void reset_grid(cv::Mat& populated_image) {
	background_with_grid.copyTo(populated_image); // Reset to original grid
	circle_generated = false;
	selected_points.clear(); //clear the selected points
}

/**
*
* Callback function that recognizes mouse clicks. This function allows the user to toggle points on
* grid to generate the best fit circle by clicking the generate button. The Best_Fitting_Circle class
* is get radius and circle center of the best fit circle
*
* @param event name of the mouse activity
* @param x x coordinate of the mouse cursor
* @param y y coordinate of the mouse cursor
* @param flags any flags that user passed in
* @param background image background
*/
void mouse_activity(int event, int x, int y, int flags, void* background) {
	cv::Mat& img = *((cv::Mat*)(background)); // 1st cast it back, then deref
	bool is_circle_computable; //Check to see if circle is computable

	if (event == cv::EVENT_LBUTTONDOWN)
	{
		// Recognize user's mouse click and set appropriate flags
		left_button_clicked = true;
		left_button_released = false;

		if (click_contains_generate_box(x, y)) //Check to see if the generate button is clicked
		{
			if (selected_points.size() >= 3 && !circle_generated) // User has to select atlease 3 points
			{

				// Create an instance of Best Fitting Circle class
				Best_Fitting_Circle* best_fit_circle = new Best_Fitting_Circle(selected_points);
				is_circle_computable = best_fit_circle->compute_best_fit_circle(); //Check to see if the circle can be computed

				if (is_circle_computable)
				{
					// if the circle is computable, get radius and center coordinates
					double radius = best_fit_circle->get_radius();
					Circle_Center circle_center = best_fit_circle->get_center_coordinate();

					// Check to see if the circle's center can fit in grid. Also accounts for invalid circle coordinates
					if (circle_center.x < 850 && circle_center.y < 850)
					{
						// Draw the best fit circle
						cv::circle(img, cv::Point(circle_center.x, circle_center.y), radius, cv::Scalar(255, 0, 0), 2, 8, 0);
						circle_generated = true;
						imshow("Digitizing Circles", img);
					}
					else
					{
						// Data points are not feasible for generating best fit circle
						std::cout << "Circle is too big or invalid. Please try different points" << std::endl;
					}
				}
			}

			else
			{
				// User has to select at least three points
				std::cout << "Please select upto 3 points" << std::endl;;
			}

		}
		else if (click_contains_reset(x, y))
		{
			// Reset the grid if user presses the reset button
			reset_grid(img);
			// Display new grid
			imshow("Digitizing Circles", img);
			std::cout << "Grid Reset\n" << std::endl;;
		}
	}
	if (event == cv::EVENT_LBUTTONUP)
	{
		//Recognize user's mouse release and set appropriate flags
		left_button_released = true;
		left_button_clicked = false;
	}

	if (left_button_clicked && clicked_flag)
	{
		clicked_flag = false;
		released_flag = true;
	}


	if (left_button_released && released_flag)
	{
		draw_circ = true;
		released_flag = false;
		clicked_flag = true;

	}

	if (draw_circ && left_button_released)
	{
		draw_circ = false;
		if (x % grid_spacing < 5 && y % grid_spacing < 5) // Check to see if region of grid point overlaps mouse click coordinates
		{
			//Calculate index of overlapped grid coordinates
			int indx_x = (x - (x % grid_spacing)) / 40;
			int indx_y = (y - (y % grid_spacing)) / 40;

			grid_points[indx_x - 1][indx_y - 1].toggle(); //Toggle the grid point 

			if (grid_points[indx_x - 1][indx_y - 1].get_is_selected()) // Check if the grid point is marked for selection
			{
				selected_points.push_back(grid_points[indx_x - 1][indx_y - 1].point);
			}
			else //if not selected, remove the grid point from the selected points list
			{
				//Remove the element from the selected points array if the point is not present
				for (auto point : selected_points) {
					if (point.x == grid_points[indx_x - 1][indx_y - 1].point.x && point.y == grid_points[indx_x - 1][indx_y - 1].point.y)
					{
						selected_points.erase(std::remove(selected_points.begin(), selected_points.end(), point), selected_points.end());
					}
				}
			}
			// Color the selected grid point based on the toggle value
			cv::rectangle(img, grid_points[indx_x - 1][indx_y - 1].point, grid_points[indx_x - 1][indx_y - 1].grid_offset, grid_points[indx_x - 1][indx_y - 1].color, -1, 8, 0);
			imshow("Digitizing Circles", img); //Display the image
		}
	}

}


int main() {
	//Create a white background of dimenstions 850 x 850
	cv::Mat white_background(850, 850, CV_8UC3, cv::Scalar(255, 255, 255));

	if (white_background.empty()) {
		std::cout << "Looks like something went wrong. Please try again" << std::endl;
		return -1; // Unsucessful image loading
	}
	//Overlay the image with grid points
	overlay_grid_points(white_background, grid_spacing);

	// Create a copy of the image with grid to use when circles need to be cleared
	white_background.copyTo(background_with_grid);

	//Create a window
	cv::namedWindow("Digitizing Circles", 1);

	//Call the Mouse Click callback function if detected a mouse click
	cv::setMouseCallback("Digitizing Circles", mouse_activity, &white_background); //Mouse Call Back

	//Display the image
	imshow("Digitizing Circles", white_background);
	cv::waitKey(0);

}

