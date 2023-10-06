#include <iostream>
#include <string>
#include <vector>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "Image.h"
#include "BoundingBox.h"
#include "NormalBox.h"

// This allows you to skip the `std::` in front of C++ standard library
// functions. You can also say `using std::cout` to be more selective.
// You should never do this in a header file.
using namespace std;

float RANDOM_COLORS[7][3] = {
	{0.0000,    0.4470,    0.7410},
	{0.8500,    0.3250,    0.0980},
	{0.9290,    0.6940,    0.1250},
	{0.4940,    0.1840,    0.5560},
	{0.4660,    0.6740,    0.1880},
	{0.3010,    0.7450,    0.9330},
	{0.6350,    0.0780,    0.1840},
};



//----------------//
// HELPER METHODS //
//----------------//



// does all of the barycentric calculations with references passed in
// made into a method because this code is used so often
// RETURNS: N/A
void barycentric(BoundingBox* box, int x, int y, float& a, float& b, float& c) {
	// finds total area of triangle
	float areaTotal = 0.5 * ((box->getX1() - box->getX0()) * (box->getY2() - box->getY0()) - (box->getX2() - box->getX0()) * (box->getY1() - box->getY0()));
	
	// finds areas of three subtriangles between the three vertices and (x, y)
	float areaOfA = 0.5 * ((box->getX1() - x) * (box->getY2() - y) - (box->getX2() - x) * (box->getY1() - y));
	float areaOfB = 0.5 * ((x - box->getX0()) * (box->getY2() - box->getY0()) - (box->getX2() - box->getX0()) * (y - box->getY0()));
	float areaOfC = 0.5 * ((box->getX1() - box->getX0()) * (y - box->getY0()) - (x - box->getX0()) * (box->getY1() - box->getY0()));
	
	// uses area formulas to find barycentric fractions
	a = areaOfA / areaTotal;
	b = areaOfB / areaTotal;
	c = areaOfC / areaTotal;
}



// checks bounds of barycentric values
// if > 1 or < 0 they are out of bounds
// RETURNS: TRUE if 0 <= (a, b, c) <= 1
//			FALSE if (a, b, c) > 1 or (a, b, c) < 0		
bool in_bounds(float& a, float& b, float& c) {
	if (a < -0.0001 || a > 1 || b < -0.0001 || b > 1 || c < -0.0001 || c > 1) {
		return false;
	}
	return true;
}



// does barycentric calculations and checks if x and y are in the triangle by checking
// the bounds of a, b, and c (if they are > 1 or < 0 we are outside the triangle)
// RETURNS: TRUE if (x, y) is in the triangle
//			FALSE if (x, y) is outside the triangle
bool barycentric_coords(BoundingBox* box, int y, int x) {
	float a, b, c;
	barycentric(box, x, y, a, b, c);
	return in_bounds(a, b, c);
}



// basically does the same thing as the method above BUT
// it interpolates the RGB values and blends the colors between the vertices
// writes to the image
// RETURNS: N/A
void barycentric_coords_coloring(std::shared_ptr<Image> i, BoundingBox* box, int y, int x, int c1r, int c1g, int c1b, int c2r, int c2g, int c2b, int c3r, int c3g, int c3b) {
	float a, b, c;
	barycentric(box, x, y, a, b, c);
	if (!in_bounds(a, b, c)) { return;  }

	float new_red = a * c1r + b * c2r + c * c3r;
	float new_green = a * c1g + b * c2g + c * c3g;
	float new_blue = a * c1b + b * c2b + c * c3b;
	i->setPixel(x, y, new_red, new_green, new_blue);
}



// performs vertical coloring by finding two fractions:
// red: represents how far the current y value is from the bottom of the object
// blue: represents how far the current y value is from the top of the object
// basically just linear interpolation on a vertical line from red to blue
// RETURNS: N/A
void vertical_coloring(std::shared_ptr<Image> i, BoundingBox* b, float y, float x, float maxY, float minY) {
	float red = (y - minY) / (maxY - minY);
	float blue = (maxY - y) / (maxY - minY);
	i->setPixel(x, y, 255*red, 0, 255*blue);
}



// performs more barycentric calculations BUT
// this time we put them to use with calculating the z coordinate
// this is basically the same as finding the pixel color, just with the z
// values of the three surrounding vertices instead
// RETURNS a float of the z-coordinate at pixel (x, y)
float compute_z(BoundingBox* box, int y, int x) {
	float a, b, c;
	barycentric(box, x, y, a, b, c);
	float new_z = a * box->getZ0() + b * box->getZ1() + c * box->getZ2();
	return new_z;
}



// calculates barycentric coords and interpolation based off the norBuf colors assigned to each vertex
// normal interpolation: normal_r, normal_g, normal_b
// mapping to [0, 255]: pixel_r, pixel_g, pixel_b
// RETURNS: N/A
void normal_barycentric_coloring(std::shared_ptr<Image> i, BoundingBox* box, int y, int x, float c1r, float c1g, float c1b, float c2r, float c2g, float c2b, float c3r, float c3g, float c3b) {
	float a, b, c;
	barycentric(box, x, y, a, b, c);
	if (!in_bounds(a, b, c)) { return; }

	float normal_r = a * c1r + b * c2r + c * c3r;
	float normal_g = a * c1g + b * c2g + c * c3g;
	float normal_b = a * c1b + b * c2b + c * c3b;

	float pixel_r = 255 * (0.5 * normal_r + 0.5); 
	float pixel_g = 255 * (0.5 * normal_g + 0.5); 
	float pixel_b = 255 * (0.5 * normal_b + 0.5); 
	i->setPixel(x, y, pixel_r, pixel_g, pixel_b);
}



// does the normal coloring calculations seen in the method above BUT
// goes a step further by taking the dot product of the normal values and the lighting vector
// and applying the resulting scalar to RGB equally (so it is black and white)
// RETURNS: N/A
void normal_coloring_with_lighting(std::shared_ptr<Image> i, BoundingBox* box, int y, int x, float c1r, float c1g, float c1b, float c2r, float c2g, float c2b, float c3r, float c3g, float c3b) {
	float a, b, c;
	barycentric(box, x, y, a, b, c);
	if (!in_bounds(a, b, c)) { return; }

	float normal_r = a * c1r + b * c2r + c * c3r;
	float normal_g = a * c1g + b * c2g + c * c3g;
	float normal_b = a * c1b + b * c2b + c * c3b;

	float c_light = 255 * (normal_r + normal_g + normal_b) / sqrt(3);

	if (0 > c_light) {
		c_light = 0;
	}

	i->setPixel(x, y, c_light, c_light, c_light);
}



//----------------//
//  MAIN METHOD   //
//----------------//



int main(int argc, char **argv)
{
	if(argc < 2) {
		cout << "Usage: A1 meshfile" << endl;
		return 0;
	}

	// command line inputs
	string meshName(argv[1]);
	string filename(argv[2]);
	int width = atoi(argv[3]);
	int height = atoi(argv[4]);
	int tasknum = atoi(argv[5]);

	// sets up the image
	auto image = make_shared<Image>(width, height);

	// keeps track of the maximum and minimum x, y, and z of the object
	float max_x = -1000;
	float max_y = -1000;
	float max_z = -1000;
	float min_x = 1000;
	float min_y = 1000;
	float min_z = 1000;

	// Load geometry
	vector<float> posBuf; // list of vertex positions
	vector<float> norBuf; // list of vertex normals
	vector<float> texBuf; // list of vertex texture coords
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	string errStr;
	bool rc = tinyobj::LoadObj(&attrib, &shapes, &materials, &errStr, meshName.c_str());
	if(!rc) {
		cerr << errStr << endl;
	} else {
		// Some OBJ files have different indices for vertex positions, normals,
		// and texture coordinates. For example, a cube corner vertex may have
		// three different normals. Here, we are going to duplicate all such
		// vertices.

		// Loop over shapes
		for(size_t s = 0; s < shapes.size(); s++) {
			// Loop over faces (polygons)
			size_t index_offset = 0;
			for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				size_t fv = shapes[s].mesh.num_face_vertices[f];
				// Loop over vertices in the face.
				for(size_t v = 0; v < fv; v++) {
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

					// updates max and min x
					posBuf.push_back(attrib.vertices[3*idx.vertex_index+0]);
					if(posBuf.back() < min_x){ min_x = posBuf.back();}
					if(posBuf.back() > max_x){ max_x = posBuf.back();}

					// updates max and min y
					posBuf.push_back(attrib.vertices[3*idx.vertex_index+1]);
					if(posBuf.back() < min_y){ min_y = posBuf.back();}
					if(posBuf.back() > max_y){ max_y = posBuf.back();}

					// updates max and min z
					posBuf.push_back(attrib.vertices[3*idx.vertex_index+2]);
					if (posBuf.back() < min_z) { min_z = posBuf.back(); }
					if (posBuf.back() > max_z) { max_z = posBuf.back(); }

					if(!attrib.normals.empty()) {
						norBuf.push_back(attrib.normals[3*idx.normal_index+0]);
						norBuf.push_back(attrib.normals[3*idx.normal_index+1]);
						norBuf.push_back(attrib.normals[3*idx.normal_index+2]);
					}
					if(!attrib.texcoords.empty()) {
						texBuf.push_back(attrib.texcoords[2*idx.texcoord_index+0]);
						texBuf.push_back(attrib.texcoords[2*idx.texcoord_index+1]);
					}
				}
				index_offset += fv;
				// per-face material (IGNORE)
				shapes[s].mesh.material_ids[f];
			}
		}
	}

	// TRANSLATING TO CENTER AND FITTING TO PICTURE
	
	// determines whether the image should be multiplied by x ratio or y ratio to fit image frame
	float ratio_y = height / (max_y - min_y);
	float ratio_x = width / (max_x - min_x);
	float ratio = 0;

	if(ratio_y < ratio_x){
		ratio = ratio_y;
	}
	else{
		ratio = ratio_x;
	}

	// updates max and min values
	max_x *= ratio;
	min_x *= ratio;
	max_y *= ratio;
	min_y *= ratio;

	// moves the image to the center of the frame
	float translate_x = (width / 2) - ((max_x+min_x) / 2);
	float translate_y = (height / 2) - ((max_y+min_y) / 2);

	// updates max and min values
	max_x += translate_x;
	min_x += translate_x;
	max_y += translate_y;
	min_y += translate_y;

	// updating x and y values
	for (size_t i = 0; i < posBuf.size(); i++) {
		if (i % 3 == 0) {
			posBuf[i] *= ratio;
			posBuf[i] += translate_x;
		}
		else if (i % 3 == 1){
			posBuf[i] *= ratio;
			posBuf[i] += translate_y;
		}
	}

	// sets up the bounding box for each triangle (holds x, y, z for every vertex)
	// sets up the normal box for each triangle (holds the bounding box and normal colors for each vertex)
	std::vector<BoundingBox*> boxes;
	std::vector<NormalBox*> nboxes;
	for (size_t f = 0; f < posBuf.size(); f += 9) {
		auto box = new BoundingBox(posBuf[f], posBuf[f + 1], posBuf[f + 2], posBuf[f + 3], posBuf[f + 4], posBuf[f + 5], posBuf[f + 6], posBuf[f + 7], posBuf[f + 8]);
		auto nbox = new NormalBox(box, norBuf[f], norBuf[f + 1], norBuf[f + 2], norBuf[f + 3], norBuf[f + 4], norBuf[f + 5], norBuf[f + 6], norBuf[f + 7], norBuf[f + 8]);
		boxes.push_back(box);
		nboxes.push_back(nbox);
	}

	// sets up the zBuffer
	vector<vector<float>> zBuf(height);
	for (size_t i = 0; i < height; i++) {
		zBuf[i] = vector<float>(width);
		for (size_t j = 0; j < width; j++) {
			zBuf[i][j] = -10000;
		}
	}

	
	// DRAWING BOUNDING BOXES (TASK 1)
	if(tasknum == 1){
		// for every box
		for(size_t f = 0; f < boxes.size(); f++){
			unsigned char r = RANDOM_COLORS[f % 7][0] * 255;
			unsigned char g = RANDOM_COLORS[f % 7][1] * 255;
			unsigned char b = RANDOM_COLORS[f % 7][2] * 255;
			// for every pixel in the box
			for(int y = boxes.at(f)->getMinY(); y < boxes.at(f)->getMaxY(); ++y) {
				for(int x = boxes.at(f)->getMinX(); x < boxes.at(f)->getMaxX(); ++x) {
					// draws box with random color
					image->setPixel(x, y, r, g, b);
				}
			}
		}
	}


	// DRAWING TRIANGLES WITH BARYCENTRIC COORDINATES (TASK 2)
	if(tasknum == 2){
		// for every box
		for(size_t v = 0; v < boxes.size(); v++){
			unsigned char r = RANDOM_COLORS[v%7][0]*255;
			unsigned char g = RANDOM_COLORS[v%7][1]*255;
			unsigned char b = RANDOM_COLORS[v%7][2]*255;
			// for every pixel in the box
			for(int y = boxes.at(v)->getMinY(); y < boxes.at(v)->getMaxY(); ++y) {
				for(int x = boxes.at(v)->getMinX(); x < boxes.at(v)->getMaxX(); ++x) {
					// determines if pixel is in the triangle; if yes, draw it
					bool in_triangle = barycentric_coords(boxes.at(v), y, x);
					if(in_triangle){
						image->setPixel(x, y, r, g, b);
					}
				}
			}
		}
	}

	// INTERPOLATING PRE VERTEX COLORS (TASK 3)
	if (tasknum == 3) {
		// for every box
		for (size_t v = 0; v < boxes.size(); v++) {
			unsigned char r1 = RANDOM_COLORS[v % 7][0] * 255;
			unsigned char g1 = RANDOM_COLORS[v % 7][1] * 255;
			unsigned char b1 = RANDOM_COLORS[v % 7][2] * 255;

			unsigned char r2 = RANDOM_COLORS[(v + 1) % 7][0] * 255;
			unsigned char g2 = RANDOM_COLORS[(v + 1) % 7][1] * 255;
			unsigned char b2 = RANDOM_COLORS[(v + 1) % 7][2] * 255;

			unsigned char r3 = RANDOM_COLORS[(v + 2) % 7][0] * 255;
			unsigned char g3 = RANDOM_COLORS[(v + 2) % 7][1] * 255;
			unsigned char b3 = RANDOM_COLORS[(v + 2) % 7][2] * 255;
			// for every pixel in the box
			for (int y = boxes.at(v)->getMinY(); y < boxes.at(v)->getMaxY(); ++y) {
				for (int x = boxes.at(v)->getMinX(); x < boxes.at(v)->getMaxX(); ++x) {
					// determines if in the triangle; if yes, find interpolated color and draw
					barycentric_coords_coloring(image, boxes.at(v), y, x,r1, g1, b1, r2, g2, b2, r3, g3, b3);
				}
			}
		}
	}

	// VERTICAL COLOR (TASK 4)
	if (tasknum == 4) {
		// for every box
		for (size_t v = 0; v < boxes.size(); v++) {
			// for every pixel within the box
			for (int y = boxes.at(v)->getMinY(); y < boxes.at(v)->getMaxY(); ++y) {
				for (int x = boxes.at(v)->getMinX(); x < boxes.at(v)->getMaxX(); ++x) {
					// determines if in the triangle; if yes, find interpolated color 
					// across the entire object and draw
					bool in_triangle = barycentric_coords(boxes.at(v), y, x);
					if (in_triangle) {
						vertical_coloring(image, boxes.at(v), y, x, max_y, min_y);
					}
				}
			}
		}
	}

	// Z-BUFFERING (TASK 5)
	if (tasknum == 5) {
		// for every box
		for (size_t v = 0; v < boxes.size(); v++) {
			// for every pixel in the box
			for (int y = boxes.at(v)->getMinY(); y < boxes.at(v)->getMaxY(); ++y) {
				for (int x = boxes.at(v)->getMinX(); x < boxes.at(v)->getMaxX(); ++x) {
					// if in the triangle, finds the current z value through interpolation
					// if current z is greater than zBuffer z, update zBuffer and the image
					// (must map z to red pixel)
					bool in_triangle = barycentric_coords(boxes.at(v), y, x);
					if (in_triangle) {
						float curr_z = compute_z(boxes.at(v), y, x);
						if(curr_z > zBuf[y][x]){
							zBuf[y][x] = curr_z;
							float red = 255 * ((curr_z - min_z) / (max_z - min_z));
							image->setPixel(x, y, red, 0, 0);
						}
					}
				}
			}
		}
	}

	// NORMAL COLORING (TASK 6)
	if (tasknum == 6) {
		// for every nbox
		for (size_t v = 0; v < nboxes.size(); v++) {

			float r1 = nboxes.at(v)->getR0();
			float g1 = nboxes.at(v)->getG0();
			float b1 = nboxes.at(v)->getB0();

			float r2 = nboxes.at(v)->getR1();
			float g2 = nboxes.at(v)->getG1();
			float b2 = nboxes.at(v)->getB1();

			float r3 = nboxes.at(v)->getR2();
			float g3 = nboxes.at(v)->getG2();
			float b3 = nboxes.at(v)->getB2();

			// for every box / set of colors in the nbox
			for (int y = nboxes.at(v)->getB()->getMinY(); y < nboxes.at(v)->getB()->getMaxY(); ++y) {
				for (int x = nboxes.at(v)->getB()->getMinX(); x < nboxes.at(v)->getB()->getMaxX(); ++x) {
					// if in the triangle, find and update zBuffer like with task 5
					// interpolate pixels using normal values stored for each vertex in nbox
					if (barycentric_coords(boxes.at(v), y, x)) {
						float curr_z = compute_z(boxes.at(v), y, x);
						if (curr_z > zBuf[y][x]) {
							zBuf[y][x] = curr_z;
							normal_barycentric_coloring(image, nboxes.at(v)->getB(), y, x, r1, g1, b1, r2, g2, b2, r3, g3, b3);
						}
					}
				}
			}
		}
	}

	// SIMPLE LIGHTING (TASK 7)
	if(tasknum == 7){
		// for every nbox
		for (size_t v = 0; v < nboxes.size(); v++) {

			float r1 = nboxes.at(v)->getR0();
			float g1 = nboxes.at(v)->getG0();
			float b1 = nboxes.at(v)->getB0();

			float r2 = nboxes.at(v)->getR1();
			float g2 = nboxes.at(v)->getG1();
			float b2 = nboxes.at(v)->getB1();

			float r3 = nboxes.at(v)->getR2();
			float g3 = nboxes.at(v)->getG2();
			float b3 = nboxes.at(v)->getB2();

			// for every box / set of colors / pixels in the nbox
			for (int y = nboxes.at(v)->getB()->getMinY(); y < nboxes.at(v)->getB()->getMaxY(); ++y) {
				for (int x = nboxes.at(v)->getB()->getMinX(); x < nboxes.at(v)->getB()->getMaxX(); ++x) {
					// if in the triangle, find and update zBuffer like task 5 and task 6
					// apply lighting techniques using normal coloring and lighting vector
					if (barycentric_coords(boxes.at(v), y, x)) {
						float curr_z = compute_z(boxes.at(v), y, x);
						if (curr_z > zBuf[y][x]) {
							zBuf[y][x] = curr_z;
							normal_coloring_with_lighting(image, nboxes.at(v)->getB(), y, x, r1, g1, b1, r2, g2, b2, r3, g3, b3);
						}
					}
				}
			}
		}
	}

	// actually draws image in output file
	image->writeToFile(filename);
	return 0;
}
