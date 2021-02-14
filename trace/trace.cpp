#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cmath>

using namespace std;

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

	Vector3D operator+(Vector3D vec){

		return Vector3D(x + vec.x, y + vec.y, z + vec.z);

	}

	Vector3D operator-(Vector3D vec){

		return Vector3D(x - vec.x, y - vec.y, z - vec.z);

	}

	Vector3D operator*(int scalar){

		return Vector3D(x * scalar, y * scalar, z * scalar);

	}

	float dot_product(Vector3D vec){

		return x * vec.x + y * vec.y + z * vec.z;

	}

	float normalization(){

		return sqrt((sqrt(pow(x, 2) + pow(y, 2)), 2) + pow(z, 2));

	}

};

class Surface{

	public:

	string surfaceID;
	float diffuseR;
	float diffuseG;
	float diffuseB;

};

class Sphere{

	public:

	string surfaceID;
	float radius;
	float centerX;
	float centerY;
	float centerZ;

};

int main(){

	// Initialize variables
	int height, width;
	float hfov, vfov;
	vector<float> background;
	Vector3D eyep;
	Vector3D lookp;
	Vector3D up;
	vector<Surface> surfaces;
	vector<Sphere> spheres;

	// Read from ray file
	string word;
	ifstream rayFile("../balls-3.ray");

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
					newSurface.diffuseR = stof(word);
					rayFile >> word;
					newSurface.diffuseG = stof(word);
					rayFile >> word;
					newSurface.diffuseB = stof(word);

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
			newSphere.centerX = stof(word);
			rayFile >> word;
			newSphere.centerY = stof(word);
			rayFile >> word;
			newSphere.centerZ = stof(word);

			spheres.push_back(newSphere);

		}

	}

	// For debugging file input
	// cout << background.at(0) << " " << background.at(1) << " " << background.at(2) << endl;
	// cout << eyep.x << " " << eyep.y << " " << eyep.z << endl;
	// cout << lookp.x << " " << lookp.y << " " << lookp.z << endl;
	// cout << up.x << " " << up.y << " " << up.z << endl;
	// cout << hfov << " " << vfov << endl;
	// cout << width << " " << height << endl;

	// for(auto it = surfaces.begin(); it != surfaces.end(); ++it){
	// 	cout << "Surface: " << it->surfaceID << " " << it->diffuseR << " " << it->diffuseG << " " << it->diffuseB << endl;
	// }

	// for(auto it = spheres.begin(); it != spheres.end(); ++it){
	// 	cout << "Sphere: " << it->surfaceID << " " << it->radius << " " << it->centerX << " " << it->centerY << " " << it->centerZ << endl;
	// }

	Vector3D d = eyep - lookp;
	float top, bottom, left, right;
	top = d.normalization() * tan((vfov * 3.14159 / 180) / 2);
	bottom = -top;
	left = d.normalization() * tan((hfov * 3.14159 / 180) / 2);
	right = -left;

	cout << top << " " << bottom << " " << left << " " << right << endl;
	cout << d.normalization() << endl;

	unsigned char *pixels = new unsigned char[height * width * 3];

	for(int y = 0; y < height; y++){

		for(int x = 0; x < width; x++){

			int t = y * width + x;
			Vector3D ray(eyep.x + t * d.x, eyep.y + t * d.y, eyep.z + t * d.z);
			// cout << ray.x << " " << ray.y << " " << ray.z << endl;

			float us, vs;

		}
	}

	// Test image drawing
	// for(int y = 0; y < height; y++){
	// 	for(int x = 0; x < width; x++){
	// 		pixels[y * width * 3 + x * 3 + 0] = 255;
	// 		pixels[y * width * 3 + x * 3 + 1] = 128;
	// 		pixels[y * width * 3 + x * 3 + 2] = 64;
	// 	}
	// }

	FILE *f = fopen("trace.ppm","wb");
	fprintf(f, "P6\n%d %d\n255\n", width, height);
	fwrite(pixels, 1, height*width*3, f);
	fclose(f);

	rayFile.close();

	return 0;

}