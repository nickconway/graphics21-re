#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <map>

using namespace std;

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
	float centerX;
	float centerY;
	float centerZ;
	float radius;

};

int main(){

	// Initialize variables
	int height, width;
	float hfov, vfov;
	vector<float> background;
	vector<float> eyep;
	vector<float> lookp;
	vector<float> up;
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

			for(int i = 0; i < 3; i++){

				rayFile >> word;
				eyep.push_back(stof(word));

			}

		// Get lookp values
		} else if(word == "lookp"){

			for(int i = 0; i < 3; i++){

				rayFile >> word;
				lookp.push_back(stof(word));

			}

		// Get up values
		} else if(word == "up"){

			for(int i = 0; i < 3; i++){

				rayFile >> word;
				up.push_back(stof(word));

			}

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
			newSphere.centerX = stof(word);
			rayFile >> word;
			newSphere.centerY = stof(word);
			rayFile >> word;
			newSphere.centerZ = stof(word);
			rayFile >> word;
			newSphere.radius = stof(word);

			spheres.push_back(newSphere);

		}

	}

	unsigned char *pixels = new unsigned char[height * width * 3];

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

	// For debugging file input
	// cout << background.at(0) << " " << background.at(1) << " " << background.at(2) << endl;
	// cout << eyep.at(0) << " " << eyep.at(1) << " " << eyep.at(2) << endl;
	// cout << lookp.at(0) << " " << lookp.at(1) << " " << lookp.at(2) << endl;
	// cout << up.at(0) << " " << up.at(1) << " " << up.at(2) << endl;
	// cout << hfov << " " << vfov << endl;
	// cout << width << " " << height << endl;

	// for(auto it = surfaces.begin(); it != surfaces.end(); ++it){

	// 	cout << "Surface: " << it->surfaceID << " " << it->diffuseR << " " << it->diffuseG << " " << it->diffuseB << endl;

	// }

	// for(auto it = spheres.begin(); it != spheres.end(); ++it){

	// 	cout << "Sphere: " << it->surfaceID << " " << it->centerX << " " << it->centerY << " " << it->centerZ << endl;

	// }

	return 0;

}