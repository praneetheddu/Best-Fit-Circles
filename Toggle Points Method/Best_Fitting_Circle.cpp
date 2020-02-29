/*
* @file Best_Fitting_Circle.cpp
* @brief Source file for Best_Fitting_Circle which is used to generate best fit circle given at least three
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
#include "Best_Fitting_Circle.h"

/**
* Constructor to setup points and initialize circle center and radius
*
* @param selected_points Points list selected by the user
*
*
*/
Best_Fitting_Circle::Best_Fitting_Circle(std::vector<cv::Point> selected_points) {
	this->selected_points = selected_points;
	this->circle_center_est.x = 0.0;
	this->circle_center_est.y = 0.0;
	this->radius_estimate = 0.0;
}

/**
* Calculates the circumcenter of point triplets
*
* @param point_i 1st point coordinates
* @param point_j 2nd point coordinates
* @param point_k 3rd point coordinates
* @param delta delta value
* @return center_est Center of the triplet points
*
*/
Circle_Center Best_Fitting_Circle::calculate_circumcenter(cv::Point point_i, cv::Point point_j, cv::Point point_k, double delta) {
	Circle_Center center_est;
	cv::Point dIJ;
	cv::Point dJK;
	cv::Point dKI;
	dIJ = cv::Point(point_j.x - point_i.x, point_j.y - point_i.y);
	dJK = cv::Point(point_k.x - point_j.x, point_k.y - point_j.y);
	dKI = cv::Point(point_i.x - point_k.x, point_i.y - point_k.y);

	double sqI = (point_i.x * point_i.x) + (point_i.y * point_i.y);
	double sqJ = (point_j.x * point_j.x) + (point_j.y * point_j.y);
	double sqK = (point_k.x * point_k.x) + (point_k.y * point_k.y);

	center_est.x = (sqI * dJK.y + sqJ * dKI.y + sqK * dIJ.y) / (2 * delta);
	center_est.y = -1 * (sqI * dJK.x + sqJ * dKI.x + sqK * dIJ.x) / (2 * delta);
	return center_est;
}

/**
* Intializer function to iterate through the point triplets and finding an estimate for
* the best circle's center coordinates
*
* @param points Points list selected by the user
* @return circle_center_est Estimate of circle's center
*
*/

Circle_Center Best_Fitting_Circle::initial_estimate(std::vector<cv::Point> points) {
	double sigma_x = 0;
	double sigma_y = 0;
	int q = 0;

	double delta;
	cv::Point ij;
	cv::Point jk;
	cv::Point ki;

	Circle_Center center_increment;
	for (auto i = 0; i < points.size() - 2; ++i) {
		for (auto j = i + 1; j < points.size() - 1; ++j) {
			for (auto k = j + 1; k < points.size(); ++k) {
				ij = cv::Point(points[j].x - points[i].x, points[j].y - points[i].y);
				jk = cv::Point(points[k].x - points[j].x, points[k].y - points[j].y);
				ki = cv::Point(points[i].x - points[k].x, points[i].y - points[k].y);
				delta = (jk.x * ij.y) - (ij.x * jk.y);
				if (abs(delta) < 1.0e-10)
				{
					// Circle cannot be computed because all the points are alligned in the same axis
					center_increment.x = -1.0;
					center_increment.y = -1.0;
					std::cout << "Invalid point selection. Please reset and select new points" << std::endl;
					return circle_center_est;
				}
				else {
					// Get an estimate for the circle's center by summing all the possible circumcenters
					center_increment = calculate_circumcenter(points[i], points[j], points[k], delta);
					circle_center_est.x += center_increment.x;
					circle_center_est.y += center_increment.y;
					++q;
				}
			}
		}
	}
	// Divide the center by the count to get an average value
	circle_center_est.x /= q;
	circle_center_est.y /= q;
	return circle_center_est;
}

/**
* Determine the best radius estimate when given a set of points
*
* @param points Points list selected by the user
* @return radius_estimate Estimation of the radius
*
*/
double Best_Fitting_Circle::compute_radius_estimate(std::vector<cv::Point> points) {
	for (auto point : points) {
		// Sum the distance to all the possible points
		radius_estimate += get_distance(point.x, circle_center_est.x, point.y, circle_center_est.y);
	}
	// Divide by number of points 
	radius_estimate /= points.size();
	return radius_estimate;
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
double Best_Fitting_Circle::get_distance(const int x1, const int x2, const int y1, const int y2) {
	return sqrt(pow(y2 - y1, 2) + pow(x2 - x1, 2));
}

/**
* Determine the cost given the updated radius and distances to be used in
* gradient computation
*
* @param points Points list selected by the user
* @return cost Cost value
*
*/
double Best_Fitting_Circle::cost_function(std::vector<cv::Point> points) {
	double cost = 0.0;
	for (auto point : points) {
		// Summation of cost value
		cost += pow(get_distance(point.x, circle_center_est.x, point.y, circle_center_est.y) - radius_estimate, 2);

	}
	return cost;
}

/**
* Determine the gradients for each coordinate to use to determine conjugate gradient
*
* @param points Points list selected by the user
* @return cost_gradient Cost gradient for each coordinate
*
*/
Gradient Best_Fitting_Circle::get_gradient_for_conjugate_gradient(std::vector<cv::Point> points) {
	Gradient cost_gradient;
	cost_gradient.x = 0.0;
	cost_gradient.y = 0.0;
	/*Iterate throught the points and determine the cost gradient to the
	   respective circle center estimate and  distance from circle center to each point
	*/
	for (auto point : points) {
		cost_gradient.x += (circle_center_est.x - point.x) * (get_distance(point.x, circle_center_est.x,
			point.y, circle_center_est.y) - radius_estimate);
		cost_gradient.y += (circle_center_est.y - point.y) * (get_distance(point.x, circle_center_est.x,
			point.y, circle_center_est.y) - radius_estimate);
	}
	// Multiply the cost gradient by 2 as given per equation
	cost_gradient.x *= 2;
	cost_gradient.y *= 2;
	return cost_gradient;
}

/**
* Computes the lambda value by performing the Newton solver on the derivative
* of the cost function.
*
* @param points Points list selected by the user
* @param u Directional gradient
* @return lambda updated lambda value
*
*/
double Best_Fitting_Circle::compute_lambda(std::vector<cv::Point> points, Gradient u) {
	double sum1 = 0, sum2 = 0, sum_fac = 0, sum_fac_dr = 0;
	// Iterate through the points and find the different sum parameters
	for (auto point : points) {
		double dx = circle_center_est.x - point.x;
		double dy = circle_center_est.y - point.y;
		double dxdy = sqrt(pow(dx, 2) + pow(dy, 2));
		double c1 = (dx * u.x + dy * u.y) / dxdy;
		double c2 = dxdy - radius_estimate;
		sum1 += c1 * c2;
		sum2 += c2 / dxdy;
		sum_fac += c1;
		sum_fac_dr += c1 * c1 / dxdy;


	}
	// Compute lambda value using Newton step method
	double lambda = (-1 * sum1);
	lambda /= ((pow(u.x, 2) + pow(u.y, 2))
		* sum2 - sum_fac * sum_fac / points.size()
		+ radius_estimate * sum_fac_dr);
	return lambda;
}

/**
* Computes the conjugate gradient by using POLAK and RIBI`ERE method to
* determine if the algorithm can converge within a set of iterations to find
* the best possible circle fit
*
* @param Gradient cost_gradient cost gradient calculated in get_gradient_for_converge()
* @return the state of convergence
*
*/
bool Best_Fitting_Circle::converge(Gradient cost_gradient) {
	if (cost < 1.0e-10 || sqrt(pow(cost_gradient.x, 2) + pow(cost_gradient.y, 2)) < 1.0e-10) {
		return true; //found out minimum solution
	}
	else {
		double previous_cost = cost;
		Gradient previouos_cost_gradient = cost_gradient;
		Gradient u_prev;
		u_prev.x = 0;
		u_prev.y = 0;
		// Loop through set number of iterations to find convergence
		for (int i = 0; i < 100; i++) {
			// Directional gradient
			Gradient u;
			u.x = -1 * cost_gradient.x;
			u.y = -1 * cost_gradient.y;

			if (i > 0) {
				//Compute new directional gradient values
				double beta = (cost_gradient.x * (cost_gradient.x - previouos_cost_gradient.x) + cost_gradient.y * (cost_gradient.y - previouos_cost_gradient.y));
				beta /= (previouos_cost_gradient.x * previouos_cost_gradient.x + previouos_cost_gradient.y + previouos_cost_gradient.y);
				u.x += beta * u_prev.x;
				u.y += beta * u_prev.y;
			}
			// Store the current values for future use
			previouos_cost_gradient.x = cost_gradient.x;
			previouos_cost_gradient.y = cost_gradient.y;
			u_prev.x = u.x;
			u_prev.y = u.y;
			double previous_cost;

			do {
				// Reducing the circle parameters until convergence is found
				previous_cost = cost;
				double lambda = compute_lambda(selected_points, u);
				circle_center_est.x += lambda * u.x;
				circle_center_est.y += lambda * u.y;
				radius_estimate = compute_radius_estimate(selected_points);
				cost = cost_function(selected_points);
			} while (++i < 10 && (abs(cost - previous_cost)) / (cost > 0.1));
			if ((abs(cost - previous_cost)) / cost < 1.0e-12)
			{ // If convergence is found, return true
				return true;
			}
			previous_cost = cost;
		}
	}
	return false;
}
/**
* Check to see if a best fit circle can be computed using the POLAK and RIBI`ERE reducing method
*
* @return true if a best fit circle is cimputable or else return false
*
*/
bool Best_Fitting_Circle::compute_best_fit_circle() {
	Circle_Center circle_center_estimate;
	circle_center_estimate = initial_estimate(selected_points); //Calculate intial estimate for center coordinates
	if (circle_center_estimate.x > -1 && circle_center_estimate.y > -1) { //Center can be computed
		compute_radius_estimate(selected_points); //Calculate intial radius
		cost = cost_function(selected_points); //Calculate cost 
		Gradient cost_gradient = get_gradient_for_conjugate_gradient(selected_points); //Calculate cost gradients
		bool convergence = converge(cost_gradient); //State of convergence
		if (!convergence)
		{
			// Circle cannot be formed
			std::cout << "Cannot Compute circle with given points. Please reset and enter new points";
			return false;
		}
		// Print out circle's estimates of radius and center coordinates
		std::cout << "radius estimate = " << radius_estimate << std::endl;
		std::cout << "circle center = " << circle_center_est.x << " , " << circle_center_est.y << std::endl;
		return true;
	}
	return false;
}

/**
* returns the circle's calculated center coordinates
*
* @return circle_center_est Estimated center coordinates
*/
Circle_Center Best_Fitting_Circle::get_center_coordinate() {
	return circle_center_est;
}

/**
* returns the circle's calculated radius
*
* @return radius_estimate Estiamted radius
*/
double Best_Fitting_Circle::get_radius() {
	return radius_estimate;
}