#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cmath>

using namespace std;

const float epsilon = .01;

class Color{

	public:

	float r;
	float g;
	float b;

	Color(){
		r = 0;
		b = 0;
		g = 0;
	}

	Color(float r, float g, float b): r(r), g(g), b(b){}

	// Vector addition
	Color operator+(Color color){

		return Color(r + color.r, g + color.g, b + color.b);

	}

	// Vector scalar multiplication
	Color operator*(float scalar){

		return Color(r * scalar, g * scalar, b * scalar);

	}

};

// Vector in 3d space
class Vector3D{
	public:

	float x;
	float y;
	float z;

	Vector3D(){

		x = 0;
		y = 0;
		z = 0;

	}

	Vector3D(float x, float y, float z): x(x), y(y), z(z){}

	// Vector addition
	Vector3D operator+(Vector3D vec){

		return Vector3D(x + vec.x, y + vec.y, z + vec.z);

	}

	// Vector subtraction
	Vector3D operator-(Vector3D vec){

		return Vector3D(x - vec.x, y - vec.y, z - vec.z);

	}

	// Vector scalar multiplication
	Vector3D operator*(float scalar){

		return Vector3D(x * scalar, y * scalar, z * scalar);

	}

	// Calculate vector length
	float length(){

		return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));

	}

	// Calculation normalization of vector
	Vector3D normalization(){

		return Vector3D(x / length(), y / length(), z / length());

	}

	// Calculate cross product of two vectors
	Vector3D crossProduct(Vector3D vec){

		return Vector3D(y * vec.z - z * vec.y, z * vec.x - x * vec.z, x * vec.y - y * vec.x);

	}

	// Calculate dot product of two vectors
	float dotProduct(Vector3D vec){

		return x * vec.x + y * vec.y + z * vec.z;

	}

};

class Surface{

	public:

	string surfaceID;
	Color ambient;
	Color diffuse;
	Color specular;
	float specpow;
	float reflect;

};

class Sphere{

	public:

	string surfaceID;
	float radius;
	Vector3D center;

};

class Intersection{

	public:

	float t;
	Sphere sphere;
	Vector3D location;

	bool operator==(const Intersection& rhs) const{
		return this->location.x == rhs.location.x &&
			this->location.y == rhs.location.y &&
			this->location.z == rhs.location.z &&
			this->t == rhs.t;
	}

};

class Light{

	public:

	float intensity;
	string type;
	Vector3D position;

};

class Ray{

	public:

	// Origin, direction, list of intersections
	Vector3D e;
	Vector3D d;
	vector<Intersection> intersections;
	float n, f;

	Ray(Vector3D e, Vector3D d, float n, float f): e(e), d(d), n(n), f(f){}

	// Parametric ray form
	Vector3D parametric(float t){

		return Vector3D(e + d * t);

	}

	// Calculate discriminant for finding intersection
	float getDiscriminant(Vector3D c, float r){

		return pow(d.dotProduct(e - c), 2) - d.dotProduct(d) * ((e - c).dotProduct(e - c) - pow(r, 2));

	}

	// Checks if the ray intersects a sphere
	bool intersectsSphere(Sphere sphere){

		return getDiscriminant(sphere.center, sphere.radius) >= 0;

	}

	// Calculate intersections
	void getIntersections(vector<Sphere> spheres){

		intersections.clear();

		for(auto sphere = spheres.begin(); sphere != spheres.end(); ++sphere){

			if(intersectsSphere(*sphere)){

				float t;
				Intersection intersection;
				Vector3D c = sphere->center;
				float r = sphere->radius;

				// Calculate intersection locations along the ray
				t = ((d * -1).dotProduct(e - c) + sqrt(getDiscriminant(c, r))) / (d.dotProduct(d));
				if(t > 0.001){
					intersection.t = t;
					intersection.sphere = *sphere;
					intersection.location = parametric(t);
					intersections.push_back(intersection);
				}

				t = ((d * -1).dotProduct(e - c) - sqrt(getDiscriminant(c, r))) / (d.dotProduct(d));
				if(t > 0.001){
					intersection.t = t;
					intersection.sphere = *sphere;
					intersection.location = parametric(t);
					intersections.push_back(intersection);
				}

			}

		}

	}

	// Find closest object
	Intersection getClosestIntersection(){

		float closestT = intersections.begin()->t;
		Intersection closestIntersection;
		for(auto intersection : intersections){

			if(0 < intersection.t && intersection.t < closestT){

				closestIntersection.t = intersection.t;
				closestIntersection.sphere = intersection.sphere;
				closestIntersection.location = intersection.location;

			}

		}

		return closestIntersection;

	}


};

bool anyhit(Ray ray, float distance, vector<Sphere> spheres){

	ray.getIntersections(spheres);

	for(auto intersection : ray.intersections){

		if(intersection.t > 0.01 && (intersection.location - ray.e).length() < distance && intersection.t <= 1){

			return true;

		}

	}

	return false;

}

struct Node{

	vector<Sphere> intersectedSpheres;
	int axis;
	float split;
	Node* left;
	Node* right;

};

Node* kdBuild(vector<Sphere> spheres){

	if(spheres.size() <= 0){
		return nullptr;
	}

	Node* root = new Node;
	vector<Sphere> leftSpheres, rightSpheres;

	root->left = nullptr;
	root->right = nullptr;

	float xMax = -INFINITY;
	float xMin = INFINITY;
	float yMax = -INFINITY;
	float yMin = INFINITY;
	float zMax = -INFINITY;
	float zMin = INFINITY;

	for(auto sphere: spheres){
		xMax = max(sphere.center.x + sphere.radius, xMax);
		xMin = min(sphere.center.x - sphere.radius, xMin);
		yMax = max(sphere.center.y + sphere.radius, yMax);
		yMin = min(sphere.center.y - sphere.radius, yMin);
		zMax = max(sphere.center.z + sphere.radius, zMax);
		zMin = min(sphere.center.z - sphere.radius, zMin);
	}

	float xRange = xMax - xMin;
	float yRange = yMax - yMin;
	float zRange = zMax - zMin;
	float maxRange = max(max(xRange, yRange), max(yRange, zRange));

	if(maxRange == xRange){
		root->split = maxRange / 2;
		root->axis = 0;
	} else if(maxRange == yRange){
		root->split = maxRange / 2;
		root->axis = 1;
	} else if(maxRange == zRange){
		root->split = maxRange / 2;
		root->axis = 2;
	}

	cout << maxRange << " " << xRange << " " << yRange << " " << zRange << " (" << root->split << ")" << endl;

	for(auto sphere: spheres){

		float distance, sphereCenterAxis;
		if(root->axis == 0){
			distance = abs(sphere.center.x - root->split);
			sphereCenterAxis = sphere.center.x;
		} else if(root->axis == 1){
			distance = abs(sphere.center.y - root->split);
			sphereCenterAxis = sphere.center.y;
		} else if(root->axis == 2){
			distance = abs(sphere.center.z - root->split);
			sphereCenterAxis = sphere.center.z;
		}

		if(distance < sphere.radius){
			root->intersectedSpheres.push_back(sphere);
		} else if(sphereCenterAxis < root->split) {
			leftSpheres.push_back(sphere);
		} else if(sphereCenterAxis > root->split) {
			rightSpheres.push_back(sphere);
		}

	}

	root->left = kdBuild(leftSpheres);
	root->right = kdBuild(rightSpheres);

	return root;

}

float planeIntersect(Ray ray, int axis, float split){

	float t;

	return t;

}

// kdtree
void kdTraverse(Intersection& closest, Ray ray, Node* root, Vector3D p){

	float t;
	Intersection intersection;
	int pAxis;
	if(root->axis == 0){
		pAxis = p.x;
	} else if(root->axis == 1){
		pAxis = p.y;
	} else {
		pAxis = p.z;
	}

	if(root == nullptr){
		closest.t = INFINITY;
		return;
	}

	for(auto sphere: root->intersectedSpheres){

		if(ray.intersectsSphere(sphere)){

			Vector3D c = sphere.center;
			float r = sphere.radius;

			float newT = ((ray.d * -1).dotProduct(ray.e - c) - sqrt(ray.getDiscriminant(c, r))) / (ray.d.dotProduct(ray.d));
			if(newT < t){
				t = newT;
				closest.t = t;
				closest.sphere = sphere;
				closest.location = ray.parametric(t);
			}

		}

	}

	if(ray.n < closest.t < ray.f){
		ray.f = t;
		closest = intersection;
	}

	if(pAxis < root->split){
		kdTraverse(closest, ray, root->left, p);
		t = planeIntersect(ray, root->axis, root->split);
	}

}

Color trace(Node* root, Ray ray, Color color, Color background, vector<Light> lights, vector<Surface> surfaces, vector<Sphere> spheres, int depth, int maxdepth, float cutoff, float reflectionCoefficient){


	ray.getIntersections(spheres);
	Surface closestSurface;

	if(ray.intersections.empty()){

		return background;

	}

	Intersection closestIntersection = ray.getClosestIntersection();

	for(auto surface = surfaces.begin(); surface != surfaces.end(); ++surface){

		if(surface->surfaceID == closestIntersection.sphere.surfaceID){

			closestSurface = *surface;

		}

	}

	reflectionCoefficient *= closestSurface.reflect;

	if(depth == maxdepth || reflectionCoefficient < cutoff){

		return color;

	}

	color = closestSurface.ambient;
	Vector3D P(closestIntersection.location);
	Vector3D N((P - closestIntersection.sphere.center).normalization());
	Vector3D R((ray.d - N * 2 * N.dotProduct(ray.d)).normalization());

	for(auto light = lights.begin(); light != lights.end(); light++){

		Vector3D L = (light->position - P).normalization();
		Vector3D H((ray.d * -1 + L).normalization());

		if(!anyhit(Ray(P, L, epsilon, INFINITY), (light->position - P).length(), spheres) && N.dotProduct(L) > 0){

			// Calculate diffusion value
			color = color + closestSurface.diffuse * light->intensity * N.dotProduct(L);

			if(N.dotProduct(H) > 0){

				// Calculate specular value
				color = color + closestSurface.specular * light->intensity * pow(N.dotProduct(H), closestSurface.specpow);

			}

		}

	}

	color = color + trace(root, Ray(P, R, epsilon, INFINITY), color, background, lights, surfaces, spheres, depth + 1, maxdepth, cutoff, reflectionCoefficient) * closestSurface.reflect;

	return color;

}

int main(){

	// Initialize variables
	int height = 0, width = 0;
	float hfov = 0, vfov = 0;
	int maxdepth = 15;
	float cutoff = .002;
	Color background;
	Vector3D eyep;
	Vector3D lookp;
	Vector3D up;
	vector<Surface> surfaces;
	vector<Sphere> spheres;
	vector<Light> lights;

	// Read from ray file
	ifstream rayFile("../balls-3.ray");

	string word;
	while(rayFile >> word){

		// Get background color
		if(word == "background"){

			rayFile >> word;
			background.r = stof(word);
			rayFile >> word;
			background.g = stof(word);
			rayFile >> word;
			background.b = stof(word);

		// Get eyep values
		} else if(word == "eyep"){

			rayFile >> word;
			eyep.x = stof(word);
			rayFile >> word;
			eyep.y = stof(word);
			rayFile >> word;
			eyep.z = stof(word);


		// Get lookp values
		} else if(word == "lookp"){

			rayFile >> word;
			lookp.x = stof(word);
			rayFile >> word;
			lookp.y = stof(word);
			rayFile >> word;
			lookp.z = stof(word);

		// Get up values
		} else if(word == "up"){

			rayFile >> word;
			up.x = stof(word);
			rayFile >> word;
			up.y = stof(word);
			rayFile >> word;
			up.z = stof(word);

		// Get fov values
		} else if(word == "fov"){

			rayFile >> hfov;
			rayFile >> vfov;

		// Get image size
		} else if(word == "screen"){

			rayFile >> width;
			rayFile >> height;

		// Get max depth
		} else if(word == "maxdepth"){

			rayFile >> maxdepth;

		// Get image size
		} else if(word == "cutoff"){

			rayFile >> cutoff;

		// Get lights
		} else if(word == "light"){

			Light newLight;
			rayFile >> newLight.intensity;
			rayFile >> newLight.type;

			rayFile >> word;
			newLight.position.x = stof(word);
			rayFile >> word;
			newLight.position.y = stof(word);
			rayFile >> word;
			newLight.position.z = stof(word);

			lights.push_back(newLight);

		// Get surface ID
		} else if(word == "surface"){

			Surface newSurface = Surface();
			rayFile >> newSurface.surfaceID;
			surfaces.push_back(newSurface);

		// Get surface ambient values
		} else if(word == "ambient"){

			rayFile >> word;
			surfaces[surfaces.size() - 1].ambient.r = stof(word);
			rayFile >> word;
			surfaces[surfaces.size() - 1].ambient.g = stof(word);
			rayFile >> word;
			surfaces[surfaces.size() - 1].ambient.b = stof(word);

		// Get surface diffusion values
		} else if(word == "diffuse"){

			rayFile >> word;
			surfaces[surfaces.size() - 1].diffuse.r = stof(word);
			rayFile >> word;
			surfaces[surfaces.size() - 1].diffuse.g = stof(word);
			rayFile >> word;
			surfaces[surfaces.size() - 1].diffuse.b = stof(word);

		// Get surface specular values
		} else if(word == "specular"){

			rayFile >> word;
			surfaces[surfaces.size() - 1].specular.r = stof(word);
			rayFile >> word;
			surfaces[surfaces.size() - 1].specular.g = stof(word);
			rayFile >> word;
			surfaces[surfaces.size() - 1].specular.b = stof(word);

		// Get surface specular power
		} else if(word == "specpow"){

			rayFile >> word;
			surfaces[surfaces.size() - 1].specpow = stof(word);

		// Get surface reflection value
		} else if(word == "reflect"){

			rayFile >> word;
			surfaces[surfaces.size() - 1].reflect = stof(word);

		// Get spheres
		} else if(word == "sphere"){

			Sphere newSphere;
			rayFile >> newSphere.surfaceID;

			// Get sphere values
			rayFile >> word;
			newSphere.radius = stof(word);
			rayFile >> word;
			newSphere.center.x = stof(word);
			rayFile >> word;
			newSphere.center.y = stof(word);
			rayFile >> word;
			newSphere.center.z = stof(word);

			spheres.push_back(newSphere);

		}

	}

	Node* root;
	// root = kdBuild(spheres);

	// Create orthogonal basis and other necessities
	Vector3D d = eyep - lookp;

	float top, bottom, left, right;
	top = d.length() * tan((vfov * 3.14159 / 180) / 2);
	bottom = -top;
	left = d.length() * tan(- (hfov * 3.14159 / 180) / 2);
	right = -left;

	Vector3D w(d.normalization());
	Vector3D u(up.crossProduct(w).normalization());
	Vector3D v(w.crossProduct(u));

	unsigned char *pixels = new unsigned char[height * width * 3];

	// Find color for each pixel
	for(int y = 0; y < height; y++){

		for(int x = 0; x < width; x++){

			// Calculate pixel location in world space
			float us = left + (right - left) * (x + .5) / width;
			float vs = top + (bottom - top) * (y + .5) / height;
			Vector3D s(eyep + u.normalization() * us + v.normalization() * vs - w.normalization() * d.length());

			// Calculate ray from pixel location
			Ray ray(eyep, (s - eyep), epsilon, INFINITY);

			// Get pixel color
			Color color = trace(root, ray, background, background, lights, surfaces, spheres, 0, maxdepth, cutoff, 1);

			// Calculate pixel color values
			pixels[y * width * 3 + x * 3 + 0] = (color.r < 0) ? 0 : (color.r > 1) ? 255 : (unsigned char)(color.r * 255);
			pixels[y * width * 3 + x * 3 + 1] = (color.g < 0) ? 0 : (color.g > 1) ? 255 : (unsigned char)(color.g * 255);
			pixels[y * width * 3 + x * 3 + 2] = (color.b < 0) ? 0 : (color.b > 1) ? 255 : (unsigned char)(color.b * 255);

		}

	}


	// Write to image file
	FILE *f = fopen("trace.ppm","wb");
	fprintf(f, "P6\n%d %d\n255\n", width, height);
	fwrite(pixels, 1, height * width * 3, f);
	fclose(f);

	rayFile.close();

	return 0;

}