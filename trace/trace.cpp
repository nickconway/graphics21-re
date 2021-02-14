#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cmath>

using namespace std;

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
	vector<float> diffuse;

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

};

class Ray{

	public:

	// Origin, direction, list of intersections
	Vector3D e;
	Vector3D d;
	vector<Intersection> intersections;

	Ray(Vector3D e, Vector3D d): e(e), d(d){}

	// Parametric ray form
	Vector3D p(float t){

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

	// Gets intersection data
	void getIntersections(Vector3D c, float r, Sphere sphere){

		float t;
		Intersection intersection;

		// Calculate intersection locations along the ray
		t = ((d * -1).dotProduct(e - c) + sqrt(getDiscriminant(c, r))) / (d.dotProduct(d));
		intersection.t = t;
		intersection.sphere = sphere;
		intersections.push_back(intersection);

		t = ((d * -1).dotProduct(e - c) - sqrt(getDiscriminant(c, r))) / (d.dotProduct(d));
		intersection.t = t;
		intersection.sphere = sphere;
		intersections.push_back(intersection);

	}

};

int main(){

	// Initialize variables
	int height = 0, width = 0;
	float hfov = 0, vfov = 0;
	vector<float> background;
	Vector3D eyep;
	Vector3D lookp;
	Vector3D up;
	vector<Surface> surfaces;
	vector<Sphere> spheres;

	// Read from ray file
	ifstream rayFile("../balls-3.ray");

	string word;
	while(rayFile >> word){

		// Get background color
		if(word == "background"){

			for(int i = 0; i < 3; i++){

				rayFile >> word;
				background.push_back(stof(word));

			}

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

		// Get surfaces
		} else if(word == "surface"){

			Surface newSurface;
			rayFile >> newSurface.surfaceID;

			// Get surface attributes
			while(rayFile >> word){

				// Get surface diffusion values
				if(word == "diffuse"){

					rayFile >> word;
					newSurface.diffuse.push_back(stof(word));
					rayFile >> word;
					newSurface.diffuse.push_back(stof(word));
					rayFile >> word;
					newSurface.diffuse.push_back(stof(word));

					surfaces.push_back(newSurface);

					break;

				}

			}

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

			// Color is the background color if there is no intersection
			vector<float> color = background;

			// Calculate pixel location in world space
			float us = left + (right - left) * (x + .5) / width;
			float vs = top + (bottom - top) * (y + .5) / height;
			Vector3D s(eyep + u.normalization() * us + v.normalization() * vs - w.normalization() * d.length());

			// Calculate ray from pixel location
			Ray ray(eyep, s - eyep);

			// Calculate intersections
			for(auto sphere = spheres.begin(); sphere != spheres.end(); ++sphere){

				if(ray.intersectsSphere(*sphere)){

					ray.getIntersections(sphere->center, sphere->radius, *sphere);

				}

			}

			// When ray hits an object
			if(!ray.intersections.empty()){

				// Find closest object
				float closestT = ray.intersections.begin()->t;
				Sphere closestSphere;
				for(auto intersection = ray.intersections.begin(); intersection != ray.intersections.end(); ++intersection){

					if(0 < intersection->t && intersection->t < closestT){

						closestT = intersection->t;
						closestSphere = intersection->sphere;

					}

				}

				// Get pixel color from surface
				for(auto surface = surfaces.begin(); surface != surfaces.end(); ++surface){

					if(surface->surfaceID == closestSphere.surfaceID){

						color = surface->diffuse;

					}

				}

			}

			// Calculate pixel color values
			pixels[y * width * 3 + x * 3 + 0] = (color.at(0) < 0) ? 0 : (color.at(0) > 1) ? 255 : (unsigned char)(color.at(0) * 255);
			pixels[y * width * 3 + x * 3 + 1] = (color.at(1) < 0) ? 0 : (color.at(1) > 1) ? 255 : (unsigned char)(color.at(1) * 255);
			pixels[y * width * 3 + x * 3 + 2] = (color.at(2) < 0) ? 0 : (color.at(2) > 1) ? 255 : (unsigned char)(color.at(2) * 255);

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