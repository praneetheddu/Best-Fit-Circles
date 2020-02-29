#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <stdlib.h>


// Parameters to check for mouse activity
bool left_button_clicked = false;
bool left_button_released = true;
bool clicked_flag = true;
bool released_flag = true;
bool is_clicked = false;

// Circle coordinates
unsigned int center_x, center_y;
unsigned int circle_edge_x, circle_edge_y;


unsigned int grid_spacing = 40; // Grid Spacing
cv::Mat background_with_grid; // Original grid image with no plots
cv::Point grid_coordinates[20][20]; // Grid coordinates

/**
 * Overlays grid points on white background
 *
 *
 * @param background image of white background
 * @param grid_spacing space between the grid points
 */
void overlay_grid_points(cv::Mat& background, unsigned int grid_spacing) {
	std::cout << "Creating Grid ...\n";

	for (unsigned int i = 1; i < 21; ++i) {
		for (unsigned int j = 1; j < 21; ++j) {
			grid_coordinates[i - 1][j - 1] = cv::Point(grid_spacing * i, grid_spacing * j); //Storing Rectangle points  
			cv::rectangle(background, cv::Point(grid_spacing * i, grid_spacing * j), cv::Point((grid_spacing * i) + 5, (grid_spacing * j) + 5), cv::Scalar(128, 128, 128), -1, 8, 0);
		}
	}
	std::cout << "Grid completed ...\n";

}

/**
 * Calculates the distance between two coordinates
 *
 *
 * @param x1 First coordinate's x value
 * @param x2 Second coordinate's x value
 * @param y1 First coordinate's x value
 * @param y2 Second coordinate's x value
 * @return distance between the coordinate points
 *
 */

double get_distance(const int x1, const int x2, const int y1, const int y2) {
	return sqrt(pow(y2 - y1, 2) + pow(x2 - x1, 2));
}

/**
 * Calculates the x or y starting index of the grid to start checking for best fit points
 *
 * Finds the closest x or y point that is closest to the top right of the circle
 *
 * @param x1 First coordinate's x value
 * @param x2 Second coordinate's x value
 * @param y1 First coordinate's x value
 * @return start_indx x or y index of the top right location
 */
int get_start_indx(int center_coordinate, int radius, unsigned int grid_spacing) {
	int start_indx = center_coordinate - radius;
	start_indx -= (int)start_indx % grid_spacing; //top left x or y point closest to the circle
	if (start_indx < 40) // Only points in the grid are selected
	{
		start_indx = 40;
	}
	return start_indx;
}

/**
 * Calculates the x or y ending index of the grid to start checking for best fit points
 *
 * Finds the closest x or y point that is closest to the bottom of the circle
 *
 * @param x1 First coordinate's x value
 * @param x2 Second coordinate's x value
 * @param y1 First coordinate's x value
 * @return end_indx x or y index of the bottom right location
 */
int get_end_indx(int start_indx, int radius, unsigned int grid_spacing) {
	int end_indx = start_indx + ((int(radius) << 1));
	end_indx = end_indx + (grid_spacing - (end_indx % grid_spacing)); // bottom right x or y point closest to the circle
	if (end_indx > 800) { // Only points in the grid are selected
		end_indx = 800;
	}
	return end_indx;
}

/**
* Clears any objects drawn on the grid and displays the original grid
*
* @param populated_image Image that needs to be cleared
*/
void reset_grid(cv::Mat& populated_image) {
	background_with_grid.copyTo(populated_image);

}

/**
* Computes the best fit points given a circle and returns the distances between the best fit points and circle center
*
* Starts off by computing the indicies required to only account for points that are close to the circle and iterates through those
* points and checks to see whether the distance between the potential best fit point and center and within the threshold limit. If the
* point is a best fit, then the distances are stored and point is colored blue
*
*
* @param center_x x coordinate of the circle center
* @param center_y y coordinate of the circle center
* @param radius radius of the circle
* @param threshold Check to see if the distance between the point is within a certain threshold to be considered as best fit
* @param background image that the points are plotted on
* @return distances list of distances between points and center
*/
std::vector<double>  get_best_fit_distances(unsigned int center_x, unsigned int center_y, double radius, int threshold, cv::Mat& background) {
	// Calculate the start and end indicies of nearby points
	int start_indx_x = get_start_indx(center_x, radius, grid_spacing);
	int start_indx_y = get_start_indx(center_y, radius, grid_spacing);
	int end_indx_x = get_end_indx(start_indx_x, radius, grid_spacing);
	int end_indx_y = get_end_indx(start_indx_y, radius, grid_spacing);

	int point_x;
	int point_y;

	std::vector<double> distances;
	// Loop through the given grid to check to check for best fitting points.
	for (unsigned int i = start_indx_x / grid_spacing; i <= end_indx_x / grid_spacing; ++i) {
		for (unsigned int j = start_indx_y / grid_spacing; j <= end_indx_y / grid_spacing; ++j) {
			double dist_bwn_pt_to_cntr = get_distance(grid_coordinates[i - 1][j - 1].x, center_x, grid_coordinates[i - 1][j - 1].y, center_y);
			if (dist_bwn_pt_to_cntr >= abs(radius - threshold) && dist_bwn_pt_to_cntr <= abs(radius + threshold)) // Check to see if the point is a best point
			{
				point_x = grid_spacing * i;
				point_y = grid_spacing * j;
				distances.push_back(dist_bwn_pt_to_cntr); //Add the best point to the distance vector
				cv::rectangle(background, cv::Point(point_x, point_y), cv::Point((point_x)+5, (point_y)+5), cv::Scalar(255, 0, 0), -1, 8, 0); //Plot the best point on the grid

			}
		}
	}
	return distances;
}

/**
*
* Calculate and plot the inner and outer circle that the best points can fit
*
* @param center_x x coordinate of the circle center
* @param center_y y coordinate of the circle center
* @param radius radius of the circle
* @param distances Distance vector
* @param threshold Check to see if the distance between the point is within a certain threshold to be considered as best fit
* @param increment increment for the radius
* @param background image that the points are plotted on
*/
void draw_threshold_circles(int center_x, int center_y, double radius, std::vector<double> distances, int threshold, double increment, cv::Mat& background) {
	int inner_count;
	int outer_count;
	double inner_radius = radius;
	double outer_radius = radius;
	bool found_inner_radius = false;
	bool found_outer_radius = false;

	// Loop until best inner or outer circle is found
	while (!found_inner_radius && !found_outer_radius) {
		if (!found_inner_radius)
			inner_radius -= increment;//increament the inner circle's radius by given increment
		if (!found_outer_radius)
			outer_radius += increment; //increament the outer circle's radius by given increment

		inner_count = 0;
		outer_count = 0;
		// Interate between the distances to check if all the distances fall within a threshold
		for (auto distance : distances) {
			if (distance >= abs(inner_radius - threshold) && distance <= abs(inner_radius + threshold))
			{
				++inner_count;
			}
			if (inner_count <= distances.size() - 1)
			{
				found_inner_radius = true; //if at least one of the points dont fit, the radius is found
			}
			if (distance >= abs(outer_radius - threshold) && distance <= abs(outer_radius + threshold))
			{
				++outer_count;
			}
			if (outer_count <= distances.size() - 1)
			{
				found_outer_radius = true; //if at least one of the points dont fit, the radius is found
			}
		}
	}

	// Plot the new circles
	cv::circle(background, cv::Point(center_x, center_y), inner_radius, cv::Scalar(0, 0, 255), 2, 8, 0);
	cv::circle(background, cv::Point(center_x, center_y), outer_radius, cv::Scalar(0, 0, 255), 2, 8, 0);
}

/**
*
* Call Back function that recognizes mouse clicks. The function calculates the user generated radius by draggin the point
* and creates an initial circle and calls the get_best_fit_distances and draw_threshold_circles to generate and plot best
* fit points, inner and outer threshold circles
*
* @param event name of the mouse activity
* @param x x coordinate of the mouse cursor
* @param y y coordinate of the mouse cursor
* @param flags any flags that user passed in
* @param background image background
*/
void mouse_activity(int event, int x, int y, int flags, void* background) {
	if (event == cv::EVENT_LBUTTONDOWN)
	{
		// Recognize user's mouse click and set appropriate flags
		left_button_clicked = true;
		left_button_released = false;
		is_clicked = true; // user has to click at alease once to generate plots
	}
	if (event == cv::EVENT_LBUTTONUP)
	{
		// Recognize user's mouse release and set appropriate flags
		left_button_released = true;
		left_button_clicked = false;
	}
	if (left_button_clicked && clicked_flag)
	{
		// Record the coordinates when the user clicks the left button
		clicked_flag = false;
		released_flag = true;
		center_x = x;
		center_y = y;
	}

	if (left_button_released && released_flag)
	{
		// Record the coordinates when the user released the left button
		released_flag = false;
		clicked_flag = true;
		circle_edge_x = x;
		circle_edge_y = y;
	}

	if (left_button_released && is_clicked) //Once the user clicks and releases the mouse
	{
		cv::Mat& img = *((cv::Mat*)(background)); // 1st cast it back, then deref
		cv::Mat& original_image = *((cv::Mat*)(background)); //get a copy of original image


		reset_grid(img); // Reset grid to display new circle

		// Calculate the radius and compute best fir distances
		double radius = get_distance(center_x, circle_edge_x, center_y, circle_edge_y);
		std::vector<double> distances;
		distances = get_best_fit_distances(center_x, center_y, radius, 30, img);

		if (!distances.empty())
			draw_threshold_circles(center_x, center_y, radius, distances, 0.5, 10, img); //draw the threshold circles on the grid

		// Plot the user generted circle
		cv::circle(img, cv::Point(center_x, center_y), radius, cv::Scalar(255, 0, 0), 2, 8, 0);
		imshow("Digitizing Circles", img);
	}



}



int main() {
	//Create a white background
	cv::Mat white_background(850, 850, CV_8UC3, cv::Scalar(255, 255, 255));

	if (white_background.empty()) {
		std::cout << "Looks like clicked_flag went wrong. Please try again";
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

	//Show the stitched image
	imshow("Digitizing Circles", white_background);
	cv::waitKey(0);
	return 0;

}

