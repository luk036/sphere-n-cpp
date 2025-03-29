#include <libqhull/qhull_a.h>

#include <iostream>
#include <vector>

// Function to convert points to Qhull's format
void convertPointsToQhullFormat(const std::vector<std::vector<double>>& points, coordT* coords,
                                int& numpoints) {
    numpoints = points.size();
    for (int i = 0; i < numpoints; ++i) {
        coords[3 * i] = points[i][0];
        coords[3 * i + 1] = points[i][1];
        coords[3 * i + 2] = points[i][2];
    }
}

int main() {
    // Define a set of points
    std::vector<std::vector<double>> points
        = {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {1, 1, 1}};

    // Prepare for Qhull
    int numpoints;
    coordT* coords = new coordT[3 * points.size()];
    convertPointsToQhullFormat(points, coords, numpoints);

    // Set up Qhull control parameters
    QhullControl qhCtrl;
    qhCtrl.setDimension(3);  // We are working in 3D space
    qhCtrl.setPoints(coords, numpoints);
    qhCtrl.setOption("Qbb Qc Qz");  // Some common options for Qhull
    qhCtrl.runQhull();

    // Check if Qhull ran successfully
    if (!qhCtrl.qhullStatus()) {
        std::cerr << "Qhull did not terminate successfully.\n";
        delete[] coords;
        return 1;
    }

    std::cout << "Qhull completed successfully.\n";

    // Clean up
    delete[] coords;

    return 0;
}
