#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <SFML/Graphics.hpp>


struct Point {
    double x, y;
};
int distSq(Point p1, Point p2) {
    int dx = p1.x - p2.x;
    int dy = p1.y - p2.y;
    return dx * dx + dy * dy;
}
Point pivot; 

int orientation(Point p, Point q, Point r) {
    int val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
    if (val == 0) return 0;
    return (val > 0) ? 1 : -1;
}

bool compare(Point p, Point q) {
    int orientation_val = orientation(pivot, p, q);
    if (orientation_val == 0) {
        return (distSq(pivot, p) < distSq(pivot, q));
    }
    return (orientation_val == -1);
}

std::vector<Point> computeConvexHull(std::vector<Point>& points) {
    int min_x = points[0].x;
    int min_idx = 0;
    int n = points.size();
    for (int i = 1; i < n; i++) {
        int curr_x = points[i].x;
        if (curr_x < min_x || (curr_x == min_x && points[i].y < points[min_idx].y)) {
            min_x = curr_x;
            min_idx = i;
        }
    }

    std::swap(points[0], points[min_idx]);

    pivot = points[0];

    std::sort(points.begin() + 1, points.end(), compare);

    std::vector<Point> convexHull;
    convexHull.push_back(points[0]);
    convexHull.push_back(points[1]);
    convexHull.push_back(points[2]);

    for (int i = 3; i < n; i++) {
        while (convexHull.size() >= 2 && orientation(convexHull[convexHull.size() - 2], convexHull[convexHull.size() - 1], points[i]) != -1) {
            convexHull.pop_back();
        }
        convexHull.push_back(points[i]);
    }

    return convexHull;
}


double distance(const Point& p1, const Point& p2) {
    double dx = p1.x - p2.x;
    double dy = p1.y - p2.y;
    return std::sqrt(dx * dx + dy * dy);
}

double getCircleRadius(const Point& center, const std::vector<Point>& points) {
    double radius = 0.0;
    for (const auto& point : points) {
        double dist = distance(center, point);
        radius = std::max(radius, dist);
    }
    return radius;
}

Point getCircleCenter(const Point& p1, const Point& p2) {
    double x = (p1.x + p2.x) / 2.0;
    double y = (p1.y + p2.y) / 2.0;
    return { x, y };
}

std::pair<Point, double> findMinimumEnclosingCircle(std::vector<Point>& points) {
    std::shuffle(points.begin(), points.end(), std::default_random_engine());
    Point center = points[0];
    double radius = 0.0;

    for (int i = 1; i < points.size(); ++i) {
        if (distance(center, points[i]) > radius) {
            center = points[i];
            radius = 0.0;

            for (int j = 0; j < i; ++j) {
                if (distance(center, points[j]) > radius) {
                    center = getCircleCenter(points[i], points[j]);
                    radius = getCircleRadius(center, std::vector<Point>(points.begin(), points.begin() + i + 1));
                }
            }
        }
    }

    return { center, radius };
}


int main() {
    int n;
    std::cout << "Enter count of points: ";
    std::cin >> n;

    std::vector<Point> points(n);
    std::cout << "Enter coordinates:\n";
    for (int i = 0; i < n; ++i) {
        std::cout << "Point  " << (i + 1) << " (x y): ";
        std::cin >> points[i].x >> points[i].y;
    }

    std::vector<Point> convexHull = computeConvexHull(points);

    std::cout << "Convex Hull Points:" << std::endl;
    for (const auto& point : convexHull) {
        std::cout << "(" << point.x << ", " << point.y << ")" << std::endl;
    }

    std::pair<Point, double> minimumEnclosingCircle = findMinimumEnclosingCircle(convexHull);

    Point center = minimumEnclosingCircle.first;
    double radius = minimumEnclosingCircle.second;

    std::cout << "Center: (" << center.x << ", " << center.y << ")" << std::endl;
    std::cout << "Radius: " << radius << std::endl;


    sf::RenderWindow window(sf::VideoMode(1200, 1000), "Circle");

    sf::CircleShape point(3); 
    point.setFillColor(sf::Color::Red); 

    sf::Vector2f centerPoint(center.x * 100, center.y * 100); // Центр кола
    float circleRadius = radius * 100; // Радіус кола


    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::White);

        sf::CircleShape point(3);
        point.setFillColor(sf::Color::Red);
        for (const auto& p : points) {
            point.setPosition(p.x * 100.f, p.y * 100.f);
            window.draw(point);
        }


        sf::CircleShape circle;
        circle.setRadius(circleRadius); 
        circle.setFillColor(sf::Color(0, 0, 255, 128));
        circle.setOutlineColor(sf::Color(0, 0, 255, 128)); 
        circle.setOutlineThickness(7); 

        // Задаємо позицію центра кола
        float centerX = centerPoint.x;
        float centerY = centerPoint.y;
        circle.setPosition(centerX - circle.getRadius(), centerY - circle.getRadius());
        window.draw(circle);

        window.display();
    }
    return 0;
}



